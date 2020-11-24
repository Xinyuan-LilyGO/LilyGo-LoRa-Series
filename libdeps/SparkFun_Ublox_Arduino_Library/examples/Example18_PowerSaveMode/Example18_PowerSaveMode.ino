/*
  Power Save Mode
  By: Paul Clark (PaulZC)
  Date: April 22nd, 2020

  Based extensively on Example3_GetPosition
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 3rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to put the Ublox module into power save mode and then
  query its lat/long/altitude. We also turn off the NMEA output on the I2C port.
  This decreases the amount of I2C traffic dramatically.

  ** When it is able to ** the module will reduce its current draw.
  For the ZOE-M8Q with a passive antenna, you should see the current drop
  from (approx.) 25-28mA to (approx.) 9mA when power save mode kicks in.

  Note: this will fail on the ZED (protocol version >= 27) as UBX-CFG-RXM is not supported

  Note: Long/lat are large numbers because they are * 10^7. To convert lat/long
  to something google maps understands simply divide the numbers by 10,000,000. We
  do this so that we don't have to use floating point numbers.

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

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //myGPS.enableDebugging(); // Uncomment this line to enable debug messages

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  //myGPS.saveConfiguration(); //Uncomment this line to save the current settings to flash and BBR

  Serial.println("Power save example.");
  Serial.println("1) Enable power saving");
  Serial.println("2) Disable power saving");
}

void loop()
{
  if (Serial.available())
  {
    byte incoming = Serial.read();

    if (incoming == '1')
    {
      // Put the GNSS into power save mode
      // (If you want to disable power save mode, call myGPS.powerSaveMode(false) instead)
      // This will fail on the ZED (protocol version >= 27) as UBX-CFG-RXM is not supported
      if (myGPS.powerSaveMode()) // Defaults to true
        Serial.println(F("Power Save Mode enabled."));
      else
        Serial.println(F("***!!! Power Save Mode FAILED !!!***"));
    }
    else if (incoming == '2')
    {
      //Go to normal power mode (not power saving mode)
      if (myGPS.powerSaveMode(false))
        Serial.println(F("Power Save Mode disabled."));
      else
        Serial.println(F("***!!! Power Save Disable FAILED !!!***"));
    }

    // Read and print the new low power mode
    uint8_t lowPowerMode = myGPS.getPowerSaveMode();
    if (lowPowerMode == 255)
    {
      Serial.println(F("***!!! getPowerSaveMode FAILED !!!***"));
    }
    else
    {
      Serial.print(F("The low power mode is: "));
      Serial.print(lowPowerMode);
      if (lowPowerMode == 0)
      {
        Serial.println(F(" (Continuous)"));
      }
      else if (lowPowerMode == 1)
      {
        Serial.println(F(" (Power Save)"));
      }
      else if (lowPowerMode == 4)
      {
        Serial.println(F(" (Continuous)"));
      }
      else
      {
        Serial.println(F(" (Unknown!)"));
      }
    }
  }

  //Query module every 10 seconds so it is easier to monitor the current draw
  if (millis() - lastTime > 10000)
  {
    lastTime = millis(); //Update the timer

    byte fixType = myGPS.getFixType(); // Get the fix type
    Serial.print(F("Fix: "));
    Serial.print(fixType);
    if (fixType == 0)
      Serial.print(F("(No fix)"));
    else if (fixType == 1)
      Serial.print(F("(Dead reckoning)"));
    else if (fixType == 2)
      Serial.print(F("(2D)"));
    else if (fixType == 3)
      Serial.print(F("(3D)"));
    else if (fixType == 4)
      Serial.print(F("(GNSS + Dead reckoning)"));

    long latitude = myGPS.getLatitude();
    Serial.print(F(" Lat: "));
    Serial.print(latitude);

    long longitude = myGPS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGPS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    Serial.println();
  }
}
