/**
 * @file      BPFFactory.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2024-08-07
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SH1106Wire.h>
#include "OLEDDisplayUi.h"
#include <RadioLib.h>
#include "utilities.h"
#include <AceButton.h>
#include "LoRaBoards.h"
#include <TinyGPS++.h>
#include <Roboto_Mono_Medium_12.h>

using namespace ace_button;

void radioTx(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);
void radioRx(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);
void gpsInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);
void hwInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);



// BPF Freq range : 144Mhz ~ 148MHz
#if     defined(USING_SX1276)

#define CONFIG_RADIO_FREQ           144.0
#define CONFIG_RADIO_OUTPUT_POWER   17
#define CONFIG_RADIO_BW             125.0
SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1278)

#define CONFIG_RADIO_FREQ           144.0
#define CONFIG_RADIO_OUTPUT_POWER   17
#define CONFIG_RADIO_BW             125.0
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#endif

#define PREV_BUTTON_PIN             3
#define NEXT_BUTTON_PIN             0

enum TransmissionDirection {
    TRANSMISSION,
    RECEIVE,
};
// save transmission state between loops
int             transmissionState = RADIOLIB_ERR_NONE;
bool            transmittedFlag = false;
uint32_t        transmissionCounter = 0;
uint32_t        recvCounter = 0;
float           radioRSSI   =   0;
bool            isRadioOnline = false;
uint32_t        radioRunInterval = 0;
bool            led_blink = false;
int             currentFrames = 0;
uint8_t         lastFrames = 0;
TransmissionDirection  transmissionDirection = TRANSMISSION;



SH1106Wire      display(0x3c, I2C_SDA, I2C_SCL);
OLEDDisplayUi   ui( &display );
FrameCallback   frames[] = { hwInfo,  radioTx, radioRx, gpsInfo};
const uint8_t   frames_count =  sizeof(frames) / sizeof(frames[0]);
AceButton       prevButton;
AceButton       nextButton;
TinyGPSPlus     gps;


void setFlag(void)
{
    // we got a packet, set the flag
    transmittedFlag = true;
}

void handleMenu()
{
    Serial.printf("currentFrames : %d\n", currentFrames);
    switch (currentFrames) {
    case 0:
        break;
    case 1:
        Serial.println("Start transmit");
        transmissionDirection = TRANSMISSION;
        transmissionState = radio.transmit((uint8_t *)&transmissionCounter, 4);
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.println(F("[Radio] transmit packet failed!"));
        }
        break;
    case 2:
        Serial.println("Start receive");
        transmissionDirection = RECEIVE;
        transmissionState = radio.startReceive();
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.println(F("[Radio] Received packet failed!"));
        }
        break;
    case 3:
        break;
    default:
        break;
    }
    ui.transitionToFrame(currentFrames);
}

void sleepDevice()
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 32, "Sleep");
    display.display();

    digitalWrite(RADIO_RST_PIN, HIGH);
    gpio_hold_en((gpio_num_t) RADIO_RST_PIN);
    gpio_deep_sleep_hold_en();

    radio.sleep();

    delay(2000);

    display.displayOff();

#ifdef BOARD_LED
    digitalWrite(BOARD_LED, LOW);
#endif

#ifdef  RADIO_TCXO_ENABLE
    digitalWrite(RADIO_TCXO_ENABLE, LOW);
#endif

    // Disable all power output
    disablePeripherals();

    while (digitalRead(0) == LOW)
        delay(1);

    SPI.end();

    Wire.end();

    Serial.flush();

    Serial.end();

    pinMode(RADIO_CS_PIN, INPUT);

    pinMode(RADIO_RST_PIN, INPUT);

#ifdef RADIO_DIO0_PIN
    pinMode(RADIO_DIO0_PIN, INPUT);
#endif

#ifdef RADIO_DIO1_PIN
    pinMode(RADIO_DIO1_PIN, INPUT);
#endif

#ifdef RADIO_DIO9_PIN
    pinMode(RADIO_DIO9_PIN, INPUT);
#endif

#ifdef RADIO_BUSY_PIN
    pinMode(RADIO_BUSY_PIN, INPUT);
#endif

#ifdef RADIO_LDO_EN
    pinMode(RADIO_LDO_EN, INPUT);
#endif

#ifdef RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, INPUT);
#endif

    pinMode(RADIO_CS_PIN, INPUT);
    pinMode(I2C_SDA, INPUT);
    pinMode(I2C_SCL, INPUT);
    pinMode(RADIO_SCLK_PIN, INPUT);
    pinMode(RADIO_MISO_PIN, INPUT);
    pinMode(RADIO_MOSI_PIN, INPUT);
    pinMode(SDCARD_MOSI, INPUT);
    pinMode(SDCARD_MISO, INPUT);
    pinMode(SDCARD_SCLK, INPUT);
    pinMode(SDCARD_CS, INPUT);

    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);

    // BPF ext1 sleep  ~ 434 uA
    esp_sleep_enable_ext1_wakeup(_BV(0), ESP_EXT1_WAKEUP_ALL_LOW);

    // BPF  Timer sleep ~ 434 uA
    // esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);

    esp_deep_sleep_start();

    Serial.println("Never print()");
}

void prevButtonHandleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{
    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.printf("prevButtonHandleEvent currentFrames:%d frames_count:%d\n", currentFrames, frames_count);
        currentFrames =  ((currentFrames + 1) >= frames_count) ? currentFrames : currentFrames + 1;
        handleMenu();
        break;
    case AceButton::kEventLongPressed:
        Serial.println("Long pressed!");
        sleepDevice();
        break;
    default:
        break;
    }
}

void nextButtonHandleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{

    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.printf("nextButtonHandleEvent currentFrames:%d frames_count:%d\n", currentFrames, frames_count);
        currentFrames =  ((currentFrames - 1) < 0) ? currentFrames : currentFrames - 1;
        handleMenu();
        break;
    case AceButton::kEventLongPressed:
        Serial.println("Long pressed!");
        sleepDevice();
        break;
    }
}

void setup()
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_ALL) {
        gpio_deep_sleep_hold_dis();
        gpio_hold_dis((gpio_num_t) RADIO_RST_PIN);
    }

    bool disable_u8g2 = true;

    setupBoards(disable_u8g2);

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    static ButtonConfig prevButtonConfigure;
    prevButtonConfigure.setEventHandler(prevButtonHandleEvent);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureClick);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureLongPress);
    pinMode(PREV_BUTTON_PIN, INPUT_PULLUP);
    prevButton.init(PREV_BUTTON_PIN);
    prevButton.setButtonConfig(&prevButtonConfigure);


    static ButtonConfig nextButtonConfigure;
    nextButtonConfigure.setEventHandler(nextButtonHandleEvent);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureClick);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureLongPress);
    pinMode(NEXT_BUTTON_PIN, INPUT_PULLUP);
    nextButton.init(NEXT_BUTTON_PIN);
    nextButton.setButtonConfig(&nextButtonConfigure);


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
    ui.setFrames(frames, frames_count);

    ui.disableAutoTransition();

    // Initialising the UI will init the display too.
    ui.init();

    ui.switchToFrame(0);

    display.flipScreenVertically();

    Serial.print(F("[Radio] Initializing ... "));

    transmissionState = radio.begin();
    if ( transmissionState == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.println(F("failed!"));
    }
    isRadioOnline = transmissionState == RADIOLIB_ERR_NONE;

    /*
     *   Sets carrier frequency.
     *   SX1278/SX1276 : Allowed values range from 137.0 MHz to 525.0 MHz.
     * * * */

    if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY) {
        Serial.println(F("Selected frequency is invalid for this module!"));
        while (true);
    }

    /*
    *   Sets LoRa link bandwidth.
    *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
    * * * */
    if (radio.setBandwidth(CONFIG_RADIO_BW) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
        while (true);
    }

    /*
    * Sets LoRa link spreading factor.
    * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
    * * * */
    if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        while (true);
    }

    /*
    * Sets LoRa coding rate denominator.
    * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
    * * * */
    if (radio.setCodingRate(6) == RADIOLIB_ERR_INVALID_CODING_RATE) {
        Serial.println(F("Selected coding rate is invalid for this module!"));
        while (true);
    }

    /*
    * Sets LoRa sync word.
    * SX1278/SX1276/SX1268/SX1262/SX1280 : Sets LoRa sync word. Only available in LoRa mode.
    * * */
    if (radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
        Serial.println(F("Unable to set sync word!"));
        while (true);
    }

    /*
    * Sets transmission output power.
    * SX1278/SX1276 :  Allowed values range from -3 to 15 dBm (RFO pin) or +2 to +17 dBm (PA_BOOST pin). High power +20 dBm operation is also supported, on the PA_BOOST pin. Defaults to PA_BOOST.
    * * * */
    if (radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }

    /*
    * Sets current limit for over current protection at transmitter amplifier.
    * SX1278/SX1276 : Allowed values range from 45 to 120 mA in 5 mA steps and 120 to 240 mA in 10 mA steps.
    * NOTE: set value to 0 to disable overcurrent protection
    * * * */
    if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        Serial.println(F("Selected current limit is invalid for this module!"));
        while (true);
    }

    /*
    * Sets preamble length for LoRa or FSK modem.
    * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
    * * */
    if (radio.setPreambleLength(15) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
        Serial.println(F("Selected preamble length is invalid for this module!"));
        while (true);
    }

    // Enables or disables CRC check of received packets.
    if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
        Serial.println(F("Selected CRC is invalid for this module!"));
        while (true);
    }



    // set the function that will be called
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);

    // start listening for LoRa packets
    Serial.print(F("[Radio] Starting to listen ... "));
    transmissionState = radio.startReceive();
    if (transmissionState != RADIOLIB_ERR_NONE) {
        Serial.println(F("[Radio] Received packet failed!"));
    }
}

void loop()
{
    prevButton.check();
    nextButton.check();
    ui.update();
    delay(2);
}


void radioTx(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_LEFT);


    if (millis() - radioRunInterval > 1000) {
        if (transmissionDirection == TRANSMISSION) {
            if (transmittedFlag) {

                // reset flag
                transmittedFlag = false;

                if (transmissionState == RADIOLIB_ERR_NONE) {
                    // packet was successfully sent
                    Serial.println(F("transmission finished!"));

                    // NOTE: when using interrupt-driven transmit method,
                    //       it is not possible to automatically measure
                    //       transmission data rate using getDataRate()

                } else {
                    Serial.print(F("failed, code "));
                    Serial.println(transmissionState);

                }

                // clean up after transmission is finished
                // this will ensure transmitter is disabled,
                // RF switch is powered down etc.
                radio.finishTransmit();

                // send another one
                Serial.print(F("[Radio] Sending another packet ... "));

                // you can transmit C-string or Arduino string up to
                // 256 characters long
                // transmissionState = radio.startTransmit("Hello World!");
                radio.transmit((uint8_t *)&transmissionCounter, 4);
                transmissionCounter++;

                // you can also transmit byte array up to 256 bytes long
                /*
                  byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                                    0x89, 0xAB, 0xCD, 0xEF};
                  int transmissionState = radio.startTransmit(byteArr, 8);
                */

                if (PMU) {
                    PMU->setChargingLedMode(led_blink);
                    led_blink ^= 1;
                }

                Serial.println("Radio TX done !");
            }
        }
        radioRunInterval = millis();
    }

    if (transmissionState !=  RADIOLIB_ERR_NONE) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 32 + y, "Radio Tx Failed!");
    } else {
        display->drawString(0 + x, 0 + y, "Radio Tx " + String(CONFIG_RADIO_FREQ) + "MHz");
        display->drawString(0 + x, 16 + y, "TX :" + String(transmissionCounter));
    }
}


void radioRx(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    display->setFont(Roboto_Mono_Medium_12);

    if (transmissionDirection == RECEIVE) {
        // check if the flag is set
        if (transmittedFlag) {

            Serial.println("Radio RX done !");

            if (PMU) {
                PMU->setChargingLedMode(led_blink);
                led_blink ^= 1;
            }

            // reset flag
            transmittedFlag = false;

            // you can read received data as an Arduino String
            transmissionState = radio.readData((uint8_t *)&recvCounter, 4);

            // you can also read received data as byte array
            /*
              byte byteArr[8];
              int transmissionState = radio.readData(byteArr, 8);
            */

            if (transmissionState == RADIOLIB_ERR_NONE) {
                // packet was successfully received
                Serial.println(F("[Radio] Received packet!"));
                radioRSSI = radio.getRSSI();

            } else if (transmissionState == RADIOLIB_ERR_CRC_MISMATCH) {
                // packet was received, but is malformed
                Serial.println(F("[Radio] CRC error!"));

            } else {
                // some other error occurred
                Serial.print(F("[Radio] Failed, code "));
                Serial.println(transmissionState);
            }

            // put module back to listen mode
            radio.startReceive();
        }
    }

    if (transmissionState !=  RADIOLIB_ERR_NONE) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 32 + y, "Radio Rx Failed!");
    } else {
        // The coordinates define the left starting point of the text
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 0 + y, "Radio Rx " + String(CONFIG_RADIO_FREQ) + "MHz");
        display->drawString(0 + x, 16 + y, "RX :" + String(recvCounter));
        display->drawString(0 + x, 32 + y, "RSSI:" + String(radioRSSI));
    }
}

void hwInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    static uint32_t  detect_interval = 0;
    static char buffer[64];
    if (millis() > detect_interval) {
        float voltage = PMU ? PMU->getBattVoltage() / 1000.0 : 0;
        sprintf(buffer, "%.2fV", voltage > 4.2 ? 4.2 : voltage);
        detect_interval = millis() +  1000;
    }

    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0 + x, 0 + y, "Radio  ");
    display->drawString(50 + x, 0 + y, isRadioOnline & 1 ? "+" : "NA");
    display->drawString(0 + x, 16 + y, "SD   ");
    uint64_t  card_size = SD.cardSize();
    if (card_size != 0) {
        float size_gb = card_size / 1024.0 / 1024.0 / 1024.0;
        display->drawString(50 + x, 16 + y, String(size_gb, 2) + "GB" );
    } else {
        display->drawString(50 + x, 16 + y, "NA");
    }
    display->drawString(0 + x, 32 + y, "BAT   ");
    display->drawString(50 + x, 32 + y, buffer);
}

void gpsInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    const uint8_t buffer_size = 128;
    static char buffer[buffer_size];
    static uint32_t gps_interval = 0;

    display->setFont(Roboto_Mono_Medium_12);

    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        snprintf(buffer, buffer_size, "lat:%.6f",  gps.location.lat());
        display->drawString(0 + x, 5 + y, buffer);
        snprintf(buffer, buffer_size, "lng:%.6f",   gps.location.lng());
        display->drawString(0 + x, 20 + y, buffer);
        snprintf(buffer, buffer_size, "%d/%d/%d %d:%d:%d", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
        display->drawString(0 + x, 35 + y, buffer);
    } else {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 16 + y, "RX:" + String(gps.charsProcessed()));
        display->drawString(64 + x, 32 + y, "GPS No Lock");
    }

    while (SerialGPS.available()) {
        gps.encode(SerialGPS.read());
    }
}


