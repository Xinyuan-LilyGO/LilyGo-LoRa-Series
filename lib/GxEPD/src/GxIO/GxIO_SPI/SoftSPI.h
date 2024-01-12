/**
 * @file      SoftSPI.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022
 * @date      2022-05-11
 *
 */

#ifndef _SOFTSPI_H
#define _SOFTSPI_H

# include <Arduino.h>

class SoftSPI
{
private:
    void wait(uint_fast8_t del);

private:
    uint8_t _cke;
    uint8_t _ckp;
    uint8_t _delay;
    uint8_t _miso;
    uint8_t _mosi;
    uint8_t _sck;
    uint8_t _order;

public:
    SoftSPI(int8_t sck, int8_t miso, int8_t mosi, int8_t ss = -1);
    void begin();
    void end();
    void setBitOrder(uint8_t);
    void setDataMode(uint8_t);
    void setClockDivider(uint32_t);
    void writeBytes(const uint8_t *data, uint32_t size);
    void writePattern(const uint8_t *data, uint8_t size, uint32_t repeat);
    uint8_t transfer(uint8_t);
    uint16_t transfer16(uint16_t data);
    void transfer(void *buf, size_t count);
};

#endif
