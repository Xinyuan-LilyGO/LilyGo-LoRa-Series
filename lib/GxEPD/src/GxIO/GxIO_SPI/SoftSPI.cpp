/**
 * @file      SoftSPI.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022
 * @date      2022-05-11
 *
 */

#include "SoftSPI.h"

SoftSPI::SoftSPI(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    _mosi = mosi;
    _miso = miso;
    _sck = sck;
    _delay = 2;
    _cke = 0;
    _ckp = 0;
    _order = MSBFIRST;
}

void SoftSPI::begin()
{
    if (_miso !=  -1) {
        pinMode(_miso, INPUT);
    }
    pinMode(_mosi, OUTPUT);
    pinMode(_sck, OUTPUT);
}

void SoftSPI::end()
{
    if (_miso !=  -1) {
        pinMode(_miso, INPUT);
    }
    pinMode(_mosi, INPUT);
    pinMode(_sck, INPUT);
}

void SoftSPI::setBitOrder(uint8_t order)
{
    _order = order & 1;
}

void SoftSPI::setDataMode(uint8_t mode)
{
    switch (mode) {
    case SPI_MODE0:
        _ckp = 0;
        _cke = 0;
        break;
    case SPI_MODE1:
        _ckp = 0;
        _cke = 1;
        break;
    case SPI_MODE2:
        _ckp = 1;
        _cke = 0;
        break;
    case SPI_MODE3:
        _ckp = 1;
        _cke = 1;
        break;
    }
    digitalWrite(_sck, _ckp ? HIGH : LOW);
}



void SoftSPI::setClockDivider(uint32_t div)
{
    switch (div) {
    case SPI_CLOCK_DIV2:
        _delay = 2;
        break;
    case SPI_CLOCK_DIV4:
        _delay = 4;
        break;
    case SPI_CLOCK_DIV8:
        _delay = 8;
        break;
    case SPI_CLOCK_DIV16:
        _delay = 16;
        break;
    case SPI_CLOCK_DIV32:
        _delay = 32;
        break;
    case SPI_CLOCK_DIV64:
        _delay = 64;
        break;
    case SPI_CLOCK_DIV128:
        _delay = 128;
        break;
    default:
        _delay = 128;
        break;
    }
}

void SoftSPI::wait(uint_fast8_t del)
{
    for (uint_fast8_t i = 0; i < del; i++) {
        asm volatile("nop");
    }
}

uint8_t SoftSPI::transfer(uint8_t val)
{
    uint8_t out = 0;
    if (_order == MSBFIRST) {
        uint8_t v2 =
            ((val & 0x01) << 7) |
            ((val & 0x02) << 5) |
            ((val & 0x04) << 3) |
            ((val & 0x08) << 1) |
            ((val & 0x10) >> 1) |
            ((val & 0x20) >> 3) |
            ((val & 0x40) >> 5) |
            ((val & 0x80) >> 7);
        val = v2;
    }

    uint8_t del = _delay >> 1;

    uint8_t bval = 0;
    /*
     * CPOL := 0, CPHA := 0 => INIT = 0, PRE = Z|0, MID = 1, POST =  0
     * CPOL := 1, CPHA := 0 => INIT = 1, PRE = Z|1, MID = 0, POST =  1
     * CPOL := 0, CPHA := 1 => INIT = 0, PRE =  1 , MID = 0, POST = Z|0
     * CPOL := 1, CPHA := 1 => INIT = 1, PRE =  0 , MID = 1, POST = Z|1
     */

    int sck = (_ckp) ? HIGH : LOW;

    for (uint8_t bit = 0u; bit < 8u; bit++) {
        if (_cke) {
            sck ^= 1;
            digitalWrite(_sck, sck);
            wait(del);
        }

        /* ... Write bit */
        digitalWrite(_mosi, ((val & (1 << bit)) ? HIGH : LOW));

        wait(del);

        sck ^= 1u; digitalWrite(_sck, sck);

        if (_miso != -1) {
            if (digitalRead(_miso)) {
                bval = digitalRead(_miso);

                if (_order == MSBFIRST) {
                    out <<= 1;
                    out |= bval;
                } else {
                    out >>= 1;
                    out |= bval << 7;
                }
            }
        }
        wait(del);

        if (!_cke) {
            sck ^= 1u;
            digitalWrite(_sck, sck);
        }
    }
    return out;
}

uint16_t SoftSPI::transfer16(uint16_t data)
{
    union {
        uint16_t val;
        struct {
            uint8_t lsb;
            uint8_t msb;
        };
    } in, out;
    in.val = data;
    if ( _order == MSBFIRST ) {
        out.msb = transfer(in.msb);
        out.lsb = transfer(in.lsb);
    } else {
        out.lsb = transfer(in.lsb);
        out.msb = transfer(in.msb);
    }
    return out.val;
}

void SoftSPI::transfer(void *buf, size_t count)
{
    uint8_t *buffer = (uint8_t *) buf;
    for (size_t i = 0; i < count; i++) {
        buffer[i] = transfer(buffer[i]);
    }
}



void SoftSPI::writeBytes(const uint8_t *data, uint32_t size)
{
    transfer((void *)data, size);

}
void SoftSPI::writePattern(const uint8_t *data, uint8_t size, uint32_t repeat)
{
    log_i("writePattern:%u", size);
}
