// class GxDEPG0290R :
// Driver: SSD1680Z8

#include "GxDEPG0290R.h"

//#define DISABLE_DIAGNOSTIC_OUTPUT

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// Partial Update Delay, may have an influence on degradation
#define GxDEPG0290R_PU_DELAY 500

GxDEPG0290R::GxDEPG0290R(GxIO &io, int8_t rst, int8_t busy)
    : GxEPD(GxDEPG0290R_WIDTH, GxDEPG0290R_HEIGHT), IO(io),
      _current_page(-1), _using_partial_mode(false), _diag_enabled(false),
      _rst(rst), _busy(busy)
{
}

void GxDEPG0290R::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
        swap(x, y);
        x = GxDEPG0290R_WIDTH - x - 1;
        break;
    case 2:
        x = GxDEPG0290R_WIDTH - x - 1;
        y = GxDEPG0290R_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxDEPG0290R_HEIGHT - y - 1;
        break;
    }
    uint16_t i = x / 8 + y * GxDEPG0290R_WIDTH / 8;
    if (_current_page < 1) {
        if (i >= sizeof(_black_buffer))
            return;
    } else {
        y -= _current_page * GxDEPG0290R_PAGE_HEIGHT;
        if ((y < 0) || (y >= GxDEPG0290R_PAGE_HEIGHT))
            return;
        i = x / 8 + y * GxDEPG0290R_WIDTH / 8;
    }

    _black_buffer[i] = (_black_buffer[i] & (0xFF ^ (1 << (7 - x % 8)))); // white
    _red_buffer[i]   = (_red_buffer[i]   & (0xFF ^ (1 << (7 - x % 8)))); // white
    if (color == GxEPD_WHITE) {
        _red_buffer[i] = (_red_buffer[i] | (1 << (7 - x % 8)));
    } else if (color == GxEPD_BLACK) {  ///error !!!
        _black_buffer[i] = (_black_buffer[i] | (1 << (7 - x % 8)));
        _red_buffer[i] = (_red_buffer[i] | (1 << (7 - x % 8)));
    } else if (color == GxEPD_RED) {
        return;
    }
}


void GxDEPG0290R::init(uint32_t serial_diag_bitrate)
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
    }
    fillScreen(GxEPD_WHITE);
    _current_page = -1;
    _using_partial_mode = false;
}

void GxDEPG0290R::fillScreen(uint16_t color)
{
    uint8_t black = 0x00;
    uint8_t red = 0x00;
    if (color == GxEPD_WHITE) {
        black = 0x00;
        red = 0xFF;
    } else if (color == GxEPD_BLACK) {
        red = 0xFF;
        black = 0xFF;
    } else if (color == GxEPD_RED) {
        black = 0x00;
        red = 0x00;
    } else if ((color & 0xF100) > (0xF100 / 2)) {
        red = 0xFF;
    } else if ((((color & 0xF100) >> 11) + ((color & 0x07E0) >> 5) + (color & 0x001F)) < 3 * 255 / 2) {
        black = 0xFF;
    }
    for (uint16_t x = 0; x < sizeof(_black_buffer); x++) {
        _black_buffer[x] = black;
        _red_buffer[x] = red;
    }
}

void GxDEPG0290R::update(void)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _setRamDataEntryMode(0x03);
    _writeCommand(0x24);
    for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
        _writeData((i < sizeof(_black_buffer)) ? ~_black_buffer[i] : 0xFF);
    }
    _writeCommand(0x26);
    for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
        _writeData((i < sizeof(_red_buffer)) ? ~_red_buffer[i] : 0xFF);
    }
    _writeCommand(0x20);
    _waitWhileBusy("update");
    _sleep();
}

void  GxDEPG0290R::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
    if (mode & bm_default) mode |= bm_invert;
    drawBitmapBM(bitmap, x, y, w, h, color, mode);
}

void GxDEPG0290R::drawExamplePicture(const uint8_t *black_bitmap, const uint8_t *red_bitmap, uint32_t black_size, uint32_t red_size)
{
    drawPicture(black_bitmap, red_bitmap, black_size, red_size, bm_invert_red);
}

void GxDEPG0290R::drawPicture(const uint8_t *black_bitmap, const uint8_t *red_bitmap, uint32_t black_size, uint32_t red_size, int16_t mode)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x24);
    for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
        uint8_t data = 0xFF; // white is 0xFF on device
        if (i < black_size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
            data = pgm_read_byte(&black_bitmap[i]);
#else
            data = black_bitmap[i];
#endif
            if (mode & bm_invert) data = ~data;
        }
        _writeData(data);
    }
    _writeCommand(0x26);
    for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
        uint8_t data = 0xFF; // white is 0xFF on device
        if (i < red_size) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
            data = pgm_read_byte(&red_bitmap[i]);
#else
            data = red_bitmap[i];
#endif
            if (mode & bm_invert_red) data = ~data;
        }
        _writeData(data);
    }
    _writeCommand(0x22);
    _writeData(0xF7);
    _writeCommand(0x20);
    _waitWhileBusy("drawPicture");
    _sleep();
}

void GxDEPG0290R::drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode)
{
    if (_current_page != -1) return;
    // example bitmaps are normal on b/w, but inverted on red
    if (mode & bm_default) mode |= bm_normal;
    if (mode & bm_partial_update) {
        _using_partial_mode = true;
        _wakeUp();
        IO.writeCommandTransaction(0x91); // partial in
        _setPartialRamArea(0, 0, GxDEPG0290R_WIDTH - 1, GxDEPG0290R_HEIGHT - 1);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
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
        _writeCommand(0x26);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x22);
        _writeData(0xF7);
        _writeCommand(0x20);
        _waitWhileBusy("drawBitmap");
        IO.writeCommandTransaction(0x92); // partial out
    } else {
        _using_partial_mode = false;
        _wakeUp();
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
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
        _writeCommand(0x26);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x22);
        _writeData(0xF7);
        _writeCommand(0x20);
        _waitWhileBusy("drawBitmap");
        _sleep();
    }
}

void GxDEPG0290R::eraseDisplay(bool using_partial_update)
{
    if (_current_page != -1) return;
    if (using_partial_update) {
        if (!_using_partial_mode) _wakeUp();
        _using_partial_mode = true; // remember
        IO.writeCommandTransaction(0x91); // partial in
        _setPartialRamArea(0, 0, GxDEPG0290R_WIDTH - 1, GxDEPG0290R_HEIGHT - 1);
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE * 2; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x26);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x22);
        _writeData(0xF7);
        _writeCommand(0x20);
        _waitWhileBusy("eraseDisplay");
        IO.writeCommandTransaction(0x92); // partial out
    } else {
        _using_partial_mode = false; // remember
        _wakeUp();
        _writeCommand(0x24);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE * 2; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x26);
        for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x22);
        _writeData(0xF7);
        _writeCommand(0x20);
        _waitWhileBusy("eraseDisplay");
        _sleep();
    }
}

void GxDEPG0290R::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
    if (_current_page != -1) return;
    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(x, y);
            swap(w, h);
            x = GxDEPG0290R_WIDTH - x - w - 1;
            break;
        case 2:
            x = GxDEPG0290R_WIDTH - x - w - 1;
            y = GxDEPG0290R_HEIGHT - y - h - 1;
            break;
        case 3:
            swap(x, y);
            swap(w, h);
            y = GxDEPG0290R_HEIGHT - y  - h - 1;
            break;
        }
    }
    if (x >= GxDEPG0290R_WIDTH) return;
    if (y >= GxDEPG0290R_HEIGHT) return;
    // x &= 0xFFF8; // byte boundary, not here, use encompassing rectangle
    uint16_t xe = gx_uint16_min(GxDEPG0290R_WIDTH, x + w) - 1;
    uint16_t ye = gx_uint16_min(GxDEPG0290R_HEIGHT, y + h) - 1;
    // x &= 0xFFF8; // byte boundary, not needed here
    uint16_t xs_bx = x / 8;
    uint16_t xe_bx = (xe + 7) / 8;
    if (!_using_partial_mode) _wakeUp();
    _using_partial_mode = true;
    IO.writeCommandTransaction(0x91); // partial in
    _setPartialRamArea(x, y, xe, ye);
    IO.writeCommandTransaction(0x24);
    for (int16_t y1 = y; y1 <= ye; y1++) {
        for (int16_t x1 = xs_bx; x1 < xe_bx; x1++) {
            uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_black_buffer)) ? _black_buffer[idx] : 0x00; // white is 0x00 in buffer
            IO.writeDataTransaction(~data); // white is 0xFF on device
        }
    }
    IO.writeCommandTransaction(0x26);
    for (int16_t y1 = y; y1 <= ye; y1++) {
        for (int16_t x1 = xs_bx; x1 < xe_bx; x1++) {
            uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_red_buffer)) ? _red_buffer[idx] : 0x00; // white is 0x00 in buffer
            IO.writeDataTransaction(~data); // white is 0xFF on device
        }
    }
    IO.writeCommandTransaction(0x20);      //display refresh
    _waitWhileBusy("updateWindow");
    IO.writeCommandTransaction(0x92); // partial out
    delay(GxDEPG0290R_PU_DELAY); // don't stress this display
}

void GxDEPG0290R::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
    if (!_using_partial_mode) _wakeUp();
    _using_partial_mode = true;
    _writeToWindow(xs, ys, xd, yd, w, h, using_rotation);
    IO.writeCommandTransaction(0x20);      //display refresh
    _waitWhileBusy("updateToWindow");
    delay(GxDEPG0290R_PU_DELAY); // don't stress this display
}

void GxDEPG0290R::_writeToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            xs = GxDEPG0290R_WIDTH - xs - w - 1;
            xd = GxDEPG0290R_WIDTH - xd - w - 1;
            break;
        case 2:
            xs = GxDEPG0290R_WIDTH - xs - w - 1;
            ys = GxDEPG0290R_HEIGHT - ys - h - 1;
            xd = GxDEPG0290R_WIDTH - xd - w - 1;
            yd = GxDEPG0290R_HEIGHT - yd - h - 1;
            break;
        case 3:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            ys = GxDEPG0290R_HEIGHT - ys  - h - 1;
            yd = GxDEPG0290R_HEIGHT - yd  - h - 1;
            break;
        }
    }
    if (xs >= GxDEPG0290R_WIDTH) return;
    if (ys >= GxDEPG0290R_HEIGHT) return;
    if (xd >= GxDEPG0290R_WIDTH) return;
    if (yd >= GxDEPG0290R_HEIGHT) return;
    // the screen limits are the hard limits
    uint16_t xde = gx_uint16_min(GxDEPG0290R_WIDTH, xd + w) - 1;
    uint16_t yde = gx_uint16_min(GxDEPG0290R_HEIGHT, yd + h) - 1;
    IO.writeCommandTransaction(0x91); // partial in
    // soft limits, must send as many bytes as set by _SetRamArea
    uint16_t yse = ys + yde - yd;
    uint16_t xss_d8 = xs / 8;
    uint16_t xse_d8 = xss_d8 + _setPartialRamArea(xd, yd, xde, yde);
    IO.writeCommandTransaction(0x24);
    for (int16_t y1 = ys; y1 <= yse; y1++) {
        for (int16_t x1 = xss_d8; x1 < xse_d8; x1++) {
            uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_black_buffer)) ? _black_buffer[idx] : 0x00; // white is 0x00 in buffer
            IO.writeDataTransaction(~data); // white is 0xFF on device
        }
    }
    IO.writeCommandTransaction(0x26);
    for (int16_t y1 = ys; y1 <= yse; y1++) {
        for (int16_t x1 = xss_d8; x1 < xse_d8; x1++) {
            uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
            uint8_t data = (idx < sizeof(_red_buffer)) ? _red_buffer[idx] : 0x00; // white is 0x00 in buffer
            IO.writeDataTransaction(~data); // white is 0xFF on device
        }
    }
    IO.writeCommandTransaction(0x92); // partial out
}

void GxDEPG0290R::powerDown()
{
    _using_partial_mode = false; // force _wakeUp()
    _sleep();
}

uint16_t GxDEPG0290R::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye)
{
    x &= 0xFFF8; // byte boundary
    xe = (xe - 1) | 0x0007; // byte boundary - 1
    IO.writeCommandTransaction(0x90); // partial window
    //IO.writeDataTransaction(x / 256);
    IO.writeDataTransaction(x % 256);
    //IO.writeDataTransaction(xe / 256);
    IO.writeDataTransaction(xe % 256);
    IO.writeDataTransaction(y / 256);
    IO.writeDataTransaction(y % 256);
    IO.writeDataTransaction(ye / 256);
    IO.writeDataTransaction(ye % 256);
    IO.writeDataTransaction(0x01); // don't see any difference
    //IO.writeDataTransaction(0x00); // don't see any difference
    return (7 + xe - x) / 8; // number of bytes to transfer per line
}

void GxDEPG0290R::_writeCommand(uint8_t command)
{
    IO.writeCommandTransaction(command);
}

void GxDEPG0290R::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxDEPG0290R::_waitWhileBusy(const char *comment)
{
    unsigned long start = micros();
    while (1) {
        if (digitalRead(_busy) == 0) break;
        delay(1);
        if (micros() - start > 20000000) { // >14.9s !
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


void GxDEPG0290R::_setRamDataEntryMode(uint8_t em)
{
    const uint16_t xPixelsPar = GxDEPG0290R_WIDTH - 1;
    const uint16_t yPixelsPar = GxDEPG0290R_HEIGHT - 1;
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

void GxDEPG0290R::_SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
    _writeCommand(0x4e);
    _writeData(addrX + 1);
    _writeCommand(0x4f);
    _writeData(addrY);
    _writeData(addrY1);
}

void GxDEPG0290R::_SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
    _writeCommand(0x44);
    _writeData(Xstart + 1 );
    _writeData(Xend + 1);
    _writeCommand(0x45);
    _writeData(Ystart);
    _writeData(Ystart1);
    _writeData(Yend);
    _writeData(Yend1);
}

void GxDEPG0290R::_wakeUp()
{
    // reset required for wakeup
    digitalWrite(_rst, 0);
    delay(100);
    digitalWrite(_rst, 1);
    delay(100);
    _waitWhileBusy();
    _writeCommand(0x12); // soft reset
    _waitWhileBusy();
}

void GxDEPG0290R::_sleep(void)
{
    _writeCommand(0x10);      //power off
    _writeData(0x01);
    delay(100);
    // _waitWhileBusy("_sleep Power Off");
    // if (_rst >= 0) {
    //     _writeCommand(0x07); // deep sleep
    //     _writeData (0xa5);
    // }
}

void GxDEPG0290R::drawPaged(void (*drawCallback)(void))
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback();
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_black_buffer)) ? _black_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _writeCommand(0x26);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback();
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_red_buffer)) ? _red_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _current_page = -1;
    _writeCommand(0x22); //display refresh
    _writeData(0xC7);
    _writeCommand(0x20);
    _waitWhileBusy("drawPaged");
    _sleep();
}

void GxDEPG0290R::drawPaged(void (*drawCallback)(uint32_t), uint32_t p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_black_buffer)) ? _black_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _writeCommand(0x26);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_red_buffer)) ? _red_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _current_page = -1;
    _writeCommand(0x22);
    _writeData(0xF7);
    _writeCommand(0x20);
    _waitWhileBusy("drawPaged");
    _sleep();
}

void GxDEPG0290R::drawPaged(void (*drawCallback)(const void *), const void *p)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_black_buffer)) ? _black_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _writeCommand(0x26);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p);
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_red_buffer)) ? _red_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _current_page = -1;
    _writeCommand(0x22);
    _writeData(0xF7);
    _writeCommand(0x20);
    _waitWhileBusy("drawPaged");
    _sleep();
}

void GxDEPG0290R::drawPaged(void (*drawCallback)(const void *, const void *), const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x24);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p1, p2);
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_black_buffer)) ? _black_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _writeCommand(0x26);
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        fillScreen(GxEPD_WHITE);
        drawCallback(p1, p2);
        for (int16_t y1 = 0; y1 < GxDEPG0290R_PAGE_HEIGHT; y1++) {
            for (int16_t x1 = 0; x1 < GxDEPG0290R_WIDTH / 8; x1++) {
                uint16_t idx = y1 * (GxDEPG0290R_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_red_buffer)) ? _red_buffer[idx] : 0x00;
                _writeData(~data); // white is 0xFF on device
            }
        }
    }
    _current_page = -1;
    // _writeCommand(0x20); //display refresh
    _writeCommand(0x22);
    _writeData(0xF7);
    _writeCommand(0x20);
    _waitWhileBusy("drawPaged");
    _sleep();
}

void GxDEPG0290R::_rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
{
    switch (getRotation()) {
    case 1:
        swap(x, y);
        swap(w, h);
        x = GxDEPG0290R_WIDTH - x - w - 1;
        break;
    case 2:
        x = GxDEPG0290R_WIDTH - x - w - 1;
        y = GxDEPG0290R_HEIGHT - y - h - 1;
        break;
    case 3:
        swap(x, y);
        swap(w, h);
        y = GxDEPG0290R_HEIGHT - y - h - 1;
        break;
    }
}

void GxDEPG0290R::drawPagedToWindow(void (*drawCallback)(void), uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxDEPG0290R_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxDEPG0290R_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback();
            uint16_t ys = yds % GxDEPG0290R_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds, false);
        }
    }
    _current_page = -1;
    IO.writeCommandTransaction(0x20);      //display refresh
    _waitWhileBusy("drawPagedToWindow");
    delay(GxDEPG0290R_PU_DELAY); // don't stress this display
}

void GxDEPG0290R::drawPagedToWindow(void (*drawCallback)(uint32_t), uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxDEPG0290R_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxDEPG0290R_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = yds % GxDEPG0290R_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds, false);
        }
    }
    _current_page = -1;
    IO.writeCommandTransaction(0x20);      //display refresh
    _waitWhileBusy("drawPagedToWindow");
    delay(GxDEPG0290R_PU_DELAY); // don't stress this display
}

void GxDEPG0290R::drawPagedToWindow(void (*drawCallback)(const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxDEPG0290R_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxDEPG0290R_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p);
            uint16_t ys = yds % GxDEPG0290R_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds, false);
        }
    }
    _current_page = -1;
    IO.writeCommandTransaction(0x20);      //display refresh
    _waitWhileBusy("drawPagedToWindow");
    delay(GxDEPG0290R_PU_DELAY); // don't stress this display
}

void GxDEPG0290R::drawPagedToWindow(void (*drawCallback)(const void *, const void *), uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *p1, const void *p2)
{
    if (_current_page != -1) return;
    _rotate(x, y, w, h);
    if (!_using_partial_mode) {
        eraseDisplay(false);
        eraseDisplay(true);
    }
    _using_partial_mode = true;
    for (_current_page = 0; _current_page < GxDEPG0290R_PAGES; _current_page++) {
        uint16_t yds = gx_uint16_max(y, _current_page * GxDEPG0290R_PAGE_HEIGHT);
        uint16_t yde = gx_uint16_min(y + h, (_current_page + 1) * GxDEPG0290R_PAGE_HEIGHT);
        if (yde > yds) {
            fillScreen(GxEPD_WHITE);
            drawCallback(p1, p2);
            uint16_t ys = yds % GxDEPG0290R_PAGE_HEIGHT;
            _writeToWindow(x, ys, x, yds, w, yde - yds, false);
        }
    }
    _current_page = -1;
    IO.writeCommandTransaction(0x20);      //display refresh
    _waitWhileBusy("drawPagedToWindow");
    delay(GxDEPG0290R_PU_DELAY); // don't stress this display
}

void GxDEPG0290R::drawCornerTest(uint8_t em)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x24);
    for (uint32_t y = 0; y < GxDEPG0290R_HEIGHT; y++) {
        for (uint32_t x = 0; x < GxDEPG0290R_WIDTH / 8; x++) {
            uint8_t data = 0xFF; // white is 0xFF on device
            if ((x < 1) && (y < 8)) data = 0x00;
            if ((x > GxDEPG0290R_WIDTH / 8 - 3) && (y < 16)) data = 0x00;
            if ((x > GxDEPG0290R_WIDTH / 8 - 4) && (y > GxDEPG0290R_HEIGHT - 25)) data = 0x00;
            if ((x < 4) && (y > GxDEPG0290R_HEIGHT - 33)) data = 0x00;
            _writeData(data);
        }
    }
    _writeCommand(0x26);
    for (uint32_t i = 0; i < GxDEPG0290R_BUFFER_SIZE; i++) {
        _writeData(0xFF); // white is 0xFF on device
    }
    _writeCommand(0x22);
    _writeData(0xF7);
    _writeCommand(0x20);
    _waitWhileBusy("drawCornerTest");
    _sleep();
}

