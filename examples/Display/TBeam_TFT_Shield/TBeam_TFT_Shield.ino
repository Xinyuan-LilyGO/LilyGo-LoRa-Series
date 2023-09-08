/**
 * @file      T-Beam-TFT-Shield.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-08
 *
 */
#include <TFT_eSPI.h>         // Hardware-specific library
#include <TouchDrvCSTXXX.hpp> // Hardware-specific library  https://github.com/lewisxhe/SensorLib
#include <SPI.h>
#include "image.h"

#define GPS_RX_PIN                  34
#define GPS_TX_PIN                  12
#define BUTTON_PIN                  38
#define BUTTON_PIN_MASK             GPIO_SEL_38
#define I2C_SDA                     21
#define I2C_SCL                     22
#define PMU_IRQ                     35

#define BOARD_LED                   4
#define LED_ON                      LOW
#define LED_OFF                     HIGH

#define TFT_SHIELD_TOUCH_IRQ        35

TouchDrvCSTXXX touch;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

int16_t x[5], y[5];

void setup(void)
{
    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    Wire.begin(I2C_SDA, I2C_SCL);

    touch.init(Wire, I2C_SDA, I2C_SCL, CST226SE_SLAVE_ADDRESS);

    // Depending on the touch panel, not all touch panels have touch buttons.
    touch.setHomeButtonCallback([](void *user_data) {
        Serial.println("Home key pressed!");
        static uint32_t checkMs = 0;
        if (millis() > checkMs) {
            checkMs = millis() + 200;
            if (digitalRead(TFT_BL)) {
                digitalWrite(TFT_BL, LOW);
            } else {
                digitalWrite(TFT_BL, HIGH);
            }
        }

    }, NULL);

    tft.setSwapBytes(true);
    tft.pushColors((uint8_t *)gImage_image, tft.width() * tft.height() * 2);

    spr.setColorDepth(8);
    spr.createSprite(tft.width(), 60);
    spr.fillSprite(TFT_LIGHTGREY);
    spr.setTextDatum(CL_DATUM);
    spr.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    spr.drawString("Powered By LilyGo ST7796 222x480 IPS T-Beam TFT Shield", 60, 10, 2);
    spr.pushSprite(0, tft.height() - 60);
}

void loop()
{

    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        String buf = "";
        for (int i = 0; i < touched; ++i) {
            Serial.print("X[");
            Serial.print(i);
            Serial.print("]:");
            Serial.print(x[i]);
            Serial.print(" ");
            Serial.print(" Y[");
            Serial.print(i);
            Serial.print("]:");
            Serial.print(y[i]);
            Serial.print(" ");

            buf += "[" + String(i) + "]:";
            buf += String(x[i]);
            buf += " ";
            buf += " [" + String(i) + "]:";
            buf += String(y[i]);
            buf += " ";
        }
        Serial.println();
        spr.drawString("Powered By LilyGo ST7796 222x480 IPS T-Beam TFT Shield", 60, 10, 2);
        spr.drawString(buf, 5, 30, 2);
        spr.pushSprite(0, tft.height() - 60);
        spr.fillSprite(TFT_LIGHTGREY);
    }

    delay(5);
}








