// class GxGDEW0102I4F : Display class for e-Paper from Dalian Good Display Co., Ltd.: www.good-display.com
//
// based on Demo Example from Good Display, available here: http://www.e-paper-display.cn/products_detail/productId=424.html
// Controller: IL0323 : http://www.e-paper-display.cn/products_detail/productId=424.html
//
// Author : Lewis He
//
// Version : see library.properties
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// Library: https://github.com/lewisxhe/GxEPD

#ifndef _GxGDEW0102I4F_H_
#define _GxGDEW0102I4F_H_

#include "../GxEPD.h"

// the physical number of pixels (for controller parameter)
#define GxGDEW0102I4F_X_PIXELS 80
#define GxGDEW0102I4F_Y_PIXELS 128

// the logical width and height of the display
#define GxGDEW0102I4F_WIDTH GxGDEW0102I4F_X_PIXELS
#define GxGDEW0102I4F_HEIGHT GxGDEW0102I4F_Y_PIXELS

#define GxGDEW0102I4F_BUFFER_SIZE (uint32_t(GxGDEW0102I4F_WIDTH) * uint32_t(GxGDEW0102I4F_HEIGHT) / 8)

// divisor for AVR, should be factor of GxGDEW0102I4F_HEIGHT
#define GxGDEW0102I4F_PAGES 5

#define GxGDEW0102I4F_PAGE_HEIGHT (GxGDEW0102I4F_HEIGHT / GxGDEW0102I4F_PAGES)
#define GxGDEW0102I4F_PAGE_SIZE (GxGDEW0102I4F_BUFFER_SIZE / GxGDEW0102I4F_PAGES)

class GxGDEW0102I4F : public GxEPD
{
  public:
#if defined(ESP8266)
    // use pin numbers, other ESP8266 than Wemos may not use Dx names
    GxGDEW0102I4F(GxIO& io, int8_t rst = 2, int8_t busy = 4);
#else
    GxGDEW0102I4F(GxIO& io, int8_t rst = 9, int8_t busy = 7);
#endif
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void init(uint32_t serial_diag_bitrate = 0); // = 0 : disabled
    void fillScreen(uint16_t color); // 0x0 black, >0x0 white, to buffer
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
    // paged drawing, for limited RAM, drawCallback() is called GxGDEW0102I4F_PAGES times
    // each call of drawCallback() should draw the same
    void drawPaged(void (*drawCallback)(void));
    void drawPaged(void (*drawCallback)(uint32_t), uint32_t);
    void drawPaged(void (*drawCallback)(const void*), const void*);
    void drawPaged(void (*drawCallback)(const void*, const void*), const void*, const void*);
    // paged drawing to screen rectangle at (x,y) using partial update
    void drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t);
    void drawPagedToWindow(void (*drawCallback)(const void*), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void*);
    void drawPagedToWindow(void (*drawCallback)(const void*, const void*), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void*, const void*);
    void drawCornerTest(uint8_t em = 0x01);
  private:
    template <typename T> static inline void
    swap(T& a, T& b)
    {
      T t = a;
      a = b;
      b = t;
    }
    void _writeToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h);
    void _writeData(uint8_t data);
    void _writeCommand(uint8_t command);
    void _writeCommandData(const uint8_t* pCommandData, uint8_t datalen);
    void _SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1);
    void _SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1);
    void _PowerOff(void);
    void _waitWhileBusy(const char* comment=0);
    void _Init_Full(uint8_t em);
    void _Init_Part(uint8_t em);
    void _Update_Full(void);
    void _Update_Part(void);
    void _rotate(uint16_t& x, uint16_t& y, uint16_t& w, uint16_t& h);
    void _reset(void);
  protected:
#if defined(__AVR)
    uint8_t _buffer[GxGDEW0102I4F_PAGE_SIZE];
#else
    uint8_t _buffer[GxGDEW0102I4F_BUFFER_SIZE];
#endif
  private:
    GxIO& IO;
    int16_t _current_page;
    bool _using_partial_mode;
    bool _diag_enabled;
    int8_t _rst;
    int8_t _busy;
    static const uint8_t LUTDefault_full_w[];
    static const uint8_t LUTDefault_full_b[];
    static const uint8_t LUTDefault_part_w[];
    static const uint8_t LUTDefault_part_b[];
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
#define GxEPD_Class GxGDEW0102I4F
#define GxEPD_WIDTH GxGDEW0102I4F_WIDTH
#define GxEPD_HEIGHT GxGDEW0102I4F_HEIGHT
#define GxEPD_BitmapExamples <GxGDEW0102I4F/BitmapExamples.h>
#define GxEPD_BitmapExamplesQ "GxGDEW0102I4F/BitmapExamples.h"
#endif

#endif

