/*
  Copyright 2021 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 17/12/19
  New version 23/12/20, support for modules no using PA_BOOST added
  New version, 24/08/21, Reliable packets support added
  New version, 19/09/21, Data Transfer and support for modules with no DIO0 pin added
*/

/*
  Parts of code Copyright (c) 2013, SEMTECH S.A.
  See LICENSE.TXT file included in the library
*/

#include <SX127XLT.h>
#include <SPI.h>

#define LTUNUSED(v) (void) (v)       //add LTUNUSED(variable); in functions to avoid compiler warnings 
#define USE_SPI_TRANSACTION          //this is the standard behaviour of library, use SPI Transaction switching

//#define SX127XDEBUG1               //enable level 1 debug messages
//#define SX127XDEBUG2               //enable level 2 debug messages
//#define SX127XDEBUG3               //enable level 3 debug messages
//#define DEBUGPHANTOM               //used to set bebuging for Phantom packets
//#define SX127XDEBUGPINS            //enable pin allocation debug messages
//#define DEBUGFSKRTTY               //enable for FSKRTTY debugging
//#define PACONFIGDEBUG              //enable for debugging the RF port switching
//#define APPLYERRATANOTE_2_3        //if enabled the changes suggested in SX1276_77_8_ErrataNote_1_1 are applied
//#define SX127XDEBUGRELIABLE        //enable for debugging reliable and data transfer (DT) packets


SX127XLT::SX127XLT()
{

}

/* Formats for :begin
  1 original   > begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, int8_t pinDIO1, int8_t pinDIO2, uint8_t device);
  2 Simplified > begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, uint8_t device);
  3 Simplified > begin(int8_t pinNSS, int8_t pinNRESET, uint8_t device);
  4 Bare minimum, no NRESET or DIO0 pins > begin(int8_t pinNSS, uint8_t device);
*/


bool SX127XLT::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, int8_t pinDIO1, int8_t pinDIO2, uint8_t device)
{
  //format 1 pins, assign the all available pins
#ifdef SX127XDEBUG1
  Serial.println(F("1 begin() "));
#endif

  //assign the passed pins to the class private variabled
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _DIO0 = pinDIO0;
  _DIO1 = pinDIO1;
  _DIO2 = pinDIO2;
  _Device = device;            //device type needs to be assigned before reset
  _TXDonePin = pinDIO0;        //this is defalt pin for sensing TX done
  _RXDonePin = pinDIO0;        //this is defalt pin for sensing RX done
  _ReliableConfig = 0;         //reliable config needs to start at 0

  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, LOW);

  if (_DIO0 >= 0)
  {
    pinMode( _DIO0, INPUT);
  }

  if (_DIO1 >= 0)
  {
    pinMode( _DIO1,  INPUT);
  }

  if (_DIO2 >= 0)
  {
    pinMode( _DIO2,  INPUT);
  }

  resetDevice();

#ifdef SX127XDEBUGPINS
  Serial.println(F("1 begin()"));
  Serial.println(F("SX127XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("DIO0 "));
  Serial.println(_DIO0);
  Serial.print(F("DIO1 "));
  Serial.println(_DIO1);
  Serial.print(F("DIO2 "));
  Serial.println(_DIO2);
#endif

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX127XLT::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, uint8_t device)
{
  //format 2 pins, simplified
#ifdef SX127XDEBUG1
  Serial.println(F("2 begin() "));
#endif

  //assign the passed pins to the class private variabled
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _DIO0 = pinDIO0;
  _DIO1 = -1;                  //pin not used
  _DIO2 = -1;                  //pin not used
  _Device = device;            //device type needs to be assigned before reset
  _TXDonePin = pinDIO0;        //this is defalt pin for sensing TX done
  _RXDonePin = pinDIO0;        //this is defalt pin for sensing RX done
  _ReliableConfig = 0;         //reliable config needs to start at 0

  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, HIGH);

  if (_DIO0 >= 0)
  {
    pinMode( _DIO0, INPUT);
  }

  resetDevice();

#ifdef SX127XDEBUGPINS
  Serial.println(F("2 begin() "));
  Serial.println(F("SX127XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("DIO0 "));
  Serial.println(_DIO0);
#endif

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX127XLT::begin(int8_t pinNSS, int8_t pinNRESET, uint8_t device)
{
  //format 3 pins, simplified, no DIO0
#ifdef SX127XDEBUG1
  Serial.println(F("3 begin() "));
#endif

  //assign the passed pins to the class private variabled
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _DIO0 = -1;                  //pin not used
  _DIO1 = -1;                  //pin not used
  _DIO2 = -1;                  //pin not used
  _Device = device;            //device type needs to be assigned before reset
  _TXDonePin = _DIO0;          //this is default pin for sensing TX done
  _RXDonePin = _DIO0;          //this is default pin for sensing RX done
  _ReliableConfig = 0;         //reliable config needs to start at 0

  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, HIGH);

  resetDevice();

#ifdef SX127XDEBUGPINS
  Serial.println(F("3 begin() "));
  Serial.println(F("SX127XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("DIO0 "));
  Serial.println(_DIO0);
#endif

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX127XLT::begin(int8_t pinNSS, uint8_t device)
{
  //format 4 pins, simplified further, no DIO0 or NRESET
#ifdef SX127XDEBUG1
  Serial.println(F("4 begin()"));
#endif

  _NSS = pinNSS;
  _NRESET = -1;                //pin not used
  _DIO0 = -1;                  //pin not used
  _DIO1 = -1;                  //pin not used
  _DIO2 = -1;                  //pin not used
  _Device = device;            //device type needs to be assigned before reset
  _TXDonePin = _DIO0;          //this is default pin for sensing TX done
  _RXDonePin = _DIO0;          //this is default pin for sensing RX done
  _ReliableConfig = 0;         //reliable config needs to start at 0

  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);


#ifdef SX127XDEBUGPINS
  Serial.println(F("4 begin()"));
  Serial.println(F("SX127XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
#endif

  if (checkDevice())
  {
    return true;
  }

  return false;
}


void SX127XLT::resetDevice()
{
#ifdef SX127XDEBUG1
  Serial.println(F("resetDevice() "));
#endif

  if (_NRESET >= 0)
  {
    if (_Device == DEVICE_SX1272)
    {
      digitalWrite(_NRESET, HIGH);
      delay(2);
      digitalWrite(_NRESET, LOW);
      delay(20);
    }
    else
    {
      digitalWrite(_NRESET, LOW);
      delay(2);
      digitalWrite(_NRESET, HIGH);
      delay(20);
    }
  }
}


void SX127XLT::setMode(uint8_t modeconfig)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setMode() "));
#endif

  uint8_t regdata;

  regdata = modeconfig + _PACKET_TYPE;
  writeRegister(REG_OPMODE, regdata);
}


void SX127XLT::setSleep(uint8_t sleepconfig)
{
  //settings passed via sleepconfig are ignored, feature retained for compatibility with SX128x,SX126x

#ifdef SX127XDEBUG1
  Serial.println(F("setSleep() "));
#endif

  LTUNUSED(sleepconfig);

  uint8_t regdata;

  regdata = readRegister(REG_OPMODE);
  writeRegister(REG_OPMODE, (regdata & 0xF8));       //clear bits 0,1,2 to set sleepmode
  delay(1);                                          //allow time for shutdown
}


bool SX127XLT::checkDevice()
{
  //check there is a device out there, writes a register and reads back

#ifdef SX127XDEBUG1
  Serial.println(F("checkDevice() "));
#endif

  uint8_t Regdata1, Regdata2;
  Regdata1 = readRegister(REG_FRMID);               //low byte of frequency setting
  writeRegister(REG_FRMID, (Regdata1 + 1));
  Regdata2 = readRegister(REG_FRMID);               //read changed value back
  writeRegister(REG_FRMID, Regdata1);               //restore register to original value

  if (Regdata2 == (Regdata1 + 1))
  {
    return true;
  }
  else
  {
    return false;
  }
}


void SX127XLT::wake()
{
#ifdef SX127XDEBUG1
  Serial.println(F("wake() "));
#endif
  uint8_t regdata;

  regdata = readRegister(REG_OPMODE);
  writeRegister(REG_OPMODE, (regdata | 0x01));      //set bit 0 to goto stdby mode
}


void SX127XLT::calibrateImage(uint8_t null)
{
#ifdef SX127XDEBUG1
  Serial.println(F("calibrateImage() "));
#endif

  LTUNUSED(null);

  uint8_t regdata, savedmode;

  savedmode = readRegister(REG_OPMODE);
  writeRegister(REG_OPMODE, 0x00);              //sleep
  writeRegister(REG_OPMODE, 0x00);              //sleep
  writeRegister(REG_OPMODE, 0x01);              //standby FSK mode
  regdata = (readRegister(REG_IMAGECAL) | 0x40);
  writeRegister(REG_IMAGECAL, regdata);         //start calibration
  delay(15);                                    //calibration time 10mS
  writeRegister(REG_OPMODE, 0x00);              //sleep
  writeRegister(REG_OPMODE, savedmode & 0xFE);
  writeRegister(REG_OPMODE, savedmode);
}


void SX127XLT::printRegister(uint8_t reg)
{
  uint8_t regdata;

  Serial.print(F("Register 0x"));

  if (reg < 0x10)
  {
    Serial.print(F("0"));
  }

  Serial.print(reg, HEX);
  regdata = readRegister(reg);
  Serial.print(F(" 0x"));

  if (regdata < 0x10)
  {
    Serial.print(F("0"));
  }

  Serial.print(regdata, HEX);
}


uint16_t SX127XLT::CRCCCITT(uint8_t *buffer, uint32_t size, uint16_t startvalue)
{
#ifdef SX127XDEBUG1
  Serial.println(F("CRCCCITT() "));
#endif

  uint16_t index, libraryCRC;
  uint8_t j;

  libraryCRC = startvalue;                                  //start value for CRC16

  for (index = 0; index < size; index++)
  {
    libraryCRC ^= (((uint16_t)buffer[index]) << 8);
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


uint16_t SX127XLT::CRCCCITTSX(uint8_t startadd, uint8_t endadd, uint16_t startvalue)
{
  //generates a CRC of an area of the internal SX buffer
  //note the startReadSXBuffer() call will alter _RXPacketL

#ifdef SX127XDEBUG1
  Serial.println(F("CRCCCITTSX() "));
#endif

  uint16_t index, libraryCRC;
  uint8_t j;

  libraryCRC = startvalue;                                  //start value for CRC16
  startReadSXBuffer(startadd);                              //begin the buffer read

  for (index = startadd; index <= endadd; index++)
  {
    libraryCRC ^= (((uint16_t) readUint8() ) << 8);
    for (j = 0; j < 8; j++)
    {
      if (libraryCRC & 0x8000)
        libraryCRC = (libraryCRC << 1) ^ 0x1021;
      else
        libraryCRC <<= 1;
    }
  }

  endReadSXBuffer();                                         //end the buffer read

  return libraryCRC;
}


void SX127XLT::setDevice(uint8_t type)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setDevice() "));
#endif

  _Device = type;
}


void SX127XLT::printDevice()
{
#ifdef SX127XDEBUG1
  Serial.println(F("printDevice() "));
#endif

  switch (_Device)
  {
    case DEVICE_SX1272_PABOOST:
      Serial.print(F("SX1272_PABOOST"));
      break;

    case DEVICE_SX1276_PABOOST:
      Serial.print(F("SX1276_PABOOST"));
      break;

    case DEVICE_SX1277_PABOOST:
      Serial.print(F("SX1277_PABOOST"));
      break;

    case DEVICE_SX1278_PABOOST:
      Serial.print(F("SX1278_PABOOST"));
      break;

    case DEVICE_SX1279_PABOOST:
      Serial.print(F("SX1279_PABOOST"));
      break;

    case DEVICE_SX1276_RFO:
      Serial.print(F("SX1276_RFO"));
      break;

    case DEVICE_SX1277_RFO:
      Serial.print(F("SX1277_RFO"));
      break;

    case DEVICE_SX1278_RFO:
      Serial.print(F("SX1278_RFO"));
      break;

    case DEVICE_SX1279_RFO:
      Serial.print(F("SX1279_RFO"));
      break;

    default:
      Serial.print(F("Unknown Device"));
  }
}


uint8_t SX127XLT::getOperatingMode()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getOperatingMode() "));
#endif

  return readRegister(REG_OPMODE);
}


bool SX127XLT::isReceiveDone()
{
#ifdef SX127XDEBUG1
  Serial.println(F("isReceiveDone()"));
#endif

  return digitalRead(_RXDonePin);
}


bool SX127XLT::isTransmitDone()
{
#ifdef SX127XDEBUG1
  Serial.println(F("isTransmitDone() "));
#endif

  return digitalRead(_TXDonePin);
}


void SX127XLT::writeRegister(uint8_t address, uint8_t value)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeRegister() "));
#endif

#ifdef USE_SPI_TRANSACTION                  //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                  //set NSS low
  SPI.transfer(address | 0x80);             //mask address for write
  SPI.transfer(value);                      //write the byte
  digitalWrite(_NSS, HIGH);                 //set NSS high

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

#ifdef SX127XDEBUG2
  Serial.print(F("Write register "));
  printHEXByte0x(address);
  Serial.print(F(" "));
  printHEXByte0x(value);
  Serial.println();
  Serial.flush();
#endif
}


uint8_t SX127XLT::readRegister(uint8_t address)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readRegister() "));
#endif

  uint8_t regdata;

#ifdef USE_SPI_TRANSACTION         //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);         //set NSS low
  SPI.transfer(address & 0x7F);    //mask address for read
  regdata = SPI.transfer(0);       //read the byte
  digitalWrite(_NSS, HIGH);        //set NSS high

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

#ifdef SX127XDEBUG2
  Serial.print(F("Read register "));
  printHEXByte0x(address);
  Serial.print(F(" "));
  printHEXByte0x(regdata);
  Serial.println();
  Serial.flush();
#endif

  return regdata;
}


void SX127XLT::printRegisters(uint16_t Start, uint16_t End)
{
  //prints the contents of SX127x registers to serial monitor

#ifdef SX127XDEBUG
  Serial.println(F("printRegisters() "));
#endif

  uint16_t Loopv1, Loopv2, RegData;

  Serial.print(F("Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Serial.println();

  for (Loopv1 = Start; Loopv1 <= End;)           //32 lines
  {
    Serial.print(F("0x"));
    if (Loopv1 < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print((Loopv1), HEX);                 //print the register number
    Serial.print(F("  "));
    for (Loopv2 = 0; Loopv2 <= 15; Loopv2++)
    {
      RegData = readRegister(Loopv1);
      if (RegData < 0x10)
      {
        Serial.print(F("0"));
      }
      Serial.print(RegData, HEX);                //print the register number
      Serial.print(F(" "));
      Loopv1++;
    }
    Serial.println();
  }
}


void SX127XLT::printOperatingMode()
{
#ifdef SX127XDEBUG1
  Serial.println(F("printOperatingMode() "));
#endif

  uint8_t regdata;

  regdata = getOpmode();

  switch (regdata)
  {
    case 0:
      Serial.print(F("SLEEP"));
      break;

    case 1:
      Serial.print(F("STDBY"));
      break;

    case 2:
      Serial.print(F("FSTX"));
      break;

    case 3:
      Serial.print(F("TX"));
      break;

    case 4:
      Serial.print(F("FSRX"));
      break;

    case 5:
      Serial.print(F("RXCONTINUOUS"));
      break;

    case 6:
      Serial.print(F("RXSINGLE"));
      break;

    case 7:
      Serial.print(F("CAD"));
      break;

    default:
      Serial.print(F("NOIDEA"));
      break;
  }
}


void SX127XLT::printOperatingSettings()
{
#ifdef SX127XDEBUG1
  Serial.println(F("printOperatingSettings() "));
#endif

  printDevice();
  Serial.print(F(","));
  printOperatingMode();
  Serial.print(F(",Version_"));
  Serial.print(getVersion(), HEX);
  Serial.print(F(",PacketMode_"));

  if (getPacketMode())
  {
    Serial.print(F("LoRa"));
  }
  else
  {
    Serial.print(F("FSK"));
  }

  if (getHeaderMode())
  {
    Serial.print(F(",Implicit"));
  }
  else
  {
    Serial.print(F(",Explicit"));
  }

  Serial.print(F(",CRC_"));
  if (getCRCMode())
  {
    Serial.print(F("On"));
  }
  else
  {
    Serial.print(F("Off"));
  }

  Serial.print(F(",AGCauto_"));
  if (getAGC())
  {
    Serial.print(F("On"));
  }
  else
  {
    Serial.print(F("Off"));
  }

  Serial.print(F(",LNAgain_"));
  Serial.print(getLNAgain());

  if (!bitRead(_Device, 4))               //if bit 4 of _Device is 0 then either RFO LF_ANT or HF_ANT pin used for RF output
  {
    Serial.print(F(",LNAboostHF_"));
    if (getLNAboostHF())
    {
      Serial.print(F("On"));
    }
    else
    {
      Serial.print(F("Off"));
    }

    Serial.print(F(",LNAboostLF_"));
    if (getLNAboostLF())
    {
      Serial.print(F("Reserved"));
    }
    else
    {
      Serial.print(F("Default"));
    }
  }
}


void SX127XLT::setTxParams(int8_t txPower, uint8_t rampTime)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setTxParams() "));
#endif

  uint8_t MaxPower, OutputPower, OcpTrim, boostval;

  if (_Device & 0x10)
  {
    //start setTxParams() for PABOOST

#ifdef  PACONFIGDEBUG
    Serial.print(F(" PABOOST Device "));
#endif

    boostval = PABOOSTON;
    MaxPower = MAXPOWER17dBm;

    if (txPower > 20)                      //upper power limit for SX127X
    {
      txPower = 20;
    }

    if (txPower < 2)
    {
      txPower = 2;
    }

    if (txPower > 17)
    {
      writeRegister(REG_PADAC, 0x87);      //Reg 0x4D this is same for SX1272 and SX1278
      OutputPower = txPower - 5;           //power range is 15dBm, max is now 20dBm min is 5dBm
    }
    else
    {
      writeRegister(REG_PADAC, 0x84);       //Reg 0x4D this is same for SX1272 and SX1278
      OutputPower = txPower - 2;            //power range is 15dBm, max is 17dBm min is 2dBm
    }

    if (_Device == DEVICE_SX1272_PABOOST)   //power calculation for SX1272 is the same for < 17dbm and > 17dBm
    {
      OutputPower = txPower - 2;
    }

    //end setTxParams() for PABOOST
  }
  else
  {
    //start setTxParams() for RFO

#ifdef  PACONFIGDEBUG
    Serial.print(F(" RFO Device "));
#endif

    boostval = PABOOSTOFF;
    MaxPower = MAXPOWER14dBm;

    if (txPower > 14)                          //14dBm is upper power limit for RFO
    {
      txPower = 14;
    }

    if (txPower < 0)
    {
      txPower = 0;
    }

    OutputPower = txPower + 1;                 //power range is 15dBm, max is 14dBm min is 0dBm
    writeRegister(REG_PADAC, 0x84);            //must turn off high power setting for RFO mode

    //end setTxParams() for RFO
  }

  //common routines for PABOOST and RFO
  OcpTrim = OCP_TRIM_110MA;                    //value for OcpTrim 11dBm to 16dBm

  if (txPower >= 17)
  {
    OcpTrim = OCP_TRIM_150MA;
  }

  if (txPower <= 10)
  {
    OcpTrim = OCP_TRIM_80MA;
  }

  writeRegister(REG_PARAMP, rampTime);
  writeRegister(REG_OCP, (OcpTrim + 0x20));
  writeRegister(REG_PACONFIG, (boostval  + MaxPower + OutputPower));    //MaxPower does not care for SX1272

#ifdef  PACONFIGDEBUG
  Serial.print(F("txPower,"));
  Serial.print(txPower);
  Serial.print(F(",REG_PACONFIG,"));
  Serial.print(readRegister(REG_PACONFIG), HEX);
  Serial.print(F(",PABOOST,"));

  if (readRegister(REG_PACONFIG) & 0x80)
  {
    Serial.print(F("ON,"));
  }
  else
  {
    Serial.print(F("OFF,"));
  }

  Serial.print(F("MaxPower,"));
  Serial.print(MaxPower, HEX);
  Serial.print(F(",OutputPower,"));
  Serial.print(OutputPower, HEX);
  Serial.print(F(","));
  printOCPTRIM();
#endif
}


void SX127XLT::printOCPTRIM()
{
#ifdef SX127XDEBUG1
  Serial.println(F("printOCPTRIM() "));
#endif

  uint8_t regdata;

  regdata = readRegister(REG_OCP);
  Serial.print(F("REG_OCP,"));
  Serial.print(regdata, HEX);
  Serial.print(F(","));

  if (!(regdata & 0x20))
  {
    Serial.print(F("OCP_TRIM_OFF "));
    return;
  }

  //OCP must be on, so print trim value

  regdata = regdata & 0x1F;                //mask off trim value

  switch (regdata)
  {
    case OCP_TRIM_45MA:
      Serial.print(F("OCP_TRIM_45MA "));
      break;

    case OCP_TRIM_80MA:
      Serial.print(F("OCP_TRIM_80MA "));
      break;

    case OCP_TRIM_100MA:
      Serial.print(F("OCP_TRIM_100MA "));
      break;

    case OCP_TRIM_110MA:
      Serial.print(F("OCP_TRIM_110MA "));
      break;

    case OCP_TRIM_120MA:
      Serial.print(F("OCP_TRIM_120MA "));
      break;

    case OCP_TRIM_130MA:
      Serial.print(F("OCP_TRIM_130MA "));
      break;

    case OCP_TRIM_140MA:
      Serial.print(F("OCP_TRIM_140MA "));
      break;

    case OCP_TRIM_150MA:
      Serial.print(F("OCP_TRIM_150MA "));
      break;
  }

  return;
}


void SX127XLT::setPacketParams(uint16_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5)
{
  //format is PreambleLength, Fixed\Variable length packets, Packetlength, CRC mode, IQ mode

#ifdef SX127XDEBUG1
  Serial.println(F("SetPacketParams() "));
#endif

  uint8_t preambleMSB, preambleLSB, regdata;


  //*******************************************************
  //These changes are the same for SX1272 and SX127X
  //PreambleLength reg 0x20, 0x21

  preambleMSB = packetParam1 >> 8;
  preambleLSB = packetParam1 & 0xFF;
  writeRegister(REG_PREAMBLEMSB, preambleMSB);
  writeRegister(REG_PREAMBLELSB, preambleLSB);

  //TX Packetlength reg 0x22
  writeRegister(REG_PAYLOADLENGTH, packetParam3);                //when in implicit mode, this is used as receive length also

  //IQ mode reg 0x33 and 0x3B
  if (packetParam5 == LORA_IQ_INVERTED)
  {
    writeRegister(REG_INVERTIQ,  0x66);
    writeRegister(REG_INVERTIQ2, 0x19);
  }

  if (packetParam5 == LORA_IQ_NORMAL)
  {
    writeRegister(REG_INVERTIQ,  0x27);
    writeRegister(REG_INVERTIQ2, 0x1d);
  }

  //regdata = ( (readRegister(REG_INVERTIQ)) & 0xBE );           //mask off invertIQ bit 6 and bit 0
  //writeRegister(REG_INVERTIQ, (regdata + packetParam5));
  //*******************************************************


  //CRC mode
  _UseCRC = packetParam4;                                         //save CRC use status

  if (_Device != DEVICE_SX1272)
  {
    //for all devices apart from SX1272
    //Fixed\Variable length packets
    regdata = ( (readRegister(REG_MODEMCONFIG1)) & (~READ_IMPLCIT_AND_X)); //mask off bit 0
    writeRegister(REG_MODEMCONFIG1, (regdata + packetParam2));             //write out with bit 0 set appropriatly

    //CRC on payload
    regdata = ( (readRegister(REG_MODEMCONFIG2)) & (~READ_HASCRC_AND_X));  //mask off all bits bar CRC on - bit 2
    writeRegister(REG_MODEMCONFIG2, (regdata + (packetParam4 << 2)));      //write out with CRC bit 2 set appropriatly
  }
  else
  {
    //for SX1272
    //Fixed\Variable length packets
    regdata = ( (readRegister(REG_MODEMCONFIG1)) & (~READ_IMPLCIT_AND_2)); //mask off bit 2
    writeRegister(REG_MODEMCONFIG1, (regdata + (packetParam2 << 2)));      //write out with bit 2 set appropriatly

    //CRC on payload
    regdata = ( (readRegister(REG_MODEMCONFIG1)) & (~READ_HASCRC_AND_2));  //mask of all bits bar CRC on - bit 1
    writeRegister(REG_MODEMCONFIG1, (regdata + (packetParam4 << 1)));      //write out with CRC bit 1 set appropriatly
  }
}


void SX127XLT::setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint8_t  modParam4)
{
  //order is SpreadingFactor, Bandwidth, CodeRate, Optimisation

#ifdef SX127XDEBUG1
  Serial.println(F("setModulationParams() "));
#endif

  uint8_t regdata, bw;

  //Spreading factor - same for SX1272 and SX127X - reg 0x1D
  regdata = (readRegister(REG_MODEMCONFIG2) & (~READ_SF_AND_X));
  writeRegister(REG_MODEMCONFIG2, (regdata + (modParam1 << 4)));

  if (_Device != DEVICE_SX1272)
  {
    //for all devices apart from SX1272

    //bandwidth
    regdata = (readRegister(REG_MODEMCONFIG1) & (~READ_BW_AND_X));
    writeRegister(REG_MODEMCONFIG1, (regdata + modParam2));

    //Coding rate
    regdata = (readRegister(REG_MODEMCONFIG1) & (~READ_CR_AND_X));
    writeRegister(REG_MODEMCONFIG1, (regdata + (modParam3)));

    //Optimisation
    if (modParam4 == LDRO_AUTO)
    {
      modParam4 = returnOptimisation(modParam2, modParam1);
    }

    regdata = (readRegister(REG_MODEMCONFIG3) & (~READ_LDRO_AND_X));
    writeRegister(REG_MODEMCONFIG3, (regdata + (modParam4 << 3)));

  }
  else
  {
    //for SX1272
    regdata = (readRegister(REG_MODEMCONFIG1) & (~READ_BW_AND_2));   //value will be LORA_BW_500 128, LORA_BW_250 64, LORA_BW_125 0

    switch (modParam2)
    {
      case LORA_BW_125:
        bw = 0x00;
        break;

      case LORA_BW_500:
        bw = 0x80;
        break;

      case LORA_BW_250:
        bw = 0x40;
        break;

      default:
        bw = 0x00;                       //defaults to LORA_BW_125
    }

    writeRegister(REG_MODEMCONFIG1, (regdata + bw));

    //Coding rate
    regdata = (readRegister(REG_MODEMCONFIG1) & (~READ_CR_AND_2));
    writeRegister(REG_MODEMCONFIG1, (regdata + (modParam3 << 2)));

    //Optimisation
    if (modParam4 == LDRO_AUTO)
    {
      modParam4 = returnOptimisation(modParam2, modParam1);
    }

    regdata = (readRegister(REG_MODEMCONFIG1) & (~READ_LDRO_AND_2));
    writeRegister(REG_MODEMCONFIG1, (regdata + modParam4));

  }

#ifdef APPLYERRATANOTE_2_3
  //optimisations called by SX1276_77_8_ErrataNote_1_1

  //ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal
  if ( modParam2 < LORA_BW_500 )
  {
    writeRegister( REG_DETECTOPTIMIZE, readRegister(REG_DETECTOPTIMIZE) & 0x7F );
    writeRegister( REG_LRTEST30, 0x00 );

    switch (modParam2)
    {
      case LORA_BW_007: // 7.8 kHz
        writeRegister(REG_LRTEST2F, 0x48);
        _savedOffset = _savedOffset + 7800;
        break;
      case LORA_BW_010: // 10.4 kHz
        writeRegister(REG_LRTEST2F, 0x44);
        _savedOffset = _savedOffset + 10400;
        break;
      case LORA_BW_015: // 15.6 kHz
        writeRegister(REG_LRTEST2F, 0x44);
        _savedOffset = _savedOffset + 15600;
        break;
      case LORA_BW_020: // 20.8 kHz
        writeRegister(REG_LRTEST2F, 0x44);
        _savedOffset = _savedOffset + 20800;
        break;
      case LORA_BW_031: // 31.2 kHz
        writeRegister(REG_LRTEST2F, 0x44);
        _savedOffset = _savedOffset + 31200;
        break;
      case LORA_BW_041: // 41.4 kHz
        writeRegister(REG_LRTEST2F, 0x44 );
        _savedOffset = _savedOffset + 41400;
        break;
      case LORA_BW_062: // 62.5 kHz
        writeRegister(REG_LRTEST2F, 0x40);
        break;
      case LORA_BW_125: // 125 kHz
        writeRegister(REG_LRTEST2F, 0x40);
        break;
      case LORA_BW_250: // 250 kHz
        writeRegister(REG_LRTEST2F, 0x40);
        break;
    }
  }
  else
  {
    writeRegister(REG_DETECTOPTIMIZE, readRegister(REG_DETECTOPTIMIZE ) | 0x80 );
  }


  setRfFrequency(_savedFrequency, _savedOffset);                     //apply the updated frequency
#endif

  //ERRATA 2.1 - SX1276_77_8_ErrataNote_1_1
  if ( ( modParam2 == LORA_BW_500 ) && ( _savedFrequency >= 862000000 ) )
  {
    //ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth >= 862Mhz
    writeRegister(REG_HIGHBWOPTIMIZE1, 0x02);
    writeRegister(REG_HIGHBWOPTIMIZE2, 0x64);
  }
  else if ( modParam2 == LORA_BW_500 )
  {
    //ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth 410Mhz to 525Mhz
    writeRegister(REG_HIGHBWOPTIMIZE1, 0x02);
    writeRegister(REG_HIGHBWOPTIMIZE2, 0x7F);
  }
  else
  {
    // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
    writeRegister(REG_HIGHBWOPTIMIZE1, 0x03);
  }

  //Datasheet SX1276-7-8-9_May_2020, page 115, REG_DETECTOPTIMIZE = 0x31, REG_LRDETECTIONTHRESHOLD = 0x37
  if ( modParam1 == LORA_SF6 )
  {
    writeRegister(REG_DETECTOPTIMIZE, ( readRegister( REG_DETECTOPTIMIZE ) & 0xF8 ) | 0x05 );
    writeRegister(REG_DETECTIONTHRESHOLD, 0x0C);
  }
  else
  {
    writeRegister( REG_DETECTOPTIMIZE, ( readRegister( REG_DETECTOPTIMIZE ) & 0xF8 ) | 0x03 );
    writeRegister( REG_DETECTIONTHRESHOLD, 0x0A );
  }
}


void SX127XLT::setRfFrequency(uint64_t freq64, int32_t offset)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setRfFrequency() "));
#endif

  _savedFrequency = freq64;
  _savedOffset = offset;
  freq64 = freq64 + offset;
  freq64 = ((uint64_t)freq64 << 19) / 32000000;
  _freqregH = freq64 >> 16;
  _freqregM = freq64 >> 8;
  _freqregL = freq64;
  writeRegister(REG_FRMSB, _freqregH);
  writeRegister(REG_FRMID, _freqregM);
  writeRegister(REG_FRLSB, _freqregL);
}


uint32_t SX127XLT::getFreqInt()
{
  //get the current set LoRa device frequency, return as long integer
#ifdef SX127XDEBUG1
  Serial.println(F("getFreqInt() "));
#endif

  uint8_t Msb, Mid, Lsb;
  uint32_t uinttemp;
  float floattemp;

  Msb = readRegister(REG_FRMSB);
  Mid = readRegister(REG_FRMID);
  Lsb = readRegister(REG_FRLSB);
  floattemp = ((Msb * 0x10000ul) + (Mid * 0x100ul) + Lsb);
  floattemp = ((floattemp * 61.03515625) / 1000000ul);
  uinttemp = (uint32_t)(floattemp * 1000000);
  return uinttemp;
}


int32_t SX127XLT::getFrequencyErrorRegValue()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getFrequencyErrorRegValue() "));
#endif

  int32_t FrequencyError;
  uint32_t msb, mid, lsb;
  uint32_t allreg;

  setMode(MODE_STDBY_RC);
  msb = readRegister(REG_FEIMSB);
  mid = readRegister(REG_FEIMID);
  lsb = readRegister(REG_FEILSB);

#ifdef SX127XDEBUG1
  Serial.println();
  Serial.print(F("Registers "));
  Serial.print(msb, HEX);
  Serial.print(F(" "));
  Serial.print(mid, HEX);
  Serial.print(F(" "));
  Serial.println(lsb, HEX);
#endif

  allreg = (uint32_t) ( msb << 16 ) | ( mid << 8 ) | lsb;

  if (allreg & 0x80000)
  {
    FrequencyError = (0xFFFFF - allreg) * -1;
  }
  else
  {
    FrequencyError = allreg;
  }

  return FrequencyError;
}


int32_t SX127XLT::getFrequencyErrorHz()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getFrequencyErrorHz() "));
#endif

  uint16_t msb, mid, lsb;
  int16_t freqerr;
  uint8_t bw;
  float bwconst;

  msb = readRegister(REG_FEIMSB);
  mid = readRegister(REG_FEIMID);
  lsb = readRegister(REG_FEILSB);

  if (_Device != DEVICE_SX1272)
  {
    //for all devices apart from SX1272
    bw = (readRegister(REG_MODEMCONFIG1)) & (0xF0);

    switch (bw)
    {
      case LORA_BW_125:           //ordered with most commonly used first
        bwconst = 2.097;
        break;

      case LORA_BW_062:
        bwconst = 1.049;
        break;

      case LORA_BW_041:
        bwconst = 0.6996;
        break;

      case LORA_BW_007:
        bwconst = 0.1309;
        break;

      case LORA_BW_010:
        bwconst = 0.1745;
        break;

      case LORA_BW_015:
        bwconst = 0.2617;
        break;

      case LORA_BW_020:
        bwconst = 0.3490;
        break;

      case LORA_BW_031:
        bwconst = 0.5234;
        break;

      case LORA_BW_250:
        bwconst = 4.194;
        break;

      case LORA_BW_500:
        bwconst = 8.389;
        break;

      default:
        bwconst = 0x00;
    }
  }
  else
  {
    //for the SX1272
    bw = (readRegister(REG_MODEMCONFIG1)) & (0xF0);

    switch (bw)
    {
      case 0:                   //this is LORA_BW_125
        bwconst = 2.097;
        break;

      case 64:                  //this is LORA_BW_250
        bwconst = 4.194;
        break;

      case 128:                 //this is LORA_BW_250
        bwconst = 8.389;
        break;

      default:
        bwconst = 0x00;
    }
  }

  freqerr = msb << 12;          //shift lower 4 bits of msb into high 4 bits of freqerr
  mid = (mid << 8) + lsb;
  mid = (mid >> 4);
  freqerr = freqerr + mid;
  freqerr = (int16_t) (freqerr * bwconst);

  return freqerr;
}


void SX127XLT::setTx(uint32_t timeout)
{
  //There is no TX timeout function for SX127X, the value passed is ignored

#ifdef SX127XDEBUG1
  Serial.println(F("setTx() "));
#endif

  LTUNUSED(timeout);                                  //unused TX timeout passed for compatibility with SX126x, SX128x

  clearIrqStatus(IRQ_RADIO_ALL);

  /* This function not used on current SX127x
    if (_rxtxpinmode)
    {
    rxEnable();
    }
  */

  writeRegister(REG_OPMODE, (MODE_TX + 0x88));       //TX on LoRa mode
}


void SX127XLT::setRx(uint32_t timeout)
{
  //no timeout in this routine, left in for compatibility
#ifdef SX127XDEBUG1
  Serial.println(F("setRx()"));
#endif

  LTUNUSED(timeout);

  clearIrqStatus(IRQ_RADIO_ALL);

  /* This function not used on current SX127x
    if (_rxtxpinmode)
    {
    rxEnable();
    }
  */

  writeRegister(REG_OPMODE, (MODE_RXCONTINUOUS + 0x80));    //RX on LoRa mode
}


bool SX127XLT::readTXIRQ()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readTXIRQ()"));
#endif

  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_TX_DONE)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SX127XLT::readRXIRQ()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readRXIRQ() "));
#endif

  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_RX_DONE)
  {
    return true;
  }
  else
  {
    return false;
  }
}


void SX127XLT::setLowPowerReceive()
{
  //set min LNA gain, AGC off
#ifdef SX127XDEBUG1
  Serial.println(F("setLowPowerReceive() "));
#endif
  uint8_t regdata;

  regdata = readRegister(REG_MODEMCONFIG3);
  bitClear(regdata, 2);                      //set bit 2 to 0 turns off AGC
  writeRegister(REG_MODEMCONFIG3, regdata ); //write data back

  writeRegister(REG_LNA, 0xC0 );             //Minimum gain for PA_BOOST and default for RFO_HF default LNA current
}


void SX127XLT::setHighSensitivity()
{
  //set max LNA gain and Boosted LNA for HF mode

#ifdef SX127XDEBUG1
  Serial.println(F("setHighSensitivity() "));
#endif

  writeRegister(REG_LNA, 0x23 ); //MAX gain for PA_BOOST and for RFO_HF set 150% LNA current.
}


void SX127XLT::setRXGain(uint8_t config)
{
  //set RX power saving mode

#ifdef SX127XDEBUG1
  Serial.println(F("setRXGain() "));
#endif

  uint8_t regdata;

  if (_Device != DEVICE_SX1272)
  {
    //for all devices apart from SX1272
    regdata = readRegister(REG_MODEMCONFIG3);
    writeRegister(REG_MODEMCONFIG3, (regdata &  (~READ_AGCAUTO_AND_X))); //clear bit AgcAutoOn (2) to ensure RegLNA is controlling gain
    writeRegister(REG_LNA, config);
  }
  else
  {
    regdata = readRegister(REG_MODEMCONFIG2);
    writeRegister(REG_MODEMCONFIG2, (regdata & (~READ_AGCAUTO_AND_2))); //clear bit AgcAutoOn (2) to ensure RegLNA is controlling gain
    writeRegister(REG_LNA, config);
  }
}


uint8_t SX127XLT::getAGC()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getAGC() "));
#endif

  uint8_t regdata;

  if (_Device != DEVICE_SX1272)
  {
    regdata = readRegister(REG_MODEMCONFIG3);
    regdata = (regdata & READ_AGCAUTO_AND_X);
  }
  else
  {
    regdata = readRegister(REG_MODEMCONFIG2);
    regdata = (regdata & READ_AGCAUTO_AND_2);
  }

  return (regdata >> 2);
}


uint8_t SX127XLT::getLNAgain()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getLNAgain() "));
#endif

  uint8_t regdata;

  regdata = readRegister(REG_LNA);
  regdata = (regdata & READ_LNAGAIN_AND_X);
  return (regdata >> 5);
}


uint8_t SX127XLT::getCRCMode()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getCRCMode() "));
#endif

  uint8_t regdata;

  regdata = readRegister(REG_MODEMCONFIG2);

  if (_Device != DEVICE_SX1272)
  {
    regdata = readRegister(REG_MODEMCONFIG2);
    regdata = ((regdata & READ_HASCRC_AND_X) >> 2);
  }
  else
  {
    regdata = readRegister(REG_MODEMCONFIG1);
    regdata = ((regdata & READ_HASCRC_AND_2) >> 1);
  }

  return regdata;
}


uint8_t SX127XLT::getHeaderMode()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getHeaderMode() "));
#endif

  uint8_t regdata;

  regdata = readRegister(REG_MODEMCONFIG1);

  if (_Device != DEVICE_SX1272)
  {
    regdata = (regdata & READ_IMPLCIT_AND_X);
  }
  else
  {
    regdata = ((regdata & READ_IMPLCIT_AND_2) >> 2);
  }

  return regdata;
}


uint8_t SX127XLT::getLNAboostLF()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getLNAboostLFLF() "));
#endif

  uint8_t regdata;

  regdata = readRegister(REG_LNA);

  if (_Device != DEVICE_SX1272)
  {
    regdata = (regdata & READ_LNABOOSTLF_AND_X);
  }
  else
  {
    regdata = (regdata & READ_LNABOOSTLF_AND_2);
  }

  return (regdata >> 3);
}


uint8_t SX127XLT::getLNAboostHF()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getLNAboostHF() "));
#endif

  uint8_t regdata;

  regdata = readRegister(REG_LNA);

  if (_Device != DEVICE_SX1272)
  {
    regdata = (regdata & READ_LNABOOSTHF_AND_X);
  }
  else
  {
    regdata = (regdata & READ_LNABOOSTHF_AND_2);
  }

  return regdata;
}


uint8_t SX127XLT::getOpmode()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getOpmode()"));
#endif

  uint8_t regdata;

  regdata = (readRegister(REG_OPMODE) & READ_OPMODE_AND_X);

  return regdata;
}


uint8_t SX127XLT::getPacketMode()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getPacketMode() "));
#endif

  uint8_t regdata;

  regdata = (readRegister(REG_OPMODE) & READ_RANGEMODE_AND_X);

  return (regdata >> 7);
}


uint8_t SX127XLT::readRXPacketL()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readRXPacketL() "));
#endif

  _RXPacketL = readRegister(REG_RXNBBYTES);
  return _RXPacketL;
}



uint8_t SX127XLT::readTXPacketL()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readTXPacketL() "));
#endif

  return _TXPacketL;
}


int16_t SX127XLT::readPacketRSSI()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readPacketRSSI() "));
#endif

  //actual read SNR register values normally seen in practice are from 0 to 48 (0dB to +12dB)
  //and 167 to 255 (-22dB to 0dB)

  int16_t _PacketRSSI;                                        //RSSI of received packet
  int8_t SNRregdata;

  if (_savedFrequency < 779000000)                            //779Mhz is lower frequency limit for SX1279 on band1 (HF Port)
  {
    _PacketRSSI = -164 + readRegister(REG_PKTRSSIVALUE);
  }
  else
  {
    _PacketRSSI = -157 + readRegister(REG_PKTRSSIVALUE);
  }

  SNRregdata = readRegister(REG_PKTSNRVALUE);

  if (SNRregdata < 0)                                          //check for negative SNR value
  {
    _PacketRSSI = (_PacketRSSI + (SNRregdata >> 2));           //add datasheet fiddle factor
  }

  return _PacketRSSI;
}


int16_t SX127XLT::readCurrentRSSI()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readCurrentRSSI() "));
#endif

  return readRegister(REG_CURRENTRSSIVALUE);
}


int8_t SX127XLT::readPacketSNR()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readPacketSNR() "));
#endif

  uint8_t regdata;
  int8_t  _PacketSNR;

  regdata = readRegister(REG_PKTSNRVALUE);

  if (regdata > 127)
  {
    _PacketSNR  =  ((255 - regdata) / 4) * (-1);
  }
  else
  {
    _PacketSNR  = regdata / 4;
  }

  return _PacketSNR;
}


bool SX127XLT::readPacketCRCError()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readPacketCRCError() "));
#endif

  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_CRC_ERROR)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SX127XLT::readPacketHeaderValid()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readPacketHeaderValid() "));
#endif

  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_HEADER_VALID)
  {
    return true;
  }
  else
  {
    return false;
  }
}


uint8_t SX127XLT::packetOK()
{
#ifdef SX127XDEBUG1
  Serial.println(F("packetOK() "));
#endif

  bool packetHasCRC;

  packetHasCRC = (readRegister(REG_HOPCHANNEL) & 0x40);                  //read the packet has CRC bit in RegHopChannel

#ifdef DEBUGPHANTOM
  Serial.print(F("PacketHasCRC = "));
  Serial.println(packetHasCRC);
  Serial.print(F("_UseCRC = "));
  Serial.println(_UseCRC);
#endif

  if ( !packetHasCRC && _UseCRC )
  {
    _IRQmsb = _IRQmsb + IRQ_NO_PACKET_CRC;                              //flag the phantom packet
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                           //no RX done and header valid only, could be CRC error
  }

  return 1;                                                             //packet is OK
}


uint8_t SX127XLT::readRXPacketType()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readRXPacketType() "));
#endif

  return _RXPacketType;
}


uint8_t SX127XLT::readRXDestination()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readRXDestination() "));
#endif
  return _RXDestination;
}


uint8_t SX127XLT::readRXSource()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readRXSource() "));
#endif

  return _RXSource;
}


void SX127XLT::setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setBufferBaseAddress() "));
#endif

  writeRegister(REG_FIFOTXBASEADDR, txBaseAddress);
  writeRegister(REG_FIFORXBASEADDR, rxBaseAddress);
}


void SX127XLT::setPacketType(uint8_t packettype )
{
#ifdef SX127XDEBUG1
  Serial.println(F("setPacketType() "));
#endif
  uint8_t regdata;

  regdata = (readRegister(REG_OPMODE) & 0x7F);           //save all register bits bar the LoRa\FSK bit 7

  if (packettype == PACKET_TYPE_LORA)
  {
    _PACKET_TYPE = PACKET_TYPE_LORA;
    writeRegister(REG_OPMODE, 0x80);                     //REG_OPMODE, need to set to sleep mode before configure for LoRa mode
    writeRegister(REG_OPMODE, (regdata + 0x80));         //back to original standby mode with LoRa set
  }
  else
  {
    _PACKET_TYPE = PACKET_TYPE_GFSK;
    writeRegister(REG_OPMODE, 0x00);                     //REG_OPMODE, need to set to sleep mode before configure for FSK
    writeRegister(REG_OPMODE, regdata);                  //back to original standby mode with FSK set
  }
}


void SX127XLT::clearIrqStatus(uint16_t irqMask)
{
#ifdef SX127XDEBUG1
  Serial.println(F("clearIrqStatus() "));
#endif

  uint8_t masklsb;
  uint16_t maskmsb;
  _IRQmsb = _IRQmsb & 0xFF00;                                 //make sure _IRQmsb does not have LSB bits set.
  masklsb = (irqMask & 0xFF);
  maskmsb = (irqMask & 0xFF00);
  writeRegister(REG_IRQFLAGS, masklsb);                       //clear standard IRQs
  _IRQmsb = (_IRQmsb & (~maskmsb));                           //only want top bits set.
}


uint16_t SX127XLT::readIrqStatus()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readIrqStatus()"));
#endif

  bool packetHasCRC;
  uint8_t regdata;
  regdata = readRegister(REG_IRQFLAGS);

  packetHasCRC = (readRegister(REG_HOPCHANNEL) & 0x40);        //read the packet has CRC bit in RegHopChannel


#ifdef DEBUGPHANTOM
  Serial.print(F("PacketHasCRC = "));
  Serial.println(packetHasCRC);
  Serial.print(F("_UseCRC = "));
  Serial.println(_UseCRC);
#endif

  if (bitRead(regdata, 6))                                     //check if its a packet receive (IRQ_RX_DONE set)
  {
    if (!packetHasCRC && _UseCRC)                              //check if packet header indicates no CRC on packet, byt use CRC set
    {
      bitSet(_IRQmsb, 10);                                     //flag the phantom packet, set bit 10
    }
  }
  return (regdata + _IRQmsb);
}


void SX127XLT::setDioIrqParams(uint16_t irqMask, uint16_t dio0Mask, uint16_t dio1Mask, uint16_t dio2Mask)
{
  //note the irqmask contains the bit values of the interrupts that are allowed, so for all interrupts value is 0xFFFF

#ifdef SX127XDEBUG1
  Serial.println(F("setDioIrqParams() "));
#endif

  uint8_t mask0, mask1, mask2;

  LTUNUSED(dio2Mask);                      //variable left in call for compatibility with other libraries

  switch (dio0Mask)
  {
    case IRQ_RX_DONE:
      mask0 = 0;
      break;

    case IRQ_TX_DONE:
      mask0 = 0x40;
      break;

    case IRQ_CAD_DONE:
      mask0 = 0x80;
      break;

    default:
      mask0 = 0x0C;
  }

  switch (dio1Mask)                        //for compatibility with other libraries IRQ_RX_TIMEOUT = IRQ_RX_TX_TIMEOUT
  {
    case IRQ_RX_TIMEOUT:
      mask1 = 0;
      break;

    case IRQ_FSHS_CHANGE_CHANNEL:
      mask1 = 0x10;
      break;

    case IRQ_CAD_ACTIVITY_DETECTED:
      mask1 = 0x20;
      break;

    default:
      mask1 = 0x30;
  }

  mask2 = 0x00;                             //is always IRQ_FSHS_CHANGE_CHANNEL

  writeRegister(REG_IRQFLAGSMASK, ~irqMask);
  writeRegister(REG_DIOMAPPING1, (mask0 + mask1 + mask2));
}


void SX127XLT::printIrqStatus()
{
#ifdef SX127XDEBUG1
  Serial.println(F("printIrqStatus() "));
#endif

  uint8_t _IrqStatus;
  _IrqStatus = readIrqStatus();

  //0x01
  if (_IrqStatus & IRQ_CAD_ACTIVITY_DETECTED)
  {
    Serial.print(F(",IRQ_CAD_ACTIVITY_DETECTED"));
  }

  //0x02
  if (_IrqStatus & IRQ_FSHS_CHANGE_CHANNEL)
  {
    Serial.print(F(",IRQ_FSHS_CHANGE_CHANNEL"));
  }

  //0x04
  if (_IrqStatus & IRQ_CAD_DONE)
  {
    Serial.print(F(",IRQ_CAD_DONE"));
  }

  //0x08
  if (_IrqStatus & IRQ_TX_DONE)
  {
    Serial.print(F(",IRQ_TX_DONE"));
  }

  //0x10
  if (_IrqStatus & IRQ_HEADER_VALID)
  {
    Serial.print(F(",IRQ_HEADER_VALID"));
  }

  //0x20
  if (_IrqStatus & IRQ_CRC_ERROR)
  {
    Serial.print(F(",IRQ_CRC_ERROR"));
  }

  //0x40
  if (_IrqStatus & IRQ_RX_DONE)
  {
    Serial.print(F(",IRQ_RX_DONE"));
  }

  //0x80
  if (_IrqStatus & IRQ_RX_TIMEOUT )
  {
    Serial.print(F(",IRQ_RX_TIMEOUT "));
  }

  if (_IRQmsb & IRQ_TX_TIMEOUT  )
  {
    Serial.print(F(",TX_TIMEOUT"));
  }

  if (_IRQmsb & IRQ_RX_TIMEOUT  )
  {
    Serial.print(F(",RX_TIMEOUT"));
  }

  if (_IRQmsb & IRQ_NO_PACKET_CRC  )
  {
    Serial.print(F(",NO_PACKET_CRC"));
  }
}


void SX127XLT::printASCIIPacket(uint8_t *buffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("printASCIIPacket() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    Serial.write(buffer[index]);
  }
}


void SX127XLT::printHEXPacket(uint8_t *buffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("printHEXPacket() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByte(buffer[index]);
    Serial.print(F(" "));
  }
}


void SX127XLT::printASCIIorHEX(uint8_t temp)
{
  //prints as ASCII if within range, otherwise as HEX.
#ifdef SX127XDEBUG1
  Serial.println(F("printASCIIorHEX() "));
#endif

  if ((temp < 0x10) || (temp > 0x7E))
  {
    Serial.print(F(" ("));
    printHEXByte(temp);
    Serial.print(F(") "));
  }
  else
  {
    Serial.write(temp);
  }
}


void SX127XLT::printHEXByte(uint8_t temp)
{
#ifdef SX127XDEBUG1
  Serial.println(F("printHEXByte() "));
#endif

  if (temp < 0x10)
  {
    Serial.print(F("0"));
  }
  Serial.print(temp, HEX);
}


void SX127XLT::printHEXByte0x(uint8_t temp)
{
  //print a byte, adding 0x

#ifdef SX127XDEBUG1
  Serial.println(F("printHEXByte0x()"));
#endif

  Serial.print(F("0x"));
  if (temp < 0x10)
  {
    Serial.print(F("0"));
  }
  Serial.print(temp, HEX);
}


bool SX127XLT::isRXdone()
{
#ifdef SX127XDEBUG1
  Serial.println(F("isRXdone() "));
#endif

  return digitalRead(_DIO0);
}

bool SX127XLT::isTXdone()
{
#ifdef SX127XDEBUG1
  Serial.println(F("isTXdone()"));
#endif

  return digitalRead(_DIO0);
}


bool SX127XLT::isRXdoneIRQ()
{
#ifdef SX127XDEBUG1
  Serial.println(F("isRXdoneIRQ() "));
#endif

  return (readRegister(REG_IRQFLAGS) & IRQ_RX_DONE);
}


bool SX127XLT::isTXdoneIRQ()
{
#ifdef SX127XDEBUG1
  Serial.println(F("isTXdoneIRQ() "));
#endif

  return (readRegister(REG_IRQFLAGS) & IRQ_TX_DONE);
}


void SX127XLT::setTXDonePin(uint8_t pin)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setTXDonePin() "));
#endif

  _TXDonePin = pin;
}


void SX127XLT:: setRXDonePin(uint8_t pin)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setRXDonePin() "));
#endif

  _RXDonePin = pin;
}


uint8_t SX127XLT::receive(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUG1
  Serial.println(F("receive()"));
#endif

  uint16_t index;
  uint32_t startmS;
  uint8_t regdata;

  setMode(MODE_STDBY_RC);
  regdata = readRegister(REG_FIFORXBASEADDR);                              //retrieve the RXbase address pointer
  writeRegister(REG_FIFOADDRPTR, regdata);                                 //and save in FIFO access ptr

  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE), 0, 0);                     //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    //change to allow for millis() rollover
    //code was  endtimeoutmS = millis() + rxtimeout; while (!digitalRead(_RXDonePin) && (millis() < endtimeoutmS));
    startmS = millis();
    while (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if DIO still low, if so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }


  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL > size)                      //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                        //truncate packet if not enough space in passed buffer
  }

#ifdef USE_SPI_TRANSACTION   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < _RXPacketL; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


uint8_t SX127XLT::receiveIRQ(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUG1
  Serial.println(F("receive()"));
#endif

  uint16_t index;
  uint32_t startmS;
  uint8_t regdata;

  setMode(MODE_STDBY_RC);
  regdata = readRegister(REG_FIFORXBASEADDR);                              //retrieve the RXbase address pointer
  writeRegister(REG_FIFOADDRPTR, regdata);                                 //and save in FIFO access ptr

  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE), 0, 0);                     //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!isRXdoneIRQ());                                                //Wait for IRQ to go high, no timeout, RX DONE
  }
  else
  {
    //change to allow for millis() rollover
    //code was  endtimeoutmS = millis() + rxtimeout; while (!digitalRead(_RXDonePin) && (millis() < endtimeoutmS));
    startmS = millis();
    while (!isRXdoneIRQ() && ((uint32_t) (millis() - startmS) < rxtimeout));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!isRXdoneIRQ())                                                      //check if IRQ still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }


  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL > size)                      //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                        //truncate packet if not enough space in passed buffer
  }

#ifdef USE_SPI_TRANSACTION   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < _RXPacketL; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


uint8_t SX127XLT::receiveAddressed(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait)
{
#ifdef SX127XDEBUG1
  Serial.println(F("receiveAddressed() "));
#endif

  uint16_t index;
  uint32_t startmS;
  uint8_t regdata;

  setMode(MODE_STDBY_RC);
  regdata = readRegister(REG_FIFORXBASEADDR);                                //retrieve the RXbase address pointer
  writeRegister(REG_FIFOADDRPTR, regdata);                                   //and save in FIFO access ptr
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_HEADER_VALID), 0, 0);    //set for IRQ on RX done
  setRx(0);                                                                  //no actual RX timeout in this function

  if (!wait)
  {
    return 0;
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if not DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                 //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL > size)                      //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                        //truncate packet if not enough space in passed buffer
  }

#ifdef USE_SPI_TRANSACTION   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);

  _RXPacketType = SPI.transfer(0);
  _RXDestination = SPI.transfer(0);
  _RXSource = SPI.transfer(0);

  for (index = 0; index < _RXPacketL; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


uint8_t SX127XLT::readPacket(uint8_t *rxbuffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readPacket() "));
#endif

  uint8_t index, regdata;

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                   //no RX done and header valid only, could be CRC error
  }

  setMode(MODE_STDBY_RC);
  regdata = readRegister(REG_FIFORXBASEADDR);   //retrieve the RXbase address pointer
  writeRegister(REG_FIFOADDRPTR, regdata);      //and save in FIFO access ptr
  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL > size)                        //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                          //truncate packet if not enough space
  }

#ifdef USE_SPI_TRANSACTION   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                      //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < _RXPacketL; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


uint8_t SX127XLT::readPacketAddressed(uint8_t *rxbuffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readPacketAddressed() "));
#endif

  uint8_t index, regdata;

  setMode(MODE_STDBY_RC);
  regdata = readRegister(REG_FIFORXBASEADDR);  //retrieve the RXbase address pointer
  writeRegister(REG_FIFOADDRPTR, regdata);     //and save in FIFO access ptr
  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL > size)                       //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                         //truncate packet if not enough space
  }

#ifdef USE_SPI_TRANSACTION   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                     //start the burst read
  SPI.transfer(REG_FIFO);
  _RXPacketType = SPI.transfer(0);
  _RXDestination = SPI.transfer(0);
  _RXSource = SPI.transfer(0);

  for (index = 0; index < _RXPacketL; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


uint8_t SX127XLT::transmit(uint8_t *txbuffer, uint8_t size, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUG1
  Serial.println(F("transmit()"));
#endif

  uint8_t index, ptr;
  uint8_t bufferdata;
  uint32_t startmS;

  if (size == 0)
  {
    return false;
  }

  setMode(MODE_STDBY_RC);
  ptr = readRegister(REG_FIFOTXBASEADDR);       //retrieve the TXbase address pointer
  writeRegister(REG_FIFOADDRPTR, ptr);          //and save in FIFO access ptr

#ifdef USE_SPI_TRANSACTION                      //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < size; index++)
  {
    bufferdata = txbuffer[index];
    SPI.transfer(bufferdata);
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  _TXPacketL = size;
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                  //Wait for pin to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;                                   //no timeout, so TXdone must have been set
}


uint8_t SX127XLT::transmitIRQ(uint8_t *txbuffer, uint8_t size, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUG1
  Serial.println(F("transmit()"));
#endif

  uint8_t index, ptr;
  uint8_t bufferdata;
  uint32_t startmS;

  if (size == 0)
  {
    return false;
  }

  setMode(MODE_STDBY_RC);
  ptr = readRegister(REG_FIFOTXBASEADDR);       //retrieve the TXbase address pointer
  writeRegister(REG_FIFOADDRPTR, ptr);          //and save in FIFO access ptr

#ifdef USE_SPI_TRANSACTION                   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < size; index++)
  {
    bufferdata = txbuffer[index];
    SPI.transfer(bufferdata);
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  _TXPacketL = size;
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!isTXdoneIRQ());                            //Wait for IRQ to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!isTXdoneIRQ() && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!isTXdoneIRQ())
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;                                   //no timeout, so TXdone must have been set
}


uint8_t SX127XLT::transmitAddressed(uint8_t *txbuffer, uint8_t size, char txpackettype, char txdestination, char txsource, uint32_t txtimeout, int8_t txpower, uint8_t wait )
{
#ifdef SX127XDEBUG1
  Serial.println(F("transmitAddressed() "));
#endif

  uint8_t index, ptr;
  uint8_t bufferdata;
  uint32_t startmS;

  if (size == 0)
  {
    return false;
  }

  setMode(MODE_STDBY_RC);
  ptr = readRegister(REG_FIFOTXBASEADDR);         //retrieve the TXbase address pointer
  writeRegister(REG_FIFOADDRPTR, ptr);            //and save in FIFO access ptr

#ifdef USE_SPI_TRANSACTION                        //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  SPI.transfer(txpackettype);                     //Write the packet type
  SPI.transfer(txdestination);                    //Destination node
  SPI.transfer(txsource);                         //Source node
  _TXPacketL = 3 + size;                          //we have added 3 header bytes to size

  for (index = 0; index < size; index++)
  {
    bufferdata = txbuffer[index];
    SPI.transfer(bufferdata);
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                   //Wait for DIO0 to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                               //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))                         //its a timeout if _TXDonepin still high
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;                                     //no timeout, so TXdone must have been set
}


void SX127XLT::setupLoRa(uint32_t Frequency, int32_t Offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint8_t modParam4)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setupLoRa() "));
#endif

  setMode(MODE_STDBY_RC);                            //go into standby mode to configure device
  setPacketType(PACKET_TYPE_LORA);                   //use LoRa packets
  setRfFrequency(Frequency, Offset);                 //set the operating frequncy
  calibrateImage(0);                                 //run calibration after setting frequency
  setModulationParams(modParam1, modParam2, modParam3, modParam4);
  setBufferBaseAddress(0x00, 0x00);                  //set bases addresses for TX and RX packets in SX buffer
  setPacketParams(8, LORA_PACKET_VARIABLE_LENGTH, 255, LORA_CRC_ON, LORA_IQ_NORMAL); //set the packet options
  setSyncWord(LORA_MAC_PRIVATE_SYNCWORD);            //syncword to use, 0x12 for standard private, 0x34 for public (LORAWAN)
  setHighSensitivity();
}


void SX127XLT::setupLoRa(uint32_t Frequency)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setupLoRa() "));
#endif

  setMode(MODE_STDBY_RC);                            //go into standby mode to configure device
  setPacketType(PACKET_TYPE_LORA);                   //use LoRa packets
  setRfFrequency(Frequency, 0);                      //set the operating frequncy
  calibrateImage(0);                                 //run calibration after setting frequency
  setModulationParams(LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO);
  setBufferBaseAddress(0x00, 0x00);                  //set bases addresses for TX and RX packets in SX buffer
  setPacketParams(8, LORA_PACKET_VARIABLE_LENGTH, 255, LORA_CRC_ON, LORA_IQ_NORMAL); //set the packet options
  setSyncWord(LORA_MAC_PRIVATE_SYNCWORD);            //syncword to use, 0x12 for standard private, 0x34 for public (LORAWAN)
  setHighSensitivity();
}


uint8_t SX127XLT::getLoRaSF()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getLoRaSF() "));
#endif

  uint8_t regdata;
  regdata = readRegister(REG_MODEMCONFIG2);
  regdata = ((regdata & READ_SF_AND_X) >> 4);        //SX1272 and SX1276 store SF in same place

  return regdata;
}


uint8_t SX127XLT::getLoRaCodingRate()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getLoRaCodingRate() "));
#endif

  uint8_t regdata;
  regdata = readRegister(REG_MODEMCONFIG1);

  if (_Device != DEVICE_SX1272)
  {
    //for all devices apart from SX1272
    regdata = (((regdata & READ_CR_AND_X) >> 1) + 4);
  }
  else
  {
    regdata = (((regdata & READ_CR_AND_2) >> 3) + 4);
  }

  return regdata;
}


uint8_t SX127XLT::getOptimisation()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getOptimisation() "));
#endif

  uint8_t regdata;

  if (_Device != DEVICE_SX1272)
  {
    //for all devices apart from SX1272
    regdata = readRegister(REG_MODEMCONFIG3);
    regdata = ((regdata & READ_LDRO_AND_X) >> 3);
  }
  else
  {
    regdata = readRegister(REG_MODEMCONFIG1);
    regdata = (regdata & READ_LDRO_AND_2);
  }

  return regdata;
}


uint8_t SX127XLT::getSyncWord()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getSyncWord() "));
#endif

  return readRegister(REG_SYNCWORD);
}


uint8_t SX127XLT::getInvertIQ()
{
  //IQ mode reg 0x33
#ifdef SX127XDEBUG1
  Serial.println(F("getInvertIQ() "));
#endif

  uint8_t regdata;
  regdata =  ( (readRegister(REG_INVERTIQ)) & 0x40 );
  return regdata;
}


uint8_t SX127XLT::getVersion()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getVersion() "));
#endif

  return readRegister(REG_VERSION);
}


uint16_t SX127XLT::getPreamble()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getPreamble() "));
#endif

  uint16_t regdata;
  regdata =  ( (readRegister(REG_PREAMBLEMSB) << 8) + readRegister(REG_PREAMBLELSB) );
  return regdata;
}


uint32_t SX127XLT::returnBandwidth(byte BWregvalue)
{
#ifdef SX127XDEBUG1
  Serial.println(F("returnBandwidth() "));
#endif

  if (_Device == DEVICE_SX1272)
  {
    switch (BWregvalue)
    {
      case 0:
        return 125000;

      case 64:
        return 250000;

      case 128:
        return 500000;

      default:
        return 0xFF;                      //so that a bandwidth invalid entry can be identified ?
    }
  }
  else
  {

    switch (BWregvalue)
    {
      case 0:
        return 7800;

      case 16:
        return 10400;

      case 32:
        return 15600;

      case 48:
        return 20800;

      case 64:
        return 31200;

      case 80:
        return 41700;

      case 96:
        return 62500;

      case 112:
        return 125000;

      case 128:
        return 250000;

      case 144:
        return 500000;

      default:
        return 0xFF;                      //so that a bandwidth invalid entry can be identified ?
    }
  }
}


uint8_t SX127XLT::returnOptimisation(uint8_t Bandwidth, uint8_t SpreadingFactor)
{
  //from the passed bandwidth (bandwidth) and spreading factor this routine
  //calculates whether low data rate optimisation should be on or off

#ifdef SX127XDEBUG1
  Serial.println(F("returnOptimisation() "));
#endif

  uint32_t tempBandwidth;
  float symbolTime;

  tempBandwidth = returnBandwidth(Bandwidth);
  symbolTime = calcSymbolTime(tempBandwidth, SpreadingFactor);

#ifdef SX127XDEBUG1
  Serial.print(F("Symbol Time "));
  Serial.print(symbolTime, 3);
  Serial.println(F("mS"));
#endif

  if (symbolTime > 16)
  {
#ifdef SX127XDEBUG1
    Serial.println(F("LDR Opt on"));
#endif
    return LDRO_ON;
  }
  else
  {
#ifdef SX127XDEBUG1
    Serial.println(F("LDR Opt off"));
#endif
    return LDRO_OFF;
  }
}


float SX127XLT::calcSymbolTime(float Bandwidth, uint8_t SpreadingFactor)
{
  //calculates symbol time from passed bandwidth (lbandwidth) and Spreading factor (lSF)and returns in mS

#ifdef SX127XDEBUG1
  Serial.println(F("calcSymbolTime() "));
#endif

  float symbolTimemS;
  symbolTimemS = (Bandwidth / pow(2, SpreadingFactor));
  symbolTimemS = (1000 / symbolTimemS);
  return symbolTimemS;
}


void SX127XLT::printModemSettings()
{
#ifdef SX127XDEBUG1
  Serial.println(F("printModemSettings()"));
#endif

  uint8_t regdata;

  printDevice();
  Serial.print(F(","));
  Serial.print(getFreqInt());
  Serial.print(F("hz,SF"));
  Serial.print(getLoRaSF());
  Serial.print(F(",BW"));


  if (_Device == DEVICE_SX1272)
  {
    regdata = (readRegister(REG_MODEMCONFIG1) & READ_BW_AND_2);
  }
  else
  {
    regdata = (readRegister(REG_MODEMCONFIG1) & READ_BW_AND_X);
  }

  Serial.print(returnBandwidth(regdata));
  Serial.print(F(",CR4:"));
  Serial.print(getLoRaCodingRate());
  Serial.print(F(",LDRO_"));

  if (getOptimisation())
  {
    Serial.print(F("On"));
  }
  else
  {
    Serial.print(F("Off"));
  }

  Serial.print(F(",SyncWord_0x"));
  Serial.print(getSyncWord(), HEX);
  if (getInvertIQ() == LORA_IQ_INVERTED)
  {
    Serial.print(F(",IQInverted"));
  }
  else
  {
    Serial.print(F(",IQNormal"));
  }
  Serial.print(F(",Preamble_"));
  Serial.print(getPreamble());
}


void SX127XLT::setSyncWord(uint8_t syncword)
{
#ifdef SX127XDEBUG1
  Serial.println(F("setSyncWord() "));
#endif

  writeRegister(REG_SYNCWORD, syncword);
}


uint8_t SX127XLT::receiveSXBuffer(uint8_t startaddr, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUG1
  Serial.println(F("receiveSXBuffer()"));
#endif

  uint32_t startmS;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFORXBASEADDR, startaddr);                             //set start address of RX packet in buffer
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_HEADER_VALID), 0, 0);   //set for IRQ on RX done
  setRx(0);                                                                 //no actual RX timeout in this function

  if (!wait)
  {
    return 0;
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                       //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout));
  }

  setMode(MODE_STDBY_RC);                                                   //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                             //check if not DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                  //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  return _RXPacketL;
}


uint8_t SX127XLT::receiveSXBufferIRQ(uint8_t startaddr, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUG1
  Serial.println(F("receiveSXBuffer()"));
#endif

  uint32_t startmS;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFORXBASEADDR, startaddr);                             //set start address of RX packet in buffer
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_HEADER_VALID), 0, 0);   //set for IRQ on RX done
  setRx(0);                                                                 //no actual RX timeout in this function

  if (!wait)
  {
    return 0;
  }

  if (rxtimeout == 0)
  {
    while (!isRXdoneIRQ());                                                 //Wait for RXIRQ to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while (!isRXdoneIRQ() && ((uint32_t) (millis() - startmS) < rxtimeout));
  }

  setMode(MODE_STDBY_RC);                                                   //ensure to stop further packet reception

  if (!isRXdoneIRQ())                                                       //check if RXIRQ still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                  //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  return _RXPacketL;
}


uint8_t SX127XLT::transmitSXBuffer(uint8_t startaddr, uint8_t length, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUG1
  Serial.println(F("transmitSXBuffer() "));
#endif

  uint32_t startmS;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOTXBASEADDR, startaddr);         //set start address of packet in buffer
  writeRegister(REG_PAYLOADLENGTH, length);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);             //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);    //set for IRQ on TX done
  setTx(0);                                             //TX timeout is not handled in setTX()

  if (!wait)
  {
    return length;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                   //Wait for DIO0 to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                               //ensure we leave function with TX off


  if (!digitalRead(_TXDonePin))                         //if _TXDonePin still high then TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;

    return 0;
  }

  return length;                                         //no timeout, so TXdone must have been set
}


uint8_t SX127XLT::transmitSXBufferIRQ(uint8_t startaddr, uint8_t length, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUG1
  Serial.println(F("transmitSXBuffer() "));
#endif

  uint32_t startmS;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOTXBASEADDR, startaddr);         //set start address of packet in buffer
  writeRegister(REG_PAYLOADLENGTH, length);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);             //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);    //set for IRQ on TX done
  setTx(0);                                             //TX timeout is not handled in setTX()

  if (!wait)
  {
    return length;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                   //Wait for DIO0 to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!isTXdoneIRQ() && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                               //ensure we leave function with TX off

  if (!isTXdoneIRQ())                                   //if _TXDonePin still high then TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;

    return 0;
  }

  return length;                                         //no timeout, so TXdone must have been set
}


void SX127XLT::printSXBufferHEX(uint8_t start, uint8_t end)
{
#ifdef SX127XDEBUG1
  Serial.println(F("printSXBufferHEX() "));
#endif

  uint8_t index, regdata;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOADDRPTR, start);         //set FIFO access ptr to start

#ifdef USE_SPI_TRANSACTION                       //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                       //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = start; index <= end; index++)
  {
    regdata = SPI.transfer(0);
    printHEXByte(regdata);
    Serial.print(F(" "));

  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

}


void SX127XLT::printSXBufferASCII(uint8_t start, uint8_t end)
{
#ifdef SX127XDEBUG1
  Serial.println(F("printSXBufferASCII() "));
#endif

  uint8_t index, regdata;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOADDRPTR, start);      //and save in FIFO access ptr

#ifdef USE_SPI_TRANSACTION                    //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = start; index <= end; index++)
  {
    regdata = SPI.transfer(0);
    Serial.write(regdata);
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


void SX127XLT::fillSXBuffer(uint8_t startaddress, uint8_t size, uint8_t character)
{
#ifdef SX127XDEBUG1
  Serial.println(F("fillSXBuffer() "));
#endif
  uint8_t index;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOADDRPTR, startaddress);     //and save in FIFO access ptr

#ifdef USE_SPI_TRANSACTION                          //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                          //start the burst write
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < size; index++)
  {
    SPI.transfer(character);
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


uint8_t SX127XLT::getByteSXBuffer(uint8_t addr)
{
#ifdef SX127XDEBUG1
  Serial.println(F("getByteSXBuffer() "));
#endif

  uint8_t regdata;
  setMode(MODE_STDBY_RC);                     //this is needed to ensure we can read from buffer OK.

  writeRegister(REG_FIFOADDRPTR, addr);       //set FIFO access ptr to location

#ifdef USE_SPI_TRANSACTION                    //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);
  regdata = SPI.transfer(0);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return regdata;
}


void SX127XLT::writeByteSXBuffer(uint8_t addr, uint8_t regdata)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeByteSXBuffer() "));
#endif

  setMode(MODE_STDBY_RC);                 //this is needed to ensure we can write to buffer OK.
  writeRegister(REG_FIFOADDRPTR, addr);   //set FIFO access ptr to location

#ifdef USE_SPI_TRANSACTION                //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                //start the burst read
  SPI.transfer(WREG_FIFO);
  SPI.transfer(regdata);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


void SX127XLT::startWriteSXBuffer(uint8_t ptr)
{
#ifdef SX127XDEBUG1
  Serial.println(F("startWriteSXBuffer() "));
#endif

  setMode(MODE_STDBY_RC);
  _TXPacketL = 0;                               //this variable used to keep track of bytes written
  writeRegister(REG_FIFOADDRPTR, ptr);          //set buffer access ptr

#ifdef USE_SPI_TRANSACTION                      //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);
}


uint8_t SX127XLT::endWriteSXBuffer()
{
#ifdef SX127XDEBUG1
  Serial.println(F("endWriteSXBuffer() "));
#endif

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _TXPacketL;
}


void SX127XLT::startReadSXBuffer(uint8_t ptr)
{
#ifdef SX127XDEBUG1
  Serial.println(F("startReadSXBuffer() "));
#endif

  setMode(MODE_STDBY_RC);
  _RXPacketL = 0;
  writeRegister(REG_FIFOADDRPTR, ptr);           //set buffer access ptr

#ifdef USE_SPI_TRANSACTION                       //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                       //start the burst read
  SPI.transfer(REG_FIFO);

  //next line would be data = SPI.transfer(0);
  //SPI interface ready for byte to read from
}


uint8_t SX127XLT::endReadSXBuffer()
{
#ifdef SX127XDEBUG1
  Serial.println(F("endReadSXBuffer() "));
#endif

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


void SX127XLT::writeUint8(uint8_t x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeUint8() "));
#endif

  SPI.transfer(x);
  _TXPacketL++;                      //increment count of bytes written
}


uint8_t SX127XLT::readUint8()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readUint8() "));
#endif

  uint8_t x;

  x = SPI.transfer(0);
  _RXPacketL++;                       //increment count of bytes read

  return (x);
}


void SX127XLT::writeInt8(int8_t x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeInt8() "));
#endif

  SPI.transfer(x);
  _TXPacketL++;                       //increment count of bytes written
}


int8_t SX127XLT::readInt8()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readInt8() "));
#endif

  int8_t x;

  x = SPI.transfer(0);
  _RXPacketL++;                      //increment count of bytes read

  return (x);
}


void SX127XLT::writeChar(char x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeChar() "));
#endif

  SPI.transfer(x);
  _TXPacketL++;                     //increment count of bytes written
}


char SX127XLT::readChar()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readChar() "));
#endif

  char x;

  x = SPI.transfer(0);
  _RXPacketL++;                      //increment count of bytes read

  return (x);
}


void SX127XLT::writeUint16(uint16_t x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeUint16() "));
#endif

  SPI.transfer(lowByte(x));
  SPI.transfer(highByte(x));
  _TXPacketL = _TXPacketL + 2;       //increment count of bytes written
}


uint16_t SX127XLT::readUint16()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readUint16() "));
#endif

  uint8_t lowbyte, highbyte;

  lowbyte = SPI.transfer(0);
  highbyte = SPI.transfer(0);
  _RXPacketL = _RXPacketL + 2;        //increment count of bytes read

  return ((highbyte << 8) + lowbyte);
}


void SX127XLT::writeInt16(int16_t x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeInt16() "));
#endif

  SPI.transfer(lowByte(x));
  SPI.transfer(highByte(x));
  _TXPacketL = _TXPacketL + 2;         //increment count of bytes written
}


int16_t SX127XLT::readInt16()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readInt16() "));
#endif

  uint8_t lowbyte, highbyte;

  lowbyte = SPI.transfer(0);
  highbyte = SPI.transfer(0);
  _RXPacketL = _RXPacketL + 2;         //increment count of bytes read

  return ((highbyte << 8) + lowbyte);
}


void SX127XLT::writeUint32(uint32_t x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeUint32() "));
#endif

  uint8_t i, j;

  union
  {
    uint8_t b[4];
    uint32_t f;
  } data;
  data.f = x;

  for (i = 0; i < 4; i++)
  {
    j = data.b[i];
    SPI.transfer(j);
  }

  _TXPacketL = _TXPacketL + 4;         //increment count of bytes written
}


uint32_t SX127XLT::readUint32()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readUint32() "));
#endif

  uint8_t i, j;

  union
  {
    uint8_t b[4];
    uint32_t f;
  } readdata;

  for (i = 0; i < 4; i++)
  {
    j = SPI.transfer(0);
    readdata.b[i] = j;
  }
  _RXPacketL = _RXPacketL + 4;         //increment count of bytes read
  return readdata.f;
}


void SX127XLT::writeInt32(int32_t x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeInt32() "));
#endif

  uint8_t i, j;

  union
  {
    uint8_t b[4];
    int32_t f;
  } data;
  data.f = x;

  for (i = 0; i < 4; i++)
  {
    j = data.b[i];
    SPI.transfer(j);
  }

  _TXPacketL = _TXPacketL + 4;         //increment count of bytes written
}


int32_t SX127XLT::readInt32()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readInt32() "));
#endif

  uint8_t i, j;

  union
  {
    uint8_t b[4];
    int32_t f;
  } readdata;

  for (i = 0; i < 4; i++)
  {
    j = SPI.transfer(0);
    readdata.b[i] = j;
  }
  _RXPacketL = _RXPacketL + 4;         //increment count of bytes read
  return readdata.f;
}


void SX127XLT::writeFloat(float x)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeFloat() "));
#endif

  uint8_t i, j;

  union
  {
    uint8_t b[4];
    float f;
  } data;
  data.f = x;

  for (i = 0; i < 4; i++)
  {
    j = data.b[i];
    SPI.transfer(j);
  }

  _TXPacketL = _TXPacketL + 4;         //increment count of bytes written
}


float SX127XLT::readFloat()
{
#ifdef SX127XDEBUG1
  Serial.println(F("readFloat() "));
#endif

  uint8_t i, j;

  union
  {
    uint8_t b[4];
    float f;
  } readdata;

  for (i = 0; i < 4; i++)
  {
    j = SPI.transfer(0);
    readdata.b[i] = j;
  }
  _RXPacketL = _RXPacketL + 4;         //increment count of bytes read
  return readdata.f;
}


void SX127XLT::writeBuffer(uint8_t *txbuffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeBuffer() "));
#endif

  uint8_t index, regdata;

  _TXPacketL = _TXPacketL + size;      //these are the number of bytes that will be added

  size--;                              //loose one byte from size, the last byte written MUST be a 0

  for (index = 0; index < size; index++)
  {
    regdata = txbuffer[index];
    SPI.transfer(regdata);
  }

  SPI.transfer(0);                     //this ensures last byte of buffer writen really is a null (0)
}


void SX127XLT::writeBufferChar(char *txbuffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeBufferChar() "));
#endif

  uint8_t index, regdata;

  _TXPacketL = _TXPacketL + size;      //these are the number of bytes that will be added

  size--;                              //loose one byte from size, the last byte written MUST be a 0

  for (index = 0; index < size; index++)
  {
    regdata = txbuffer[index];
    SPI.transfer(regdata);
  }

  SPI.transfer(0);                     //this ensures last byte of buffer writen really is a null (0)
}


void SX127XLT::writeBufferChar(char *txbuffer)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeBufferChar() "));
#endif

  uint8_t index = 0, regdata;

  do
  {
    regdata = txbuffer[index];           //read data from txbuffer
    SPI.transfer(regdata);               //write to device buffer
    index++;
  } while (regdata != 0);                //keep reading until we have reached the null (0) at the buffer end or exceeded size of buffer allowed

  _TXPacketL = _TXPacketL + index;       //increment count of bytes written

  SPI.transfer(0);                       //this ensures last byte of buffer writen really is a null (0)
}


uint8_t SX127XLT::readBuffer(uint8_t *rxbuffer)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readBuffer("));
#endif

  uint8_t index = 0, regdata;

  do
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;           //fill the rxbuffer.
    index++;
  } while (regdata != 0);                //keep reading until we have reached the null (0) at the buffer end or exceeded size of buffer allowed

  _RXPacketL = _RXPacketL + index;       //increment count of bytes read

  return index;                          //return the actual size of the buffer, when the null (0) detected
}


uint8_t SX127XLT::readBuffer(uint8_t *rxbuffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readBuffer() "));
#endif

  uint8_t index, regdata;

  for (index = 0; index < size; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;            //fill the rxbuffer.
  }

  _RXPacketL = _RXPacketL + size;        //increment count of bytes read

  return size;                           //return the actual size of the buffer
}


uint8_t SX127XLT::readBufferChar(char *rxbuffer)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readBufferChar() "));
#endif

  uint8_t index = 0, regdata;

  do
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;           //fill the buffer.
    index++;
  } while (regdata != 0);                //keep reading until we have reached the null (0) at the buffer end

  _RXPacketL = _RXPacketL + index;       //increment count of bytes read

  return index;                          //return the actual size of the buffer, when the null (0) detected
}


void SX127XLT::rxtxInit(int8_t pinRXEN, int8_t pinTXEN)
{
  //not used on current SX127x modules
#ifdef SX127XDEBUG1
  Serial.println(F("rxtxInit() "));
#endif

  _RXEN = pinRXEN;
  _TXEN = pinTXEN;

  pinMode(pinRXEN, OUTPUT);
  digitalWrite(pinRXEN, LOW);           //pins needed for RX\TX switching
  pinMode(pinTXEN, OUTPUT);
  digitalWrite(pinTXEN, LOW);           //pins needed for RX\TX switching
}


void SX127XLT::rxEnable()
{
  //not used on current SX127x modules
#ifdef SX127XDEBUG1
  Serial.println(F("rxEnable() "));
#endif

  digitalWrite(_RXEN, HIGH);
  digitalWrite(_TXEN, LOW);
}


void SX127XLT::txEnable()
{
  //not used on current SX127x modules
#ifdef SX127XDEBUG1
  Serial.println(F("txEnable() "));
#endif

  digitalWrite(_RXEN, LOW);
  digitalWrite(_TXEN, HIGH);
}


void SX127XLT::setTXDirect()
{
  //turns on transmitter, in direct mode for FSK and audio  power level is from 2 to 17
#ifdef SX127XDEBUG1
  Serial.println(F("setTXDirect()"));
#endif
  writeRegister(REG_OPMODE, 0x0B);           //TX on direct mode, low frequency mode
}


void SX127XLT::toneFM(uint16_t frequency, uint32_t length, uint32_t deviation, float adjust, int8_t txpower)
{
#ifdef SX127XDEBUG1
  Serial.println(F("toneFM() "));
#endif

  uint16_t index;
  uint32_t ToneDelayus;
  uint32_t registershift;
  uint32_t freqreg;
  uint32_t shiftedfreqregH, shiftedfreqregL;
  uint32_t loopcount;
  uint8_t freqregH, freqregM, freqregL;

#ifdef USE_SPI_TRANSACTION           //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);           //set NSS low
  SPI.transfer(REG_FRMSB & 0x7F);    //mask address for read
  freqregH = SPI.transfer(0);
  freqregM = SPI.transfer(0);
  freqregL = SPI.transfer(0);
  digitalWrite(_NSS, HIGH);          //set NSS high

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  freqreg = ( ( (uint32_t) freqregH << 16 ) | ( (uint32_t) freqregM << 8 ) | ( freqregL ) );

  registershift = deviation / FREQ_STEP;
  shiftedfreqregH = freqreg + registershift;
  shiftedfreqregL = freqreg - registershift;

  uint8_t ShiftH = shiftedfreqregH >> 16;
  uint8_t ShiftM = shiftedfreqregH >> 8;
  uint8_t ShiftL = shiftedfreqregH;
  uint8_t NoShiftH = shiftedfreqregL >> 16;
  uint8_t NoShiftM = shiftedfreqregL >> 8;
  uint8_t NoShiftL = shiftedfreqregL;

  ToneDelayus = ((500000 / frequency));
  loopcount = (length * 500) / (ToneDelayus);
  ToneDelayus = ToneDelayus * adjust;

#ifdef SX127XDEBUG3
  Serial.print(F("frequency "));
  Serial.println(frequency);
  Serial.print(F("length "));
  Serial.println(length);

  Serial.print(F("freqreg "));
  Serial.println(freqreg, HEX);
  Serial.print(F("registershift "));
  Serial.println(registershift);
  shiftedfreqregH = freqreg + (registershift / 2);
  shiftedfreqregL = freqreg - (registershift / 2);
  Serial.print(F("shiftedfreqregH "));
  Serial.println(shiftedfreqregH, HEX);
  Serial.print(F("shiftedfreqregL "));
  Serial.println(shiftedfreqregL, HEX);

  Serial.print(F("ShiftedHigh,"));
  Serial.print(ShiftH, HEX);
  Serial.print(F(","));
  Serial.print(ShiftM, HEX);
  Serial.print(F(","));
  Serial.println(ShiftL, HEX);

  Serial.print(F("ShiftedLow,"));
  Serial.print(NoShiftH, HEX);
  Serial.print(F(","));
  Serial.print(NoShiftM, HEX);
  Serial.print(F(","));
  Serial.println(NoShiftL, HEX);
  Serial.print(F("ToneDelayus,"));
  Serial.println(ToneDelayus);
  Serial.print(F("loopcount,"));
  Serial.println(loopcount);
  Serial.println();
  Serial.println();
#endif

  writeRegister(REG_PLLHOP, 0xAD);            //set fast hop mode, needed for fast changes of frequency

  setTxParams(txpower, RADIO_RAMP_DEFAULT);
  setTXDirect();

#ifdef USE_SPI_TRANSACTION                    //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  for (index = 1; index <= loopcount; index++)
  {
    digitalWrite(_NSS, LOW);                  //set NSS low
    SPI.transfer(0x86);                       //address for write to REG_FRMSB
    SPI.transfer(ShiftH);
    SPI.transfer(ShiftM);
    SPI.transfer(ShiftL);
    digitalWrite(_NSS, HIGH);                 //set NSS high

    delayMicroseconds(ToneDelayus);

    digitalWrite(_NSS, LOW);                  //set NSS low
    SPI.transfer(0x86);                       //address for write to REG_FRMSB
    SPI.transfer(NoShiftH);
    SPI.transfer(NoShiftM);
    SPI.transfer(NoShiftL);
    digitalWrite(_NSS, HIGH);                 //set NSS high

    delayMicroseconds(ToneDelayus);
  }
  //now set the frequency registers back to centre
  digitalWrite(_NSS, LOW);                    //set NSS low
  SPI.transfer(0x86);                         //address for write to REG_FRMSB
  SPI.transfer(freqregH);
  SPI.transfer(freqregM);
  SPI.transfer(freqregL);
  digitalWrite(_NSS, HIGH);                   //set NSS high

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PLLHOP, 0x2D);            //restore PLLHOP register value
  setMode(MODE_STDBY_RC);                     //turns off carrier
}


void SX127XLT::setupDirect(uint32_t frequency, int32_t offset)
{
  //setup LoRa device for direct modulation mode
#ifdef SX127XDEBUG1
  Serial.println(F("setupDirect() "));
#endif

  _PACKET_TYPE = PACKET_TYPE_GFSK;            //need to swap packet type
  setMode(MODE_SLEEP);                        //can only swap to direct mode in sleepmode
  setMode(MODE_SLEEP);                        //can only swap to direct mode in sleepmode
  setMode(MODE_STDBY_RC);
  writeRegister(REG_DETECTOPTIMIZE, 0x00);    //set continuous mode
  setRfFrequency(frequency, offset);          //set the operating frequncy
  calibrateImage(0);                          //run calibration after setting frequency
  writeRegister(REG_FDEVLSB, 0);              //We are generating a tone by frequency shift so set deviation to 0
}


int8_t SX127XLT::getDeviceTemperature()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getDeviceTemperature()"));
#endif

  int8_t temperature;
  uint8_t regdata;

  regdata = readRegister(REG_IMAGECAL & 0xFE);
  writeRegister(REG_IMAGECAL, 0x00);                //register back to power up default
  writeRegister(REG_OPMODE, 0x00);                  //goto sleep
  writeRegister(REG_OPMODE, 0x00);                  //make sure switch to FSK mode
  writeRegister(REG_OPMODE, 0x01);                  //go into FSK standby
  delay(5);                                         //wait for oscillator startup
  writeRegister(REG_OPMODE, 0x04);                  //put device in FSK RX synth mode
  writeRegister(REG_IMAGECAL, 0x00);                //set TempMonitorOff = 0
  delay(1);                                         //wait at least 140uS
  writeRegister(REG_IMAGECAL, 0x01);                //set TempMonitorOff = 1
  setMode(MODE_STDBY_RC);                           //go back to standby
  writeRegister(REG_IMAGECAL, (regdata + 1));       //register back to previous setting, with TempMonitorOff set
  temperature = readRegister(REG_TEMP);

  if (temperature & 0x80 )                          //The sign bit is 1
  {
    temperature = ( ( ~temperature + 1 ) & 0xFF );  //Invert and divide by 4
  }
  else
  {
    temperature = ( temperature & 0xFF );           //Divide by 4
  }

  writeRegister(REG_OPMODE, MODE_STDBY);

  return temperature;
}


void SX127XLT::fskCarrierOn(int8_t txpower)
{
#ifdef SX127XDEBUG1
  Serial.println(F("fskCarrierOn() "));
#endif

  writeRegister(REG_PLLHOP, 0xAD);                   //set fast hop mode, needed for fast changes of frequency
  setTxParams(txpower, RADIO_RAMP_DEFAULT);
  setTXDirect();
}


void SX127XLT::fskCarrierOff()
{
#ifdef SX127XDEBUG1
  Serial.println(F("fskCarrierOff() "));
#endif

  setMode(MODE_STDBY_RC);                             //turns off carrier
}


void SX127XLT::setRfFrequencyDirect(uint8_t high, uint8_t mid, uint8_t low)
{

#ifdef SX127XDEBUG1
  Serial.println(F("setRfFrequencyDirect() "));
#endif

#ifdef USE_SPI_TRANSACTION                  //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                  //set NSS low
  SPI.transfer(0x86);                       //address for write to REG_FRMSB
  SPI.transfer(high);
  SPI.transfer(mid);
  SPI.transfer(low);
  digitalWrite(_NSS, HIGH);                 //set NSS high

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


void SX127XLT::getRfFrequencyRegisters(uint8_t *buff)
{
  //returns the register values for the current set frequency

#ifdef SX127XDEBUG1
  Serial.println(F("getRfFrequencyRegisters() "));
#endif

#ifdef USE_SPI_TRANSACTION                  //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                  //set NSS low
  SPI.transfer(REG_FRMSB & 0x7F);           //mask address for read
  buff[0] = SPI.transfer(0);                //read the byte into buffer
  buff[1] = SPI.transfer(0);                //read the byte into buffer
  buff[2] = SPI.transfer(0);                //read the byte into buffer
  digitalWrite(_NSS, HIGH);                 //set NSS high

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


void SX127XLT::startFSKRTTY(uint32_t freqshift, uint8_t pips, uint16_t pipPeriodmS, uint16_t pipDelaymS, uint16_t leadinmS)
{

#ifdef SX127XDEBUG1
  Serial.println(F("startFSKRTTY() "));
#endif

  uint8_t freqShiftRegs[3];                        //to hold returned registers for shifted frequency
  uint32_t setCentreFrequency;                     //the configured centre frequency
  uint8_t index;
  uint32_t startmS;

  setCentreFrequency = _savedFrequency;                         //to avoid using the savedFrequency
  writeRegister(REG_PLLHOP, 0xAD);                              //set fast hop mode, needed for fast changes of frequency
  setRfFrequency((_savedFrequency + freqshift), _savedOffset);  //temporaily set the RF frequency
  getRfFrequencyRegisters(freqShiftRegs);                       //fill first 3 bytes with current frequency registers
  setRfFrequency(setCentreFrequency, _savedOffset);             //reset the base frequency registers
  _ShiftfreqregH = freqShiftRegs[0];
  _ShiftfreqregM = freqShiftRegs[1];
  _ShiftfreqregL = freqShiftRegs[2];
  writeRegister(REG_PLLHOP, 0xAD);                              //set fast hop mode, needed for fast changes of frequency
  setTxParams(10, RADIO_RAMP_DEFAULT);

  for (index = 1; index <= pips; index++)
  {
    setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL);    //set carrier frequency
    setTXDirect();                                                           //turn on carrier
    delay(pipPeriodmS);
    setMode(MODE_STDBY_RC);                                                  //turns off carrier
    delay(pipDelaymS);
  }

  setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL);      //set carrier frequency
  startmS = millis();
  setTXDirect();                                                             //turn on carrier

  while (((uint32_t) (millis() - startmS) < leadinmS));
}


void SX127XLT::transmitFSKRTTY(uint8_t chartosend, uint8_t databits, uint8_t stopbits, uint8_t parity, uint16_t baudPerioduS, int8_t pin)
{
#ifdef SX127XDEBUG1
  Serial.println(F("transmitFSKRTTY() "));
#endif

  uint8_t numbits;
  uint32_t startuS;
  uint8_t bitcount = 0;                                  //set when a bit is 1

  startuS = micros();
  setRfFrequencyDirect(_freqregH, _freqregM, _freqregL); //set carrier frequency  (low)

  if (pin >= 0)
  {
    digitalWrite(pin, LOW);
  }

  while (((uint32_t) (micros() - startuS) < baudPerioduS));

  for (numbits = 1;  numbits <= databits; numbits++)     //send bits, LSB first
  {

    startuS = micros();
    if ((chartosend & 0x01) != 0)                        //test for bit set, a 1
    {
      bitcount++;
      if (pin >= 0)
      {
        digitalWrite(pin, HIGH);
      }
      setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL); //set carrier frequency for a 1
    }
    else
    {
      if (pin >= 0)
      {
        digitalWrite(pin, LOW);
      }
      setRfFrequencyDirect(_freqregH, _freqregM, _freqregL);      //set carrier frequency for a 0
    }
    chartosend = (chartosend >> 1);                               //get the next bit

    while (((uint32_t) (micros() - startuS) < baudPerioduS));
  }

  startuS = micros();
  switch (parity)
  {
    case ParityNone:
      break;

    case ParityZero:
      setRfFrequencyDirect(_freqregH, _freqregM, _freqregL);                  //set carrier frequency for a 0

      while (((uint32_t) (micros() - startuS) < baudPerioduS));
      break;

    case ParityOne:

      setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL);   //set carrier frequency for a 1

      while (((uint32_t) (micros() - startuS) < baudPerioduS));
      break;

    case ParityOdd:
      if (bitRead(bitcount, 0))                                               //test odd bit count, i.e. when bit 0 = 1
      {
        setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL); //set carrier frequency for a 1
      }
      else
      {
        setRfFrequencyDirect(_freqregH, _freqregM, _freqregL);                //set carrier frequency for a 0
      }
      while (((uint32_t) (micros() - startuS) < baudPerioduS));
      break;

    case ParityEven:
      if (bitRead(bitcount, 0))                                               //test odd bit count, i.e. when bit 0 = 1
      {
        setRfFrequencyDirect(_freqregH, _freqregM, _freqregL);                //set carrier frequency for a 0
      }
      else
      {
        setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL); //set carrier frequency for a 1
      }
      while (((uint32_t) (micros() - startuS) < baudPerioduS));
      break;

    default:
      break;
  }

  startuS = micros();

  if (pin >= 0)
  {
    digitalWrite(pin, HIGH);
  }

  setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL); //set carrier frequency

  while ( (uint32_t) (micros() - startuS) < (baudPerioduS * stopbits));
}


void SX127XLT::transmitFSKRTTY(uint8_t chartosend, uint16_t baudPerioduS, int8_t pin)
{
  //This overloaded version of transmitFSKRTTY() defaults to 1 start bit, 7 data bits, no parity and 2 stop bits.

#ifdef SX127XDEBUG1
  Serial.println(F("transmitFSKRTTY() "));
#endif

  uint8_t numbits;
  uint32_t startuS;

  //startbit

  startuS = micros();
  setRfFrequencyDirect(_freqregH, _freqregM, _freqregL); //set carrier frequency  (low)

  if (pin >= 0)
  {
    digitalWrite(pin, LOW);
  }

  while (((uint32_t) (micros() - startuS) < baudPerioduS));

  for (numbits = 1;  numbits <= 7; numbits++)           //send bits, LSB first
  {
    startuS = micros();
    if ((chartosend & 0x01) != 0)                       //test for bit set, a 1
    {
      if (pin >= 0)
      {
        digitalWrite(pin, HIGH);
      }
      setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL); //set carrier frequency for a 1
    }
    else
    {
      if (pin >= 0)
      {
        digitalWrite(pin, LOW);
      }
      setRfFrequencyDirect(_freqregH, _freqregM, _freqregL);                //set carrier frequency for a 0
    }
    chartosend = (chartosend >> 1);           //get the next bit
    while (((uint32_t) (micros() - startuS) < baudPerioduS));
  }

  //stop bits

  startuS = micros();

  if (pin >= 0)
  {
    digitalWrite(pin, HIGH);
  }

  setRfFrequencyDirect(_ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL);     //set carrier frequency

  while ((uint32_t) (micros() - startuS) < (baudPerioduS * 2));
}


void SX127XLT::printRTTYregisters()
{

#ifdef SX127XDEBUG1
  Serial.println(F("printRTTYregisters() "));
#endif

  Serial.print(F("NoShift Registers "));
  Serial.print(_freqregH, HEX);
  Serial.print(F(" "));
  Serial.print(_freqregM, HEX);
  Serial.print(F(" "));
  Serial.println(_freqregL, HEX);
  Serial.print(F("Shifted Registers "));
  Serial.print(_ShiftfreqregH, HEX);
  Serial.print(F(" "));
  Serial.print(_ShiftfreqregM, HEX);
  Serial.print(F(" "));
  Serial.println(_ShiftfreqregL, HEX);
}


void SX127XLT::endFSKRTTY()
{
#ifdef SX127XDEBUG1
  Serial.println(F("endFSKRTTY() "));
#endif

  setMode(MODE_STDBY_RC);
}


void SX127XLT::doAFC()
{
#ifdef SX127XDEBUG1
  Serial.println(F("doAFC() "));
#endif

  _savedOffset = _savedOffset - getFrequencyErrorHz();
  setRfFrequency(_savedFrequency, _savedOffset);     //adjust the operating frequency for AFC
}


void SX127XLT::doAFCPPM()
{
#ifdef SX127XDEBUG1
  Serial.println(F("doAFCPPM() "));
#endif

  int32_t frequencyerror;
  float Ferr_ppm;                                    //ppm error
  int8_t PPM_offset;

  frequencyerror = getFrequencyErrorHz();
  _savedOffset = _savedOffset - frequencyerror;
  setRfFrequency(_savedFrequency, _savedOffset);     //adjust the operating frequency for AFC

  //now deal with the PPM correction
  Ferr_ppm = frequencyerror / (_savedFrequency / 1E6);
  PPM_offset = 0.95 * Ferr_ppm;
  writeRegister(REG_PPMCORRECTION, PPM_offset);      //write PPM adjust value to device register
}


uint8_t SX127XLT::getPPM()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getPPM() "));
#endif

  return readRegister(REG_PPMCORRECTION);
}


int32_t SX127XLT::getOffset()
{
#ifdef SX127XDEBUG1
  Serial.println(F("getOffset() "));
#endif
  return _savedOffset;
}


uint8_t SX127XLT::readBufferbytes(uint8_t *buffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("readBufferbytes() "));
#endif

  uint8_t ptr = 0, regdata, index;

  for (index = 1; index <= size; index++)
  {
    regdata = SPI.transfer(0);
    buffer[ptr] = regdata;              //fill the buffer.
    ptr++;
  }

  _RXPacketL = _RXPacketL + size;       //increment count of bytes read and written to buffer

  return index;                         //return the actual size of the buffer
}


uint8_t SX127XLT::writeBufferbytes(uint8_t *buffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeBufferbytes() "));
#endif

  uint8_t regdata, index;

  for (index = 0; index < size; index++)
  {
    regdata = buffer[index];
    SPI.transfer(regdata);
  }

  _TXPacketL = _TXPacketL + size;       //increment count of bytes read and written to buffer

  return index;                         //return the actual size of the buffer
}


void SX127XLT::setDevicePABOOST()
{
#ifdef PACONFIGDEBUG
  Serial.println(F("setDevicePABOOST() "));
#endif

  bitSet(_Device, 4);                    //set bit 4 of _Device type which defines device as using PABoost RF output
}


void SX127XLT::setDeviceRFO()
{
#ifdef PACONFIGDEBUG
  Serial.println(F("setDeviceRFO() "));
#endif

  bitClear(_Device, 4);                  //clear bit 4 of _Device type which defines device as using RFO RF output
}


//****************************************************************************
//
// Reliable packet routines - added August 2021
// Routines assume that REG_FIFOTXBASEADDR and REG_FIFORXBASEADDR are set to 0 by setupLoRa()
// 20/09/20 Changed operation of getRXPayloadCRC(), getTXPayloadCRC(),getRXNetworkID(),getTXNetworkID()
//****************************************************************************


uint8_t SX127XLT::transmitReliable(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitRELIABLE() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
  Serial.print(F(" {RELIABLE} Payload length "));
  Serial.println(size);
#endif

  uint8_t index, tempdata;
  uint16_t payloadcrc;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (size > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  _TXPacketL = size + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITT(txbuffer, size, 0xFFFF);
  }

  writeRegister(REG_FIFOADDRPTR, 0);          //set PTR to start of FIFO

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < size; index++)
  {
    tempdata = txbuffer[index];
    SPI.transfer(tempdata);
  }

  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadcrc));
  SPI.transfer(highByte(payloadcrc));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);

  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                  //Wait for pin to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))                        //if _TXDonePin is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::transmitReliableAutoACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} transmitReliableAutoACK() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
  Serial.print(F(" {RELIABLE} Payload length "));
  Serial.println(size);
#endif

  uint8_t index, tempdata, RXPacketL;
  uint16_t payloadcrc;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (size > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  _TXPacketL = size + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITT(txbuffer, size, 0xFFFF);
  }

  writeRegister(REG_FIFOADDRPTR, 0);                        //and save in FIFO access ptr

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < size; index++)
  {
    tempdata = txbuffer[index];
    SPI.transfer(tempdata);
  }

  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadcrc));
  SPI.transfer(highByte(payloadcrc));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                  //Wait for pin to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))                        //if _TXDonePin is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  RXPacketL = waitReliableACK(networkID, payloadcrc, acktimeout);

  if (RXPacketL != 4)
  {
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::receiveReliable(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{

  //Maximum LoRa packet size is 255 bytes, so allowing for the 4 bytes appended to the end of a reliable
  //packet, the maximum payload size is 251 bytes. So to avoid overwriting memory, we do need to check if
  //the passed array is big enough to take the payload received in the packet. The assumed payload length will
  //always be 4 bytes less than the received packet length

#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveReliable()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint32_t startmS;
  uint8_t regdataL, regdataH;
  uint8_t index;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (size > 251 )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  writeRegister(REG_FIFOADDRPTR, 0);                                       //and save in FIFO access ptr
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while   ( (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout)) || ((readRegister(REG_MODEMSTAT) & 0x03)) )  ;
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL < 4)    //check received packet is 4 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if ((_RXPacketL - 4) > size )                        //check if calculated payload size (_RXPacketL -4) fits in array
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  writeRegister(REG_FIFOADDRPTR, 0);

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                              //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < (_RXPacketL - 4); index++)
  {
    regdataL = SPI.transfer(0);
    rxbuffer[index] = regdataL;
  }

  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);
  RXnetworkID = ((uint16_t) regdataH << 8) + regdataL;
  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITT(rxbuffer, (_RXPacketL - 4), 0xFFFF);
    RXcrc = ((uint16_t) regdataH << 8) + regdataL;

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (_ReliableErrors)                                      //if there has been a reliable error return a RX fail
  {
    return 0;
  }

  return _RXPacketL;                                        //return and indicate RX OK.
}


uint8_t SX127XLT::receiveReliableAutoACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait )
{
  //Maximum LoRa packet size is 255 bytes, so allowing for the 4 bytes appended to the end of a reliable
  //packet, the maximum payload size is 251 bytes. So to avoid overwriting memory, we do need to check if
  //the passed array is big enough to take the payload received in the packet. The assumed payload length will
  //always be 4 bytes less than the received packet length


#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveReliableAutoACK()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint32_t startmS;
  uint8_t regdataL, regdataH, index;

  _ReliableErrors = 0;
  _ReliableFlags = 0;


  if (size > 251 )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  writeRegister(REG_FIFOADDRPTR, 0);
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL < 4)    //check received packet is 4 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if ((_RXPacketL - 4) > size )                      //check if calculated payload size (_RXPacketL -4) fits in array
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  writeRegister(REG_FIFOADDRPTR, 0);             //set FIFO access ptr to beginning of packet

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < (_RXPacketL - 4); index++)
  {
    regdataL = SPI.transfer(0);
    rxbuffer[index] = regdataL;
  }

  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);
  RXnetworkID = ((uint16_t) regdataH << 8) + regdataL;
  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITT(rxbuffer, (_RXPacketL - 4), 0xFFFF);
    RXcrc = ((uint16_t) regdataH << 8) + regdataL;

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (_ReliableErrors)                                    //if there has been a reliable error return a RX fail
  {
    return 0;
  }

  delay(ackdelay);
  _TXPacketL = sendReliableACK(RXnetworkID, payloadcrc, txpower);

  if (_TXPacketL != 4)
  {
    return 0;
  }

  return _RXPacketL;                                        //return and indicate RX OK.
}


uint8_t SX127XLT::sendReliableACK(uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} sendReliableACK()"));
#endif

  uint32_t startmS, txtimeout = 60000;                       //set TX timeout to 60 seconds

  _TXPacketL = 4;                                            //packet is networkId (2 bytes) + payloadCRC (2 bytes)

  setMode(MODE_STDBY_RC);

  writeUint16SXBuffer(0, networkID);
  writeUint16SXBuffer(2, payloadcrc);

  writeRegister(REG_PAYLOADLENGTH, 4);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);                  //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);         //set for IRQ on TX done
  setTx(0);                                                  //start transmission

  startmS = millis();
  while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));

  setMode(MODE_STDBY_RC);                                    //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return 4;                                                  //TX OK so return TXpacket length
}


uint8_t SX127XLT::sendReliableACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{
  //overloaded version of sendReliableACK() for use when ack contains payload data
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendReliableACK() "));
  Serial.print(F("buffer size "));
  Serial.println(size);
#endif

  uint32_t startmS, txtimeout = 60000;                         //set TX timeout to 15 seconds
  uint8_t bufferdata, index;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOADDRPTR, 0);                           //set ptr to write packet
  _TXPacketL = size + 4;

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < size; index++)
  {
    bufferdata = txbuffer[index];
    SPI.transfer(bufferdata);
  }

  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadcrc));
  SPI.transfer(highByte(payloadcrc));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);                  //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);         //set for IRQ on TX done
  setTx(0);                                                  //start transmission

  startmS = millis();
  while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));

  setMode(MODE_STDBY_RC);                                    //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                          //TX OK so return TXpacket length
}


uint8_t SX127XLT::waitReliableACK(uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitReliableACK()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;

  setReliableRX();
  startmS = millis();

  do
  {
    if (digitalRead(_RXDonePin))                                             //has a packet arrived ?
    {
      if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
      {
        setReliableRX();
        continue;
      }

      _RXPacketL = readRegister(REG_RXNBBYTES);

      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        return 4;                                                            //return value of 4 indicates valid ack
      }
      else
      {
        setReliableRX();
        continue;
      }
    }

  } while ( ((uint32_t) (millis() - startmS) < acktimeout)  || ((readRegister(REG_MODEMSTAT) & 0x03)) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}


uint8_t SX127XLT::waitReliableACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{
  //overloaded version of waitReliableACK() for use when ack contains payload data
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitReliableACK()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t regdata, index;


  if (size > 251 )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setReliableRX();
  startmS = millis();

  do
  {
    if (digitalRead(_RXDonePin))                                             //has a packet arrived ?
    {
      if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
      {
        setReliableRX();
        continue;
      }

      _RXPacketL = readRegister(REG_RXNBBYTES);
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        if ((_RXPacketL - 4) > size )                        //check passed buffer is big enough for payload
        {
          bitSet(_ReliableErrors, ReliableACKError);
          bitSet(_ReliableErrors, ReliableSizeError);
          return 0;
        }

        bitSet(_ReliableFlags, ReliableACKReceived);
        writeRegister(REG_FIFOADDRPTR, 0);                   //set FIFO access ptr

#ifdef USE_SPI_TRANSACTION
        SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

        digitalWrite(_NSS, LOW);                             //start the burst read
        SPI.transfer(REG_FIFO);

        for (index = 0; index < (_RXPacketL - 4); index++)   //read packet into rxbuffer
        {
          regdata = SPI.transfer(0);
          rxbuffer[index] = regdata;
        }
        digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
        SPI.endTransaction();
#endif

        return _RXPacketL;                                    //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX();
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout)  || ((readRegister(REG_MODEMSTAT) & 0x03)) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}


void SX127XLT::writeUint16SXBuffer(uint8_t addr, uint16_t regdata)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} writeUint16SXBuffer() 0x"));
  Serial.print(addr, HEX);
  Serial.print(F(" 0x"));
  Serial.println(regdata, HEX);
#endif

  setMode(MODE_STDBY_RC);                 //this is needed to ensure we can write to buffer OK.
  writeRegister(REG_FIFOADDRPTR, addr);   //set FIFO access ptr to location

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                //start the burst read
  SPI.transfer(WREG_FIFO);
  SPI.transfer(lowByte(regdata));
  SPI.transfer(highByte(regdata));
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


uint16_t SX127XLT::readUint16SXBuffer(uint8_t addr)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} readUint16SXBuffer() 0x"));
  Serial.println(addr, HEX);
#endif

  uint8_t regdatalow, regdatahigh;
  setMode(MODE_STDBY_RC);                     //this is needed to ensure we can read from buffer OK.

  writeRegister(REG_FIFOADDRPTR, addr);       //set FIFO access ptr to location

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(REG_FIFO);
  regdatalow = SPI.transfer(0);
  regdatahigh = SPI.transfer(0);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return (regdatalow + (regdatahigh << 8));
}


uint8_t SX127XLT::readReliableErrors()
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} readReliableErrors()"));
#endif

  return _ReliableErrors;
}


uint8_t SX127XLT::readReliableFlags()
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} readReliableFlags()"));
#endif

  return _ReliableFlags;
}


uint8_t SX127XLT::readReliableConfig()
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} readReliableConfig()"));
#endif

  return _ReliableConfig;
}


void SX127XLT::setReliableConfig(uint8_t bitset)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} setReliableConfig() bit "));
  Serial.println(bitset);
#endif

  bitSet(_ReliableConfig, bitset);
  return;
}


void SX127XLT::clearReliableConfig(uint8_t bitclear)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} clearReliableConfig() bit "));
  Serial.println(bitclear);
#endif

  bitClear(_ReliableConfig, bitclear);
  return;
}


uint8_t SX127XLT::getReliableConfig(uint8_t bitread)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} getReliableConfig() "));
  Serial.println(_ReliableConfig);
#endif

  return bitRead(_ReliableConfig, bitread);
}


void SX127XLT::printReliableConfig()

{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} printReliableConfig() "));
#endif

  Serial.print(F("ReliableConfig"));

  //bit 0x00
  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    Serial.print(F(",CRCdisabled"));
  }
  else
  {
    Serial.print(F(",CRCenabled"));
  }

  //bit 0x01
  if (bitRead(_ReliableConfig, NoAutoACK))
  {
    Serial.print(F(",AutoACKdisabled"));
  }
  else
  {
    Serial.print(F(",AutoACKenabled"));
  }

}


void SX127XLT::printReliableStatus()
{
#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} printReliableStatus() "));
#endif

  //0x00
  if (bitRead(_ReliableErrors, ReliableCRCError))
  {
    Serial.print(F(",ReliableCRCError"));
  }

  //0x01
  if (bitRead(_ReliableErrors, ReliableIDError))
  {
    Serial.print(F(",ReliableIDError"));
  }

  //0x02
  if (bitRead(_ReliableErrors, ReliableSizeError))
  {
    Serial.print(F(",ReliableSizeError"));
  }

  //0x03
  if (bitRead(_ReliableErrors, ReliableACKError))
  {
    Serial.print(F(",NoReliableACK"));
  }

  if (bitRead(_ReliableErrors, ReliableTimeout))
  {
    Serial.print(F(",ReliableTimeout"));
  }


  //0x00
  if (bitRead(_ReliableFlags, ReliableACKSent))
  {
    Serial.print(F(",ACKsent"));
  }


  //0x01
  if (bitRead(_ReliableFlags, ReliableACKReceived))
  {
    Serial.print(F(",ACKreceived"));
  }

}


void SX127XLT::setReliableRX()
{
  //existing setRx() does not setup LoRa device as a receiver completly, just turns on receiver mode
  //this routine does all the required setup for receive mode
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} setReliableRX()"));
#endif

  setMode(MODE_STDBY_RC);                                                  //stops receiver
  clearIrqStatus(IRQ_RADIO_ALL);                                           //clear current interrupt flags
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  writeRegister(REG_OPMODE, (MODE_RXCONTINUOUS + 0x80));                   //RX on LoRa continuous mode
}


uint16_t SX127XLT::CRCCCITTReliable(uint8_t startadd, uint8_t endadd, uint16_t startvalue)
{
  //generates a CRC of bytes from the internal SX buffer, _RXPackletL and _TXPackletL are not affected

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} CRCCCITTReliable() "));
#endif

  uint16_t index, libraryCRC;
  uint8_t j, readSX;

  libraryCRC = startvalue;                       //start value for CRC16

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOADDRPTR, startadd);      //set buffer access ptr

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                       //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = startadd; index <= endadd; index++)
  {
    readSX = SPI.transfer(0);
    libraryCRC ^= (((uint16_t) readSX ) << 8);
    for (j = 0; j < 8; j++)
    {
      if (libraryCRC & 0x8000)
        libraryCRC = (libraryCRC << 1) ^ 0x1021;
      else
        libraryCRC <<= 1;
    }
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return libraryCRC;
}


void SX127XLT::writeArray(uint8_t *txbuffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("writeArray() "));
#endif

  uint8_t index, regdata;
  _TXPacketL = _TXPacketL + size;      //these are the number of bytes that will be added

  for (index = 0; index <= size; index++)
  {
    regdata = txbuffer[index];
    SPI.transfer(regdata);
  }
}


void SX127XLT::printASCIIArray(uint8_t *buffer, uint8_t size)
{
#ifdef SX127XDEBUG1
  Serial.println(F("printASCIIArray() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    Serial.write(buffer[index]);
  }
}


uint16_t SX127XLT::getRXPayloadCRC(uint8_t length)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getRXPayloadCRC() "));
#endif
  uint8_t regdata = readRegister(REG_FIFORXBASEADDR);       //retrieve the RXbase address pointer
  return readUint16SXBuffer(regdata + length - 2);
}


uint16_t SX127XLT::getTXPayloadCRC(uint8_t length)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getTXPayloadCRC() "));
#endif
  uint8_t regdata = readRegister(REG_FIFOTXBASEADDR);       //retrieve the TXbase address pointer
  return readUint16SXBuffer(regdata + length - 2);
}



uint16_t SX127XLT::getRXNetworkID(uint8_t length)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getRXnetworkID() "));
#endif
  uint8_t regdata = readRegister(REG_FIFORXBASEADDR);       //retrieve the RXbase address pointer
  return readUint16SXBuffer(regdata + length - 4);
}


uint16_t SX127XLT::getTXNetworkID(uint8_t length)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getTXnetworkID() "));
#endif
  uint8_t regdata = readRegister(REG_FIFOTXBASEADDR);       //retrieve the TXbase address pointer
  return readUint16SXBuffer(regdata + length - 4);
}

//****************************************************************************
//
// Reliable SX packet routines - added August 2021
// Note that the startaddr is writen to the appropriate base address;
// REG_FIFORXBASEADDR or REG_FIFOTXBASEADDR
//
//****************************************************************************

uint8_t SX127XLT::transmitSXReliable(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitSXReliable() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc;
  uint32_t startmS;

  writeRegister(REG_FIFOTXBASEADDR, startaddr);      //save base TX ptr
  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (startaddr + length > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  _TXPacketL = length + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF);
  }

  writeUint16SXBuffer(startaddr + _TXPacketL - 4, networkID);
  writeUint16SXBuffer(startaddr + _TXPacketL - 2, payloadcrc);
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                  //Wait for pin to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))                        //if _TXDonePin is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::transmitSXReliableIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitSXReliableIRQ() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc;
  uint32_t startmS;

  writeRegister(REG_FIFOTXBASEADDR, startaddr);      //save base TX ptr
  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (startaddr + length > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  _TXPacketL = length + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF);
  }

  writeUint16SXBuffer(startaddr + _TXPacketL - 4, networkID);
  writeUint16SXBuffer(startaddr + _TXPacketL - 2, payloadcrc);
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!isTXdoneIRQ());                            //Wait for IRQ to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!isTXdoneIRQ() && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!isTXdoneIRQ())                                  //if IRQ is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::transmitSXReliableAutoACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitSXReliableAutoACK() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint8_t RXPacketL;
  uint16_t payloadcrc;
  uint32_t startmS;

  writeRegister(REG_FIFOTXBASEADDR, startaddr);      //save base TX ptr
  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (startaddr + length > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  _TXPacketL = length + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF);
  }

  writeUint16SXBuffer(startaddr + _TXPacketL - 4, networkID);
  writeUint16SXBuffer(startaddr + _TXPacketL - 2, payloadcrc);
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                  //Wait for pin to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))                        //if _TXDonePin is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  RXPacketL = waitReliableACK(networkID, payloadcrc, acktimeout);

  if (RXPacketL != 4)
  {

    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::receiveSXReliable(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveSXReliable()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;
  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFORXBASEADDR, startaddr);                            //set start address of RX packet in buffer
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while   ((!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout)) || ((readRegister(REG_MODEMSTAT) & 0x03)));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL < 4)                                                      //check received packet is 4 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITTReliable(startaddr, (startaddr + _RXPacketL - 5), 0xFFFF);
    RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (_ReliableErrors)                                      //if there has been a reliable error return a RX fail
  {
    return 0;
  }
  return _RXPacketL;                                         //return and RX OK.
}


uint8_t SX127XLT::receiveSXReliableIRQ(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveSXReliableIRQ()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFORXBASEADDR, startaddr);                            //set start address of RX packet in buffer

  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!isRXdoneIRQ());                                                //Wait for IRQ to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while   ((!isRXdoneIRQ() && ((uint32_t) (millis() - startmS) < rxtimeout)) || ((readRegister(REG_MODEMSTAT) & 0x03)));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!isRXdoneIRQ())                                                      //check if IRQ still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL < 4)                                                      //check received packet is 4 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITTReliable(startaddr, (startaddr + _RXPacketL - 5), 0xFFFF);
    RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (_ReliableErrors)                                       //if there has been a reliable error return a RX fail
  {
    return 0;
  }
  return _RXPacketL;                                         //return and RX OK.
}


uint8_t SX127XLT::receiveSXReliableAutoACK(uint8_t startaddr, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveSXReliableAutoACK()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint16_t temp1, temp2;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFORXBASEADDR, startaddr);                            //set start address of RX packet in buffer

  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while   ( (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout)) || ((readRegister(REG_MODEMSTAT) & 0x03)));
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (_RXPacketL < 4)                                                      //check received packet is 4 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITTReliable(startaddr, (startaddr + _RXPacketL - 5), 0xFFFF);
    RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (_ReliableErrors)                                            //if there has been a reliable error return a RX fail
  {
    return 0;
  }

  delay(ackdelay);
  temp1 = readUint16SXBuffer(startaddr);                          //save bytes that would be overwritten by ack
  temp2 = readUint16SXBuffer(startaddr + 2);                      //save bytes that would be overwritten by ack

  _TXPacketL = sendReliableACK(RXnetworkID, payloadcrc, txpower);

  writeUint16SXBuffer(startaddr, temp1);                          //restore bytes that would be overwritten by ack
  writeUint16SXBuffer(startaddr + 2, temp2);                      //restore bytes that would be overwritten by ack

  if (_TXPacketL != 4)
  {
    bitSet(_ReliableErrors, ReliableACKError);
    return 0;
  }

  return _RXPacketL;                                              //return indicating RX ack sent OK.
}


uint8_t SX127XLT::sendSXReliableACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendSXReliableACK() "));
#endif

  uint32_t startmS, txtimeout = 60000;                            //set TX timeout to 60 seconds

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOTXBASEADDR, startaddr);                   //set FIFO access ptr
  _TXPacketL = length + 4;
  writeUint16SXBuffer((length + startaddr), networkID);
  writeUint16SXBuffer((length + startaddr + 2), payloadcrc);
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);                  //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);         //set for IRQ on TX done
  setTx(0);                                                  //start transmission

  startmS = millis();
  while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));

  setMode(MODE_STDBY_RC);                                    //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                          //TX OK so return TXpacket length
}


uint8_t SX127XLT::sendSXReliableACKIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendSXReliableACKIRQ() "));
  Serial.print(F("payload size "));
  Serial.println(length);
#endif

  uint32_t startmS, txtimeout = 60000;                         //set TX timeout to 60 seconds

  setMode(MODE_STDBY_RC);
  writeRegister(REG_FIFOTXBASEADDR, startaddr);                //set FIFO access ptr
  _TXPacketL = length + 4;
  writeUint16SXBuffer((length + startaddr), networkID);
  writeUint16SXBuffer((length + startaddr + 2), payloadcrc);
  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);                     //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);            //set for IRQ on TX done
  setTx(0);                                                     //start transmission

  startmS = millis();
  while (!isTXdoneIRQ() && ((uint32_t) (millis() - startmS) < txtimeout));

  setMode(MODE_STDBY_RC);                                        //ensure we leave function with TX off

  if (!isTXdoneIRQ())
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                              //TX OK so return TXpacket length
}


uint8_t SX127XLT::waitSXReliableACK(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitSXReliableACK()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;

  writeRegister(REG_FIFORXBASEADDR, startaddr);                    //set the RX base address pointer
  setReliableRX();
  startmS = millis();

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} Waiting RXDonePin"));
#endif

  do
  {
    if (digitalRead(_RXDonePin))                                    //has a packet arrived ?
    {
      _RXPacketL = readRegister(REG_RXNBBYTES);

      if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
      {
        setReliableRX();
        continue;
      }

      RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);
      RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        return _RXPacketL;                                          //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX();
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout)  || ((readRegister(REG_MODEMSTAT) & 0x03)) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}


uint8_t SX127XLT::waitSXReliableACKIRQ(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitSXReliableACKIRQ()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;

  writeRegister(REG_FIFORXBASEADDR, startaddr);              //set the RX base address pointer
  setReliableRX();
  startmS = millis();

  do
  {
    if (isRXdoneIRQ())                                       //has a packet arrived ?
    {
      _RXPacketL = readRegister(REG_RXNBBYTES);

      if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
      {
        setReliableRX();
        continue;
      }
      RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);
      RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        return _RXPacketL;                                   //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX();
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout)  || ((readRegister(REG_MODEMSTAT) & 0x03)) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}

//*****************************************************************************************
//Data Transfer functions - added September 2021 - TX and RX base addresses assumed to be 0
//*****************************************************************************************

uint8_t SX127XLT::transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} transmitDT() "));
#endif

  uint8_t index, bufferdata;
  uint16_t payloadcrc;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (datasize > (251 - headersize))                  //its 251 because of 4 bytes appended to packet
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  _TXPacketL = headersize + datasize + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITT(dataarray, datasize, 0xFFFF);
  }

  writeRegister(REG_FIFOADDRPTR, 0);                             //set PTR to start of FIFO

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  //load up the header
  for (index = 0; index < headersize; index++)
  {
    bufferdata = header[index];
    SPI.transfer(bufferdata);
  }

  //load up the data array
  for (index = 0; index < datasize; index++)
  {
    bufferdata = dataarray[index];
    SPI.transfer(bufferdata);
  }

  //append the network ID and payload CRC at end
  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadcrc));
  SPI.transfer(highByte(payloadcrc));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);

  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!digitalRead(_TXDonePin));                  //Wait for pin to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))                        //if _TXDonePin is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} receiveDT()"));
#endif

  uint16_t index, payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint32_t startmS;
  uint8_t regdataL, regdataH;
  uint8_t RXHeaderL;
  uint8_t RXDataL;

  _ReliableErrors = 0;
  _ReliableFlags = 0;
  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!digitalRead(_RXDonePin));                                      //Wait for DIO0 to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while   ( (!digitalRead(_RXDonePin) && ((uint32_t) (millis() - startmS) < rxtimeout)) || ((readRegister(REG_MODEMSTAT) & 0x03)) )  ;
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!digitalRead(_RXDonePin))                                            //check if DIO still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  RXHeaderL = getByteSXBuffer(2);
  RXDataL = getByteSXBuffer(3);
  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (RXHeaderL > headersize )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if (RXDataL > datasize )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if (_RXPacketL < 8)                                      //check received packet is 8 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  writeRegister(REG_FIFOADDRPTR, 0);                       //set FIFO access ptr to beginning of packet

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                                 //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < RXHeaderL; index++)
  {
    regdataL = SPI.transfer(0);
    header[index] = regdataL;
  }

  for (index = 0; index < RXDataL; index++)
  {
    regdataL = SPI.transfer(0);
    dataarray[index] = regdataL;
  }

  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);
  RXnetworkID = ((uint16_t) regdataH << 8) + regdataL;
  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITT(dataarray, RXDataL, 0xFFFF);
    RXcrc = ((uint16_t) regdataH << 8) + regdataL;

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (_ReliableErrors)                                            //if there has been a reliable error return a RX fail
  {
    return 0;
  }

  return _RXPacketL;                                              //return and indicate RX OK.
}


uint8_t SX127XLT::sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendACKDT() "));
#endif

  uint32_t startmS, txtimeout = 60000;                            //set TX timeout to 60 seconds
  uint8_t bufferdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;

  setMode(MODE_STDBY_RC);
  _TXPacketL = headersize + 4;
  networkID = readUint16SXBuffer(_RXPacketL - 4);
  payloadCRC = readUint16SXBuffer(_RXPacketL - 2);

  writeRegister(REG_FIFOADDRPTR, 0);                               //set FIFO access ptr

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < headersize; index++)
  {
    bufferdata = header[index];
    SPI.transfer(bufferdata);
  }

  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadCRC));
  SPI.transfer(highByte(payloadCRC));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);                  //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);         //set for IRQ on TX done
  setTx(0);                                                  //start transmission

  startmS = millis();
  while (!digitalRead(_TXDonePin) && ((uint32_t) (millis() - startmS) < txtimeout));

  setMode(MODE_STDBY_RC);                                    //ensure we leave function with TX off

  if (!digitalRead(_TXDonePin))
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                          //TX OK so return TXpacket length
}


uint8_t SX127XLT::waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitACKDT()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t regdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;

  networkID = readUint16SXBuffer(_TXPacketL - 4);              //get networkID used to transmit previous packet, before next RX
  payloadCRC = readUint16SXBuffer(_TXPacketL - 2);             //get payloadCRC used to transmit previous packet, before next RX
  setReliableRX();

  startmS = millis();

  do
  {
    if (digitalRead(_RXDonePin))                                //has a packet arrived ?
    {
      if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
      {
        setReliableRX();
        continue;
      }

      _RXPacketL = readRegister(REG_RXNBBYTES);
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ((_RXPacketL - 4) > headersize )                       //check passed buffer is big enough for header
      {
        setReliableRX();
        continue;
      }

      if (!bitRead(_ReliableConfig, NoReliableCRC))
      {
        if (payloadCRC != RXcrc)
        {
          bitSet(_ReliableErrors, ReliableCRCError);
          setReliableRX();
          continue;
        }
      }

      if ( (RXnetworkID == networkID))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        writeRegister(REG_FIFOADDRPTR, 0);                      //set FIFO access ptr

#ifdef USE_SPI_TRANSACTION
        SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

        digitalWrite(_NSS, LOW);                                //start the burst read
        SPI.transfer(REG_FIFO);

        for (index = 0; index < (_RXPacketL - 4); index++)      //read packet into rxbuffer
        {
          regdata = SPI.transfer(0);
          header[index] = regdata;
        }
        digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
        SPI.endTransaction();
#endif

        return _RXPacketL;                                      //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX();
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout)  || ((readRegister(REG_MODEMSTAT) & 0x03)) );

  bitSet(_ReliableErrors, ReliableACKError);
  bitSet(_ReliableErrors, ReliableTimeout);
  return 0;
}


uint8_t SX127XLT::transmitDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} transmitDTIRQ() "));
#endif

  uint8_t index, bufferdata;
  uint16_t payloadcrc;
  uint32_t startmS;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (datasize > (251 - headersize))                           //its 251 because of 4 bytes appended to packet
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  _TXPacketL = headersize + datasize + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITT(dataarray, datasize, 0xFFFF);
  }

  writeRegister(REG_FIFOADDRPTR, 0);                           //set PTR to start of FIFO

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  //load up the header
  for (index = 0; index < headersize; index++)
  {
    bufferdata = header[index];
    SPI.transfer(bufferdata);
  }

  //load up the data array
  for (index = 0; index < datasize; index++)
  {
    bufferdata = dataarray[index];
    SPI.transfer(bufferdata);
  }

  //append the network ID and payload CRFC at end
  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadcrc));
  SPI.transfer(highByte(payloadcrc));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);            //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done on first DIO pin
  setTx(0);                                            //TX timeout is not handled in setTX()

  if (!wait)
  {
    return _TXPacketL;
  }

  if (txtimeout == 0)
  {
    while (!isTXdoneIRQ());                           //Wait for IRQ to go high, TX finished
  }
  else
  {
    startmS = millis();
    while (!isTXdoneIRQ() && ((uint32_t) (millis() - startmS) < txtimeout));
  }

  setMode(MODE_STDBY_RC);                             //ensure we leave function with TX off

  if (!isTXdoneIRQ())                                 //if  TXdone IRQ is still low its a TX timeout
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX127XLT::receiveDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} receiveDTIRQ()"));
#endif

  uint16_t index, payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint32_t startmS;
  uint8_t regdataL, regdataH;
  uint8_t RXHeaderL;
  uint8_t RXDataL;

  _ReliableErrors = 0;
  _ReliableFlags = 0;
  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);                       //set for IRQ on RX done
  setRx(0);                                                                //no actual RX timeout in this function

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  if (rxtimeout == 0)
  {
    while (!isRXdoneIRQ());                                               //Wait for IRQ to go high, no timeout, RX DONE
  }
  else
  {
    startmS = millis();
    while   ( (!isRXdoneIRQ() && ((uint32_t) (millis() - startmS) < rxtimeout)) || ((readRegister(REG_MODEMSTAT) & 0x03)) )  ;
  }

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if (!isRXdoneIRQ())                                                      //check if IRQ still low, is so must be RX timeout
  {
    _IRQmsb = IRQ_RX_TIMEOUT;
    return 0;
  }

  if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
  {
    return 0;                                                              //no RX done IRQ and header valid only, could be CRC error
  }

  RXHeaderL = getByteSXBuffer(2);
  RXDataL = getByteSXBuffer(3);
  _RXPacketL = readRegister(REG_RXNBBYTES);

  if (RXHeaderL > headersize )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if (RXDataL > datasize )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if (_RXPacketL < 8)                              //check received packet is 8 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  writeRegister(REG_FIFOADDRPTR, 0);              //set FIFO access ptr to beginning of packet

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                        //start the burst read
  SPI.transfer(REG_FIFO);

  for (index = 0; index < RXHeaderL; index++)
  {
    regdataL = SPI.transfer(0);
    header[index] = regdataL;
  }

  for (index = 0; index < RXDataL; index++)
  {
    regdataL = SPI.transfer(0);
    dataarray[index] = regdataL;
  }

  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);
  RXnetworkID = ((uint16_t) regdataH << 8) + regdataL;
  regdataL = SPI.transfer(0);
  regdataH = SPI.transfer(0);

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  if (!bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = CRCCCITT(dataarray, RXDataL, 0xFFFF);
    RXcrc = ((uint16_t) regdataH << 8) + regdataL;

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (RXnetworkID != networkID)
  {
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (_ReliableErrors)                                             //if there has been a reliable error return a RX fail
  {
    return 0;
  }

  return _RXPacketL;                                               //return and indicate RX OK.
}


uint8_t SX127XLT::sendACKDTIRQ(uint8_t *header, uint8_t headersize, int8_t txpower)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendACKDTIRQ() "));
#endif

  uint32_t startmS, txtimeout = 60000;                             //set TX timeout to 15 seconds
  uint8_t bufferdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;

  setMode(MODE_STDBY_RC);
  _TXPacketL = headersize + 4;
  networkID = readUint16SXBuffer(_RXPacketL - 4);
  payloadCRC = readUint16SXBuffer(_RXPacketL - 2);
  writeRegister(REG_FIFOADDRPTR, 0);                               //set FIFO access ptr

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(WREG_FIFO);

  for (index = 0; index < headersize; index++)
  {
    bufferdata = header[index];
    SPI.transfer(bufferdata);
  }

  SPI.transfer(lowByte(networkID));
  SPI.transfer(highByte(networkID));
  SPI.transfer(lowByte(payloadCRC));
  SPI.transfer(highByte(payloadCRC));

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  writeRegister(REG_PAYLOADLENGTH, _TXPacketL);
  setTxParams(txpower, RADIO_RAMP_DEFAULT);                  //TX power and ramp time
  setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);         //set for IRQ on TX done
  setTx(0);                                                  //start transmission

  startmS = millis();
  while (!isTXdoneIRQ() && ((uint32_t) (millis() - startmS) < txtimeout));

  setMode(MODE_STDBY_RC);                                    //ensure we leave function with TX off

  if (!isTXdoneIRQ())
  {
    _IRQmsb = IRQ_TX_TIMEOUT;
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                          //TX OK so return TXpacket length
}


uint8_t SX127XLT::waitACKDTIRQ(uint8_t *header, uint8_t headersize, uint32_t acktimeout)
{

#ifdef SX127XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitACKDTIRQ()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t regdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;

  networkID = readUint16SXBuffer(_TXPacketL - 4);                  //get networkID used to transmit previous packet, before next RX
  payloadCRC = readUint16SXBuffer(_TXPacketL - 2);                 //get payloadCRC used to transmit previous packet, before next RX
  setReliableRX();

  startmS = millis();

  do
  {
    if (isRXdoneIRQ())                                             //has a packet arrived ?
    {
      if ( readIrqStatus() != (IRQ_RX_DONE + IRQ_HEADER_VALID) )
      {
        setReliableRX();
        continue;
      }

      _RXPacketL = readRegister(REG_RXNBBYTES);
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ((_RXPacketL - 4) > headersize )                           //check passed buffer is big enough for header
      {
        setReliableRX();
        continue;
      }

      if (!bitRead(_ReliableConfig, NoReliableCRC))
      {
        if (payloadCRC != RXcrc)
        {
          bitSet(_ReliableErrors, ReliableCRCError);
          setReliableRX();
          continue;
        }
      }

      if ( (RXnetworkID == networkID))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        writeRegister(REG_FIFOADDRPTR, 0);                     //set FIFO access ptr

#ifdef USE_SPI_TRANSACTION
        SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

        digitalWrite(_NSS, LOW);                               //start the burst read
        SPI.transfer(REG_FIFO);

        for (index = 0; index < (_RXPacketL - 4); index++)     //read packet header buffer
        {
          regdata = SPI.transfer(0);
          header[index] = regdata;
        }
        digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
        SPI.endTransaction();
#endif

        return _RXPacketL;                                     //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX();
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout)  || ((readRegister(REG_MODEMSTAT) & 0x03)) );

  bitSet(_ReliableErrors, ReliableACKError);
  bitSet(_ReliableErrors, ReliableTimeout);
  return 0;
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
