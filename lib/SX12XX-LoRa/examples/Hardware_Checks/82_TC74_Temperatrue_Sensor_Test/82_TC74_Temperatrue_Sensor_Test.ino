/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The is a test program for the TC74 using an ATmega328P based Arduino.
  
  The TC74 is read for the temperature and the value printed to the serial monitor.
  
  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <Wire.h>

const uint8_t TC74_ADDRESS = 0x4C;           //I2C bus address of TC74

int8_t temperature;                          //the TC74 temperature value


void loop()
{
  temperature = TC74read(TC74_ADDRESS);      //read the TC74
  
  Serial.print(F("Temperature,"));
  Serial.print(temperature);
  Serial.println(F("c"));
  delay(2000);
}


int8_t TC74read(uint8_t addr)
{

  int8_t regdata = 128;                      //max temperature is 127 degrees, so 128 returned indicates a read error. 
  
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom((int8_t) addr, 1);

  if (Wire.available())
  {
    regdata = Wire.read();
    if (regdata & 0x80)
    {
      regdata = -1 * ((regdata ^ 0xFF ) + 1);
    }
    return regdata;
  }
  else
  {
    return 128;
  }
}


void setup()
{
  Serial.begin(9600);
  Wire.begin();
}
