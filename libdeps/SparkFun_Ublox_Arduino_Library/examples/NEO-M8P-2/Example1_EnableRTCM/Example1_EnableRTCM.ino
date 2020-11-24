/*
  Send UBX binary commands to enable RTCM sentences on Ublox NEO-M8P module
  By: Nathan Seidle
  SparkFun Electronics
  Date: September 7th, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example sends the command to enable the four RTCM messages needed for RTK. This 
  is the first part of a larger tutorial and example to setup an RTK base station.
  These commands are only accepted by the NEO-M8P module.
 
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

void setup()
{
  Serial.begin(115200);
  while(!Serial); //Wait for user to open terminal
  Serial.println("Ublox RTCM Enable Example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  while(Serial.available()) Serial.read(); //Clear any latent chars in serial buffer
  Serial.println("Press any key to send commands to enable RTCM 3.x");
  while(Serial.available() == 0) ; //Wait for user to press a key

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR

  boolean response = true;
  response &= myGPS.enableRTCMmessage(UBX_RTCM_1005, COM_PORT_I2C, 1); //Enable message 1005 to output through I2C port, message every second
  response &= myGPS.enableRTCMmessage(UBX_RTCM_1077, COM_PORT_I2C, 1);
  response &= myGPS.enableRTCMmessage(UBX_RTCM_1087, COM_PORT_I2C, 1);
  response &= myGPS.enableRTCMmessage(UBX_RTCM_1230, COM_PORT_I2C, 10); //Enable message every 10 seconds

  if (response == true)
  {
    Serial.println("RTCM messages enabled");
  }
  else
  {
    Serial.println("RTCM failed to enable. Are you sure you have an NEO-M8P?");
    while(1); //Freeze
  }

  //RTCM is now enabled but we haven't done a 'survey-in'
  //See example 4 for the full Base RTK setup
}

void loop()
{
  myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

  delay(250); //Don't pound too hard on the I2C bus
}
