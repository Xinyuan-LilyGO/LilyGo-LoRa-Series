
//This program only tests T3_V1.6.1
#include <LoRa.h>
#include "boards.h"

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println("sleep test");

    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    delay(2000);

    u8g2->sleepOn();
    LoRa.sleep();

    SPI.end();
    SDSPI.end();

    pinMode(RADIO_CS_PIN, INPUT);
    pinMode(RADIO_RST_PIN, INPUT);
    pinMode(RADIO_DI0_PIN, INPUT);
    pinMode(RADIO_CS_PIN, INPUT);
    pinMode(I2C_SDA, INPUT);
    pinMode(I2C_SDA, INPUT);
    pinMode(I2C_SCL, INPUT);
    pinMode(OLED_RST, INPUT);
    pinMode(RADIO_SCLK_PIN, INPUT);
    pinMode(RADIO_MISO_PIN, INPUT);
    pinMode(RADIO_MOSI_PIN, INPUT);
    pinMode(SDCARD_MOSI, INPUT);
    pinMode(SDCARD_MISO, INPUT);
    pinMode(SDCARD_SCLK, INPUT);
    pinMode(SDCARD_CS, INPUT);
    pinMode(BOARD_LED, INPUT);
    pinMode(ADC_PIN, INPUT);

    delay(2000);

    esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);
    esp_deep_sleep_start();

}

void loop()
{
}
