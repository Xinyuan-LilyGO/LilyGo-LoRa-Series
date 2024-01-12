// class GxQYEG0290BN : Display class for GDEH029A1 e-Paper from Dalian Good Display Co., Ltd.: www.good-display.com
//
// based on Demo Example from Good Display, available here: http://www.good-display.com/download_detail/downloadsId=515.html
// Controller: IL3820 : http://www.good-display.com/download_detail/downloadsId=540.html
//
// Author : J-M Zingg
//
// Version : see library.properties
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// Library: https://github.com/ZinggJM/GxEPD

#include "GxQYEG0290BN.h"

//#define DISABLE_DIAGNOSTIC_OUTPUT

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// Partial Update Delay, may have an influence on degradation
#define GxQYEG0290BN_PU_DELAY 300

const uint8_t GxQYEG0290BN::LUTDefault_part[] = {
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00,
    0x00, 0x00, 0x22, 0x17, 0x41, 0xB0, 0x32,
    0x36,
};


GxQYEG0290BN::GxQYEG0290BN(GxIO &io, int8_t rst, int8_t busy) :
    GxEPD(GxQYEG0290BN_WIDTH, GxQYEG0290BN_HEIGHT), IO(io),
    _current_page(-1), _using_partial_mode(false), _diag_enabled(false),
    _rst(rst), _busy(busy)
{
}

void GxQYEG0290BN::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
        swap(x, y);
        x = GxQYEG0290BN_WIDTH - x - 1;
        break;
    case 2:
        x = GxQYEG0290BN_WIDTH - x - 1;
        y = GxQYEG0290BN_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxQYEG0290BN_HEIGHT - y - 1;
        break;
    }
    uint16_t i = x / 8 + y * GxQYEG0290BN_WIDTH / 8;
    if (_current_page < 1) {
        if (i >= sizeof(_buffer)) return;
    } else {
        y -= _current_page * GxQYEG0290BN_PAGE_HEIGHT;
        if ((y < 0) || (y >= GxQYEG0290BN_PAGE_HEIGHT)) return;
        i = x / 8 + y * GxQYEG0290BN_WIDTH / 8;
    }

    if (!color)
        _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
    else
        _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}

void GxQYEG0290BN::init(uint32_t serial_diag_bitrate)
{
    if (serial_diag_bitrate > 0) {
        Serial.begin(serial_diag_bitrate);
        _diag_enabled = true;
    }
    IO.init();
    IO.setFrequency(4000000); // 4MHz
    pinMode(_busy, INPUT);
    if (_rst >= 0) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, LOW);
        delay(10);
        digitalWrite(_rst, HIGH);
        delay(10);
        _waitWhileBusy();
    }
    fillScreen(GxEPD_WHITE);
    _current_page = -1;
    _using_partial_mode = false;
}

void GxQYEG0290BN::fillScreen(uint16_t color)
{
    uint8_t data = (color == GxEPD_BLACK) ? 0xFF : 0x00;
    for (uint16_t x = 0; x < sizeof(_buffer); x++) {
        _buffer[x] = data;
    }
}

void GxQYEG0290BN::update(void)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    _writeCommand(0x24);
    for (uint16_t y = 0; y < GxQYEG0290BN_HEIGHT; y++) {
        for (uint16_t x = 0; x < GxQYEG0290BN_WIDTH / 8; x++) {
            uint16_t idx = y * (GxQYEG0290BN_WIDTH / 8) + x;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Full();
    _PowerOff();
}

void  GxQYEG0290BN::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
    if (mode & bm_default) mode |= bm_flip_x | bm_invert;
    drawBitmapBM(bitmap, x, y, w, h, color, mode);
}

void GxQYEG0290BN::drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode)
{
    if (_current_page != -1) return;
    // example bitmaps are made for y-decrement, x-increment, for origin on opposite corner
    // bm_flip_x for normal display (bm_flip_y would be rotated)
    if (mode & bm_default) mode |= bm_flip_x;
    uint8_t ram_entry_mode = 0x03; // y-increment, x-increment : normal mode
    if ((mode & bm_flip_y) && (mode & bm_flip_x)) ram_entry_mode = 0x00; // y-decrement, x-decrement
    else if (mode & bm_flip_y) ram_entry_mode = 0x01; // y-decrement, x-increment
    else if (mode & bm_flip_x) ram_entry_mode = 0x02; // y-increment, x-decrement
    if (mode & bm_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(ram_entry_mode);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxQYEG0290BN_BUFFER_SIZE; i++) {
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
        delay(GxQYEG0290BN_PU_DELAY);
        // update erase buffer
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxQYEG0290BN_BUFFER_SIZE; i++) {
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
        delay(GxQYEG0290BN_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(ram_entry_mode);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxQYEG0290BN_BUFFER_SIZE; i++) {
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

void GxQYEG0290BN::eraseDisplay(bool using_partial_update)
{
    if (_current_page != -1) return;
    if (using_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(0x01);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxQYEG0290BN_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Part();
        delay(GxQYEG0290BN_PU_DELAY);
        // update erase buffer
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxQYEG0290BN_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        delay(GxQYEG0290BN_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(0x01);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxQYEG0290BN_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Full();
        _PowerOff();
    }
}

void GxQYEG0290BN::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
    if (_current_page != -1) return;
    if (using_rotation) _rotate(x, y, w, h);
    if (x >= GxQYEG0290BN_WIDTH) return;
    if (y >= GxQYEG0290BN_HEIGHT) return;
    uint16_t xe = gx_uint16_min(GxQYEG0290BN_WIDTH, x + w) - 1;
    uint16_t ye = gx_uint16_min(GxQYEG0290BN_HEIGHT, y + h) - 1;
    uint16_t xs_d8 = x / 8;
    uint16_t xe_d8 = xe / 8;
    _Init_Part(0x03);
    _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = y; y1 <= ye; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Part();
    delay(GxQYEG0290BN_PU_DELAY);
    // update erase buffer
    _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = y; y1 <= ye; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    delay(GxQYEG0290BN_PU_DELAY);
}

void GxQYEG0290BN::_writeToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h)
{
    //Serial.printf("_writeToWindow(%d, %d, %d, %d, %d, %d)\n", xs, ys, xd, yd, w, h);
    // the screen limits are the hard limits
    if (xs >= GxQYEG0290BN_WIDTH) return;
    if (ys >= GxQYEG0290BN_HEIGHT) return;
    if (xd >= GxQYEG0290BN_WIDTH) return;
    if (yd >= GxQYEG0290BN_HEIGHT) return;
    w = gx_uint16_min(w, GxQYEG0290BN_WIDTH - xs);
    w = gx_uint16_min(w, GxQYEG0290BN_WIDTH - xd);
    h = gx_uint16_min(h, GxQYEG0290BN_HEIGHT - ys);
    h = gx_uint16_min(h, GxQYEG0290BN_HEIGHT - yd);
    uint16_t xds_d8 = xd / 8;
    uint16_t xde_d8 = (xd + w - 1) / 8;
    uint16_t yde = yd + h - 1;
    // soft limits, must send as many bytes as set by _SetRamArea
    uint16_t xse_d8 = xs / 8 + xde_d8 - xds_d8;
    uint16_t yse = ys + h - 1;
    _SetRamArea(xds_d8, xde_d8, yd % 256, yd / 256, yde % 256, yde / 256); // X-source area,Y-gate area
    _SetRamPointer(xds_d8, yd % 256, yd / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = ys; y1 <= yse; y1++) {
        for (int16_t x1 = xs / 8; x1 <= xse_d8; x1++) {
            uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
}

void GxQYEG0290BN::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            xs = GxQYEG0290BN_WIDTH - xs - w - 1;
            xd = GxQYEG0290BN_WIDTH - xd - w - 1;
            break;
        case 2:
            xs = GxQYEG0290BN_WIDTH - xs - w - 1;
            ys = GxQYEG0290BN_HEIGHT - ys - h - 1;
            xd = GxQYEG0290BN_WIDTH - xd - w - 1;
            yd = GxQYEG0290BN_HEIGHT - yd - h - 1;
            break;
        case 3:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            ys = GxQYEG0290BN_HEIGHT - ys  - h - 1;
            yd = GxQYEG0290BN_HEIGHT - yd  - h - 1;
            break;
        }
    }
    _Init_Part(0x03);
    _writeToWindow(xs, ys, xd, yd, w, h);
    _Update_Part();
    delay(GxQYEG0290BN_PU_DELAY);
    // update erase buffer
    _writeToWindow(xs, ys, xd, yd, w, h);
    delay(GxQYEG0290BN_PU_DELAY);
}

void GxQYEG0290BN::powerDown()
{
    _using_partial_mode = false;
    _PowerOff();
}

void GxQYEG0290BN::_writeCommand(uint8_t command)
{
    if (digitalRead(_busy)) {
        String str = String("command 0x") + String(command, HEX);
        _waitWhileBusy(str.c_str());
    }
    IO.writeCommandTransaction(command);
}

void GxQYEG0290BN::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxQYEG0290BN::_writeCommandData(const uint8_t *pCommandData, uint8_t datalen)
{
    if (digitalRead(_busy)) {
        String str = String("command 0x") + String(pCommandData[0], HEX);
        _waitWhileBusy(str.c_str());
    }
    IO.startTransaction();
    IO.writeCommand(*pCommandData++);
    for (uint8_t i = 0; i < datalen - 1; i++) { // sub the command
        IO.writeData(*pCommandData++);
    }
    IO.endTransaction();

}

void GxQYEG0290BN::_waitWhileBusy(const char *comment)
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

void GxQYEG0290BN::_setRamDataEntryMode(uint8_t em)
{
    const uint16_t xPixelsPar = GxQYEG0290BN_X_PIXELS - 1;
    const uint16_t yPixelsPar = GxQYEG0290BN_Y_PIXELS - 1;
    em = gx_uint16_min(em, 0x03);
    _writeCommand(0x11);
    _writeData(em);
    switch (em) {
    case 0x00: // x decrease, y decrease
        _SetRamArea(xPixelsPar / 8, 0x00, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
        _SetRamPointer(xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256); // set ram
        break;
    case 0x01: // x increase, y decrease : as in demo code
        _SetRamArea(0x00, xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
        _SetRamPointer(0x00, yPixelsPar % 256, yPixelsPar / 256); // set ram
        break;
    case 0x02: // x decrease, y increase
        _SetRamArea(xPixelsPar / 8, 0x00, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
        _SetRamPointer(xPixelsPar / 8, 0x00, 0x00); // set ram
        break;
    case 0x03: // x increase, y increase : normal mode
        _SetRamArea(0x00, xPixelsPar / 8, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
        _SetRamPointer(0x00, 0x00, 0x00); // set ram
        break;
    }
}

void GxQYEG0290BN::_SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    _writeCommand(0x44);
    _writeData(Xstart + 1);
    _writeData(Xend + 1);
    _writeCommand(0x45);
    _writeData(Ystart);
    _writeData(Ystart1);
    _writeData(Yend);
    _writeData(Yend1);
}

void GxQYEG0290BN::_SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
    _writeCommand(0x4e);
    _writeData(addrX + 1 );
    _writeCommand(0x4f);
    _writeData(addrY);
    _writeData(addrY1);
}

void GxQYEG0290BN::_PowerOn(void)
{
}

void GxQYEG0290BN::_PowerOff(void)
{
}

void GxQYEG0290BN::_InitDisplay(uint8_t em)
{
    _writeData(0x12);
    _waitWhileBusy("_InitDisplay");
    _setRamDataEntryMode(em);
}

void GxQYEG0290BN::_Init_Full(uint8_t em)
{
    _InitDisplay(em);
    _PowerOn();
}

void GxQYEG0290BN::_Init_Part(uint8_t em)
{
    _InitDisplay(em);
    // _writeCommandData()
    _PowerOn();
}

void GxQYEG0290BN::_Update_Full(void)
{
    _writeCommand(0x20);
    _waitWhileBusy("_Update_Full");
}

void GxQYEG0290BN::_Update_Part(void)
{
    _writeCommand(0x20);
    _waitWhileBusy("_Update_Part");
}

void GxQYEG0290BN::drawPaged(void (*drawCallback)(void))
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback();
        for (int16_t y1 = 0; y1 < GxQYEG0290BN_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxQYEG0290BN_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxQYEG0290BN::drawPaged(void (*drawCallback)(uint32_t), uint32_t p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxQYEG0290BN_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxQYEG0290BN_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxQYEG0290BN::drawPaged(void (*drawCallback)(const void *), const void *p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxQYEG0290BN_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxQYEG0290BN_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxQYEG0290BN::drawPaged(void (*drawCallback)(const void *, const void *), const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x03);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p1, p2);
        for (int16_t y1 = 0; y1 < GxQYEG0290BN_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxQYEG0290BN_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxQYEG0290BN_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxQYEG0290BN::_rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
{
    switch (getRotation()) {
    case 1:
        swap(x, y);
        swap(w, h);
        x = GxQYEG0290BN_WIDTH - x - w - 1;
        break;
    case 2:
        x = GxQYEG0290BN_WIDTH - x - w - 1;
        y = GxQYEG0290BN_HEIGHT - y - h - 1;
        break;
    case 3:
        swap(x, y);
        swap(w, h);
        y = GxQYEG0290BN_HEIGHT - y - h - 1;
        break;
    }
}

void GxQYEG0290BN::drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxQYEG0290BN_PU_DELAY);
    // update erase buffer
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    delay(GxQYEG0290BN_PU_DELAY);
    _current_page = -1;
    _PowerOff();
}

void GxQYEG0290BN::drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxQYEG0290BN_PU_DELAY);
    // update erase buffer
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    delay(GxQYEG0290BN_PU_DELAY);
    _current_page = -1;
    _PowerOff();
}

void GxQYEG0290BN::drawPagedToWindow(void (*drawCallback)(const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxQYEG0290BN_PU_DELAY);
    // update erase buffer
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    delay(GxQYEG0290BN_PU_DELAY);
    _current_page = -1;
    _PowerOff();
}

void GxQYEG0290BN::drawPagedToWindow(void (*drawCallback)(const void *, const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x03);
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxQYEG0290BN_PU_DELAY);
    // update erase buffer
    for (_current_page = 0; _current_page < GxQYEG0290BN_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxQYEG0290BN_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxQYEG0290BN_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = yds % GxQYEG0290BN_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    delay(GxQYEG0290BN_PU_DELAY);
    _current_page = -1;
    _PowerOff();
}

void GxQYEG0290BN::drawCornerTest(uint8_t em)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(em);
    _writeCommand(0x24);
    for (uint32_t y = 0; y < GxQYEG0290BN_HEIGHT; y++) {
        for (uint32_t x = 0; x < GxQYEG0290BN_WIDTH / 8; x++) {
            uint8_t data = 0xFF;
            if ((x < 1) && (y < 8)) data = 0x00;
            if ((x > GxQYEG0290BN_WIDTH / 8 - 3) && (y < 16)) data = 0x00;
            if ((x > GxQYEG0290BN_WIDTH / 8 - 4) && (y > GxQYEG0290BN_HEIGHT - 25)) data = 0x00;
            if ((x < 4) && (y > GxQYEG0290BN_HEIGHT - 33)) data = 0x00;
            _writeData(data);
        }
    }
    _Update_Full();
    _PowerOff();
}

