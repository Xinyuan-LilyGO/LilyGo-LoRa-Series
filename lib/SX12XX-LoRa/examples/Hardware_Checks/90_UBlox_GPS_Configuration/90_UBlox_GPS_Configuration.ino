/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 28/08/18

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple program to configure a Ublox GPS. 

  At startup GPS characters are read and sent to the IDE serial monitor for 2 seconds. 

  The GPS configuration is then cleared and a further 2 seconds of characters are sent. 

  Then most GPS sentences are turned off, leaving only GPRMC and GPGGA that are those needed for location
  and speed data etc. The refresh rate is then changed to 10hz.

  The GPS characters are again copied to serial monitor using the new configuration.
  
  GPS baud rate set at 9600 baud, Serial monitor set at 115200 baud. If the data displayed on the serial
  terminal appears to be random text with odd symbols its very likely you have the GPS serial baud rate
  set incorrectly for the GPS.

  Note that not all pins on all Arduinos will work with software serial, see here;

  https://www.arduino.cc/en/Reference/softwareSerial

  Serial monitor baud rate is set at 115200.

*******************************************************************************************************/

#define RXpin A3              //this is the pin that the Arduino will use to receive data from the GPS
#define TXpin A2              //this is the pin that the Arduino can use to send data (commands) to the GPS

#include <SoftwareSerial.h>
SoftwareSerial GPS(RXpin, TXpin);

const PROGMEM  uint8_t ClearConfig[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x19, 0x98};
const PROGMEM  uint8_t GPGLLOff[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B};
const PROGMEM  uint8_t GPGSVOff[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39};
const PROGMEM  uint8_t GPVTGOff[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47};
const PROGMEM  uint8_t GPGSAOff[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32};
const PROGMEM  uint8_t GPGGAOff[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24};
const PROGMEM  uint8_t GPRMCOff[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40};
const PROGMEM  uint8_t Navrate10hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};

uint32_t startmS;


void loop()
{
  startmS = millis();

  //lets see 2 seconds of GPS characters at normal power up GPS configuration
  
  do
  {
    while (GPS.available())
    {
      Serial.write(GPS.read());
    }
  }
  while (millis() < (startmS + 2000));

  Serial.println();
  Serial.println();
  Serial.flush();

  Serial.print("ClearConfig ");
  GPS_SendConfig(ClearConfig, 21);

  Serial.println();
  Serial.println();
  Serial.flush();

  delay(2000);

  startmS = millis();

  //now lets see 2 seconds of GPS characters at cleared GPS configuration
  
  do
  {
    while (GPS.available())
    {
      Serial.write(GPS.read());
    }
  }
  while (millis() < (startmS + 2000));

  Serial.println();
  Serial.println();
  Serial.flush();

  //now turn off most of the GPS sentences and set the refresh rate to 10hz

  Serial.print("GPGLLOff ");
  GPS_SendConfig(GPGLLOff, 16);

  Serial.print("GPGSVOff ");
  GPS_SendConfig(GPGSVOff, 16);

  Serial.print("GPVTGOff ");
  GPS_SendConfig(GPVTGOff, 16);

  Serial.print("GPGSAOff ");
  GPS_SendConfig(GPGSAOff, 16);

  Serial.print("Navrate10hz ");
  GPS_SendConfig(Navrate10hz, 14);

  Serial.println();
  Serial.println();
  Serial.flush();

  delay(2000);

  do
  {
    if (GPS.available())
    {
      Serial.write(GPS.read());
    }
  } while (1);
}


void GPS_SendConfig(const uint8_t *Progmem_ptr, uint8_t arraysize)
{
  uint8_t byteread, index;

  Serial.print(F("GPSSend  "));

  for (index = 0; index < arraysize; index++)
  {
    byteread = pgm_read_byte_near(Progmem_ptr++);
    if (byteread < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print(byteread, HEX);
    Serial.print(F(" "));
  }

  Serial.println();
  Progmem_ptr = Progmem_ptr - arraysize;                  //set Progmem_ptr back to start

  for (index = 0; index < arraysize; index++)
  {
    byteread = pgm_read_byte_near(Progmem_ptr++);
    GPS.write(byteread);
  }
  delay(100);

}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("90_UBlox_GPS_Configuration Starting");
  Serial.println();
  Serial.println();
  GPS.begin(9600);
}
