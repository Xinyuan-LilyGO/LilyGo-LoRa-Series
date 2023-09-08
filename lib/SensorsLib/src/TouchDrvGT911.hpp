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
 * @file      TouchDrvGT911.tpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-12
 *
 */


#include "REG/GT911Constants.h"
#include "SensorCommon.tpp"


typedef struct GT911_Struct {
    uint8_t trackID;
    int16_t x;
    int16_t y;
    int16_t size;
} GT911Point_t;

class TouchDrvGT911 :
    public SensorCommon<TouchDrvGT911>
{
    friend class SensorCommon<TouchDrvGT911>;
public:


#if defined(ARDUINO)
    TouchDrvGT911(TwoWire &w,
                  int sda = DEFAULT_SDA,
                  int scl = DEFAULT_SCL,
                  uint8_t addr = GT911_SLAVE_ADDRESS_H)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __rst = SENSOR_PIN_NONE;
        __irq = SENSOR_PIN_NONE;
        __addr = addr;
    }
#endif

    TouchDrvGT911()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __rst = SENSOR_PIN_NONE;
        __irq = SENSOR_PIN_NONE;
        __addr = GT911_SLAVE_ADDRESS_H;
    }

    ~TouchDrvGT911()
    {
        deinit();
    }

#if defined(ARDUINO)
    bool init(TwoWire &w,
              int sda = DEFAULT_SDA,
              int scl = DEFAULT_SCL,
              uint8_t addr = GT911_SLAVE_ADDRESS_H)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __addr = addr;
        return begin();
    }
#endif

    bool init()
    {
        return begin();
    }

    void deinit()
    {
        // end();
    }

    void setPins(int rst, int irq)
    {
        __irq = irq;
        __rst = rst;
    }

    void setIrqPin(int irq)
    {
        __irq = irq;
    }

    void setRstPin(int rst)
    {
        __rst = rst;
    }

    void reset()
    {
        writeRegister(GT911_COMMAND, 0x02);
    }

    void sleep()
    {
        writeRegister(GT911_COMMAND, 0x05);
    }


    uint8_t getPoint(int16_t *x, int16_t *y, uint8_t size = 1)
    {
        uint8_t touchedID;
        uint8_t buffer[39];

        if (x == NULL || y == NULL || size == 0)
            return DEV_WIRE_ERR;

        int touchPoint = readRegister(GT911_POINT_INFO);
        if (touchPoint == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }
        // Serial.printf("touchPoint:%d\n", touchPoint);
        touchPoint &= 0x0F;
        if (touchPoint == 0) {
            clearBuffer();
            return 0;
        }
        clearBuffer();

        if (readRegister(GT911_POINT_1, buffer, 39) == DEV_WIRE_ERR) {
            return DEV_WIRE_ERR;
        }


        //debug.
        GT911Point_t p[5];

        for (uint8_t i = 0; i < size; i++) {
            p[i].trackID = buffer[i * 8];
            p[i].x =  buffer[0x01 + i * 8] ;
            p[i].x |= (buffer[0x02 + i * 8] << 8 );
            p[i].y =  buffer[0x03 + i * 8] ;
            p[i].y |= (buffer[0x04 + i * 8] << 8);
            p[i].size = buffer[0x05 + i * 8] ;
            p[i].size |= (buffer[0x06 + i * 8] << 8) ;

            x[i] = p[i].x;
            y[i] = p[i].y;
        }

#ifdef LOG_PORT
        LOG_PORT.println("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
        LOG_PORT.println("Touched  [0]ID  [0]Size  [0]X   [0]Y  [1]ID  [1]Size   [1]X    [1]Y  [2]ID  [2]Size   [2]X    [2]Y  [3]ID  [3]Size  [3]X    [3]Y  [4]ID  [4]Size  [4]X    [4]Y  ");
        LOG_PORT.print(touchPoint); LOG_PORT.print("\t");
        for (int i = 0; i < size; ++i) {
            LOG_PORT.print(p[i].trackID); LOG_PORT.print("\t");
            LOG_PORT.print(p[i].size); LOG_PORT.print("\t");
            LOG_PORT.print( p[i].x); LOG_PORT.print("\t");
            LOG_PORT.print( p[i].y); LOG_PORT.print("\t");
        }
        LOG_PORT.println();
#endif

        return touchPoint;
    }


    bool getTouched()
    {
        if (__irq != SENSOR_PIN_NONE) {
            if (__irq_mode == FALLING) {
                return digitalRead(__irq) == LOW;
            } else if (__irq_mode == RISING ) {
                return digitalRead(__irq) == HIGH;
            }
        } else {
            uint8_t val = readRegister(GT911_POINT_INFO) & 0x0F;
            clearBuffer();
            return val & 0x0F;
        }
        return false;
    }

    //In the tested GT911 only the falling edge is valid to use, the rest are incorrect
    bool setInterruptMode(uint8_t mode)
    {
        int val = readRegister(GT911_MODULE_SWITCH_1);
        val &= 0XFC;
        if (mode == FALLING) {
            val |= 0x03;
        } else if (mode == RISING ) {
            val |= 0x02;
        }
        __irq_mode = mode;
        return writeRegister(GT911_MODULE_SWITCH_1, val) != -1;
    }


    uint8_t getPoint()
    {
        uint8_t val = readRegister(GT911_POINT_INFO) & 0x0F;
        clearBuffer();
        return val & 0x0F;
    }


    int getProductID()
    {
        char product_id[4] = {0};
        for (int i = 0; i < 4; ++i) {
            product_id[i] = readRegister(GT911_PRODUCT_ID + i);
        }
        return atoi(product_id);
    }

    uint16_t getFwVersion()
    {
        uint8_t fw_ver[2] = {0};
        for (int i = 0; i < 2; ++i) {
            fw_ver[i] = readRegister(GT911_FIRMWARE_VERSION + i);
        }
        return fw_ver[0] | (fw_ver[1] << 8);
    }

    void getResolution(int &x, int &y)
    {
        uint8_t x_resolution[2] = {0}, y_resolution[2] = {0};

        for (int i = 0; i < 2; ++i) {
            x_resolution[i] = readRegister(GT911_X_RESOLUTION + i);
        }
        for (int i = 0; i < 2; ++i) {
            y_resolution[i] = readRegister(GT911_Y_RESOLUTION + i);
        }

        x = x_resolution[0] | (x_resolution[1] << 8);
        y = y_resolution[0] | (y_resolution[1] << 8);

    }

    int getVendorID()
    {
        return readRegister(GT911_VENDOR_ID);
    }

    void setRsetUseCallback(bool enable)
    {
        __rst_use_cb = enable;
    }

private:

    void inline clearBuffer()
    {
        uint8_t val = 0x00;
        writeRegister(GT911_POINT_INFO, 0x00);
    }

    void setRstValue(uint8_t value)
    {
        if (__rst_use_cb) {
            thisDigitalWriteCallback(__rst, value);
        } else {
            digitalWrite(__rst, value);
        }
    }

    void setRstPinMode(uint8_t mode)
    {
        if (__rst_use_cb) {
            thisPinModeCallback(__rst, mode);
        } else {
            pinMode(__rst, mode);
        }
    }

    bool initImpl()
    {
        int x, y;

        // GT911 register address uses two bytes
        setRegAddressLenght(2);

        if (__addr == GT911_SLAVE_ADDRESS_H  &&
                __rst != SENSOR_PIN_NONE &&
                __irq != SENSOR_PIN_NONE) {

            LOG("GT911 using 0x28 address!\n");
            // pinMode(__rst, OUTPUT);
            setRstPinMode(OUTPUT);
            pinMode(__irq, OUTPUT);
            // digitalWrite(__rst, LOW);
            setRstValue(LOW);
            digitalWrite(__irq, HIGH);
            delayMicroseconds(120);
            setRstValue(HIGH);
            // digitalWrite(__rst, HIGH);
            delay(8);
            pinMode(__irq, INPUT);

        } else if (__addr == GT911_SLAVE_ADDRESS_L &&
                   __rst != SENSOR_PIN_NONE &&
                   __irq != SENSOR_PIN_NONE) {

            LOG("GT911 using 0xBA address!\n");

            // pinMode(__rst, OUTPUT);
            setRstPinMode(OUTPUT);
            pinMode(__irq, OUTPUT);
            digitalWrite(__irq, LOW);
            // digitalWrite(__rst, LOW);
            setRstValue(LOW);
            delayMicroseconds(120);
            // digitalWrite(__rst, HIGH);
            setRstValue(HIGH);
            delay(8);
            pinMode(__irq, INPUT);

        } else {
            reset();
        }


        LOG("Product id:%d\n", getProductID());

        if (getProductID() != 911) {
            LOG("Not find device GT911\n");
            return false;
        }
        LOG("Firmware version: 0x%x\n", getFwVersion());
        getResolution(x, y);
        LOG("Resolution : X = %d Y = %d\n", x, y);

        LOG("Vendor id:%d\n", getVendorID());


        return true;
    }

    int getReadMaskImpl()
    {
        return -1;
    }


protected:
    int __rst;
    int __irq;
    int __irq_mode;
    bool __rst_use_cb = false;
};



