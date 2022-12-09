/**
 * @file      UBlox_Recovery.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-12-09
 * @note      This sample file is intended to restore the UBLOX GNSS module to the factory default value, and the default output baud rate is 9600
 */

#include "boards.h"

#define SerialGNSS Serial1

uint8_t buffer[256];

int getAck(uint8_t *buffer, uint16_t size, uint8_t requestedClass, uint8_t requestedID)
{
    uint16_t    ubxFrameCounter = 0;
    bool        ubxFrame = 0;
    uint32_t    startTime = millis();
    uint16_t    needRead;

    while (millis() - startTime < 800) {
        while (SerialGNSS.available()) {
            int c = SerialGNSS.read();
            switch (ubxFrameCounter) {
            case 0:
                if (c == 0xB5) {
                    ubxFrameCounter++;
                }
                break;
            case 1:
                if (c == 0x62) {
                    ubxFrameCounter++;
                } else {
                    ubxFrameCounter = 0;
                }
                break;
            case 2:
                if (c == requestedClass) {
                    ubxFrameCounter++;
                } else {
                    ubxFrameCounter = 0;
                }
                break;
            case 3:
                if (c == requestedID) {
                    ubxFrameCounter++;
                } else {
                    ubxFrameCounter = 0;
                }
                break;
            case 4:
                needRead = c;
                ubxFrameCounter++;
                break;
            case 5:
                needRead |=  (c << 8);
                ubxFrameCounter++;
                break;
            case 6:
                if (needRead >= size) {
                    ubxFrameCounter = 0;
                    break;
                }
                if (SerialGNSS.readBytes(buffer, needRead) != needRead) {
                    ubxFrameCounter = 0;
                } else {
                    return needRead;
                }
                break;

            default:
                break;
            }
        }
    }
    return 0;
}

bool recovery()
{
    uint8_t cfg_clear1[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1C, 0xA2};
    uint8_t cfg_clear2[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA1};
    uint8_t cfg_clear3[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x1D, 0xB3};
    SerialGNSS.write(cfg_clear1, sizeof(cfg_clear1));

    if (getAck(buffer, 256, 0x05, 0x01)) {
        Serial.println("Get ack successed!");
    }
    SerialGNSS.write(cfg_clear2, sizeof(cfg_clear2));
    if (getAck(buffer, 256, 0x05, 0x01)) {
        Serial.println("Get ack successed!");
    }
    SerialGNSS.write(cfg_clear3, sizeof(cfg_clear3));
    if (getAck(buffer, 256, 0x05, 0x01)) {
        Serial.println("Get ack successed!");
    }

    // UBX-CFG-RATE, Size 8, 'Navigation/measurement rate settings'
    uint8_t cfg_rate[] = {0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};
    SerialGNSS.write(cfg_rate, sizeof(cfg_rate));
    if (getAck(buffer, 256, 0x06, 0x08)) {
        Serial.println("Get ack successed!");
    } else {
        return false;
    }
    return true;
}


void setup()
{

    initBoard();

    if (recovery()) {
        Serial.println("recovery successed!");
    } else {
        Serial.println("recovery failed!");
    }

}

void loop()
{
    while (1) {
        while (Serial.available()) {
            SerialGNSS.write(Serial.read());
        }
        while (SerialGNSS.available()) {
            Serial.write(SerialGNSS.read());
        }
    }
}
