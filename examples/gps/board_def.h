/*
*   This factory is just to test LilyGo T-Beam series hardware
*   Created by Lewis he
* */
#ifndef BOARD_DEF_H
#define BOARD_DEF_H

// #define T_BEAM_V07
#define T_BEAM_V10      //same v1.1 version

#if defined(T_BEAM_V07)
#define GPS_RX_PIN 12
#define GPS_TX_PIN 15
#define BUTTON_PIN 39
#define BUTTON_PIN_MASK GPIO_SEL_39
#elif defined(T_BEAM_V10)
#define GPS_RX_PIN 34
#define GPS_TX_PIN 12
#define BUTTON_PIN 38
#define BUTTON_PIN_MASK GPIO_SEL_38
#endif

#define I2C_SDA             21
#define I2C_SCL             22
#define PMU_IRQ             35

#endif /*BOARD_DEF_H*/