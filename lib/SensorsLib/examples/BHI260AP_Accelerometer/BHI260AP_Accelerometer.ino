/**
 *
 * @license MIT License
 *
 * Copyright (c) 2023 lewis he
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
 * @file      BHI260AP_Accelerometer.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-09-06
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorBHI260AP.hpp"

#ifdef BHY2_USE_I2C
#define BHI260AP_SDA          21
#define BHI260AP_SCL          22
#define BHI260AP_IRQ          39
#else
#define BHI260AP_MOSI         33
#define BHI260AP_MISO         34
#define BHI260AP_SCK          35
#define BHI260AP_CS           36
#define BHI260AP_INT          37
#define BHI260AP_RST          -1
#endif


SensorBHI260AP bhy;

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    bhy.setPins(BHI260AP_RST, BHI260AP_INT);

#ifdef BHY2_USE_I2C
    if (!bhy.init(Wire, BHI260AP_SLAVE_ADDRESS, BHI260AP_SDA, BHI260AP_SCL)) {
        Serial.println("Failed to find BHI260AP - check your wiring!");
        while (1) {
            delay(1000);
        }
    }
#else
    if (!bhy.init(SPI, BHI260AP_CS, BHI260AP_MOSI, BHI260AP_MISO, BHI260AP_SCK)) {
        Serial.println("Failed to find BHI260AP - check your wiring!");
        while (1) {
            delay(1000);
        }
    }
#endif

    Serial.println("Init BHI260AP Sensor success!");


    bhy.printSensors(Serial);

    //TODO:
}


void loop()
{
    delay(50);
}



