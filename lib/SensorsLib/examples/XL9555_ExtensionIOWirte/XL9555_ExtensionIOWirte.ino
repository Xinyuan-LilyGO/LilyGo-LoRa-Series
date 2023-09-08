/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
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
 * @file      XL9555_ExtensionIOWirte.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-01-03
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <time.h>
#include "ExtensionIOXL9555.hpp"

#define I2C_SDA                     8
#define I2C_SCL                     9
#define XL_IRQ                      3

ExtensionIOXL9555 extIO;

void setup()
{
    Serial.begin(115200);
    while (!Serial);


    Wire.begin(I2C_SDA, I2C_SCL);
    // Device address 0x20~0x27
    if (!extIO.begin(Wire, XL9555_SLAVE_ADDRESS4, I2C_SDA, I2C_SCL)) {
        Serial.println("Failed to find XL9555 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }
    // Set PORT0 as output
    extIO.configPort(ExtensionIOXL9555::PORT0, OUTPUT);
    // Set PORT1 as output
    extIO.configPort(ExtensionIOXL9555::PORT1, OUTPUT);
}

void loop()
{
    // Set all PORTs to 1, and the parameters here are mask values, corresponding to the 0~7 bits
    Serial.println("Set port HIGH");
    extIO.writePort(ExtensionIOXL9555::PORT0, 0xFF);
    delay(1000);

    Serial.println("Set port LOW");
    // Set all PORTs to 0, and the parameters here are mask values, corresponding to the 0~7 bits
    extIO.writePort(ExtensionIOXL9555::PORT1, 0x00);
    delay(1000);

    Serial.println("digitalWrite");
    extIO.digitalWrite(ExtensionIOXL9555::IO0, HIGH);
    delay(1000);

    Serial.println("digitalToggle");
    extIO.digitalToggle(ExtensionIOXL9555::IO0);
    delay(1000);


}



