/*
   RadioLib Transmit with Interrupts Example

   This example transmits packets using SX1276/SX1278/SX1262/SX1268/SX1280/LR1121 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

#include "LoRaBoards.h"
#include <RadioLib.h>

#if     defined(USING_SX1276)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           868.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   17
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1278)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           433.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   17
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1262)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           850.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_SX1280)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           2400.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   13
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             203.125
#endif
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif  defined(USING_SX1280PA)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           2400.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   3           // PA Version power range : -18 ~ 3dBm
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             203.125
#endif
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_SX1268)
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           433.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
SX1268 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_LR1121)

// The maximum power of LR1121 2.4G band can only be set to 13 dBm
// #ifndef CONFIG_RADIO_FREQ
// #define CONFIG_RADIO_FREQ           2450.0
// #endif
// #ifndef CONFIG_RADIO_OUTPUT_POWER
// #define CONFIG_RADIO_OUTPUT_POWER   13
// #endif
// #ifndef CONFIG_RADIO_BW
// #define CONFIG_RADIO_BW             125.0
// #endif

// The maximum power of LR1121 Sub 1G band can only be set to 22 dBm
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           868.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   22
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif

LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO9_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif

void drawMain();

// save transmission state between loops
static int transmissionState = RADIOLIB_ERR_NONE;
// flag to indicate that a packet was sent
static volatile bool transmittedFlag = false;
static uint32_t counter = 0;
static String payload;

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
}

void setup()
{
    setupBoards();

    // When the power is turned on, a delay is required.
    delay(1500);

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    // initialize radio with default settings
    int state = radio.begin();

    printResult(state == RADIOLIB_ERR_NONE);

    Serial.printf("[%s]:", RADIO_TYPE_STR);
    Serial.print(F("Radio Initializing ... "));
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    // set the function that will be called
    // when packet transmission is finished
    radio.setPacketSentAction(setFlag);

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
    if (radio.setSpreadingFactor(12) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
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
    * SX1280        : Allowed values range from 1 to 65535. preamble length is multiple of 4
    * LR1121        : Allowed values range from 1 to 65535.
    * * */
    if (radio.setPreambleLength(16) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
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

#ifdef USING_DIO2_AS_RF_SWITCH
#ifdef USING_SX1262
    // Some SX126x modules use DIO2 as RF switch. To enable
    // this feature, the following method can be used.
    // NOTE: As long as DIO2 is configured to control RF switch,
    //       it can't be used as interrupt pin!
    if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
        Serial.println(F("Failed to set DIO2 as RF switch!"));
        while (true);
    }
#endif //USING_SX1262
#endif //USING_DIO2_AS_RF_SWITCH

#ifdef RADIO_RX_PIN
    // SX1280 PA Version
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif

#ifdef RADIO_SWITCH_PIN
    // T-MOTION
    const uint32_t pins[] = {
        RADIO_SWITCH_PIN, RADIO_SWITCH_PIN, RADIOLIB_NC,
    };
    static const Module::RfSwitchMode_t table[] = {
        {Module::MODE_IDLE,  {0,  0} },
        {Module::MODE_RX,    {1, 0} },
        {Module::MODE_TX,    {0, 1} },
        END_OF_MODE_TABLE,
    };
    radio.setRfSwitchTable(pins, table);
#endif

#ifdef RADIO_CTRL
    Serial.println("Turn off LAN,Trun on PA, Enter Tx mode.");
    /*
    * 2W LoRa LAN Control ,set Low turn off LAN , TX Mode
    * */
    digitalWrite(RADIO_CTRL, LOW);
#endif /*RADIO_CTRL*/


    // start transmitting the first packet
    Serial.print(F("Radio Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    transmissionState = radio.startTransmit(String(counter).c_str());

    // you can also transmit byte array up to 256 bytes long
    /*
      byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
      state = radio.startTransmit(byteArr, 8);
    */
    delay(1000);

    drawMain();
}

void loop()
{
    // check if the previous transmission finished
    if (transmittedFlag) {

        payload = "#" + String(counter++);

        // reset flag
        transmittedFlag = false;

        flashLed();


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


        drawMain();
        // wait a second before transmitting again
        delay(1000);

        // send another one
        Serial.print(F("Radio Sending another packet ... "));

        // you can transmit C-string or Arduino string up to
        // 256 characters long
        transmissionState = radio.startTransmit(payload);
        // you can also transmit byte array up to 256 bytes long
        /*
          byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                            0x89, 0xAB, 0xCD, 0xEF};
          int state = radio.startTransmit(byteArr, 8);
        */

    }
}


void drawMain()
{
    if (u8g2) {
        u8g2->clearBuffer();
        u8g2->drawRFrame(0, 0, 128, 64, 5);

        u8g2->setFont(u8g2_font_pxplusibmvga8_mr);
        u8g2->setCursor(22, 25);
        u8g2->print("TX:");
        u8g2->setCursor(22, 40);
        u8g2->print("STATE:");

        u8g2->setFont(u8g2_font_crox1h_tr);
        u8g2->setCursor( U8G2_HOR_ALIGN_RIGHT(payload.c_str()) - 21, 25 );
        u8g2->print(payload);
        String state = transmissionState == RADIOLIB_ERR_NONE ? "NONE" : String(transmissionState);
        u8g2->setCursor( U8G2_HOR_ALIGN_RIGHT(state.c_str()) -  21, 40 );
        u8g2->print(state);
        u8g2->sendBuffer();
    }
}