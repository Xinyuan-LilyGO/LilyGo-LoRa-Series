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
#include "LoRaBoards.h"


SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);


void setup()
{
    Serial.begin(115200);

    setupBoards();

    // initialize SX1280 with default settings
    Serial.print(F("[SX1280] Initializing ... "));
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true) {
            delay(10);
        }
    }

#ifdef RADIO_RX_PIN
    // SX1280 PA Version
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif

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
      int state = radio.range(false, 0x12345678);
    */

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
    String result = "Wait..";

    if (state == RADIOLIB_ERR_NONE) {
        // ranging finished successfully
        Serial.println(F("success!"));
        Serial.print(F("[SX1280] Distance:\t\t\t"));
        Serial.print(radio.getRangingResult());
        Serial.println(F(" meters (raw)"));

        result = radio.getRangingResult();


    } else if (state == RADIOLIB_ERR_RANGING_TIMEOUT) {
        // timed out waiting for ranging packet
        Serial.println(F("timed out!"));

    } else {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);

    }

    if (u8g2) {
        u8g2->clearBuffer();
        u8g2->drawRFrame(0, 0, 128, 64, 5);

        u8g2->setFont(u8g2_font_pxplusibmvga8_mr);
        u8g2->setCursor(22, 25);
        u8g2->print("Meters(raw):");
        u8g2->setCursor(22, 40);
        u8g2->print("STATE:");

        u8g2->setFont(u8g2_font_crox1h_tr);
        u8g2->setCursor( U8G2_HOR_ALIGN_RIGHT(result.c_str()) - 21, 25 );
        u8g2->print(result);
        String state = state == RADIOLIB_ERR_NONE ? "NONE" : String(state);
        u8g2->setCursor( U8G2_HOR_ALIGN_RIGHT(state.c_str()) -  21, 40 );
        u8g2->print(state);
        u8g2->sendBuffer();
    }


    // wait for a second before ranging again
    delay(1000);
}

