/*
  u-blox M8 geofence example

  Written by Paul Clark (PaulZC)
  10th December 2019

  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example demonstrates how to use the addGeofence and getGeofenceState functions

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15210
  ZOE-M8Q: https://www.sparkfun.com/products/15193

  This example powers up the GPS and reads the fix.
  Once a valid 3D fix has been found, the code reads the latitude and longitude.
  The code then sets four geofences around that position with a radii of 5m, 10m, 15m and 20m with 95% confidence.
  The code then monitors the geofence status.
  The LED will be illuminated if you are inside the _combined_ geofence (i.e. within the 20m radius).

  This code has been tested on the ZOE-M8Q.
*/

#define LED LED_BUILTIN // Change this if your LED is on a different pin

#include <Wire.h> // Needed for I2C

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

void setup()
{
  pinMode(LED, OUTPUT);

  // Set up the I2C pins
  Wire.begin();

  // Start the console serial port
  Serial.begin(115200);
  while (!Serial); // Wait for the user to open the serial monitor
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println(F("u-blox M8 geofence example"));
  Serial.println();
  Serial.println();

  delay(1000); // Let the GPS power up

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  //myGPS.enableDebugging(); // Enable debug messages
  myGPS.setI2COutput(COM_TYPE_UBX); // Limit I2C output to UBX (disable the NMEA noise)

  Serial.println(F("Waiting for a 3D fix..."));

  byte fixType = 0;

  while (fixType < 3)
  {
    fixType = myGPS.getFixType(); // Get the fix type
    Serial.print(F("Fix: ")); // Print it
    Serial.print(fixType);
    if(fixType == 0) Serial.print(F(" = No fix"));
    else if(fixType == 1) Serial.print(F(" = Dead reckoning"));
    else if(fixType == 2) Serial.print(F(" = 2D"));
    else if(fixType == 3) Serial.print(F(" = 3D"));
    else if(fixType == 4) Serial.print(F(" = GNSS + Dead reckoning"));
    else if(fixType == 5) Serial.print(F(" = Time only"));
    Serial.println();
    delay(1000);
  }

  Serial.println(F("3D fix found!"));

  long latitude = myGPS.getLatitude(); // Get the latitude in degrees * 10^-7
  Serial.print(F("Lat: "));
  Serial.print(latitude);

  long longitude = myGPS.getLongitude(); // Get the longitude in degrees * 10^-7
  Serial.print(F("   Long: "));
  Serial.println(longitude);

  uint32_t radius = 500; // Set the radius to 5m (radius is in m * 10^-2 i.e. cm)

  byte confidence = 2; // Set the confidence level: 0=none, 1=68%, 2=95%, 3=99.7%, 4=99.99%

  // Call clearGeofences() to clear all existing geofences.
  Serial.print(F("Clearing any existing geofences. clearGeofences returned: "));
  Serial.println(myGPS.clearGeofences());

  // It is possible to define up to four geofences.
  // Call addGeofence up to four times to define them.
  Serial.println(F("Setting the geofences:"));

  Serial.print(F("addGeofence for geofence 1 returned: "));
  Serial.println(myGPS.addGeofence(latitude, longitude, radius, confidence));

  radius = 1000; // 10m
  Serial.print(F("addGeofence for geofence 2 returned: "));
  Serial.println(myGPS.addGeofence(latitude, longitude, radius, confidence));

  radius = 1500; // 15m
  Serial.print(F("addGeofence for geofence 3 returned: "));
  Serial.println(myGPS.addGeofence(latitude, longitude, radius, confidence));

  radius = 2000; // 20m
  Serial.print(F("addGeofence for geofence 4 returned: "));
  Serial.println(myGPS.addGeofence(latitude, longitude, radius, confidence));
}

void loop()
{
  geofenceState currentGeofenceState; // Create storage for the geofence state

  boolean result = myGPS.getGeofenceState(currentGeofenceState);

  Serial.print(F("getGeofenceState returned: ")); // Print the combined state
  Serial.print(result); // Get the geofence state

  if (!result) // If getGeofenceState did not return true
  {
    Serial.println(F(".")); // Tidy up
    return; // and go round the loop again
  }

  Serial.print(F(". status is: ")); // Print the status
  Serial.print(currentGeofenceState.status);

  Serial.print(F(". numFences is: ")); // Print the numFences
  Serial.print(currentGeofenceState.numFences);

  Serial.print(F(". combState is: ")); // Print the combined state
  Serial.print(currentGeofenceState.combState);

  if (currentGeofenceState.combState == 0)
  {
    Serial.print(F(" = Unknown"));
    digitalWrite(LED, LOW);
  }
  if (currentGeofenceState.combState == 1)
  {
    Serial.print(F(" = Inside"));
    digitalWrite(LED, HIGH);
  }
  else if (currentGeofenceState.combState == 2)
  {
    Serial.print(F(" = Outside"));
    digitalWrite(LED, LOW);
  }

  Serial.print(F(". The individual states are: ")); // Print the state of each geofence
  for(int i = 0; i < currentGeofenceState.numFences; i++)
  {
    if (i > 0) Serial.print(F(","));
    Serial.print(currentGeofenceState.states[i]);
  }
  Serial.println();

  delay(1000);
}
