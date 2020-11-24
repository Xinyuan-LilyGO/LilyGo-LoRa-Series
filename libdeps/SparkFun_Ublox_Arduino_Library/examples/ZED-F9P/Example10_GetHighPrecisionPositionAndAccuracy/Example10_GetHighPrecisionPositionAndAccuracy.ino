/*
  Get the high precision geodetic solution for latitude and longitude
  By: Nathan Seidle
  Modified by: Steven Rowland and Paul Clark
  SparkFun Electronics
  Date: April 17th, 2020
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to inspect the accuracy of the high-precision
  positional solution. Please see below for information about the units.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005

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
  while (!Serial); //Wait for user to open terminal

  Wire.begin();

  //myGPS.enableDebugging(Serial);

  if (myGPS.begin(Wire) == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.setNavigationFrequency(20); //Set output to 20 times a second

  byte rate = myGPS.getNavigationFrequency(); //Get the update rate of this module
  Serial.print("Current update rate: ");
  Serial.println(rate);

  //myGPS.saveConfiguration(); //Save the current settings to flash and BBR
}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    // getHighResLatitude: returns the latitude from HPPOSLLH as an int32_t in degrees * 10^-7
    // getHighResLatitudeHp: returns the high resolution component of latitude from HPPOSLLH as an int8_t in degrees * 10^-9
    // getHighResLongitude: returns the longitude from HPPOSLLH as an int32_t in degrees * 10^-7
    // getHighResLongitudeHp: returns the high resolution component of longitude from HPPOSLLH as an int8_t in degrees * 10^-9
    // getElipsoid: returns the height above ellipsoid as an int32_t in mm
    // getElipsoidHp: returns the high resolution component of the height above ellipsoid as an int8_t in mm * 10^-1
    // getMeanSeaLevel: returns the height above mean sea level as an int32_t in mm
    // getMeanSeaLevelHp: returns the high resolution component of the height above mean sea level as an int8_t in mm * 10^-1
    // getHorizontalAccuracy: returns the horizontal accuracy estimate from HPPOSLLH as an uint32_t in mm * 10^-1

    // If you want to use the high precision latitude and longitude with the full 9 decimal places
    // you will need to use a 64-bit double - which is not supported on all platforms

    // To allow this example to run on standard platforms, we cheat by converting lat and lon to integer and fractional degrees

    // The high resolution altitudes can be converted into standard 32-bit float

    // First, let's collect the position data
    int32_t latitude = myGPS.getHighResLatitude();
    int8_t latitudeHp = myGPS.getHighResLatitudeHp();
    int32_t longitude = myGPS.getHighResLongitude();
    int8_t longitudeHp = myGPS.getHighResLongitudeHp();
    int32_t ellipsoid = myGPS.getElipsoid();
    int8_t ellipsoidHp = myGPS.getElipsoidHp();
    int32_t msl = myGPS.getMeanSeaLevel();
    int8_t mslHp = myGPS.getMeanSeaLevelHp();
    uint32_t accuracy = myGPS.getHorizontalAccuracy();

    // Defines storage for the lat and lon units integer and fractional parts
    int32_t lat_int; // Integer part of the latitude in degrees
    int32_t lat_frac; // Fractional part of the latitude
    int32_t lon_int; // Integer part of the longitude in degrees
    int32_t lon_frac; // Fractional part of the longitude

    // Calculate the latitude and longitude integer and fractional parts
    lat_int = latitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
    lat_frac = latitude - (lat_int * 10000000); // Calculate the fractional part of the latitude
    lat_frac = (lat_frac * 100) + latitudeHp; // Now add the high resolution component
    if (lat_frac < 0) // If the fractional part is negative, remove the minus sign
    {
      lat_frac = 0 - lat_frac;
    }
    lon_int = longitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
    lon_frac = longitude - (lon_int * 10000000); // Calculate the fractional part of the longitude
    lon_frac = (lon_frac * 100) + longitudeHp; // Now add the high resolution component
    if (lon_frac < 0) // If the fractional part is negative, remove the minus sign
    {
      lon_frac = 0 - lon_frac;
    }

    // Print the lat and lon
    Serial.print("Lat (deg): ");
    Serial.print(lat_int); // Print the integer part of the latitude
    Serial.print(".");
    Serial.print(lat_frac); // Print the fractional part of the latitude
    Serial.print(", Lon (deg): ");
    Serial.print(lon_int); // Print the integer part of the latitude
    Serial.print(".");
    Serial.println(lon_frac); // Print the fractional part of the latitude

    // Now define float storage for the heights and accuracy
    float f_ellipsoid;
    float f_msl;
    float f_accuracy;

    // Calculate the height above ellipsoid in mm * 10^-1
    f_ellipsoid = (ellipsoid * 10) + ellipsoidHp;
    // Now convert to m
    f_ellipsoid = f_ellipsoid / 10000.0; // Convert from mm * 10^-1 to m

    // Calculate the height above mean sea level in mm * 10^-1
    f_msl = (msl * 10) + mslHp;
    // Now convert to m
    f_msl = f_msl / 10000.0; // Convert from mm * 10^-1 to m

    // Convert the horizontal accuracy (mm * 10^-1) to a float
    f_accuracy = accuracy;
    // Now convert to m
    f_accuracy = f_accuracy / 10000.0; // Convert from mm * 10^-1 to m

    // Finally, do the printing
    Serial.print("Ellipsoid (m): ");
    Serial.print(f_ellipsoid, 4); // Print the ellipsoid with 4 decimal places

    Serial.print(", Mean Sea Level(m): ");
    Serial.print(f_msl, 4); // Print the mean sea level with 4 decimal places

    Serial.print(", Accuracy (m): ");
    Serial.println(f_accuracy, 4); // Print the accuracy with 4 decimal places
  }
}
