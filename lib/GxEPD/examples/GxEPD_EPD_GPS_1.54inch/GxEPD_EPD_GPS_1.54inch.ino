/*
    LilyGo epd display + gps Test
        - Created by Lewis he
*/

#define LILYGO_EPD_DISPLAY_154

#include <boards.h>
#include <GxEPD.h>

#if !defined(LILYGO_EPD_DISPLAY_154)
#error "No support !!!"
#endif

#include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w   form GoodDisplay

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <TinyGPS++.h>          // Depend https://github.com/mikalhart/TinyGPSPlus


// #define USING_SOFT_SPI      //Uncomment this line to use software SPI

#if defined(USING_SOFT_SPI)
GxIO_Class io(EPD_SCLK, EPD_MISO, EPD_MOSI,  EPD_CS, EPD_DC,  EPD_RSET);
#else
GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
#endif

GxEPD_Class     display(io, EPD_RSET, EPD_BUSY);

TinyGPSPlus     gps;
uint32_t        last = 0;

void displayInfo();

void setupGps()
{
    pinMode(GPS_1PPS_PIN, INPUT);
    pinMode(GPS_RESET_PIN, OUTPUT);
    pinMode(GPS_WAKEUP_PIN, OUTPUT);

    digitalWrite(GPS_RESET_PIN, HIGH); delay(20);
    digitalWrite(GPS_RESET_PIN, LOW); delay(20);
    digitalWrite(GPS_RESET_PIN, HIGH);

    digitalWrite(GPS_WAKEUP_PIN, HIGH);

    Serial1.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}



void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

#if !defined(USING_SOFT_SPI)
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
#endif

    display.init();
    display.setTextColor(GxEPD_BLACK);

    setupGps();

    display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
    display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);

    // Test backlight
    pinMode(EPD_BACKLIGHT_PIN, OUTPUT);
    digitalWrite(EPD_BACKLIGHT_PIN, HIGH);

    //Test motor
    pinMode(MOTOR_PIN, OUTPUT);
    int i = 3;
    while (i--) {
        digitalWrite(MOTOR_PIN, !digitalRead(MOTOR_PIN)); delay(500);
    }
    digitalWrite(MOTOR_PIN, LOW);


    display.update();

    delay(1000);

    display.setFont(&FreeMonoBold9pt7b);
    display.fillScreen(GxEPD_WHITE);
    display.update();
    delay(1000);
}

void loop()
{
    while (Serial1.available()) {
        int r = Serial1.read();
        Serial.write(r);
        if (gps.encode(r)) {
            displayInfo();
        }
    }
    if (millis() > 10000 && gps.charsProcessed() < 10) {
        Serial.println(F("No GPS detected: check wiring."));
        while (true);
    }
}


void displayInfo()
{
    if (millis() - last > 5000) {
        int xoffset =  92;
        display.fillRect(xoffset, 0, GxEPD_WIDTH - xoffset, GxEPD_HEIGHT, GxEPD_WHITE);

        display.setCursor(0, 25);
        display.print("[DIAGS]");
        display.setCursor(xoffset, 25);
        display.print(":");
        display.println(gps.charsProcessed());

        display.print("[Fix]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.println(gps.sentencesWithFix());

        display.print("[Pass]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.println(gps.passedChecksum());

        display.print("[SATE]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.println(gps.satellites.value());

        display.print("[Year]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.println(gps.date.year());
        display.print("[MM:DD]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.print(gps.date.month());
        display.print("/");
        display.println(gps.date.day());

        display.print("[H:M]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.print(gps.time.hour());
        display.print(":");
        display.println(gps.time.minute());

        display.print("[ln/la]");
        display.setCursor(xoffset, display.getCursorY());
        display.print(":");
        display.print(gps.location.lng(), 2);
        display.print("/");
        display.println(gps.location.lat(), 2);

        display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

        last = millis();
    }
}