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
 * @file      sensor_bhi260.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-01-23
 *
 */
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "SensorBHI260AP.hpp"
#include <bosch/BoschSensorDataHelper.hpp>

#if CONFIG_BHI260


// The firmware runs in RAM and will be lost if the power is off. The firmware will be loaded from RAM each time it is run.
#define BOSCH_APP30_SHUTTLE_BHI260_FW
// #define BOSCH_APP30_SHUTTLE_BHI260_AUX_BMM150FW
// #define BOSCH_APP30_SHUTTLE_BHI260_BME68X
// #define BOSCH_APP30_SHUTTLE_BHI260_BMP390
// #define BOSCH_APP30_SHUTTLE_BHI260_TURBO
// #define BOSCH_BHI260_AUX_BEM280
// #define BOSCH_BHI260_AUX_BMM150_BEM280
// #define BOSCH_BHI260_AUX_BMM150_BEM280_GPIO
// #define BOSCH_BHI260_AUX_BMM150_GPIO
// #define BOSCH_BHI260_GPIO

// Firmware is stored in flash and booted from flash,Depends on BHI260 hardware connected to SPI Flash
// #define BOSCH_APP30_SHUTTLE_BHI260_AUX_BMM150_FLASH
// #define BOSCH_APP30_SHUTTLE_BHI260_BME68X_FLASH
// #define BOSCH_APP30_SHUTTLE_BHI260_BMP390_FLASH
// #define BOSCH_APP30_SHUTTLE_BHI260_FLASH
// #define BOSCH_APP30_SHUTTLE_BHI260_TURBO_FLASH
// #define BOSCH_BHI260_AUX_BEM280_FLASH
// #define BOSCH_BHI260_AUX_BMM150_BEM280_FLASH
// #define BOSCH_BHI260_AUX_BMM150_BEM280_GPIO_FLASH
// #define BOSCH_BHI260_AUX_BMM150_GPIO_FLASH
// #define BOSCH_BHI260_GPIO_FLASH

#include <BoschFirmware.h>

static const char *TAG = "BHI";

extern uint32_t hal_callback(SensorCommCustomHal::Operation op, void *param1, void *param2);

SensorBHI260AP bhy;

static volatile bool dataReady = false;

static bool init_done = false;

#if CONFIG_SENSOR_IRQ != -1
static void IRAM_ATTR bhi260_set_flag(void *arg)
{
    dataReady = true;
}
#endif

static void bhi260_isr_init()
{
#if CONFIG_SENSOR_IRQ != -1
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << CONFIG_SENSOR_IRQ);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_intr_type((gpio_num_t)CONFIG_SENSOR_IRQ, GPIO_INTR_POSEDGE);
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add((gpio_num_t)CONFIG_SENSOR_IRQ, bhi260_set_flag, NULL);
#endif
}

static void accel_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp, void *user_data)
{
    struct bhy2_data_xyz data;
    float scaling_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &data);
    ESP_LOGI(TAG, "%s: x: %f, y: %f, z: %f;", bhy.getSensorName(sensor_id),
             data.x * scaling_factor,
             data.y * scaling_factor,
             data.z * scaling_factor);
}

static void gyro_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp, void *user_data)
{
    struct bhy2_data_xyz data;
    float scaling_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &data);
    ESP_LOGI(TAG, "%s: x: %f, y: %f, z: %f;", bhy.getSensorName(sensor_id),
             data.x * scaling_factor,
             data.y * scaling_factor,
             data.z * scaling_factor);
}

esp_err_t bhi260_init()
{
    ESP_LOGI(TAG, "----DRIVER BHI260AP----");

    // Set the reset pin
    bhy.setPins(CONFIG_SENSOR_RST);

#if CONFIG_USE_I2C_INTERFACE
    // BHI260AP_SLAVE_ADDRESS_L = 0x28
    // BHI260AP_SLAVE_ADDRESS_H = 0x29
    uint8_t address = BHI260AP_SLAVE_ADDRESS_L;

    //* Implemented using read and write callback methods, applicable to other platforms
#if CONFIG_I2C_COMMUNICATION_METHOD_CALLBACK_RW

    // * Provide the device address to the callback function
    i2c_drv_device_init(address);

    ESP_LOGI(TAG, "Implemented using read and write callback methods");
    if (bhy.begin(i2c_wr_function, hal_callback, address)) {
        ESP_LOGI(TAG, "Initialize BHI260AP using I2C interface");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BHI260AP !");
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

    if (bhy.begin(bus_handle, address)) {
        ESP_LOGI(TAG, "Initialize BHI260AP using I2C interface");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BHI260AP !");
        return ESP_FAIL;
    }

#else

    ESP_LOGI(TAG, "Implemented using built-in read and write methods (Use lower version < 5.0 API)");
    if (bhy.begin((i2c_port_t)CONFIG_I2C_MASTER_PORT_NUM, address, CONFIG_SENSOR_SDA, CONFIG_SENSOR_SCL)) {
        ESP_LOGI(TAG, "Initialize BHI260AP using I2C interface");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BHI260AP !");
        return ESP_FAIL;
    }
#endif //ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#endif //CONFIG_I2C_COMMUNICATION_METHOD_BUILTIN_RW


#else //USE_I2C_INTERFACE

    // Bus handle
    spi_device_handle_t spi = NULL;

    // Using SPI interface
    if (bhy.begin(SPI2_HOST, spi, CONFIG_SPI_CS, CONFIG_SPI_MOSI, CONFIG_SPI_MISO, CONFIG_SPI_SCK)) {
        ESP_LOGI(TAG, "Initialize BHI260AP using SPI interface");
    } else {
        ESP_LOGE(TAG, "Failed to initialize the BHI260AP !");
        return ESP_FAIL;
    }
#endif //USE_SPI_INTERFACE

    // Output all sensors info to Serial
    BoschSensorInfo info = bhy.getSensorInfo();
    info.printInfo();

    float sample_rate = 1.0;      /* Read out hintr_ctrl measured at 1Hz */
    uint32_t report_latency_ms = 0; /* Report immediately */

    // Enable acceleration
    bhy.configure(SensorBHI260AP::ACCEL_PASSTHROUGH, sample_rate, report_latency_ms);
    // Enable gyroscope
    bhy.configure(SensorBHI260AP::GYRO_PASSTHROUGH, sample_rate, report_latency_ms);

    // Set the acceleration sensor result callback function
    bhy.onResultEvent(SensorBHI260AP::ACCEL_PASSTHROUGH, accel_process_callback);

    // Set the gyroscope sensor result callback function
    bhy.onResultEvent(SensorBHI260AP::GYRO_PASSTHROUGH, gyro_process_callback);

    // Registration interruption
    bhi260_isr_init();

    init_done = true;

    return ESP_OK;
}

void bhi260_loop()
{
    if (!init_done) {
        return;
    }
#if CONFIG_SENSOR_IRQ != -1
    if (dataReady) {
        dataReady = false;
#endif
        bhy.update();
#if CONFIG_SENSOR_IRQ != -1
    }
#endif
}

#endif