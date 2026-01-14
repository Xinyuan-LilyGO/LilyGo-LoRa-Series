/**
 * @file      boards.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2024-04-25
 * @last-update 2024-08-07
 */

#pragma once


#include "utilities.h"

#ifdef HAS_SDCARD
#include <SD.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)
#include <FS.h>
#include <WiFi.h>
#endif

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#ifdef DISPLAY_MODEL
#include <U8g2lib.h>
#endif

#ifdef HAS_PMU
#include <XPowersLib.h>
#endif

#include <esp_mac.h>


#ifndef DISPLAY_ADDR
#define DISPLAY_ADDR               0x3C
#endif


// #define ENABLE_BLE      //Enable ble function

enum {
    POWERMANAGE_ONLINE  = _BV(0),
    DISPLAY_ONLINE      = _BV(1),
    RADIO_ONLINE        = _BV(2),
    GPS_ONLINE          = _BV(3),
    PSRAM_ONLINE        = _BV(4),
    SDCARD_ONLINE       = _BV(5),
    AXDL345_ONLINE      = _BV(6),
    BME280_ONLINE       = _BV(7),
    BMP280_ONLINE       = _BV(8),
    BME680_ONLINE       = _BV(9),
    QMC6310U_ONLINE      = _BV(10),
    QMC6310N_ONLINE      = _BV(11),
    QMI8658_ONLINE      = _BV(12),
    PCF8563_ONLINE      = _BV(13),
    OSC32768_ONLINE      = _BV(14)
};



typedef struct {
    String          chipModel;
    float           psramSize;
    uint8_t         chipModelRev;
    uint8_t         chipFreq;
    uint8_t         flashSize;
    uint8_t         flashSpeed;
} DevInfo_t;


void setupBoards(bool disable_u8g2 = false);

#ifdef HAS_SDCARD
bool beginSDCard();
#else
#define beginSDCard()
#endif

#ifdef DISPLAY_MODEL
bool beginDisplay();
#endif


void printResult(bool radio_online);

#ifdef BOARD_LED
void flashLed();
#else
#define flashLed()
#endif

void scanDevices(TwoWire *w);

bool beginGPS();

bool recoveryGPS();

void scanWiFi();

#ifdef HAS_PMU
extern XPowersLibInterface *PMU;
extern bool pmuInterrupt;
void loopPMU(void (*pressed_cb)(void));
bool beginPower();
void disablePeripherals();
#else
#define beginPower()
#define disablePeripherals()
#endif

#ifdef DISPLAY_MODEL
extern DISPLAY_MODEL *u8g2;
#define U8G2_HOR_ALIGN_CENTER(t)    ((u8g2->getDisplayWidth() -  (u8g2->getUTF8Width(t))) / 2)
#define U8G2_HOR_ALIGN_RIGHT(t)     ( u8g2->getDisplayWidth()  -  u8g2->getUTF8Width(t))
#endif


#if defined(ARDUINO_ARCH_ESP32)

#if defined(HAS_SDCARD)
extern SPIClass SDCardSPI;
#endif

#define SerialGPS Serial1
#elif defined(ARDUINO_ARCH_STM32)
extern HardwareSerial  SerialGPS;
#endif

#ifdef NTC_PIN
float getTempForNTC();
#endif

#ifdef ENABLE_BLE
void setupBLE();
#else
#define setupBLE()
#endif

extern uint32_t deviceOnline;
