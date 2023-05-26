/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 12/10/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation -  This program uses a GPS to indicate speed, should work with most GPSs. Speed update
  rate approx once per second. GPS characters are output to serial monitor when checking for update from
  GPS. Prints speed in large text on an SSD1306 or SH1106 OLED.

  Display and serial monitor updatesto speed only occur when the a new update has been decoded from the
  GPS.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <TinyGPS++.h>                             //get library here > http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                   //create the TinyGPS++ object

#define RXpin A3                                   //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                   //pin number for GPS TX output from Arduino- RX into GPS

#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);

#include <U8g2lib.h>                               //get library here > https://github.com/olikraus/u8g2
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C disp(U8G2_R0, U8X8_PIN_NONE);   //use this line for 0.96" SSD1306 OLED
//U8G2_SH1106_128X64_NONAME_1_HW_I2C disp(U8G2_R0, U8X8_PIN_NONE);  //use this line for 1.3" SH1106 OLED

float GPSSpeed;                                    //Speed of GPS, mph

uint32_t startGetFixmS;
uint32_t endFixmS;


void loop()
{
  if (gpsWaitFix(3))
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

    displayscreen1();            //print GPS data on display
    startGetFixmS = millis();    //have a fix, next thing that happens is checking for a fix, so restart timer
  }
  else
  {
    displayscreen2();
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


void displayscreen1()
{
  disp.setFont(u8g2_font_logisoso62_tn);
  disp.firstPage();
  do {
    disp.setCursor(0, 63);
    disp.print(GPSSpeed, 0);
  } while ( disp.nextPage() );
}


void displayscreen2()
{
  disp.setFont(u8g2_font_lubB14_tr);
  disp.firstPage();
  do {
    disp.setCursor(35, 15);
    disp.print(F("Speedo"));
    disp.setCursor(15, 45);
    disp.print(F("No GPS fix"));
  } while ( disp.nextPage() );
}


void setup()
{
  delay(1000);
  Serial.begin(115200);
  GPSserial.begin(9600);

  disp.begin();
  disp.clear();
  displayscreen2();
  disp.print(F("Display Ready"));

  Serial.println(F("72A_GPS_Speedo_With_Display Starting"));
  Serial.println();

  startGetFixmS = millis();
}
