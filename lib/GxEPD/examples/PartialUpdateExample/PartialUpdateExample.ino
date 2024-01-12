/*
    LilyGo Ink Screen Series Test
        - Created by Lewis he
*/

// According to the board, cancel the corresponding macro definition
// #define LILYGO_T5_V213
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28
// #define LILYGO_T5_V102
// #define LILYGO_T5_V266
// #define LILYGO_EPD_DISPLAY_102
// #define LILYGO_EPD_DISPLAY_154

#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>


#if defined(LILYGO_T5_V102) || defined(LILYGO_EPD_DISPLAY_102)
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
#elif defined(LILYGO_T5_V266)
#include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
#elif defined(LILYGO_T5_V213)
#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
#else
// #include <GxDEPG0097BW/GxDEPG0097BW.h>     // 0.97" b/w  form DKE GROUP
// #include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w 80x128 , form good display
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54"  b/w/r 200x200 , form good display
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54"  b/w/r 152x152 , form good display
// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54"  b/w   , form good display

// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.51"  b/w   form DKE GROUP

// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7"   b/w/r, form good display
// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7"   b/w  , form good display

// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13"  b/w/r, form good display

// old panel
// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13"  b/w old panel, form good display
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13"  b/w old panel, form good display

// new panel
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13"  b/w    form DKE GROUP
// #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13"  b/w    form GoodDisplay 4-color, form good display
// #include <GxGDEW0213M21/GxGDEW0213M21.h>  // 2.13"  b/w    Ultra wide temperature, form good display

// #include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66"  b/w    form DKE GROUP
// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9"   b/w    form DKE GROUP
// #include <GxQYEG0290BN/GxQYEG0290BN.h>    // 2.9"   b/w    form DKE GROUP
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9"   b/w    form DKE GROUP
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9"   b/w/r  form good display
// #include <GxDEPG0290R/GxDEPG0290R.h>      // 2.9"   b/w/r  form DKE GROUP
// #include <GxDEPG0750BN/GxDEPG0750BN.h>    // 7.5"   b/w    form DKE GROUP
#endif

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

// #define USING_SOFT_SPI      //Uncomment this line to use software SPI

#if defined(USING_SOFT_SPI)
GxIO_Class io(EPD_SCLK, EPD_MISO, EPD_MOSI,  EPD_CS, EPD_DC,  EPD_RSET);
#else
GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
#endif
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);


void setup(void)
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

#if defined(LILYGO_EPD_DISPLAY_102)
    pinMode(EPD_POWER_ENABLE, OUTPUT);
    digitalWrite(EPD_POWER_ENABLE, HIGH);
#endif /*LILYGO_EPD_DISPLAY_102*/

#if !defined(USING_SOFT_SPI)
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
#endif
    display.init(); // enable diagnostic output on Serial
    Serial.println("setup done");
}


void loop()
{
// use asymmetric values for test
#if defined(_GxGDGDEW0102T4_H_)
    uint16_t box_x = 5;
    uint16_t box_y = 5;
    uint16_t box_w = 35;
    uint16_t box_h = 15;
    uint16_t cursor_y = box_y + box_h - 10;
#else
    uint16_t box_x = 10;
    uint16_t box_y = 15;
    uint16_t box_w = 70;
    uint16_t box_h = 20;
    uint16_t cursor_y = box_y + box_h - 6;
    display.setFont(&FreeMonoBold9pt7b);
#endif
    float value = 13.95;
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(0);
    // draw background
    display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
    display.update();
    delay(2000);

    // partial update to full screen to preset for partial update of box window
    // (this avoids strange background effects)
    display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

    // show where the update box is
    for (uint16_t r = 0; r < 4; r++) {
        display.setRotation(r);
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
        display.updateWindow(box_x, box_y, box_w, box_h, true);
        delay(1000);
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        display.updateWindow(box_x, box_y, box_w, box_h, true);
    }
    // show updates in the update box
    for (uint16_t r = 0; r < 4; r++) {
        // reset the background
        display.setRotation(0);
        display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
        display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
        display.setRotation(r);
        for (uint16_t i = 1; i <= 10; i++) {
            display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
            display.setCursor(box_x, cursor_y);
            display.print(value * i, 2);
            display.updateWindow(box_x, box_y, box_w, box_h, true);
            delay(2000);
        }
        delay(2000);
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        display.updateWindow(box_x, box_y, box_w, box_h, true);
    }
    // should have checked this, too
    box_x = GxEPD_HEIGHT - box_x - box_w - 1; // not valid for all corners
    // should show on right side of long side
    // reset the background
    display.setRotation(0);
    display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
    display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
    // show where the update box is
    for (uint16_t r = 0; r < 4; r++) {
        display.setRotation(r);
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
        display.updateWindow(box_x, box_y, box_w, box_h, true);
        delay(1000);
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        display.updateWindow(box_x, box_y, box_w, box_h, true);
    }
    // show updates in the update box
    for (uint16_t r = 0; r < 4; r++) {
        // reset the background
        display.setRotation(0);
        display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
        display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
        display.setRotation(r);
        if (box_x >= display.width()) continue; // avoid delay
        for (uint16_t i = 1; i <= 10; i++) {
            display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
            display.setCursor(box_x, cursor_y);
            display.print(value * i, 2);
            display.updateWindow(box_x, box_y, box_w, box_h, true);
            delay(2000);
        }
        delay(2000);
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        display.updateWindow(box_x, box_y, box_w, box_h, true);
    }
    display.setRotation(0);
    display.powerDown();
    delay(30000);
}
