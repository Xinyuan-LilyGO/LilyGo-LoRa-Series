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

#include "GxGDEW0102I4F.h"

//#define DISABLE_DIAGNOSTIC_OUTPUT

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// Partial Update Delay, may have an influence on degradation
#define GxGDEW0102I4F_PU_DELAY 300

const uint8_t GxGDEW0102I4F::LUTDefault_full_w[] = {
    0x23,   //command
    0x60, 0x5A, 0x5A, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t GxGDEW0102I4F::LUTDefault_full_b[] = {
    0x24,   //command
    0x90, 0x5A, 0x5A, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t GxGDEW0102I4F::LUTDefault_part_w[] = {
    0x23,   //command
    0x60, 0x01, 0x01, 0x00, 0x00, 0x01,
    0x80, 0x0f, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t GxGDEW0102I4F::LUTDefault_part_b[] = {
    0x24,   //command
    0x90, 0x01, 0x01, 0x00, 0x00, 0x01,
    0x40, 0x0f, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

GxGDEW0102I4F::GxGDEW0102I4F(GxIO &io, int8_t rst, int8_t busy) :
    GxEPD(GxGDEW0102I4F_WIDTH, GxGDEW0102I4F_HEIGHT), IO(io),
    _current_page(-1), _using_partial_mode(false), _diag_enabled(false),
    _rst(rst), _busy(busy)
{
}

void GxGDEW0102I4F::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;
    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
        swap(x, y);
        x = GxGDEW0102I4F_WIDTH - x - 1;
        break;
    case 2:
        x = GxGDEW0102I4F_WIDTH - x - 1;
        y = GxGDEW0102I4F_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxGDEW0102I4F_HEIGHT - y - 1;
        break;
    }
    // flip y for y-decrement mode
    y = GxGDEW0102I4F_HEIGHT - y - 1;
    uint16_t i = x / 8 + y * GxGDEW0102I4F_WIDTH / 8;
    if (_current_page < 1) {
        if (i >= sizeof(_buffer)) return;
    } else {
        y -= _current_page * GxGDEW0102I4F_PAGE_HEIGHT;
        if ((y < 0) || (y >= GxGDEW0102I4F_PAGE_HEIGHT)) return;
        i = x / 8 + y * GxGDEW0102I4F_WIDTH / 8;
    }

    if (!color)
        _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
    else
        _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}

void GxGDEW0102I4F::init(uint32_t serial_diag_bitrate)
{
    if (serial_diag_bitrate > 0) {
        Serial.begin(serial_diag_bitrate);
        _diag_enabled = true;
    }
    IO.init();
    IO.setFrequency(4000000); // 4MHz
    if (_rst >= 0) {
        digitalWrite(_rst, HIGH);
        pinMode(_rst, OUTPUT);
    }
    pinMode(_busy, INPUT);
    fillScreen(GxEPD_WHITE);
    _current_page = -1;
    _using_partial_mode = false;
}

void GxGDEW0102I4F::fillScreen(uint16_t color)
{
    uint8_t data = (color == GxEPD_BLACK) ? 0xFF : 0x00;
    for (uint16_t x = 0; x < sizeof(_buffer); x++) {
        _buffer[x] = data;
    }
}

void GxGDEW0102I4F::update(void)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x13);
    for (uint16_t y = 0; y < GxGDEW0102I4F_HEIGHT; y++) {
        for (uint16_t x = 0; x < GxGDEW0102I4F_WIDTH / 8; x++) {
            uint16_t idx = y * (GxGDEW0102I4F_WIDTH / 8) + x;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Full();
    _PowerOff();
}

void  GxGDEW0102I4F::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
    if (mode & bm_default) mode |= bm_flip_x | bm_invert;
    drawBitmapBM(bitmap, x, y, w, h, color, mode);
}

void GxGDEW0102I4F::drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode)
{
    if (_current_page != -1) return;
    // example bitmaps are made for y-decrement, x-increment, for origin on opposite corner
    // bm_flip_x for normal display (bm_flip_y would be rotated)
    // y-increment is not available with this controller
    if (mode & bm_default) mode |= bm_flip_x;
    uint8_t ram_entry_mode = 0x01; // (always) y-decrement, x-increment for normal mode
    if ((mode & bm_flip_y) && (mode & bm_flip_x)) ram_entry_mode = 0x00; // y-decrement, x-decrement
    else if (mode & bm_flip_y) ram_entry_mode = 0x00; // (always) y-decrement, x-decrement
    //else if (mode & bm_flip_x) ram_entry_mode = 0x02; // (always) y-decrement
    if (mode & bm_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(ram_entry_mode);
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDEW0102I4F_BUFFER_SIZE; i++) {
            uint8_t data = 0xFF; // white is 0xFF on device
            if (i < size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                data = pgm_read_byte(&bitmap[i]);
#else
                data = bitmap[i];
#endif
                if (mode & bm_invert) data = ~data;
            }
            _writeData(data);
        }
        _Update_Part();
        delay(GxGDEW0102I4F_PU_DELAY);
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDEW0102I4F_BUFFER_SIZE; i++) {
            uint8_t data = 0xFF; // white is 0xFF on device
            if (i < size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                data = pgm_read_byte(&bitmap[i]);
#else
                data = bitmap[i];
#endif
                if (mode & bm_invert) data = ~data;
            }
            _writeData(data);
        }
        delay(GxGDEW0102I4F_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(ram_entry_mode);
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDEW0102I4F_BUFFER_SIZE; i++) {
            uint8_t data = 0xFF; // white is 0xFF on device
            if (i < size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
                data = pgm_read_byte(&bitmap[i]);
#else
                data = bitmap[i];
#endif
                if (mode & bm_invert) data = ~data;
            }
            _writeData(data);
        }
        _Update_Full();
        _PowerOff();
    }
}

void GxGDEW0102I4F::eraseDisplay(bool using_partial_update)
{
    if (_current_page != -1) return;
    if (using_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(0x01);
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDEW0102I4F_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Part();
        delay(GxGDEW0102I4F_PU_DELAY);
        // update erase buffer
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDEW0102I4F_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        delay(GxGDEW0102I4F_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(0x01);
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDEW0102I4F_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Full();
        _PowerOff();
    }
}

void GxGDEW0102I4F::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
    if (_current_page != -1) return;
    if (using_rotation) _rotate(x, y, w, h);
    if (x >= GxGDEW0102I4F_WIDTH) return;
    if (y >= GxGDEW0102I4F_HEIGHT) return;
    uint16_t xe = gx_uint16_min(GxGDEW0102I4F_WIDTH, x + w) - 1;
    uint16_t ye = gx_uint16_min(GxGDEW0102I4F_HEIGHT, y + h) - 1;
    uint16_t xs_d8 = x / 8;
    uint16_t xe_d8 = xe / 8;
    uint16_t ys_bx = GxGDEW0102I4F_HEIGHT - ye - 1;
    uint16_t ye_bx = GxGDEW0102I4F_HEIGHT - y - 1;
    _Init_Part(0x01);
    _writeCommand(0x91);
    _SetRamArea(xs_d8, xe_d8, ye % 256, ye / 256, y % 256, y / 256); // X-source area,Y-gate area
    // _SetRamPointer(xs_d8, ye % 256, ye / 256); // set ram
    _writeCommand(0x10);
    for (int16_t y1 = ys_bx; y1 <= ye_bx; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    // update erase buffer
    _writeCommand(0x13);
    for (int16_t y1 = ys_bx; y1 <= ye_bx; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    delay(GxGDEW0102I4F_PU_DELAY);
    _writeCommand(0x12);
    _waitWhileBusy();
}

void GxGDEW0102I4F::_writeToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h)
{
    //Serial.printf("_writeToWindow(%d, %d, %d, %d, %d, %d)\n", xs, ys, xd, yd, w, h);
    // the screen limits are the hard limits
    if (xs >= GxGDEW0102I4F_WIDTH) return;
    if (ys >= GxGDEW0102I4F_HEIGHT) return;
    if (xd >= GxGDEW0102I4F_WIDTH) return;
    if (yd >= GxGDEW0102I4F_HEIGHT) return;
    // flip y for y-decrement mode
    ys = GxGDEW0102I4F_HEIGHT - ys - h;
    w = gx_uint16_min(w, GxGDEW0102I4F_WIDTH - xs);
    w = gx_uint16_min(w, GxGDEW0102I4F_WIDTH - xd);
    h = gx_uint16_min(h, GxGDEW0102I4F_HEIGHT - ys);
    h = gx_uint16_min(h, GxGDEW0102I4F_HEIGHT - yd);
    uint16_t xds_d8 = xd / 8;
    uint16_t xde_d8 = (xd + w - 1) / 8;
    uint16_t yde = yd + h - 1;
    // soft limits, must send as many bytes as set by _SetRamArea
    uint16_t xse_d8 = xs / 8 + xde_d8 - xds_d8;
    uint16_t yse = ys + h - 1;
    _SetRamArea(xds_d8, xde_d8, yde % 256, yde / 256, yd % 256, yd / 256); // X-source area,Y-gate area
    _SetRamPointer(xds_d8, yde % 256, yde / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x13);
    for (int16_t y1 = ys; y1 <= yse; y1++) {
        for (int16_t x1 = xs / 8; x1 <= xse_d8; x1++) {
            uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
}

void GxGDEW0102I4F::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{

    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            xs = GxGDEW0102I4F_WIDTH - xs - w - 1;
            xd = GxGDEW0102I4F_WIDTH - xd - w - 1;
            break;
        case 2:
            xs = GxGDEW0102I4F_WIDTH - xs - w - 1;
            ys = GxGDEW0102I4F_HEIGHT - ys - h - 1;
            xd = GxGDEW0102I4F_WIDTH - xd - w - 1;
            yd = GxGDEW0102I4F_HEIGHT - yd - h - 1;
            break;
        case 3:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            ys = GxGDEW0102I4F_HEIGHT - ys  - h - 1;
            yd = GxGDEW0102I4F_HEIGHT - yd  - h - 1;
            break;
        }
    }
    _Init_Part(0x01);
    _writeToWindow(xs, ys, xd, yd, w, h);
    _Update_Part();
    delay(GxGDEW0102I4F_PU_DELAY);
    // update erase buffer
    _writeToWindow(xs, ys, xd, yd, w, h);
    delay(GxGDEW0102I4F_PU_DELAY);
}

void GxGDEW0102I4F::powerDown()
{
    _using_partial_mode = false;
    _PowerOff();
}

void GxGDEW0102I4F::_writeCommand(uint8_t command)
{
    IO.writeCommandTransaction(command);
}

void GxGDEW0102I4F::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxGDEW0102I4F::_writeCommandData(const uint8_t *pCommandData, uint8_t datalen)
{
    IO.startTransaction();
    IO.writeCommand(*pCommandData++);
    for (uint8_t i = 0; i < datalen - 1; i++) { // sub the command
        IO.writeData(*pCommandData++);
    }
    IO.endTransaction();
}

void GxGDEW0102I4F::_waitWhileBusy(const char *comment)
{
    unsigned long start = micros();
    while (1) {
        if (!digitalRead(_busy)) break;
        delay(1);
        if (micros() - start > 10000000) {
            if (_diag_enabled) Serial.println("Busy Timeout!");
            break;
        }
    }
    if (comment) {
#if !defined(DISABLE_DIAGNOSTIC_OUTPUT)
        if (_diag_enabled) {
            unsigned long elapsed = micros() - start;
            Serial.print(comment);
            Serial.print(" : ");
            Serial.println(elapsed);
        }
#endif
    }
    (void) start;
}


void GxGDEW0102I4F::_SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    _writeCommand(0x90);
    _writeData(Xstart);
    _writeData(Xend);
    _writeData(Ystart);
    _writeData(Yend);
    _writeData(0);
}

void GxGDEW0102I4F::_SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
}

void GxGDEW0102I4F::_PowerOff(void)
{
}

void GxGDEW0102I4F::_reset(void)
{
    digitalWrite(_rst, 0);
    delay(10);
    digitalWrite(_rst, 1);
    delay(10);
}

void GxGDEW0102I4F::_Init_Full(uint8_t em)
{
    // reset required for wakeup
    _reset();
    _writeCommand(0xD2);
    _writeData(0x3F);

    _writeCommand(0x00);
    _writeData (0x67);

    _writeCommand(0x01);
    _writeData (0x03);
    _writeData (0x00);
    _writeData (0x2b);
    _writeData (0x2b);

    _writeCommand(0x06);
    _writeData(0x3f);

    _writeCommand(0x2A);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x30);
    _writeData(0x13);

    _writeCommand(0x50);
    _writeData(0x57);

    _writeCommand(0x60);
    _writeData(0x22);

    _writeCommand(0x61);
    _writeData (0x50);
    _writeData (0x80);

    _writeCommand(0x82);
    _writeData(0x12);

    _writeCommand(0xe3);
    _writeData(0x33);

    _writeCommandData(LUTDefault_full_w, sizeof(LUTDefault_full_w));
    _writeCommandData(LUTDefault_full_b, sizeof(LUTDefault_full_b));
}

void GxGDEW0102I4F::_Init_Part(uint8_t em)
{
    _reset();

    _writeCommand(0xD2);
    _writeData(0x3F);

    _writeCommand(0x00);
    _writeData (0x67);  //LUT from registers.
    //old= 0b0110 0111

    _writeCommand(0x01);
    _writeData (0x03);
    _writeData (0x00);
    _writeData (0x2b);
    _writeData (0x2b);

    _writeCommand(0x06);
    _writeData(0x3f);

    _writeCommand(0x2A);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x30);// The command controls the clock frequency.
    _writeData(0x05);   // 15.0 Hz

    _writeCommand(0x50); //This command indicates the interval of VCOM and data output. When setting the vertical back porch, the total blanking will bekept (10 Hsync)
    _writeData(0xF2);

    _writeCommand(0x60);    //This command defines non-overlap period of Gate and Source.
    _writeData(0x22);

    _writeCommand(0x82); //This command sets VCOM_DC value.
    _writeData(0x00);
    /*
    * This command is set for saving power during refreshing period. If the output voltage of VCOM / Source is from negative to
    * positive or from positive to negative, the power saving mechanism will be activated. The active period width is defined by the
    * following two parameters.
    * */
    _writeCommand(0xe3);
    _writeData(0x33);

    _writeCommandData(LUTDefault_part_w, sizeof(LUTDefault_part_w));
    _writeCommandData(LUTDefault_part_b, sizeof(LUTDefault_part_b));
}

void GxGDEW0102I4F::_Update_Full(void)
{
    _writeCommand(0x04);
    _waitWhileBusy("_Update_Full");
    _writeCommand(0x12);
    _waitWhileBusy("_Update_Full");
    _writeCommand(0x02);
    _waitWhileBusy("_Update_Full");
}

void GxGDEW0102I4F::_Update_Part(void)
{
    _writeCommand(0x04);
    _waitWhileBusy("_Update_Part");
    _writeCommand(0x12);
    _waitWhileBusy("_Update_Part");
    _writeCommand(0x02);
    _waitWhileBusy("_Update_Part");
}

void GxGDEW0102I4F::drawPaged(void (*drawCallback)(void))
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x13);
    for (_current_page = 0; _current_page < GxGDEW0102I4F_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback();
        for (int16_t y1 = 0; y1 < GxGDEW0102I4F_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDEW0102I4F_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDEW0102I4F::drawPaged(void (*drawCallback)(uint32_t), uint32_t p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x13);
    for (_current_page = 0; _current_page < GxGDEW0102I4F_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxGDEW0102I4F_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDEW0102I4F_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDEW0102I4F::drawPaged(void (*drawCallback)(const void *), const void *p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x13);
    for (_current_page = 0; _current_page < GxGDEW0102I4F_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxGDEW0102I4F_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDEW0102I4F_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDEW0102I4F::drawPaged(void (*drawCallback)(const void *, const void *), const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x13);
    for (_current_page = 0; _current_page < GxGDEW0102I4F_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p1, p2);
        for (int16_t y1 = 0; y1 < GxGDEW0102I4F_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDEW0102I4F_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDEW0102I4F_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDEW0102I4F::_rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
{
    switch (getRotation()) {
    case 1:
        swap(x, y);
        swap(w, h);
        x = GxGDEW0102I4F_WIDTH - x - w - 1;
        break;
    case 2:
        x = GxGDEW0102I4F_WIDTH - x - w - 1;
        y = GxGDEW0102I4F_HEIGHT - y - h - 1;
        break;
    case 3:
        swap(x, y);
        swap(w, h);
        y = GxGDEW0102I4F_HEIGHT - y - h - 1;
        break;
    }
}

void GxGDEW0102I4F::drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDEW0102I4F_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDEW0102I4F::drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDEW0102I4F_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDEW0102I4F::drawPagedToWindow(void (*drawCallback)(const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDEW0102I4F_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDEW0102I4F::drawPagedToWindow(void (*drawCallback)(const void *, const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDEW0102I4F_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDEW0102I4F_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDEW0102I4F_PAGES - _current_page - 1) * GxGDEW0102I4F_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDEW0102I4F_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = (GxGDEW0102I4F_PAGES - 1) * GxGDEW0102I4F_PAGE_HEIGHT + (yds % GxGDEW0102I4F_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDEW0102I4F::drawCornerTest(uint8_t em)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(em);
    _writeCommand(0x13);
    for (uint32_t y = 0; y < GxGDEW0102I4F_HEIGHT; y++) {
        for (uint32_t x = 0; x < GxGDEW0102I4F_WIDTH / 8; x++) {
            uint8_t data = 0xFF;
            if ((x < 1) && (y < 8)) data = 0x00;
            if ((x > GxGDEW0102I4F_WIDTH / 8 - 3) && (y < 16)) data = 0x00;
            if ((x > GxGDEW0102I4F_WIDTH / 8 - 4) && (y > GxGDEW0102I4F_HEIGHT - 25)) data = 0x00;
            if ((x < 4) && (y > GxGDEW0102I4F_HEIGHT - 33)) data = 0x00;
            _writeData(data);
        }
    }
    _Update_Full();
    _PowerOff();
}

