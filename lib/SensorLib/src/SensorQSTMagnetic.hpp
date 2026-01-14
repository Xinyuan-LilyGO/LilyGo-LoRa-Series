/**
 *
 * @license MIT License
 *
 * Copyright (c) 2025 lewis he
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
 * @file      SensorQSTMagnetic.hpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-08-16
 *
 */
#pragma once
#include "REG/QSTMagneticConstants.h"
#include "SensorPlatform.hpp"

static constexpr uint8_t QMC6310U_SLAVE_ADDRESS = 0x1C;
static constexpr uint8_t QMC6310N_SLAVE_ADDRESS = 0x3C;
static constexpr uint8_t QMC5883P_SLAVE_ADDRESS = 0x2C;

class Polar
{
public:
    /**
     * @brief Default constructor. Initializes all member variables to 0.
     *
     * This constructor creates a Polar object with initial values of 0 for
     * polar angle, Gauss, and microtesla fields.
     */
    Polar() : polar(0), Gauss(0), uT(0) {}

    /**
     * @brief Parameterized constructor. Initializes member variables with user-provided values.
     *
     * @param polar The polar angle value to initialize.
     * @param Gauss The magnetic field strength in Gauss to initialize.
     * @param uT The magnetic field strength in microtesla to initialize.
     */
    Polar(float polar, float Gauss, float uT) : polar(polar), Gauss(Gauss), uT(uT) {}

    float polar;   ///< Stores the polar angle value
    float Gauss;   ///< Stores the magnetic field strength in Gauss
    float uT;      ///< Stores the magnetic field strength in microtesla
};



class SensorQSTMagnetic :  public QSTMagneticConstants
{
public:
    /**
         * @brief Enumeration defining the types of supported magnetic sensor chips.
         */
    enum ChipType {
        CHIP_QMC6310U,   ///< Represents the QMC6310U chip type
        CHIP_QMC6310N,   ///< Represents the QMC6310N chip type
        CHIP_QMC5883P,   ///< Represents the QMC5883P chip type
        CHIP_UNKNOWN     ///< Represents an unknown or unsupported chip type
    };

    /**
     * @brief Enumeration defining the operating modes of the sensor.
     */
    enum SensorMode {
        MODE_SUSPEND,     ///< Low-power suspend mode with limited functionality
        MODE_NORMAL,      ///< Normal continuous measurement mode
        MODE_SINGLE,      ///< Single measurement mode (enters suspend after one reading)
        MODE_CONTINUOUS   ///< Continuous uninterrupted measurement mode
    };

    /**
     * @brief Enumeration defining the magnetic field measurement ranges (in Gauss).
     */
    enum MagRange {
        RANGE_30G,   ///< 30 Gauss measurement range
        RANGE_12G,   ///< 12 Gauss measurement range
        RANGE_8G,    ///< 8 Gauss measurement range
        RANGE_2G     ///< 2 Gauss measurement range
    };

    /**
     * @brief Enumeration defining the data output rates of the sensor.
     */
    enum OutputRate {
        DATARATE_10HZ,   ///< 10 Hz data output rate
        DATARATE_50HZ,   ///< 50 Hz data output rate
        DATARATE_100HZ,  ///< 100 Hz data output rate
        DATARATE_200HZ   ///< 200 Hz data output rate
    };

    /**
     * @brief Enumeration defining control register operations.
     */
    enum CtrlReg {
        SET_RESET_ON,   ///< Operation to set reset enable
        SET_ONLY_ON,    ///< Operation to only set enable
        SET_RESET_OFF   ///< Operation to set reset disable
    };

    /**
     * @brief Enumeration defining oversample ratios for the sensor.
     */
    enum OverSampleRatio {
        OSR_8,   ///< 8x oversample ratio
        OSR_4,   ///< 4x oversample ratio
        OSR_2,   ///< 2x oversample ratio
        OSR_1    ///< 1x oversample ratio
    };

    /**
     * @brief Enumeration defining downsample ratios for the sensor.
     */
    enum DownSampleRatio {
        DSR_1,   ///< 1x downsample ratio
        DSR_2,   ///< 2x downsample ratio
        DSR_4,   ///< 4x downsample ratio
        DSR_8    ///< 8x downsample ratio
    };

    /**
     * @brief Default constructor. Initializes communication and hardware abstraction pointers to nullptr.
     *
     * Creates a SensorQSTMagnetic object with no active communication or hardware
     * abstraction instances.
     */
    SensorQSTMagnetic() : comm(nullptr), hal(nullptr) {}

    /**
     * @brief Destructor. Deinitializes the communication object if it exists.
     *
     * Ensures proper cleanup by deinitializing the communication interface
     * when the sensor object is destroyed.
     */
    ~SensorQSTMagnetic()
    {
        if (comm) {
            comm->deinit();
        }
    }

#if defined(ARDUINO)
    /**
      * @brief Initialization function for the Arduino platform. Sets up the sensor over I2C.
      *
      * @param wire Reference to a TwoWire object (typically Wire) for I2C communication.
      * @param addr I2C slave address of the sensor. Defaults to QMC6310U_SLAVE_ADDRESS.
      * @param sda I2C SDA pin number. Defaults to -1 (use board's default pin).
      * @param scl I2C SCL pin number. Defaults to -1 (use board's default pin).
      * @return true if initialization is successful, false otherwise.
      */
    bool begin(TwoWire &wire, uint8_t addr = QMC6310U_SLAVE_ADDRESS, int sda = -1, int scl = -1)
    {
        if (!beginCommon<SensorCommI2C, HalArduino>(comm, hal, wire, addr, sda, scl)) {
            return false;
        }
        return initImpl(addr);
    }
#elif defined(ESP_PLATFORM)

#if defined(USEING_I2C_LEGACY)
    /**
     * @brief Initialization function for ESP32 platform (legacy I2C mode).
     *
     * @param port_num I2C port number to use for communication.
     * @param addr I2C slave address of the sensor. Defaults to QMC6310U_SLAVE_ADDRESS.
     * @param sda I2C SDA pin number. Defaults to -1 (use default pin).
     * @param scl I2C SCL pin number. Defaults to -1 (use default pin).
     * @return true if initialization is successful, false otherwise.
     */
    bool begin(i2c_port_t port_num, uint8_t addr = QMC6310U_SLAVE_ADDRESS, int sda = -1, int scl = -1)
    {
        if (!beginCommon<SensorCommI2C, HalEspIDF>(comm, hal, port_num, addr, sda, scl)) {
            return false;
        }
        return initImpl(addr);
    }
#else
    /**
     * @brief Initialization function for ESP32 platform (new I2C master mode).
     *
     * @param handle I2C master bus handle for communication.
     * @param addr I2C slave address of the sensor. Defaults to QMC6310U_SLAVE_ADDRESS.
     * @return true if initialization is successful, false otherwise.
     *
     * @note Assumes `sda` and `scl` are handled via the bus handle configuration.
     */
    bool begin(i2c_master_bus_handle_t handle, uint8_t addr = QMC6310U_SLAVE_ADDRESS)
    {
        if (!beginCommon<SensorCommI2C, HalEspIDF>(comm, hal, handle, addr, sda, scl)) {
            return false;
        }
        return initImpl(addr);
    }
#endif
#endif

    /**
     * @brief Initialization function for custom communication with callback support.
     *
     * @param callback Custom communication callback function for read/write operations.
     * @param hal_callback Custom hardware abstraction callback function (e.g., delays).
     * @param addr I2C slave address of the sensor. Defaults to QMC6310U_SLAVE_ADDRESS.
     * @return true if initialization is successful, false otherwise.
     */
    bool begin(SensorCommCustom::CustomCallback callback,
               SensorCommCustomHal::CustomHalCallback hal_callback,
               uint8_t addr = QMC6310U_SLAVE_ADDRESS)
    {
        if (!beginCommCustomCallback<SensorCommCustom, SensorCommCustomHal>(COMM_CUSTOM,
                callback, hal_callback, addr, comm, hal)) {
            return false;
        }
        return initImpl(addr);
    }

    /**
     * @brief Resets the sensor by writing to the command register.
     *
     * Performs a soft reset by writing a reset command to REG_CMD2, then clears
     * the reset bit after a short delay.
     */
    void reset()
    {
        comm->writeRegister(REG_CMD2, (uint8_t)0x80);
        hal->delay(10);
        comm->writeRegister(REG_CMD2, (uint8_t)0x00);
    }

    /**
     * @brief Reads the chip ID from the sensor.
     *
     * @return The chip ID value read from REG_CHIP_ID.
     */
    uint8_t getChipID()
    {
        return comm->readRegister(REG_CHIP_ID);
    }

    /**
     * @brief Gets the status of the sensor by reading the status register.
     *
     * @return The status register value from REG_STAT.
     */
    int getStatus()
    {
        return comm->readRegister(REG_STAT);
    }

    /**
     * @brief Checks if new sensor data is ready to be read.
     *
     * @return true if the data ready bit (bit 0) in REG_STAT is set, false otherwise.
     */
    bool isDataReady()
    {
        if (comm->readRegister(REG_STAT) & 0x01) {
            return true;
        }
        return false;
    }

    /**
     * @brief Checks if a data overflow has occurred.
     *
     * @return true if the data overflow bit (bit 1) in REG_STAT is set, false otherwise.
     */
    bool isDataOverflow()
    {
        if (comm->readRegister(REG_STAT) & 0x02) {
            return true;
        }
        return false;
    }

    /**
     * @brief Enables or disables the self-test feature of the sensor.
     *
     * @param en true to enable self-test (sets bit 1 in REG_CMD2), false to disable (clears bit 1).
     */
    void setSelfTest(bool en)
    {
        en ? comm->setRegisterBit(REG_CMD2, 1)
        : comm->clrRegisterBit(REG_CMD2, 1);
    }

    /**
     * @brief Sets the operating mode of the sensor.
     *
     * @param m The SensorMode to set. Writes the mode value to REG_CMD1.
     * @return The result of the register write operation (0 for success, non-zero for failure).
     */
    int setMode(SensorMode m)
    {
        return comm->writeRegister(REG_CMD1, 0xFC, m);
    }

    /**
     * @brief Configures the control register (REG_CMD2) with the specified operation.
     *
     * @param c The CtrlReg operation to perform. Writes the control value to REG_CMD2.
     * @return The result of the register write operation (0 for success, non-zero for failure).
     */
    int setCtrlRegister(CtrlReg c)
    {
        return comm->writeRegister(REG_CMD2, 0xFC, c);
    }

    /**
     * @brief Sets the data output rate of the sensor.
     *
     * @param odr The OutputRate to set. Shifts the rate value and writes to REG_CMD1.
     * @return The result of the register write operation (0 for success, non-zero for failure).
     */
    int setDataOutputRate(OutputRate odr)
    {
        return comm->writeRegister(REG_CMD1, 0xF3, (odr << 2));
    }

    /**
     * @brief Sets the oversample rate of the sensor.
     *
     * @param osr The OverSampleRatio to set. Shifts the ratio value and writes to REG_CMD1.
     * @return The result of the register write operation (0 for success, non-zero for failure).
     */
    int setOverSampleRate(OverSampleRatio osr)
    {
        return comm->writeRegister(REG_CMD1, 0xCF, (osr << 4));
    }

    /**
     * @brief Sets the downsample rate of the sensor.
     *
     * @param dsr The DownSampleRatio to set. Shifts the ratio value and writes to REG_CMD1.
     * @return The result of the register write operation (0 for success, non-zero for failure).
     */
    int setDownSampleRate(DownSampleRatio dsr)
    {
        return comm->writeRegister(REG_CMD1, 0x3F, (dsr << 6));
    }

    /**
     * @brief Sets the sign configuration for the X, Y, and Z axes.
     *
     * @param x Sign bit for the X axis (0 or 1).
     * @param y Sign bit for the Y axis (0 or 1).
     * @param z Sign bit for the Z axis (0 or 1).
     * @return The result of the register write operation (0 for success, non-zero for failure).
     *
     * Computes a combined sign value and writes it to REG_SIGN.
     */
    int setSign(uint8_t x, uint8_t y, uint8_t z)
    {
        int sign = x + y * 2 + z * 4;
        return comm->writeRegister(REG_SIGN, sign);
    }

    /**
     * @brief Configures the magnetometer with multiple parameters.
     *
     * @param mode The SensorMode to set.
     * @param range The MagRange to set.
     * @param odr The OutputRate to set.
     * @param osr The OverSampleRatio to set.
     * @param dsr The DownSampleRatio to set.
     * @return 0 if all configuration steps succeed, -1 if any step fails.
     *
     * Calls individual configuration functions and checks for errors.
     */
    int configMagnetometer(SensorMode mode, MagRange range, OutputRate odr,
                           OverSampleRatio osr, DownSampleRatio dsr)
    {
        if (setMagRange(range) < 0) {
            return -1;;
        }
        if (comm->writeRegister(REG_CMD1, 0xFC, mode) < 0) {
            return -1;;
        }
        if (comm->writeRegister(REG_CMD1, 0xF3, (odr << 2)) < 0) {
            return -1;;
        }
        if (comm->writeRegister(REG_CMD1, 0xCF, (osr << 4)) < 0) {
            return -1;;
        }
        if (comm->writeRegister(REG_CMD1, 0x3F, (dsr << 6)) < 0) {
            return -1;;
        }
        return 0;
    }

    /**
     * @brief Sets the magnetic field range and updates the sensitivity.
     *
     * @param range The MagRange to set.
     * @return The result of the register write operation (0 for success, non-zero for failure).
     *
     * Updates the _sensitivity member based on the selected range and writes
     * the range value to REG_CMD2.
     */
    int setMagRange(MagRange range)
    {
        switch (range) {
        case RANGE_30G:
            _sensitivity = 0.1;
            break;
        case RANGE_12G:
            _sensitivity = 0.04;
            break;
        case RANGE_8G:
            _sensitivity = 0.026;
            break;
        case RANGE_2G:
            _sensitivity = 0.0066;
            break;
        default:
            break;
        }
        return comm->writeRegister(REG_CMD2, 0xF3, (range << 2));
    }

    /**
     * @brief Sets offset values for X, Y, and Z axes to calibrate the sensor.
     *
     * These offsets are subtracted from the raw sensor data during read operations to compensate for constant errors.
     * @param x Offset value for X-axis (in raw sensor units).
     * @param y Offset value for Y-axis (in raw sensor units).
     * @param z Offset value for Z-axis (in raw sensor units).
     */
    void setOffset(int x, int y, int z)
    {
        _x_offset = x; _y_offset = y; _z_offset = z;
    }

    /**
     * @brief Reads raw magnetic field data from the sensor, processes it, and stores the results.
     *
     * Reads 6 bytes of data (LSB and MSB for X, Y, Z axes) from the data register (starting at REG_LSB_DX),
     * converts them to 16-bit values, applies offset correction, and calculates the magnetic field strength in Gauss.
     * @return 0 if data read and processing is successful; -1 if read fails.
     */
    int readData()
    {
        uint8_t buffer[6];
        int16_t x, y, z;
        if (comm->readRegister(REG_LSB_DX, buffer,
                               6) != -1) {
            x = (int16_t)(buffer[1] << 8) | (buffer[0]);  // Combine X LSB and MSB
            y = (int16_t)(buffer[3] << 8) | (buffer[2]);  // Combine Y LSB and MSB
            z = (int16_t)(buffer[5] << 8) | (buffer[4]);  // Combine Z LSB and MSB

            // Handle special case for negative maximum value (2's complement adjustment)
            if (x == 32767) {
                x = -((65535 - x) + 1);
            }
            x = (x - _x_offset);  // Apply X-axis offset
            if (y == 32767) {
                y = -((65535 - y) + 1);
            }
            y = (y - _y_offset);  // Apply Y-axis offset
            if (z == 32767) {
                z = -((65535 - z) + 1);
            }
            z = (z - _z_offset);  // Apply Z-axis offset

            _raw[0] = x;  // Store processed raw X value
            _raw[1] = y;  // Store processed raw Y value
            _raw[2] = z;  // Store processed raw Z value

            // Convert raw values to Gauss using sensitivity (depends on selected magnetic range)
            _mag[0] = (float)x * _sensitivity;
            _mag[1] = (float)y * _sensitivity;
            _mag[2] = (float)z * _sensitivity;
            return 0;
        }
        return -1;;
    }

    /**
     * @brief Sets the declination angle for compass heading correction.
     *
     * Declination is the angle between magnetic north and true north, used to adjust the calculated heading to true north.
     * @param dec Declination angle in degrees.
     */
    void setDeclination(float dec)
    {
        _declination = dec;
    }

    /**
     * @brief Reads magnetic field data and calculates polar coordinates (angle and magnitude).
     *
     * Checks if new data is ready, reads the data, computes the heading angle (adjusted by declination),
     * and calculates the total magnetic field magnitude.
     * @param p Reference to a Polar object to store the calculated angle (degrees), magnitude in Gauss, and magnitude in microtesla.
     * @return true if data is ready and calculation is successful; false otherwise.
     */
    bool readPolar(Polar &p)
    {
        if (isDataReady()) {
            readData();
            float x = getX();
            float y = getY();
            float z = getZ();
            // Calculate heading angle (converted from radians to degrees) and adjust with declination
            float angle = (atan2(x, -y) / PI) * 180.0 + _declination;
            angle = _convertAngleToPositive(angle);  // Ensure angle is in 0-360 degrees
            float magnitude = sqrt(x * x + y * y + z * z);  // Total magnetic field magnitude in Gauss
            p = Polar(angle, magnitude * 100, magnitude);  // Convert Gauss to microtesla (1 Gauss = 100 microtesla)
            return true;
        }
        return false;
    }

    /**
     * @brief Gets the processed raw X-axis magnetic field value.
     * @return Raw X-axis value (after offset correction) in sensor units.
     */
    int16_t getRawX()
    {
        return _raw[0];
    }

    /**
     * @brief Gets the processed raw Y-axis magnetic field value.
     * @return Raw Y-axis value (after offset correction) in sensor units.
     */
    int16_t getRawY()
    {
        return _raw[1];
    }

    /**
     * @brief Gets the processed raw Z-axis magnetic field value.
     * @return Raw Z-axis value (after offset correction) in sensor units.
     */
    int16_t getRawZ()
    {
        return _raw[2];
    }

    /**
     * @brief Gets the X-axis magnetic field strength in Gauss.
     * @return X-axis magnetic field strength in Gauss.
     */
    float getX()
    {
        return _mag[0];
    }

    /**
     * @brief Gets the Y-axis magnetic field strength in Gauss.
     * @return Y-axis magnetic field strength in Gauss.
     */
    float getY()
    {
        return _mag[1];
    }

    /**
     * @brief Gets the Z-axis magnetic field strength in Gauss.
     * @return Z-axis magnetic field strength in Gauss.
     */
    float getZ()
    {
        return _mag[2];
    }

    /**
     * @brief Retrieves all three axes' magnetic field strengths in Gauss.
     * @param x Reference to store X-axis magnetic field strength in Gauss.
     * @param y Reference to store Y-axis magnetic field strength in Gauss.
     * @param z Reference to store Z-axis magnetic field strength in Gauss.
     */
    void getMag(float &x, float &y, float &z)
    {
        x = _mag[0];
        y = _mag[1];
        z = _mag[2];
    }

    /**
     * @brief Reads and logs the control registers (REG_CMD1 and REG_CMD2) for debugging.
     *
     * Reads 2 bytes starting from REG_CMD1 and prints their hexadecimal values using log_d.
     */
    void dumpCtrlRegister()
    {
        uint8_t buffer[2];
        comm->readRegister(REG_CMD1, buffer, 2);
        for (int i = 0; i < 2; ++i) {
            log_d("CMD%d: 0x%02x", i + 1, buffer[i]);
        }
    }

    /**
     * @brief Gets the name of the connected sensor chip.
     * @return String literal representing the chip name (e.g., "QMC6310U", "QMC5883P") or "UNKNOWN" if not recognized.
     */
    const char *getChipName()
    {
        switch (_type) {
        case CHIP_QMC6310U: return "QMC6310U";
        case CHIP_QMC6310N: return "QMC6310N";
        case CHIP_QMC5883P: return "QMC5883P";
        default:
            break;
        }
        return "UNKNOWN";
    }


private:

    /**
     * @brief Converts an angle to a positive value within the range [0.0, 360.0) degrees.
     *
     * This function adjusts the input angle to ensure it falls within the standard compass heading range (0 to 360 degrees).
     * If the angle is 360 degrees or more, it subtracts 360 degrees until it is less than 360.
     * If the angle is negative, it adds 360 degrees until it is non-negative.
     * This aligns with the compass heading accuracy (1° to 2°) supported by the sensors, as specified in the datasheets.
     * @param angle The input angle in degrees (may be negative or greater than/equal to 360.0).
     * @return The adjusted angle in degrees, within the range [0.0, 360.0).
     */
    float _convertAngleToPositive(float angle)
    {
        if (angle >= 360.0) {
            angle = angle - 360.0;
        }
        if (angle < 0) {
            angle = angle + 360.0;
        }
        return angle;
    }


    bool initImpl(uint8_t addr)
    {
        reset();
        // QMC5883P Chip id same QMC6310
        if (getChipID() == QMC6310_CHIP_ID) {
            switch (addr) {
            case QMC6310U_SLAVE_ADDRESS:
                _type = CHIP_QMC6310U;
                break;
            case QMC6310N_SLAVE_ADDRESS:
                _type = CHIP_QMC6310N;
                break;
            case QMC5883P_SLAVE_ADDRESS:
                _type = CHIP_QMC5883P;
                break;
            default:
                _type = CHIP_UNKNOWN;
                break;
            }
            return true;
        }
        return false;
    }

protected:
    std::unique_ptr<SensorCommBase> comm;
    std::unique_ptr<SensorHal> hal;
    int16_t _raw[3];
    float _mag[3];
    float _declination;
    float _sensitivity;
    int16_t _x_offset = 0, _y_offset = 0, _z_offset = 0;
    ChipType _type;
};
