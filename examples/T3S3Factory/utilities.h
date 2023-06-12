/**
 * @file      utilities.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-05-13
 *
 */

#pragma once

/*
* This sample program only supports T3 S3
* */
// #define LILYGO_T3_S3_V1_0    //SUPPORT
// #define LILYGO_T3_S3_V1_2    //SUPPORT


#define UNUSE_PIN                   (0)


#define I2C_SDA                     18
#define I2C_SCL                     17
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              3
#define RADIO_MOSI_PIN              6
#define RADIO_CS_PIN                7
#define RADIO_DIO1_PIN              33      //SX1280 DIO1 = IO9
#define RADIO_BUSY_PIN              34      //SX1280 BUSY = IO36
#define RADIO_RST_PIN               8

//!SX1276/78 module only
#define RADIO_DIO0_PIN              9
#define RADIO_DIO3_PIN              21
#define RADIO_DIO4_PIN              10
#define RADIO_DIO5_PIN              36
//! end

//! SX1280 module only
#define RADIO_RX_PIN                21
#define RADIO_TX_PIN                10
//! end

#define SDCARD_MOSI                 11
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   37
#define LED_ON                      HIGH

#define BAT_ADC_PIN                1
#define BUTTON_PIN                 0

#define HAS_SDCARD
#define HAS_DISPLAY


#if !defined(LILYGO_T3_S3_V1_0) && !defined(LILYGO_T3_S3_V1_2)
#error "For the first use, please define the board version and model in <utilities. h>"
#endif







