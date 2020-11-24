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

	After calibrating the module and securing it to your vehicle such that it's
  stable within 2 degrees, and the board is oriented correctly with regards to
  the vehicle's frame, you can now read the vehicle's "attitude". The attitude
  includes the vehicle's heading, pitch, and roll. You can also check the
  accuracy of those readings. 

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

  if (myGPS.getEsfInfo()){

    Serial.print(F("Fusion Mode: "));  
    Serial.println(myGPS.imuMeas.fusionMode);  

    if (myGPS.imuMeas.fusionMode == 1){
      Serial.println(F("Fusion Mode is Initialized!"));  
		}
		else {
      Serial.println(F("Fusion Mode is either disabled or not initialized - Freezing!"));  
			Serial.println(F("Please see Example 1 description at top for more information."));
		}
  }
}

void loop()
{
		myGPS.getVehAtt(); // Give the sensor you want to check on. 
		Serial.print(F("Roll: ")); 
		Serial.println(myGPS.vehAtt.roll);
		Serial.print(F("Pitch: ")); 
		Serial.println(myGPS.vehAtt.pitch);
		Serial.print(F("Heading: ")); 
		Serial.println(myGPS.vehAtt.heading);
		Serial.print(F("Roll Accuracy: ")); 
		Serial.println(myGPS.vehAtt.accRoll);
		Serial.print(F("Pitch Accuracy: ")); 
		Serial.println(myGPS.vehAtt.accPitch);
		Serial.print(F("Heading Accuracy: ")); 
		Serial.println(myGPS.vehAtt.accHeading);

    delay(250);
}


