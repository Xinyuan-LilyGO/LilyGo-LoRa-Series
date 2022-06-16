/*
  Read NMEA sentences over Serial using Ublox module SAM-M8Q, NEO-M8P, ZED-F9P, etc
  This example reads the NMEA setences from the Ublox module over Serial and outputs
  them to the serial port
  Base on SparkFun_Ublox_Arduino_Library //https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
*/
#include "SparkFun_Ublox_Arduino_Library.h"
#include "utilities.h"

SFE_UBLOX_GPS myGPS;

void setup()
{
    initBoard();

    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println("SparkFun Ublox Example");
    myGPS.enableDebugging();

    if (myGPS.begin(Serial1) == false) {
        Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
        while (1);
    }

    //This will pipe all NMEA sentences to the serial port so we can see them
    myGPS.setNMEAOutputPort(Serial);
}

void loop()
{
    myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    delay(250); //Don't pound too hard on the I2C bus
}
