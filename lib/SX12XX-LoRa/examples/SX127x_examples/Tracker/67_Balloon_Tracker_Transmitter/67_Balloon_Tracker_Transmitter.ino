/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a tracker intended for use as a high altitude balloon (HAB) tracker. The
  program sends out a standard format payload with LoRa that is compatible with the HABHUB online tracking
  system.

  The HAB payload is constructed thus;

  PayloadID,Sequence,Time,Lat,Lon,Alt,Satellites,Volts,Temperature,Resets,Status,Errors,TXGPSfixms,Checksum
  Field 0      1      2    3   4   5      6        7       8         9      10     11        12       13

  The LoRa and frequency settings can be changed in the Settings.h file. There is the option of sending
  out a much shorter Search mode binary location only payload. This is intended for ground based searching
  and locating. The frequency and LoRa settings of the Search mode packet can be different to the Tracker
  mode used by the HAB payload. There is also the option of sending the HAB payload in FSK RTTY format,
  see the Settings.h file for all the options. FSK RTTY gets sent at the same frequency as the Tracker mode
  HAB packet. The LT.transmitFSKRTTY() function sends at 1 start bit, 7 data bits, no parity and 2 stop bits.
  For full control of the FSK RTTY setting you can use the following alternative function;

  LT.transmitFSKRTTY(chartosend, databits, stopbits, parity, baudPerioduS, pin)

  There is a matching Balloon Tracker Receiver program which writes received data to the Serial monitor as well
  as a small OLED display.

  In the Settings.h file you can set the configuration for either a Ublox GPS or a Quectel L70\L80. The GPSs
  are configured for high altitude balloon mode.

  It is strongly recommended that a FRAM option is fitted for this transmitter. The sequence, resets and error
  nembers are stred in non-volatile memory. This defaults to EEPROM which has a limited endurance of only
  100,000 writes, so in theory the limt is reached after the transmission of 100,000 hab packets. The use of
  a FRAM will extend the life of the tracker to circa 100,000,000,000,000 transmissions.

  Changes:
  240420 - Change to work with Easy Pro Mini style modules
  300420 - Improve error detection for UBLOX GPS library

  ToDo:  - Add TC74 temperature sensor

  Serial monitor baud rate is set at 115200
*******************************************************************************************************/

#include <Arduino.h>
#include <SX127XLT.h>                           //get library here >https://github.com/StuartsProjects/SX12XX-LoRa  

SX127XLT LT;                                    //create a library class instance called LT

#include "Settings.h"
#include "ProgramLT_Definitions.h"

//**************************************************************************************************
// HAB tracker data - these are the variables transmitted in payload
//**************************************************************************************************
uint32_t TXSequence;                             //sequence number of payload
uint8_t TXHours;                                 //Hours
uint8_t TXMinutes;                               //Minutes
uint8_t TXSeconds;                               //Seconds
float TXLat;                                     //latitude from GPS
float TXLon;                                     //longitude from GPS
uint16_t TXAlt;                                  //altitude from GPS
uint8_t TXSatellites;                            //satellites used by GPS
uint16_t TXVolts;                                //measured tracker supply volts
int8_t TXTemperature;                            //measured temperature
uint16_t TXResets;                               //number of tracker resets
uint8_t TXStatus = 0;                            //used to store current status flag bits
uint16_t TXErrors;                               //number of tracker Errors
uint32_t TXGPSfixms;                             //fix time of GPS
//**************************************************************************************************

uint8_t TXPacketL;                               //length of LoRa packet sent
uint8_t  TXBUFFER[TXBUFFER_SIZE];                //buffer for packet to send

#include Memory_Library

#include <SPI.h>

#include <TinyGPS++.h>                           //http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                 //create the TinyGPS++ object

//#include <NeoSWSerial.h>                       //https://github.com/SlashDevin/NeoSWSerial
//NeoSWSerial GPSserial(RXpin, TXpin);           //alternative - this library is more relaible at GPS init than software serial

#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);


#include GPS_Library                             //include previously defined GPS Library 

uint32_t GPSstartms;                             //start time waiting for GPS to get a fix

#include <Wire.h>

uint8_t temperature = 0;


void loop()
{
  Serial.println(F("Start Loop"));

  GPSstartms = millis();

  if (!gpsWaitFix(WaitGPSFixSeconds))
  {
    GPS_OutputOff();
    sendCommand(NoFix);                          //report a GPS fix error
    delay(1000);                                 //give receiver enough time to report NoFix
  }
  Serial.println();

  do_Transmissions();                            //do the transmissions

  Serial.println(F("Sleep"));
  LT.setSleep(CONFIGURATION_RETENTION);          //put LoRa device to sleep, preserve lora register settings
  Serial.flush();                                //make sure no serial output pending before goint to sleep

  delay(SleepTimesecs * 1000);

  Serial.println(F("Wake"));
  LT.wake();                                     //wake the LoRa device from sleep
}


void do_Transmissions()
{
  //this is where all the transmisions get sent
  uint32_t startTimemS;
  uint8_t index;

  incMemoryUint32(addr_SequenceNum);             //increment Sequence number

  if (readConfigByte(SearchEnable))
  {
    setSearchMode();
    TXPacketL = buildLocationOnly(TXLat, TXLon, TXAlt, TXStatus);  //put location data in SX12xx buffer
    Serial.print(F("Search packet > "));
    Serial.print(TXLat, 5);
    Serial.print(F(","));
    Serial.print(TXLon, 5);
    Serial.print(F(","));
    Serial.print(TXAlt);
    Serial.print(F(","));
    Serial.print(TXStatus);
    digitalWrite(LED1, HIGH);
    startTimemS = millis();
    TXPacketL = LT.transmitSXBuffer(0, TXPacketL, 10000, SearchTXpower, WAIT_TX);
    printTXtime(startTimemS, millis());
    reportCompletion();
    Serial.println();
  }

  delay(1000);                                        //gap between transmissions

  setTrackerMode();

  TXPacketL = buildHABPacket();
  Serial.print(F("HAB Packet > "));
  printBuffer(TXBUFFER, (TXPacketL + 1));             //print the buffer (the packet to send) as ASCII
  digitalWrite(LED1, HIGH);
  startTimemS = millis();
  TXPacketL = LT.transmit(TXBUFFER, (TXPacketL + 1), 10000, TrackerTXpower, WAIT_TX); //will return packet length sent if OK, otherwise 0 if transmit error
  digitalWrite(LED1, LOW);
  printTXtime(startTimemS, millis());
  reportCompletion();
  Serial.println();

  delay(1000);                                        //gap between transmissions

  if (readConfigByte(FSKRTTYEnable))
  {
    LT.setupDirect(TrackerFrequency, Offset);
    LT.startFSKRTTY(FrequencyShift, NumberofPips, PipPeriodmS, PipDelaymS, LeadinmS);

    startTimemS = millis() - LeadinmS;

    Serial.print(F("FSK RTTY > $$$"));
    Serial.flush();
    LT.transmitFSKRTTY('$', BaudPerioduS, LED1);            //send a '$' as sync
    LT.transmitFSKRTTY('$', BaudPerioduS, LED1);            //send a '$' as sync
    LT.transmitFSKRTTY('$', BaudPerioduS, LED1);            //send a '$' as sync

    for (index = 0; index <= (TXPacketL - 1); index++)      //its  TXPacketL-1 since we dont want to send the null at the end
    {
      LT.transmitFSKRTTY(TXBUFFER[index], BaudPerioduS, LED1);
      Serial.write(TXBUFFER[index]);
    }

    LT.transmitFSKRTTY(13, BaudPerioduS, LED1);              //send carriage return
    LT.transmitFSKRTTY(10, BaudPerioduS, LED1);              //send line feed
    LT.endFSKRTTY(); //stop transmitting carrier
    digitalWrite(LED1, LOW);                                 //LED off
    printTXtime(startTimemS, millis());
    TXPacketL += 4;                                          //add the two $ at beginning and CR/LF at end
    reportCompletion();
    Serial.println();
  }
}


void printTXtime(uint32_t startmS, uint32_t endmS)
{
  Serial.print(F(" "));
  Serial.print(endmS - startmS);
  Serial.print(F("mS"));
}


void reportCompletion()
{
  Serial.print(F(" "));
  if (TXPacketL == 0)
  {
    Serial.println();
    reporttransmitError();
  }
  else
  {
    Serial.print(TXPacketL);
    Serial.print(F("bytes"));
    setStatusByte(LORAError, 0);
  }
}


void printBuffer(uint8_t *buffer, uint8_t size)
{
  uint8_t index;

  for (index = 0; index < size; index++)
  {
    Serial.write(buffer[index]);
  }
}


uint8_t buildHABPacket()
{
  //build the HAB tracker payload
  uint16_t index, j, CRC;
  uint8_t Count, len;
  char LatArray[12], LonArray[12];

  TXSequence = readMemoryUint32(addr_SequenceNum);               //Sequence number is kept in non-volatile memory so it survives TXResets
  TXResets =  readMemoryUint16(addr_ResetCount);                 //reset count is kept in non-volatile memory so it survives TXResets
  TXVolts = readSupplyVoltage();
  TXTemperature = readTempTC74(TC74_ADDRESS);
  TXErrors = readMemoryUint16(addr_TXErrors);

  dtostrf(TXLat, 7, 5, LatArray);                                //format is dtostrf(FLOAT,WIDTH,PRECISION,BUFFER);
  dtostrf(TXLon, 7, 5, LonArray);                                //converts float to character array

  len = sizeof(TXBUFFER);
  memset(TXBUFFER, 0, len);                                      //clear array to 0s
  Count = snprintf((char*) TXBUFFER,
                   TXBUFFER_SIZE,
                   "$%s,%lu,%02d:%02d:%02d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%lu",
                   FlightID,
                   TXSequence,
                   TXHours,
                   TXMinutes,
                   TXSeconds,
                   LatArray,
                   LonArray,
                   TXAlt,
                   TXSatellites,
                   TXVolts,
                   TXTemperature,
                   TXResets,
                   TXStatus,
                   TXErrors,
                   TXGPSfixms
                  );

  CRC = 0xffff;                                   //start value for CRC16

  for (index = 1; index < Count; index++)         //element 1 is first character after $ at start (for LoRa)
  {
    CRC ^= (((uint16_t)TXBUFFER[index]) << 8);
    for (j = 0; j < 8; j++)
    {
      if (CRC & 0x8000)
        CRC = (CRC << 1) ^ 0x1021;
      else
        CRC <<= 1;
    }
  }

  TXBUFFER[Count++] = '*';
  TXBUFFER[Count++] = Hex((CRC >> 12) & 15);      //add the checksum bytes to the end
  TXBUFFER[Count++] = Hex((CRC >> 8) & 15);
  TXBUFFER[Count++] = Hex((CRC >> 4) & 15);
  TXBUFFER[Count] = Hex(CRC & 15);
  return Count;
}


char Hex(uint8_t lchar)
{
  //used in CRC calculation in buildHABPacket
  char Table[] = "0123456789ABCDEF";
  return Table[lchar];
}


uint8_t buildLocationOnly(float Lat, float Lon, uint16_t Alt, uint8_t stat)
{
  uint8_t len;
  LT.startWriteSXBuffer(0);                   //initialise buffer write at address 0
  LT.writeUint8(LocationBinaryPacket);        //identify type of packet
  LT.writeUint8(Broadcast);                   //who is the packet sent too
  LT.writeUint8(ThisNode);                    //tells receiver where is packet from
  LT.writeFloat(Lat);                         //add latitude
  LT.writeFloat(Lon);                         //add longitude
  LT.writeInt16(Alt);                         //add altitude
  LT.writeUint8(stat);                        //add tracker status
  len = LT.endWriteSXBuffer();                //close buffer write
  return len;
}


void reporttransmitError()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();              //read the the interrupt register
  Serial.print(F("TXError,"));
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);                //print IRQ status
  LT.printIrqStatus();                         //prints the text of which IRQs set
  incMemoryUint16(addr_TXErrors);              //increase the error count
  setStatusByte(LORAError, 1);
}


void incMemoryUint32(uint32_t addr)
{
  uint32_t val = readMemoryUint32(addr);
  val++;
  writeMemoryUint32(addr, val);
}


void incMemoryUint16(uint32_t addr)
{
  uint16_t val = readMemoryUint16(addr);
  val++;
  writeMemoryUint16(addr, val);
}


void setStatusByte(uint8_t bitnum, uint8_t bitval)
{
  //program the status byte

  if (bitval == 0)
  {
    bitClear(TXStatus, bitnum);
  }
  else
  {
    bitSet(TXStatus, bitnum);
  }
}


uint8_t readConfigByte(uint8_t bitnum)
{
  return bitRead(Default_config1, bitnum);
}


void setTrackerMode()
{
  Serial.println(F("setTrackerMode"));
  LT.setupLoRa(TrackerFrequency, Offset, TrackerSpreadingFactor, TrackerBandwidth, TrackerCodeRate, TrackerOptimisation);
}


void setSearchMode()
{
  Serial.println(F("setSearchMode"));
  LT.setupLoRa(SearchFrequency, Offset, SearchSpreadingFactor, SearchBandwidth, SearchCodeRate, SearchOptimisation);
}


uint8_t sendCommand(char cmd)
{
  uint8_t len;
  TXVolts = readSupplyVoltage();
  Serial.print(F("Send Cmd "));
  Serial.write(cmd);
  Serial.println();

  LT.startWriteSXBuffer(0);                 //start the write packet to buffer process
  LT.writeUint8(cmd);                       //this byte defines the packet type
  LT.writeUint8(Broadcast);                 //destination address of the packet, the receivers address
  LT.writeUint8(ThisNode);                  //source address of this node
  LT.writeUint16(TXVolts);                  //add the battery voltage
  LT.writeUint8(TXStatus);                  //add the status byte
  len = LT.endWriteSXBuffer();              //close the packet, get the length of data to be sent

  //now transmit the packet, set a timeout of 5000mS, wait for it to complete sending

  digitalWrite(LED1, HIGH);                 //turn on LED as an indicator
  TXPacketL = LT.transmitSXBuffer(0, len, 5000, TrackerTXpower, WAIT_TX);
  digitalWrite(LED1, LOW);                  //turn off indicator LED

  return TXPacketL;                         //TXPacketL will be 0 if there was an error sending
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  //flash LED to show tracker is alive
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void clearAllMemory()
{
  //clears the whole of non-volatile
  Serial.println(F("Clear Memory"));
  fillMemory(addr_StartMemory, addr_EndMemory, 0);
}


void printSupplyVoltage()
{
  //get and display supply volts on terminal or monitor
  Serial.print(F("Volts "));
  Serial.print(readSupplyVoltage());
  Serial.println(F("mV"));
}


uint16_t readSupplyVoltage()
{
  //relies on internal 1v1 reference and 91K & 11K resistor divider
  //returns supply in mV @ 10mV per AD bit read
  uint16_t temp;
  uint16_t volts = 0;
  uint8_t index;

  analogReference(INTERNAL);
  temp = analogRead(SupplyAD);

  for (index = 0; index <= 4; index++)        //sample AD 5 times
  {
    temp = analogRead(SupplyAD);
    volts = volts + temp;
  }
  volts = ((volts / 5) * ADMultiplier);

  return volts;
}


//***********************************************************
// Start GPS Functions
//***********************************************************

void GPSTest()
{
  uint32_t endmS;

  endmS = millis() + 2000;                     //run GPS echo for 2000mS

  while (millis() < endmS)
  {
    while (GPSserial.available() > 0)
      Serial.write(GPSserial.read());
  }
  Serial.println();
  Serial.println();
  Serial.flush();
}


bool gpsWaitFix(uint16_t waitSecs)
{
  //waits a specified number of seconds for a fix, returns true for good fix

  uint32_t endwaitmS, millistowait, currentmillis;
  uint8_t GPSchar;

  Serial.flush();

  Serial.print(F("Wait GPS Fix "));
  Serial.print(waitSecs);
  Serial.print(F("s "));
  Serial.flush();

  GPS_OutputOn();
  Serial.flush();

  currentmillis = millis();
  millistowait = waitSecs * 1000;
  endwaitmS = currentmillis + millistowait;

  while (GPSserial.read() >= 0);                  //clear the GPS serial input buffer

  while (millis() < endwaitmS)
  {

    if (GPSserial.available() > 0)
    {
      GPSchar = GPSserial.read();
      gps.encode(GPSchar);
    }

    if (gps.location.isUpdated() && gps.altitude.isUpdated())
    {
      TXLat = gps.location.lat();
      TXLon = gps.location.lng();
      TXAlt = (uint16_t) gps.altitude.meters();

      //Altitude is used as an unsigned integer, so that the binary payload is as short as possible.
      //However gps.altitude.meters(); can return a negative value which converts to
      //65535 - Altitude, which we dont want. So we will assume any value over 60,000M is zero

      if (TXAlt > 60000)
      {
        TXAlt = 0;
      }

      TXHours = gps.time.hour(),
      TXMinutes = gps.time.minute(),
      TXSeconds = gps.time.second(),
      TXSatellites = gps.satellites.value();

      setStatusByte(GPSFix, 1);

      TXGPSfixms = millis() - GPSstartms;

      Serial.print(F("Have GPS Fix "));
      Serial.print(TXGPSfixms);
      Serial.print(F("mS"));
      Serial.println();

      return true;
    }

  }

  //if here then there has been no fix and a timeout
  GPS_OutputOff();
  setStatusByte(GPSFix, 0);                     //set status bit to flag no fix
  incMemoryUint16(addr_TXErrors);
  Serial.println(F("Error No GPS Fix"));
  return false;
}

//***********************************************************
// End GPS Functions
//***********************************************************

void printTempTC74()
{
  int8_t TC74TXTemperature;
  TC74TXTemperature = readTempTC74(TC74_ADDRESS);
  Serial.print(F("Temperature "));
  Serial.print(TC74TXTemperature);
  Serial.println(F("c"));
}


int8_t readTempTC74(uint8_t addr)
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
    return regdata + TC74_Calibration;
  }
  else
  {
    return 128;
  }
}


void setup()
{
  uint32_t i;
  uint16_t j;

  Serial.begin(115200);                     //Setup Serial console ouput
  Serial.println();
  Serial.println();
  Serial.println(F(__FILE__));

  memoryStart(Memory_Address);              //setup the memory
  j = readMemoryUint16(addr_ResetCount);
  j++;
  writeMemoryUint16(addr_ResetCount, j);
  j = readMemoryUint16(addr_ResetCount);

  Serial.print(F("TXResets "));
  Serial.println(j);

#ifdef QUECTELINUSE
  Serial.println(F("Quectel GPS library"));
#endif

#ifdef UBLOXINUSE
  Serial.println(F("UBLOX GPS library"));
#endif

#ifdef ClearAllMemory
  clearAllMemory();
#endif

  SPI.begin();                              //initialize SPI

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("LoRa Device error"));
    while (1)
    {
      led_Flash(50, 50);                     //long fast speed flash indicates device error
    }
  }

  setTrackerMode();

  Serial.print(F("Config "));
  Serial.println(Default_config1, BIN);

  j = readMemoryUint16(addr_TXErrors);
  Serial.print(F("TXErrors "));
  Serial.println(j);

  Serial.print(F("TXSequence "));
  i = readMemoryUint32(addr_SequenceNum);
  Serial.println(i);

  Serial.print(F("ThisNode "));
  Serial.println(ThisNode);

  LT.printModemSettings();                    //reads and prints the configured LoRa settings, useful check

  Serial.println();
  printSupplyVoltage();
  printTempTC74();
  Serial.println();

  TXStatus = 0;                               //clear all TX status bits

  sendCommand(PowerUp);                       //send power up command, includes supply mV and config, on tracker settings

  GPS_OutputOn();
  GPSTest();
  GPS_Setup();                                //GPS should have had plenty of time to initialise by now

  delay(2000);

  if (GPS_CheckBalloonMode())                 //Check that GPS is configured for high altitude mode
  {
    Serial.println();
    GPS_OutputOff();                          //GPS interrupts cause problems with lora device, so turn off for now
    setStatusByte(GPSError, 0);
    setStatusByte(GPSConfigError, 0);

    //Alert user to GPS OK, turn LED on and send a FM tone.
    digitalWrite(LED1, HIGH);
    Serial.println(F("GPS Config OK"));        //check tone indicates navigation model 6 set
    Serial.println();
    Serial.flush();
    LT.setupDirect(TrackerFrequency, Offset);  //need direct mode for tones
    LT.toneFM(1500, 500, deviation, adjustfreq, TrackerTXpower); //Transmit an FM tone, 1000hz, 3000ms
    delay(1000);
    digitalWrite(LED1, LOW);
  }
  else
  {
    setStatusByte(GPSConfigError, 1);
    incMemoryUint16(addr_TXErrors);
    Serial.println(F("GPS Error"));
    Serial.println();
    setTrackerMode();
    sendCommand(NoGPS);                          //make sure receiver knows about GPS error
    led_Flash(100, 25);                          //long very rapid flash for GPS error
  }

  GPSstartms = millis();

  setTrackerMode();                              //so that commands indicating wait for a GPS go out

  while (!gpsWaitFix(5))                         //wait for the initial GPS fix, this could take a while
  {
    sendCommand(NoFix);
    led_Flash(2, 50);                            //two short LED flashes to indicate GPS waiting for fix
  }

  LT.setupDirect(TrackerFrequency, Offset);      //need direct mode for tones
  digitalWrite(LED1, HIGH);
  LT.toneFM(500, 2000, deviation, adjustfreq, TrackerTXpower);
  digitalWrite(LED1, LOW);
  GPS_OutputOn();
  delay(2000);                                   //GPS may be in software backup allow time for it to wakeup
  GPS_SetCyclicMode();                           //set this regardless of whether hot fix mode is enabled
  GPS_OutputOff();
}
