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
 * @file      sensor_pcf8563.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-01-19
 *
 */
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "i2c_driver.h"
#include "freertos/FreeRTOS.h"

#ifdef CONFIG_PCF8563

#include "SensorPCF8563.hpp"

static const char *TAG = "RTC";

SensorPCF8563 rtc;

static bool init_done = false;

esp_err_t pcf8563_init()
{
    ESP_LOGI(TAG, "----DRIVER PCF8563 ----");

    //* Implemented using read and write callback methods, applicable to other platforms
#if CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW

    uint8_t address = 0x51;

    // * Provide the device address to the callback function
    i2c_drv_device_init(address);

    ESP_LOGI(TAG, "Implemented using read and write callback methods");
    if (rtc.begin(i2c_wr_function)) {
        ESP_LOGI(TAG, "Initializing PCF8563 real-time clock successfully!");
    } else {
        ESP_LOGE(TAG, "Failed to initialize PCF8563 real time clock!");
        return ESP_FAIL;
    }
#endif

    //* Use the built-in esp-idf communication method
#if CONFIG_I2C_COMMUNICATION_METHOD_BUILTIN_RW
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)) && defined(CONFIG_SENSORLIB_ESP_IDF_NEW_API)

    ESP_LOGI(TAG, "Implemented using built-in read and write methods (Use higher version >= 5.0 API)");

    // * Using the new API of esp-idf 5.x, you need to pass the I2C BUS handle,
    // * which is useful when the bus shares multiple devices.
    extern i2c_master_bus_handle_t bus_handle;

    if (rtc.begin(bus_handle)) {
        ESP_LOGI(TAG, "Initializing PCF8563 real-time clock successfully!");
    } else {
        ESP_LOGE(TAG, "Failed to initialize PCF8563 real time clock!");
        return ESP_FAIL;
    }

#else

    ESP_LOGI(TAG, "Implemented using built-in read and write methods (Use lower version < 5.0 API)");
    if (rtc.begin((i2c_port_t)CONFIG_I2C_MASTER_PORT_NUM, CONFIG_SENSOR_SDA, CONFIG_SENSOR_SCL)) {
        ESP_LOGI(TAG, "Initializing PCF8563 real-time clock successfully!");
    } else {
        ESP_LOGE(TAG, "Failed to initialize PCF8563 real time clock!");
        return ESP_FAIL;
    }
#endif //ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#endif //CONFIG_I2C_COMMUNICATION_METHOD_BUILTIN_RW

    // Unix tm structure sets the time
    struct tm timeinfo;
    timeinfo.tm_yday = 2025 - 1900; //Counting starts from 1900, so subtract 1900 here
    timeinfo.tm_mon = 1 - 1;        //Months start at 0, so you need to subtract 1.
    timeinfo.tm_mday = 17;
    timeinfo.tm_hour = 4;
    timeinfo.tm_min = 30;
    timeinfo.tm_sec = 30;
    rtc.setDateTime(timeinfo);

    init_done = true;

    return ESP_OK;
}

void pcf8563_loop()
{
    if (!init_done) {
        return;
    }
    char buf[64];
    struct tm timeinfo;
    // Get the time C library structure
    rtc.getDateTime(&timeinfo);
    // Format the output using the strftime function
    // For more formats, please refer to :
    // https://man7.org/linux/man-pages/man3/strftime.3.html
    size_t written = strftime(buf, 64, "%A, %B %d %Y %H:%M:%S", &timeinfo);
    if (written != 0) {
        ESP_LOGI("RTC", "%s", buf);
    }
    written = strftime(buf, 64, "%b %d %Y %H:%M:%S", &timeinfo);
    if (written != 0) {
        ESP_LOGI("RTC", "%s", buf);
    }
    written = strftime(buf, 64, "%A, %d. %B %Y %I:%M%p", &timeinfo);
    if (written != 0) {
        ESP_LOGI("RTC", "%s", buf);
    }
}

#endif