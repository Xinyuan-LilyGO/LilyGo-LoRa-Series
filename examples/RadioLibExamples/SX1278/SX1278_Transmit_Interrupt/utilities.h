
#pragma once

/*
* This sample program only supports SX1278
* */
// #define LILYGO_TBeam_V0_7
// #define LILYGO_TBeam_V1_X
// #define LILYGO_T3_V1_0
// #define LILYGO_T3_V1_3
// #define LILYGO_T3_V1_6
// #define LILYGO_T3_V2_0


/*
* The default program uses 433MHz,
* if you need to change it,
* please open this note and change to the frequency you need to test
* */

// #define LoRa_frequency      470.0


#define UNUSE_PIN                   (0)

#if defined(LILYGO_TBeam_V0_7)
#define GPS_RX_PIN                  12
#define GPS_TX_PIN                  15
#define BUTTON_PIN                  39
#define BUTTON_PIN_MASK             GPIO_SEL_39
#define I2C_SDA                     21
#define I2C_SCL                     22

#define RADIO_SCLK_PIN               5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define GPS_BAUD_RATE               9600
#define HAS_GPS
#define HAS_DISPLAY                 //Optional, bring your own board, no OLED !!

#elif defined(LILYGO_TBeam_V1_X)

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
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define GPS_BAUD_RATE               9600
#define HAS_GPS
#define HAS_DISPLAY                 //Optional, bring your own board, no OLED !!
#define HAS_PMU
#elif defined(LILYGO_T3_V1_0)
#define I2C_SDA                     4
#define I2C_SCL                     15
#define OLED_RST                    16

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               14
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define HAS_DISPLAY

#elif defined(LILYGO_T3_V1_3)

#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               14
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define ADC_PIN                     35

#define HAS_DISPLAY
#elif defined(LILYGO_T3_V1_6)
#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
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

#elif defined(LILYGO_T3_V2_0)
#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DIO0_PIN               26
#define RADIO_RST_PIN               14
#define RADIO_DIO1_PIN              UNUSE_PIN
#define RADIO_BUSY_PIN              UNUSE_PIN

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   0
#define LED_ON                      LOW

#define HAS_DISPLAY
#define HAS_SDCARD

#elif defined(LILYGO_T3_S3_V1_0) ||defined(LILYGO_T3_S3_V1_2)

#define I2C_SDA                     18
#define I2C_SCL                     17
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              3
#define RADIO_MOSI_PIN              6
#define RADIO_CS_PIN                7
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              34
#define RADIO_RST_PIN               8

//!SX1276/78 module only
#define RADIO_DIO0_PIN              9
#define RADIO_DIO3_PIN              21
#define RADIO_DIO4_PIN              10
#define RADIO_DIO5_PIN              36
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


#else
#error "For the first use, please define the board version and model in <utilities. h>"
#endif









