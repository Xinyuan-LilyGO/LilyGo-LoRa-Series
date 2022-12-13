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
 * @file      QMI8658_WakeOnMotion.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2022-11-05
 *
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SensorQMI8658.hpp"
#include "boards.h"


SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;


bool interruptFlag = false;

void setFlag(void)
{
    interruptFlag = true;

}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    initBoard();

    // Initialize SPI bus, SDCard shares SPI bus with QMI8658
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    pinMode(SPI_CS, OUTPUT);    //sdcard pin set high
    digitalWrite(SPI_CS, HIGH);

    if (!qmi.begin(IMU_CS)) {
        Serial.println("Failed to find QMI8658 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }


    /* Get chip id*/
    Serial.print("Device ID:");
    Serial.println(qmi.getChipID(), HEX);

    // enabling wake on motion low power mode with a threshold of 120 mg and
    // an accelerometer data rate of 128 Hz.
    qmi.configWakeOnMotion();

    /*
    * When the QMI8658 is configured as Wom, the interrupt level is arbitrary,
    * not absolute high or low, and it is in the jump transition state
    */
    pinMode(IMU_INT, INPUT_PULLUP);
    attachInterrupt(IMU_INT, setFlag, CHANGE);


    // Print register configuration information
    qmi.dumpCtrlRegister();
}


void loop()
{

    if (interruptFlag) {
        interruptFlag = false;
        uint8_t status =  qmi.getStatusRegister();
        Serial.printf("status:0x%X BIN:", status);
        Serial.println(status, BIN);

        if (status & SensorQMI8658::EVENT_SIGNIFICANT_MOTION) {
            Serial.println("EVENT_SIGNIFICANT_MOTION");
        } else  if (status & SensorQMI8658::EVENT_NO_MOTION) {
            Serial.println("EVENT_NO_MOITON");
        } else  if (status & SensorQMI8658::EVENT_ANY_MOTION) {
            Serial.println("EVENT_ANY_MOTION");
        } else  if (status & SensorQMI8658::EVENT_PEDOMETER_MOTION) {
            Serial.println("EVENT_PEDOMETER_MOTION");
        } else  if (status & SensorQMI8658::EVENT_WOM_MOTION) {
            Serial.println("EVENT_WOM_MOTION");
        } else  if (status & SensorQMI8658::EVENT_TAP_MOTION) {
            Serial.println("EVENT_TAP_MOTION");
        }
    }
}



