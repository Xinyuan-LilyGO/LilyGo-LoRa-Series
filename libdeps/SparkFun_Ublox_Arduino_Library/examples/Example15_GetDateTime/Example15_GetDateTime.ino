/*
  Getting time and date using Ublox commands
  By: davidallenmann
  SparkFun Electronics
  Date: April 16th, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to query a Ublox module for the current time and date. We also
  turn off the NMEA output on the I2C port. This decreases the amount of I2C traffic
  dramatically.

  Leave NMEA parsing behind. Now you can simply ask the module for the datums you want!

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a BlackBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; //Wait for user to open terminal
    Serial.println("SparkFun Ublox Example");

    Wire.begin();

    if (myGPS.begin() == false) { //Connect to the Ublox module using Wire port
        Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
        while (1)
            ;
    }

    myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    myGPS.saveConfiguration();        //Save the current settings to flash and BBR
}

void loop()
{
    //Query module only every second. Doing it more often will just cause I2C traffic.
    //The module only responds when a new position is available
    if (millis() - lastTime > 1000) {
        lastTime = millis(); //Update the timer

        long latitude = myGPS.getLatitude();
        Serial.print(F("Lat: "));
        Serial.print(latitude);

        long longitude = myGPS.getLongitude();
        Serial.print(F(" Long: "));
        Serial.print(longitude);
        Serial.print(F(" (degrees * 10^-7)"));

        long altitude = myGPS.getAltitude();
        Serial.print(F(" Alt: "));
        Serial.print(altitude);
        Serial.print(F(" (mm)"));

        byte SIV = myGPS.getSIV();
        Serial.print(F(" SIV: "));
        Serial.print(SIV);

        Serial.println();
        Serial.print(myGPS.getYear());
        Serial.print("-");
        Serial.print(myGPS.getMonth());
        Serial.print("-");
        Serial.print(myGPS.getDay());
        Serial.print(" ");
        Serial.print(myGPS.getHour());
        Serial.print(":");
        Serial.print(myGPS.getMinute());
        Serial.print(":");
        Serial.print(myGPS.getSecond());

        Serial.print("  Time is ");
        if (myGPS.getTimeValid() == false) {
            Serial.print("not ");
        }
        Serial.print("valid  Date is ");
        if (myGPS.getDateValid() == false) {
            Serial.print("not ");
        }
        Serial.print("valid");

        Serial.println();
    }
}
