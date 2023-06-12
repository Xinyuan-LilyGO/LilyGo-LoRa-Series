/*
   RadioLib SX128x Ranging Example

   This example performs ranging exchange between two
   SX1280 LoRa radio modules. Ranging allows to measure
   distance between the modules using time-of-flight
   measurement.

   Only SX1280 and SX1282 without external RF switch support ranging!

   Note that to get accurate ranging results, calibration is needed!
   The process is described in Semtech SX1280 Application Note AN1200.29

   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx128x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>
#include "boards.h"

SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);


void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    if (u8g2) {
        u8g2->clearBuffer();
        u8g2->setCursor(0, 16);
        u8g2->println( "RangingMaster");
        u8g2->sendBuffer();
    }


    // initialize SX1280 with default settings
    Serial.print(F("[SX1280] Initializing ... "));
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

#if defined(RADIO_RX_PIN) && defined(RADIO_TX_PIN)
    //Set ANT Control pins
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif


#ifdef LILYGO_T3_S3_V1_0
    // T3 S3 V1.1 with PA Version Set output power to 3 dBm    !!Cannot be greater than 3dbm!!
    int8_t TX_Power = 3;
#else
    // T3 S3 V1.2 (No PA) Version Set output power to 3 dBm    !!Cannot be greater than 3dbm!!
    int8_t TX_Power = 13;
#endif
    if (radio.setOutputPower(TX_Power) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }

    Serial.println("SX128x_RangingMaster start!");


}

void loop()
{
    Serial.print(F("[SX1280] Ranging ... "));

    // start ranging exchange
    // range as master:             true
    // slave address:               0x12345678
    int state = radio.range(true, 0x12345678);

    // the other module must be configured as slave with the same address
    /*
    */
    // int state = radio.range(false, 0x12345678);

    // if ranging calibration is known, it can be provided
    // this should improve the accuracy and precision
    /*
      uint16_t calibration[3][6] = {
        { 10299, 10271, 10244, 10242, 10230, 10246 },
        { 11486, 11474, 11453, 11426, 11417, 11401 },
        { 13308, 13493, 13528, 13515, 13430, 13376 }
      };

      int state = radio.range(true, 0x12345678, calibration);
    */

    if (u8g2) {
        u8g2->clearBuffer();
        u8g2->setCursor(0, 16);
        u8g2->println( "RangingMaster");
        u8g2->setCursor(0, 32);
    }

    if (state == RADIOLIB_ERR_NONE) {
        // ranging finished successfully

        float raw = radio.getRangingResult();
        Serial.println(F("success!"));
        Serial.print(F("[SX1280] Distance:\t\t\t"));
        Serial.print(raw);
        Serial.println(F(" meters (raw)"));

        if (u8g2) {
            u8g2->print( "Distance:"); u8g2->print( raw); u8g2->print("meters(raw)");
        }

    } else if (state == RADIOLIB_ERR_RANGING_TIMEOUT) {
        // timed out waiting for ranging packet
        Serial.println(F("timed out!"));

        if (u8g2) {
            u8g2->print( "Timed out!");
        }

    } else {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
        if (u8g2) {
            u8g2->print( "Failed!");
        }
    }

    if (u8g2) {
        u8g2->sendBuffer();
    }

    // wait for a second before ranging again
    delay(1000);
}
