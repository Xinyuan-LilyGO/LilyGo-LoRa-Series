/*
  Copyright 2020 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 12/05/20
*/


/*******************************************************************************************************
  Program Operation - This is a library file for the Quectel L70,L76,L80 and L86 GPSs
  
  The routines assume that the GPS has been setup on GPSserial which could be either software serial or
  hardware serial. The calling program should include a define for the GPS baud rate as follows
  
  #define GPSBaud 9600
  
  If this define is missing then 9600 baud is assumed
*******************************************************************************************************/

const PROGMEM  uint8_t SetBalloonMode[]  = {"$PMTK886,3*2B"}; //response should be $PMTK001,886,3*36
const PROGMEM  uint8_t ClearConfig[]  = {"$PMTK104*37"};      //no response  
const PROGMEM  uint8_t PMTK_ACK[]  = {"$PMTK001,xxx,?"};      //? = 0 = invalid, 1 = unsupported, 2 = valid failed, 3 = valids succeeded 
const PROGMEM  uint8_t SoftwareBackup[]  = {"$PMTK161,0*28"}; //response should be $PMTK001,161,3*36
const PROGMEM  uint8_t HotStart[]  = {"$PMTK101*32"};

uint8_t GPS_GetByte();
void GPS_OutputOn();
void GPS_OutputOff();
void GPS_PowerOn(int8_t pin, uint8_t state);
void GPS_PowerOff(int8_t pin, uint8_t state);
bool GPS_Setup();
bool GPS_SendConfig(const uint8_t *Progmem_ptr, uint8_t arraysize, uint8_t replylength, uint8_t attempts);
bool GPS_WaitChar(uint8_t waitforchar, uint32_t waitmS);
uint8_t GPS_GetNextChar(uint32_t waitmS);
bool GPS_WaitAck(uint32_t waitms, uint8_t length);
bool GPS_CheckConfiguration();
bool GPS_CheckAck();
bool GPS_SetBalloonMode();
bool GPS_ClearConfig();
bool GPS_SetCyclicMode();
bool GPS_SoftwareBackup();
bool GPS_HotStart();


const uint32_t GPS_WaitAck_mS = 2000;             //number of mS to wait for an ACK response from GPS
const uint8_t GPS_attempts = 5;                   //number of times the sending of GPS config will be attempted.
const uint8_t GPS_Reply_Size = 20;                //size of GPS reply buffer
const uint16_t GPS_Clear_DelaymS = 2000;          //mS to wait after a GPS Clear command is sent

uint8_t GPS_Reply[GPS_Reply_Size];                //byte array for storing GPS reply to UBX commands
 
#define QUECTELINUSE                              //so complier can know which GPS library is used                             //so complier can know which GPS library is used
//#define GPSDebug

#ifndef GPSConfigSerial                           // if GPSDebugSerial is not defined set to Serial as default
 #define GPSConfigSerial Serial
#endif

#ifndef GPSDebugSerial                           // if GPSDebugSerial is not defined set to Serial as default
 #define GPSDebugSerial Serial
#endif

#ifndef GPSBaud
#define GPSBaud 9600
#endif

void GPS_OutputOn()
{
#ifdef GPSDebug
  Serial.print(F("GPS_OutputOn() "));
#endif
  //turns on serial output from GPS
  GPSserial.begin(GPSBaud);
}


void GPS_OutputOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_OutputOff() "));
#endif
  //turns off serial output from GPS
  GPSserial.end();
}


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


bool GPS_Setup()
{
#ifdef GPSDebug
  Serial.print(F("GPS_Setup() "));
#endif

  if (!GPS_SetBalloonMode())
  {
  return false;
  }
  
  return true;
}


bool GPS_SendConfig(const uint8_t *Progmem_ptr, uint8_t arraysize, uint8_t replylength, uint8_t attempts)
{
#ifdef GPSDebug
  Serial.print(F("GPS_SendConfig() "));
#endif

  uint8_t byteread, index;
  uint8_t config_attempts = attempts;

  memset(GPS_Reply, 0, sizeof(GPS_Reply));  //clear the reply buffer

  Serial.flush();                           //ensure there are no pending interrupts from serial monitor printing

  do
  {
    if (config_attempts == 0)
    {
      return false;
    }

    Serial.print(F("GPSSend   "));

    for (index = 0; index < arraysize; index++)
    {
      byteread = pgm_read_byte_near(Progmem_ptr++);
      Serial.write(byteread);
    }

    Serial.flush();          //make sure serial out buffer is empty

    Progmem_ptr = Progmem_ptr - arraysize;                  //set Progmem_ptr back to start

    for (index = 0; index < arraysize; index++)
    {
      byteread = pgm_read_byte_near(Progmem_ptr++);
      GPSserial.write(byteread);
    }

    Progmem_ptr = Progmem_ptr - arraysize;                  //set Progmem_ptr back to start, in case config command retried


    GPSserial.write(13);
    GPSserial.write(10);

    if (replylength == 0)
    {
      break;
    }

    config_attempts--;
  }
  while (!GPS_WaitAck(GPS_WaitAck_mS, replylength));

  delay(100);                                    //GPS can sometimes be a bit slow getting ready for next config
  return true;
}


bool GPS_WaitChar(uint8_t waitforchar, uint32_t waitmS)
{
  uint8_t GPSchar;

  do
  {
    if (GPSserial.available())
    {
      GPSchar = GPSserial.read();
      if (GPSchar == waitforchar)
      {
        GPS_Reply[0] = GPSchar;
        return true;
      }
    }
  }
  while ((millis() < waitmS));   //use the timeout to ensure a lack of GPS does not cause the program to hang
  return false;
}


uint8_t GPS_GetNextChar(uint32_t waitmS)
{
  uint8_t GPSchar;

  do
  {
    if (GPSserial.available())
    {
      GPSchar = GPSserial.read();
      GPS_Reply[1] = GPSchar;
      return GPSchar;
    }
  }
  while ((millis() < waitmS));   //use the timeout to ensure a lack of GPS does not cause the program to hang
  return '*';
}


bool GPS_WaitAck(uint32_t waitmS, uint8_t length)
{
#ifdef GPSDebug
  Serial.print(F("GPS_WaitAck() "));
  Serial.print(F(" Reply length "));
  Serial.print(length);
  Serial.print(F(" "));
#endif

  uint8_t GPSchar;

  uint32_t endmS;
  endmS = millis() + waitmS;
  uint8_t ptr = 0;                             //used as pointer to store GPS reply
  bool found;

  Serial.println();
  Serial.print(F("Received  "));
  Serial.flush();
  
  endmS = millis() + GPS_WaitAck_mS;

  found = false;

  do
  {
    if (!GPS_WaitChar('$', endmS))
    {
      Serial.print(F("Timeout Error"));
      found = false;
      break;
    }

    Serial.print(F("$"));

    GPSchar = GPS_GetNextChar(endmS);
    Serial.write(GPSchar);

    if (GPSchar == 'P')
    {
      found = true;
      break;
    }
    else
    {
      Serial.print(F(" "));
      found = false;
    }
  }
  while (millis() <= endmS);

  if (found)
  {
    ptr = 2;
    do
    {
      if (GPSserial.available())
      {
        GPSchar = GPSserial.read();
        GPS_Reply[ptr++] = GPSchar;
      }
    }
    while ((millis() < endmS) && (ptr < length));   //use the timeout to ensure a lack of GPS does not cause the program to hang
  }


  for (ptr = 2; ptr < length; ptr++)
  {
    GPSchar = GPS_Reply[ptr];
    Serial.write(GPSchar);
  }

  Serial.println();

  if (GPS_CheckAck())
  {
  return true;
  }

  return false;
}


bool GPS_CheckConfiguration()
{
  //Reply to set balloon mode ought to be $PMTK886,3*2B, the 3 is for balloon mode

#ifdef GPSDebug
  Serial.print(F("GPS_CheckConfiguration() "));
#endif

  if (GPS_SetBalloonMode())
  {
    return true;
  }
  return false;
}



bool GPS_CheckAck()
{

#ifdef GPSDebug
  Serial.print(F("GPS_CheckAck() "));
#endif

  uint8_t response[] = {"$PMTK001,xxx,?"};
  uint8_t index;

  for (index = 2; index <= 7; index++)        //check up to the eighth ([7]) character, which is the ','
  {
    if ( GPS_Reply[index] != response[index])
    {
      return false;
    }
  }

  if ((index == 8) && (GPS_Reply[13] == '3') )   //if we got to index 8 there was a match for $PMTK001, so check reply type
  {
  return true;  
  }
  
  Serial.print(index);
  Serial.print(F(" "));
  return false;
}


/*********************************************************************
  // GPS configuration commands
*********************************************************************/


bool GPS_SetBalloonMode()
{
#ifdef GPSDebug
  Serial.print(F("GPS_SetBalloonMode() "));
#endif

  Serial.println(F("SetBalloonMode "));
  size_t SIZE = sizeof(SetBalloonMode);
  
  if (GPS_SendConfig(SetBalloonMode, SIZE, 17, GPS_attempts))
  {
  Serial.println(F("OK"));
  Serial.println();
  return true;  
  }
  Serial.println(F("Fail"));
  Serial.println();
  return false;
}


bool GPS_ClearConfig()
  {
  Serial.println(F("ClearConfig()"));
  size_t SIZE = sizeof(ClearConfig);
  GPS_SendConfig(ClearConfig, SIZE, 0, GPS_attempts);       //full cold start, no response is given, so replylength = 0

  #ifdef GPSDebug
  Serial.print(F("No Response given for full cold start"));
  #endif
  
  Serial.println();
  Serial.println(F("Wait clear"));
  delay(GPS_Clear_DelaymS);
  return true;
  }


bool GPS_SetCyclicMode()
  {
  #ifdef GPSDebug
  Serial.print(F("GPS_SetCyclicMode() "));
  #endif
  //no clyclic mode config for Quectel
  return true;
  
  }


 bool GPS_SoftwareBackup()
  {
  #ifdef GPSDebug
  Serial.print(F("GPS_SoftwareBackup() "));
  #endif

  size_t SIZE = sizeof(SoftwareBackup);
  
  if (GPS_SendConfig(SoftwareBackup, SIZE, 0, GPS_attempts))
  {
  return true;  
  }
  
  return false;
} 


bool GPS_HotStart()
  {
  #ifdef GPSDebug
  Serial.print(F("GPS_HotStart() "));
  #endif

  size_t SIZE = sizeof(HotStart);
  Serial.println(F("HotStart"));
  
  if (GPS_SendConfig(HotStart, SIZE, 0, GPS_attempts))           //reply is $PMTK010,002*2D, so not receognised by check ack
  {
  return true;  
  }
  
  return false;
} 


uint8_t GPS_GetByte()                                            //get a byte for GPS
{
  if (GPSserial.available() ==  0)
  {
    return 0xFF;                                                 //for compatibility with I2C reading of GPS 
  }
  else
  {
    return GPSserial.read();
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

