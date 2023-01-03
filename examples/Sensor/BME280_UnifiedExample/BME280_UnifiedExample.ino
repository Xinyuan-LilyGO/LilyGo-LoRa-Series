/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor
  This example shows how to take Sensor Events instead of direct readings

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2652

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include "boards.h"

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

void setup()
{
    Serial.begin(115200);
    Serial.println(F("BME280 Sensor event test"));

    initBoard();

    if (!bme.begin()) {
        Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
        if (u8g2) {
            u8g2->setFont(u8g2_font_ncenB08_tr);
            u8g2->clearBuffer();
            u8g2->setCursor(0, 16);
            u8g2->print("BME280 Could not find");
            u8g2->sendBuffer();
        }
        while (1) delay(10);
    }

    bme_temp->printSensorDetails();
    bme_pressure->printSensorDetails();
    bme_humidity->printSensorDetails();
}

void loop()
{
    sensors_event_t temp_event, pressure_event, humidity_event;
    bme_temp->getEvent(&temp_event);
    bme_pressure->getEvent(&pressure_event);
    bme_humidity->getEvent(&humidity_event);

    Serial.print(F("Temperature = "));
    Serial.print(temp_event.temperature);
    Serial.println(" *C");

    Serial.print(F("Humidity = "));
    Serial.print(humidity_event.relative_humidity);
    Serial.println(" %");

    Serial.print(F("Pressure = "));
    Serial.print(pressure_event.pressure);
    Serial.println(" hPa");

    Serial.println();


    if (u8g2) {
        u8g2->setFont(u8g2_font_ncenB08_tr);
        u8g2->clearBuffer();

        u8g2->setCursor(0, 16);

        u8g2->print("Temperature:");
        u8g2->print(temp_event.temperature);
        u8g2->print(" *C");

        u8g2->setCursor(0, 32);
        u8g2->print("Pressure:");
        u8g2->println(pressure_event.pressure);
        u8g2->print(" hPa");

        u8g2->setCursor(0, 48);
        u8g2->print("Humidity:");
        u8g2->println(humidity_event.relative_humidity);
        u8g2->print(" %");

        u8g2->sendBuffer();
    }

    delay(1000);
}