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
 * @file      TouchDrvCHSC5816.tpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-12
 *
 */


#include "REG/CHSC5816Constants.h"
#include "TouchDrvInterface.hpp"
#include "SensorCommon.tpp"

typedef struct __CHSC5816_Header {
    uint16_t fw_ver;
    uint16_t checksum;
    uint32_t sig;
    uint32_t vid_pid;
    uint16_t raw_offet;
    uint16_t dif_offet;
} CHSC5816_Header_t;

union __CHSC5816_PointReg {
    struct {
        uint8_t status;
        uint8_t fingerNumber;
        uint8_t x_l8;
        uint8_t y_l8;
        uint8_t z;
        uint8_t x_h4: 4;
        uint8_t y_h4: 4;
        uint8_t id: 4;
        uint8_t event: 4;
        uint8_t p2;
    } rp;
    unsigned char data[8];
};

class TouchDrvCHSC5816 :
    public TouchDrvInterface,
    public SensorCommon<TouchDrvCHSC5816>
{
    friend class SensorCommon<TouchDrvCHSC5816>;
public:


#if defined(ARDUINO)
    TouchDrvCHSC5816(TwoWire &w,
                     int sda = DEFAULT_SDA,
                     int scl = DEFAULT_SCL,
                     uint8_t addr = CHSC5816_SLAVE_ADDRESS)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __rst = SENSOR_PIN_NONE;
        __irq = SENSOR_PIN_NONE;
        __addr = addr;
    }
#endif

    TouchDrvCHSC5816()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __rst = SENSOR_PIN_NONE;
        __irq = SENSOR_PIN_NONE;
        __addr = CHSC5816_SLAVE_ADDRESS;
    }

    ~TouchDrvCHSC5816()
    {
        // deinit();
    }

#if defined(ARDUINO)
    bool init(TwoWire &w,
              int sda = DEFAULT_SDA,
              int scl = DEFAULT_SCL,
              uint8_t addr = CHSC5816_SLAVE_ADDRESS)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __addr = addr;
        return begin();
    }
#endif

    bool init(int rst, int irq)
    {
        __rst = rst;
        __irq = irq;
        return initImpl();
    }

    void setPins(int rst, int irq)
    {
        __irq = irq;
        __rst = rst;
    }

    void reset()
    {
        if (__rst != SENSOR_PIN_NONE) {
            digitalWrite(__rst, LOW);
            delay(3);
            digitalWrite(__rst, HIGH);
            delay(5);
        }
    }



    uint8_t getPoint(int16_t *x_array, int16_t *y_array, uint8_t get_point = 1)
    {
        __CHSC5816_PointReg touch;
        readRegister(CHSC5816_REG_POINT, touch.data, 8);
        if (touch.rp.status == 0xFF && touch.rp.fingerNumber == 0) {
            return 0;
        }
        if (x_array) {
            *x_array = (unsigned int)(touch.rp.x_h4 << 8) | touch.rp.x_l8;
        }
        if (y_array) {
            *y_array = (unsigned int)(touch.rp.y_h4 << 8) | touch.rp.y_l8;
        }
        return 1;
    }

    bool isPressed()
    {
        if (__irq != SENSOR_PIN_NONE) {
            return digitalRead(__irq) == LOW;
        }
        return getPoint(NULL, NULL);
    }

    bool enableInterrupt()
    {
        return false;
    }

    bool disableInterrupt()
    {
        return false;
    }

    uint8_t getChipID()
    {
        return false;
    }

    const char *getModelName()
    {
        return "CHSC5816";
    }

    //2uA
    void sleep()
    {
        uint8_t buffer[16] = {
            0xFB, 0x16, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE9
        };
        writeRegister(CHSC5816_REG_CMD_BUFF, buffer, 16);
    }

    void wakeup()
    {
        //getGesture wakup ?
        // uint8_t buffer[16] = {
        //     0xFB, 0x16, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
        //     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE9
        // };
        // writeRegister(CHSC5816_REG_CMD_BUFF, buffer, 16);
    }

    void idle()
    {
        uint8_t buffer[16] = {
            0x20, 0x16, 0x02, 0x00, 0xDB, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE9
        };
        writeRegister(CHSC5816_REG_CMD_BUFF, buffer, 16);
    }

    bool writeConfig(uint8_t *data, uint32_t size)
    {
        return false;
    }

    uint8_t getSupportTouchPoint()
    {
        return 1;
    }

    bool getResolution(int16_t *x, int16_t *y)
    {
        uint8_t buffer[16] = {
            0xFC, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xe9
        };
        writeRegister(CHSC5816_REG_CMD_BUFF, buffer, 16);
        memset(buffer, 0, 16);
        readRegister(CHSC5816_REG_CMD_BUFF, buffer, 16);
        for (int i = 0; i < 16; ++i) {
            Serial.print(buffer[i], HEX);
            Serial.print(",");
        }
        Serial.println();
        return true;
    }

    uint8_t getGesture()
    {
        return 0;
    }

private:
    bool checkOnline()
    {
        CHSC5816_Header_t tmp;
        memset(&tmp, 0, sizeof(CHSC5816_Header_t));
        memset(&__header, 0, sizeof(CHSC5816_Header_t));

        uint32_t bootClean = 0x00;
        writeRegister(CHSC5816_REG_BOOT_STATE, (uint8_t *)&bootClean, 4);

        reset();

        for (int i = 0; i < 10; ++i) {
            delay(10);
            readRegister(CHSC5816_REG_IMG_HEAD, (uint8_t *)&__header, sizeof(CHSC5816_Header_t));
            readRegister(CHSC5816_REG_IMG_HEAD, (uint8_t *)&tmp, sizeof(CHSC5816_Header_t));
            if (memcmp(&tmp, &__header, sizeof(CHSC5816_Header_t)) != 0 ) {
                continue;
            }
            if (__header.sig == CHSC5816_SIG_VALUE) {
                return true;
            }
        }
        return false;
    }

    bool initImpl()
    {
        setRegAddressLenght(4);

        if (__irq != SENSOR_PIN_NONE) {
            pinMode(__irq, INPUT);
        }

        if (__rst != SENSOR_PIN_NONE) {
            pinMode(__rst, OUTPUT);
        }

        reset();

        if (checkOnline()) {
            reset();
            return true;
        }

        return false;
    }

    int getReadMaskImpl()
    {
        return -1;
    }


protected:
    int __rst = SENSOR_PIN_NONE;
    int __irq = SENSOR_PIN_NONE;
    CHSC5816_Header_t __header;
};



