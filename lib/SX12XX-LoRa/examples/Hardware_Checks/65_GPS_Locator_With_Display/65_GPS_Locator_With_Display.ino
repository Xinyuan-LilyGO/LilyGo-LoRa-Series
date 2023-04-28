/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 17/05/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation -  This program uses a GPS to indicate the distance and direction to a given location
  which is specified as the TestLat (Latitude) and TestLon (Longitude) at the top of the program. GPS 
  characters are output to the serial monitor until a fix is obtained. The GPS latitude and longitude is
  also displayed on the serial monitor. 
  
  Displays distance and direction on an SSD1306 or SH1106 OLED.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <TinyGPS++.h>                             //get library here > http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                   //create the TinyGPS++ object

#define RXpin 2                                    //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin 3                                    //pin number for GPS TX output from Arduino- RX into GPS

#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);

#include <U8g2lib.h>                               //get library here > https://github.com/olikraus/u8g2
#include <Wire.h>

//U8G2_SSD1306_128X64_NONAME_1_HW_I2C disp(U8G2_R0, U8X8_PIN_NONE);   //use this line for 0.96" SSD1306 OLED
U8G2_SH1106_128X64_NONAME_1_HW_I2C disp(U8G2_R0, U8X8_PIN_NONE);  //use this line for 1.3" SH1106 OLED

float GPSLatitude;
float GPSLongitude;
float distance;                                    //calculated distance to tracker
uint16_t direction;                                //calculated direction to location 
bool HaveFix = false;                              //set to true when there is a fix


const float TestLat = 51.50812;                    //Tower of London
const float TestLon = -0.07620;


void loop()
{
  GPSserial.begin(9600);
  
  if (gpsWaitFix(3))
  {
    GPSserial.end();
    GPSLatitude = gps.location.lat();
    GPSLongitude = gps.location.lng();
    direction = (int16_t) TinyGPSPlus::courseTo(TestLat, TestLon, GPSLatitude, GPSLongitude);
    distance = TinyGPSPlus::distanceBetween(TestLat, TestLon, GPSLatitude, GPSLongitude);
    
    Serial.println();
    Serial.print(GPSLatitude, 6);
    Serial.print(F(","));
    Serial.print(GPSLongitude, 6);
    Serial.print(F(","));
    Serial.print(distance, 2);
    Serial.print(F("m,"));
    Serial.print(direction, 1);
    Serial.print(F("d"));
    Serial.flush();

    displayscreen1();            //print GPS data on display
  }
  else
  {
    Serial.println();
    Serial.flush();
    displayscreen2();
  }
}


bool gpsWaitFix(uint16_t waitSecs)
{
  //waits a specified number of seconds for a fix, returns true for good fix

  uint32_t startmS, waitmS;
  uint8_t GPSchar;

  //Serial.print(F("Wait GPS Fix "));
  //Serial.print(waitSecs);
  //Serial.println(F(" seconds"));

  waitmS = waitSecs * 1000;                              //convert seconds wait into mS
  startmS = millis();

  while ((uint32_t) (millis() - startmS) < waitmS)
  {
    if (GPSserial.available() > 0)
    {
      GPSchar = GPSserial.read();
      gps.encode(GPSchar);
      if (!HaveFix)                                       //while waiting for fix, display characters 
      {
      Serial.write(GPSchar);
      }
    }

    if (gps.location.isUpdated())
    {
      HaveFix = true;
      return true;
    }
  }

  HaveFix = false;
  return false;
}


void displayscreen1()
{
  disp.setFont(u8g2_font_lubB14_tr);
  disp.firstPage();
  do {
    disp.setCursor(0, 15);
    disp.print(distance,2);
    disp.print(F("m"));
    disp.setCursor(0, 45);
    disp.print(direction);
    disp.print(F("d"));
  } while ( disp.nextPage() );
}


void displayscreen2()
{
  disp.setFont(u8g2_font_lubB14_tr);
  disp.firstPage();
  do {
    disp.setCursor(25, 15);
    disp.print(F("Locator"));
    disp.setCursor(10, 45);
    disp.print(F("No GPS fix"));
  } while ( disp.nextPage() );
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  
  disp.begin();
  disp.clear();
  displayscreen2();

  Serial.println(F("65_GPS_Locator_With_Display Starting"));
  Serial.println();
  }
