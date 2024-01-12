// created by Jean-Marc Zingg to be the GxIO_SPI io class for the GxEPD library
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//

#include "GxIO_SPI.h"

#if defined(PARTICLE)
// transfer16 is not used by GxEPD
#define transfer16 transfer
#endif

GxIO_SPI::GxIO_SPI(SPIClass &spi, int8_t cs, int8_t dc, int8_t rst, int8_t bl) :
    _spi(spi), _spi_settings(4000000, MSBFIRST, SPI_MODE0),
    _cs(cs), _dc(dc), _rst(rst), _bl(bl), _soft_spi(NULL)
{
    _using_soft_spi = false;
}

GxIO_SPI::GxIO_SPI(int sck, int miso, int mosi,  int8_t cs, int8_t dc, int8_t rst, int8_t bl):
    _spi_settings(4000000, MSBFIRST, SPI_MODE0),
    _cs(cs), _dc(dc), _rst(rst), _bl(bl), _spi(SPI)
{
    _soft_spi = new  SoftSPI(sck, miso, mosi);
    _using_soft_spi = true;
}

void GxIO_SPI::reset()
{
    if (_rst >= 0) {
        delay(20);
        digitalWrite(_rst, LOW);
        delay(20);
        digitalWrite(_rst, HIGH);
        delay(200);
    }
}

void GxIO_SPI::init()
{
    if (_cs >= 0) {
        digitalWrite(_cs, HIGH);
        pinMode(_cs, OUTPUT);
    }
    if (_dc >= 0) {
        digitalWrite(_dc, HIGH);
        pinMode(_dc, OUTPUT);
    }
    if (_rst >= 0) {
        digitalWrite(_rst, HIGH);
        pinMode(_rst, OUTPUT);
    }
    if (_bl >= 0) {
        digitalWrite(_bl, HIGH);
        pinMode(_bl, OUTPUT);
    }
    reset();
    _using_soft_spi ? _soft_spi->begin() : _spi.begin();
}

void GxIO_SPI::setFrequency(uint32_t freq)
{
    if (_using_soft_spi)return;
    _spi_settings = SPISettings(freq, MSBFIRST, SPI_MODE0);
}

uint8_t GxIO_SPI::transferTransaction(uint8_t d)
{
    uint8_t rv = 0;

    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);
    if (_using_soft_spi) {
        rv = _soft_spi->transfer(d);
    } else {
        rv = _spi.transfer(d);
    }
    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
    return rv;
}

uint16_t GxIO_SPI::transfer16Transaction(uint16_t d)
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);
    uint16_t rv =   _using_soft_spi ? _soft_spi->transfer16(d) : _spi.transfer16(d);
    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
    return rv;
}

uint8_t GxIO_SPI::readDataTransaction()
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);
    uint16_t rv =   _using_soft_spi ? _soft_spi->transfer(0xFF) : _spi.transfer(0xFF);
    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
    return rv;
}

uint16_t GxIO_SPI::readData16Transaction()
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);
    uint16_t rv =   _using_soft_spi ? _soft_spi->transfer16(0xFFFF) : _spi.transfer16(0xFFFF);
    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
    return rv;
}

uint8_t GxIO_SPI::readData()
{
    return _using_soft_spi ? _soft_spi->transfer(0xFF) : _spi.transfer(0xFF);

}

uint16_t GxIO_SPI::readData16()
{
    return _using_soft_spi ? _soft_spi->transfer16(0xFFFF) : _spi.transfer16(0xFFFF);

}

void GxIO_SPI::writeCommandTransaction(uint8_t c)
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }    if (_dc >= 0) digitalWrite(_dc, LOW);
    if (_cs >= 0) digitalWrite(_cs, LOW);

    // _spi.transfer(c);
    _using_soft_spi ? _soft_spi->transfer(c) : _spi.transfer(c);

    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (_dc >= 0) digitalWrite(_dc, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
}

void GxIO_SPI::writeDataTransaction(uint8_t d)
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);

    // _spi.transfer(d);
    _using_soft_spi ? _soft_spi->transfer(d) : _spi.transfer(d);

    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
}

void GxIO_SPI::writeData16Transaction(uint16_t d, uint32_t num)
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);
    writeData16(d, num);
    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
}

void GxIO_SPI::writeCommand(uint8_t c)
{
    if (_dc >= 0) digitalWrite(_dc, LOW);
    _using_soft_spi ? _soft_spi->transfer(c) : _spi.transfer(c);
    if (_dc >= 0) digitalWrite(_dc, HIGH);
}

void GxIO_SPI::writeData(uint8_t d)
{
    _using_soft_spi ? _soft_spi->transfer(d) : _spi.transfer(d);
}

void GxIO_SPI::writeData(uint8_t *d, uint32_t num)
{
#if defined(ESP8266) || defined(ESP32)
    _using_soft_spi ? _soft_spi->writeBytes(d, num) : _spi.writeBytes(d, num);
#else
    while (num > 0) {
        _using_soft_spi ? _soft_spi->transfer(*d) : _spi.transfer(*d);
        d++;
        num--;
    }
#endif
}

void GxIO_SPI::writeData16(uint16_t d, uint32_t num)
{
#if defined(ESP8266) || defined(ESP32)
    uint8_t b[2] = {uint8_t(d >> 8), uint8_t(d)};
    _using_soft_spi ? _soft_spi->writePattern(b, 2, num) : _spi.writePattern(b, 2, num);
#else
    while (num > 0) {
        _using_soft_spi ? _soft_spi->transfer16(d) : _spi.transfer16(d);
        num--;
    }
#endif
}

void GxIO_SPI::writeAddrMSBfirst(uint16_t d)
{
    if (_using_soft_spi) {
        _soft_spi->transfer(d >> 8);
        _soft_spi->transfer(d & 0xFF);
    } else {
        _spi.transfer(d >> 8);
        _spi.transfer(d & 0xFF);
    }
}

void GxIO_SPI::startTransaction()
{
    if (!_using_soft_spi) {
        _spi.beginTransaction(_spi_settings);
    }
    if (_cs >= 0) digitalWrite(_cs, LOW);
}

void GxIO_SPI::endTransaction()
{
    if (_cs >= 0) digitalWrite(_cs, HIGH);
    if (!_using_soft_spi) {
        _spi.endTransaction();
    }
}

void GxIO_SPI::selectRegister(bool rs_low)
{
    if (_dc >= 0) digitalWrite(_dc, (rs_low ? LOW : HIGH));
}

void GxIO_SPI::setBackLight(bool lit)
{
    if (_bl >= 0) digitalWrite(_bl, (lit ? HIGH : LOW));
}
