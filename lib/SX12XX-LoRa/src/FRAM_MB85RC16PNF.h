/*
  Copyright 2020 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  14/04/20
*/

/*******************************************************************************************************
  Library Operation - This library is for reading from and writing to the 16kbit (2Kbyte) I2C FRAMS.
  These FRAMS present as a series of eight 256 byte pages at I2C addresses 0X50 to 0x57. The library
  calculates from the passed address which page to write to. For variables that are larger than one
  uint8_t take care not to set the address so that a write would cross a page boundary. The library
  does not deal with writes or reads that cross the 256 byte pages of the device.
*******************************************************************************************************/

#include <Wire.h>

int16_t Memory_I2C_Addr = 0x50;              //default I2C address of MB85RC16PNF FRAM


void memoryStart(int16_t addr)
{
  Memory_I2C_Addr = addr;
  Wire.begin();
}


void memorySetAddress(int16_t addr)
{
  Memory_I2C_Addr = addr;
  
}


void memoryEnd()
{
  //left empty for future use
}


/***************************************************************************
  Write Routines
 **************************************************************************/

void writeMemoryChar(uint16_t addr, char x)
{
  uint8_t msb_addr = (highByte(addr) & 0x07);             //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.write(x);
  Wire.endTransmission();
}


void writeMemoryInt8(uint16_t addr, int8_t x)
{
  uint8_t msb_addr = (highByte(addr) & 0x07);             //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.write(x);
  Wire.endTransmission();
}


void writeMemoryUint8(uint16_t addr, uint8_t x)
{
  uint8_t msb_addr = (highByte(addr) & 0x07);             //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.write(x);
  Wire.endTransmission();
}


void writeMemoryInt16(uint16_t addr, int16_t x)
{

  uint8_t msb_addr = (highByte(addr) & 0x07);              //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);
  uint8_t msb_data = highByte(x);
  uint8_t lsb_data = lowByte(x);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.write(lsb_data);
  Wire.write(msb_data);
  Wire.endTransmission();
}


void writeMemoryUint16(uint16_t addr, uint16_t x)
{
  uint8_t msb_addr = (highByte(addr) & 0x07);              //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);
  uint8_t msb_data = highByte(x);
  uint8_t lsb_data = lowByte(x);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.write(lsb_data);
  Wire.write(msb_data);
  Wire.endTransmission();
}


void writeMemoryInt32(uint16_t addr, int32_t x)
{
  uint8_t index, val;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  union
  {
    uint8_t b[4];
    uint32_t f;
  } data;

  data.f = x;

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);

  for (index = 0; index < 4; index++)
  {
    val = data.b[index];
    Wire.write(val);                                   //write the data
  }

  Wire.endTransmission();
}



void writeMemoryUint32(uint16_t addr, uint32_t x)
{
  uint8_t index, val;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  union
  {
    uint8_t b[4];
    uint32_t f;
  } data;

  data.f = x;

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);

  for (index = 0; index < 4; index++)
  {
    val = data.b[index];
    Wire.write(val);                                   //write the data
  }

  Wire.endTransmission();
}


void writeMemoryFloat(uint16_t addr, float x)
{
  uint8_t index, val;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  union
  {
    uint8_t b[4];
    float f;
  } data;

  data.f = x;

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);

  for (index = 0; index < 4; index++)
  {
    val = data.b[index];
    Wire.write(val);                                          //write the data
  }

  Wire.endTransmission();
}

/***************************************************************************
  Read Routines
 **************************************************************************
*/


char readMemoryChar(uint16_t addr)
{
  char data;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom((Memory_I2C_Addr + msb_addr), 1);
  data = Wire.read();
  return data;
}


int8_t readMemoryInt8(uint16_t addr)
{
  int8_t data;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom((Memory_I2C_Addr + msb_addr), 1);
  data = Wire.read();
  return data;
}


uint8_t readMemoryUint8(uint16_t addr)
{
  uint8_t data;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom((Memory_I2C_Addr + msb_addr), 1);
  data = Wire.read();
  return data;
}


int16_t readMemoryInt16(uint16_t addr)
{

  uint8_t lsb_data, msb_data;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom(Memory_I2C_Addr + msb_addr, 2);
  lsb_data = Wire.read();
  msb_data = Wire.read();

  return (lsb_data + (msb_data * 256));
}


uint16_t readMemoryUint16(uint16_t addr)
{

  uint8_t lsb_data, msb_data;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom(Memory_I2C_Addr + msb_addr, 2);
  lsb_data = Wire.read();
  msb_data = Wire.read();

  return (lsb_data + (msb_data * 256));
}


int32_t readMemoryInt32(uint16_t addr)
{
  uint8_t val, index;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom(Memory_I2C_Addr + msb_addr, 4);

  union
  {
    uint8_t b[4];
    uint32_t f;
  } readdata;

  for (index = 0; index < 4; index++)
  {
    val = Wire.read();
    readdata.b[index] = val;
  }

  return readdata.f;
}


uint32_t readMemoryUint32(uint16_t addr)
{
  uint8_t val, index;
  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom(Memory_I2C_Addr + msb_addr, 4);

  union
  {
    uint8_t b[4];
    uint32_t f;
  } readdata;

  for (index = 0; index < 4; index++)
  {
    val = Wire.read();
    readdata.b[index] = val;
  }

  return readdata.f;
}


float readMemoryFloat(uint16_t addr)
{
  uint8_t val, index;

  uint8_t msb_addr = (highByte(addr) & 0x07);            //get 3 MSB bits of address
  uint8_t lsb_addr = lowByte(addr);

  Wire.beginTransmission(Memory_I2C_Addr + msb_addr);
  Wire.write(lsb_addr);
  Wire.endTransmission();
  Wire.requestFrom(Memory_I2C_Addr + msb_addr, 4);

  union
  {
    uint8_t b[4];
    float f;
  } readdata;


  for (index = 0; index < 4; index++)
  {
    val = Wire.read();
    readdata.b[index] = val;
  }
  return readdata.f;
}


/***************************************************************************
  Start of general purpose memory routines
***************************************************************************/

uint16_t CRCMemory(uint16_t startaddr, uint16_t endaddr, uint16_t startval)
{
  uint16_t i, libraryCRC;
  uint8_t j;

  libraryCRC = startval;                                     //start value for CRC16, often 0xffff

  for (i = startaddr; i <= endaddr; i++)                     //element 4 is first character after $$$$ at start
  {
    libraryCRC ^= ((uint16_t)readMemoryUint8(i) << 8);
    for (j = 0; j < 8; j++)
    {
      if (libraryCRC & 0x8000)
        libraryCRC = (libraryCRC << 1) ^ 0x1021;
      else
        libraryCRC <<= 1;
    }
  }
  return libraryCRC;

}


void printMemory(uint16_t start_addr, uint16_t end_addr)
{
  //print the contents of Memory

  uint8_t value;

  for (uint16_t a = start_addr; a <= end_addr; a++)
  {
    value = readMemoryUint8(a);
    if ((a % 16) == 0)
    {
      Serial.println();
      Serial.print(F("0x"));
      if (a < 0x10)
      {
        Serial.print('0');
      }
      Serial.print(a, HEX);
      Serial.print(F(": "));
    }
    Serial.print(F("0x"));
    if (value < 0x10)
      Serial.print('0');
    Serial.print(value, HEX);
    Serial.print(F(" "));
  }
  Serial.println();
}


void fillMemory(uint16_t startaddr, uint16_t endaddr, uint8_t lval)
{
  uint16_t i;
  for (i = startaddr; i <= endaddr; i++)
  {
    writeMemoryUint8(i, lval);
  }
}


/*
  MIT license

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions
  of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/



