/*
  Send command to reset module over I2C
  By: Nathan Seidle
  Date: January 29rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to reset the U-Blox module to factory defaults over I2C.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Connect the U-Blox serial port to Serial1
  If you're using an Uno or don't have a 2nd serial port (Serial1), consider using software serial
  Open the serial monitor at 115200 baud to see the output
*/

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

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

  while (Serial.available()) Serial.read(); //Trash any incoming chars
  Serial.println("Press a key to reset module to factory defaults");
  while (Serial.available() == false) ; //Wait for user to send character

  myGPS.factoryReset(); //Reset everything: baud rate, I2C address, update rate, everything.

  if (myGPS.begin() == false) //Attempt to re-connect
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  Serial.println("Unit has now been factory reset. Freezing...");
  while(1);
}

void loop()
{

}
