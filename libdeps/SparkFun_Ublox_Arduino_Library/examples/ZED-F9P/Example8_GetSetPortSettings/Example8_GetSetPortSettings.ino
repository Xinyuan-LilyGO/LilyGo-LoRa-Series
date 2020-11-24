/*
  Configuring port settings using the newer getVal/setVal methods
  By: Nathan Seidle
  SparkFun Electronics
  Date: October 23rd, 2020
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to query a u-blox module for its UART1 settings and
  then change them if the settings aren't what we want.

  Note: getVal/setVal/delVal are only support in u-blox protocol versions 27 and higher.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a RedBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  if (myGPS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  bool response = true;

  //Read the settings from RAM (what the module is running right now, not BBR, Flash, or default)
  uint8_t currentUART1Setting_ubx = myGPS.getVal8(UBLOX_CFG_UART1INPROT_UBX);
  uint8_t currentUART1Setting_nmea = myGPS.getVal8(UBLOX_CFG_UART1INPROT_NMEA);
  uint8_t currentUART1Setting_rtcm3 = myGPS.getVal8(UBLOX_CFG_UART1INPROT_RTCM3X);

  Serial.print("currentUART1Setting_ubx: ");
  Serial.println(currentUART1Setting_ubx);
  Serial.print("currentUART1Setting_nmea: ");
  Serial.println(currentUART1Setting_nmea);
  Serial.print("currentUART1Setting_rtcm3: ");
  Serial.println(currentUART1Setting_rtcm3);

  //Check if NMEA and RTCM are enabled for UART1
  if (currentUART1Setting_ubx == 0 || currentUART1Setting_nmea == 0)
  {
    Serial.println("Updating UART1 configuration");

    //setVal sets the values for RAM, BBR, and Flash automatically so no .saveConfiguration() is needed
    response &= myGPS.setVal8(UBLOX_CFG_UART1INPROT_UBX, 1);    //Enable UBX on UART1 Input
    response &= myGPS.setVal8(UBLOX_CFG_UART1INPROT_NMEA, 1);   //Enable NMEA on UART1 Input
    response &= myGPS.setVal8(UBLOX_CFG_UART1INPROT_RTCM3X, 0); //Disable RTCM on UART1 Input

    if (response == false)
      Serial.println("SetVal failed");
    else
      Serial.println("SetVal succeeded");
  }
  else
    Serial.println("No port change needed");

  //Change speed of UART2
  uint32_t currentUART2Baud = myGPS.getVal32(UBLOX_CFG_UART2_BAUDRATE);
  Serial.print("currentUART2Baud: ");
  Serial.println(currentUART2Baud);

  if (currentUART2Baud != 57600)
  {
    response &= myGPS.setVal32(UBLOX_CFG_UART2_BAUDRATE, 57600);
    if (response == false)
      Serial.println("SetVal failed");
    else
      Serial.println("SetVal succeeded");
  }
  else
    Serial.println("No baud change needed");

  Serial.println("Done");
}

void loop()
{
}