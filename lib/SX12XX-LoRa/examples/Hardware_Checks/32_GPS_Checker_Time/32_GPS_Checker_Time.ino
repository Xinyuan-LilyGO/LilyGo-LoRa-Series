/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 05/04/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -  This program is GPS Time checker. It reads the GPS NMEA output and displays the time
  on the Arduino IDE serial monitor. If you have the serial monitor output on screen as the same time as
  observing a web site displaying the current time you can check how close your GPS is reporting to real time. 

  At power up a GPS will not normally display the time accurate to the exact second until it receives the
  navigation message that is the updated value of current leaps seconds which is sent out every 12.5 minutes.  

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#define Program_Version "V1.0"
#define authorname "Stuart Robinson"

#include <TinyGPS++.h>                             //get library here > http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                   //create the TinyGPS++ object

#define RXpin A3                                   //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                   //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER -1                                //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE HIGH                            //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                            //logic level to turn GPS off via pin GPSPOWER 

#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);

uint8_t hours, mins, secs;


void loop()
{
  if (gpsWaitFix(5))
  {
    hours = gps.time.hour();
    mins = gps.time.minute();
    secs = gps.time.second();

    printGPSfix();

  }
  else
  {
    Serial.println(F("Timeout - No GPS Fix "));
  }
}


bool gpsWaitFix(uint16_t waitSecs)
{
  //waits a specified number of seconds for a fix, returns true for updated fix

  uint32_t endwaitmS;
  uint8_t GPSchar;

  endwaitmS = millis() + (waitSecs * 1000);

  while (millis() < endwaitmS)
  {
    if (GPSserial.available() > 0)
    {
      GPSchar = GPSserial.read();
      gps.encode(GPSchar);
      //Serial.write(GPSchar);
    }

    if (gps.location.isUpdated() && gps.time.isUpdated())
    {
      return true;
    }
  }

  return false;
}


void printGPSfix()
{
  Serial.print(F("Time,"));

  if (hours < 10)
  {
    Serial.print(F("0"));
  }

  Serial.print(hours);
  Serial.print(F(":"));

  if (mins < 10)
  {
    Serial.print(F("0"));
  }

  Serial.print(mins);
  Serial.print(F(":"));

  if (secs < 10)
  {
    Serial.print(F("0"));
  }

  Serial.print(secs);
  
  Serial.println();
}


void GPSON()
{
  if (GPSPOWER)
  {
    digitalWrite(GPSPOWER, GPSONSTATE);                         //power up GPS
  }
}


void GPSOFF()
{
  if (GPSPOWER)
  {
    digitalWrite(GPSPOWER, GPSOFFSTATE);                        //power off GPS
  }
}


void setup()
{
  if (GPSPOWER >= 0)
  {
    pinMode(GPSPOWER, OUTPUT);
    GPSON();
  }

  GPSserial.begin(9600);

  Serial.begin(115200);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("32_GPS_Checker_Time Starting"));
  Serial.println();
}
