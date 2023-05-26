// --------------------------------------
// i2c_scanner
// from: https://playground.arduino.cc/Main/I2cScanner/

// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.


#include <Wire.h>
uint16_t counter;

void setup()
{
  Wire1.begin();

  Serial.begin(9600);
  Serial.println(F("I2C Scanner starting"));
  Serial.println();
}


void loop()
{
  uint8_t error, address;
  int16_t nDevices;

  counter++;
  Serial.print(counter);
  Serial.println(F(" Scanning..."));

  nDevices = 0;
  //for (address = 1; address < 127; address++ ) //addresses 1 to 7 and 120 to 127 are not valid addresses
  for (address = 8; address < 120; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0)
    {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16)
        Serial.print(F("0"));
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16)
        Serial.print(F("0"));
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
  {
    Serial.println(F("No I2C devices found"));
  }
  else
  {
    Serial.println();
    Serial.println(F("Done"));
    Serial.println();
  }

  delay(5000);           // wait 5 seconds for next scan
}
