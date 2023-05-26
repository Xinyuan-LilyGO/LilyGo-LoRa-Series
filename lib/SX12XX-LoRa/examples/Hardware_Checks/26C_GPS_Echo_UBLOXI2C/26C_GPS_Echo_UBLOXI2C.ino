/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple program to test a UBLOX GPS. It reads characters from the GPS using
  the I2C interface and sends them (echoes) to the IDE serial monitor. If your ever having problems with a
  GPS (or just think you are) use this program first.

  Serial monitor baud rate is set at 115200.

*******************************************************************************************************/

#include <Wire.h>
const uint16_t GPSI2CAddress = 0x42;


void loop()
{
  uint8_t GPSByte;
  
  do
    {
      Wire.requestFrom(GPSI2CAddress, 1);
      GPSByte = Wire.read();

      if (GPSByte != 0xFF)
      {
        Serial.write(GPSByte);
      }
    }
    while (true);
}


void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("26C_GPS_Echo_UBLOXI2C Starting");
}
