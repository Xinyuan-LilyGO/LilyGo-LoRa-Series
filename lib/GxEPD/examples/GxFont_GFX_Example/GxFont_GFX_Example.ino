
// According to the board, cancel the corresponding macro definition
// #define LILYGO_T5_V213
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28
// #define LILYGO_T5_V102
// #define LILYGO_T5_V266
// #define LILYGO_EPD_DISPLAY_102      //Depend  https://github.com/adafruit/Adafruit_NeoPixel
// #define LILYGO_EPD_DISPLAY_154


#include <boards.h>
#include <GxEPD.h>

#if defined(LILYGO_T5_V102) || defined(LILYGO_EPD_DISPLAY_102)
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
#elif defined(LILYGO_T5_V266)
#include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
#elif defined(LILYGO_T5_V213)
#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
#else
// #include <GxDEPG0097BW/GxDEPG0097BW.h>     // 0.97" b/w  form DKE GROUP
// #include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.51" b/w   form DKE GROUP
// #include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
// #include <GxDEPG0290R/GxDEPG0290R.h>      // 2.9" b/w/r  form DKE GROUP
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9" b/w    form DKE GROUP
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r  form GoodDisplay
// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r form GoodDisplay
// #include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color
// #include <GxGDEW0213M21/GxGDEW0213M21.h>  // 2.13"  b/w Ultra wide temperature , form GoodDisplay
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w   form GoodDisplay
// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r form GoodDisplay
// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w   form GoodDisplay
// #include <GxDEPG0750BN/GxDEPG0750BN.h>    // 7.5" b/w   form DKE GROUP
#endif

#if !defined(_GxFont_GFX_TFT_eSPI_H_)
// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
//#include <Fonts/FreeMonoBold18pt7b.h>
//#include <Fonts/FreeMonoBold24pt7b.h>
#endif
#if defined(_ADAFRUIT_TF_GFX_H_)
#include <Fonts/Open_Sans_Bold_12pt.h>
#endif


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>


// #define USING_SOFT_SPI      //Uncomment this line to use software SPI

#if defined(USING_SOFT_SPI)
GxIO_Class io(EPD_SCLK, EPD_MISO, EPD_MOSI,  EPD_CS, EPD_DC,  EPD_RSET);
#else
GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
#endif
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

#if defined(_GxGDEW0154Z04_H_) || defined(_GxGDEW0213Z16_H_) || defined(_GxGDEW029Z10_H_) || defined(_GxGDEW027C44_H_) || defined(_GxGDEW042Z15_H_) || defined(_GxGDEW075Z09_H_)
#define HAS_RED_COLOR
#endif

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

#if !defined(USING_SOFT_SPI)
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
#endif

    display.init(115200); // enable diagnostic output on Serial

    Serial.println("setup done");
}

void loop()
{
#if !defined(_ADAFRUIT_TF_GFX_H_) && !defined(_GxFont_GFX_TFT_eSPI_H_) && !defined(U8g2_for_Adafruit_GFX_h)
    showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
#endif
#if defined(U8g2_for_Adafruit_GFX_h)
    showFont("u8g2_font_helvR14_tf", u8g2_font_helvR14_tf); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    delay(2000);
    showFont("u8g2_font_profont22_mr", u8g2_font_profont22_mr); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    delay(2000);
#endif
#if defined(_GxFont_GFX_TFT_eSPI_H_)
    showFreeFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
    delay(2000);
    //showFreeFont("NULL", NULL);
    showTextFont("Font 1", 1, 1);
    delay(2000);
    showTextFont("Font 1", 1, 2);
    delay(2000);
    showTextFont("Font 1", 1, 3);
    delay(2000);
    showTextFont("Font 2", 2, 1);
    delay(2000);
    showTextFont("Font 2", 2, 2);
    delay(2000);
    showTextFont("Font 2", 2, 3);
    delay(2000);
    showTextFont("Font 4", 4, 1);
    delay(2000);
    showTextFont("Font 4", 4, 2);
    delay(2000);
    showTextFont("Font 4", 4, 3);
#endif
#if defined(_ADAFRUIT_TF_GFX_H_)
    showFont("Open_Sans_Bold_12pt", OPENSANSBOLD_12);
#endif
    delay(10000);
}

void showFont(const char name[], const GFXfont *f)
{
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(f);
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
    display.setTextColor(GxEPD_RED);
#endif
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
    display.update();
    delay(5000);
}

#if defined(U8g2_for_Adafruit_GFX_h)
void showFont(const char name[], const uint8_t *f)
{
    display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
    display.setFontMode(1);                   // use u8g2 transparent mode (this is default)
    display.setFontDirection(0);              // left to right (this is default)
    display.setForegroundColor(GxEPD_BLACK);  // apply Adafruit GFX color
    display.setBackgroundColor(GxEPD_WHITE);  // apply Adafruit GFX color
    display.setFont(f); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
    display.setForegroundColor(GxEPD_RED);
#endif
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
    display.println("Umlaut ÄÖÜäéöü");
    display.update();
}
#endif

#if defined(_ADAFRUIT_TF_GFX_H_)
void showFont(const char name[], uint8_t f)
{
    display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(f);
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
    display.setTextColor(GxEPD_RED);
#endif
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
    display.update();
}
#endif

#if defined(_GxFont_GFX_TFT_eSPI_H_)
void showFreeFont(const char name[], const GFXfont *f)
{
    display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFreeFont(f);
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
    display.setTextColor(GxEPD_RED);
#endif
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
    display.update();
}
void showTextFont(const char name[], uint8_t f, uint8_t size)
{
    display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK, GxEPD_WHITE);
    display.setTextFont(f);
    display.setTextSize(size);
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
    display.setTextColor(GxEPD_RED);
#endif
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
    display.update();
}
#endif
