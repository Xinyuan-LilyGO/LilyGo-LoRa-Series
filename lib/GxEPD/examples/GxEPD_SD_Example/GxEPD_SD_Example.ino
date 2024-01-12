
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
#include <SD.h>
#include <FS.h>

#if defined(LILYGO_T5_V102) || defined(LILYGO_EPD_DISPLAY_102)
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
#include <Adafruit_NeoPixel.h>             //Depend  https://github.com/adafruit/Adafruit_NeoPixel
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



// has support for FAT32 support with long filenames
#define SdFile File
#define seekSet seek


// include library, include base class, make path known
#include <GxEPD.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>


#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
SPIClass SPI2(HSPI);
#endif

// #define USING_SOFT_SPI      //Uncomment this line to use software SPI

#if defined(USING_SOFT_SPI)
GxIO_Class io(EPD_SCLK, EPD_MISO, EPD_MOSI,  EPD_CS, EPD_DC,  EPD_RSET);
#else
GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
#endif
GxEPD_Class display(io, /*RST=*/ EPD_RSET, /*BUSY=*/ EPD_BUSY); // arbitrary selection of (16), 4


// function declaration with default parameter
void drawBitmapFromSD(const char *filename, int16_t x, int16_t y, bool with_color = true);

bool setupSDCard(void)
{
#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
    pinMode(SDCARD_MISO, INPUT_PULLUP);
    SPI2.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    return SD.begin(SDCARD_CS, SPI2);
#elif defined(_HAS_SDCARD_)
    return SD.begin(SDCARD_CS);
#endif
    return false;
}

void setup(void)
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("GxEPD_SD_Example");

#if !defined(USING_SOFT_SPI)
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
#endif

    display.init(115200); // enable diagnostic output on Serial

    Serial.print("Initializing SD card...");

#ifdef _HAS_SDCARD_
    if (!setupSDCard()) {
        Serial.println("failed!"); while (1)delay(100000);
    }
#endif


    drawBitmaps_200x200();
    drawBitmaps_other();

    //drawBitmaps_test();

    Serial.println("GxEPD_SD_Example done");
}

void loop()
{
}

void drawBitmaps_200x200()
{
    int16_t x = (display.width() - 200) / 2;
    int16_t y = (display.height() - 200) / 2;
    drawBitmapFromSD("logo200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("first200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("second200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("third200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("fourth200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("fifth200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("sixth200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("seventh200x200.bmp", x, y);
    delay(2000);
    drawBitmapFromSD("eighth200x200.bmp", x, y);
    delay(2000);
}

void drawBitmaps_other()
{
    int16_t w2 = display.width() / 2;
    int16_t h2 = display.height() / 2;
    drawBitmapFromSD("parrot.bmp", w2 - 64, h2 - 80);
    delay(2000);
    drawBitmapFromSD("betty_1.bmp", w2 - 100, h2 - 160);
    delay(2000);
    drawBitmapFromSD("betty_4.bmp", w2 - 102, h2 - 126);
    delay(2000);
    drawBitmapFromSD("marilyn_240x240x8.bmp", w2 - 120, h2 - 120);
    delay(2000);
    drawBitmapFromSD("miniwoof.bmp", w2 - 60, h2 - 80);
    delay(2000);
    drawBitmapFromSD("t200x200.bmp", w2 - 100, h2 - 100);
    delay(2000);
    drawBitmapFromSD("test.bmp", w2 - 120, h2 - 160);
    delay(2000);
    drawBitmapFromSD("tiger.bmp", w2 - 160, h2 - 120);
    delay(2000);
    drawBitmapFromSD("tiger_178x160x4.bmp", w2 - 89, h2 - 80);
    delay(2000);
    drawBitmapFromSD("tiger_240x317x4.bmp", w2 - 120, h2 - 160);
    delay(2000);
    drawBitmapFromSD("tiger_320x200x24.bmp", w2 - 160, h2 - 100);
    delay(2000);
    drawBitmapFromSD("tiger16T.bmp", w2 - 160, h2 - 120);
    delay(2000);
    drawBitmapFromSD("woof.bmp", w2 - 120, h2 - 160);
    delay(2000);
    drawBitmapFromSD("bitmap640x384_1.bmp", 0, 0);
    delay(2000);
}

void drawBitmaps_test()
{
    int16_t w2 = display.width() / 2;
    int16_t h2 = display.height() / 2;
    drawBitmapFromSD("betty_4.bmp", w2 - 102, h2 - 126);
    delay(2000);
    drawBitmapFromSD("bb4.bmp", 0, 0);
    delay(2000);
}

static const uint16_t input_buffer_pixels = 20; // may affect performance

static const uint16_t max_palette_pixels = 256; // for depth <= 8

uint8_t input_buffer[3 * input_buffer_pixels]; // up to depth 24
uint8_t mono_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w

void drawBitmapFrom_SD_ToBuffer(const char *filename, int16_t x, int16_t y, bool with_color)
{
    SdFile file;
    bool valid = false; // valid format to be handled
    bool flip = true; // bitmap is stored bottom-to-top
    uint32_t startTime = millis();
    if ((x >= display.width()) || (y >= display.height())) return;
    Serial.println();
    Serial.print("Loading image '");
    Serial.print(filename);
    Serial.println('\'');
#if defined(ESP32)
    file = SD.open(String("/") + filename, FILE_READ);
    if (!file) {
        Serial.print("File not found");
        return;
    }
#else
    if (!file.open(filename, FILE_READ)) {
        Serial.print("File not found");
        return;
    }
#endif
    // Parse BMP header
    if (read16(file) == 0x4D42) { // BMP signature
        uint32_t fileSize = read32(file);
        uint32_t creatorBytes = read32(file);
        uint32_t imageOffset = read32(file); // Start of image data
        uint32_t headerSize = read32(file);
        uint32_t width  = read32(file);
        uint32_t height = read32(file);
        uint16_t planes = read16(file);
        uint16_t depth = read16(file); // bits per pixel
        uint32_t format = read32(file);
        if ((planes == 1) && ((format == 0) || (format == 3))) { // uncompressed is handled, 565 also
            Serial.print("File size: "); Serial.println(fileSize);
            Serial.print("Image Offset: "); Serial.println(imageOffset);
            Serial.print("Header size: "); Serial.println(headerSize);
            Serial.print("Bit Depth: "); Serial.println(depth);
            Serial.print("Image size: ");
            Serial.print(width);
            Serial.print('x');
            Serial.println(height);
            // BMP rows are padded (if needed) to 4-byte boundary
            uint32_t rowSize = (width * depth / 8 + 3) & ~3;
            if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
            if (height < 0) {
                height = -height;
                flip = false;
            }
            uint16_t w = width;
            uint16_t h = height;
            if ((x + w - 1) >= display.width())  w = display.width()  - x;
            if ((y + h - 1) >= display.height()) h = display.height() - y;
            valid = true;
            uint8_t bitmask = 0xFF;
            uint8_t bitshift = 8 - depth;
            uint16_t red, green, blue;
            bool whitish, colored;
            if (depth == 1) with_color = false;
            if (depth <= 8) {
                if (depth < 8) bitmask >>= depth;
                //file.seekSet(54); //palette is always @ 54
                file.seekSet(imageOffset - (4 << depth)); // 54 for regular, diff for colorsimportant
                for (uint16_t pn = 0; pn < (1 << depth); pn++) {
                    blue  = file.read();
                    green = file.read();
                    red   = file.read();
                    file.read();
                    whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                    colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                    if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
                    mono_palette_buffer[pn / 8] |= whitish << pn % 8;
                    if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
                    color_palette_buffer[pn / 8] |= colored << pn % 8;
                }
            }
            display.fillScreen(GxEPD_WHITE);
            uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
            for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) { // for each line
                uint32_t in_remain = rowSize;
                uint32_t in_idx = 0;
                uint32_t in_bytes = 0;
                uint8_t in_byte = 0; // for depth <= 8
                uint8_t in_bits = 0; // for depth <= 8
                uint16_t color = GxEPD_WHITE;
                file.seekSet(rowPosition);
                for (uint16_t col = 0; col < w; col++) { // for each pixel
                    // Time to read more pixel data?
                    if (in_idx >= in_bytes) { // ok, exact match for 24bit also (size IS multiple of 3)
                        in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
                        in_remain -= in_bytes;
                        in_idx = 0;
                    }
                    switch (depth) {
                    case 24:
                        blue = input_buffer[in_idx++];
                        green = input_buffer[in_idx++];
                        red = input_buffer[in_idx++];
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                        break;
                    case 16: {
                        uint8_t lsb = input_buffer[in_idx++];
                        uint8_t msb = input_buffer[in_idx++];
                        if (format == 0) { // 555
                            blue  = (lsb & 0x1F) << 3;
                            green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                            red   = (msb & 0x7C) << 1;
                        } else { // 565
                            blue  = (lsb & 0x1F) << 3;
                            green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                            red   = (msb & 0xF8);
                        }
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                    }
                    break;
                    case 1:
                    case 4:
                    case 8: {
                        if (0 == in_bits) {
                            in_byte = input_buffer[in_idx++];
                            in_bits = 8;
                        }
                        uint16_t pn = (in_byte >> bitshift) & bitmask;
                        whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                        colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                        in_byte <<= depth;
                        in_bits -= depth;
                    }
                    break;
                    }
                    if (whitish) {
                        color = GxEPD_WHITE;
                    } else if (colored && with_color) {
                        color = GxEPD_RED;
                    } else {
                        color = GxEPD_BLACK;
                    }
                    uint16_t yrow = y + (flip ? h - row - 1 : row);
                    display.drawPixel(x + col, yrow, color);
                } // end pixel
            } // end line
            Serial.print("loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
        }
    }
    file.close();
    if (!valid) {
        Serial.println("bitmap format not handled.");
    }
}

#if defined(__AVR) //|| true

struct Parameters {
    const char *filename;
    int16_t x;
    int16_t y;
    bool with_color;
};

void drawBitmapFrom_SD_ToBuffer_Callback(const void *params)
{
    const Parameters *p = reinterpret_cast<const Parameters *>(params);
    drawBitmapFrom_SD_ToBuffer(p->filename, p->x, p->y, p->with_color);
}

void drawBitmapFromSD(const char *filename, int16_t x, int16_t y, bool with_color)
{
    Parameters parameters{filename, x, y, with_color};
    display.drawPaged(drawBitmapFrom_SD_ToBuffer_Callback, &parameters);
}

#else

void drawBitmapFromSD(const char *filename, int16_t x, int16_t y, bool with_color)
{
    drawBitmapFrom_SD_ToBuffer(filename, x, y, with_color);
    display.update();
}

#endif

uint16_t read16(SdFile &f)
{
    // BMP data is stored little-endian, same as Arduino.
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t read32(SdFile &f)
{
    // BMP data is stored little-endian, same as Arduino.
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}
