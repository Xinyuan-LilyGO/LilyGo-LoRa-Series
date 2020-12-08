/*
   RadioLib SX1278 Transmit Example

   This example transmits packets using SX1278 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   Other modules from SX127x/RFM9x family can also be used.

   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx127xrfm9x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/


#include <RadioLib.h>
#include "boards.h"

SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DI0_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

// save transmission state between loops
int transmissionState = ERR_NONE;

// flag to indicate that a packet was sent
volatile bool transmittedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;


// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void)
{
    // check if the interrupt is enabled
    if (!enableInterrupt) {
        return;
    }

    // we sent a packet, set the flag
    transmittedFlag = true;
}

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    // initialize SX1278 with default settings
    Serial.print(F("[SX1278] Initializing ... "));
#ifndef LoRa_frequency
    int state = radio.begin(433.0);
#else
    int state = radio.begin(LoRa_frequency);
#endif
#ifdef HAS_DISPLAY
    if (u8g2) {
        if (state != ERR_NONE) {
            u8g2->clearBuffer();
            u8g2->drawStr(0, 12, "Initializing: FAIL!");
            u8g2->sendBuffer();
        }
    }
#endif
    if (state == ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    // set the function that will be called
    // when packet transmission is finished
    radio.setDio0Action(setFlag);

    // start transmitting the first packet
    Serial.print(F("[SX1278] Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    transmissionState = radio.startTransmit("Hello World!");

    // you can also transmit byte array up to 256 bytes long
    /*
      byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
      state = radio.startTransmit(byteArr, 8);
    */



}


void loop()
{
// check if the previous transmission finished
    if (transmittedFlag) {
        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;

        // reset flag
        transmittedFlag = false;

        if (transmissionState == ERR_NONE) {
            // packet was successfully sent
            Serial.println(F("transmission finished!"));

            // NOTE: when using interrupt-driven transmit method,
            //       it is not possible to automatically measure
            //       transmission data rate using getDataRate()
#ifdef HAS_DISPLAY
            if (u8g2) {
                u8g2->clearBuffer();
                u8g2->drawStr(0, 12, "Transmitting: OK!");
                u8g2->drawStr(5, 30, String(millis()).c_str());
                u8g2->sendBuffer();
            }
#endif

        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);

        }

        // wait a second before transmitting again
        delay(1000);

        // send another one
        Serial.print(F("[SX1278] Sending another packet ... "));

        // you can transmit C-string or Arduino string up to
        // 256 characters long
        transmissionState = radio.startTransmit("Hello World!");

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
}


