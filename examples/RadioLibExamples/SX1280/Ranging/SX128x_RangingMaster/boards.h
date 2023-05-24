#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Ticker.h>
#include "utilities.h"

#ifdef HAS_SDCARD
#include <SD.h>
#include <FS.h>
#endif

#ifdef HAS_DISPLAY
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2 = nullptr;
#endif

Ticker ledTicker;
SPIClass SDSPI(HSPI);

void initBoard()
{
    Serial.begin(115200);
    Serial.println("initBoard");
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
    Wire.begin(I2C_SDA, I2C_SCL);


#ifdef HAS_GPS
    Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#endif

#if OLED_RST
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH); delay(20);
    digitalWrite(OLED_RST, LOW);  delay(20);
    digitalWrite(OLED_RST, HIGH); delay(20);
#endif

#ifdef BOARD_LED
    /*
    * T-BeamV1.0, V1.1 LED defaults to low level as trun on,
    * so it needs to be forced to pull up
    * * * * */
#if LED_ON == LOW
    gpio_hold_dis(GPIO_NUM_4);
#endif
    pinMode(BOARD_LED, OUTPUT);
    ledTicker.attach_ms(500, []() {
        static bool level;
        digitalWrite(BOARD_LED, level);
        level = !level;
    });
#endif


#ifdef HAS_DISPLAY
    Wire.beginTransmission(0x3C);
    if (Wire.endTransmission() == 0) {
        Serial.println("Started OLED");
        u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
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
#endif


#ifdef HAS_SDCARD
    if (u8g2) {
        u8g2->setFont(u8g2_font_ncenB08_tr);
    }
    pinMode(SDCARD_MISO, INPUT_PULLUP);
    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    if (u8g2) {
        u8g2->clearBuffer();
    }

    if (!SD.begin(SDCARD_CS, SDSPI)) {

        Serial.println("setupSDCard FAIL");
        if (u8g2) {
            do {
                u8g2->setCursor(0, 16);
                u8g2->println( "SDCard FAILED");;
            } while ( u8g2->nextPage() );
        }

    } else {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        if (u8g2) {
            do {
                u8g2->setCursor(0, 16);
                u8g2->print( "SDCard:");;
                u8g2->print(cardSize / 1024.0);;
                u8g2->println(" GB");;
            } while ( u8g2->nextPage() );
        }

        Serial.print("setupSDCard PASS . SIZE = ");
        Serial.print(cardSize / 1024.0);
        Serial.println(" GB");
    }
    if (u8g2) {
        u8g2->sendBuffer();
    }
    delay(3000);
#endif

}


