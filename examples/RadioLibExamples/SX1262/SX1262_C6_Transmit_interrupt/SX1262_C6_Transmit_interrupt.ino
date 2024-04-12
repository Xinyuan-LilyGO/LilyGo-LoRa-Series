/*
  RadioLib SX126x Transmit with Interrupts Example

  This example transmits LoRa packets with one second delays
  between them. Each packet contains up to 256 bytes
  of data, in the form of:
  - Arduino String
  - null-terminated char array (C-string)
  - arbitrary binary data (byte array)

  Other modules from SX126x family can also be used.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>
#include <WiFi.h> 
#include <Wire.h>
#include <U8g2lib.h>
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"



#define RADIO_SCLK_PIN          6
#define RADIO_MISO_PIN          1
#define RADIO_MOSI_PIN          0
#define RADIO_CS_PIN            18
#define RADIO_DIO1_PIN          23
#define RADIO_BUSY_PIN          22
#define RADIO_RST_PIN           21
#define I2C_SDA                     8
#define I2C_SCL                     9
#define LED_PIN    7

// SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);
#ifndef DISPLAY_MODEL
#define DISPLAY_MODEL U8G2_SSD1306_128X64_NONAME_F_HW_I2C
#endif

DISPLAY_MODEL *u8g2 = nullptr;

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
// or using RadioShield

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1262 radio = RadioShield.ModuleA;

// or using CubeCell
//SX1262 radio = new Module(RADIOLIB_BUILTIN_MODULE);

// save transmission state between loops
int transmissionState = RADIOLIB_ERR_NONE;

void setup() {
    Serial.begin(115200);
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.beginTransmission(0x3C);
    if (Wire.endTransmission() == 0) {
        Serial.println("Started OLED");
        u8g2 = new DISPLAY_MODEL(U8G2_R0, U8X8_PIN_NONE);
        u8g2->begin();
        u8g2->clearBuffer();
        u8g2->setFlipMode(0);
        u8g2->setFontMode(1); // Transparent
        u8g2->setDrawColor(1);
        u8g2->setFontDirection(0);
        u8g2->firstPage();
        do {
            u8g2->setFont(u8g2_font_inb19_mr);
            u8g2->drawStr(0, 30, "LilyGo");
            u8g2->drawHLine(2, 35, 47);
            u8g2->drawHLine(3, 36, 47);
            u8g2->drawVLine(45, 32, 12);
            u8g2->drawVLine(46, 33, 12);
            u8g2->setFont(u8g2_font_inb19_mf);
            u8g2->drawStr(58, 60, "LoRa");
        } while ( u8g2->nextPage() );
        u8g2->sendBuffer();
        u8g2->setFont(u8g2_font_fur11_tf);
        delay(3000);
    }
    radio.setRfSwitchPins(15, 14);
    Serial.println("0000");
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.println("1111");
    delay(100);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 0);
    Serial.println("Scannig WiFi...");
    if (u8g2) {
        u8g2->clearBuffer();
    }
    if (u8g2) {
      u8g2->setFont(u8g2_font_ncenB08_tr);
    }
    u8g2->setCursor(0, 16);
    u8g2->print("Scannig WiFi...");
    if (u8g2) {
        u8g2->sendBuffer();
    }
    int network = WiFi.scanNetworks();
    if(network == 0) {
        Serial.println("  No network found.");
        }else {
            Serial.print(network);
            Serial.println("  Network found.");
            for(int i = 0; i < network; ++i) {
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(WiFi.SSID(i));  // Get the SSID (name) of the network
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));  // Get the signal strength in dBm
                Serial.println(")");
                if (WiFi.SSID(i) == "ssid") { // Check if the current network has the specified name
                    Serial.println("WiFi network found");
                    Serial.print("wifi_channel: ");
                    Serial.println(WiFi.channel(i));  // Get the channel number of the network
        
        }
        }
    }
//   initialize SX1262 with default settings
    Serial.print(F("[SX1262] Initializing ... "));
    int state = radio.begin(868.0);
    if (state == RADIOLIB_ERR_NONE) {
        radio.setBandwidth(125);
        radio.setOutputPower(22);
        radio.setCurrentLimit(140);
        radio.setSpreadingFactor(8);
        Serial.println(F("success!"));

    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "Initializing: FAIL!");
        u8g2->sendBuffer();
        while (true);
}


  // set the function that will be called
  // when packet transmission is finished
    radio.setDio1Action(setFlag);

  // start transmitting the first packet
    Serial.print(F("[SX1262] Sending first packet ... "));

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

// flag to indicate that a packet was sent
volatile bool transmittedFlag = false;

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
    ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we sent a packet, set the flag
    transmittedFlag = true;
}

// counter to keep track of transmitted packets
int count = 0;

void loop() {
  // check if the previous transmission finished
    if(transmittedFlag) {
    // reset flag
    transmittedFlag = false;

    if (transmissionState == RADIOLIB_ERR_NONE) {
      // packet was successfully sent
        Serial.println(F("transmission finished!"));

      // NOTE: when using interrupt-driven transmit method,
      //       it is not possible to automatically measure
      //       transmission data rate using getDataRate()
    if (u8g2) {
        char buf[256];
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "Transmitting: OK!");
        u8g2->sendBuffer();
    }
    } else {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);

    }

    // clean up after transmission is finished
    // this will ensure transmitter is disabled,
    // RF switch is powered down etc.
    radio.finishTransmit();

    // wait a second before transmitting again
    delay(1000);

    // send another one
    Serial.print(F("[SX1262] Sending another packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    String str = "Hello World! #" + String(count++);
    transmissionState = radio.startTransmit(str);

    // you can also transmit byte array up to 256 bytes long
    /*
        byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
        transmissionState = radio.startTransmit(byteArr, 8);
    */
    }
}
