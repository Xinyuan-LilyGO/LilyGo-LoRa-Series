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
 * @file      SensorCM32181.tpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-14
 *
 */


#include "REG/CM32181Constants.h"
#include "SensorCommon.tpp"

enum sensor_sampling {
    SAMPLING_X1,
    SAMPLING_X2,
    SAMPLING_X1_8,
    SAMPLING_X1_4,
};

enum sensor_integration_time {
    INTEGRATION_TIME_100MS,
    INTEGRATION_TIME_200MS,
    INTEGRATION_TIME_400MS,
    INTEGRATION_TIME_800MS,
    INTEGRATION_TIME_25MS = 6,
    INTEGRATION_TIME_50MS = 8,
};

class SensorCM32181 :
    public SensorCommon<SensorCM32181>
{
    friend class SensorCommon<SensorCM32181>;
public:

#if defined(ARDUINO)
    SensorCM32181(TwoWire &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = CM32181_SLAVE_ADDRESS)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __addr = addr;
    }
#endif

    SensorCM32181()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __addr = CM32181_SLAVE_ADDRESS;
    }

    ~SensorCM32181()
    {
        deinit();
    }

#if defined(ARDUINO)
    bool init(TwoWire &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = CM32181_SLAVE_ADDRESS)
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

    void setSampling(sensor_sampling tempSampling  = SAMPLING_X1,
                     sensor_integration_time int_time = INTEGRATION_TIME_200MS,
                     bool enable_interrupt = false)
    {

        uint8_t buffer[2] = {0};
        uint16_t value = (tempSampling << 11);
        value |= (int_time << 6);
        value |= enable_interrupt;
        buffer[0] = (value >> 8) & 0xFF;
        buffer[1] = (value) & 0xFF;
        writeRegister(CM32181_REG_ALS_CONF, buffer, 2);
    }


    //todo:
    void setIntThreshold(uint16_t high_threshold, uint16_t low_threshold)
    {
        uint8_t buffer[2] = {0};
        buffer[1] = (high_threshold >> 8) & 0xFF;
        buffer[0] = (high_threshold) & 0xFF;
        writeRegister(CM32181_REG_ALS_THDH, buffer, 2);
        buffer[1] = (low_threshold >> 8) & 0xFF;
        buffer[0] = (low_threshold) & 0xFF;
        writeRegister(CM32181_REG_ALS_THDL, buffer, 2);
    }

    // Read CM32181_REG_ALS_STATUS register to clear interrupt
    uint8_t getInterruptStatus()
    {
        uint8_t buffer[2];
        readRegister(CM32181_REG_ALS_STATUS, buffer, 2);
        return (buffer[1] >> 6) ;
    }

    void enableIRQ()
    {
        setRegisterBit(CM32181_REG_ALS_CONF, 1);
    }

    void disableIRQ()
    {
        clrRegisterBit(CM32181_REG_ALS_CONF, 1);
    }

    void powerOn()
    {
        clrRegisterBit(CM32181_REG_ALS_CONF, 0);
    }

    void powerDown()
    {
        setRegisterBit(CM32181_REG_ALS_CONF, 1);
    }

    uint16_t getRaw()
    {
        uint8_t buffer[2] = {0};
        readRegister(CM32181_REG_ALS_DATA, buffer, 2);
        return (uint16_t) buffer[0] | (uint16_t)(buffer[1] << 8);
    }

    float getLux()
    {
        return getRaw() * calibration_factor;
    }

    int getChipID()
    {
        uint8_t buffer[2] = {0};
        readRegister(CM32181_REG_ID, buffer, 2);
        return  buffer[0] & 0xFF;
    }

private:

    bool initImpl()
    {
        setReadRegisterSendStop(false);

        int chipID = getChipID();
        Serial.printf("chipID:%d\n", chipID);
        if (chipID == DEV_WIRE_ERR  ) {
            return false;
        }
        if (chipID != CM32181_CHIP_ID) {
            return false;
        }
        return true;
    }

    int getReadMaskImpl()
    {
        return -1;
    }


protected:
    // The default calibration value, learned from the manual,
    // is now unable to obtain the calibration value from the specified register
    const float calibration_factor = 0.286;
};



