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
 * @file      main.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-01-22
 *
 */
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "i2c_driver.h"

static const char *TAG = "main";

extern esp_err_t sensor_drv_init();
extern esp_err_t touch_drv_init();
extern void touch_loop();
static void touch_task(void *);

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(i2c_drv_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    ESP_ERROR_CHECK(touch_drv_init());

    xTaskCreate(touch_task, "touch", 4 * 1024, NULL, 10, NULL);

    ESP_LOGI(TAG, "Run...");
}


static void touch_task(void *args)
{
    while (1) {
        touch_loop();
        // vTaskDelay(pdMS_TO_TICKS(10));
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}



