// class GxGDGDEW0102T4 : Display class for GDEW0102T4 e-Paper from Dalian Good Display Co., Ltd.: https://www.good-display.cn/product/207.html
//
// based on Demo Example from Good Display, available here: https://www.good-display.cn/product/207.html
// Controller: UC8175


#include "GxGDGDEW0102T4.h"

//#define DISABLE_DIAGNOSTIC_OUTPUT

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

//full screen update LUT
const unsigned char GxGDGDEW0102T4::lut_23_wb[] = {
    0x60, 0x5A, 0x5A, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char GxGDGDEW0102T4::lut_24_bb[] = {
    0x90, 0x5A, 0x5A, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

//partial screen update LUT

const unsigned char GxGDGDEW0102T4::lut_23_wb_partial[] = {
    0x60, 0x01, 0x01, 0x00, 0x00, 0x01,
    0x80, 0x0f, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char GxGDGDEW0102T4::lut_24_bb_partial[] = {
    0x90, 0x01, 0x01, 0x00, 0x00, 0x01,
    0x40, 0x0f, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// Partial Update Delay, may have an influence on degradation
#define GxGDGDEW0102T4_PU_DELAY 100

GxGDGDEW0102T4::GxGDGDEW0102T4(GxIO &io, int8_t rst, int8_t busy)
    : GxEPD(GxGDGDEW0102T4_WIDTH, GxGDGDEW0102T4_HEIGHT), IO(io),
      _current_page(-1), _using_partial_mode(false), _diag_enabled(false),
      _rst(rst), _busy(busy)
{
}

void GxGDGDEW0102T4::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
        swap(x, y);
        x = GxGDGDEW0102T4_WIDTH - x - 1;
        break;
    case 2:
        x = GxGDGDEW0102T4_WIDTH - x - 1;
        y = GxGDGDEW0102T4_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxGDGDEW0102T4_HEIGHT - y - 1;
        break;
    }
    uint16_t i = x / 8 + y * GxGDGDEW0102T4_WIDTH / 8;
    if (_current_page < 1) {
        if (i >= sizeof(_buffer)) return;
    } else {
        y -= _current_page * GxGDGDEW0102T4_PAGE_HEIGHT;
        if ((y < 0) || (y >= GxGDGDEW0102T4_PAGE_HEIGHT)) return;
        i = x / 8 + y * GxGDGDEW0102T4_WIDTH / 8;
    }

    if (!color)
        _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
    else
        _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}


void GxGDGDEW0102T4::init(uint32_t serial_diag_bitrate)
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

void GxGDGDEW0102T4::fillScreen(uint16_t color)
{
    uint8_t data = (color == GxEPD_BLACK) ? 0xFF : 0x00;
    for (uint16_t x = 0; x < sizeof(_buffer); x++) {
        _buffer[x] = data;
    }
}

void GxGDGDEW0102T4::update(void)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x10);
    for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
        _writeData(0xFF); // 0xFF is white
    }
    _writeCommand(0x13);
    for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
        _writeData((i < sizeof(_buffer)) ? ~_buffer[i] : 0xFF);
    }
    _writeCommand(0x12); //display refresh
    _waitWhileBusy("update");
    _sleep();
}

void  GxGDGDEW0102T4::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
    drawBitmapBM(bitmap, x, y, w, h, color, mode);
}

void GxGDGDEW0102T4::drawBitmap(const uint8_t *bitmap, uint32_t size, int16_t mode)
{
    if (_current_page != -1) return;
    if (mode & bm_default) mode |= bm_invert; // Snoopy is inverted
    if (mode & bm_partial_update) {
        if (!_using_partial_mode) _wakeUp();
        _using_partial_mode = true;
        _Init_PartialUpdate();
        for (uint16_t twice = 0; twice < 2; twice++) {
            // leave both controller buffers equal
            IO.writeCommandTransaction(0x91); // partial in
            _setPartialRamArea(0, 0, GxGDGDEW0102T4_WIDTH - 1, GxGDGDEW0102T4_HEIGHT - 1);
            _writeCommand(0x13);
            for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
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
            _writeCommand(0x12); //display refresh
            _waitWhileBusy("drawBitmap");
            IO.writeCommandTransaction(0x92); // partial out
        } // leave both controller buffers equal
        delay(GxGDGDEW0102T4_PU_DELAY); // don't stress this display
    } else {
        _using_partial_mode = false;
        _wakeUp();
        _writeCommand(0x10);
        for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
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
        _writeCommand(0x12); //display refresh
        _waitWhileBusy("drawBitmap");
        _sleep();
    }
}

void GxGDGDEW0102T4::eraseDisplay(bool using_partial_update)
{
    if (_current_page != -1) return;
    if (using_partial_update) {
        if (!_using_partial_mode) _wakeUp();
        _using_partial_mode = true;
        _Init_PartialUpdate();
        for (uint16_t twice = 0; twice < 2; twice++) {
            // leave both controller buffers equal
            IO.writeCommandTransaction(0x91); // partial in
            _setPartialRamArea(0, 0, GxGDGDEW0102T4_WIDTH - 1, GxGDGDEW0102T4_HEIGHT - 1);
            _writeCommand(0x13);
            for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
                _writeData(0xFF); // white is 0xFF on device
            }
            _writeCommand(0x12); //display refresh
            _waitWhileBusy("eraseDisplay");
            IO.writeCommandTransaction(0x92); // partial out
            if (_using_partial_mode) break;
        } // leave both controller buffers equal
    } else {
        _using_partial_mode = false; // remember
        _wakeUp();
        _writeCommand(0x10);
        for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE * 2; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x13);
        for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
            _writeData(0xFF); // white is 0xFF on device
        }
        _writeCommand(0x12); //display refresh
        _waitWhileBusy("eraseDisplay");
        _sleep();
    }
}

void GxGDGDEW0102T4::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
    if (_current_page != -1) return;
    if (using_rotation) _rotate(x, y, w, h);
    if (x >= GxGDGDEW0102T4_WIDTH) return;
    if (y >= GxGDGDEW0102T4_HEIGHT) return;
    uint16_t xe = gx_uint16_min(GxGDGDEW0102T4_WIDTH, x + w) - 1;
    uint16_t ye = gx_uint16_min(GxGDGDEW0102T4_HEIGHT, y + h) - 1;
    // x &= 0xFFF8; // byte boundary, not needed here
    uint16_t xs_bx = x / 8;
    uint16_t xe_bx = (xe + 7) / 8;
    if (!_using_partial_mode) _wakeUp();
    _using_partial_mode = true;
    _Init_PartialUpdate();
    for (uint16_t twice = 0; twice < 2; twice++) {
        // leave both controller buffers equal
        IO.writeCommandTransaction(0x91); // partial in
        _setPartialRamArea(x, y, xe, ye);
        IO.writeCommandTransaction(0x13);
        for (int16_t y1 = y; y1 <= ye; y1++) {
            for (int16_t x1 = xs_bx; x1 < xe_bx; x1++) {
                uint16_t idx = y1 * (GxGDGDEW0102T4_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00; // white is 0x00 in buffer
                IO.writeDataTransaction(~data); // white is 0xFF on device
            }
        }
        IO.writeCommandTransaction(0x12);      //display refresh
        delay(10);
        _waitWhileBusy("updateWindow");
        IO.writeCommandTransaction(0x92); // partial out
    } // leave both controller buffers equal
    delay(GxGDGDEW0102T4_PU_DELAY); // don't stress this display
}

void GxGDGDEW0102T4::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
    if (using_rotation) {
        switch (getRotation()) {
        case 1:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            xs = GxGDGDEW0102T4_WIDTH - xs - w - 1;
            xd = GxGDGDEW0102T4_WIDTH - xd - w - 1;
            break;
        case 2:
            xs = GxGDGDEW0102T4_WIDTH - xs - w - 1;
            ys = GxGDGDEW0102T4_HEIGHT - ys - h - 1;
            xd = GxGDGDEW0102T4_WIDTH - xd - w - 1;
            yd = GxGDGDEW0102T4_HEIGHT - yd - h - 1;
            break;
        case 3:
            swap(xs, ys);
            swap(xd, yd);
            swap(w, h);
            ys = GxGDGDEW0102T4_HEIGHT - ys  - h - 1;
            yd = GxGDGDEW0102T4_HEIGHT - yd  - h - 1;
            break;
        }
    }
    if (xs >= GxGDGDEW0102T4_WIDTH) return;
    if (ys >= GxGDGDEW0102T4_HEIGHT) return;
    if (xd >= GxGDGDEW0102T4_WIDTH) return;
    if (yd >= GxGDGDEW0102T4_HEIGHT) return;
    // the screen limits are the hard limits
    uint16_t xde = gx_uint16_min(GxGDGDEW0102T4_WIDTH, xd + w) - 1;
    uint16_t yde = gx_uint16_min(GxGDGDEW0102T4_HEIGHT, yd + h) - 1;
    if (!_using_partial_mode) _wakeUp();
    _using_partial_mode = true;
    _Init_PartialUpdate();
    for (uint16_t twice = 0; twice < 2; twice++) {
        // leave both controller buffers equal
        IO.writeCommandTransaction(0x91); // partial in
        // soft limits, must send as many bytes as set by _SetRamArea
        uint16_t yse = ys + yde - yd;
        uint16_t xss_d8 = xs / 8;
        uint16_t xse_d8 = xss_d8 + _setPartialRamArea(xd, yd, xde, yde);
        IO.writeCommandTransaction(0x13);
        for (int16_t y1 = ys; y1 <= yse; y1++) {
            for (int16_t x1 = xss_d8; x1 < xse_d8; x1++) {
                uint16_t idx = y1 * (GxGDGDEW0102T4_WIDTH / 8) + x1;
                uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00; // white is 0x00 in buffer
                IO.writeDataTransaction(~data); // white is 0xFF on device
            }
        }
        IO.writeCommandTransaction(0x12);      //display refresh
        _waitWhileBusy("updateToWindow");
        IO.writeCommandTransaction(0x92); // partial out
    } // leave both controller buffers equal
    delay(GxGDGDEW0102T4_PU_DELAY); // don't stress this display
}

void GxGDGDEW0102T4::powerDown()
{
    _using_partial_mode = false; // force _wakeUp()
    _sleep();
}

uint16_t GxGDGDEW0102T4::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye)
{
    IO.writeCommandTransaction(0x90); // partial window
    IO.writeDataTransaction(x);
    IO.writeDataTransaction(xe);
    IO.writeDataTransaction(y);
    IO.writeDataTransaction(ye);
    IO.writeDataTransaction(0x01);
    return 0;
}

void GxGDGDEW0102T4::_writeCommand(uint8_t command)
{
    IO.writeCommandTransaction(command);
}

void GxGDGDEW0102T4::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxGDGDEW0102T4::_waitWhileBusy(const char *comment)
{
    unsigned long start = micros();
    while (1) {
        if (digitalRead(_busy) == 1) break;
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

void GxGDGDEW0102T4::_wakeUp()
{
    // reset required for wakeup
    if (_rst >= 0) {
        digitalWrite(_rst, 0);
        delay(10);
        digitalWrite(_rst, 1);
        delay(10);
    }

    _writeCommand(0x00);
    _writeData(0x5f);

    _writeCommand(0x2A); //panel setting
    _writeData(0x00);
    _writeData(0x00);

    _writeCommand(0x04);
    _waitWhileBusy("_wakeUp Power On");

    _writeCommand(0x50); //panel setting
    _writeData(0x97);

}

void GxGDGDEW0102T4::_sleep(void)
{
    _writeCommand(0x02);      //power off
    _waitWhileBusy("_sleep Power Off");
    if (_rst >= 0) {
        _writeCommand(0x07); // deep sleep
        _writeData (0xa5);
    }
}

void GxGDGDEW0102T4::_Init_FullUpdate(void)
{
    // _writeCommand(0x30);
    // _writeData (0x13);

    // _writeCommand(0x50);
    // _writeData (0x57);

    // _writeCommand(0x60);
    // _writeData (0x22);

    // _writeCommand(0x82); //vcom_DC setting
    // _writeData (0x12);

    // _writeCommand(0xe3);
    // _writeData (0x33);

    // unsigned int count;
    // {
    //     _writeCommand(0x23);              //wb w
    //     for (count = 0; count < 42; count++) {
    //         _writeData(lut_23_wb[count]);
    //     }
    //     _writeCommand(0x24);              //bb b
    //     for (count = 0; count < 42; count++) {
    //         _writeData(lut_24_bb[count]);
    //     }
    // }
}

void GxGDGDEW0102T4::_Init_PartialUpdate(void)
{
#if 0
    _writeCommand(0x30);
    _writeData (0x05);

    _writeCommand(0x50);
    _writeData (0xF2);

    _writeCommand(0x60);
    _writeData (0x22);

    _writeCommand(0x82); //vcom_DC setting
    _writeData (0x00);

    _writeCommand(0xe3);
    _writeData (0x33);

    unsigned int count;
    {
        _writeCommand(0x23);              //wb w
        for (count = 0; count < 42; count++) {
            _writeData(lut_23_wb_partial[count]);
        }

        _writeCommand(0x24);              //bb b
        for (count = 0; count < 42; count++) {
            _writeData(lut_24_bb_partial[count]);
        }
    }
#else
    _writeCommand(0xD2);
    _writeData(0x3F);

    _writeCommand(0x00);
    _writeData (0x6F);  //from outside


    _writeCommand(0x01);             //power setting
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
    _writeData(0x13); //50Hz

    _writeCommand(0x50);
    _writeData(0xF2);

    _writeCommand(0x60);
    _writeData(0x22);

    _writeCommand(0x82);
    _writeData(0x00);//-0.1v

    _writeCommand(0xe3);
    _writeData(0x33);

    unsigned int count;
    {
        _writeCommand(0x23);              //wb w
        for (count = 0; count < 42; count++) {
            _writeData(lut_23_wb_partial[count]);
        }

        _writeCommand(0x24);              //bb b
        for (count = 0; count < 42; count++) {
            _writeData(lut_24_bb_partial[count]);
        }
    }

    _writeCommand(0x04);             //power on
    _waitWhileBusy("_Init_PartialUpdate");
#endif
}

void GxGDGDEW0102T4::_rotate(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
{
    switch (getRotation()) {
    case 1:
        swap(x, y);
        swap(w, h);
        x = GxGDGDEW0102T4_WIDTH - x - w - 1;
        break;
    case 2:
        x = GxGDGDEW0102T4_WIDTH - x - w - 1;
        y = GxGDGDEW0102T4_HEIGHT - y - h - 1;
        break;
    case 3:
        swap(x, y);
        swap(w, h);
        y = GxGDGDEW0102T4_HEIGHT - y - h - 1;
        break;
    }
}

void GxGDGDEW0102T4::drawCornerTest(uint8_t em)
{
    if (_current_page != -1) return;
    _using_partial_mode = false;
    _wakeUp();
    _writeCommand(0x10);
    for (uint32_t i = 0; i < GxGDGDEW0102T4_BUFFER_SIZE; i++) {
        _writeData(0xFF); // white is 0xFF on device
    }
    _writeCommand(0x13);
    for (uint32_t y = 0; y < GxGDGDEW0102T4_HEIGHT; y++) {
        for (uint32_t x = 0; x < GxGDGDEW0102T4_WIDTH / 8; x++) {
            uint8_t data = 0xFF; // white is 0xFF on device
            if ((x < 1) && (y < 8)) data = 0x00;
            if ((x > GxGDGDEW0102T4_WIDTH / 8 - 3) && (y < 16)) data = 0x00;
            if ((x > GxGDGDEW0102T4_WIDTH / 8 - 4) && (y > GxGDGDEW0102T4_HEIGHT - 25)) data = 0x00;
            if ((x < 4) && (y > GxGDGDEW0102T4_HEIGHT - 33)) data = 0x00;
            _writeData(data);
        }
    }
    _writeCommand(0x12); //display refresh
    _waitWhileBusy("drawCornerTest");
    _sleep();
}
