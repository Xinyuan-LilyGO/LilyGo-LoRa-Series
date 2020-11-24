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

	After calibrating the module, also known as "Fusion Mode", you can get
	data directly from the IMU. This example code walks you through trouble
  shooting or identifying the different states of any individual  
  "external" (which include internal) sensors you've hooked up (vehicle speed
  sensor) or the internal IMU used by the modules. You can see if the sensor is
  being used, if it's calibrated, ready, what data type it returns, the state
  of the measurement etc.

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

	// GetEsfInfo also gets the number of sensors used by the ublox module, this
	// includes (in the case of the ZED-F9R) wheel tick input from the vehicle
	// speed sensor attached to the module. 
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

	for(int i=1; i<=myGPS.ubloxSen.numSens; i++){
		myGPS.getSensState(i); // Give the sensor you want to check on. 
		Serial.print(F("Sensor Data Type: ")); //See ublox receiver description
                                       //or our hookup guide for information on the
                                       //return value.
		Serial.println(myGPS.ubloxSen.senType);
		Serial.print(F("Being Used: ")); 
    Serial.println(myGPS.ubloxSen.isUsed);
		Serial.print(F("Is Ready: ")); 
    Serial.println(myGPS.ubloxSen.isReady);
		Serial.print(F("Calibration Status: ")); 
    Serial.println(myGPS.ubloxSen.calibStatus);
		Serial.print(F("Time Status: ")); 
    Serial.println(myGPS.ubloxSen.timeStatus);
		Serial.print(F("Bad Measure: ")); 
    Serial.println(myGPS.ubloxSen.timeStatus);
		Serial.print(F("Bad Time Tag: ")); 
    Serial.println(myGPS.ubloxSen.badTag);
		Serial.print(F("Missed Measure : ")); 
    Serial.println(myGPS.ubloxSen.missMeas);
		Serial.print(F("Noisy Measure: ")); 
    Serial.println(myGPS.ubloxSen.noisyMeas);
	}

}


