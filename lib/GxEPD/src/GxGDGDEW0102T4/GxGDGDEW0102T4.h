// class GxGDGDEW0102T4 : Display class for GDEW0102T4 e-Paper from Dalian Good Display Co., Ltd.: https://www.good-display.cn/product/207.html
//
// based on Demo Example from Good Display, available here: https://www.good-display.cn/product/207.html
// Controller: UC8175

#ifndef _GxGDGDEW0102T4_H_
#define _GxGDGDEW0102T4_H_

#include "../GxEPD.h"

#define GxGDGDEW0102T4_WIDTH 80
#define GxGDGDEW0102T4_HEIGHT 128

#define GxGDGDEW0102T4_BUFFER_SIZE (uint32_t(GxGDGDEW0102T4_WIDTH) * uint32_t(GxGDGDEW0102T4_HEIGHT) / 8)

// divisor for AVR, should be factor of GxGDGDEW0102T4_HEIGHT
#define GxGDGDEW0102T4_PAGES 4

#define GxGDGDEW0102T4_PAGE_HEIGHT (GxGDGDEW0102T4_HEIGHT / GxGDGDEW0102T4_PAGES)
#define GxGDGDEW0102T4_PAGE_SIZE (GxGDGDEW0102T4_BUFFER_SIZE / GxGDGDEW0102T4_PAGES)

class GxGDGDEW0102T4 : public GxEPD
{
public:
#if defined(ESP8266)
    //GxGDGDEW0102T4(GxIO& io, int8_t rst = D4, int8_t busy = D2);
    // use pin numbers, other ESP8266 than Wemos may not use Dx names
    GxGDGDEW0102T4(GxIO &io, int8_t rst = 2, int8_t busy = 4);
#else
    GxGDGDEW0102T4(GxIO &io, int8_t rst = 9, int8_t busy = 7);
#endif
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void init(uint32_t serial_diag_bitrate = 0); // = 0 : disabled
    void fillScreen(uint16_t color); // to buffer
    void update(void);
    // to buffer, may be cropped, drawPixel() used, update needed
    void  drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode = bm_normal);
    // to full screen, filled with white if size is less, no update needed
    void drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode = bm_normal); // only bm_normal, bm_invert, bm_partial_update modes implemented
    void eraseDisplay(bool using_partial_update = false);
    // partial update of rectangle from buffer to screen, does not power off
    void updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation = true);
    // partial update of rectangle at (xs,ys) from buffer to screen at (xd,yd), does not power off
    void updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation = true);
    // terminate cleanly updateWindow or updateToWindow before removing power or long delays
    void powerDown();
    // paged drawing, for limited RAM, drawCallback() is called GxGDGDEW0102T4_PAGES times
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
    uint16_t _setPartialRamArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye);
    void _writeData(uint8_t data);
    void _writeCommand(uint8_t command);
    void _wakeUp();
    void _sleep();
    void _Init_FullUpdate();
    void _Init_PartialUpdate();
    void _waitWhileBusy(const char *comment = 0);
    void _rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h);
private:
#if defined(__AVR)
    uint8_t _buffer[GxGDGDEW0102T4_PAGE_SIZE];
#else
    uint8_t _buffer[GxGDGDEW0102T4_BUFFER_SIZE];
#endif
    GxIO &IO;
    int16_t _current_page;
    bool _using_partial_mode;
    bool _diag_enabled;
    int8_t _rst;
    int8_t _busy;
    static const unsigned char lut_20_vcomDC[];
    static const unsigned char lut_21_ww[];
    static const unsigned char lut_22_bw[];
    static const unsigned char lut_23_wb[];
    static const unsigned char lut_24_bb[];
    static const unsigned char lut_20_vcomDC_partial[];
    static const unsigned char lut_21_ww_partial[];
    static const unsigned char lut_22_bw_partial[];
    static const unsigned char lut_23_wb_partial[];
    static const unsigned char lut_24_bb_partial[];
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
#define GxEPD_Class GxGDGDEW0102T4
#define GxEPD_WIDTH GxGDGDEW0102T4_WIDTH
#define GxEPD_HEIGHT GxGDGDEW0102T4_HEIGHT
#define GxEPD_BitmapExamples <GxGDGDEW0102T4/BitmapExamples.h>
#define GxEPD_BitmapExamplesQ "GxGDGDEW0102T4/BitmapExamples.h"
#define GxEPD_ProductID       "1.02\""
#endif

#endif
