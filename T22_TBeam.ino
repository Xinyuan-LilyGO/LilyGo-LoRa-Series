#include "board_def.h"
#include <WiFi.h>
#include <Wire.h>
#include "axp20x.h"
#include <Button2.h>

#define AXP192_SLAVE_ADDRESS    0x34
SSD1306_OBJECT();
UBLOX_GPS_OBJECT();

AXP20X_Class axp;

bool ssd1306_found = false;
bool axp192_found = false;
bool loraBeginOK = false;

uint64_t dispMap = 0;
String dispInfo;
char buff[512];
uint64_t gpsSec = 0;
bool pmu_irq = false;
#define BUTTONS_MAP {38}

Button2 *pBtns = nullptr;
uint8_t g_btns[] =  BUTTONS_MAP;
#define ARRARY_SIZE(a)   (sizeof(a) / sizeof(a[0]))


/************************************
 *      BUTTON
 * *********************************/
void button_callback(Button2 &b)
{
    for (int i = 0; i < ARRARY_SIZE(g_btns); ++i) {
        if (pBtns[i] == b) {
            ui.nextFrame();
        }
    }
}

void button_loop()
{
    for (int i = 0; i < ARRARY_SIZE(g_btns); ++i) {
        pBtns[i].loop();
    }
}


void button_init()
{
    uint8_t args = ARRARY_SIZE(g_btns);
    pBtns = new Button2 [args];
    for (int i = 0; i < args; ++i) {
        pBtns[i] = Button2(g_btns[i]);
        pBtns[i].setPressedHandler(button_callback);
    }
}

/************************************
 *      SCREEN
 * *********************************/
String baChStatus = "No charging";
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
    static char volbuffer[128];
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);

    display->drawString(0, 0, baChStatus);

    if (axp.isBatteryConnect()) {
        snprintf(volbuffer, sizeof(volbuffer), "%.2fV/%.2fmA", axp.getBattVoltage() / 1000.0, axp.isChargeing() ? axp.getBattChargeCurrent() : axp.getBattDischargeCurrent());
        display->drawString(62, 0, volbuffer);
    }
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static uint64_t gpsMap = 0;

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);

#ifdef ENABLE_GPS
    while (Serial1.available())
        gps.encode(Serial1.read());

    if (millis() > 5000 && gps.charsProcessed() < 10) {
        display->drawString(64 + x, 11 + y, "T-Beam GPS");
        display->drawString(64 + x, 22 + y, "No GPS detected");
        return;
    }

    if (!gps.location.isValid()) {
        if (millis() - gpsMap > 1000) {
            snprintf(buff, sizeof(buff), "Positioning(%llu)", gpsSec++);
            gpsMap = millis();
        }
        display->drawString(64 + x, 11 + y, "T-Beam GPS");
        display->drawString(64 + x, 22 + y, buff);
    } else {
        snprintf(buff, sizeof(buff), "UTC:%d:%d:%d", gps.time.hour(), gps.time.minute(), gps.time.second());
        display->drawString(64 + x, 11 + y, buff);
        snprintf(buff, sizeof(buff), "LNG:%.4f", gps.location.lng());
        display->drawString(64 + x, 22 + y, buff);
        snprintf(buff, sizeof(buff), "LAT:%.4f", gps.location.lat());
        display->drawString(64 + x, 33 + y, buff);
        snprintf(buff, sizeof(buff), "satellites:%lu", gps.satellites.value());
        display->drawString(64 + x, 44 + y, buff);
    }
#endif
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static uint32_t sendCount = 0;
    static uint64_t loraMap = 0;

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 11 + y, "T-Beam Lora Sender");

    if (!loraBeginOK) {
        display->drawString(64 + x, 22 + y, "Lora Begin FAIL");
        return;
    }

    if (millis() - loraMap > 3000) {
        LoRa.beginPacket();
        LoRa.print("lora: ");
        LoRa.print(sendCount);
        LoRa.endPacket();
        ++sendCount;
        Serial.printf("Send %lu\n", sendCount);
        loraMap = millis();
    }
    display->drawString(64 + x, 22 + y, "Send " + String(sendCount));
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static String recv = "";

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);

    if (!loraBeginOK) {
        display->drawString(64 + x, 22 + y, "Lora Begin FAIL");
        return;
    }

#ifdef ENABLE_LOAR
    if (LoRa.parsePacket()) {
        recv = "";
        while (LoRa.available()) {
            recv += (char)LoRa.read();
        }
        Serial.println(recv);
    }

    display->drawString(64 + x, 9 + y, "T-Beam Lora Received");
    display->drawString(64 + x, 22 + y, recv == "" ? "No message" : recv);
    display->drawString(64 + x, 35 + y, "rssi :" + String(LoRa.packetRssi()));
#endif
}

//PMU
void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static uint8_t n = 0;
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    if (!axp192_found) {
        display->drawString(64 + x, 22 + y, "PMU Begin FAIL");
        return;
    }
    //TODO::
    display->drawString(64 + x, 22 + y, "Empty");
}


FrameCallback frames[] = {drawFrame1, drawFrame2, drawFrame3, drawFrame4};
OverlayCallback overlays[] = { msOverlay };

void ssd1306_init()
{
#ifdef ENABLE_SSD1306
    if (!ssd1306_found) {
        Serial.println("SSD1306 not found");
        return;
    }
    if (oled.init()) {
        oled.flipScreenVertically();
        oled.setFont(ArialMT_Plain_16);
        oled.setTextAlignment(TEXT_ALIGN_CENTER);
    } else {
        Serial.println("SSD1306 Begin FAIL");
    }
    Serial.println("SSD1306 Begin PASS");
    ui.setTargetFPS(30);
    ui.disableAutoTransition();
    ui.setIndicatorPosition(BOTTOM);
    ui.setIndicatorDirection(LEFT_RIGHT);
    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setFrames(frames, ARRARY_SIZE(frames));
    if (axp192_found) {
        ui.setOverlays(overlays, ARRARY_SIZE(overlays));
    }
#endif
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);

    scanI2Cdevice();
    pinMode(2, OUTPUT);
    digitalWrite(2, 0);

    if (axp192_found) {
        if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
            Serial.println("AXP192 Begin PASS");
        } else {
            Serial.println("AXP192 Begin FAIL");
        }

        axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
        axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);

        pinMode(PMU_IRQ, INPUT_PULLUP);
        attachInterrupt(PMU_IRQ, [] {
            pmu_irq = true;
        }, FALLING);

        axp.adc1Enable(AXP202_BATT_CUR_ADC1, 1);

        axp.enableIRQ(AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_BATT_REMOVED_IRQ | AXP202_BATT_CONNECT_IRQ, 1);
        axp.clearIRQ();

        if (axp.isChargeing()) {
            baChStatus = "Charging";
        }
    } else {
        Serial.println("AXP192 not found");
    }
    button_init();

    ssd1306_init();

#ifdef ENABLE_GPS
    Serial1.begin(GPS_BANUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#endif

#ifdef ENABLE_LOAR
    lora_init();
#endif
}


void lora_init()
{
#ifdef ENABLE_LOAR
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
    if (!LoRa.begin(BAND))
        Serial.println("LORA Begin FAIL");
    else {
        loraBeginOK = true;
        Serial.println("LORA Begin PASS");
    }
#endif
}


void loop()
{
    if (pmu_irq) {
        pmu_irq = false;
        axp.readIRQ();
        if (axp.isChargingIRQ()) {
            baChStatus = "Charging";
        } else {
            baChStatus = "No Charging";
        }
        if (axp.isVbusRemoveIRQ()) {
            baChStatus = "No Charging";
        }
        digitalWrite(2, !digitalRead(2));
        axp.clearIRQ();
    }

#ifdef ENABLE_SSD1306
    if (ui.update()) {
#endif
        button_loop();
#ifdef ENABLE_SSD1306
    }
#endif
}

void scanI2Cdevice(void)
{
    byte err, addr;
    int nDevices = 0;
    for (addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        err = Wire.endTransmission();
        if (err == 0) {
            Serial.print("I2C device found at address 0x");
            if (addr < 16)
                Serial.print("0");
            Serial.print(addr, HEX);
            Serial.println(" !");
            nDevices++;

            if (addr == SSD1306_ADDRESS) {
                ssd1306_found = true;
                Serial.println("ssd1306 display found");
            }
            if (addr == AXP192_SLAVE_ADDRESS) {
                axp192_found = true;
                Serial.println("axp192 PMU found");
            }
        } else if (err == 4) {
            Serial.print("Unknow error at address 0x");
            if (addr < 16)
                Serial.print("0");
            Serial.println(addr, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
}