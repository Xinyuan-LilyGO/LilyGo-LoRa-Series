#include "board_def.h"
#include <WiFi.h>
#include <Wire.h>
#include "axp20x.h"
#include <Button2.h>
#include <Ticker.h>

#define AXP192_SLAVE_ADDRESS    0x34
SSD1306_OBJECT();
UBLOX_GPS_OBJECT();

AXP20X_Class axp;
static String recv = "";
uint8_t program = 0;
bool ssd1306_found = false;
bool axp192_found = false;
bool loraBeginOK = false;

uint64_t dispMap = 0;
String dispInfo;
char buff[5][256];

uint64_t gpsSec = 0;
bool pmu_irq = false;
#define BUTTONS_MAP {38}

Button2 *pBtns = nullptr;
uint8_t g_btns[] =  BUTTONS_MAP;
#define ARRARY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

Ticker btnTick;

/************************************
 *      BUTTON
 * *********************************/
void button_callback(Button2 &b)
{
    for (int i = 0; i < ARRARY_SIZE(g_btns); ++i) {
        if (pBtns[i] == b) {
            if (ssd1306_found) {
                ui.nextFrame();
            }
            program = program + 1 > 2 ? 0 : program + 1;
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
    pBtns[0].setLongClickHandler([](Button2 & b) {
        if (ssd1306_found) {
            oled.displayOff();
        }
        Serial.println("Go to Sleep");
        axp.setChgLEDMode(AXP20X_LED_OFF);
        axp.setPowerOutPut(AXP192_LDO2, AXP202_OFF);
        axp.setPowerOutPut(AXP192_LDO3, AXP202_OFF);
        axp.setPowerOutPut(AXP192_DCDC2, AXP202_OFF);
        // axp.setPowerOutPut(AXP192_DCDC3, AXP202_OFF);
        axp.setPowerOutPut(AXP192_DCDC1, AXP202_OFF);
        axp.setPowerOutPut(AXP192_EXTEN, AXP202_OFF);

        delay(20);
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_38, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
    });
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

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);


    if (!gps.location.isValid()) {
        display->drawString(64 + x, 11 + y, buff[0]);
        display->drawString(64 + x, 22 + y, buff[1]);
    } else {
        display->drawString(64 + x, 11 + y, buff[0]);
        display->drawString(64 + x, 22 + y, buff[1]);
        display->drawString(64 + x, 33 + y, buff[2]);
        display->drawString(64 + x, 44 + y, buff[3]);
    }
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 11 + y, buff[0]);
    display->drawString(64 + x, 22 + y, buff[1]);
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 9 + y, buff[0]);
    display->drawString(64 + x, 22 + y, recv == "" ? "No message" : recv);
    display->drawString(64 + x, 35 + y, buff[1]);
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


FrameCallback frames[] = {drawFrame1, drawFrame2, drawFrame3, /*drawFrame4*/};
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


void playSound()
{
#ifdef ENABLE_BUZZER
    ledcWriteTone(0, 1000);
    delay(200);
    ledcWriteTone(0, 0);
#endif
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);

    scanI2Cdevice();

#ifdef ENABLE_BUZZER
    ledcSetup(0, 1000, 8);
    ledcAttachPin(BUZZER_PIN, 0);
#endif

    playSound();
    playSound();

    axp192_found = 1;
    if (axp192_found) {
        if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
            Serial.println("AXP192 Begin PASS");
        } else {
            Serial.println("AXP192 Begin FAIL");
        }

        // axp.setChgLEDMode(LED_BLINK_4HZ);

        Serial.printf("DCDC1: %s\n", axp.isDCDC1Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("DCDC2: %s\n", axp.isDCDC2Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("LDO2: %s\n", axp.isLDO2Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("LDO3: %s\n", axp.isLDO3Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("DCDC3: %s\n", axp.isDCDC3Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("Exten: %s\n", axp.isExtenEnable() ? "ENABLE" : "DISABLE");

        Serial.println("----------------------------------------");

        axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
        axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
        axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
        axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
        axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
        axp.setDCDC1Voltage(3300);

        Serial.printf("DCDC1: %s\n", axp.isDCDC1Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("DCDC2: %s\n", axp.isDCDC2Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("LDO2: %s\n", axp.isLDO2Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("LDO3: %s\n", axp.isLDO3Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("DCDC3: %s\n", axp.isDCDC3Enable() ? "ENABLE" : "DISABLE");
        Serial.printf("Exten: %s\n", axp.isExtenEnable() ? "ENABLE" : "DISABLE");


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

    btnTick.attach_ms(20, button_loop);
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
    static uint32_t sendCount = 0;
    static uint64_t loraMap = 0;
    static uint64_t gpsMap = 0;

    if (axp192_found && pmu_irq) {
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
    switch (program) {
    case 0:
        while (Serial1.available())
            gps.encode(Serial1.read());

        if (millis() > 5000 && gps.charsProcessed() < 10) {
            snprintf(buff[0], sizeof(buff[0]), "T-Beam GPS");
            snprintf(buff[1], sizeof(buff[1]), "No GPS detected");
            if (!ssd1306_found) {
                Serial.println(buff[1]);
            }
            return;
        }
        if (!gps.location.isValid()) {
            if (millis() - gpsMap > 1000) {
                snprintf(buff[1], sizeof(buff[1]), "Positioning(%llu)", gpsSec++);
                if (!ssd1306_found) {
                    Serial.println(buff[1]);
                }
                gpsMap = millis();
            }
        } else {
            if (millis() - gpsMap > 1000) {
                playSound();
                snprintf(buff[0], sizeof(buff[0]), "UTC:%d:%d:%d", gps.time.hour(), gps.time.minute(), gps.time.second());
                snprintf(buff[1], sizeof(buff[1]), "LNG:%.4f", gps.location.lng());
                snprintf(buff[2], sizeof(buff[2]), "LAT:%.4f", gps.location.lat());
                snprintf(buff[3], sizeof(buff[3]), "satellites:%lu", gps.satellites.value());
                if (!ssd1306_found) {
                    Serial.println(buff[0]);
                    Serial.println(buff[1]);
                    Serial.println(buff[2]);
                    Serial.println(buff[3]);
                }
                gpsMap = millis();
            }
        }
        break;
    case 1:
        snprintf(buff[0], sizeof(buff[0]), "T-Beam Lora Sender");
        if (!loraBeginOK) {
            snprintf(buff[1], sizeof(buff[1]), "Lora Begin FAIL");
            if (!ssd1306_found) {
                Serial.println(buff[1]);
            }
            return;
        }

        if (millis() - loraMap > 3000) {
            LoRa.beginPacket();
            LoRa.print("lora: ");
            LoRa.print(sendCount);
            LoRa.endPacket();
            ++sendCount;
            snprintf(buff[1], sizeof(buff[1]), "Send %lu", sendCount);
            loraMap = millis();
            if (!ssd1306_found) {
                Serial.println(buff[1]);
            }
        }
        break;
    case 2:
        if (!loraBeginOK) {
            recv =  "Lora Begin FAIL";
            if (!ssd1306_found) {
                Serial.println(recv);
            }
            return;
        }
        snprintf(buff[0], sizeof(buff[0]), "T-Beam Lora Received");
        if (LoRa.parsePacket()) {
            recv = "";
            while (LoRa.available()) {
                recv += (char)LoRa.read();
            }
            if (!ssd1306_found) {
                Serial.printf("Lora Received:%s - rssi:%d\n", recv.c_str(), LoRa.packetRssi());
            }
        } else {
            // if (!ssd1306_found) {
            //     Serial.println("Wait for received message");
            //     delay(500);
            // }
        }
        snprintf(buff[1], sizeof(buff[1]), "rssi:%d", LoRa.packetRssi());
        break;
    }
    /*
    if (ssd1306_found) {
        if (ui.update()) {
            button_loop();
        }
    } else {
        button_loop();
    }
     */
    if (ssd1306_found) {
        if (ui.update()) {
        }
    }
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

