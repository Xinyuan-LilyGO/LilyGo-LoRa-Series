/*
    RadioLib SX126x Receive with Interrupts Example

    This example listens for LoRa transmissions and tries to
    receive them. Once a packet is received, an interrupt is
    triggered. To successfully receive data, the following
    settings have to be the same on both transmitter
    and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word

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

#define RADIO_SCLK_PIN          6
#define RADIO_MISO_PIN          1
#define RADIO_MOSI_PIN          0
#define RADIO_CS_PIN            18
#define RADIO_DIO1_PIN          23
#define RADIO_BUSY_PIN          22
#define RADIO_RST_PIN           21
#define I2C_SDA                 8
#define I2C_SCL                 9
#define LED_PIN                 7

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
// https://github.com/jgromes/RadioShield
//SX1262 radio = RadioShield.ModuleA;
// or using CubeCell
//SX1262 radio = new Module(RADIOLIB_BUILTIN_MODULE);

void setup() {
    Serial.begin(115200);
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.beginTransmission(0x3C);
    if (Wire.endTransmission() == 0)
    {
        Serial.println("Started OLED");
        u8g2 = new DISPLAY_MODEL(U8G2_R0, U8X8_PIN_NONE);
        u8g2->begin();
        u8g2->clearBuffer();
        u8g2->setFlipMode(0);
        u8g2->setFontMode(1); // Transparent
        u8g2->setDrawColor(1);
        u8g2->setFontDirection(0);
        u8g2->firstPage();
        do
        {
            u8g2->setFont(u8g2_font_inb19_mr);
            u8g2->drawStr(0, 30, "LilyGo");
            u8g2->drawHLine(2, 35, 47);
            u8g2->drawHLine(3, 36, 47);
            u8g2->drawVLine(45, 32, 12);
            u8g2->drawVLine(46, 33, 12);
            u8g2->setFont(u8g2_font_inb19_mf);
            u8g2->drawStr(58, 60, "LoRa");
        } while (u8g2->nextPage());
        u8g2->sendBuffer();
        u8g2->setFont(u8g2_font_fur11_tf);
        delay(3000);
    }
    if (u8g2)
    {
        u8g2->setFont(u8g2_font_ncenB08_tr);
    }

    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 0);
    
    //Scannig WiFi
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
    // SPI.begin(5, 3, 6);
    // initialize SX1262 with default settings
    Serial.print(F("[SX1262] Initializing ... "));

    int state = radio.begin(850);//

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
    // when new packet is received
    radio.setDio1Action(setFlag);

    // start listening for LoRa packets
    Serial.print(F("[SX1262] Starting to listen ... "));
    state = radio.startReceive();

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    // if needed, 'listen' mode can be disabled by calling
    // any of the following methods:
    //
    // radio.standby()
    // radio.sleep()
    // radio.transmit();
    // radio.receive();
    // radio.scanChannel();
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
    ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we got a packet, set the flag
    if (!enableInterrupt)
    {
        return;
    }

    // we got a packet, set the flag
    receivedFlag = true;
}

void loop() {
  // check if the flag is set
    if(receivedFlag) {

    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    // you can also read received data as byte array
    /*
        byte byteArr[8];
        int numBytes = radio.getPacketLength();
        int state = radio.readData(byteArr, numBytes);
    */

    if (state == RADIOLIB_ERR_NONE) {
      // packet was successfully received
        Serial.println(F("[SX1262] Received packet!"));

      // print data of the packet
        Serial.print(F("[SX1262] Data:\t\t"));
        Serial.println(str);

      // print RSSI (Received Signal Strength Indicator)
        Serial.print(F("[SX1262] RSSI:\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
        Serial.print(F("[SX1262] SNR:\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        if (u8g2)
        {
            u8g2->clearBuffer();
            char buf[256];
            u8g2->drawStr(0, 12, "Received OK!");
            snprintf(buf, sizeof(buf), "RX:%s", str);
            u8g2->drawStr(0, 26, buf);
            snprintf(buf, sizeof(buf), "RSSI:%.2f", radio.getRSSI());
            u8g2->drawStr(0, 40, buf);
            snprintf(buf, sizeof(buf), "SNR:%.2f", radio.getSNR());
            u8g2->drawStr(0, 54, buf);
            u8g2->sendBuffer();
        }

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
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
