/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      SensorCommon.tpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2022-10-16
 *
 */


#pragma once

#if defined(ARDUINO)
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#endif

#define SENSOR_PIN_NONE     (-1)
#define DEV_WIRE_NONE       (0)
#define DEV_WIRE_ERR        (-1)
#define DEV_WIRE_TIMEOUT    (-2)

#ifdef _BV
#undef _BV
#endif
#define _BV(b)                          (1UL << (uint32_t)(b))

// #define LOG_PORT Serial
#ifdef LOG_PORT
#define LOG(fmt, ...) LOG_PORT.printf("[%s] " fmt "\n", __func__, ##__VA_ARGS__)
#define LOG_BIN(x)    LOG_PORT.println(x,BIN);
#else
#define LOG(fmt, ...)
#define LOG_BIN(x)
#endif


#define SENSORLIB_ATTR_NOT_IMPLEMENTED    __attribute__((error("Not implemented")))

#if defined(NRF52840_XXAA) || defined(NRF52832_XXAA)
#define SPI_DATA_ORDER  MSBFIRST
#define DEFAULT_SDA     (0xFF)
#define DEFAULT_SCL     (0xFF)
#define DEFAULT_SPISETTING  SPISettings()
#elif defined(ARDUINO_ARCH_RP2040)
#define SPI_DATA_ORDER  SPI_MSB_FIRST
#define DEFAULT_SDA     (0xFF)
#define DEFAULT_SCL     (0xFF)
#define DEFAULT_SPISETTING  SPISettings()
#else   //esp32
#define SPI_DATA_ORDER  SPI_MSBFIRST
#define DEFAULT_SDA     (SDA)
#define DEFAULT_SCL     (SCL)
#define DEFAULT_SPISETTING  SPISettings(__freq, __dataOrder, __dataMode);
#endif

#ifndef ESP32
#ifndef log_e
#define log_e(...)          Serial.printf(__VA_ARGS__)
#endif
#ifndef log_i
#define log_i(...)          Serial.printf(__VA_ARGS__)
#endif
#ifndef log_d
#define log_d(...)          Serial.printf(__VA_ARGS__)
#endif
#endif

#ifndef INPUT
#define INPUT                 (0x0)
#endif

#ifndef OUTPUT
#define OUTPUT                (0x1)
#endif

#ifndef RISING
#define RISING                (0x01)
#endif

#ifndef FALLING
#define FALLING               (0x02)
#endif


template <class chipType>
class SensorCommon
{
    typedef int (*iic_fptr_t)(uint8_t devAddr, int regAddr, uint8_t *data, uint8_t len);
    typedef int (*digitalReadCb_t)(uint32_t pinNumber);
    typedef void (*digitalWirteCb_t)(uint32_t pinNumber, uint8_t value);
    typedef void (*pinModeCb_t)(uint32_t pinNumber, uint8_t mode);

public:
    ~SensorCommon()
    {
#if defined(ARDUINO)
        if (__spiSetting) {
            delete __spiSetting;
        }
#endif
    }

    void setSpiSetting(uint32_t freq, uint8_t dataOrder = SPI_DATA_ORDER, uint8_t dataMode = SPI_MODE0)
    {
        __freq = freq;
        __dataOrder = dataOrder;
        __dataMode = dataMode;
    }

#if defined(ARDUINO)
    bool begin(TwoWire &w, uint8_t addr, int sda, int scl)
    {
        LOG("Using Wire interface.\n");
        if (__has_init)return thisChip().initImpl();
        __wire = &w;
        __sda = sda;
        __scl = scl;
#if defined(NRF52840_XXAA) || defined(NRF52832_XXAA)
        __wire->begin();
#elif defined(ARDUINO_ARCH_RP2040)
        __wire->end();
        __wire->setSDA(__sda);
        __wire->setSCL(__scl);
        __wire->begin();
#else
        __wire->begin(__sda, __scl);
#endif
        __addr = addr;
        __spi = NULL;
        thisReadRegCallback = NULL;
        thisWriteRegCallback = NULL;
        __has_init = thisChip().initImpl();
        return __has_init;
    }

    bool begin(int cs, int mosi = -1, int miso = -1, int sck = -1,
#if defined(ARDUINO_ARCH_RP2040)
               SPIClassRP2040 &spi = SPI
#else
               SPIClass &spi = SPI
#endif
              )
    {
        LOG("Using SPI interface.\n");
        if (__has_init)return thisChip().initImpl();
        __cs  = cs;
        __spi = &spi;
        pinMode(__cs, OUTPUT);
        digitalWrite(__cs, HIGH);
        __spiSetting = new  DEFAULT_SPISETTING;
        if (!__spiSetting) {
            return false;
        }
        if (mosi != -1 && miso != -1 && sck != -1) {
#if defined(NRF52840_XXAA) || defined(NRF52832_XXAA)
            __spi->begin();
#elif defined(ARDUINO_ARCH_RP2040)
            __spi->setSCK(sck);
            __spi->setRX(miso);
            __spi->setTX(mosi);
            __spi->begin();
#else
            __spi->begin(sck, miso, mosi);
#endif
        } else {
            __spi->begin();
        }
        __wire = NULL;
        __readMask = thisChip().getReadMaskImpl();
        __has_init = thisChip().initImpl();
        return __has_init;
    }

#endif


    bool begin(uint8_t addr, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback)
    {
        LOG("Using Custom interface.\n");
        if (__has_init)return thisChip().initImpl();
        thisReadRegCallback = readRegCallback;
        thisWriteRegCallback = writeRegCallback;
        __addr = addr;
        __spi = NULL;
        __has_init = thisChip().initImpl();
        return __has_init;
    }

    void setDigitalWriteCallback(digitalWirteCb_t cb)
    {
        thisDigitalWriteCallback = cb;
    }

    void setDigitalReadCallback(digitalReadCb_t cb)
    {
        thisDigitalReadCallback = cb;
    }

    void setPinModeCallback(pinModeCb_t cb)
    {
        thisPinModeCallback = cb;
    }


protected:


    bool probe()
    {
#if defined(ARDUINO)
        if (__wire) {
            __wire->beginTransmission(__addr);
            return __wire->endTransmission() == 0;
        }
        return false;
#endif
        return true;
    }

    //! Write method
    int writeRegister(uint8_t reg, uint8_t norVal, uint8_t orVal)
    {
        int val = readRegister(reg);
        if (val == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }
        val &= norVal;
        val |= orVal;
        return writeRegister(reg, val);
    }

    int writeRegister(int reg, uint8_t val)
    {
        if (thisWriteRegCallback) {
            return thisWriteRegCallback(__addr, reg, &val, 1);
        }
#if defined(ARDUINO)
        if (__wire) {
            __wire->beginTransmission(__addr);
            if (__reg_addr_len == 1) {
                __wire->write(reg);
            } else {
                for (int i = 0; i < __reg_addr_len; ++i) {
                    __wire->write(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            __wire->write(val);
            return (__wire->endTransmission() == 0) ? DEV_WIRE_NONE : DEV_WIRE_ERR;
        }
        if (__spi) {
            uint8_t msb = reg >> 8;
            uint8_t lsb = reg & 0xFF;
            __spi->beginTransaction(*__spiSetting);
            digitalWrite(__cs, LOW);
            if (__reg_addr_len == 1) {
                __spi->transfer(reg);
            } else {
                for (int i = 0; i < __reg_addr_len; ++i) {
                    __spi->transfer(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            __spi->transfer(val);
            digitalWrite(__cs, HIGH);
            __spi->endTransaction();
            return DEV_WIRE_NONE;
        }
#endif
        return DEV_WIRE_ERR;
    }

    int writeRegister(int reg, uint8_t *buf, uint8_t length)
    {
        if (thisWriteRegCallback) {
            return thisWriteRegCallback(__addr, reg, buf, length);
        }
#if defined(ARDUINO)
        if (__wire) {
            __wire->beginTransmission(__addr);
            if (__reg_addr_len == 1) {
                __wire->write(reg);
            } else {
                for (int i = 0; i < __reg_addr_len; ++i) {
                    __wire->write(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            __wire->write(buf, length);
            return (__wire->endTransmission() == 0) ? 0 : DEV_WIRE_ERR;
        }
        if (__spi) {
            __spi->beginTransaction(*__spiSetting);
            digitalWrite(__cs, LOW);
            if (__reg_addr_len == 1) {
                __spi->transfer(reg);
            } else {
                for (int i = 0; i < __reg_addr_len; ++i) {
                    __spi->transfer(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            __spi->transfer(buf, length);
            digitalWrite(__cs, HIGH);
            __spi->endTransaction();
            return DEV_WIRE_NONE;
        }
#endif
        return DEV_WIRE_ERR;
    }


    //! Read method
    int readRegister(int reg)
    {
        uint8_t val = 0;
        if (thisReadRegCallback) {
            if (thisReadRegCallback(__addr, reg, &val, 1) != 0) {
                return DEV_WIRE_NONE;
            }
            return val;
        }
#if defined(ARDUINO)
        if (__wire) {
            __wire->beginTransmission(__addr);
            if (__reg_addr_len == 1) {
                __wire->write(reg);
            } else {
                for (int i = 0; i < __reg_addr_len; ++i) {
                    __wire->write(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            if (__wire->endTransmission(__sendStop) != 0) {
                LOG("I2C Transfer Error!\n");
                return DEV_WIRE_ERR;
            }
            __wire->requestFrom(__addr, 1U, false);
            return __wire->read();
        }
        if (__spi) {
            uint8_t  data = 0x00;
            __spi->beginTransaction(*__spiSetting);
            digitalWrite(__cs, LOW);
            if (__reg_addr_len == 1) {
                __spi->transfer(__readMask != -1 ? (reg  | __readMask) : reg);
            } else {
                uint8_t firstBytes = reg >> (8 * ((__reg_addr_len - 1)));
                __spi->transfer(__readMask != -1 ? (firstBytes  | __readMask) : firstBytes);
                for (int i = 1; i < __reg_addr_len; ++i) {
                    __spi->transfer(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            data = __spi->transfer(0x00);
            digitalWrite(__cs, HIGH);
            __spi->endTransaction();
            return data;
        }
#endif
        return DEV_WIRE_ERR;
    }

    int readRegister(int reg, uint8_t *buf, uint8_t length)
    {
        if (thisReadRegCallback) {
            return thisReadRegCallback(__addr, reg, buf, length);
        }
#if defined(ARDUINO)
        if (__wire) {
            __wire->beginTransmission(__addr);
            if (__reg_addr_len == 1) {
                __wire->write(reg);
            } else {
                for (int i = 0; i < __reg_addr_len; ++i) {
                    __wire->write(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            if (__wire->endTransmission(__sendStop) != 0) {
                return DEV_WIRE_ERR;
            }
            __wire->requestFrom(__addr, length);
            return __wire->readBytes(buf, length) == length ? DEV_WIRE_NONE : DEV_WIRE_ERR;
        }
        if (__spi) {
            __spi->beginTransaction(*__spiSetting);
            digitalWrite(__cs, LOW);
            if (__reg_addr_len == 1) {
                __spi->transfer(__readMask != -1 ? (reg  | __readMask) : reg);
            } else {
                uint8_t firstBytes = reg >> (8 * ((__reg_addr_len - 1)));
                __spi->transfer(__readMask != -1 ? (firstBytes  | __readMask) : firstBytes);
                for (int i = 1; i < __reg_addr_len; ++i) {
                    __spi->transfer(reg >> (8 * ((__reg_addr_len - 1) - i)));
                }
            }
            for (size_t i = 0; i < length; i++) {
                buf[i] = __spi->transfer(0x00);
            }
            digitalWrite(__cs, HIGH);
            __spi->endTransaction();
            return DEV_WIRE_NONE;
        }
#endif
        return DEV_WIRE_ERR;
    }

    bool inline clrRegisterBit(int registers, uint8_t bit)
    {
        int val = readRegister(registers);
        if (val == DEV_WIRE_ERR) {
            return false;
        }
        return  writeRegister(registers, (val & (~_BV(bit)))) == 0;
    }

    bool inline setRegisterBit(int registers, uint8_t bit)
    {
        int val = readRegister(registers);
        if (val == DEV_WIRE_ERR) {
            return false;
        }
        return  writeRegister(registers, (val | (_BV(bit)))) == 0;
    }

    bool inline getRegisterBit(int registers, uint8_t bit)
    {
        int val = readRegister(registers);
        if (val == DEV_WIRE_ERR) {
            return false;
        }
        return val & _BV(bit);
    }

    uint16_t inline readRegisterH8L4(uint8_t highReg, uint8_t lowReg)
    {
        int h8 = readRegister(highReg);
        int l4 = readRegister(lowReg);
        if (h8 == DEV_WIRE_ERR || l4 == DEV_WIRE_ERR)return 0;
        return (h8 << 4) | (l4 & 0x0F);
    }

    uint16_t inline readRegisterH8L5(uint8_t highReg, uint8_t lowReg)
    {
        int h8 = readRegister(highReg);
        int l5 = readRegister(lowReg);
        if (h8 == DEV_WIRE_ERR || l5 == DEV_WIRE_ERR)return 0;
        return (h8 << 5) | (l5 & 0x1F);
    }

    uint16_t inline readRegisterH6L8(uint8_t highReg, uint8_t lowReg)
    {
        int h6 = readRegister(highReg);
        int l8 = readRegister(lowReg);
        if (h6 == DEV_WIRE_ERR || l8 == DEV_WIRE_ERR)return 0;
        return ((h6 & 0x3F) << 8) | l8;
    }

    uint16_t inline readRegisterH5L8(uint8_t highReg, uint8_t lowReg)
    {
        int h5 = readRegister(highReg);
        int l8 = readRegister(lowReg);
        if (h5 == DEV_WIRE_ERR || l8 == DEV_WIRE_ERR)return 0;
        return ((h5 & 0x1F) << 8) | l8;
    }

    void setRegAddressLenght(uint8_t len)
    {
        __reg_addr_len = len;
    }


    void setReadRegisterSendStop(bool sendStop)
    {
        __sendStop = sendStop;
    }



    /*
     * CRTP Helper
     */
protected:

    bool begin()
    {
#if defined(ARDUINO)
        if (__has_init) return thisChip().initImpl();
        __has_init = true;

        if (__wire) {
            log_i("SDA:%d SCL:%d", __sda, __scl);
#if defined(NRF52840_XXAA) || defined(NRF52832_XXAA)
            __wire->begin();
#elif defined(ARDUINO_ARCH_RP2040)
            __wire->end();
            __wire->setSDA(__sda);
            __wire->setSCL(__scl);
            __wire->begin();
#else
            __wire->begin(__sda, __scl);
#endif
        }
        if (__spi) {
            // int cs, int mosi = -1, int miso = -1, int sck = -1, SPIClass &spi = SPI
            begin(__cs, __mosi, __miso, __sck, *__spi);
        }

#endif  /*ARDUINO*/
        return thisChip().initImpl();
    }

    void end()
    {
#if defined(ARDUINO)
        if (__wire) {
#if defined(ESP_IDF_VERSION)
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4,4,0)
            __wire->end();
#endif  /*ESP_IDF_VERSION*/
#endif  /*ESP_IDF_VERSION*/
        }
#endif /*ARDUINO*/
    }


    inline const chipType &thisChip() const
    {
        return static_cast<const chipType &>(*this);
    }

    inline chipType &thisChip()
    {
        return static_cast<chipType &>(*this);
    }

protected:
    bool                __has_init              = false;
#if defined(ARDUINO)
    TwoWire             *__wire                 = NULL;
#if defined(ARDUINO_ARCH_RP2040)
    SPIClassRP2040      *__spi                  = NULL;
#else
    SPIClass            *__spi                  = NULL;
#endif

    SPISettings          *__spiSetting           = NULL;
#endif
    uint32_t            __freq                  = 1000000;
    uint8_t             __dataOrder             = SPI_DATA_ORDER;
    uint8_t             __dataMode              = SPI_MODE0;
    int                 __readMask              = -1;
    int                 __sda                   = -1;
    int                 __scl                   = -1;
    int                 __cs                    = -1;
    int                 __miso                  = -1;
    int                 __mosi                  = -1;
    int                 __sck                   = -1;
    bool                __sendStop              = true;
    uint8_t             __addr                  = 0xFF;
    uint8_t             __reg_addr_len          = 1;
    iic_fptr_t          thisReadRegCallback     = NULL;
    iic_fptr_t          thisWriteRegCallback    = NULL;
    digitalWirteCb_t    thisDigitalWriteCallback = NULL;
    digitalReadCb_t     thisDigitalReadCallback  = NULL;
    pinModeCb_t         thisPinModeCallback = NULL;
};
