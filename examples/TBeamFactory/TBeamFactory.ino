/**
 * @file      TBeamFactory.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2024-11-18
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
#include "LoRaBoards.h"

using namespace ace_button;

void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void radioRx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void hwInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);

#if     defined(USING_SX1276)
#define CONFIG_RADIO_FREQ           868.0
#define CONFIG_RADIO_OUTPUT_POWER   17
#define CONFIG_RADIO_BW             125.0
SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1278)
#define CONFIG_RADIO_FREQ           433.0
#define CONFIG_RADIO_OUTPUT_POWER   17
#define CONFIG_RADIO_BW             125.0
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1262)
#define CONFIG_RADIO_FREQ           850.0
#define CONFIG_RADIO_OUTPUT_POWER   22
#define CONFIG_RADIO_BW             125.0

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_SX1280)
#define CONFIG_RADIO_FREQ           2400.0
#define CONFIG_RADIO_OUTPUT_POWER   13
#define CONFIG_RADIO_BW             203.125
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif  defined(USING_SX1280PA)
#define CONFIG_RADIO_FREQ           2400.0
#define CONFIG_RADIO_OUTPUT_POWER   3           // PA Version power range : -18 ~ 3dBm
#define CONFIG_RADIO_BW             203.125
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_LR1121)

// The maximum power of LR1121 2.4G band can only be set to 13 dBm
#define CONFIG_RADIO_FREQ           2450.0
#define CONFIG_RADIO_OUTPUT_POWER   13
#define CONFIG_RADIO_BW             125.0

// The maximum power of LR1121 Sub 1G band can only be set to 22 dBm
// #define CONFIG_RADIO_FREQ           868.0
// #define CONFIG_RADIO_OUTPUT_POWER   22
// #define CONFIG_RADIO_BW             125.0

LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO9_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif


// save transmission state between loops
int             transmissionState = RADIOLIB_ERR_NONE;
bool            transmittedFlag = false;
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
AceButton       button;


void setFlag(void)
{
    // we got a packet, set the flag
    transmittedFlag = true;
}


void handleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{
    int state ;
    static uint8_t frameCounter = 1;
    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.printf("frameCounter : %d\n", frameCounter);
        switch (frameCounter) {
        case 0:
            break;
        case 1:
            Serial.println("Start transmit");
            state = radio.transmit((uint8_t *)&transmissionCounter, 4);
            if (state != RADIOLIB_ERR_NONE) {
                Serial.println(F("[Radio] transmit packet failed!"));
            }
            break;
        case 2:
            Serial.println("Start receive");
            state = radio.startReceive();
            if (state != RADIOLIB_ERR_NONE) {
                Serial.println(F("[Radio] Received packet failed!"));
            }
            break;
        default:
            break;
        }
        frameCounter++;
        ui.nextFrame();
        frameCounter %= 3;
        break;

    case AceButton::kEventLongPressed:

        Serial.println("Long pressed!");

        Serial.println("Enter sleep...");

#ifndef HAS_PMU
        digitalWrite(RADIO_RST_PIN, HIGH);
        gpio_hold_en((gpio_num_t) RADIO_RST_PIN);
        gpio_deep_sleep_hold_en();
#endif
        radio.sleep();

        display.clear();
        display.drawString(60, 28, "Sleep");
        display.display();
        delay(2000);
        display.displayOff();


#ifdef  RADIO_TCXO_ENABLE
        digitalWrite(RADIO_TCXO_ENABLE, LOW);
#endif

#ifdef BUTTON_PIN
        while (digitalRead(BUTTON_PIN) == LOW)
            delay(1);
#endif

        // PMU pre-sleep operation
        disablePeripherals();

        delay(200);

        SPI.end();

        Wire.end();



        // GPS pins
#ifdef HAS_GPS
        SerialGPS.end();
        pinMode(GPS_RX_PIN, INPUT);
        pinMode(GPS_TX_PIN, INPUT);
#endif

#ifdef OLED_RST
        pinMode(OLED_RST, INPUT);
#endif
        // Wire pins
        pinMode(I2C_SDA, INPUT);
        pinMode(I2C_SCL, INPUT);


        // Radio pins
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
        pinMode(RADIO_CS_PIN, INPUT);
        pinMode(RADIO_RST_PIN, INPUT);
        pinMode(RADIO_SCLK_PIN, INPUT);
        pinMode(RADIO_MISO_PIN, INPUT);
        pinMode(RADIO_MOSI_PIN, INPUT);

        // SD Card pins
#ifdef SDCARD_MOSI
        pinMode(SDCARD_MOSI, INPUT);
        pinMode(SDCARD_MISO, INPUT);
        pinMode(SDCARD_SCLK, INPUT);
        pinMode(SDCARD_CS, INPUT);
#endif

#ifdef HAS_PMU
        pinMode(PMU_IRQ, INPUT);
#endif

#ifdef BOARD_LED
        pinMode(BOARD_LED, INPUT);
#endif

#ifdef ADC_PIN
        pinMode(ADC_PIN, INPUT);
#endif

        pinMode(BUTTON_PIN, INPUT);

        // GPIO WAKE UP EXT 1 NO  OLED  Display ~ 440 uA ,
        // GPIO WAKE UP EXT 1 +  OLED  Display  ~ 450 uA ,
        // See sleep_current.jpg
        // Serial.println("GPIO WAKE UP EXT 1");
        // esp_sleep_enable_ext1_wakeup(_BV(BUTTON_PIN), ESP_EXT1_WAKEUP_ALL_LOW);


        // GPIO WAKE UP EXT 1 + TIMER WAKE UP NO  OLED  Display ~ 440 uA
        // GPIO WAKE UP EXT 1 + TIMER WAKE UP +  OLED  Display ~  450uA
        // Serial.println("GPIO WAKE UP EXT 1 + TIMER WAKE UP");
        // esp_sleep_enable_ext1_wakeup(_BV(BUTTON_PIN), ESP_EXT1_WAKEUP_ALL_LOW);
        // esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);


        // GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP  NO  OLED  Display + Disable gnss power backup power ~ 420 uA
        // GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP  NO  OLED  Display ~ 540 uA
        // GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP  +  OLED  Display  ~ 580 uA
        // Serial.println("GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP");
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);
        esp_sleep_enable_ext1_wakeup(_BV(BUTTON_PIN), ESP_EXT1_WAKEUP_ALL_LOW);
        esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);


        Serial.flush();

        Serial.end();

        delay(1000);

        esp_deep_sleep_start();

        Serial.println("Never print()");
        break;
    }
}

void setup()
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_ALL) {
#ifndef HAS_PMU
        gpio_deep_sleep_hold_dis();
        gpio_hold_dis((gpio_num_t) RADIO_RST_PIN);
#endif
    }

    setupBoards(true);

    delay(1000);

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

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

    Serial.print(F("[Radio] Initializing ... "));
    int  state = radio.begin();
    if ( state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.println(F("failed!"));
    }
    isRadioOnline = state == RADIOLIB_ERR_NONE;

#if defined(RADIO_RX_PIN) && defined(RADIO_TX_PIN)
    //The SX1280 version needs to set RX, TX antenna switching pins
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif

    /*
     *   Sets carrier frequency.
     *   SX1278/SX1276 : Allowed values range from 137.0 MHz to 525.0 MHz.
     *   SX1268/SX1262 : Allowed values are in range from 150.0 to 960.0 MHz.
     *   SX1280        : Allowed values are in range from 2400.0 to 2500.0 MHz.
     *   LR1121        : Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
     * * * */

    if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY) {
        Serial.println(F("Selected frequency is invalid for this module!"));
        while (true);
    }

    /*
    *   Sets LoRa link bandwidth.
    *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
    *   SX1268/SX1262 : Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
    *   SX1280        : Allowed values are 203.125, 406.25, 812.5 and 1625.0 kHz.
    *   LR1121        : Allowed values are 62.5, 125.0, 250.0 and 500.0 kHz.
    * * * */
    if (radio.setBandwidth(CONFIG_RADIO_BW) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
        while (true);
    }


    /*
    * Sets LoRa link spreading factor.
    * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
    * SX1262        :  Allowed values range from 5 to 12.
    * SX1280        :  Allowed values range from 5 to 12.
    * LR1121        :  Allowed values range from 5 to 12.
    * * * */
    if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        while (true);
    }

    /*
    * Sets LoRa coding rate denominator.
    * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
    * SX1280        :  Allowed values range from 5 to 8.
    * LR1121        :  Allowed values range from 5 to 8.
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
    * SX1262        :  Allowed values are in range from -9 to 22 dBm. This method is virtual to allow override from the SX1261 class.
    * SX1268        :  Allowed values are in range from -9 to 22 dBm.
    * SX1280        :  Allowed values are in range from -18 to 13 dBm. PA Version range : -18 ~ 3dBm
    * LR1121        :  Allowed values are in range from -17 to 22 dBm (high-power PA) or -18 to 13 dBm (High-frequency PA)
    * * * */
    if (radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }

#if !defined(USING_SX1280) && !defined(USING_LR1121) && !defined(USING_SX1280PA)
    /*
    * Sets current limit for over current protection at transmitter amplifier.
    * SX1278/SX1276 : Allowed values range from 45 to 120 mA in 5 mA steps and 120 to 240 mA in 10 mA steps.
    * SX1262/SX1268 : Allowed values range from 45 to 120 mA in 2.5 mA steps and 120 to 240 mA in 10 mA steps.
    * NOTE: set value to 0 to disable overcurrent protection
    * * * */
    if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        Serial.println(F("Selected current limit is invalid for this module!"));
        while (true);
    }
#endif

    /*
    * Sets preamble length for LoRa or FSK modem.
    * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
    * SX1262/SX1268 : Allowed values range from 1 to 65535.
    * SX1280        : Allowed values range from 1 to 65535.
    * LR1121        : Allowed values range from 1 to 65535.
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


#if  defined(USING_LR1121)
    // LR1121
    // set RF switch configuration for Wio WM1110
    // Wio WM1110 uses DIO5 and DIO6 for RF switching
    static const uint32_t rfswitch_dio_pins[] = {
        RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
        RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC
    };

    static const Module::RfSwitchMode_t rfswitch_table[] = {
        // mode                  DIO5  DIO6
        { LR11x0::MODE_STBY,   { LOW,  LOW  } },
        { LR11x0::MODE_RX,     { HIGH, LOW  } },
        { LR11x0::MODE_TX,     { LOW,  HIGH } },
        { LR11x0::MODE_TX_HP,  { LOW,  HIGH } },
        { LR11x0::MODE_TX_HF,  { LOW,  LOW  } },
        { LR11x0::MODE_GNSS,   { LOW,  LOW  } },
        { LR11x0::MODE_WIFI,   { LOW,  LOW  } },
        END_OF_MODE_TABLE,
    };
    radio.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);

    // LR1121 TCXO Voltage 2.85~3.15V
    radio.setTCXO(3.0);

#endif

    // set the function that will be called
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);

    // start listening for LoRa packets
    Serial.print(F("[Radio] Starting to listen ... "));
    state = radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) {
        Serial.println(F("[Radio] Received packet failed!"));
    }
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
              int state = radio.startTransmit(byteArr, 8);
            */
            digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));
        }

        Serial.println("Radio TX done !");
        radioRunInterval = millis();

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
        Serial.println("Radio RX done !");

        digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));

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

    }

    display->drawString(0 + x, 0 + y, "Radio Rx");
    display->drawString(0 + x, 22 + y, "RX :" + String(recvCounter));
    display->drawString(0 + x, 10 + y, "RSSI:" + String(radioRSSI));


}

void hwInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static char buffer[64] = {'0'};
#ifdef ADC_PIN
    if (millis() - batteryRunInterval > 1000) {
        analogReadResolution(12);
        float voltage = (analogReadMilliVolts(ADC_PIN) * 2) / 1000.0;
        sprintf(buffer, "%.2fV", voltage > 4.2 ? 4.2 : voltage);
        batteryRunInterval = millis();
    }
#endif

#ifdef HAS_PMU
    if (millis() - batteryRunInterval > 1000) {
        uint16_t voltage = PMU->getBattVoltage();
        sprintf(buffer, "%.2fV", voltage / 1000.0 );
        batteryRunInterval = millis();
    }
#endif

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0 + x, 10 + y, "Radio  ");
    display->drawString(50 + x, 10 + y, isRadioOnline & 1 ? "+" : "NA");
    display->drawString(0 + x, 20 + y, "SD   ");
    display->drawString(50 + x, 20 + y, SD.cardSize() != 0 ? "+" : "NA");
    display->drawString(0 + x, 30 + y, "BAT   ");
    display->drawString(50 + x, 30 + y, buffer);
}




