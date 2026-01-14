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
 * @file      BHI260AP_Klio_Recognition.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2025-02-02
 * @note      Changed from Boschsensortec API https://github.com/boschsensortec/BHY2_SensorAPI
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorBHI260AP.hpp"
#include "SensorBHI260AP_Klio.hpp"

// #define USE_I2C_INTERFACE        true
// #define USE_SPI_INTERFACE        true

#if !defined(USE_I2C_INTERFACE) && !defined(USE_SPI_INTERFACE)
#define USE_I2C_INTERFACE
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
SensorBHI260AP_Klio klio(bhy);

// The firmware runs in RAM and will be lost if the power is off. The firmware will be loaded from RAM each time it is run.
#define BOSCH_BHI260_KLIO

// Firmware is stored in flash and booted from flash,Depends on BHI260 hardware connected to SPI Flash
// #define BOSCH_BHI260_KLIO_FLASH
// #define BOSCH_BHI260_KLIO_TURBO_FLASH

#include <BoschFirmware.h>

// Force update of current firmware, whether it exists or not.
// Only works when external SPI Flash is connected to BHI260.
// After uploading firmware once, you can change this to false to speed up boot time.
bool force_update_flash_firmware = true;


/* Action 1 mode, BHI260 should point upwards, for action instructions,
see action1.gif in the example directory */
uint8_t action1_pattern_id = 1;
uint8_t action1_pattern[] = {
    0x52, 0x42, 0x31, 0x06, 0x03, 0xfd, 0xad, 0x80,
    0x40, 0x0a, 0xd7, 0x23, 0x3c, 0x78, 0xe2, 0x44,
    0xbf, 0x63, 0xe1, 0x0d, 0xc0, 0x19, 0x39, 0x97,
    0xbf, 0xdb, 0x93, 0x04, 0x3f, 0xce, 0x07, 0xb7,
    0x3e, 0x5e, 0xda, 0xf0, 0x3d, 0xe3, 0x6f, 0x8f,
    0x3e, 0x65, 0x7c, 0x4f, 0x40, 0x46, 0x3f, 0xb4,
    0x3f, 0xdf, 0xd1, 0x3d, 0xbf, 0xfa, 0x5a, 0x82,
    0xbf, 0x35, 0xf6, 0x16, 0x3e, 0xbe, 0x70, 0x82,
    0x40, 0xaa, 0x21, 0x70, 0x41, 0xcb, 0x27, 0xf0,
    0x40, 0x19, 0x06, 0xd9, 0xbf, 0x3a, 0x10, 0xa7,
    0xbf, 0x27, 0x07, 0x31, 0x3f, 0x27, 0x23, 0xc9,
    0xbd, 0x44, 0x29, 0x2f, 0x40, 0xa6, 0x61, 0x97,
    0xc0, 0x29, 0x5d, 0x21, 0xbe, 0x82, 0xd4, 0x0d,
    0x3e, 0xc0, 0xf0, 0x15, 0x3d, 0x00, 0xbc, 0xda,
    0x3d, 0x14, 0x0c, 0xc5, 0xbd, 0x46, 0xa0, 0x03,
    0x3e, 0xca, 0x5c, 0x95, 0x3d, 0x24, 0xe5, 0x13,
    0x3c, 0x70, 0x0a, 0x81, 0x3c, 0x69, 0x22, 0xd6,
    0x3c, 0x51, 0xa4, 0xdf, 0x3e, 0x4c, 0xa8, 0x55,
    0xbf, 0xe1, 0xe8, 0xc7, 0xbd, 0xe8, 0x7c, 0xbe,
    0x3d, 0xf7, 0x5b, 0x21, 0x3c
};

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

// Firmware update progress callback
void progress_callback(void *user_data, uint32_t total, uint32_t transferred)
{
    float progress = (float)transferred / total * 100;
    Serial.print("Upload progress: ");
    Serial.print(progress);
    Serial.println("%");
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    // Set the reset pin
    bhy.setPins(BHI260_RST);

    // Set the firmware array address and firmware size
    bhy.setFirmware(bosch_firmware_image, bosch_firmware_size, bosch_firmware_type, force_update_flash_firmware);

    // Set the firmware update processing progress callback function
    // bhy.setUpdateProcessCallback(progress_callback, NULL);

    // Set the maximum transfer bytes of I2C/SPI,The default size is I2C 32 bytes, SPI 256 bytes.
    // bhy.setMaxiTransferSize(256);

    // Set the processing fifo data buffer size,The default size is 512 bytes.
    // bhy.setProcessBufferSize(1024);

    // Set to load firmware from flash
    bhy.setBootFromFlash(bosch_firmware_type);

    Serial.println("Initializing Sensors...");

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

    Serial.println("Initializing the sensor successfully!");

    // Output all sensors info to Serial
    BoschSensorInfo info = bhy.getSensorInfo();
#ifdef PLATFORM_HAS_PRINTF
    info.printInfo(Serial);
#else
    info.printInfo();
#endif

    // Attempt to initialize the KLIO sensor.
    if (!klio.begin()) {
        while (1) {
            Serial.println("Failed to initialize Klio sensor. Are you currently using a firmware that includes Klio sensor functionality?");
            delay(1000);
        }
    }

    // Call the getMaxPatterns() method of the klio object to get the maximum number of patterns allowed by the KLIO sensor.
    // This method returns a value of type uint8_t representing the maximum number of patterns and stores it in the variable max_patterns.
    uint8_t max_patterns = klio.getMaxPatterns();
    Serial.print("Klio sensor max patterns:");
    Serial.println(max_patterns);

    // Set the callback function for the recognition event of the KLIO sensor.
    // The callback function takes a pattern ID, a count value, and a pointer to user data as parameters.
    // When a recognition event occurs, the callback function will be called, and it will print
    // information about the recognized pattern (pattern ID and count) to the serial monitor.
    // The user data pointer setting can be set to nullptr, or custom data can be passed in.
    // If the recognition action is successful, the pointer is passed to the callback function
    klio.setRecognitionCallback([](uint8_t pattern_id, float count, void *user_data) {
        Serial.print("<-Recognition[Id:");
        Serial.print(pattern_id);
        Serial.print(" Count:");
        Serial.print(count);
        Serial.print("]");
    }, nullptr);

    // Try to write a pattern to the KLIO sensor.
    // The pattern ID is specified by action1_pattern_id,
    // and the pattern data is stored in the action1_pattern array.
    // The size of the pattern data is determined by sizeof(action1_pattern).
    if (!klio.writePattern(action1_pattern_id,
                           action1_pattern,
                           sizeof(action1_pattern))) {
        Serial.println("Klio write pattern failed!");
    }

    // Start the recognition process for a specific pattern.
    // Pass the address of action1_pattern_id (indicating the pattern to be recognized)
    // and the number of patterns (1 in this case) to the recognition function.
    klio.recognition(&action1_pattern_id, 1);

    // Define the sample rate at which data will be read from the KLIO sensor.
    // Here, the sample rate is set to 25Hz, meaning data will be read 25 times per second.
    float sample_rate = 25.0;

    // Define the report latency in milliseconds.
    // A value of 0 means that the sensor will report data immediately as it is measured.
    uint32_t report_latency_ms = 0;

    // Enable the KLIO sensor with he specified sample rate and report latency.
    // Once enabled, the sensor will start collecting and reporting data according to these settings.
    klio.enable(sample_rate, report_latency_ms);

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
