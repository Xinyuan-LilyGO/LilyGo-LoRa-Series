/*
  Configuring the GPS to automatically send position reports over I2C, with explicit data parsing calls
  By: Nathan Seidle Thorsten von Eicken and Felix Jirka
  SparkFun Electronics
  Date: July 1st, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to configure the U-Blox GPS the send navigation reports automatically
  and retrieving the latest one via checkUblox when available.
  This eliminates the implicit update in getPVT when accessing data fields twice.
  Also this reduces the memory overhead of a separate buffer while introducing a slight error by inconsistencies because of the unsynchronized updates (on a multi core system).

  This can be used over serial or over I2C, this example shows the I2C use. With serial the GPS
  simply outputs the UBX_NAV_PVT packet. With I2C it queues it into its internal I2C buffer (4KB in
  size?) where it can be retrieved in the next I2C poll.

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

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example");

  Wire.begin();

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.setNavigationFrequency(2);  //Produce two solutions per second
  myGPS.setAutoPVT(true, false);    //Tell the GPS to "send" each solution and the lib not to update stale data implicitly
  myGPS.saveConfiguration();        //Save the current settings to flash and BBR
}

/*
     Calling getPVT would return false now (compare to previous example where it would return true), so we just use the data provided
     If you are using a threaded OS eg. FreeRTOS on an ESP32, the explicit mode of autoPVT allows you to use the data provided on both cores and inside multiple threads
     The data update in background creates an inconsistent state, but that should not cause issues for most applications as they usually won't change the GPS location significantly within a 2Hz - 5Hz update rate.
     Also you could oversample (10Hz - 20Hz) the data to smooth out such issues...
*/
void loop()
{
  static uint16_t counter = 0;

  if (counter % 10 == 0)
  {
    // update your AHRS filter here for a ~100Hz update rate
    // GPS data will be quasi static but data from your IMU will be changing
  }
  // debug output each half second
  if (counter % 500 == 0)
  {
    Serial.println();
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
  // call checkUblox all 50ms to capture the gps data
  if (counter % 50 == 0)
  {
    myGPS.checkUblox();
  }
  delay(1);
  counter++;
}
