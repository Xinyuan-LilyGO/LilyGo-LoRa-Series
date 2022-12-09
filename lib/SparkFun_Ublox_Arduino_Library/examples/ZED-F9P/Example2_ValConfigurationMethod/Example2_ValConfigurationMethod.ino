/*
  Configuring Ublox Module using new VALGET / VALSET / VALDEL methods
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 3rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Ublox depricated many -CFG messages and replaced them with new
  VALGET, VALSET, VALDEL methods. This shows the basics of how to use
  these methods.

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
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example");

  Wire.begin();

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  byte response;
  response = myGPS.getVal8(VAL_GROUP_I2C, VAL_ID_I2C_ADDRESS, VAL_GROUP_I2C_SIZE, VAL_LAYER_RAM);
  Serial.print(F("I2C Address: 0x"));
  Serial.println(response >> 1, HEX); //We have to shift by 1 to get the common '7-bit' I2C address format

  response = myGPS.getVal8(VAL_GROUP_I2COUTPROT, VAL_ID_I2COUTPROT_NMEA, VAL_GROUP_I2COUTPROT_SIZE, VAL_LAYER_RAM);
  Serial.print(F("Output NMEA over I2C port: 0x"));
  Serial.print(response, HEX);
}

void loop()
{
}
