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
 * @file      i2c_driver.h
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-01-22
 *
 */
#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0) && defined(CONFIG_SENSORLIB_ESP_IDF_NEW_API)
#include "driver/i2c_master.h"
#else
#include "driver/i2c.h"
#endif  //ESP_IDF_VERSION


esp_err_t i2c_drv_init(void);
void i2c_drv_scan();
bool  i2c_drv_probe(uint8_t devAddr);
int i2c_read_callback(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
int i2c_write_callback(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

#if CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW
esp_err_t i2c_drv_device_init(uint8_t address);
#endif


