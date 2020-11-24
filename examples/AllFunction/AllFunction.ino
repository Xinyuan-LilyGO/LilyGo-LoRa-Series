/*
*   This factory is just to test LilyGo T-Beam series hardware
*   Created by Lewis he
* */

#include "utilities.h"
#include <WiFi.h>
#include <Wire.h>
#include "axp20x.h"
#include <Button2.h>

#ifndef AXP192_SLAVE_ADDRESS
#define AXP192_SLAVE_ADDRESS    0x34
#endif

SSD1306_OBJECT();
UBLOX_GPS_OBJECT();

AXP20X_Class axp;
uint8_t program = 0;
bool ssd1306_found = false;
bool axp192_found = false;
bool loraBeginOK = false;

char buff[5][256];

uint64_t gpsSec = 0;
bool pmu_irq = false;
#define BUTTONS_MAP {BUTTON_PIN}

Button2 *pBtns = nullptr;
uint8_t g_btns[] =  BUTTONS_MAP;
#define ARRARY_SIZE(a)   (sizeof(a) / sizeof(a[0]))


String baChStatus = "No charging";
String recv = "";

// flag to indicate that a packet was received
bool receivedFlag = false;

// disable interrupt when it's not needed
bool enableInterrupt = true;

RADIO_TYPE radio = new Module(LORA_SS, LORA_DIO1, LORA_RST, LORA_BUSY);


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
            if (program == 2) {
                Serial.print(F("[RADIO] Starting to listen ... "));
                int state = radio.startReceive();
                if (state == ERR_NONE) {
                    Serial.println(F("success!"));
                } else {
                    Serial.print(F("failed, code "));
                    Serial.println(state);
                    while (true);
                }
            }
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
        if (axp192_found) {
            axp.setChgLEDMode(AXP20X_LED_OFF);
            axp.setPowerOutPut(AXP192_LDO2, AXP202_OFF);
            axp.setPowerOutPut(AXP192_LDO3, AXP202_OFF);
            axp.setPowerOutPut(AXP192_DCDC2, AXP202_OFF);
            // axp.setPowerOutPut(AXP192_DCDC3, AXP202_OFF);
            axp.setPowerOutPut(AXP192_DCDC1, AXP202_OFF);
            axp.setPowerOutPut(AXP192_EXTEN, AXP202_OFF);
        }

        delay(20);
        esp_sleep_enable_ext1_wakeup(BUTTON_PIN_MASK, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
    });
}

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
    static char volbuffer[128];
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);

    display->drawString(0, 0, baChStatus);

    if (axp.isBatteryConnect()) {
        snprintf(volbuffer, sizeof(volbuffer), "%.2fV/%.2fmA", axp.getBattVoltage() / 1000.0, axp.isChargeing() ? axp.getBattChargeCurrent() : axp.getBattDischargeCurrent());
        display->drawString(62, 0, volbuffer);
    } else {
        multi_heap_info_t info;
        heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
        snprintf(volbuffer, sizeof(volbuffer), "%u/%uKB",  info.total_allocated_bytes / 1024, info.total_free_bytes / 1024);
        display->drawString(75, 0, volbuffer);
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


FrameCallback frames[] = {drawFrame1, drawFrame2, drawFrame3};
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

void playSound()
{
#ifdef ENABLE_BUZZER
    ledcWriteTone(0, 1000);
    delay(200);
    ledcWriteTone(0, 0);
#endif
}

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void)
{
    // check if the interrupt is enabled
    if (!enableInterrupt) {
        return;
    }
    // we got a packet, set the flag
    receivedFlag = true;
}

void lora_init()
{
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    Serial.print(F("[Radio] Initializing ... "));
    int state = radio.begin(BAND);
    if (state == ERR_NONE) {
        loraBeginOK = true;
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);

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
        axp.setDCDC1Voltage(3300);  //esp32 core VDD    3v3
        axp.setLDO2Voltage(3300);   //LORA VDD set 3v3
        axp.setLDO3Voltage(3300);   //GPS VDD      3v3

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

}



void loop()
{
    button_loop();

    static uint32_t loraMap = 0;
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
                snprintf(buff[0], sizeof(buff[0]), "T-Beam GPS");
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
                snprintf(buff[3], sizeof(buff[3]), "satellites:%u", gps.satellites.value());
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
            int transmissionState = ERR_NONE;
            transmissionState = radio.startTransmit(String(loraMap).c_str());
            // check if the previous transmission finished
            if (receivedFlag) {
                // disable the interrupt service routine while
                // processing the data
                enableInterrupt = false;
                // reset flag
                receivedFlag = false;
                if (transmissionState == ERR_NONE) {
                    // packet was successfully sent
                    Serial.println(F("transmission finished!"));
                    // NOTE: when using interrupt-driven transmit method,
                    //       it is not possible to automatically measure
                    //       transmission data rate using getDataRate()

                } else {
                    Serial.print(F("failed, code "));
                    Serial.println(transmissionState);
                }
                // wait a second before transmitting again
                // delay(1000);

                // send another one
                Serial.print(F("[RADIO] Sending another packet ... "));

                // you can transmit C-string or Arduino string up to
                // 256 characters long
                transmissionState = radio.startTransmit(String(loraMap).c_str());

                // you can also transmit byte array up to 256 bytes long
                /*
                  byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                                    0x89, 0xAB, 0xCD, 0xEF};
                  int state = radio.startTransmit(byteArr, 8);
                */

                // we're ready to send more packets,
                // enable interrupt service routine
                enableInterrupt = true;
            }
            snprintf(buff[1], sizeof(buff[1]), "Send %u", loraMap);
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

        // check if the flag is set
        if (receivedFlag) {
            // disable the interrupt service routine while
            // processing the data
            enableInterrupt = false;

            // reset flag
            receivedFlag = false;

            // you can read received data as an Arduino String
            int state = radio.readData(recv);

            // you can also read received data as byte array
            /*
              byte byteArr[8];
              int state = radio.readData(byteArr, 8);
            */

            if (state == ERR_NONE) {
                // packet was successfully received
                Serial.println(F("[RADIO] Received packet!"));

                // print data of the packet
                Serial.print(F("[RADIO] Data:\t\t"));
                Serial.println(recv);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[RADIO] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));
                snprintf(buff[1], sizeof(buff[1]), "rssi:%.2f dBm", radio.getRSSI());

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[RADIO] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

            } else if (state == ERR_CRC_MISMATCH) {
                // packet was received, but is malformed
                Serial.println(F("CRC error!"));

            } else {
                // some other error occurred
                Serial.print(F("failed, code "));
                Serial.println(state);

            }

            // put module back to listen mode
            radio.startReceive();

            // we're ready to receive more packets,
            // enable interrupt service routine
            enableInterrupt = true;
        }
        break;
    }
    if (ssd1306_found) {
        ui.update();
    }
}
