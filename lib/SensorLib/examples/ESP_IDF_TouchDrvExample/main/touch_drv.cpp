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
 * @file      touch_drv.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-01-22
 *
 */
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "i2c_driver.h"
#include "freertos/FreeRTOS.h"
#include "TouchDrvCSTXXX.hpp"

static const char *TAG = "TOUCH";

#define SENSOR_INPUT_PIN                   (gpio_num_t)CONFIG_SENSOR_IRQ
#define SENSOR_INPUT_PIN_SEL               (1ULL<<SENSOR_INPUT_PIN)

TouchDrvCSTXXX touch;
static QueueHandle_t xQueue;

static void touch_home_button_callback(void *user_data)
{
    ESP_LOGI(TAG, "Touch Home button pressed!");
}

static void IRAM_ATTR sensor_irq_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(xQueue, &gpio_num, NULL);
}

static void irq_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = SENSOR_INPUT_PIN_SEL;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    // gpio_set_intr_type(SENSOR_INPUT_PIN, GPIO_INTR_NEGEDGE);
    // //install gpio isr service
    // gpio_install_isr_service(0);
    // //hook isr handler for specific gpio pin
    // gpio_isr_handler_add(SENSOR_INPUT_PIN, sensor_irq_handler, (void *) SENSOR_INPUT_PIN);

}

esp_err_t touch_drv_init()
{
    uint8_t address = 0xFF;

    ESP_LOGI(TAG, "----SensorLib TouchDrv Examples----");

    /*
     * CST816, CST328 Some chips have automatic sleep function.
     * If you want to scan the device address,
     * must reset the chip first. If the reset pin is not connected,
     * you may not be able to obtain the device address by scanning the bus.
    */
    gpio_num_t gpio_num = (gpio_num_t)CONFIG_SENSOR_RST;
    ESP_ERROR_CHECK(gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT));

    gpio_set_level(gpio_num, 0);
    vTaskDelay(pdMS_TO_TICKS(15));
    gpio_set_level(gpio_num, 1);
    vTaskDelay(pdMS_TO_TICKS(80));

    // Run bus scan
    i2c_drv_scan();

    // Get known address
    if (i2c_drv_probe(CST816_SLAVE_ADDRESS)) {
        address = CST816_SLAVE_ADDRESS;
        ESP_LOGI(TAG, "Find touch address 0x%X", address);
    } else if (i2c_drv_probe(CST226SE_SLAVE_ADDRESS)) {
        address = CST226SE_SLAVE_ADDRESS;
        ESP_LOGI(TAG, "Find touch address 0x%X", address);
    } else if (i2c_drv_probe(CST328_SLAVE_ADDRESS)) {
        address = CST328_SLAVE_ADDRESS;
        ESP_LOGI(TAG, "Find touch address 0x%X", address);
    }

    if (address == 0xFF) {
        ESP_LOGE(TAG, "Could't find touch chip!");
        return ESP_FAIL;
    }

    // Set reset and interrupt pin
    touch.setPins(CONFIG_SENSOR_RST, CONFIG_SENSOR_IRQ);

    //* Implemented using read and write callback methods, applicable to other platforms
#if CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW

    // * Provide the device address to the callback function
    i2c_drv_device_init(address);

    ESP_LOGI(TAG, "Implemented using read and write callback methods");
    if (touch.begin(address, i2c_read_callback, i2c_write_callback)) {
        ESP_LOGI(TAG, "Initializing the capacitive touch screen successfully");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the capacitive touch screen");
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

    if (touch.begin(bus_handle, address)) {
        ESP_LOGI(TAG, "Initializing the capacitive touch screen successfully");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the capacitive touch screen");
        return ESP_FAIL;
    }
#else

    ESP_LOGI(TAG, "Implemented using built-in read and write methods (Use lower version < 5.0 API)");
    if (touch.begin((i2c_port_t)CONFIG_I2C_MASTER_PORT_NUM, address, CONFIG_SENSOR_SDA, CONFIG_SENSOR_SCL)) {
        ESP_LOGI(TAG, "Initializing the capacitive touch screen successfully");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the capacitive touch screen");
        return ESP_FAIL;
    }
#endif //ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#endif //CONFIG_I2C_COMMUNICATION_METHOD_BUILTIN_RW

    const char *model = touch.getModelName();
    ESP_LOGI(TAG, "Using %s model", model);
    if (strncmp(model, "CST8", 4) == 0) {
        ESP_LOGI(TAG, "Some CST816 will automatically enter sleep,can use the touch.disableAutoSleep() to turn off");
        // Some CST816 will automatically enter sleep,
        // can use the touch.disableAutoSleep() to turn off
        touch.disableAutoSleep();

        /**
         * Some touch screens have touch buttons,
         * and the touch coordinates need to be given
         * before the touch button callback function can be used.
         */
        // Example touch coordinates using T-Display-S3 touch
        touch.setCenterButtonCoordinate(85, 360);

        touch.setHomeButtonCallback(touch_home_button_callback);
    }

    // Set the maximum valid coordinates
    // touch.setMaxCoordinates(320, 170);

    // Swap XY coordinates
    // touch.setSwapXY(true);

    // Mirror XY coordinates
    // touch.setMirrorXY(false, true);


    // Create a queue to handle gpio event from isr
    xQueue = xQueueCreate(5, sizeof(uint32_t));

    // Register Sensor interrupt pins
    irq_init();

    return ESP_OK;
}


void touch_drv_isr_handler()
{
    int16_t x[5], y[5];
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        for (int i = 0; i < touched; ++i) {
            printf("X[");
            printf("%d", i);
            printf("]:");
            printf("%d", x[i]);
            printf(" ");
            printf(" Y[");
            printf("%d", i);
            printf("]:");
            printf("%d", y[i]);
            printf(" ");
        }
        printf("\n");
    }
}

void touch_loop()
{
    uint32_t io_num;
    // if (xQueueReceive(xQueue, &io_num, pdMS_TO_TICKS(10))) {
    // if (gpio_get_level((gpio_num_t)CONFIG_SENSOR_IRQ) == 0) {
    touch_drv_isr_handler();
    // }
    // }
}
