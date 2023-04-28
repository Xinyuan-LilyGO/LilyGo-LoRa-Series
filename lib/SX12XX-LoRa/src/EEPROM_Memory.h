/*
  Copyright 2020 - Stuart Robinson
  24/04/20
*/

/*******************************************************************************************************
  Library Operation - This library is for reading from and writing to the EEPROM. These routines provide 
  a funtional match to the libraries for FRAM.

*******************************************************************************************************/

#include <EEPROM.h>
#define LTUNUSED(v) (void) (v)       //add LTUNUSED(variable); in functions to avoid compiler warnings 

void memoryStart(uint16_t addr)      //yes I know the EEPROM does not have an address, but the other memory types do 
{
 //left empty for future use
 LTUNUSED(addr);                     //avoids compliler warning
}


void memoryEnd()
{
 //left empty for future use
}


void writeMemoryUint8(uint16_t addr, uint8_t x)
{
  //write a byte to the EEPROM
  EEPROM.put(addr, x);
}


uint16_t readMemoryUint16(uint16_t addr)
{
  uint16_t x;
  EEPROM.get(addr, x);
  return x;
}


uint32_t readMemoryUint32(uint16_t addr)
{
  uint32_t x;
  EEPROM.get(addr, x);
  return x;
}


void writeMemoryUint32(uint16_t addr, uint32_t x)
{
  EEPROM.put(addr, x);
}


void writeMemoryUint16(uint16_t addr, uint16_t x)
{
  EEPROM.put(addr, x);
}


float readMemoryFloat(uint16_t addr)
{
  float x;
  EEPROM.get(addr, x);
  return x;
}


void writeMemoryFloat(uint16_t addr, float x)
{
  EEPROM.put(addr, x);
}


void fillMemory(uint16_t startaddr, uint16_t endaddr, uint8_t lval)
{
  uint32_t i;
  for (i = startaddr; i <= endaddr; i++)
  {
    writeMemoryUint8(i, lval);
  }
}




