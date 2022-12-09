/*
  By: Elias Santistevan
  SparkFun Electronics
  Date: May, 2020
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  NEO-M8U: https://www.sparkfun.com/products/16329
  ZED-F9R: https://www.sparkfun.com/products/16344  

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a Redboard Qwiic
  If you don't have a platform with a Qwiic connection use the 
  SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output

  To take advantage of the internal IMU of either the Dead Reckoning GPS
  boards (ZED-F9R, NEO-M8U), you must first calibrate it. This includes securing the GPS module
  to your vehicle so that it is stable within 2 degrees and that the frame of
  reference of the board is consistent with the picture outlined in the
  Receiver-Description-Prot-Spec Datasheet under Automotive/Untethered Dead
  Reckoning. You may also check either the ZED-F9R or NEO-M8U Hookup Guide for
  more information. After the board is secure, you'll need to put the module
  through certain conditions for proper calibration: acceleration, turning,
  stopping for a few minutes, getting to a speed over 30km/h all under a clear sky 
  with good GNSS signal. This example simply looks at the
  "fusionMode" status which indicates whether the SparkFun Dead Reckoning is
  not-calibrated - 0, or calibrated - 1.  
*/

#include <Wire.h> //Needed for I2C to GPS

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println(F("SparkFun Ublox Example"));

  Wire.begin();

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
}

void loop()
{

  if (myGPS.getEsfInfo()){
    Serial.print(F("Fusion Mode: "));  
    Serial.println(myGPS.imuMeas.fusionMode);  
    if (myGPS.imuMeas.fusionMode == 1)
      Serial.println(F("Sensor is calibrated!"));  
  }

  delay(250);
}
