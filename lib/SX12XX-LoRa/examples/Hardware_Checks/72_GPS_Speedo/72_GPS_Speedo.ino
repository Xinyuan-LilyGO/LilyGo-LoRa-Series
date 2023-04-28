/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/12/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation -  This program uses a GPS to indicate speed, should work with most GPSs. Speed update
  rate approx onece per second. GPS characters are output to serial monitor when checking for update from
  GPS. 
  
  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <TinyGPS++.h>                             //get library here > http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                   //create the TinyGPS++ object

#define RXpin A3                                   //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                   //pin number for GPS TX output from Arduino- RX into GPS

#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);

float GPSSpeed;                                    //Speed of GPS, mph

uint32_t startGetFixmS;
uint32_t endFixmS;


void loop()
{
  if (gpsWaitFix(5))
  {
    Serial.println();
    Serial.println();
    Serial.print(F("Fix time "));
    Serial.print(endFixmS - startGetFixmS);
    Serial.println(F("mS"));

    GPSSpeed = gps.speed.mph();

    Serial.print(F("Speed,"));
    Serial.print(GPSSpeed, 1);
    Serial.println();

    startGetFixmS = millis();    //have a fix, next thing that happens is checking for a fix, so restart timer
  }
  else
  {
    Serial.println();
    Serial.println();
    Serial.print(F("Timeout - No GPS Fix "));
    Serial.print( (millis() - startGetFixmS) / 1000 );
    Serial.println(F("s"));
  }
}


bool gpsWaitFix(uint16_t waitSecs)
{
  //waits a specified number of seconds for a fix, returns true for good fix

  uint32_t startmS, waitmS;
  uint8_t GPSchar;

  Serial.print(F("Wait GPS Fix "));
  Serial.print(waitSecs);
  Serial.println(F(" seconds"));

  waitmS = waitSecs * 1000;                              //convert seconds wait into mS
  startmS = millis();

  while ((uint32_t) (millis() - startmS) < waitmS)
  {
    if (GPSserial.available() > 0)
    {
      GPSchar = GPSserial.read();
      gps.encode(GPSchar);
      Serial.write(GPSchar);
    }

    if (gps.location.isUpdated() && gps.altitude.isUpdated() && gps.speed.isUpdated())
    {
      endFixmS = millis();                                //record the time when we got a new GPS update
      return true;
    }
  }

  return false;
}


void setup()
{
  delay(1000);
  Serial.begin(115200);
  GPSserial.begin(9600);

  Serial.println(F("72_GPS_Speedo Starting"));
  Serial.println();

  startGetFixmS = millis();
}
