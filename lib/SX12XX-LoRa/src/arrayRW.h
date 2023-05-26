/*
  Copyright 2021 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 06/05/21
*/

//Changes
//February 2022, endarrayRW() returns last location written in array

uint32_t _arraylcn;
uint8_t *_arrayaddress;

void printArrayLocation(uint8_t num);


void beginarrayRW(uint8_t *buff, uint32_t lcn)
{
  _arrayaddress = buff;
  _arraylcn = lcn;                               //sets the current location in the array
}


uint8_t endarrayRW()
{
  //_arraylcn will have incremented by 1 at every write to the array, so to return the last location
  //written we need to subtract 1 from _arraylcn
  return _arraylcn - 1;
}


void printarrayHEX(uint8_t *buff, uint32_t len)
{
  uint8_t index, buffdata;

  for (index = 0; index < len; index++)
  {
    buffdata = buff[index];
    if (buffdata < 16)
    {
      Serial.print(F("0"));
    }
    Serial.print(buffdata, HEX);
    Serial.print(F(" "));
  }

}

void printArrayLocation(uint8_t num)
{
  Serial.print(num);
  Serial.print(F(",_arraylcn,"));
  Serial.println(_arraylcn);
}


//RW uint8_t **************************************
void arrayWriteUint8(uint8_t buffdata)
{
  _arrayaddress[_arraylcn++] = buffdata;
}

uint8_t arrayReadUint8()
{
  return _arrayaddress[_arraylcn++];
}
//**************************************************


//RW int8_t ****************************************
void arrayWriteInt8(int8_t buffdata)
{
  _arrayaddress[_arraylcn++] = buffdata;
}

int8_t arrayReadInt8()
{
  return _arrayaddress[_arraylcn++];
}
//**************************************************


//RW char ******************************************
void arrayWriteChar(char buffdata)
{
  _arrayaddress[_arraylcn++] = buffdata;
}

char arrayReadChar()
{
  return _arrayaddress[_arraylcn++];
}
//**************************************************


//RW uint16_t  *************************************
void arrayWriteUint16(uint16_t buffdata)
{
  _arrayaddress[_arraylcn++] = lowByte(buffdata);
  _arrayaddress[_arraylcn++] = highByte(buffdata);
}

uint16_t arrayReadUint16()
{
  uint16_t buffdata;
  buffdata = _arrayaddress[_arraylcn++];
  buffdata = buffdata + (_arrayaddress[_arraylcn++] << 8);
  return buffdata;
}
//**************************************************


//RW int16_t ***************************************
void arrayWriteInt16(int16_t buffdata)
{
  _arrayaddress[_arraylcn++] = lowByte(buffdata);
  _arrayaddress[_arraylcn++] = highByte(buffdata);
}

int16_t arrayReadInt16()
{
  uint16_t buffdata;
  buffdata = _arrayaddress[_arraylcn++];
  buffdata = buffdata + (_arrayaddress[_arraylcn++] << 8);
  return buffdata;
}
//**************************************************


//RW float *****************************************
void arrayWriteFloat(float tempf)
{
  memcpy ((_arrayaddress + _arraylcn), &tempf, 4);
  _arraylcn = _arraylcn + 4;
}


float arrayReadFloat()
{
  float tempf;
  memcpy (&tempf, (_arrayaddress + _arraylcn), 4);
  _arraylcn = _arraylcn + 4;
  return tempf;
}
//**************************************************


/*
  commented out, not supported by some platforms and produces a warning
  //RW double *****************************************
  void arrayWriteDouble(double tempf)
  {
   memcpy ((_arrayaddress + _arraylcn), &tempf, 8);
  _arraylcn = _arraylcn + 8;
  }


  float arrayReadDouble()
  {
   double tempd;
   memcpy (&tempd, (_arrayaddress + _arraylcn), 8);
  _arraylcn = _arraylcn + 8;
  return tempd;
  }
*/


//RW uint32_t **************************************
void arrayWriteUint32(uint32_t tempdata)
{
  memcpy ((_arrayaddress + _arraylcn), &tempdata, 4);
  _arraylcn = _arraylcn + 4;
}

uint32_t arrayReadUint32()
{
  uint32_t tempdata;
  memcpy (&tempdata, (_arrayaddress + _arraylcn), 4);
  _arraylcn = _arraylcn + 4;
  return tempdata;
}
//**************************************************


//RW int32_t **************************************
void arrayWriteInt32(int32_t tempdata)
{
  memcpy ((_arrayaddress + _arraylcn), &tempdata, 4);
  _arraylcn = _arraylcn + 4;
}

int32_t arrayReadInt32()
{
  int32_t tempdata;
  memcpy (&tempdata, (_arrayaddress + _arraylcn), 4);
  _arraylcn = _arraylcn + 4;
  return tempdata;
}
//*************************************************


//RW character arrays **************************************
void arrayWriteCharArray(char *buff, uint8_t len)
{
  uint8_t index = 0;

  for (index = 0; index < len; index++)
  {
    _arrayaddress[_arraylcn] = buff[index];
    _arraylcn++;
  }
}


void arrayReadCharArray(char *buff, uint8_t len)
{
  uint8_t index = 0;

  for (index = 0; index < len; index++)
  {
    buff[index] = _arrayaddress[_arraylcn];
    _arraylcn++;
  }
}
//*************************************************


//RW byte (uint8_t) arrays **************************************
void arrayWriteByteArray(uint8_t *buff, uint8_t len)
{
  uint8_t index = 0;

  for (index = 0; index < len; index++)
  {
    _arrayaddress[_arraylcn] = buff[index];
    _arraylcn++;
  }
}


void arrayReadByteArray(uint8_t *buff, uint8_t len)
{
  uint8_t index = 0;

  for (index = 0; index < len; index++)
  {
    buff[index] = _arrayaddress[_arraylcn];
    _arraylcn++;
  }
}
//*************************************************



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
