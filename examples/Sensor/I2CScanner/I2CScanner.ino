/**
 * @file      I2CScanner.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-12-04
 * @note      T_BEAM_S3_SUPREME  If the I2C device is connected to pins 17 (SDA) or 18 (SCL), 
 *            the sensor power supply must be connected to DC1. If connected to other LDOs, 
 *            the power must be turned on before accessing the sensor I2C bus; otherwise, the I2C access will fail or freeze.
 */

#include <Arduino.h>
#include "LoRaBoards.h"


void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println("\nI2C Scanner Starting...");

    // Wire.begin(17, 18); // SDA, SCL
    setupBoards();
}

void loop()
{
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++ ) {
        // The i2c_scanner uses the return value of
        // the Wire.beginTransmission to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    delay(5000);           // wait 5 seconds for next scan
}