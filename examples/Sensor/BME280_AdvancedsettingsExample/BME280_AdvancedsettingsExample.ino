/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "boards.h"



#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

unsigned long delayTime;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("BME280 test"));

    initBoard();

    if (! bme.begin(0x77, &Wire)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        if (u8g2) {
            u8g2->setFont(u8g2_font_ncenB08_tr);
            u8g2->clearBuffer();
            u8g2->setCursor(0, 16);
            u8g2->print("BME280 Could not find");
            u8g2->sendBuffer();
        }
        while (1);
    }

    Serial.println("-- Default Test --");
    Serial.println("normal mode, 16x oversampling for all, filter off,");
    Serial.println("0.5ms standby period");
    delayTime = 5000;


    // For more details on the following scenarious, see chapter
    // 3.5 "Recommended modes of operation" in the datasheet

    /*
        // weather monitoring
        Serial.println("-- Weather Station Scenario --");
        Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
        Serial.println("filter off");
        bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_OFF   );

        // suggested rate is 1/60Hz (1m)
        delayTime = 60000; // in milliseconds
    */

    /*
        // humidity sensing
        Serial.println("-- Humidity Sensing Scenario --");
        Serial.println("forced mode, 1x temperature / 1x humidity / 0x pressure oversampling");
        Serial.println("= pressure off, filter off");
        bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1,   // temperature
                        Adafruit_BME280::SAMPLING_NONE, // pressure
                        Adafruit_BME280::SAMPLING_X1,   // humidity
                        Adafruit_BME280::FILTER_OFF );

        // suggested rate is 1Hz (1s)
        delayTime = 1000;  // in milliseconds
    */

    /*
        // indoor navigation
        Serial.println("-- Indoor Navigation Scenario --");
        Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
        Serial.println("0.5ms standby period, filter 16x");
        bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                        Adafruit_BME280::SAMPLING_X2,  // temperature
                        Adafruit_BME280::SAMPLING_X16, // pressure
                        Adafruit_BME280::SAMPLING_X1,  // humidity
                        Adafruit_BME280::FILTER_X16,
                        Adafruit_BME280::STANDBY_MS_0_5 );

        // suggested rate is 25Hz
        // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
        // T_ovs = 2
        // P_ovs = 16
        // H_ovs = 1
        // = 40ms (25Hz)
        // with standby time that should really be 24.16913... Hz
        delayTime = 41;
        */

    /*
    // gaming
    Serial.println("-- Gaming Scenario --");
    Serial.println("normal mode, 4x pressure / 1x temperature / 0x humidity oversampling,");
    Serial.println("= humidity off, 0.5ms standby period, filter 16x");
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X1,   // temperature
                    Adafruit_BME280::SAMPLING_X4,   // pressure
                    Adafruit_BME280::SAMPLING_NONE, // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5 );

    // Suggested rate is 83Hz
    // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5)
    // T_ovs = 1
    // P_ovs = 4
    // = 11.5ms + 0.5ms standby
    delayTime = 12;
    */

    Serial.println();
}


void loop()
{
    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode

    printValues();
    delay(delayTime);
}


void printValues()
{
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();

    if (u8g2) {
        u8g2->setFont(u8g2_font_ncenB08_tr);
        u8g2->clearBuffer();

        u8g2->setCursor(0, 16);

        u8g2->print("Temperature:");
        u8g2->print(bme.readTemperature());
        u8g2->print(" *C");

        u8g2->setCursor(0, 32);
        u8g2->print("Pressure:");
        u8g2->println(bme.readPressure() / 100.0F);
        u8g2->print(" hPa");

        u8g2->setCursor(0, 48);
        u8g2->print("Altitude:");
        u8g2->println(bme.readAltitude(SEALEVELPRESSURE_HPA));
        u8g2->print(" m");

        u8g2->setCursor(0, 64);
        u8g2->print("Humidity:");
        u8g2->println(bme.readHumidity());
        u8g2->print(" %");

        u8g2->sendBuffer();
    }

}
