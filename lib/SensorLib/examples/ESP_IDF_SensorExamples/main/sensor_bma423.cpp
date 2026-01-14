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
 * @file      sensor_bma423.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-01-19
 *
 */
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "i2c_driver.h"
#include "freertos/FreeRTOS.h"

#ifdef CONFIG_BMA423

#include "SensorBMA423.hpp"

static const char *TAG = "BMA";

SensorBMA423 accel;

extern uint32_t hal_callback(SensorCommCustomHal::Operation op, void *param1, void *param2);

static bool init_done = false;

esp_err_t bma423_init()
{
    uint8_t address = BMA423_I2C_ADDR_SECONDARY;

    ESP_LOGI(TAG, "----DRIVER BMA423----");

    //* Implemented using read and write callback methods, applicable to other platforms
#if CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW


    // * Provide the device address to the callback function
    i2c_drv_device_init(address);

    ESP_LOGI(TAG, "Implemented using read and write callback methods");
    if (accel.begin(i2c_wr_function, hal_callback, address)) {
        ESP_LOGI(TAG, "Initialization of BMA423 accelerometer is successful!");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BMA423 accelerometer!");
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

    if (accel.begin(bus_handle, address)) {
        ESP_LOGI(TAG, "Initialization of BMA423 accelerometer is successful!");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BMA423 accelerometer!");
        return ESP_FAIL;
    }

#else

    ESP_LOGI(TAG, "Implemented using built-in read and write methods (Use lower version < 5.0 API)");
    if (accel.begin((i2c_port_t)CONFIG_I2C_MASTER_PORT_NUM, address, CONFIG_SENSOR_SDA, CONFIG_SENSOR_SCL)) {
        ESP_LOGI(TAG, "Initialization of BMA423 accelerometer is successful!");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BMA423 accelerometer!");
        return ESP_FAIL;
    }
#endif //ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#endif //CONFIG_I2C_COMMUNICATION_METHOD_BUILTIN_RW

    //Default 4G ,200HZ
    accel.configAccelerometer();

    accel.enableAccelerometer();

    init_done = true;

    return ESP_OK;
}

void bma423_loop()
{
    if (!init_done) {
        return;
    }
    ESP_LOGI("BMA423", "Temperature:%.2f*C", accel.getTemperature(SensorBMA423::TEMP_DEG));
    ESP_LOGI("BMA423", "Temperature:%.2f*F", accel.getTemperature(SensorBMA423::TEMP_FAHRENHEIT));
    ESP_LOGI("BMA423", "Direction:%u", accel.direction());
}

#endif
