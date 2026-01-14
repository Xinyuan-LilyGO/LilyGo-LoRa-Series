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
 * @date      2025-01-19
 *
 */
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "i2c_driver.h"

static const char *TAG = "main";

#ifdef CONFIG_PCF8563
extern esp_err_t pcf8563_init();
extern void pcf8563_loop();
#endif


#ifdef CONFIG_BMA423
extern esp_err_t bma423_init();
extern void bma423_loop();
#endif

#ifdef CONFIG_FT636X
extern esp_err_t ft63x6_init();
extern void ft63x6_loop();
#endif

#ifdef CONFIG_BHI260
extern esp_err_t bhi260_init();
extern void bhi260_loop();
#endif

#ifdef CONFIG_XL9555
extern esp_err_t xl9555_init();
extern void xl9555_loop();
#endif


static void app_task(void *args);

extern "C" void app_main(void)
{

#if CONFIG_I2C_COMMUNICATION_METHOD_BUILTIN_RW  || CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW
    ESP_ERROR_CHECK(i2c_drv_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    // Run bus scan
    i2c_drv_scan();
#endif

#ifdef CONFIG_PCF8563
    ESP_ERROR_CHECK(pcf8563_init());
#endif

#ifdef CONFIG_BMA423
    ESP_ERROR_CHECK(bma423_init());
#endif

#ifdef CONFIG_FT636X
    ESP_ERROR_CHECK(ft63x6_init());
#endif

#ifdef CONFIG_BHI260
    ESP_ERROR_CHECK(bhi260_init());
#endif

#ifdef CONFIG_XL9555
    xl9555_init();
#endif

    ESP_LOGI(TAG, "Run...");

    xTaskCreate(app_task, "App", 20 * 1024, NULL, 10, NULL);

}

static void app_task(void *args)
{
    while (1) {

#ifdef CONFIG_BMA423
        bma423_loop();
        vTaskDelay(pdMS_TO_TICKS(1000));
#endif

#ifdef CONFIG_FT636X
        ft63x6_loop();
#endif

#ifdef CONFIG_BHI260
        bhi260_loop();
#endif

#ifdef CONFIG_XL9555
        xl9555_loop();
#endif

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


#if CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW
/**
 * @brief  hal_callback
 * @note   SensorLib hal callback
 * @param  op:  Operation Code
 * @param  *param1:  parameter
 * @param  *param2:  parameter
 * @retval
 */
uint32_t hal_callback(SensorCommCustomHal::Operation op, void *param1, void *param2)
{
    switch (op) {
    // Set GPIO mode
    case SensorCommCustomHal::OP_PINMODE: {
        uint8_t pin = reinterpret_cast<uintptr_t>(param1);
        uint8_t mode = reinterpret_cast<uintptr_t>(param2);
        gpio_config_t config;
        memset(&config, 0, sizeof(config));
        config.pin_bit_mask = 1ULL << pin;
        switch (mode) {
        case INPUT:
            config.mode = GPIO_MODE_INPUT;
            break;
        case OUTPUT:
            config.mode = GPIO_MODE_OUTPUT;
            break;
        }
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        config.intr_type = GPIO_INTR_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&config));
    }
    break;
    // Set GPIO level
    case SensorCommCustomHal::OP_DIGITALWRITE: {
        uint8_t pin = reinterpret_cast<uintptr_t>(param1);
        uint8_t level = reinterpret_cast<uintptr_t>(param2);
        gpio_set_level((gpio_num_t )pin, level);
    }
    break;
    // Read GPIO level
    case SensorCommCustomHal::OP_DIGITALREAD: {
        uint8_t pin = reinterpret_cast<uintptr_t>(param1);
        return gpio_get_level((gpio_num_t)pin);
    }
    break;
    // Get the current running milliseconds
    case SensorCommCustomHal::OP_MILLIS:
        return (uint32_t) (esp_timer_get_time() / 1000LL);

    // Delay in milliseconds
    case SensorCommCustomHal::OP_DELAY: {
        if (param1) {
            uint32_t ms = reinterpret_cast<uintptr_t>(param1);
            ets_delay_us((ms % portTICK_PERIOD_MS) * 1000UL);
        }
    }
    break;
    // Delay in microseconds
    case SensorCommCustomHal::OP_DELAYMICROSECONDS: {
        uint32_t us = reinterpret_cast<uintptr_t>(param1);
        ets_delay_us(us);
    }
    break;
    default:
        break;
    }
    return 0;
}


#endif
