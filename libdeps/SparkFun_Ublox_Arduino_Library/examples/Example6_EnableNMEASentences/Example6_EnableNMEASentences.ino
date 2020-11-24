/*
  Turn on/off various NMEA sentences.
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 3rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to turn on/off the NMEA sentences being output
  over UART1. We use the I2C interface on the Ublox module for configuration
  but you won't see any output from this sketch. You'll need to hook up
  a Serial Basic or other USB to Serial device to UART1 on your Ublox module
  to see the output.

  This example turns off all sentences except for the GPGGA and GPVTG sentences.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a RedBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
  Hookup a Serial Basic (https://www.sparkfun.com/products/15096) to UART1 on the Ublox module. Open a terminal at 57600bps
  and see GPGGA and GPVTG sentences.
*/
#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

unsigned long lastGPSSend = 0;

void setup()
{
  Serial.begin(115200); // Serial debug output over USB visible from Arduino IDE
  Serial.println("Example showing how to enable/disable certain NMEA sentences");

  Wire.begin();

  if (myGPS.begin() == false)
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //Disable or enable various NMEA sentences over the UART1 interface
  myGPS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1); //Several of these are on by default on virgin ublox board so let's disable them
  myGPS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
  myGPS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
  myGPS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
  myGPS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1); //Only leaving GGA/VTG enabled at current navigation rate
  myGPS.enableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);

  //Here's the advanced configure method
  //Some of the other examples in this library enable the PVT message so let's disable it
  myGPS.configureMessage(UBX_CLASS_NAV, UBX_NAV_PVT, COM_PORT_UART1, 0); //Message Class, ID, and port we want to configure, sendRate of 0 (disable).

  myGPS.setUART1Output(COM_TYPE_NMEA); //Turn off UBX and RTCM sentences on the UART1 interface

  myGPS.setSerialRate(57600); //Set UART1 to 57600bps.

  myGPS.saveConfiguration(); //Save these settings to NVM

  Serial.println(F("Messages configured. NMEA now being output over the UART1 port on the Ublox module at 57600bps."));
}

void loop()
{
  if (millis() - lastGPSSend > 200)
  {
    myGPS.checkUblox(); //See if new data is available, but we don't want to get NMEA here. Go check UART1.
    lastGPSSend = millis();
  }
}