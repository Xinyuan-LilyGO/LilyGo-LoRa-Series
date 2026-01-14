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
 * @file      BHI260AP_Klio_RecognizeMultiple.ino
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

/* Action 2 mode, BHI260 should point to the right,
for action instructions, see action2.gif in the example directory*/
uint8_t action2_pattern_id = 2;
const uint8_t action2_pattern[] = {
    0x52, 0x42, 0x31, 0x06, 0x03, 0xfd, 0xad, 0x80,
    0x40, 0x0a, 0xd7, 0x23, 0x3c, 0x38, 0xf3, 0x08,
    0x40, 0x1a, 0x9e, 0x06, 0xbf, 0xde, 0xcd, 0xca,
    0x3d, 0x88, 0xb3, 0x9b, 0x3e, 0x01, 0x26, 0xb9,
    0x3d, 0x8f, 0xe9, 0x88, 0x3d, 0x6e, 0xc3, 0x13,
    0xc1, 0x4f, 0x1e, 0x09, 0x40, 0x75, 0x7a, 0x2d,
    0xc0, 0x80, 0x31, 0xdb, 0xbd, 0x2f, 0x1e, 0x83,
    0x3f, 0xc0, 0x6f, 0x10, 0x3d, 0xf7, 0xc1, 0x26,
    0xbf, 0x5a, 0x36, 0x00, 0x3f, 0x8b, 0x31, 0x77,
    0xbe, 0x51, 0x09, 0x3b, 0xbf, 0x16, 0xa9, 0xa0,
    0xbf, 0x6d, 0x90, 0x89, 0x3d, 0x39, 0x79, 0xb3,
    0x3d, 0x39, 0x0e, 0x9f, 0xbd, 0xfc, 0x77, 0x55,
    0xbd, 0x69, 0x44, 0x13, 0xbf, 0x02, 0x31, 0x14,
    0x3e, 0x60, 0xe6, 0x75, 0x3b, 0xd2, 0x69, 0x19,
    0xbc, 0xa8, 0x41, 0x46, 0x3d, 0x87, 0x45, 0x88,
    0x3e, 0x70, 0xf7, 0x87, 0x3d, 0x81, 0x8a, 0xe2,
    0x3d, 0xea, 0x15, 0x8d, 0x3b, 0xf5, 0x2e, 0xc1,
    0x3c, 0x1e, 0xcc, 0x05, 0x3e, 0x00, 0x70, 0x16,
    0x3c, 0x41, 0x8b, 0x07, 0xbe, 0x4e, 0xd8, 0xb0,
    0x3d, 0x94, 0xd9, 0x40, 0x3b
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

/**
 * @brief Callback function for KLIO sensor recognition events.
 *
 * This function serves as a callback that gets triggered when the KLIO sensor
 * successfully recognizes a pattern. It's designed to handle the recognition
 * event and provide information about the recognized pattern.
 *
 * @param pattern_id The unique identifier of the recognized pattern.
 *                   Each pattern in the KLIO sensor's recognition library
 *                   is assigned a distinct ID, and this parameter indicates
 *                   which specific pattern has been recognized.
 * @param count A floating point value representing the number of actions with
 *              the recognized pattern
 * @param user_data A generic pointer to user - defined data. This can be used
 *                  to pass additional context or information from the calling
 *                  code to the callback function.
 */
void recognition_event_callback(uint8_t pattern_id, float count, void *user_data)
{
    Serial.print("<-Recognition[Id:");
    Serial.print(pattern_id);
    Serial.print(" Count:");
    Serial.print(count);
    Serial.print("]");

    // When the recognition counter is greater than 10 times, reset the recognition counter
    if (count > 10) {

        Serial.println("Reset recognition counter!");

        // Disable Klio
        klio.disable();

        // Restart Recognition
        beginRecognition();
    }

}

void beginRecognition()
{
// Define an array to hold the IDs of the patterns to be written to the KLIO sensor.
    // 'action1_pattern_id' and 'action2_pattern_id' are predefined pattern IDs.
    uint8_t patterns_ids[] = {action1_pattern_id, action2_pattern_id};

    // Define an array of pointers to the actual pattern data.
    // 'action1_pattern' and 'action2_pattern' are arrays containing the pattern data.
    const uint8_t *patterns[] = {action1_pattern, action2_pattern};

    // Define an array to hold the sizes of each pattern.
    // Use the sizeof operator to determine the size of each pattern array.
    uint16_t patterns_sizes[] = {sizeof(action1_pattern), sizeof(action2_pattern)};

    // Calculate the number of patterns in the 'patterns_ids' array.
    // This is done by dividing the total size of the array by the size of a single element.
    uint8_t patterns_count = sizeof(patterns_ids) / sizeof(patterns_ids[0]);

    // Try to write multiple patterns to the KLIO sensor.
    // Call the writeMultiplePatterns() method with the arrays of pattern IDs, pattern data pointers,
    // pattern sizes, and the number of patterns. If the write operation fails, print an error message.
    if (!klio.writeMultiplePatterns(patterns_ids, patterns, patterns_sizes, patterns_count)) {
        Serial.println("Klio write multiple patterns failed!");
    }

    // Start the recognition process for the specified patterns.
    // Call the recognition() method with the array of pattern IDs and the number of patterns.
    klio.recognition(patterns_ids, patterns_count);

    // Define the sample rate for data reading.
    // The sample rate is set to 25.0 Hz, meaning data will be read 25 times per second.
    float sample_rate = 25.0; /* Read out data measured at 25Hz */

    // Define the report latency in milliseconds.
    // A value of 0 means that the sensor will report data immediately as it is measured.
    uint32_t report_latency_ms = 0; /* Report immediately */

    // Enable the KLIO sensor with the specified sample rate and report latency.
    // Call the enable() method to activate the sensor and configure it according to the settings.
    klio.enable(sample_rate, report_latency_ms);
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

    // Try to initialize the KLIO sensor.
    if (!klio.begin()) {
        while (1) {
            Serial.println("Failed to initialize Klio sensor. Are you currently using a firmware that includes Klio sensor functionality?");
            delay(1000);
        }
    }

    // Set the callback function for recognition events.
    // Similar to the learning callback, the setRecognitionCallback() method registers a function
    // that will be invoked when a recognition - related event happens in the KLIO sensor.
    // 'recognition_event_callback' is the callback function, and 'nullptr' is used as the user data pointer.
    klio.setRecognitionCallback(recognition_event_callback, nullptr);

    // Start Recognition
    beginRecognition();

#ifdef USING_SENSOR_IRQ_METHOD
    // Set the specified pin (BHI260_IRQ) ​​to an input pin.
    // This makes the pin ready to receive external signals.
    // If the interrupt is already connected, if BHI260_IRQ is equal to -1 then the polling method will be used
    pinMode(BHI260_IRQ, INPUT);

    // Attach an interrupt service routine (ISR) to the specified pin (BHI260_IRQ).
    // The ISR 'dataReadyISR' will be called whenever a rising edge is detected on the pin.
    attachInterrupt(BHI260_IRQ, dataReadyISR, RISING);
#endif

    Serial.println("Please check the gif(atcion1.gif,action2.gif) image in the example directory to see the action instructions."
                   "If the action meets the record value, the action record counter will be triggered.");
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
