/*
  Set the static position of the receiver.
  By: SparkFun Electronics / Nathan Seidle
  Date: September 26th, 2020
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to set the static position of a receiver
  using an Earth-Centered, Earth-Fixed (ECEF) location. This is the
  output from a long (24 hour+) survey-in. Setting the static position
  immediately causes the receiver to begin outputting RTCM data (if
  enabled), perfect for setting up your own RTCM NTRIP caster or CORS.

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

void setup()
{
  Serial.begin(115200); // You may need to increase this for high navigation rates!
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  Wire.begin();

  //myGPS.enableDebugging(); // Uncomment this line to enable debug messages

  if (myGPS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)

  //-1280208.308,-4716803.847,4086665.811 is SparkFun HQ so...

  //Units are cm so 1234 = 12.34m
  //myGPS.setStaticPosition(-128020831, -471680385, 408666581);

  //Units are cm with a high precision extension so -1234.5678 should be called: (-123456, -78)
  myGPS.setStaticPosition(-128020830, -80, -471680384, -70, 408666581, 10); //With high precision 0.1mm parts

  //We can also set via lat/long
  //40.09029751,-105.18507900,1560.238
  //myGPS.setStaticPosition(400902975, -1051850790, 156024, true); //True at end enables lat/long input
  //myGPS.setStaticPosition(400902975, 10, -1051850790, 0, 156023, 80, true);

  //Now let's use getVals to read back the data
  //long ecefX = myGPS.getVal32(0x40030003);
  //Serial.print("ecefX: ");
  //Serial.println(ecefX);

  Serial.println(F("Done!"));
}

void loop()
{
}
