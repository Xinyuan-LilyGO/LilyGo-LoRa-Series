/*
  Change the I2C address of a Ublox module using I2C
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 3rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to change the I2C address of a Ublox module

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

long lastTime = 0; //Tracks the passing of 2000ms (2 seconds)

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example");

  Wire.begin();

  byte oldAddress = 0x42; //The default address for Ublox modules is 0x42
  byte newAddress = 0x3F; //Address you want to change to. Valid is 0x08 to 0x77.

  while (Serial.available()) Serial.read(); //Trash any incoming chars
  Serial.print("Press a key to change address to 0x");
  Serial.println(newAddress, HEX);
  while (Serial.available() == false) ; //Wait for user to send character
  
  if (myGPS.begin(Wire, oldAddress) == true) //Connect to the Ublox module using Wire port and the old address
  {
    Serial.print("GPS found at address 0x");
    Serial.println(oldAddress, HEX);

    myGPS.setI2CAddress(newAddress); //Change I2C address of this device
    //Device's I2C address is stored to memory and loaded on each power-on

    if (myGPS.begin(Wire, newAddress) == true)
    {
      myGPS.saveConfiguration(); //Save the current settings to flash and BBR
      
      Serial.print("Address successfully changed to 0x");
      Serial.println(newAddress, HEX);
      Serial.print("Now load another example sketch using .begin(Wire, 0x");
      Serial.print(newAddress, HEX);
      Serial.println(") to use this GPS module");
      Serial.println("Freezing...");
      while (1);
    }
  }

  //Something went wrong, begin looking for the I2C device
  Serial.println("Address change failed. Beginning an I2C scan.");

  Wire.begin();
}

void loop() {

  byte address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}
