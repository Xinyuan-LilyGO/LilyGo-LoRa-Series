/*
*   Test all functions of the hardware, only applicable to T-Beam, does not support other LoRa versions
*   Written on November 07, 2020 Created by Lewis he
* */

#include <WiFi.h>
#include <AceButton.h>
#include <SSD1306.h>
#include <OLEDDisplayUi.h>
#include <RadioLib.h>
#include <TinyGPS++.h>
#include "boards.h"

using namespace ace_button;


SSD1306         *oled = nullptr;
OLEDDisplayUi   *ui = nullptr;
AceButton       btn(BUTTON_PIN);
TinyGPSPlus     gps;
String          recv = "";

// flag to indicate that a packet was received
bool            receivedFlag = false;
// disable interrupt when it's not needed
bool            enableInterrupt = true;

char            buff[5][256];
uint32_t        gpsLoopMillis = 0;
uint32_t        loraLoopMillis = 0;
uint32_t        positioningMillis = 0;
uint8_t         funcSelectIndex = 0;

#ifdef RADIO_USING_SX1262
RADIO_TYPE      radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#else
RADIO_TYPE      radio = new Module(RADIO_CS_PIN, RADIO_DI0_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif

void ButtonHandleEvent(AceButton *, uint8_t eventType, uint8_t buttonState);
void MsOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);
void DrawFrame1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void DrawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void DrawFrame3(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
#ifndef LILYGO_TBeam_V0_7
void DrawFrame4(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
#endif
void SenderLoop(void);
void GpsLoop(void);
void ReceiveLoop(void);

typedef void (*funcCallBackTypedef)(void);
funcCallBackTypedef LilyGoCallBack[] = {GpsLoop, SenderLoop, ReceiveLoop, NULL};
FrameCallback frames[] = {DrawFrame1, DrawFrame2, DrawFrame3,
#ifndef LILYGO_TBeam_V0_7
                          DrawFrame4
#endif
                         };
OverlayCallback overlays[] = { MsOverlay };

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


void setup()
{
    initBoard();

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    ButtonConfig *buttonConfig = btn.getButtonConfig();
    buttonConfig->setEventHandler(ButtonHandleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);

    Wire.beginTransmission(0x3C);
    if (!Wire.endTransmission()) {
        Serial.println("Started OLED");
        oled = new SSD1306(0x3C, I2C_SDA, I2C_SCL);
        ui = new OLEDDisplayUi(oled);
        oled->init();
        oled->flipScreenVertically();
        oled->setFont(ArialMT_Plain_16);
        oled->setTextAlignment(TEXT_ALIGN_CENTER);
        ui->setTargetFPS(30);
        ui->disableAutoTransition();
        ui->setIndicatorPosition(BOTTOM);
        ui->setIndicatorDirection(LEFT_RIGHT);
        ui->setFrameAnimation(SLIDE_LEFT);
        ui->setFrames(frames, sizeof(frames) / sizeof(*frames));
        ui->setOverlays(overlays, sizeof(overlays) / sizeof(*overlays));
    }
    Serial.print(F("[Radio] Initializing ... "));
#ifndef LoRa_frequency
    int state = radio.begin(868.0);
#else
    int state = radio.begin(LoRa_frequency);
#endif

    if (state == ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    // set the function that will be called
    // when new packet is received
#if defined(RADIO_USING_SX1262)
    radio.setDio1Action(setFlag);
#elif defined(RADIO_USING_SX1268)
    radio.setDio1Action(setFlag);
#else
    radio.setDio0Action(setFlag);
#endif
}

void loop()
{
    btn.check();
    if (LilyGoCallBack[funcSelectIndex]) {
        LilyGoCallBack[funcSelectIndex]();
    }
    if (ui) {
        ui->update();
    }
}

void GpsLoop(void)
{
    while (Serial1.available()) {
        int r = Serial1.read();
        Serial.write(r);
        gps.encode(r);
    }

    if (millis() > 5000 && gps.charsProcessed() < 10) {
        snprintf(buff[0], sizeof(buff[0]), "T-Beam GPS");
        snprintf(buff[1], sizeof(buff[1]), "No GPS detected");
        Serial.println("No GPS detected");
        return;
    }
    if (!gps.location.isValid()) {
        if (millis() - gpsLoopMillis > 1000) {
            snprintf(buff[0], sizeof(buff[0]), "T-Beam GPS");
            snprintf(buff[1], sizeof(buff[1]), "Positioning(%u)S", positioningMillis++);
            gpsLoopMillis = millis();
        }
    } else {
        if (millis() - gpsLoopMillis > 1000) {
            snprintf(buff[0], sizeof(buff[0]), "UTC:%d:%d:%d", gps.time.hour(), gps.time.minute(), gps.time.second());
            snprintf(buff[1], sizeof(buff[1]), "LNG:%.4f", gps.location.lng());
            snprintf(buff[2], sizeof(buff[2]), "LAT:%.4f", gps.location.lat());
            snprintf(buff[3], sizeof(buff[3]), "satellites:%u", gps.satellites.value());

            Serial.printf("UTC:%d:%d:%d-LNG:%.4f-LAT:%.4f-satellites:%u\n",
                          gps.time.hour(),
                          gps.time.minute(),
                          gps.time.second(),
                          gps.location.lng(),
                          gps.location.lat(),
                          gps.satellites.value());
            gpsLoopMillis = millis();
        }
    }
}

void SenderLoop(void)
{
    snprintf(buff[0], sizeof(buff[0]), "T-Beam Lora Sender");
    // Send data every 3 seconds
    if (millis() - loraLoopMillis > 3000) {
        int transmissionState = ERR_NONE;
        transmissionState = radio.startTransmit(String(loraLoopMillis).c_str());
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

            transmissionState = radio.startTransmit(String(loraLoopMillis).c_str());

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
        snprintf(buff[1], sizeof(buff[1]), "Send: %u", loraLoopMillis);

        loraLoopMillis = millis();
        Serial.println(buff[1]);
    }
}

void ReceiveLoop(void)
{
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
            snprintf(buff[1], sizeof(buff[1]), "RSSI:%.2f dBm", radio.getRSSI());

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
}


/************************************
 *      BUTTON
 * *********************************/
void ButtonHandleEvent(AceButton *, uint8_t eventType, uint8_t buttonState)
{
    switch (eventType) {
    case AceButton::kEventClicked:
        if (ui) {
            ui->nextFrame();
        }
        funcSelectIndex++;
        funcSelectIndex %= sizeof(LilyGoCallBack) / sizeof(*LilyGoCallBack);
        if (funcSelectIndex == 2) {
            Serial.print(F("[RADIO] Starting to listen ... "));
            int state = radio.startReceive();
            if (state == ERR_NONE) {
                Serial.println(F("success!"));
            } else {
                Serial.print(F("failed, code "));
                Serial.println(state);
            }
            memset(buff, 0, sizeof(buff));
        }
        break;
    case AceButton::kEventLongPressed:

        //Power off all peripherals
        disablePeripherals();

        //If the power is not turned off, the peripheral should be set to sleep
        // radio.sleep();

        if (oled) {
            oled->displayOff();
        }
        // Wait for your finger to release
        while (!digitalRead(BUTTON_PIN)) {
            delay(100);
        }
        esp_sleep_enable_ext1_wakeup(BUTTON_PIN_MASK, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
        break;
    }
}

/************************************
 *      DISPLAY
 * *********************************/
void MsOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
    static char volbuffer[128];
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);

    display->drawString(0, 0, "LilyGo");
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
    snprintf(volbuffer, sizeof(volbuffer), "%u/%uKB",  info.total_allocated_bytes / 1024, info.total_free_bytes / 1024);
    display->drawString(75, 0, volbuffer);
}

void DrawFrame1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
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

void DrawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 11 + y, buff[0]);
    display->drawString(64 + x, 22 + y, buff[1]);
}

void DrawFrame3(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 9 + y, buff[0]);
    display->drawString(64 + x, 22 + y, recv == "" ? "No message" : recv);
    display->drawString(64 + x, 35 + y, buff[1]);
}

#ifndef LILYGO_TBeam_V0_7
void DrawFrame4(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    bool batteryConnect = PMU.isBatteryConnect();
    snprintf(buff[0], sizeof(buff[0]), "BATTERY:%s", batteryConnect ? "CONNECT" : "DISCONNECT");
    if (batteryConnect) {
        snprintf(buff[1], sizeof(buff[1]), "VOLTAGE:%.2f", PMU.getBattVoltage());
        snprintf(buff[2], sizeof(buff[2]), "CURRENT:%.2f", PMU.getBattDischargeCurrent());
    } else {
        snprintf(buff[1], sizeof(buff[1]), "VOLTAGE:%.2f", PMU.getVbusVoltage());
        snprintf(buff[2], sizeof(buff[2]), "CURRENT:%.2f", PMU.getVbusCurrent());
    }
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 12 + y, buff[0]);
    display->drawString(64 + x, 24 + y, buff[1]);
    display->drawString(64 + x, 37 + y, buff[2]);
}
#endif
