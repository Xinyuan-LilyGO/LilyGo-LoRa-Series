/*
  Send UBX binary commands to enable RTCM sentences on Ublox NEO-M8P-2 module
  By: Nathan Seidle
  SparkFun Electronics
  Date: September 7th, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example does all steps to configure and enable a NEO-M8P-2 as a base station:
    Begin Survey-In
    Once we've achieved 2m accuracy and 300s have passed, survey is complete
    Enable four RTCM messages
    Begin outputting RTCM bytes

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
  while (!Serial); //Wait for user to open terminal
  Serial.println("Ublox NEO-M8P-2 base station example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR

  while (Serial.available()) Serial.read(); //Clear any latent chars in serial buffer
  Serial.println("Press any key to send commands to begin Survey-In");
  while (Serial.available() == 0) ; //Wait for user to press a key

  boolean response;

  //Check if Survey is in Progress before initiating one
  response = myGPS.getSurveyStatus(2000); //Query module for SVIN status with 2000ms timeout (request can take a long time)
  if (response == false)
  {
    Serial.println("Failed to get Survey In status");
    while (1); //Freeze
  }

  if (myGPS.svin.active == true)
  {
    Serial.print("Survey already in progress.");
  }
  else
  {
    //Start survey
    response = myGPS.enableSurveyMode(300, 2.000); //Enable Survey in, 300 seconds, 2.0m
    if (response == false)
    {
      Serial.println("Survey start failed");
      while (1);
    }
    Serial.println("Survey started. This will run until 300s has passed and less than 2m accuracy is achieved.");
  }

  while(Serial.available()) Serial.read(); //Clear buffer
  
  //Begin waiting for survey to complete
  while (myGPS.svin.valid == false)
  {
    if(Serial.available())
    {
      byte incoming = Serial.read();
      if(incoming == 'x')
      {
        //Stop survey mode
        response = myGPS.disableSurveyMode(); //Disable survey
        Serial.println("Survey stopped");
        break;
      }
    }
    
    response = myGPS.getSurveyStatus(2000); //Query module for SVIN status with 2000ms timeout (req can take a long time)
    if (response == true)
    {
      Serial.print("Press x to end survey - ");
      Serial.print("Time elapsed: ");
      Serial.print((String)myGPS.svin.observationTime);

      Serial.print(" Accuracy: ");
      Serial.print((String)myGPS.svin.meanAccuracy);
      Serial.println();
    }
    else
    {
      Serial.println("SVIN request failed");
    }

    delay(1000);
  }
  Serial.println("Survey valid!");

  response = true;
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
    while (1); //Freeze
  }

  Serial.println("Base survey complete! RTCM now broadcasting.");
}

void loop()
{
  myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

  delay(250); //Don't pound too hard on the I2C bus
}

//This function gets called from the SparkFun Ublox Arduino Library.
//As each RTCM byte comes in you can specify what to do with it
//Useful for passing the RTCM correction data to a radio, Ntrip broadcaster, etc.
void SFE_UBLOX_GPS::processRTCM(uint8_t incoming)
{
  //Let's just pretty-print the HEX values for now
  if (myGPS.rtcmFrameCounter % 16 == 0) Serial.println();
  Serial.print(" ");
  if (incoming < 0x10) Serial.print("0");
  Serial.print(incoming, HEX);
}
