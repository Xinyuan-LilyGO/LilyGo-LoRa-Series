/*
  Configuring the GPS to automatically send HPPOSLLH position reports over I2C
  By: Paul Clark
  Date: October 27th 2020

  Based on an earlier example:
  By: Nathan Seidle and Thorsten von Eicken
  SparkFun Electronics
  Date: January 3rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to configure the U-Blox GPS the send navigation reports automatically
  and retrieving the latest one via getHPPOSLLH. This eliminates the blocking in getHPPOSLLH while the GPS
  produces a fresh navigation solution at the expense of returning a slighly old solution.

  This can be used over serial or over I2C, this example shows the I2C use. With serial the GPS
  simply outputs the UBX_NAV_HPPOSLLH packet. With I2C it queues it into its internal I2C buffer (4KB in
  size?) where it can be retrieved in the next I2C poll.

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

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example");

  Wire.begin();

  //myGPS.enableDebugging(); // Uncomment this line to enable lots of helpful debug messages

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  // Uncomment the next line if you want to reset your module back to the default settings with 1Hz navigation rate
  //myGPS.factoryDefault(); delay(5000);

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save the communications port settings to flash and BBR
  
  myGPS.setNavigationFrequency(1); //Produce one solution per second
  

  // The acid test: all four of these combinations should work seamlessly :-)
  
  //myGPS.setAutoPVT(false); // Library will poll each reading
  //myGPS.setAutoHPPOSLLH(false); // Library will poll each reading
  
  //myGPS.setAutoPVT(true); // Tell the GPS to "send" each solution automatically
  //myGPS.setAutoHPPOSLLH(false); // Library will poll each reading

  //myGPS.setAutoPVT(false); // Library will poll each reading
  //myGPS.setAutoHPPOSLLH(true); // Tell the GPS to "send" each hi res solution automatically
  
  myGPS.setAutoPVT(true); // Tell the GPS to "send" each solution automatically
  myGPS.setAutoHPPOSLLH(true); // Tell the GPS to "send" each hi res solution automatically
}

void loop()
{
  // Calling getHPPOSLLH returns true if there actually is a fresh navigation solution available.
  // Calling getPVT returns true if there actually is a fresh navigation solution available.
  if ((myGPS.getHPPOSLLH()) || (myGPS.getPVT()))
  {
    Serial.println();
      
    long highResLatitude = myGPS.getHighResLatitude();
    Serial.print(F("Hi Res Lat: "));
    Serial.print(highResLatitude);
  
    int highResLatitudeHp = myGPS.getHighResLatitudeHp();
    Serial.print(F(" "));
    Serial.print(highResLatitudeHp);
  
    long highResLongitude = myGPS.getHighResLongitude();
    Serial.print(F(" Hi Res Long: "));
    Serial.print(highResLongitude);
  
    int highResLongitudeHp = myGPS.getHighResLongitudeHp();
    Serial.print(F(" "));
    Serial.print(highResLongitudeHp);
  
    unsigned long horizAccuracy = myGPS.getHorizontalAccuracy();
    Serial.print(F(" Horiz accuracy: "));
    Serial.print(horizAccuracy);
  
    long latitude = myGPS.getLatitude();
    Serial.print(F(" Lat: "));
    Serial.print(latitude);
  
    long longitude = myGPS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.println(longitude);
  }
  else
  {
    Serial.print(".");
    delay(50);
  }
}
