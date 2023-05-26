/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 24/10/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -  This program is a portable GPS checker. It reads the GPS for 5 seconds and copies
  the characters from the GPS to the serial monitor, this is an example printout from a working GPS that
  has just been powered on;

  28_GPS_Checker Starting
  Wait GPS Fix 5 seconds
  Timeout - No GPS Fix 5s
  Wait GPS Fix 5 seconds
  $PGACK,103*40
  $PGACK,105*46
  $PMTK011,MTKGPS*08
  $PMTK010,001*2E
  $PMTK010,00æ*2D
  $GPGGA,235942.800,,,,,0,0,,,M,,M,,*4B
  $GPGSA,A,1,,,,,,,,,,,,,,,*1E
  $GPRMC,235942.800,V,,,,,0.00,0.00,050180,,,N*42
  $GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32
  $GPGSV,1,1,03,30,,,43,07,,,43,05,,,38*70

  Timeout - No GPS Fix 5s
  Wait GPS Fix 5 seconds

  That printout is from a Mediatek GPS, the Ublox ones are similar. The data from the GPS is also fed into
  the TinyGPS++ library and if there is no fix a message is printed on the serial monitor.

  When the program detects that the GPS has a fix, it prints the Latitude, Longitude, Altitude, Number
  of satellites in use, the HDOP value, time and date to the serial monitor. The GPS data is logged to a
  file on the attached SD card also.

  The program has the option of using a pin to control the power to the GPS, if the GPS module being used
  has this feature. To use the option change the define; '#define GPSPOWER -1' from -1 to the pin number
  being used. Also set the GPSONSTATE and GPSOFFSTATE to the appropriate logic levels.

  There is a set of GPS co-ordinates defined, TestLatitude and TestLongitude, when the GPS has its location
  fix the distance and direction to the Test location is calculated and shown on the serial monitor.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>
#include <SD.h>

#include <TinyGPS++.h>                             //get library here > http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                   //create the TinyGPS++ object

#define RXpin A3                                   //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                   //pin number for GPS TX output from Arduino- RX into GPS
#define LED1 8                                     //pin number for LED, turns on when printing Fix data 
#define SDCS 4

#define GPSPOWER -1                                //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE HIGH                            //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                            //logic level to turn GPS off via pin GPSPOWER 

#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);

char filename[] = "/LOG0000.TXT";                  //filename used as base for creating root, 0000 replaced with numbers
File logFile;

float GPSLat;                                      //Latitude from GPS
float GPSLon;                                      //Longitude from GPS
float GPSAlt;                                      //Altitude from GPS
uint8_t GPSSats;                                   //number of GPS satellites in use
uint32_t GPSHdop;                                  //HDOP from GPS
uint8_t hours, mins, secs, day, month;
uint16_t year;
uint32_t startGetFixmS;
uint32_t endFixmS;

//GPS co-ordinates to use for the test location transmission
const float TestLatitude  = 51.48230;              //Cardiff castle keep, used for location testing purposes
const float TestLongitude  = -3.18136;


void loop()
{
  if (gpsWaitFix(5))
  {
    digitalWrite(LED1, HIGH);                      //LED on to indicate fix
    Serial.println();
    Serial.println();
    Serial.print(F("Fix time "));
    Serial.print(endFixmS - startGetFixmS);
    Serial.println(F("mS"));

    GPSLat = gps.location.lat();
    GPSLon = gps.location.lng();
    GPSAlt = gps.altitude.meters();
    GPSSats = gps.satellites.value();
    GPSHdop = gps.hdop.value();

    hours = gps.time.hour();
    mins = gps.time.minute();
    secs = gps.time.second();
    day = gps.date.day();
    month = gps.date.month();
    year = gps.date.year();

    printGPSfix();
    logGPSfix();
    startGetFixmS = millis();    //have a fix, next thing that happens is checking for a fix, so restart timer
    digitalWrite(LED1, LOW);
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
  //waits a specified number of seconds for a fix, returns true for updated fix

  uint32_t startmS, waitmS;
  uint8_t GPSchar;

  Serial.print(F("Wait GPS Fix "));
  Serial.print(waitSecs);
  Serial.println(F(" seconds"));

  waitmS = waitSecs * 1000;                               //convert seconds wait into mS

  startmS = millis();

  while ( (uint32_t) (millis() - startmS) < waitmS)       //allows for millis() overflow
  {
    if (GPSserial.available() > 0)
    {
      GPSchar = GPSserial.read();
      gps.encode(GPSchar);
      Serial.write(GPSchar);
    }

    if (gps.location.isUpdated() && gps.altitude.isUpdated() && gps.date.isUpdated())
    {
      endFixmS = millis();                                //record the time when we got a GPS fix
      return true;
    }
  }
  return false;
}


void printGPSfix()
{
  float tempfloat;
  uint32_t distance;
  uint16_t direction;

  Serial.print(F("New GPS Fix "));

  tempfloat = ( (float) GPSHdop / 100);

  Serial.print(F("Lat,"));
  Serial.print(GPSLat, 6);
  Serial.print(F(",Lon,"));
  Serial.print(GPSLon, 6);
  Serial.print(F(",Alt,"));
  Serial.print(GPSAlt, 1);
  Serial.print(F("m,Sats,"));
  Serial.print(GPSSats);
  Serial.print(F(",HDOP,"));
  Serial.print(tempfloat, 2);
  Serial.print(F(",Time,"));

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
  Serial.print(F(",Date,"));

  Serial.print(day);
  Serial.print(F("/"));
  Serial.print(month);
  Serial.print(F("/"));
  Serial.print(year);

  distance = gps.distanceBetween(GPSLat, GPSLon, TestLatitude, TestLongitude);
  direction = gps.courseTo(GPSLat, GPSLon, TestLatitude, TestLongitude);

  Serial.println();
  Serial.print(F("Distance to Test Location ("));
  Serial.print(TestLatitude, 6);
  Serial.print((","));
  Serial.print(TestLongitude, 6);
  Serial.print((") "));
  Serial.print(distance);
  Serial.print(("m"));
  Serial.println();
  Serial.print(F("Direction to Test Location ("));
  Serial.print(TestLatitude, 6);
  Serial.print((","));
  Serial.print(TestLongitude, 6);
  Serial.print((") "));
  Serial.print(direction);
  Serial.print(("d"));
  Serial.println();
  Serial.println();
}


void logGPSfix()
{
  float tempfloat;
  uint32_t distance;
  uint16_t direction;

  Serial.print(F("Log GPS Fix "));
  Serial.println(filename);

  tempfloat = ( (float) GPSHdop / 100);

  logFile.print(F("Lat,"));
  logFile.print(GPSLat, 6);
  logFile.print(F(",Lon,"));
  logFile.print(GPSLon, 6);
  logFile.print(F(",Alt,"));
  logFile.print(GPSAlt, 1);
  logFile.print(F("m,Sats,"));
  logFile.print(GPSSats);
  logFile.print(F(",HDOP,"));
  logFile.print(tempfloat, 2);
  logFile.print(F(",Time,"));

  if (hours < 10)
  {
    logFile.print(F("0"));
  }

  logFile.print(hours);
  logFile.print(F(":"));

  if (mins < 10)
  {
    logFile.print(F("0"));
  }

  logFile.print(mins);
  logFile.print(F(":"));

  if (secs < 10)
  {
    logFile.print(F("0"));
  }

  logFile.print(secs);
  logFile.print(F(",Date,"));

  logFile.print(day);
  logFile.print(F("/"));
  logFile.print(month);
  logFile.print(F("/"));
  logFile.print(year);

  distance = gps.distanceBetween(GPSLat, GPSLon, TestLatitude, TestLongitude);
  direction = gps.courseTo(GPSLat, GPSLon, TestLatitude, TestLongitude);

  logFile.println();
  logFile.print(F("Distance to Test Location ("));
  logFile.print(TestLatitude, 6);
  logFile.print((","));
  logFile.print(TestLongitude, 6);
  logFile.print((") "));
  logFile.print(distance);
  logFile.print(("m"));
  logFile.println();
  logFile.print(F("Direction to Test Location ("));
  logFile.print(TestLatitude, 6);
  logFile.print((","));
  logFile.print(TestLongitude, 6);
  logFile.print((") "));
  logFile.print(direction);
  logFile.print(("d"));
  logFile.println();
  logFile.flush();
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


uint8_t setupSDLOG(char *buff)
{
  //creats a new filename

  uint16_t index;

  File dir;

  for (index = 1; index <= 9999; index++)
  {
    buff[4] = index / 1000 + '0';
    buff[5] = ((index % 1000) / 100) + '0';
    buff[6] = ((index % 100) / 10) + '0';
    buff[7] = index % 10 + '0' ;

    if (! SD.exists(filename))
    {
      // only open a new file if it doesn't exist
      dir = SD.open(buff, FILE_WRITE);
      break;
    }
  }

  logFile = SD.open(filename, FILE_WRITE);

}


void setup()
{
  pinMode(LED1, OUTPUT);

  if (GPSPOWER >= 0)
  {
    pinMode(GPSPOWER, OUTPUT);
    GPSON();
  }

  GPSserial.begin(9600);

  Serial.begin(115200);
  Serial.println();
  Serial.println(F(__FILE__));
  Serial.println();

  SPI.begin();

  if (!SD.begin(SDCS))
  {
    Serial.println();
    Serial.println("ERROR Card Mount Failed");
    Serial.println();
    delay(2000);
  }

  setupSDLOG(filename);

  Serial.print(F("Card Mount OK > "));
  Serial.println(filename);

  Serial.println(F("28A_GPS_SD_Logger Starting"));
  Serial.println();

  startGetFixmS = millis();
}
