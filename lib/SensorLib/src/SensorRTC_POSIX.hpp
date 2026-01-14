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
 * @file      SensorRTC_ESP32.hpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-08-15
 *
 */
#pragma once

#include "SensorRTC.h"
#include "SensorPlatform.hpp"

/**
 * @brief Currently only the Espressif platform supports the POSIX interface
 */
#if defined(ARDUINO_ARCH_ESP32) || defined(CONFIG_IDF_TARGET)

/**
 * @brief POSIX-based implementation of the SensorRTC interface
 *
 * This class provides RTC (Real-Time Clock) functionality using POSIX system calls,
 * allowing interaction with the system clock on POSIX-compliant operating systems.
 */
class SensorRTC_POSIX  : public SensorRTC
{
public:
    /**
     * @brief Inherit overloaded setDateTime method from base class
     */
    using SensorRTC::setDateTime;

    /**
     * @brief Inherit overloaded getDateTime method from base class
     */
    using SensorRTC::getDateTime;

    /**
     * @brief Default constructor for SensorRTC_POSIX
     *
     * Initializes a new instance of the SensorRTC_POSIX class.
     * No system resources are allocated during construction.
     */
    SensorRTC_POSIX ()
    {
    }

    /**
     * @brief Destructor for SensorRTC_POSIX
     *
     * Cleans up any resources used by the SensorRTC_POSIX instance.
     */
    ~SensorRTC_POSIX ()
    {
    }

#if defined(ARDUINO)
    /**
     * @brief Arduino-specific initialization method (implementation required for inheritance)
     *
     * This method exists solely to satisfy the pure virtual function requirement from the base SensorRTC class.
     * It does not perform any actual initialization and does not need to be called explicitly.
     *
     * @param wire Reference to TwoWire object for I2C communication (unused)
     * @param sda SDA pin number for I2C communication (unused)
     * @param scl SCL pin number for I2C communication (unused)
     * @return true Always returns true as no initialization is needed
     */
    bool begin(TwoWire &wire, int sda, int scl)
    {
        return true;
    }
#endif  //ARDUINO

    /**
     * @brief Initialization method with custom callback (implementation required for inheritance)
     *
     * This method exists solely to satisfy the pure virtual function requirement from the base SensorRTC class.
     * It does not perform any actual initialization and does not need to be called explicitly.
     *
     * @param callback Custom communication callback function (unused)
     * @return true Always returns true as no initialization is needed
     */
    bool begin(SensorCommCustom::CustomCallback callback)
    {
        return true;
    }


    /**
     * @brief Sets the system date and time using POSIX clock functions
     *
     * Converts the provided RTC_DateTime object to a POSIX timespec and updates
     * the system clock using clock_settime with CLOCK_REALTIME.
     *
     * @param datetime RTC_DateTime object containing the new date and time
     */
    void setDateTime(RTC_DateTime datetime)
    {
        struct tm tm_time;
        struct timespec ts;
        memset(&tm_time, 0, sizeof(struct tm));
        tm_time.tm_year = datetime.getYear() - 1900;
        tm_time.tm_mon = datetime.getMonth() - 1;
        tm_time.tm_mday = datetime.getDay();
        tm_time.tm_hour = datetime.getHour();
        tm_time.tm_min = datetime.getMinute();
        tm_time.tm_sec = datetime.getSecond();

        time_t epoch_seconds = mktime(&tm_time);
        if (epoch_seconds == (time_t) -1) {
            log_e("Invalid date and time");
            return;
        }
        ts.tv_sec = epoch_seconds;
        ts.tv_nsec = 0;
        if (clock_settime(CLOCK_REALTIME, &ts) == -1) {
            log_e("set system time failed");
        }
    }

    /**
     * @brief Retrieves the current date and time from the system clock
     *
     * Gets the current system time using POSIX time functions, converts it
     * to local time, and returns it as an RTC_DateTime object.
     *
     * @return RTC_DateTime Object containing the current date and time
     */
    RTC_DateTime getDateTime()
    {
        time_t t = time(NULL);
        struct tm *local_tm = localtime(&t);
        return RTC_DateTime(local_tm->tm_year + 1900, local_tm->tm_mon + 1, local_tm->tm_mday,
                            local_tm->tm_hour, local_tm->tm_min, local_tm->tm_sec, local_tm->tm_wday);
    }

    /**
     * @brief Gets the name of the chip/board variant
     *
     * Returns the Arduino variant name as the chip identifier for this POSIX implementation.
     *
     * @return const char* Pointer to a string containing the chip/board name
     */
    const char *getChipName()
    {
        return ARDUINO_VARIANT;
    }
};

#endif

