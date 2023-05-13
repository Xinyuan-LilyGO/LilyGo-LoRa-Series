/**
 * @file      T3S3Factory.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-05-13
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SD.h>
#include <esp_adc_cal.h>
#include <SSD1306Wire.h>
#include "OLEDDisplayUi.h"
#include <RadioLib.h>
#include "utilities.h"
#include <AceButton.h>

using namespace ace_button;

bool readkey();
void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void radioRx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void hwInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void setFlag(void);
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);


// ! Please select the corresponding RF module
#define USING_SX1262
// #define USING_SX1268
// #define USING_SX1276
// #define USING_SX1278
// #define USING_SX1280


#if     defined(USING_SX1262)
uint8_t txPower = 22;
float radioFreq = 868.0;
SX1262
#elif   defined(USING_SX1268)
uint8_t txPower = 22;
float radioFreq = 433.0;
SX1268
#elif   defined(USING_SX1276)
uint8_t txPower = 20;
float radioFreq = 868.0;
SX1276
#elif   defined(USING_SX1278)
uint8_t txPower = 20;
float radioFreq = 433.0;
SX1278
#elif   defined(USING_SX1280)
#undef RADIO_DIO1_PIN
#define RADIO_DIO1_PIN              9       //SX1280 DIO1 = IO9
uint8_t txPower = 3;
float radioFreq = 2400.0;
SX1280
#else

#error "No define radio type !"
#endif
radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);


// save transmission state between loops
int             transmissionState = RADIOLIB_ERR_NONE;
bool            transmittedFlag = false;
volatile bool   enableInterrupt = true;
uint32_t        transmissionCounter = 0;
uint32_t        recvCounter = 0;
float           radioRSSI   =   0;

bool            isRadioOnline = false;
bool            isSdCardOnline = false;
bool            rxStatus = true;
uint32_t        radioRunInterval = 0;
uint32_t        batteryRunInterval = 0;

SSD1306Wire     display(0x3c, I2C_SDA, I2C_SCL);
OLEDDisplayUi   ui( &display );
FrameCallback   frames[] = { hwInfo,  radioTx, radioRx};
SPIClass        SDSPI(HSPI);
AceButton       button;


void setFlag(void)
{
    // check if the interrupt is enabled
    if (!enableInterrupt) {
        return;
    }
    // we got a packet, set the flag
    transmittedFlag = true;
}


void handleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{
    static uint8_t framCounter;
    switch (eventType) {
    case AceButton::kEventClicked:
        switch (framCounter++) {
        case 0:
            enableInterrupt = false;
            break;
        case 1:
            enableInterrupt = true;
            radio.transmit((uint8_t *)&transmissionCounter, 4);
            break;
        case 2:
            enableInterrupt = true;
            radio.startReceive();
            break;
        default:
            break;
        }
        ui.nextFrame();
        framCounter %= 3;
        break;
    case AceButton::kEventLongPressed:
        break;
    }
}

void setup()
{

    Serial.begin(115200);
    Serial.println("initBoard");

    pinMode(BOARD_LED, OUTPUT);
    digitalWrite(BOARD_LED, LED_ON);

    Wire.begin(I2C_SDA, I2C_SCL);




    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    isSdCardOnline = SD.begin(SDCARD_CS, SDSPI);
    if (!isSdCardOnline) {
        Serial.println("setupSDCard FAIL");
    } else {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.print("setupSDCard PASS . SIZE = ");
        Serial.print(cardSize);
        Serial.println(" MB");
    }

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    button.init(BUTTON_PIN);
    ButtonConfig *buttonConfig = button.getButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);

    // Initialising the UI will init the display too.
    ui.setTargetFPS(60);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    ui.setIndicatorPosition(BOTTOM);

    // Defines where the first frame is located in the bar.
    ui.setIndicatorDirection(LEFT_RIGHT);

    // You can change the transition that is used
    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
    ui.setFrameAnimation(SLIDE_LEFT);

    // Add frames
    ui.setFrames(frames, sizeof(frames) / sizeof(frames[0]));

    ui.disableAutoTransition();

    // Initialising the UI will init the display too.
    ui.init();

    display.flipScreenVertically();


    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
    Serial.print(F("[Radio] Initializing ... "));
    int  state = radio.begin(radioFreq);
    if ( state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.println(F("failed!"));
    }
    isRadioOnline = state == RADIOLIB_ERR_NONE;


    // set output power to 10 dBm (accepted range is -3 - 17 dBm)
    // NOTE: 20 dBm value allows high power operation, but transmission
    //       duty cycle MUST NOT exceed 1%
    if (radio.setOutputPower(txPower) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
    }

#ifdef USING_SX1280
    //Set ANT Control pins
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif


#ifndef USING_SX1280

    // set bandwidth to 250 kHz
    if (radio.setBandwidth(250.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
    }

    // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    if (radio.setCurrentLimit(120) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        Serial.println(F("Selected current limit is invalid for this module!"));
    }
#endif

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);

    // start listening for LoRa packets
    Serial.print(F("[Radio] Starting to listen ... "));
    state = radio.startReceive();

}

void loop()
{
    button.check();
    ui.update();
    delay(2);
}




void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    if (millis() - radioRunInterval > 1000) {
        radio.transmit((uint8_t *)&transmissionCounter, 4);
        transmissionCounter++;
        radioRunInterval = millis();
        digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));
    }

    display->drawString(0 + x, 0 + y, "Radio Tx");
    display->drawString(0 + x, 12 + y, "TX :" + String(transmissionCounter));
}


void radioRx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(ArialMT_Plain_10);
    // The coordinates define the left starting point of the text
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    // check if the flag is set
    if (transmittedFlag) {

        digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));

        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;

        // reset flag
        transmittedFlag = false;

        // you can read received data as an Arduino String
        int state = radio.readData((uint8_t *)&recvCounter, 4);

        // you can also read received data as byte array
        /*
          byte byteArr[8];
          int state = radio.readData(byteArr, 8);
        */

        if (state == RADIOLIB_ERR_NONE) {
            // packet was successfully received
            Serial.println(F("[Radio] Received packet!"));
            radioRSSI = radio.getRSSI();

        } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
            // packet was received, but is malformed
            Serial.println(F("[Radio] CRC error!"));

        } else {
            // some other error occurred
            Serial.print(F("[Radio] Failed, code "));
            Serial.println(state);
        }

        // put module back to listen mode
        radio.startReceive();

        // we're ready to receive more packets,
        // enable interrupt service routine
        enableInterrupt = true;
    }

    display->drawString(0 + x, 0 + y, "Radio Rx");
    display->drawString(0 + x, 22 + y, "RX :" + String(recvCounter));
    display->drawString(0 + x, 10 + y, "RSSI:" + String(radioRSSI));


}

void hwInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static char buffer[64];
    if (millis() - batteryRunInterval > 1000) {
        esp_adc_cal_characteristics_t adc_chars;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
        uint16_t raw = analogRead(BAT_ADC_PIN);
        float volotage = (float)(esp_adc_cal_raw_to_voltage(raw, &adc_chars) * 2) / 1000.0;
        sprintf(buffer, "%.2fV", volotage);
        batteryRunInterval = millis();
    }

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0 + x, 10 + y, "Radio  ");
    display->drawString(50 + x, 10 + y, isRadioOnline & 1 ? "+" : "NA");
    display->drawString(0 + x, 20 + y, "SD   ");
    display->drawString(50 + x, 20 + y, isSdCardOnline & 1 ? "+" : "NA");
    display->drawString(0 + x, 30 + y, "BAT   ");
    display->drawString(50 + x, 30 + y, buffer);
}




