/*
  Reading lat and long via UBX binary commands using an RX-only UART
  By: Nathan Seidle, Adapted from Example11 by Felix Jirka
  SparkFun Electronics
  Date: July 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to configure the library for serial port use with a single wire connection using the assumeAutoPVT method.
  Saving your pins for other stuff :-)

  Leave NMEA parsing behind. Now you can simply ask the module for the datums you want!

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Preconditions:
  U-Blox module is configured to send cyclical PVT message
  Hardware Connections:
  Connect the U-Blox serial TX pin to Rx of Serial2 (default: GPIO16) on your ESP32
  Open the serial monitor at 115200 baud to see the output
*/

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example 17");

  //Use any Serial port with at least a Rx Pin connected or a receive only version of SoftwareSerial here
  //Assume that the U-Blox GPS is running at 9600 baud (the default)
  Serial2.begin(9600);
  // no need to check return value as internal call to isConnected() will not succeed
  myGPS.begin(Serial2);

  // tell lib, we are expecting the module to send PVT messages by itself to our Rx pin
  // you can set second parameter to "false" if you want to control the parsing and eviction of the data (need to call checkUblox cyclically)
  myGPS.assumeAutoPVT(true, true);

}

void loop()
{
  // if implicit updates are allowed, this will trigger parsing the incoming messages
  // and be true once a PVT message has been parsed
  // In case you want to use explicit updates, wrap this in a timer and call checkUblox as often as needed, not to overflow your UART buffers
  if (myGPS.getPVT())
  {
    long latitude = myGPS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGPS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGPS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    byte SIV = myGPS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();
  }
  else {
    Serial.println(F("Wait for GPS data"));
    delay(500);
  }
}
