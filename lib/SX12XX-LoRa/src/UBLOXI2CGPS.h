/*
  Copyright 2020 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 27/06/20
*/

/*******************************************************************************************************
  Program Operation - This is a library file for the UBLOX 6,7 and 8 series GPSs.
  
  This GPS library file is designed for use with the GPSs I2C interface.

*******************************************************************************************************/



//For use with I2C the configurations sent must be even numbers of bytes, thus the last byte in some cases may be a 0x00 padding byte
const PROGMEM  uint8_t ClearConfig[]  =    {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x19, 0x98, 0x00}; //22
const PROGMEM  uint8_t GLONASSOff[]  =    {0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x00, 0x20, 0x01, 0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, 0x8F, 0xB2}; //20
const PROGMEM  uint8_t GPGLLOff[]  =      {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A}; //16
const PROGMEM  uint8_t GPGLSOff[]  =      {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46}; //16
const PROGMEM  uint8_t GPGSAOff[]  =      {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31}; //16
const PROGMEM  uint8_t GPGSVOff[]  =      {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38}; //16
const PROGMEM  uint8_t GNSSmode[] = {0xB5, 0x62, 0x06, 0x3E, 0x2C, 0x00, 0x00, 0x00, 0x20, 0x05, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E, 0x00, 
0x00, 0x00, 0x01, 0x01, 0xFC, 0x11};

const PROGMEM  uint8_t SetBalloonMode[]  =  {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27,
                                            0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC}; //44
const PROGMEM  uint8_t SaveConfig[]  =     {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA9, 0x00}; //22
const PROGMEM  uint8_t PollNavigation[]  = {0xB5, 0x62, 0x06, 0x24, 0x00, 0x00, 0x2A, 0x84}; //8
const PROGMEM  uint8_t SetCyclicMode[]  =  {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01, 0x22, 0x92}; //10
const PROGMEM  uint8_t SoftwareBackup[]  = {0xB5, 0x62, 0x06, 0x57, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x4B, 0x43, 0x42, 0x86, 0x46};  //16
const PROGMEM  uint8_t EnableI2C[]  =      {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x92}; //28
//const PROGMEM  uint8_t PMREQBackup[]  =    {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};  //16 

uint8_t GPS_GetByte();
void GPS_OutputOn();
void GPS_OutputOff();
void GPS_PowerOn(int8_t pin, uint8_t state);
void GPS_PowerOff(int8_t pin, uint8_t state);
bool GPS_Setup();
bool GPS_SendConfig(unsigned int Progmem_ptr, byte length, byte replylength);
bool GPS_WaitAck(unsigned long waitms, byte length);
//uint8_t GPS_GetNextChar(uint32_t waitmS);
//bool GPS_CheckAck();
bool GPS_SetBalloonMode();
bool GPS_CheckBalloonMode();
bool GPS_ClearConfig();
bool GPS_SetCyclicMode();
bool GPS_SoftwareBackup();
//bool GPS_HotStart();
bool GPS_PollNavigation();
bool GPS_SaveConfig();
bool GPS_GLONASSOff();
bool GPS_GPGLLOff();
bool GPS_GPGLSOff();
bool GPS_GPGSAOff();
bool GPS_GPGSVOff();
bool GPS_GNSSmode();
bool GPS_GGARMCOnly();
//void GPS_PMREQBackup();
//void GPS_StartRead();
//void GPS_SetGPMode();
//void GPS_StopMessages();

const int16_t GPSI2CAddress = 0x42;
const uint32_t GPS_WaitAck_mS = 1000;            //number of mS to wait for an ACK response from GPS
const uint8_t GPS_attempts = 5;                  //number of times the sending of GPS config will be attempted.
const uint8_t GPS_Reply_Size = 16;               //size of GPS reply buffer
const uint16_t GPS_Clear_DelaymS = 2000;         //mS to wait after a GPS Clear command is sent
uint8_t GPS_Reply[GPS_Reply_Size];               //byte array for storing GPS reply to UBX commands


#define USING_I2CGPS                             //so the rest of the program knows I2C GPS is in use
#define UBLOX                                    //so the rest of the program knows UBLOX GPS is in use
//#define GPSDEBUG                                 //include define to see some debug messages  



uint8_t GPS_GetByte()                            //get and process output from GPS
{
  uint8_t GPSchar;

  Wire.requestFrom(GPSI2CAddress, 1);
  GPSchar = Wire.read();

  return GPSchar;
}



void GPS_OutputOn()
{
//not used function for I2C library, included for compatibility with other libraries 
}


void GPS_OutputOff()
{
//not used function for I2C library, included for compatibility with other libraries 
}


/*
void GPS_StartRead()
{
  Wire.beginTransmission(GPSI2CAddress);
  Wire.write(0xFF);
}
*/

void GPS_PowerOn(int8_t pin, uint8_t state)
{
#ifdef GPSDebug
  Serial.print(F("GPS_PowerOn() "));
#endif
  
  if (pin >= 0)
  {
  digitalWrite(pin, state); 
  }
  
}

 
void GPS_PowerOff(int8_t pin, uint8_t state)
{
#ifdef GPSDebug
  Serial.print(F("GPS_PowerOff() "));
#endif

if (pin >= 0)
  {
  digitalWrite(pin, state); 
  }

}


bool GPS_WaitAck(uint32_t waitmS, uint8_t length)
{
  //wait for Ack from GPS
  byte i, j;
  uint32_t startmS;
  
  startmS = millis();
   
  byte ptr = 0;                             //used as pointer to store GPS reply

  Wire.beginTransmission(GPSI2CAddress);
  Wire.write(0xFF);

  do
  {
    Wire.requestFrom(GPSI2CAddress, 1);
    i = Wire.read();
  }
  while ((i != 0xb5) && ((uint32_t) (millis() - startmS) < waitmS));

  if (i != 0xb5)
  {
    Serial.print(F("Timeout "));
    return false;
  }
  else
  {
    Serial.print(F("Ack "));
    Serial.print(i, HEX);

    length--;

    for (j = 1; j <= length; j++)
    {
      Serial.print(F(" "));

      Wire.requestFrom(GPSI2CAddress, 1);
      i = Wire.read();

      if (j < 12)
      {
        GPS_Reply[ptr++] = i;                    //save reply in buffer, but no more than 10 characters
      }

      if (i < 0x10)
      {
        Serial.print(F("0"));
      }
      Serial.print(i, HEX);
    }

  }
  Serial.println();
  return true;
}


bool GPS_SendConfig(const uint8_t *Progmem_ptr, uint8_t arraysize, uint8_t replylength, uint8_t attempts)
{
  uint8_t byteread1, byteread2, index, length;
  uint8_t config_attempts = attempts;

  do
  {

    if (config_attempts == 0)
    {
      Serial.println(F("Fail"));
      Serial.println();
      return false;
    }

    length = arraysize / 2;                     //we are sending messages 2 bytes at a time

    for (index = 0; index < length; index++)
    {
      byteread1 = pgm_read_byte_near(Progmem_ptr++);      //we will read and write 2 bytes at a time 
      byteread2 = pgm_read_byte_near(Progmem_ptr++);
      Wire.beginTransmission(GPSI2CAddress);
      Wire.write(byteread1);
      Wire.write(byteread2);
      Wire.endTransmission();
      if (byteread1 < 0x10)
      {
        Serial.print(F("0"));
      }
      Serial.print(byteread1, HEX);
      Serial.print(F(" "));
      if (byteread2 < 0x10)
      {
        Serial.print(F("0"));
      }
      Serial.print(byteread2, HEX);
      Serial.print(F(" "));
    }

    Progmem_ptr = Progmem_ptr - arraysize;     //put Progmem_ptr back to start value in case we need to re-send the config

    Serial.println();

    if (replylength == 0)
    {  
       #ifdef GPSDebug
       Serial.println(F("Reply not required"));
       #endif
       break;
    }

    config_attempts--;
  } while (!GPS_WaitAck(GPS_WaitAck_mS, replylength));
  
  delay(50);                                  //GPS can sometimes be a bit slow getting ready for next config
  return true;
}


bool GPS_Setup()
{
  #ifdef GPSDebug
  Serial.println(F("GPS_Setup()"));
  #endif
  /*
  Wire.begin();
  GPS_ClearConfig();
  GPS_StopMessages();
  GPS_SetBalloonMode();
  GPS_SaveConfig();
  */
  
  Wire.begin();
  
  if (!GPS_ClearConfig())
  {
    return false;
  }

  if (!GPS_SetBalloonMode())
  {
    return false;
  }
  
  if (!GPS_GNSSmode())
  {
    return false;
  } 

  if (!GPS_GPGLLOff())
  {
    return false;
  } 

   if (!GPS_GPGLSOff())
  {
    return false;
  }  

  if (!GPS_GPGSAOff())
  {
    return false;
  }  

  if (!GPS_GPGSVOff())
  {
    return false;
  }   

  if (!GPS_SaveConfig())
  {
    return false;
  }

  return true;
}


bool GPS_CheckBalloonMode()
{ 
  #ifdef GPSDebug
  Serial.println(F("GPS_CheckBalloonMode()"));
  #endif

  uint8_t j;

  GPS_Reply[7] = 0xff;

  GPS_PollNavigation();

  j = GPS_Reply[7];

  Serial.print(F("Dynamic Model is "));
  Serial.println(j);

  if (j != 6)
  {
    Serial.println(F("Dynamic Model 6 not Set !"));
    return false;
  }
  else
  {
    return true;
  }
}

bool GPS_ClearConfig()
{
  #ifdef GPSDebug
  Serial.println(F("GPS_ClearConfig()"));
  #endif

  Serial.println(F("ClearConfig"));
  size_t SIZE = sizeof(ClearConfig);  
  
  if (!GPS_SendConfig(ClearConfig,SIZE,10,GPS_attempts))
  {
   return false;
  }

  Serial.println(F("Wait clear"));
  delay(GPS_Clear_DelaymS);                            //wait a while for GPS to clear its settings 
    
  return true;
}

/*
void GPS_StopMessages()
{
  #ifdef GPSDebug
  Serial.print(F("GPS GPGLLOff "));
  #endif
  
  size_t SIZE;
  
  SIZE = sizeof(GPGLLOff); 
  GPS_SendConfig(GPGLLOff, SIZE, 10, GPS_attempts);

  #ifdef GPSDebug
  Serial.print(F("GPS GPGLSOff "));
  #endif
  
  SIZE = sizeof(GPGLSOff); 
  GPS_SendConfig(GPGLSOff, SIZE, 10, GPS_attempts);

  #ifdef GPSDebug
  Serial.print(F("GPS GPGSAOff "));
  #endif
  
  SIZE = sizeof(GPGSAOff); 
  GPS_SendConfig(GPGSAOff, SIZE, 10, GPS_attempts);

  #ifdef GPSDebug
  Serial.print(F("GPS GPGSVOff "));
  #endif
  
  SIZE = sizeof(GPGSVOff); 
  GPS_SendConfig(GPGSVOff, SIZE, 10, GPS_attempts);
}
*/

bool GPS_SetBalloonMode()
{
  #ifdef GPSDebug
  Serial.print(F("GPS SetBalloonMode "));
  #endif
  
  size_t SIZE = sizeof(SetBalloonMode); 
  return GPS_SendConfig(SetBalloonMode, SIZE, 10, GPS_attempts);
}


bool GPS_SaveConfig()
{
  #ifdef GPSDebug
  Serial.print(F("GPS_SaveConfig()"));
  #endif
  
  size_t SIZE = sizeof(SaveConfig); 
  return GPS_SendConfig(SaveConfig, SIZE, 10, GPS_attempts);
}


bool GPS_PollNavigation()
{
  #ifdef GPSDebug
  Serial.print(F("GPS_PollNavigation()"));
  #endif

  size_t SIZE = sizeof(PollNavigation); 
  return GPS_SendConfig(PollNavigation, SIZE, 44, GPS_attempts);
}

/*
void GPS_SetGPMode()
{
  #ifdef GPSDebug
  Serial.print(F("GPS_SetGPMode()"));
  #endif
    
  size_t SIZE = sizeof(SetCyclicMode); 
  GPS_SendConfig(SetCyclicMode, SIZE, 10, GPS_attempts);
}
*/

bool GPS_SetCyclicMode()
{
#ifdef GPSDebug
  Serial.print(F("GPS_SetCyclicMode() "));
#endif

  Serial.println(F("SetCyclicMode"));
  size_t SIZE = sizeof(SetCyclicMode);
  
  return GPS_SendConfig(SetCyclicMode, SIZE, 10, GPS_attempts);
}


bool GPS_SoftwareBackup()
{
  #ifdef GPSDebug
  Serial.print(F("GPS_SoftwareBackup()"));
  #endif
  
  size_t SIZE = sizeof(SoftwareBackup); 
  return GPS_SendConfig(SoftwareBackup, SIZE, 0, GPS_attempts);
}

/*
void GPS_PMREQBackup()
{
  #ifdef GPSDebug
  Serial.print(F("GPS_PMREQBackup()"));
  #endif

  size_t SIZE = sizeof(PMREQBackup); 
  GPS_SendConfig(PMREQBackup, SIZE, 0, GPS_attempts);
}
*/


bool GPS_GLONASSOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GLONASSOff() "));
#endif

  size_t SIZE = sizeof(GLONASSOff);
  return GPS_SendConfig(GLONASSOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGLLOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGLLOff() "));
#endif

  size_t SIZE = sizeof(GPGLLOff);
  return GPS_SendConfig(GPGLLOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGLSOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGLSOff() "));
#endif

  size_t SIZE = sizeof(GPGLSOff);
  return GPS_SendConfig(GPGLSOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGSAOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGSAOff() "));
#endif

  size_t SIZE = sizeof(GPGSAOff);
  return GPS_SendConfig(GPGSAOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGSVOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGSVOff() "));
#endif

  size_t SIZE = sizeof(GPGSVOff);
  return GPS_SendConfig(GPGSVOff, SIZE, 10, GPS_attempts);
}


bool GPS_GNSSmode()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GNSSmode() "));
#endif

  size_t SIZE = sizeof(GNSSmode);
  return GPS_SendConfig(GNSSmode, SIZE, 10, GPS_attempts);
}


bool GPS_GGARMCOnly()
{
//null function, not used with Ublox library
return true;  
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
