/**
 *
 * @license MIT License
 *
 * Copyright (c) 2024 lewis he
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
 * @file      BHI260AP_UpdateFirmware.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2024-10-23
 * @note      Demonstrates loading firmware from a file into BHI260, only testing the NRF52840 platform
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#if defined(ARDUINO_ARCH_NRF52)

#include <SdFat.h>              //Deplib https://github.com/adafruit/SdFat.git
#include <SensorBHI260AP.hpp>

// #define USE_I2C_INTERFACE        true
// #define USE_SPI_INTERFACE        true

#if !defined(USE_I2C_INTERFACE) && !defined(USE_SPI_INTERFACE)
#define USE_SPI_INTERFACE
#warning "No interface type is selected, use I2C interface"
#endif

#if defined(USE_SPI_INTERFACE)
#ifndef SPI_MOSI
#define SPI_MOSI    33
#endif

#ifndef SPI_MISO
#define SPI_MISO    34
#endif

#ifndef SPI_SCK
#define SPI_SCK     35
#endif

// If BHI260_IRQ is set to -1, sensor interrupts are not used and the sensor polling method is used instead.
#ifndef BHI260_IRQ
#define BHI260_IRQ  37
#endif

#ifndef BHI260_CS
#define BHI260_CS   36
#endif

#else   //* I2C */

#ifndef BHI260_SDA
#define BHI260_SDA  2
#endif

#ifndef BHI260_SCL
#define BHI260_SCL  3
#endif

// If BHI260_IRQ is set to -1, sensor interrupts are not used and the sensor polling method is used instead.
#ifndef BHI260_IRQ
#define BHI260_IRQ  8
#endif
#endif  /*USE_SPI_INTERFACE*/

#ifndef BHI260_RST
#define BHI260_RST -1
#endif

SensorBHI260AP bhy;


#define CS_PIN                5

/***************************************
 *  SD CARD
 ***************************************/
SdFat32 sd;

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(8)
//------------------------------------------------------------------------------
// Store error strings in flash to save RAM.
#define error(s) sd.errorHalt(&Serial, F(s))
#define SD_CONFIG SdSpiConfig(CS_PIN, DEDICATED_SPI, SPI_CLOCK)

#if BHI260_IRQ > 0
#define USING_SENSOR_IRQ_METHOD
#endif

#ifdef USING_SENSOR_IRQ_METHOD
bool isReadyFlag = false;

void dataReadyISR()
{
    isReadyFlag = true;
}
#endif /*USING_SENSOR_IRQ_METHOD*/

void parse_bme280_sensor_data(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp, void *user_data)
{
    float humidity = 0;
    float temperature = 0;
    float pressure = 0;
    switch (sensor_id) {
    case SensorBHI260AP::HUMIDITY:
        bhy2_parse_humidity(data_ptr, &humidity);
        Serial.print("humidity:"); Serial.print(humidity); Serial.println("%");
        break;
    case SensorBHI260AP::TEMPERATURE:
        bhy2_parse_temperature_celsius(data_ptr, &temperature);
        Serial.print("temperature:"); Serial.print(temperature); Serial.println("*C");
        break;
    case SensorBHI260AP::BAROMETER:
        bhy2_parse_pressure(data_ptr, &pressure);
        Serial.print("pressure:"); Serial.print(pressure); Serial.println("hPa");
        break;
    default:
        Serial.println("Unkown.");
        break;
    }
}

void printResult(uint8_t sensor_id, float sample_rate, bool rlst)
{
    const char  *sensorName = bhy.getSensorName(sensor_id);
    Serial.print("Configure ");
    Serial.print(sensorName);
    Serial.print(" sensor ");
    Serial.print(sample_rate, 2);
    Serial.print(" HZ ");
    if (rlst) {
        Serial.print("successfully");
    } else {
        Serial.print("failed");
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    // In this example, BHI260 and SD Card are on the same SPI bus
    SPI.setPins(SPI_MISO, SPI_SCK, SPI_MOSI);

    SPI.begin();

    /***************************************
      *  SD CARD
      ***************************************/
    //  If multiple SPI peripherals are mounted on a single bus, first set the CS of other peripherals to HIGH
    const uint8_t other_spi_dev_cs_pin[] = {5, 28, 40};
    for (size_t i = 0; i < sizeof(other_spi_dev_cs_pin); ++i) {
        pinMode(other_spi_dev_cs_pin[i], OUTPUT);
        digitalWrite(other_spi_dev_cs_pin[i], HIGH);
    }

    // Initialize the SD card.
    Serial.println("Initializing SD Card ...");
    if (!sd.begin(SD_CONFIG)) {
        sd.initErrorHalt(&Serial);
        while (1);
    } else {
        Serial.println(" success");
    }

    File firmware_file = sd.open("/BHI260AP_aux_BMM150_BME280_GPIO-flash.fw", FILE_READ);
    if (!firmware_file) {
        Serial.println("Open firmware file failed!");
        while (1);
    }

    size_t fw_size = firmware_file.size();
    Serial.println("Read firmware file successfully .");

    uint8_t *firmware = (uint8_t *)malloc(fw_size);
    if (!firmware) {
        Serial.println("malloc memory failed!");
        while (1);
    }

    firmware_file.readBytes(firmware, fw_size);

    firmware_file.close();


    /***************************************
      *  BHI260 Initializing
      ***************************************/
    Serial.println("Initializing Sensors...");
    // Set the reset pin
    bhy.setPins(BHI260_RST);
    // Force update of the current firmware, regardless of whether it exists.
    // After uploading the firmware once, you can change it to false to speed up the startup time.
    bool force_update = true;
    // true : Write firmware to flash , false : Write to ram
    bool write_to_flash = true;
    // Set the firmware array address and firmware size
    bhy.setFirmware(firmware, fw_size, write_to_flash, force_update);
    // Set to load firmware from flash or ram
    bhy.setBootFromFlash(write_to_flash);

#ifdef USE_I2C_INTERFACE
    // Using I2C interface
    // BHI260AP_SLAVE_ADDRESS_L = 0x28
    // BHI260AP_SLAVE_ADDRESS_H = 0x29
    if (!bhy.begin(Wire, BHI260AP_SLAVE_ADDRESS_L, BHI260_SDA, BHI260_SCL)) {
        Serial.print("Failed to initialize sensor - error code:");
        Serial.println(bhy.getError());
        while (1) {
            delay(1000);
        }
    }
#endif

#ifdef USE_SPI_INTERFACE
    // Using SPI interface
    if (!bhy.begin(SPI, BHI260_CS, SPI_MOSI, SPI_MISO, SPI_SCK)) {
        Serial.print("Failed to initialize sensor - error code:");
        Serial.println(bhy.getError());
        while (1) {
            delay(1000);
        }
    }
#endif

    // Release the requested memory space
    free(firmware);

    Serial.println("Initializing the sensor successfully!");

    // Output all sensors info to Serial
    BoschSensorInfo info = bhy.getSensorInfo();
#ifdef PLATFORM_HAS_PRINTF
    info.printInfo(Serial);
#else
    info.printInfo();
#endif

    /*
    * Enable monitoring.
    * sample_rate ​​can only control the rate of the pressure sensor.
    * Temperature and humidity will only be updated when there is a change.
    * * */
    float sample_rate = 1.0;      /* Set to 1Hz update frequency */
    uint32_t report_latency_ms = 0; /* Report immediately */
    bool rlst = false;

    rlst = bhy.configure(SensorBHI260AP::TEMPERATURE, sample_rate, report_latency_ms);
    printResult(SensorBHI260AP::TEMPERATURE, sample_rate, rlst);
    rlst = bhy.configure(SensorBHI260AP::BAROMETER, sample_rate, report_latency_ms);
    printResult(SensorBHI260AP::BAROMETER, sample_rate, rlst);
    rlst = bhy.configure(SensorBHI260AP::HUMIDITY, sample_rate, report_latency_ms);
    printResult(SensorBHI260AP::HUMIDITY, sample_rate, rlst);

    // Register BME280 data parse callback function
    Serial.println("Register sensor result callback function");
    bhy.onResultEvent(SensorBHI260AP::TEMPERATURE, parse_bme280_sensor_data);
    bhy.onResultEvent(SensorBHI260AP::HUMIDITY, parse_bme280_sensor_data);
    bhy.onResultEvent(SensorBHI260AP::BAROMETER, parse_bme280_sensor_data);

#ifdef USING_SENSOR_IRQ_METHOD
    // Set the specified pin (BHI260_IRQ) ​​to an input pin.
    // This makes the pin ready to receive external signals.
    // If the interrupt is already connected, if BHI260_IRQ is equal to -1 then the polling method will be used
    pinMode(BHI260_IRQ, INPUT);

    // Attach an interrupt service routine (ISR) to the specified pin (BHI260_IRQ).
    // The ISR 'dataReadyISR' will be called whenever a rising edge is detected on the pin.
    attachInterrupt(BHI260_IRQ, dataReadyISR, RISING);
#endif
}

void loop()
{
#ifdef USING_SENSOR_IRQ_METHOD
    if (isReadyFlag) {
        isReadyFlag = false;
#endif /*USING_SENSOR_IRQ_METHOD*/

        /* If the interrupt is connected to the sensor and BHI260_IRQ is not equal to -1,
         * the interrupt function will be enabled, otherwise the method of polling the sensor is used
         */
        bhy.update();

#ifdef USING_SENSOR_IRQ_METHOD
    }
#endif /*USING_SENSOR_IRQ_METHOD*/

    delay(50);
}
#else
void setup() {}
void loop() {}
#endif
