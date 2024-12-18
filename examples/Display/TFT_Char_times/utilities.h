/**
 * @file      utilities.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2024-05-12
 * @last-update 2024-08-07
 */
#pragma once


// Support board list , Macro definition below, select the board definition to be used

// #define T3_V1_3_SX1276
// #define T3_V1_3_SX1278

// #define T3_V1_6_SX1276
// #define T3_V1_6_SX1278

// #define T3_V1_6_SX1276_TCXO
// #define T3_V3_0_SX1276_TCXO

// #define T_BEAM_SX1262
// #define T_BEAM_SX1276
// #define T_BEAM_SX1278
// #define T_BEAM_LR1121

// #define T_BEAM_S3_SUPREME_SX1262
// #define T_BEAM_S3_SUPREME_LR1121

// #define T3_S3_V1_2_SX1262
// #define T3_S3_V1_2_SX1276
// #define T3_S3_V1_2_SX1278
// #define T3_S3_V1_2_SX1280
// #define T3_S3_V1_2_SX1280_PA
// #define T3_S3_V1_2_LR1121

// #define T_MOTION

// #define T3_C6

// #define T_BEAM_S3_BPF


#define UNUSED_PIN                   (0)

#if defined(T_BEAM_SX1262) || defined(T_BEAM_SX1276) || defined(T_BEAM_SX1278) || defined(T_BEAM_LR1121)


#if   defined(T_BEAM_SX1262)
#ifndef USING_SX1262
#define USING_SX1262
#endif
#elif defined(T_BEAM_SX1276)
#ifndef USING_SX1276
#define USING_SX1276
#endif
#elif defined(T_BEAM_SX1278)
#ifndef USING_SX1278
#define USING_SX1278
#endif
#elif defined(T_BEAM_LR1121)
#ifndef USING_LR1121
#define USING_LR1121
#endif
#endif // T_BEAM_SX1262


#define GPS_RX_PIN                  34
#define GPS_TX_PIN                  12
#define BUTTON_PIN                  38
#define BUTTON_PIN_MASK             GPIO_SEL_38
#define I2C_SDA                     21
#define I2C_SCL                     22
#define PMU_IRQ                     35

#define RADIO_SCLK_PIN               5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN              26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
// SX1276/78
#define RADIO_DIO2_PIN              32
// SX1262
#define RADIO_BUSY_PIN              32

// LR1121 Only
#define RADIO_DIO9_PIN              33


#define BOARD_LED                   4
#define LED_ON                      LOW
#define LED_OFF                     HIGH

#define BUTTON_PIN                  38

#define GPS_BAUD_RATE               9600
#define HAS_GPS
#define HAS_DISPLAY                 //Optional, bring your own board, no OLED !!
#define HAS_PMU

#define BOARD_VARIANT_NAME          "T-Beam"
#define DISPLAY_MODEL_SSD_LIB       SSD1306Wire

#elif defined(T3_V1_3_SX1276) || defined(T3_V1_3_SX1278)


#if   defined(T3_V1_3_SX1276)

#ifndef USING_SX1276
#define USING_SX1276
#endif

#elif defined(T3_V1_3_SX1278)

#ifndef USING_SX1278
#define USING_SX1278
#endif

#endif // T3_V1_3_SX1276



#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSED_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               14
#define RADIO_DIO1_PIN              33

// SX1276/78
#define RADIO_DIO2_PIN              32
// SX1262
#define RADIO_BUSY_PIN              32


#define ADC_PIN                     35
#define HAS_DISPLAY
#define BOARD_VARIANT_NAME          "T3 V1.3"

#elif defined(T3_V1_6_SX1276) || defined(T3_V1_6_SX1278)


#if   defined(T3_V1_6_SX1276)
#ifndef USING_SX1276
#define USING_SX1276
#endif
#elif defined(T3_V1_6_SX1278)
#ifndef USING_SX1278
#define USING_SX1278
#endif
#endif // T3_V1_6_SX1276

#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSED_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN              26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
// SX1276/78
#define RADIO_DIO2_PIN              32
// SX1262
#define RADIO_BUSY_PIN              32

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   25
#define LED_ON                      HIGH

#define ADC_PIN                     35

#define HAS_SDCARD
#define HAS_DISPLAY

#define BOARD_VARIANT_NAME          "T3 V1.6"


#elif defined(T3_V1_6_SX1276_TCXO)

#ifndef USING_SX1276
#define USING_SX1276
#endif

#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSED_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN              26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              -1//33
/*
* In the T3 V1.6.1 TCXO version, Radio DIO1 is connected to Radio’s
* internal temperature-compensated crystal oscillator enable
* */
// TCXO pin must be set to HIGH before enabling Radio
#define RADIO_TCXO_ENABLE           33
#define RADIO_BUSY_PIN              32

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   25
#define LED_ON                      HIGH

#define ADC_PIN                     35

#define HAS_SDCARD
#define HAS_DISPLAY

#define BOARD_VARIANT_NAME          "T3 V1.6 TCXO"



#elif defined(T3_V3_0)


#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    4

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_RST_PIN               23

// TCXO pin must be set to HIGH before enabling Radio
#define RADIO_TCXO_ENABLE           12  //only sx1276 tcxo version
#define RADIO_BUSY_PIN              32


#if defined(USING_SX1262)

#define RADIO_DIO1_PIN              26
#define RADIO_BUSY_PIN              32

#elif defined(USING_SX1276) || defined(USING_SX1278)
//!SX1276/78 module only

#define RADIO_DIO0_PIN              26
#define RADIO_DIO1_PIN              32

#elif defined(USING_LR1121)

#define RADIO_DIO9_PIN              26      //LR1121 DIO9  
#define RADIO_BUSY_PIN              32      //LR1121 BUSY  

#endif

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   25
#define LED_ON                      HIGH

#define ADC_PIN                     35

#define HAS_SDCARD
#define HAS_DISPLAY

#define BOARD_VARIANT_NAME          "T3 V3.0"

#define BUTTON_PIN                  0
#define ADC_PIN                     35


#elif   defined(T3_S3_V1_2_SX1262)    ||   defined(ARDUINO_LILYGO_T3S3_SX1262)   ||    \
        defined(T3_S3_V1_2_SX1276)    ||   defined(ARDUINO_LILYGO_T3S3_SX1276)   ||    \
        defined(T3_S3_V1_2_SX1278)    ||   defined(ARDUINO_LILYGO_T3S3_SX1278)   ||    \
        defined(T3_S3_V1_2_SX1280)    ||   defined(ARDUINO_LILYGO_T3S3_SX1280)   ||    \
        defined(T3_S3_V1_2_SX1280_PA) ||   defined(ARDUINO_LILYGO_T3S3_SX1280PA) ||      \
        defined(T3_S3_V1_2_LR1121)    ||   defined(ARDUINO_LILYGO_T3S3_LR1121)


#if   defined(T3_S3_V1_2_SX1262) ||   defined(ARDUINO_LILYGO_T3S3_SX1262)
#ifndef USING_SX1262
#define USING_SX1262
#endif
#elif defined(T3_S3_V1_2_SX1276) ||   defined(ARDUINO_LILYGO_T3S3_SX1276)
#ifndef USING_SX1276
#define USING_SX1276
#endif
#elif defined(T3_S3_V1_2_SX1278) ||   defined(ARDUINO_LILYGO_T3S3_SX1278)
#ifndef USING_SX1278
#define USING_SX1278
#endif
#elif defined(T3_S3_V1_2_SX1280) ||   defined(ARDUINO_LILYGO_T3S3_SX1280)
#ifndef USING_SX1280
#define USING_SX1280
#endif
#elif defined(T3_S3_V1_2_SX1280_PA) ||   defined(ARDUINO_LILYGO_T3S3_SX1280PA)
#ifndef USING_SX1280PA
#define USING_SX1280PA
#endif
#elif defined(T3_S3_V1_2_LR1121) ||   defined(ARDUINO_LILYGO_T3S3_LR1121)
#ifndef USING_LR1121
#define USING_LR1121
#endif

#endif // T3_S3_V1_2_SX1262


#define I2C_SDA                     18
#define I2C_SCL                     17
#define OLED_RST                    UNUSED_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              3
#define RADIO_MOSI_PIN              6
#define RADIO_CS_PIN                7

#define SDCARD_MOSI                 11
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   37
#define LED_ON                      HIGH

#define BUTTON_PIN                  0
#define ADC_PIN                     1

#define RADIO_RST_PIN               8

#if defined(USING_SX1262)

#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              34

#elif defined(USING_SX1276) || defined(USING_SX1278)
//!SX1276/78 module only
#define RADIO_BUSY_PIN              33      //DIO1

#define RADIO_DIO0_PIN              9
#define RADIO_DIO1_PIN              33
#define RADIO_DIO2_PIN              34
#define RADIO_DIO3_PIN              21
#define RADIO_DIO4_PIN              10
#define RADIO_DIO5_PIN              36

#elif defined(USING_SX1280)

#define RADIO_DIO1_PIN              9       //SX1280 DIO1 = IO9
#define RADIO_BUSY_PIN              36      //SX1280 BUSY = IO36

#elif defined(USING_SX1280PA)

#define RADIO_DIO1_PIN              9       //SX1280 DIO1 = IO9
#define RADIO_BUSY_PIN              36      //SX1280 BUSY = IO36
#define RADIO_RX_PIN                21
#define RADIO_TX_PIN                10


#elif defined(USING_LR1121)

#define RADIO_DIO9_PIN              36      //LR1121 DIO9  = IO36
#define RADIO_BUSY_PIN              34      //LR1121 BUSY  = IO34

#endif

#define BUTTON_PIN                  0

#define HAS_SDCARD
#define HAS_DISPLAY

#define BOARD_VARIANT_NAME          "T3-S3-V1.X"


#elif defined(T_BEAM_S3_SUPREME_SX1262) || defined(T_BEAM_S3_SUPREME_LR1121)

#ifndef T_BEAM_S3_SUPREME
#define T_BEAM_S3_SUPREME
#endif

#if   defined(T_BEAM_S3_SUPREME_SX1262)
#ifndef USING_SX1262
#define USING_SX1262
#endif
#elif defined(T_BEAM_S3_SUPREME_LR1121)
#ifndef USING_LR1121
#define USING_LR1121
#endif
#endif

#define I2C_SDA                     (17)
#define I2C_SCL                     (18)

#define I2C1_SDA                    (42)
#define I2C1_SCL                    (41)
#define PMU_IRQ                     (40)

#define GPS_RX_PIN                  (9)
#define GPS_TX_PIN                  (8)
#define GPS_WAKEUP_PIN              (7)
#define GPS_PPS_PIN                 (6)

#define BUTTON_PIN                  (0)
#define BUTTON_PIN_MASK             (GPIO_SEL_0)
#define BUTTON_COUNT                (1)
#define BUTTON_ARRAY                {BUTTON_PIN}

#define RADIO_SCLK_PIN              (12)
#define RADIO_MISO_PIN              (13)
#define RADIO_MOSI_PIN              (11)
#define RADIO_CS_PIN                (10)
#define RADIO_DIO0_PIN              (-1)
#define RADIO_RST_PIN               (5)
#define RADIO_DIO1_PIN              (1)
#define RADIO_BUSY_PIN              (4)

// LR1121 Version
#define RADIO_DIO9_PIN               (1)

#define SPI_MOSI                    (35)
#define SPI_SCK                     (36)
#define SPI_MISO                    (37)
#define SPI_CS                      (47)
#define IMU_CS                      (34)
#define IMU_INT                     (33)

#define SDCARD_MOSI                 SPI_MOSI
#define SDCARD_MISO                 SPI_MISO
#define SDCARD_SCLK                 SPI_SCK
#define SDCARD_CS                   SPI_CS

#define RTC_INT                     (14)

#define GPS_BAUD_RATE               (9600)

#define HAS_SDCARD
#define HAS_GPS
#define HAS_DISPLAY
#define HAS_PMU

#define __HAS_SPI1__
#define HAS_SENSOR

#define PMU_WIRE_PORT               Wire1
#define DISPLAY_MODEL               U8G2_SH1106_128X64_NONAME_F_HW_I2C
#define DISPLAY_MODEL_SSD_LIB       SH1106Wire
#define BOARD_VARIANT_NAME          "T-Beam S3"

#elif defined(T_MOTION_S76G)

#ifndef USING_SX1276
#define USING_SX1276
#endif


#define RADIO_SCLK_PIN                                  PB13
#define RADIO_MISO_PIN                                  PB14
#define RADIO_MOSI_PIN                                  PB15
#define RADIO_CS_PIN                                    PB12
#define RADIO_RST_PIN                                   PB10

#define RADIO_DIO0_PIN                                  PB11
#define RADIO_DIO1_PIN                                  PC13
#define RADIO_DIO2_PIN                                  PB9
#define RADIO_DIO3_PIN                                  PB4
#define RADIO_DIO4_PIN                                  PB3
#define RADIO_DIO5_PIN                                  PA15

#undef RADIO_BUSY_PIN
#undef RADIO_DIO1_PIN
#define RADIO_BUSY_PIN                                  PC13       //DIO1
#define RADIO_DIO1_PIN                                  PB11       //DIO0

#define RADIO_SWITCH_PIN                                PA1     //1:Rx, 0:Tx

#define GPS_EN_PIN                                      PC6
#define GPS_RST_PIN                                     PB2
#define GPS_RX_PIN                                      PC11
#define GPS_TX_PIN                                      PC10
#define GPS_ENABLE_PIN                                  PC6
#define GPS_BAUD_RATE                                   115200
#define GPS_PPS_PIN                                     PB5

#define UART_RX_PIN                                     PA10
#define UART_TX_PIN                                     PA9

#define I2C_SCL                                         PB6
#define I2C_SDA                                         PB7

#define BOARD_VARIANT_NAME                             "T-Motion S76G"

#define HAS_GPS

#elif defined(T3_C6)


#ifndef USING_SX1262
#define USING_SX1262
#endif


#define RADIO_SCLK_PIN          6
#define RADIO_MISO_PIN          1
#define RADIO_MOSI_PIN          0
#define RADIO_CS_PIN            18
#define RADIO_DIO1_PIN          23
#define RADIO_BUSY_PIN          22
#define RADIO_RST_PIN           21

#define I2C_SDA                 8
#define I2C_SCL                 9

#define BOARD_LED               7
#define LED_ON                   HIGH
#define RADIO_RX_PIN                15
#define RADIO_TX_PIN                14


#define BOARD_VARIANT_NAME                             "T3-C6"

#define USING_DIO2_AS_RF_SWITCH


#elif defined(T_BEAM_S3_BPF)


#ifndef USING_SX1278
#define USING_SX1278
#endif

#define I2C_SDA                     8
#define I2C_SCL                     9

#define PMU_IRQ                     4

#define GPS_RX_PIN                  5
#define GPS_TX_PIN                  6
#define GPS_PPS_PIN                 7

#define BUTTON_PIN                  0
#define BUTTON_PIN_MASK             GPIO_SEL_0  /*BUTTON 1 = GPIO0*/
#define BUTTON_COUNT                (2)
#define BUTTON_ARRAY                {BUTTON_PIN, 3 /*BUTTON 2 = GPIO3*/}

#define RADIO_SCLK_PIN              (41)
#define RADIO_MISO_PIN              (42)
#define RADIO_MOSI_PIN              (2)
#define RADIO_CS_PIN                (1)
#define RADIO_RST_PIN               (18)

#define RADIO_DIO0_PIN              (14)
#define RADIO_DIO1_PIN              (21)
#define RADIO_DIO2_PIN              (15)

#define RADIO_TCXO_ENABLE           (17)
#define RADIO_LDO_EN                (16)
#define RADIO_BUSY_PIN              (RADIO_DIO1_PIN)

#define SPI_MOSI                    (11)
#define SPI_SCK                     (12)
#define SPI_MISO                    (13)
#define SPI_CS                      (10)

#define SDCARD_MOSI                 SPI_MOSI
#define SDCARD_MISO                 SPI_MISO
#define SDCARD_SCLK                 SPI_SCK
#define SDCARD_CS                   SPI_CS


#define GPS_BAUD_RATE               9600

#define HAS_SDCARD
#define HAS_GPS
#define HAS_DISPLAY
#define HAS_PMU

#define __HAS_SPI1__
#define HAS_SENSOR

#define PMU_WIRE_PORT               Wire
#define DISPLAY_MODEL               U8G2_SH1106_128X64_NONAME_F_HW_I2C
#define DISPLAY_MODEL_SSD_LIB       SSD1306Wire
#define BOARD_VARIANT_NAME          "T-Beam BPF"


#elif defined(T_BEAM_2W)

#ifndef USING_SX1262
#define USING_SX1262
#endif

#define I2C_SDA                     (8)
#define I2C_SCL                     (9)
#define GPS_RX_PIN                  (5)
#define GPS_TX_PIN                  (6)
#define GPS_PPS_PIN                 (7)

#define BUTTON_PIN                  (0)         /*BUTTON 1 = GPIO0*/
#define BUTTON2_PIN                 (3)         /*BUTTON 2 = GPIO3*/

#define BUTTON_PIN_MASK             GPIO_SEL_0
#define BUTTON_CONUT                (2)
#define BUTTON_ARRAY                {BUTTON_PIN,BUTTON2_PIN/*BUTTON 2 = GPIO3*/}


#define RADIO_SCLK_PIN              (16)
#define RADIO_MISO_PIN              (17)
#define RADIO_MOSI_PIN              (18)
#define RADIO_CS_PIN                (15)
#define RADIO_RST_PIN               (3)
#define RADIO_LDO_EN                (40)
#define RADIO_CTRL                  (21)

#define RADIO_DIO1_PIN              (1)
// #define RADIO_DIO3_PIN              (2)

#define RADIO_BUSY_PIN              (38)

#define SPI_MOSI                    (11)
#define SPI_SCK                     (13)
#define SPI_MISO                    (12)
#define SPI_CS                      (10)

#define SDCARD_MOSI                 SPI_MOSI
#define SDCARD_MISO                 SPI_MISO
#define SDCARD_SCLK                 SPI_SCK
#define SDCARD_CS                   SPI_CS


#define NTC_PIN                     (14)
#define FAN_CTRL                    (41)
#define ADC_PIN                     (4)

#define GPS_BAUD_RATE               9600

#define HAS_SDCARD
#define HAS_GPS
#define HAS_DISPLAY

#define __HAS_SPI1__

#define DISPLAY_MODEL               U8G2_SH1106_128X64_NONAME_F_HW_I2C
#define DISPLAY_MODEL_SSD_LIB       SH1106Wire
#define BOARD_VARIANT_NAME          "LoRa 2W"

/*
* 2w LoRa max set power is +3 dBm ,After passing through PA, the power can reach 33dBm
* -3dBm = +27dBm
* 0 dBm = +30dBm
* 3 dBm = +33dBm
* */
#define RADIO_MAX_OUTPUT_POWER      3


#else
#error "When using it for the first time, please define the board model in <utilities.h>"
#endif









