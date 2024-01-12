// class GxGDE0213B72B : Display class for GDE0213B1 e-Paper from Dalian Good Display Co., Ltd.: www.good-display.com
//
// based on Demo Example from Good Display, available here: http://www.good-display.com/download_detail/downloadsId=515.html
// Controller: IL3895 : http://www.good-display.com/download_detail/downloadsId=538.html
//
// Author : J-M Zingg
//
// Version : see library.properties
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// Library: https://github.com/ZinggJM/GxEPD

#include "GxGDE0213B72B.h"

//#define DISABLE_DIAGNOSTIC_OUTPUT

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// Partial Update Delay, may have an influence on degradation
#define GxGDE0213B72B_PU_DELAY 300



const uint8_t GxGDE0213B72B::LUTDefault_full[] = {
    0x32,  // command
    0xA0,   0x90,   0x50,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x50,   0x90,   0xA0,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0xA0,   0x90,   0x50,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x50,   0x90,   0xA0,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,

    0x0F,   0x0F,   0x00,   0x00,   0x00,
    0x0F,   0x0F,   0x00,   0x00,   0x03,
    0x0F,   0x0F,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,

    0x17,   0x41,   0xA8,   0x32,   0x50, 0x0A, 0x09,

};

const uint8_t GxGDE0213B72B::LUTDefault_part[] = {
    0x32,  // command
    0x40,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x80,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x40,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x80,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,

    0x0A,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,
    0x15,   0x41,   0xA8,   0x32,   0x50,   0x2C, 0x0B,
};

const uint8_t GxGDE0213B72B::GDOControl[] = {0x01, (GxGDE0213B72B_Y_PIXELS - 1) % 256, (GxGDE0213B72B_Y_PIXELS - 1) / 256, 0x00}; //for 2.13inch
const uint8_t GxGDE0213B72B::softstart[] = {0x0c, 0xd7, 0xd6, 0x9d};
const uint8_t GxGDE0213B72B::VCOMVol[] = {0x2c, 0xa8};  // VCOM 7c
const uint8_t GxGDE0213B72B::DummyLine[] = {0x3a, 0x1a}; // 4 dummy line per gate
const uint8_t GxGDE0213B72B::Gatetime[] = {0x3b, 0x08};  // 2us per line

GxGDE0213B72B::GxGDE0213B72B(GxIO &io, int8_t rst, int8_t busy) :
    GxEPD(GxGDE0213B72B_VISIBLE_WIDTH, GxGDE0213B72B_HEIGHT), IO(io),
    _current_page(-1), _using_partial_mode(false), _diag_enabled(false),
    _rst(rst), _busy(busy)
{
}

void GxGDE0213B72B::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
        swap(x, y);
        x = GxGDE0213B72B_VISIBLE_WIDTH - x - 1;
        break;
    case 2:
        x = GxGDE0213B72B_VISIBLE_WIDTH - x - 1;
        y = GxGDE0213B72B_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxGDE0213B72B_HEIGHT - y - 1;
        break;
    }
    // flip y for y-decrement mode
    y = GxGDE0213B72B_HEIGHT - y - 1;
    uint16_t i = x / 8 + y * GxGDE0213B72B_WIDTH / 8;
    if (_current_page < 1) {
        if (i >= sizeof(_buffer)) return;
    } else {
        y -= _current_page * GxGDE0213B72B_PAGE_HEIGHT;
        if ((y < 0) || (y >= GxGDE0213B72B_PAGE_HEIGHT)) return;
        i = x / 8 + y * GxGDE0213B72B_WIDTH / 8;
    }

    if (!color)
        _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
    else
        _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}

void GxGDE0213B72B::init(uint32_t serial_diag_bitrate)
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

void GxGDE0213B72B::fillScreen(uint16_t color)
{
    uint8_t data = (color == GxEPD_BLACK) ? 0xFF : 0x00;
    for (uint16_t x = 0; x < sizeof(_buffer); x++) {
        _buffer[x] = data;
    }
}

void GxGDE0213B72B::update(void)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x24);
    for (uint16_t y = 0; y < GxGDE0213B72B_HEIGHT; y++) {
        for (uint16_t x = 0; x < GxGDE0213B72B_WIDTH / 8; x++) {
            uint16_t idx = y * (GxGDE0213B72B_WIDTH / 8) + x;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Full();
    _PowerOff();
}

void  GxGDE0213B72B::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
    if (mode & bm_default) mode |= bm_flip_x | bm_invert;
    drawBitmapBM(bitmap, x, y, w, h, color, mode);
}

void GxGDE0213B72B::drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode)
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
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDE0213B72B_BUFFER_SIZE; i++) {
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
        delay(GxGDE0213B72B_PU_DELAY);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDE0213B72B_BUFFER_SIZE; i++) {
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
        delay(GxGDE0213B72B_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(ram_entry_mode);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDE0213B72B_BUFFER_SIZE; i++) {
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

void GxGDE0213B72B::eraseDisplay(bool using_partial_update)
{
    if (_current_page != -1) return;
    if (using_partial_update) {
        _using_partial_mode = true; // remember
        _Init_Part(0x01);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDE0213B72B_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Part();
        delay(GxGDE0213B72B_PU_DELAY);
        // update erase buffer
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDE0213B72B_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        delay(GxGDE0213B72B_PU_DELAY);
        _PowerOff();
    } else {
        _using_partial_mode = false; // remember
        _Init_Full(0x01);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxGDE0213B72B_BUFFER_SIZE; i++) {
            _writeData(0xFF);
        }
        _Update_Full();
        _PowerOff();
    }
}

void GxGDE0213B72B::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
    if (_current_page != -1) return;
    if (using_rotation) _rotate(x, y, w, h);
    if (x >= GxGDE0213B72B_WIDTH) return;
    if (y >= GxGDE0213B72B_HEIGHT) return;
    uint16_t xe = gx_uint16_min(GxGDE0213B72B_WIDTH, x + w) - 1;
    uint16_t ye = gx_uint16_min(GxGDE0213B72B_HEIGHT, y + h) - 1;
    uint16_t xs_d8 = x / 8;
    uint16_t xe_d8 = xe / 8;
    uint16_t ys_bx = GxGDE0213B72B_HEIGHT - ye - 1;
    uint16_t ye_bx = GxGDE0213B72B_HEIGHT - y - 1;
    _Init_Part(0x01);
    _SetRamArea(xs_d8, xe_d8, ye % 256, ye / 256, y % 256, y / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, ye % 256, ye / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = ys_bx; y1 <= ye_bx; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    _Update_Part();
    delay(GxGDE0213B72B_PU_DELAY);
    // update erase buffer
    _SetRamArea(xs_d8, xe_d8, ye % 256, ye / 256, y % 256, y / 256); // X-source area,Y-gate area
    _SetRamPointer(xs_d8, ye % 256, ye / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = ys_bx; y1 <= ye_bx; y1++) {
        for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++) {
            uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
    delay(GxGDE0213B72B_PU_DELAY);
}

void GxGDE0213B72B::_writeToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h)
{
    //Serial.printf("_writeToWindow(%d, %d, %d, %d, %d, %d)\n", xs, ys, xd, yd, w, h);
    // the screen limits are the hard limits
    if (xs >= GxGDE0213B72B_WIDTH) return;
    if (ys >= GxGDE0213B72B_HEIGHT) return;
    if (xd >= GxGDE0213B72B_WIDTH) return;
    if (yd >= GxGDE0213B72B_HEIGHT) return;
    // flip y for y-decrement mode
    ys = GxGDE0213B72B_HEIGHT - ys - h;
    w = gx_uint16_min(w, GxGDE0213B72B_WIDTH - xs);
    w = gx_uint16_min(w, GxGDE0213B72B_WIDTH - xd);
    h = gx_uint16_min(h, GxGDE0213B72B_HEIGHT - ys);
    h = gx_uint16_min(h, GxGDE0213B72B_HEIGHT - yd);
    uint16_t xds_d8 = xd / 8;
    uint16_t xde_d8 = (xd + w - 1) / 8;
    uint16_t yde = yd + h - 1;
    // soft limits, must send as many bytes as set by _SetRamArea
    uint16_t xse_d8 = xs / 8 + xde_d8 - xds_d8;
    uint16_t yse = ys + h - 1;
    _SetRamArea(xds_d8, xde_d8, yde % 256, yde / 256, yd % 256, yd / 256); // X-source area,Y-gate area
    _SetRamPointer(xds_d8, yde % 256, yde / 256); // set ram
    _waitWhileBusy();
    _writeCommand(0x24);
    for (int16_t y1 = ys; y1 <= yse; y1++) {
        for (int16_t x1 = xs / 8; x1 <= xse_d8; x1++) {
            uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
            _writeData(~data);
        }
    }
}

void GxGDE0213B72B::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            xs = GxGDE0213B72B_WIDTH - xs - w - 1;
            xd = GxGDE0213B72B_WIDTH - xd - w - 1;
            break;
        case 2:
            xs = GxGDE0213B72B_WIDTH - xs - w - 1;
            ys = GxGDE0213B72B_HEIGHT - ys - h - 1;
            xd = GxGDE0213B72B_WIDTH - xd - w - 1;
            yd = GxGDE0213B72B_HEIGHT - yd - h - 1;
            break;
        case 3:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            ys = GxGDE0213B72B_HEIGHT - ys  - h - 1;
            yd = GxGDE0213B72B_HEIGHT - yd  - h - 1;
            break;
        }
    }
    _Init_Part(0x01);
    _writeToWindow(xs, ys, xd, yd, w, h);
    _Update_Part();
    delay(GxGDE0213B72B_PU_DELAY);
    // update erase buffer
    _writeToWindow(xs, ys, xd, yd, w, h);
    delay(GxGDE0213B72B_PU_DELAY);
}

void GxGDE0213B72B::powerDown()
{
    _using_partial_mode = false;
    _PowerOff();
}

void GxGDE0213B72B::_writeCommand(uint8_t command)
{
    if (digitalRead(_busy)) {
        String str = String("command 0x") + String(command, HEX);
        _waitWhileBusy(str.c_str());
    }
    IO.writeCommandTransaction(command);
}

void GxGDE0213B72B::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxGDE0213B72B::_writeDatas(const uint8_t *pData, uint8_t datalen)
{
    if (digitalRead(_busy)) {
        String str = String("command 0x") + String(pData[0], HEX);
        _waitWhileBusy(str.c_str());
    }
    IO.startTransaction();
    for (uint8_t i = 0; i < datalen - 1; i++) { // sub the command
        IO.writeData(*pData++);
    }
    IO.endTransaction();

}


void GxGDE0213B72B::_writeCommandData(const uint8_t *pCommandData, uint8_t datalen)
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

void GxGDE0213B72B::_waitWhileBusy(const char *comment)
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

void GxGDE0213B72B::_setRamDataEntryMode(uint8_t em)
{
    const uint16_t xPixelsPar = GxGDE0213B72B_X_PIXELS - 1;
    const uint16_t yPixelsPar = GxGDE0213B72B_Y_PIXELS - 1;
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

void GxGDE0213B72B::_SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    _writeCommand(0x44);
    _writeData(Xstart);
    _writeData(Xend);
    _writeCommand(0x45);
    _writeData(Ystart);
    _writeData(Ystart1);
    _writeData(Yend);
    _writeData(Yend1);
}

void GxGDE0213B72B::_SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
    _writeCommand(0x4e);
    _writeData(addrX);
    _writeCommand(0x4f);
    _writeData(addrY);
    _writeData(addrY1);
}

void GxGDE0213B72B::_PowerOn(void)
{
    _writeCommand(0x22);
    _writeData(0xc0);
    _writeCommand(0x20);
    _waitWhileBusy("_PowerOn");
}

void GxGDE0213B72B::_PowerOff(void)
{
    _writeCommand(0x22);
    _writeData(0xc3);
    _writeCommand(0x20);
    _waitWhileBusy("_PowerOff");
}

void GxGDE0213B72B::_InitDisplay(uint8_t em)
{
    _waitWhileBusy("_InitDisplay");
    _writeCommand(0x12);
    _waitWhileBusy("_InitDisplay");

    _writeCommand(0x74); //set analog block control
    _writeData(0x54);
    _writeCommand(0x7E); //set digital block control
    _writeData(0x3B);

    _writeCommand(0x01); //Driver output control
    _writeData(0xF9);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x11); //data entry mode
    _writeData(0x01);

    _writeCommand(0x44); //set Ram-X address start/end position
    _writeData(0x00);
    _writeData(0x0F);    //0x0C-->(15+1)*8=128

    _writeCommand(0x45); //set Ram-Y address start/end position
    _writeData(0xF9);   //0xF9-->(249+1)=250
    _writeData(0x00);
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x3C); //BorderWavefrom
    _writeData(0x03);

    _writeCommand(0x2C);     //VCOM Voltage
    _writeData(0x50);    //

    _writeCommand(0x03); //
    _writeData(LUTDefault_full[101]);
    // _writeData(LUT_DATA[100]);

    _writeCommand(0x04); //
    _writeData(LUTDefault_full[102]);
    _writeData(LUTDefault_full[103]);
    _writeData(LUTDefault_full[104]);

    _writeCommand(0x3A);     //Dummy Line
    _writeData(LUTDefault_full[106]);
    _writeCommand(0x3B);     //Gate time
    _writeData(LUTDefault_full[107]);

    _writeCommandData(LUTDefault_full, 101);

    _writeCommand(0x4E);   // set RAM x address count to 0;
    _writeData(0x00);
    _writeCommand(0x4F);   // set RAM y address count to 0X127;
    _writeData(0xF9);
    _writeData(0x00);
    _waitWhileBusy("_InitDisplay");
}

void GxGDE0213B72B::_Init_Full(uint8_t em)
{
    _InitDisplay(em);
    _writeCommandData(LUTDefault_full, sizeof(LUTDefault_full));
    _PowerOn();
}

void GxGDE0213B72B::_Init_Part(uint8_t em)
{
    _InitDisplay(em);
    _writeCommandData(LUTDefault_part, sizeof(LUTDefault_part));
    _PowerOn();
}

void GxGDE0213B72B::_Update_Full(void)
{
    _writeCommand(0x22);
    // _writeData(0xc4);
    _writeData(0xC7);
    _writeCommand(0x20);
    _waitWhileBusy("_Update_Full");
    // _writeCommand(0xff);
}

void GxGDE0213B72B::_Update_Part(void)
{
    _writeCommand(0x22);
    _writeData(0x04);
    _writeCommand(0x20);
    _waitWhileBusy("_Update_Part");
    _writeCommand(0xff);
}

void GxGDE0213B72B::drawPaged(void (*drawCallback)(void))
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxGDE0213B72B_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback();
        for (int16_t y1 = 0; y1 < GxGDE0213B72B_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDE0213B72B_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDE0213B72B::drawPaged(void (*drawCallback)(uint32_t), uint32_t p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxGDE0213B72B_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxGDE0213B72B_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDE0213B72B_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDE0213B72B::drawPaged(void (*drawCallback)(const void *), const void *p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxGDE0213B72B_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxGDE0213B72B_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDE0213B72B_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDE0213B72B::drawPaged(void (*drawCallback)(const void *, const void *), const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(0x01);
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxGDE0213B72B_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p1, p2);
        for (int16_t y1 = 0; y1 < GxGDE0213B72B_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxGDE0213B72B_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxGDE0213B72B_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
                _writeData(~data);
            }
        }
    }
    _current_page = -1;
    _Update_Full();
    _PowerOff();
}

void GxGDE0213B72B::_rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
{
    switch (getRotation()) {
    case 1:
        swap(x, y);
        swap(w, h);
        x = GxGDE0213B72B_WIDTH - x - w - 1;
        break;
    case 2:
        x = GxGDE0213B72B_WIDTH - x - w - 1;
        y = GxGDE0213B72B_HEIGHT - y - h - 1;
        break;
    case 3:
        swap(x, y);
        swap(w, h);
        y = GxGDE0213B72B_HEIGHT - y - h - 1;
        break;
    }
}

void GxGDE0213B72B::drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDE0213B72B_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDE0213B72B::drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDE0213B72B_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDE0213B72B::drawPagedToWindow(void (*drawCallback)(const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDE0213B72B_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDE0213B72B::drawPagedToWindow(void (*drawCallback)(const void *, const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    _Init_Part(0x01);
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _Update_Part();
    delay(GxGDE0213B72B_PU_DELAY);
    // update erase buffer
    for (_current_page = GxGDE0213B72B_PAGES - 1; _current_page >= 0; _current_page--) {
        // flip y for y-decrement mode
        uint16_t yds = (GxGDE0213B72B_PAGES - _current_page - 1) * GxGDE0213B72B_PAGE_HEIGHT;
        uint16_t yde = yds + GxGDE0213B72B_PAGE_HEIGHT;
        yds = gx_uint16_max(y, yds);
        yde = gx_uint16_min(y + h, yde);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = (GxGDE0213B72B_PAGES - 1) * GxGDE0213B72B_PAGE_HEIGHT + (yds % GxGDE0213B72B_PAGE_HEIGHT);
            _writeToWindow(x, ys, x, yds, w, yde - yds);
        }
    }
    _current_page = -1;
    _PowerOff();
}

void GxGDE0213B72B::drawCornerTest(uint8_t em)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _Init_Full(em);
    _writeCommand(0x24);
    for (uint32_t y = 0; y < GxGDE0213B72B_HEIGHT; y++) {
        for (uint32_t x = 0; x < GxGDE0213B72B_WIDTH / 8; x++) {
            uint8_t data = 0xFF;
            if ((x < 1) && (y < 8)) data = 0x00;
            if ((x > GxGDE0213B72B_WIDTH / 8 - 3) && (y < 16)) data = 0x00;
            if ((x > GxGDE0213B72B_WIDTH / 8 - 4) && (y > GxGDE0213B72B_HEIGHT - 25)) data = 0x00;
            if ((x < 4) && (y > GxGDE0213B72B_HEIGHT - 33)) data = 0x00;
            _writeData(data);
        }
    }
    _Update_Full();
    _PowerOff();
}

