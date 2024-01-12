// class GxDEPG0290R :
// Driver: SSD1680Z8
#ifndef _GxDEPG0290R_H_
#define _GxDEPG0290R_H_

#include "../GxEPD.h"

#define GxDEPG0290R_WIDTH 128
#define GxDEPG0290R_HEIGHT 296

#define GxDEPG0290R_BUFFER_SIZE (uint32_t(GxDEPG0290R_WIDTH) * uint32_t(GxDEPG0290R_HEIGHT) / 8)

// divisor for AVR, should be factor of GxDEPG0290R_HEIGHT
#define GxDEPG0290R_PAGES 8

#define GxDEPG0290R_PAGE_HEIGHT (GxDEPG0290R_HEIGHT / GxDEPG0290R_PAGES)
#define GxDEPG0290R_PAGE_SIZE (GxDEPG0290R_BUFFER_SIZE / GxDEPG0290R_PAGES)

class GxDEPG0290R : public GxEPD
{
public:
#if defined(ESP8266)
    //GxDEPG0290R(GxIO& io, int8_t rst = D4, int8_t busy = D2);
    // use pin numbers, other ESP8266 than Wemos may not use Dx names
    GxDEPG0290R(GxIO &io, int8_t rst = 2, int8_t busy = 4);
#else
    GxDEPG0290R(GxIO &io, int8_t rst = 9, int8_t busy = 7);
#endif
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void init(uint32_t serial_diag_bitrate = 0); // = 0 : disabled
    void fillScreen(uint16_t color); // to buffer
    void update(void);
    // to buffer, may be cropped, drawPixel() used, update needed
    void  drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode = bm_normal);
    // to full screen, filled with white if size is less, no update needed, black  /white / red, for example bitmaps
    void drawExamplePicture(const uint8_t *black_bitmap, const uint8_t *red_bitmap, uint32_t black_size, uint32_t red_size);
    // to full screen, filled with white if size is less, no update needed, black  /white / red, general version
    void drawPicture(const uint8_t *black_bitmap, const uint8_t *red_bitmap, uint32_t black_size, uint32_t red_size, int16_t mode = bm_normal);
    // to full screen, filled with white if size is less, no update needed
    void drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode = bm_normal); // only bm_normal, bm_invert, bm_partial_update modes implemented
    void eraseDisplay(bool using_partial_update = false);
    // partial update of rectangle from buffer to screen, does not power off
    void updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation = true);
    // partial update of rectangle at (xs,ys) from buffer to screen at (xd,yd), does not power off
    void updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation = true);
    // terminate cleanly updateWindow or updateToWindow before removing power or long delays
    void powerDown();
    // paged drawing, for limited RAM, drawCallback() is called GxDEPG0290R_PAGES times
    // each call of drawCallback() should draw the same
    void drawPaged(void (*drawCallback)(void));
    void drawPaged(void (*drawCallback)(uint32_t), uint32_t);
    void drawPaged(void (*drawCallback)(const void *), const void *);
    void drawPaged(void (*drawCallback)(const void *, const void *), const void *, const void *);
    // paged drawing to screen rectangle at (x,y) using partial update
    void drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t);
    void drawPagedToWindow(void (*drawCallback)(const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *);
    void drawPagedToWindow(void (*drawCallback)(const void *, const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *, const void *);
    void drawCornerTest(uint8_t em = 0x01);
private:
    template <typename T> static inline void
    swap(T &a, T &b)
    {
        T t = a;
        a = b;
        b = t;
    }

    void _writeToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation = true);
    uint16_t _setPartialRamArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye);
    void _writeData(uint8_t data);
    void _writeCommand(uint8_t command);
    void _wakeUp();
    void _sleep();
    void _waitWhileBusy(const char *comment = 0);
    void _rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h);
    void _setRamDataEntryMode(uint8_t em);
    void _SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1);
    void _SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1);;

private:
#if defined(__AVR)
    uint8_t _black_buffer[GxDEPG0290R_PAGE_SIZE];
    uint8_t _red_buffer[GxDEPG0290R_PAGE_SIZE];
#else
    uint8_t _black_buffer[GxDEPG0290R_BUFFER_SIZE];
    uint8_t _red_buffer[GxDEPG0290R_BUFFER_SIZE];
#endif
    GxIO &IO;
    int16_t _current_page;
    bool _using_partial_mode;
    bool _diag_enabled;
    int8_t _rst;
    int8_t _busy;
#if defined(ESP8266) || defined(ESP32)
public:
    // the compiler of these packages has a problem with signature matching to base classes
    void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color)
    {
        Adafruit_GFX::drawBitmap(x, y, bitmap, w, h, color);
    };
#endif
};

#ifndef GxEPD_Class
#define GxEPD_Class GxDEPG0290R
#define GxEPD_WIDTH GxDEPG0290R_WIDTH
#define GxEPD_HEIGHT GxDEPG0290R_HEIGHT
#define GxEPD_BitmapExamples <GxDEPG0290R/BitmapExamples.h>
#define GxEPD_BitmapExamplesQ "GxDEPG0290R/BitmapExamples.h"
#define GxEPD_ProductID       "2.9\""
#endif

#endif

