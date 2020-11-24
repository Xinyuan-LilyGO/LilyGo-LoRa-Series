/*
  Send UBX binary commands to enable RTCM sentences on u-blox ZED-F9P module
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 9th, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  u-blox changed how to configure their modules in 2019. As of version 23 of the UBX protocol the
  UBX-CFG commands are deprecated; they still work, they just recommend using VALSET, VALGET, and VALDEL
  commands instead. This example shows how to use this new command structure.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a RedBoard Qwiic or BlackBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("u-blox getVal example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

  if (myGPS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  myGPS.enableDebugging(); //Enable debug messages over Serial (default)
  //myGPS.enableDebugging(SerialUSB); //Enable debug messages over Serial USB

  bool setValueSuccess;

  //These key values are hard coded and defined in u-blox_config_keys.h.
  //You can obtain them from the ZED-F9P interface description doc
  //or from u-center's Messages->CFG->VALSET window. Keys must be 32-bit.
  //setValueSuccess = myGPS.setVal(UBLOX_CFG_NMEA_HIGHPREC, 0); //Enable high precision NMEA
  setValueSuccess = myGPS.setVal(UBLOX_CFG_RATE_MEAS, 100); //Set measurement rate to 100ms (10Hz update rate)
  //setValueSuccess = myGPS.setVal(UBLOX_CFG_RATE_MEAS, 1000); //Set measurement rate to 1000ms (1Hz update rate)

  //Below is the original way we enabled the RTCM message on the I2C port. After that, we show how to do the same
  //but with setVal().
  //Original: myGPS.enableRTCMmessage(UBX_RTCM_1005, COM_PORT_I2C, 1); //Enable message 1005 to output through I2C port, message every second
  //setValueSuccess = myGPS.setVal(0x209102bd, 1); //Set output rate of msg 1005 over the I2C port to once per second

  if (setValueSuccess == true)
  {
    Serial.println("Value was successfully set");
  }
  else
    Serial.println("Value set failed");
}

void loop()
{
}
