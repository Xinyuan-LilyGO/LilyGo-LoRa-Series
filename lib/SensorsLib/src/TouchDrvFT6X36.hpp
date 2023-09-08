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
 * @file      TouchDrvFT6X36.tpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-01
 *
 */


#include "REG/FT6X36Constants.h"
#include "SensorCommon.tpp"

class TouchDrvFT6X36 :
    public SensorCommon<TouchDrvFT6X36>
{
    friend class SensorCommon<TouchDrvFT6X36>;
public:


    enum GesTrue {
        NO_GESTRUE,
        MOVE_UP,
        MOVE_LEFT,
        MOVE_DOWN,
        MOVE_RIGHT,
        ZOOM_IN,
        ZOOM_OUT,
    } ;

    enum EventFlag {
        EVENT_PUT_DOWN,
        EVENT_PUT_UP,
        EVENT_CONTACT,
        EVENT_NONE,
    } ;

    enum PowerMode {
        PMODE_ACTIVE = 0,         // ~4mA
        PMODE_MONITOR = 1,        // ~3mA
        PMODE_DEEPSLEEP = 3,      // ~100uA  The reset pin must be pulled down to wake up
    } ;

    EventFlag event;

#if defined(ARDUINO)
    TouchDrvFT6X36(TwoWire &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = FT6X36_SLAVE_ADDRESS)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __addr = addr;
    }
#endif

    TouchDrvFT6X36()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __addr = FT6X36_SLAVE_ADDRESS;
    }

    ~TouchDrvFT6X36()
    {
        deinit();
    }

#if defined(ARDUINO)
    bool init(TwoWire &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = FT6X36_SLAVE_ADDRESS)
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

    uint8_t getDeviceMode(void)
    {
        return readRegister(FT6X36_REG_MODE) & 0x03;
    }

    // Obtaining gestures depends on whether the built-in firmware of the chip has this function
    GesTrue getGesture(void)
    {
        int val = readRegister(FT6X36_REG_GEST);
        switch (val) {
        case 0x10:
            return MOVE_UP;
        case 0x14:
            return MOVE_RIGHT;
        case 0x18:
            return MOVE_DOWN;
        case 0x1C:
            return MOVE_LEFT;
        case 0x48:
            return ZOOM_IN;
        case 0x49:
            return ZOOM_OUT;
        default:
            break;
        }
        return NO_GESTRUE;
    }

    void setTheshold(uint8_t value)
    {
        writeRegister(FT6X36_REG_THRESHHOLD, value);
    }

    uint8_t getThreshold(void)
    {
        return readRegister(FT6X36_REG_THRESHHOLD);
    }

    uint8_t getMonitorTime(void)
    {
        return readRegister(FT6X36_REG_MONITORTIME);
    }

    void setMonitorTime(uint8_t sec)
    {
        writeRegister(FT6X36_REG_MONITORTIME, sec);
    }

    // Calibration useless actually,
    // any value  set will not be valid,
    // depending on the internal firmware of the chip.
    /*
    void enableAutoCalibration(void)
    {
        writeRegister(FT6X36_REG_AUTO_CLB_MODE, 0x00);
    }

    void disableAutoCalibration(void)
    {
        writeRegister(FT6X36_REG_AUTO_CLB_MODE, 0xFF);
    }
    */

    uint16_t getLibraryVersion()
    {
        uint8_t buffer[2];
        readRegister(FT6X36_REG_LIB_VERSIONH, buffer, 2);
        return (buffer[0] << 8) | buffer[1];
    }

    // The interrupt is triggered only if a touch is detected during the scan cycle
    void interruptPolling(void)
    {
        //datasheet this bit is 0,Actually, it's wrong
        writeRegister(FT6X36_REG_INT_STATUS, 1);
    }

    // Triggers an interrupt whenever a touch is detected
    void interruptTrigger(void)
    {
        //datasheet this bit is 1,Actually, it's wrong
        writeRegister(FT6X36_REG_INT_STATUS, 0);
    }

    uint8_t getPoint(int16_t *x, int16_t *y, uint8_t size = 1)
    {
        uint8_t touchedID;
        uint8_t buffer[16];

        if (x == NULL || y == NULL || size == 0)
            return 0;

        if (readRegister(FT6X36_REG_MODE, buffer, 16) == -1) {
            return false;
        }

        uint8_t mode = buffer[0];
        //REG 0x01
        uint8_t gestrue = buffer[1];
        //REG 0x02
        uint8_t touched = buffer[2] & 0x0F;

        //REG 0x03 ~ 0x04
        uint8_t eventFlag = (buffer[3] & 0xC0) >> 6;
        uint16_t posX = ((buffer[3] & 0x0F) << 8) | buffer[4];
        //REG 0x05 ~ 0x06
        uint16_t posY = ((buffer[5] & 0x0F) << 8) | buffer[6] ;


        if (touched == 0) {
            return false;
        }

        x[0] = posX;
        y[0] = posY;

#ifdef LOG_PORT
        LOG_PORT.println("----------------------------------------------------------------------------");
        LOG_PORT.println("Touched Gesture EvenFlag    [0]PosX    [0]PosY  [1]PosX    [1]PosY");
        LOG_PORT.print(touched); LOG_PORT.print("\t");
        LOG_PORT.print(gestrue); LOG_PORT.print("\t");
        LOG_PORT.print(eventFlag); LOG_PORT.print("\t");
        LOG_PORT.print(posX); LOG_PORT.print("\t");
        LOG_PORT.print(posY); LOG_PORT.print("\t");
#endif

        if (touched == 2) {
            //REG 0x09 ~ 0x0A
            posX = ((buffer[9] & 0x0F) << 8) | buffer[10];
            //REG 0x0B ~ 0x0C
            posY = ((buffer[11] & 0x0F) << 8) | buffer[12] ;

#ifdef LOG_PORT
            LOG_PORT.print(posX); LOG_PORT.print("\t");
            LOG_PORT.print(posY); LOG_PORT.print("\t");
#endif

            if (size == 2) {
                x[1] = posX;
                y[1] = posY;
            }
        }
#ifdef LOG_PORT
        LOG_PORT.println();
#endif
        return touched;
    }

    uint8_t getTouched()
    {
        return readRegister(FT6X36_REG_STATUS);
    }

    void setPowerMode(PowerMode mode)
    {
        writeRegister(FT6X36_REG_POWER_MODE, mode);
    }

    uint8_t getChipID(void)
    {
        return readRegister(FT6X36_REG_CHIPID);
    }

    uint8_t getVendorID(void)
    {
        return readRegister(FT6X36_REG_VENDOR1_ID);
    }

    uint8_t getErrorCode(void)
    {
        return readRegister(FT6X36_REG_ERROR_STATUS);
    }

    const char *getModelName()
    {
        switch (chipID) {
        case FT6206_CHIPID: return "FT6206";
        case FT6236_CHIPID: return "FT6236";
        case FT6236U_CHIPID: return "FT6236U";
        case FT3267_CHIPID: return "FT3267";
        default: return "UNKOWN";
        }
    }

private:
    bool initImpl()
    {
        uint8_t vendId = readRegister(FT6X36_REG_VENDOR1_ID);
        chipID = readRegister(FT6X36_REG_CHIPID);

#ifdef LOG_PORT
        LOG_PORT.print("Vend ID: 0x");
        LOG_PORT.println(vendId, HEX);
        LOG_PORT.print("Chip ID: 0x");
        LOG_PORT.println(chipID, HEX);
        LOG_PORT.print("Firm V: "); LOG_PORT.println(readRegister(FT6X36_REG_FIRMVERS));
        LOG_PORT.print("Point Rate Hz: ");
        LOG_PORT.println(readRegister(FT6X36_REG_PERIODACTIVE));
        LOG_PORT.print("Thresh: ");
        LOG_PORT.println(readRegister(FT6X36_REG_THRESHHOLD));
#endif
        // change threshhold to be higher/lower
        writeRegister(FT6X36_REG_THRESHHOLD, 60);

        if (vendId != FT6X36_VENDID) {
            LOG("Vendor id is not match!");
            return false;
        }
        if ((chipID != FT6206_CHIPID) &&
                (chipID != FT6236_CHIPID) &&
                (chipID != FT6236U_CHIPID)  &&
                (chipID != FT3267_CHIPID)
           ) {
            LOG("Vendor id is not match!");
            LOG("ChipID:0x%x should be 0x06 or 0x36 or 0x64\n", chipID);
            return false;
        }

        uint16_t version = getLibraryVersion();
        LOG("Chip library version : 0x%x\n", version);

        // This register describes period of monitor status, it should not less than 30.
        uint8_t val = readRegister(FT6X36_REG_PERIODMONITOR);
        LOG("Chip period of monitor status : 0x%x\n", val);

        // This register describes the period of active status, it should not less than 12


        return true;
    }

    int getReadMaskImpl()
    {
        return -1;
    }

protected:
    uint8_t chipID;
};



