/*
  Copyright 2019 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  06/02/20
*/

/*
  Parts of code Copyright (c) 2013, SEMTECH S.A.
  See LICENSE.TXT file included in the library
*/

#include <SX128XLT.h>
#include <SPI.h>

#define LTUNUSED(v) (void) (v)       //add LTUNUSED(variable); to avoid compiler warnings 
#define USE_SPI_TRANSACTION

//#define SX128XDEBUG                //enable debug messages
//#define RANGINGDEBUG               //enable debug messages for ranging
//#define SX128XDEBUGRXTX            //enable debug messages for RX TX switching
//#define SX128XDEBUGPINS            //enable pin allocation debug messages
//#define SX128XDEBUGRELIABLE        //enable for debugging reliable and data transfer (DT) packets
//#define USEPAYLOADLENGTHREGISTER   //enable autoamtic setting of Payload length with register write
//#define DETECTRELIABLERRORS        //enable to improve error detect reliable errors such as incorrect packet size etc
#define REVISEDCHECKBUSY             //change to checkBusy() made November 2021 to improve speed, comment this #define out to use previous code.

//Changes November 2021
//Revised packet RSSI
//Added ranging RSSI
//Revised checkBusy() to improve performance
//Revised receive functions to preven lock up in receive mode on header error
//Revised receive functions to include failure on syncword fail, needed for relaible FLRC error detection
//Added begin() functions to allow for limited pin use
//Added setPayloadLength() to more easily use LoRa or FLRC packets
//Added option, by enabling #define USEPAYLOADLENGTHREGISTER, to set payload length via a discovered register write
//Added reliable packet functions
//Added data transfer functions



SX128XLT::SX128XLT()
{
}

/* Formats for :begin
  1 All pins > begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinRXEN, int8_t pinTXEN, uint8_t device)
  2 NiceRF   > begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, uint8_t device)
  3 Ebyte    > begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
  4 IRQ      > begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, uint8_t device)
  5 IRQ      > begin(int8_t pinNSS, int8_t pinRFBUSY, uint8_t device)
*/


bool SX128XLT::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinRXEN, int8_t pinTXEN, uint8_t device)
{

  //format 1 pins, assign all available pins
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _RFBUSY = pinRFBUSY;
  _DIO1 = pinDIO1;
  _DIO2 = pinDIO2;
  _DIO3 = pinDIO3;
  _RXEN = pinRXEN;
  _TXEN = pinTXEN;
  _Device = device;
  _TXDonePin = pinDIO1;        //this is defalt pin for sensing TX done
  _RXDonePin = pinDIO1;        //this is defalt pin for sensing RX done

  digitalWrite(_NSS, HIGH);
  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, LOW);
  pinMode(_RFBUSY, INPUT);


#ifdef SX128XDEBUGPINS
  Serial.println(F("1 begin()"));
  Serial.println(F("SX128XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("RFBUSY "));
  Serial.println(_RFBUSY);
  Serial.print(F("DIO1 "));
  Serial.println(_DIO1);
  Serial.print(F("DIO2 "));
  Serial.println(_DIO2);
  Serial.print(F("DIO3 "));
  Serial.println(_DIO3);
  Serial.print(F("RX_EN "));
  Serial.println(_RXEN);
  Serial.print(F("TX_EN "));
  Serial.println(_TXEN);
#endif

  if (_DIO1 >= 0)
  {
    pinMode( _DIO1, INPUT);
  }

  if (_DIO2 >= 0)
  {
    pinMode( _DIO2, INPUT);
  }

  if (_DIO3 >= 0)
  {
    pinMode( _DIO3, INPUT);
  }

  if ((_RXEN >= 0) && (_TXEN >= 0))
  {
#ifdef SX128XDEBUGPINS
    Serial.println(F("RX_EN & TX_EN switching enabled"));
#endif
    pinMode(_RXEN, OUTPUT);
    pinMode(_TXEN, OUTPUT);
    _rxtxpinmode = true;
  }
  else
  {
#ifdef SX128XDEBUGPINS
    Serial.println(F("RX_EN & TX_EN switching disabled"));
#endif
    _rxtxpinmode = false;
  }

  resetDevice();

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX128XLT::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, uint8_t device)
{
  //format 2 pins for NiceRF, NSS, NRESET, RFBUSY, DIO1
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _RFBUSY = pinRFBUSY;
  _DIO1 = pinDIO1;
  _DIO2 = -1;                  //not defined, so mark as unused
  _DIO3 = -1;                  //not defined, so mark as unused
  _RXEN = -1;                  //not defined, so mark as unused
  _TXEN = -1;                  //not defined, so mark as unused
  _Device = device;
  _TXDonePin = pinDIO1;        //this is defalt pin for sensing TX done
  _RXDonePin = pinDIO1;        //this is defalt pin for sensing RX done

  digitalWrite(_NSS, HIGH);
  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, LOW);
  pinMode(_RFBUSY, INPUT);

#ifdef SX128XDEBUGPINS
  Serial.println(F("2 begin()"));
  Serial.println(F("SX128XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("RFBUSY "));
  Serial.println(_RFBUSY);
  Serial.print(F("DIO1 "));
  Serial.println(_DIO1);
  Serial.print(F("DIO2 "));
  Serial.println(_DIO2);
  Serial.print(F("DIO3 "));
  Serial.println(_DIO3);
  Serial.print(F("RX_EN "));
  Serial.println(_RXEN);
  Serial.print(F("TX_EN "));
  Serial.println(_TXEN);
#endif

  if (_DIO1 >= 0)
  {
    pinMode( _DIO1, INPUT);
  }

#ifdef SX128XDEBUGPINS
  Serial.println(F("RX_EN & TX_EN switching disabled"));
#endif

  _rxtxpinmode = false;

  resetDevice();

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX128XLT::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinRXEN, int8_t pinTXEN, uint8_t device)
{
  //format 3 pins for Ebyte, NSS, NRESET, RFBUSY, DIO1, RX_EN, TX_EN
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _RFBUSY = pinRFBUSY;
  _DIO1 = pinDIO1;
  _DIO2 = -1;                  //not defined, so mark as unused
  _DIO3 = -1;                  //not defined, so mark as unused
  _RXEN = pinRXEN;
  _TXEN = pinTXEN;
  _Device = device;
  _TXDonePin = pinDIO1;        //this is defalt pin for sensing TX done
  _RXDonePin = pinDIO1;        //this is defalt pin for sensing RX done

  digitalWrite(_NSS, HIGH);
  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, LOW);
  pinMode(_RFBUSY, INPUT);

#ifdef SX128XDEBUGPINS
  Serial.println(F("3 begin()"));
  Serial.println(F("SX128XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("RFBUSY "));
  Serial.println(_RFBUSY);
  Serial.print(F("DIO1 "));
  Serial.println(_DIO1);
  Serial.print(F("DIO2 "));
  Serial.println(_DIO2);
  Serial.print(F("DIO3 "));
  Serial.println(_DIO3);
  Serial.print(F("RX_EN "));
  Serial.println(_RXEN);
  Serial.print(F("TX_EN "));
  Serial.println(_TXEN);
#endif

  if (_DIO1 >= 0)
  {
    pinMode( _DIO1, INPUT);
  }

  if ((_RXEN >= 0) && (_TXEN >= 0))
  {
#ifdef SX128XDEBUGPINS
    Serial.println(F("RX_EN & TX_EN switching enabled"));
#endif
    pinMode(_RXEN, OUTPUT);
    pinMode(_TXEN, OUTPUT);
    _rxtxpinmode = true;
  }
  else
  {
#ifdef SX128XDEBUGPINS
    Serial.println(F("RX_EN & TX_EN switching disabled"));
#endif
    _rxtxpinmode = false;
  }

  resetDevice();

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX128XLT::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, uint8_t device)
{
  //format 4 pins for IRQ use, NSS, NRESET, RFBUSY
  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _RFBUSY = pinRFBUSY;
  _DIO1 = -1;                  //not defined, so mark as unused
  _DIO2 = -1;                  //not defined, so mark as unused
  _DIO3 = -1;                  //not defined, so mark as unused
  _RXEN = -1;                  //not defined, so mark as unused
  _TXEN = -1;                  //not defined, so mark as unused
  _Device = device;
  _TXDonePin = -1;             //not defined, so mark as unused
  _RXDonePin = -1;             //not defined, so mark as unused

  digitalWrite(_NSS, HIGH);
  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, LOW);
  pinMode(_RFBUSY, INPUT);

#ifdef SX128XDEBUGPINS
  Serial.println(F("4 IRQ begin()"));
  Serial.println(F("SX128XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("RFBUSY "));
  Serial.println(_RFBUSY);
#endif

  _rxtxpinmode = false;
  resetDevice();

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool SX128XLT::begin(int8_t pinNSS, int8_t pinRFBUSY, uint8_t device)
{
  //Format 5 pins for IRQ use, NSS, RFBUSY.  Some form of additional external reset is needed for
  //the device, since this routine does not use resetDevice();
  //A pull up on NSS is recommended, a low to high transitions on NSS at startup can put the device
  //into a busy state, with no reset available to clear it.

  _NSS = pinNSS;
  _NRESET = -1;                //not defined, so mark as unused
  _RFBUSY = pinRFBUSY;
  _DIO1 = -1;                  //not defined, so mark as unused
  _DIO2 = -1;                  //not defined, so mark as unused
  _DIO3 = -1;                  //not defined, so mark as unused
  _RXEN = -1;                  //not defined, so mark as unused
  _TXEN = -1;                  //not defined, so mark as unused
  _Device = device;
  _TXDonePin = -1;             //not defined, so mark as unused
  _RXDonePin = -1;             //not defined, so mark as unused

  digitalWrite(_NSS, HIGH);    //make sure NSS state is high, before turning on output, a low to high transision here causes issues
  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_RFBUSY, INPUT);

#ifdef SX128XDEBUGPINS
  Serial.println(F("5 IRQ begin()"));
  Serial.println(F("SX128XLT constructor instantiated successfully"));
  Serial.print(F("NSS "));
  Serial.println(_NSS);
  Serial.print(F("NRESET "));
  Serial.println(_NRESET);
  Serial.print(F("RFBUSY "));
  Serial.println(_RFBUSY);
#endif

  _rxtxpinmode = false;

  if (checkDevice())
  {
    return true;
  }

  return false;
}


void SX128XLT::rxEnable()
{
  //Enable RX mode on device such as Ebyte E28-2G4M20S which have RX and TX enable pins
#ifdef SX128XDEBUGRXTX
  Serial.println(F("rxEnable()"));
#endif

  digitalWrite(_RXEN, HIGH);
  digitalWrite(_TXEN, LOW);
}


void SX128XLT::txEnable()
{
  //Enable RX mode on device such as Ebyte E28-2G4M20S which have RX and TX enable pins
#ifdef SX128XDEBUGRXTX
  Serial.println(F("txEnable()"));
#endif

  digitalWrite(_RXEN, LOW);
  digitalWrite(_TXEN, HIGH);
}


//Changes made November 2021 to improve performance of checkBusy();
//As published the revised check busy function will used
//To use the previous version of code, remove the #define REVISEDCHECKBUSY at the top of this file
#ifdef REVISEDCHECKBUSY
void SX128XLT::checkBusy()
{
#ifdef SX128XDEBUG
  Serial.println(F("checkBusy() Revised"));
#endif

  uint32_t startmS = millis();

  do
  {
    if ( ((uint32_t) (millis() - startmS) > 9))   //wait 10mS for busy to complete
    {
      Serial.println(F("ERROR - Busy Timeout!"));
      resetDevice();
      setMode(MODE_STDBY_RC);
      config();                                   //re-run saved config
      break;
    }
  } while (digitalRead(_RFBUSY));

}
#endif


#ifndef REVISEDCHECKBUSY
void SX128XLT::checkBusy()
{
#ifdef SX128XDEBUG
  Serial.println(F("checkBusy() Original"));
#endif

  uint8_t busy_timeout_cnt;
  busy_timeout_cnt = 0;

  while (digitalRead(_RFBUSY))
  {
    delay(1);
    busy_timeout_cnt++;

    if (busy_timeout_cnt > 10)                    //wait 10mS for busy to complete
    {
      Serial.println(F("ERROR - Busy Timeout!"));
      resetDevice();
      setMode(MODE_STDBY_RC);
      config();                                   //re-run saved config
      break;
    }
  }

}
#endif


bool SX128XLT::config()
{
#ifdef SX128XDEBUG
  Serial.println(F("config()"));
#endif

  resetDevice();
  setMode(MODE_STDBY_RC);
  setRegulatorMode(savedRegulatorMode);
  setPacketType(savedPacketType);
  setRfFrequency(savedFrequency, savedOffset);
  setModulationParams(savedModParam1, savedModParam2, savedModParam3);
  setPacketParams(savedPacketParam1, savedPacketParam2, savedPacketParam3, savedPacketParam4, savedPacketParam5, savedPacketParam6, savedPacketParam7);
  setDioIrqParams(savedIrqMask, savedDio1Mask, savedDio2Mask, savedDio3Mask);       //set for IRQ on RX done on DIO1
  setHighSensitivity();
  return true;
}


void SX128XLT::readRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
  uint16_t index;
  uint8_t addr_l, addr_h;

  addr_h = address >> 8;
  addr_l = address & 0x00FF;
  checkBusy();

#ifdef USE_SPI_TRANSACTION            //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_READ_REGISTER);
  SPI.transfer(addr_h);               //MSB
  SPI.transfer(addr_l);               //LSB
  SPI.transfer(0xFF);
  for (index = 0; index < size; index++)
  {
    *(buffer + index) = SPI.transfer(0xFF);
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

}


uint8_t SX128XLT::readRegister(uint16_t address)
{
  uint8_t data;

  readRegisters(address, &data, 1);
  return data;
}


void SX128XLT::writeRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
  uint8_t addr_l, addr_h;
  uint8_t i;

  addr_l = address & 0xff;
  addr_h = address >> 8;
  checkBusy();

#ifdef USE_SPI_TRANSACTION     //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_REGISTER);
  SPI.transfer(addr_h);        //MSB
  SPI.transfer(addr_l);        //LSB

  for (i = 0; i < size; i++)
  {
    SPI.transfer(buffer[i]);
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

}


void SX128XLT::writeRegister(uint16_t address, uint8_t value)
{
  writeRegisters( address, &value, 1 );
}


void SX128XLT::writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size)
{
#ifdef SX128XDEBUG
  //Serial.print(F("writeCommand() "));
  //Serial.println(Opcode, HEX);
#endif

  uint8_t index;
  checkBusy();

#ifdef USE_SPI_TRANSACTION                 //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(Opcode);

  for (index = 0; index < size; index++)
  {
    SPI.transfer(buffer[index]);
  }
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  if (Opcode != RADIO_SET_SLEEP)
  {
    checkBusy();
  }
}


void SX128XLT::readCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size)
{
#ifdef SX128XDEBUG
  //Serial.print(F("readCommand() "));
  //Serial.println(Opcode, HEX);
#endif

  uint8_t i;
  checkBusy();

#ifdef USE_SPI_TRANSACTION                   //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(Opcode);
  SPI.transfer(0xFF);

  for ( i = 0; i < size; i++ )
  {
    *(buffer + i) = SPI.transfer(0xFF);
  }
  digitalWrite(_NSS, HIGH);


#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


void SX128XLT::resetDevice()
{
#ifdef SX128XDEBUG
  Serial.println(F("resetDevice()"));
#endif

  //Note: in the IRQ TX and RX examples _NRESET is set to -1, if so dont attempt to toggle pin

  if (_NRESET >= 0)
  {
    delay(20);
    digitalWrite(_NRESET, LOW);
    delay(50);
    digitalWrite(_NRESET, HIGH);
    delay(20);
  }
}


bool SX128XLT::checkDevice()
{
  //check there is a device out there, writes a register and reads back
#ifdef SX128XDEBUG
  Serial.println(F("checkDevice()"));
#endif

  uint8_t Regdata1, Regdata2;
  Regdata1 = readRegister(0x0908);               //low byte of frequency setting
  writeRegister(0x0908, (Regdata1 + 1));
  Regdata2 = readRegister(0x0908);               //read changed value back
  writeRegister(0x0908, Regdata1);               //restore register to original value

  if (Regdata2 == (Regdata1 + 1))
  {
    return true;
  }
  else
  {
    return false;
  }
}


void SX128XLT::setupLoRa(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3)
{
#ifdef SX128XDEBUG
  Serial.println(F("setupLoRa()"));
#endif

  setMode(MODE_STDBY_RC);
  setRegulatorMode(USE_LDO);
  setPacketType(PACKET_TYPE_LORA);
  setRfFrequency(frequency, offset);
  setBufferBaseAddress(0, 0);
  setModulationParams(modParam1, modParam2, modParam3);
  setPacketParams(12, LORA_PACKET_VARIABLE_LENGTH, 255, LORA_CRC_ON, LORA_IQ_NORMAL, 0, 0);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);
  setHighSensitivity();
}


void SX128XLT::setupFLRC(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint32_t syncword)
{
  //see data sheet SX1280-1_V3.2 16.4 FLRC Modem: Increased PER in FLRC Packets with Synch
  //some patterns of syncword can create increased error rates in FLRC

#ifdef SX128XDEBUG
  Serial.println(F("setupFLRC()"));
#endif
  setMode(MODE_STDBY_RC);
  setRegulatorMode(USE_LDO);
  setPacketType(PACKET_TYPE_FLRC);
  setRfFrequency(frequency, offset);
  setBufferBaseAddress(0, 0);
  setModulationParams(modParam1, modParam2, modParam3);
  setPacketParams(PREAMBLE_LENGTH_32_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 127, RADIO_CRC_3_BYTES, RADIO_WHITENING_OFF);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);
  setSyncWord1(syncword);
  setHighSensitivity();
}


void SX128XLT::setMode(uint8_t modeconfig)
{
#ifdef SX128XDEBUG
  Serial.println(F("setMode()"));
#endif

  uint8_t Opcode = 0x80;

  checkBusy();

#ifdef USE_SPI_TRANSACTION               //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(Opcode);
  SPI.transfer(modeconfig);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  _OperatingMode = modeconfig;

}


void SX128XLT::setRegulatorMode(uint8_t mode)
{
#ifdef SX128XDEBUG
  Serial.println(F("setRegulatorMode()"));
#endif

  savedRegulatorMode = mode;

  writeCommand(RADIO_SET_REGULATORMODE, &mode, 1);
}


void SX128XLT::setPacketType(uint8_t packettype )
{
#ifdef SX128XDEBUG
  Serial.println(F("setPacketType()"));
#endif
  savedPacketType = packettype;

  writeCommand(RADIO_SET_PACKETTYPE, &packettype, 1);
}


void SX128XLT::setRfFrequency(uint32_t frequency, int32_t offset)
{
#ifdef SX128XDEBUG
  Serial.println(F("setRfFrequency()"));
#endif

  savedFrequency = frequency;
  savedOffset = offset;

  frequency = frequency + offset;
  uint8_t buffer[3];
  uint32_t freqtemp = 0;
  freqtemp = ( uint32_t )( (double) frequency / (double)FREQ_STEP);
  buffer[0] = ( uint8_t )( ( freqtemp >> 16 ) & 0xFF );
  buffer[1] = ( uint8_t )( ( freqtemp >> 8 ) & 0xFF );
  buffer[2] = ( uint8_t )( freqtemp & 0xFF );
  writeCommand(RADIO_SET_RFFREQUENCY, buffer, 3);
}


void SX128XLT::setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
#ifdef SX128XDEBUG
  Serial.println(F("setBufferBaseAddress()"));
#endif

  uint8_t buffer[2];

  buffer[0] = txBaseAddress;
  buffer[1] = rxBaseAddress;
  writeCommand(RADIO_SET_BUFFERBASEADDRESS, buffer, 2);
}


void SX128XLT::setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3)
{
  //sequence is spreading factor, bandwidth, coding rate.

#ifdef SX128XDEBUG
  Serial.println(F("setModulationParams()"));
#endif

  uint8_t buffer[3];

  savedModParam1 = modParam1;
  savedModParam2 = modParam2;
  savedModParam3 = modParam3;

  buffer[0] = modParam1;
  buffer[1] = modParam2;
  buffer[2] = modParam3;

  writeCommand(RADIO_SET_MODULATIONPARAMS, buffer, 3);

  //implement data sheet additions, datasheet SX1280-1_V3.2section 14.47

  writeRegister( 0x93C, 0x1 );

  switch (modParam1)
  {
    case LORA_SF5:
      writeRegister( 0x925, 0x1E );
      break;
    case LORA_SF6:
      writeRegister( 0x925, 0x1E );
      break;

    case LORA_SF7:
      writeRegister( 0x925, 0x37 );
      break;

    case LORA_SF8:
      writeRegister( 0x925, 0x37 );
      break;

    case LORA_SF9:
      writeRegister( 0x925, 0x32 );
      break;

    case LORA_SF10:
      writeRegister( 0x925, 0x32 );
      break;

    case LORA_SF11:
      writeRegister( 0x925, 0x32 );
      break;

    case LORA_SF12:
      writeRegister( 0x925, 0x32 );
      break;

    default:
      break;
  }
}


void SX128XLT::setPacketParams(uint8_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5, uint8_t packetParam6, uint8_t packetParam7)
{
  //This function is for FLRC mode
  //For FLRC: order is PreambleLength, SyncWordLength, SyncWordMatch, HeaderType, PayloadLength, CrcLength, Whitening

#ifdef SX128XDEBUG
  Serial.println(F("SetPacketParams(7)"));
#endif

  savedPacketParam1 = packetParam1;
  savedPacketParam2 = packetParam2;
  savedPacketParam3 = packetParam3;
  savedPacketParam4 = packetParam4;
  savedPacketParam5 = packetParam5;
  savedPacketParam6 = packetParam6;
  savedPacketParam7 = packetParam7;

  uint8_t buffer[7];
  buffer[0] = packetParam1;
  buffer[1] = packetParam2;
  buffer[2] = packetParam3;
  buffer[3] = packetParam4;
  buffer[4] = packetParam5;
  buffer[5] = packetParam6;
  buffer[6] = packetParam7;
  writeCommand(RADIO_SET_PACKETPARAMS, buffer, 7);
}


void SX128XLT::setPacketParams(uint8_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5)
{
  //This function is for LORa mode
  //For LoRa: order is PreambleLength, HeaderType, PayloadLength, CRC, InvertIQ/chirp invert

#ifdef SX128XDEBUG
  Serial.println(F("SetPacketParams(5)"));
#endif

  savedPacketParam1 = packetParam1;
  savedPacketParam2 = packetParam2;
  savedPacketParam3 = packetParam3;
  savedPacketParam4 = packetParam4;
  savedPacketParam5 = packetParam5;

  uint8_t buffer[7];
  buffer[0] = packetParam1;
  buffer[1] = packetParam2;
  buffer[2] = packetParam3;
  buffer[3] = packetParam4;
  buffer[4] = packetParam5;
  writeCommand(RADIO_SET_PACKETPARAMS, buffer, 5);
}


void SX128XLT::setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask )
{
#ifdef SX128XDEBUG
  Serial.println(F("setDioIrqParams()"));
#endif

  savedIrqMask = irqMask;
  savedDio1Mask = dio1Mask;
  savedDio2Mask = dio2Mask;
  savedDio3Mask = dio3Mask;

  uint8_t buffer[8];

  buffer[0] = (uint8_t) (irqMask >> 8);
  buffer[1] = (uint8_t) (irqMask & 0xFF);
  buffer[2] = (uint8_t) (dio1Mask >> 8);
  buffer[3] = (uint8_t) (dio1Mask & 0xFF);
  buffer[4] = (uint8_t) (dio2Mask >> 8);
  buffer[5] = (uint8_t) (dio2Mask & 0xFF);
  buffer[6] = (uint8_t) (dio3Mask >> 8);
  buffer[7] = (uint8_t) (dio3Mask & 0xFF);
  writeCommand(RADIO_SET_DIOIRQPARAMS, buffer, 8);
}


void SX128XLT::setHighSensitivity()
{
  //set bits 7,6 of REG_LNA_REGIME
#ifdef SX128XDEBUG
  Serial.println(F("setHighSensitivity()"));
#endif

  writeRegister(REG_LNA_REGIME, (readRegister(REG_LNA_REGIME) | 0xC0));
}


void SX128XLT::setLowPowerRX()
{
  //clear bits 7,6 of REG_LNA_REGIME
#ifdef SX128XDEBUG
  Serial.println(F("setLowPowerRX()"));
#endif

  writeRegister(REG_LNA_REGIME, (readRegister(REG_LNA_REGIME) & 0x3F));
}


void SX128XLT::printModemSettings()
{
  //modified November 2021 to provide more details on FLRC
#ifdef SX128XDEBUG
  Serial.println(F("printModemSettings()"));
#endif

  printDevice();

  Serial.print(F(",PACKET_TYPE_"));

  switch (savedPacketType)
  {
    case PACKET_TYPE_GFSK:
      Serial.print(F("GFSK"));
      break;

    case PACKET_TYPE_LORA:
      Serial.print(F("LORA"));
      break;

    case PACKET_TYPE_RANGING:
      Serial.print(F("RANGING"));
      break;

    case PACKET_TYPE_FLRC:
      Serial.print(F("FLRC"));
      break;

    case PACKET_TYPE_BLE:
      Serial.print(F("BLE"));
      break;

    default:
      Serial.print(F("Unknown"));
  }

  Serial.print(F(","));
  Serial.print(getFreqInt());
  Serial.print(F("hz"));

  if ((savedPacketType == PACKET_TYPE_LORA) || (savedPacketType == PACKET_TYPE_RANGING))
  {
    Serial.print(F(",SF"));
    Serial.print(getLoRaSF());
    Serial.print(F(",BW"));
    Serial.print(returnBandwidth(savedModParam2));
    Serial.print(F(",CR4:"));
    Serial.print((getLoRaCodingRate() + 4));
  }

  if (savedPacketType == PACKET_TYPE_FLRC)
  {
    Serial.print(F(",BandwidthBitRate_"));
    switch (savedModParam1)
    {
      case FLRC_BR_1_300_BW_1_2:
        Serial.print(F("FLRC_BR_1_300_BW_1_2"));
        break;

      case FLRC_BR_1_000_BW_1_2:
        Serial.print(F("FLRC_BR_1_000_BW_1_2"));
        break;

      case FLRC_BR_0_650_BW_0_6:
        Serial.print(F("FLRC_BR_0_650_BW_0_6"));
        break;

      case FLRC_BR_0_520_BW_0_6:
        Serial.print(F("FLRC_BR_0_520_BW_0_6"));
        break;

      case FLRC_BR_0_325_BW_0_3:
        Serial.print(F("FLRC_BR_0_325_BW_0_3"));
        break;

      case FLRC_BR_0_260_BW_0_3:
        Serial.print(F("FLRC_BR_0_260_BW_0_3"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    Serial.print(F(",CodingRate_"));
    switch (savedModParam2)
    {
      case FLRC_CR_1_2:
        Serial.print(F("CR_1_2"));
        break;

      case FLRC_CR_3_4:
        Serial.print(F("CR_3_4"));
        break;

      case FLRC_CR_1_0:
        Serial.print(F("CR_1_0"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    Serial.print(F(",BT_"));
    switch (savedModParam3)
    {
      case BT_DIS:
        Serial.print(F("NoFiltering"));
        break;

      case BT_1:
        Serial.print(F("1"));
        break;

      case BT_0_5:
        Serial.print(F("0_5"));
        break;

      default:
        Serial.print(F("Unknown"));
    }
  }
}


void SX128XLT::printDevice()
{
#ifdef SX128XDEBUG
  Serial.println(F("printDevice()"));
#endif

  switch (_Device)
  {
    case DEVICE_SX1280:
      Serial.print(F("SX1280"));
      break;

    case DEVICE_SX1281:
      Serial.print(F("SX1281"));
      break;

    default:
      Serial.print(F("Unknown Device"));
  }
}


uint32_t SX128XLT::getFreqInt()
{

#ifdef SX128XDEBUG
  Serial.println(F("getFreqInt"));
#endif

  //get the current set device frequency, return as long integer
  uint8_t Msb = 0;
  uint8_t Mid = 0;
  uint8_t Lsb = 0;

  uint32_t uinttemp;
  float floattemp;

  LTUNUSED(Msb);           //to prevent a compiler warning
  LTUNUSED(Mid);           //to prevent a compiler warning
  LTUNUSED(Lsb);           //to prevent a compiler warning

  if (savedPacketType == PACKET_TYPE_LORA)
  {
    Msb = readRegister(REG_RFFrequency23_16);
    Mid = readRegister(REG_RFFrequency15_8);
    Lsb = readRegister(REG_RFFrequency7_0);
  }

  if (savedPacketType == PACKET_TYPE_RANGING)
  {
    Msb = readRegister(REG_RFFrequency23_16);
    Mid = readRegister(REG_RFFrequency15_8);
    Lsb = readRegister(REG_RFFrequency7_0);
  }

  if (savedPacketType == PACKET_TYPE_FLRC)
  {
    Msb = readRegister(REG_FLRC_RFFrequency23_16);
    Mid = readRegister(REG_FLRC_RFFrequency15_8);
    Lsb = readRegister(REG_FLRC_RFFrequency7_0);
  }

  floattemp = ((Msb * 0x10000ul) + (Mid * 0x100ul) + Lsb);
  floattemp = ((floattemp * FREQ_STEP) / 1000000ul);
  uinttemp = (uint32_t)(floattemp * 1000000);
  return uinttemp;
}


uint8_t SX128XLT::getLoRaSF()
{
#ifdef SX128XDEBUG
  Serial.println(F("getLoRaSF()"));
#endif
  return (savedModParam1 >> 4);
}


uint32_t SX128XLT::returnBandwidth(uint8_t data)
{
#ifdef SX128XDEBUG
  Serial.println(F("returnBandwidth()"));
#endif

  switch (data)
  {
    case LORA_BW_0200:
      return 203125;

    case LORA_BW_0400:
      return 406250;

    case LORA_BW_0800:
      return 812500;

    case LORA_BW_1600:
      return 1625000;

    default:
      break;
  }

  return 0x0;                      //so that a bandwidth not set can be identified
}


uint8_t SX128XLT::getLoRaCodingRate()
{
#ifdef SX128XDEBUG
  Serial.println(F("getLoRaCodingRate"));
#endif

  return savedModParam3;
}


uint8_t SX128XLT::getInvertIQ()
{
  //IQ mode reg 0x33
#ifdef SX128XDEBUG
  Serial.println(F("getInvertIQ"));
#endif

  return savedPacketParam5;
}


uint16_t SX128XLT::getPreamble()
{
#ifdef SX128XDEBUG
  Serial.println(F("getPreamble"));
#endif

  return savedPacketParam1;
}


void SX128XLT::printOperatingSettings()
{
  //modified November 2021 to provide more details on FLRC
#ifdef SX128XDEBUG
  Serial.println(F("printOperatingSettings()"));
#endif

  printDevice();

  Serial.print(F(",PACKET_TYPE_"));

  switch (savedPacketType)
  {
    case PACKET_TYPE_GFSK:
      Serial.print(F("GFSK"));
      break;

    case PACKET_TYPE_LORA:
      Serial.print(F("LORA"));
      break;

    case PACKET_TYPE_RANGING:
      Serial.print(F("RANGING"));
      break;

    case PACKET_TYPE_FLRC:
      Serial.print(F("FLRC"));
      break;


    case PACKET_TYPE_BLE:
      Serial.print(F("BLE"));
      break;

    default:
      Serial.print(F("Unknown"));
  }

  if ((savedPacketType == PACKET_TYPE_LORA) || (savedPacketType == PACKET_TYPE_RANGING))
  {
    Serial.print(F(",Preamble_"));
    Serial.print(savedPacketParam1);

    switch (savedPacketParam2)
    {
      case LORA_PACKET_VARIABLE_LENGTH:
        Serial.print(F(",Explicit"));
        break;

      case LORA_PACKET_FIXED_LENGTH:
        Serial.print(F(",Implicit"));
        break;

      default:
        Serial.print(F(",Unknown"));
    }

    Serial.print(F(",PayloadL_"));
    Serial.print(savedPacketParam3);   //savedPacketParam3 is payload length
    Serial.print(F(",CRC_"));

    switch (savedPacketParam4)
    {
      case LORA_CRC_ON:
        Serial.print(F("ON"));
        break;

      case LORA_CRC_OFF:
        Serial.print(F("OFF"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    Serial.print(F(",IQ_"));

    switch (savedPacketParam5)
    {
      case LORA_IQ_INVERTED:
        Serial.print(F("INVERTED"));
        break;

      case LORA_IQ_NORMAL:
        Serial.print(F("NORMAL"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    Serial.print(F(",LNAgain_"));

    if (getLNAgain() == 0xC0)
    {
      Serial.print(F("HighSensitivity"));
    }
    else
    {
      Serial.print(F("LowPowerRX"));
    }
  }


  if (savedPacketType == PACKET_TYPE_FLRC)
  {
    Serial.print(F(",Preamble_"));
    Serial.print(((savedPacketParam1 >> 4) * 4) + 4);
    Serial.print(F("_BITS"));
    Serial.print(F(",SyncWordLength_"));
    Serial.print(savedPacketParam2);
    Serial.print(F(",SyncWordMatch_"));

    switch (savedPacketParam3)
    {
      case RADIO_RX_MATCH_SYNCWORD_1:
        Serial.print(F("1"));
        break;

      case RADIO_RX_MATCH_SYNCWORD_2:
        Serial.print(F("2"));
        break;

      case RADIO_RX_MATCH_SYNCWORD_1_2:
        Serial.print(F("1_2"));
        break;

      case RADIO_RX_MATCH_SYNCWORD_3:
        Serial.print(F("3"));
        break;

      case RADIO_RX_MATCH_SYNCWORD_1_3:
        Serial.print(F("1_3"));
        break;

      case RADIO_RX_MATCH_SYNCWORD_2_3:
        Serial.print(F("2_3"));
        break;

      case RADIO_RX_MATCH_SYNCWORD_1_2_3:
        Serial.print(F("1_2_3"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    switch (savedPacketParam4)
    {
      case RADIO_PACKET_VARIABLE_LENGTH:
        Serial.print(F(",VariableLengthPacket"));
        break;

      case RADIO_PACKET_FIXED_LENGTH:
        Serial.print(F(",FixedLengthPacket"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    Serial.print(F(",PayloadLength_"));
    Serial.print(savedPacketParam5);
    Serial.print(F(",CRC_"));
    Serial.print(savedPacketParam6 >> 4);
    Serial.print(F("_Bytes"));
    Serial.print(F(",Whitening_"));

    switch (savedPacketParam7)
    {
      case RADIO_WHITENING_OFF:
        Serial.print(F("OFF"));
        break;

      case RADIO_WHITENING_ON:
        Serial.print(F("ON"));
        break;

      default:
        Serial.print(F("Unknown"));
    }

    Serial.print(F(",LNAgain_"));

    if (getLNAgain() == 0xC0)
    {
      Serial.print(F("HighSensitivity"));
    }
    else
    {
      Serial.print(F("LowPowerRX"));
    }
  }
}


uint8_t SX128XLT::getLNAgain()
{
#ifdef SX128XDEBUG
  Serial.println(F("getLNAgain"));
#endif

  return (readRegister(REG_LNA_REGIME) & 0xC0);
}


void SX128XLT::printRegisters(uint16_t Start, uint16_t End)
{
  //prints the contents of SX1280 registers to serial monitor

#ifdef SX128XDEBUG
  Serial.println(F("printRegisters()"));
#endif

  uint16_t Loopv1, Loopv2, RegData;

  Serial.print(F("Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Serial.println();

  for (Loopv1 = Start; Loopv1 <= End;)           //32 lines
  {
    Serial.print(F("0x"));
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


void SX128XLT::printASCIIPacket(uint8_t *buffer, uint8_t size)
{
#ifdef SX128XDEBUG
  Serial.println(F("printASCIIPacket()"));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    Serial.write(buffer[index]);
  }

}


uint8_t SX128XLT::transmit(uint8_t *txbuffer, uint8_t size, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("transmit()"));
#endif
  uint8_t index;
  uint8_t bufferdata;

  if (size == 0)
  {
    return false;
  }

  setMode(MODE_STDBY_RC);
  checkBusy();

#ifdef USE_SPI_TRANSACTION          //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(timeout);                                                            //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                    //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )            //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


uint8_t SX128XLT::transmitIRQ(uint8_t *txbuffer, uint8_t size, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("transmitIRQ()"));
#endif
  uint8_t index;
  uint8_t bufferdata;

  if (size == 0)
  {
    return false;
  }

  setMode(MODE_STDBY_RC);
  checkBusy();

#ifdef USE_SPI_TRANSACTION     //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setTx(timeout);                                      //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  //0x4001   = IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT
  while (!(readIrqStatus() & 0x4001 ));   //wait for IRQs going active

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )            //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


void SX128XLT::setTxParams(int8_t TXpower, uint8_t RampTime)
{
#ifdef SX128XDEBUG
  Serial.println(F("setTxParams()"));
#endif

  uint8_t buffer[2];

  savedTXPower = TXpower;

  //power register is set to 0 to 31 which is -18dBm to +12dBm
  buffer[0] = (TXpower + 18);
  buffer[1] = (uint8_t)RampTime;
  writeCommand(RADIO_SET_TXPARAMS, buffer, 2);
}


void SX128XLT::setTx(uint16_t timeout)
{

#ifdef SX128XDEBUG
  Serial.println(F("setTx()"));
#endif

  if (_rxtxpinmode)
  {
    txEnable();
  }

  uint8_t buffer[3];

  clearIrqStatus(IRQ_RADIO_ALL);                             //clear all interrupt flags
  buffer[0] = _PERIODBASE;
  buffer[1] = ( uint8_t )( ( timeout >> 8 ) & 0x00FF );
  buffer[2] = ( uint8_t )( timeout & 0x00FF );
  writeCommand(RADIO_SET_TX, buffer, 3 );
}


void SX128XLT::clearIrqStatus(uint16_t irqMask)
{
#ifdef SX128XDEBUG
  Serial.println(F("clearIrqStatus()"));
#endif

  uint8_t buffer[2];

  buffer[0] = (uint8_t) (irqMask >> 8);
  buffer[1] = (uint8_t) (irqMask & 0xFF);
  writeCommand(RADIO_CLR_IRQSTATUS, buffer, 2);
}


uint16_t SX128XLT::readIrqStatus()
{
#ifdef SX128XDEBUG
  Serial.print(F("readIrqStatus()"));
#endif

  uint16_t temp;
  uint8_t buffer[2];

  readCommand(RADIO_GET_IRQSTATUS, buffer, 2);
  temp = ((buffer[0] << 8) + buffer[1]);
  return temp;
}


void SX128XLT::printIrqStatus()
{
#ifdef SX128XDEBUG
  Serial.println(F("printIrqStatus()"));
#endif

  uint16_t _IrqStatus;
  _IrqStatus = readIrqStatus();

  //0x0001
  if (_IrqStatus & IRQ_TX_DONE)
  {
    Serial.print(F(",IRQ_TX_DONE"));
  }

  //0x0002
  if (_IrqStatus & IRQ_RX_DONE)
  {
    Serial.print(F(",IRQ_RX_DONE"));
  }

  //0x0004
  if (_IrqStatus & IRQ_SYNCWORD_VALID)
  {
    Serial.print(F(",IRQ_SYNCWORD_VALID"));
  }

  //0x0008
  if (_IrqStatus & IRQ_SYNCWORD_ERROR)
  {
    Serial.print(F(",IRQ_SYNCWORD_ERROR"));
  }

  //0x0010
  if (_IrqStatus & IRQ_HEADER_VALID)
  {
    Serial.print(F(",IRQ_HEADER_VALID"));
  }

  //0x0020
  if (_IrqStatus & IRQ_HEADER_ERROR)
  {
    Serial.print(F(",IRQ_HEADER_ERROR"));
  }

  //0x0040
  if (_IrqStatus & IRQ_CRC_ERROR)
  {
    Serial.print(F(",IRQ_CRC_ERROR"));
  }

  //0x0080
  if (_IrqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE)
  {
    Serial.print(F(",IRQ_RANGING_SLAVE_RESPONSE_DONE"));
  }

  //0x0100
  if (_IrqStatus & IRQ_RANGING_SLAVE_REQUEST_DISCARDED)
  {
    Serial.print(F(",IRQ_RANGING_SLAVE_REQUEST_DISCARDED"));
  }

  //0x0200
  if (_IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID)
  {
    Serial.print(F(",IRQ_RANGING_MASTER_RESULT_VALID"));
  }

  //0x0400
  if (_IrqStatus & IRQ_RANGING_MASTER_RESULT_TIMEOUT)
  {
    Serial.print(F(",IRQ_RANGING_MASTER_RESULT_TIMEOUT"));
  }

  //0x0800
  if (_IrqStatus & IRQ_RANGING_SLAVE_REQUEST_VALID)
  {
    Serial.print(F(",IRQ_RANGING_SLAVE_REQUEST_VALID"));
  }

  //0x1000
  if (_IrqStatus & IRQ_CAD_DONE)
  {
    Serial.print(F(",IRQ_CAD_DONE"));
  }

  //0x2000
  if (_IrqStatus & IRQ_CAD_ACTIVITY_DETECTED)
  {
    Serial.print(F(",IRQ_CAD_ACTIVITY_DETECTED"));
  }

  //0x4000
  if (_IrqStatus & IRQ_RX_TX_TIMEOUT)
  {
    Serial.print(F(",IRQ_RX_TX_TIMEOUT"));
  }

  //0x8000
  if (_IrqStatus & IRQ_PREAMBLE_DETECTED)
  {
    Serial.print(F(",IRQ_PREAMBLE_DETECTED"));
  }
}


uint16_t SX128XLT::CRCCCITT(uint8_t *buffer, uint32_t size, uint16_t start)
{
#ifdef SX128XDEBUG
  Serial.println(F("CRCCCITT()"));
#endif

  uint16_t index, libraryCRC;
  uint8_t j;

  libraryCRC = start;                                  //start value for CRC16

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


uint8_t SX128XLT::receive(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("receive()"));
#endif

  uint8_t index, RXstart, RXend;
  uint16_t regdata;
  uint8_t buffer[2];

  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(timeout);

  if (!wait)
  {
    return 0;                          //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));    //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);              //ensure to stop further packet reception
  regdata = readIrqStatus();

  if ( (regdata & IRQ_HEADER_ERROR) | (regdata & IRQ_CRC_ERROR) | (regdata & IRQ_RX_TX_TIMEOUT ) | (regdata & IRQ_SYNCWORD_ERROR )) //check if any of the preceding IRQs is set
  {
    return 0;                          //packet is errored somewhere so return 0
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL > size)               //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                 //truncate packet if not enough space
  }

  RXstart = buffer[1];
  RXend = RXstart + _RXPacketL;
  checkBusy();

#ifdef USE_SPI_TRANSACTION             //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);             //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

  for (index = RXstart; index < RXend; index++)
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


uint8_t SX128XLT::receiveIRQ(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("receiveIRQ()"));
#endif

  uint8_t index, RXstart, RXend;
  uint16_t regdata;
  uint8_t buffer[2];

  setRx(timeout);

  if (!wait)
  {
    return 0;                                                               //not wait requested so no packet length to pass
  }

  //0x4022   = IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR
  while (!(readIrqStatus() & 0x4022 ));   //wait for IRQs going active

  setMode(MODE_STDBY_RC);                                                   //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                          //packet is errored somewhere so return 0
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL > size)               //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                 //truncate packet if not enough space
  }

  RXstart = buffer[1];
  RXend = RXstart + _RXPacketL;

  checkBusy();

#ifdef USE_SPI_TRANSACTION              //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);              //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

  for (index = RXstart; index < RXend; index++)
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


int16_t SX128XLT::readPacketRSSI2()
{
#ifdef SX128XDEBUG
  Serial.println(F("readPacketRSSI2()"));
#endif

  uint8_t status[5];
  int16_t rssi;

  readCommand(RADIO_GET_PACKETSTATUS, status, 5) ;
  rssi = -status[0] / 2;

  return rssi;
}


int16_t SX128XLT::readPacketRSSI()
{
#ifdef SX128XDEBUG
  Serial.println(F("readPacketRSSI()"));
#endif

  int8_t snr;
  int16_t rssi = 0;                      //so routine returns 0 if packet not LoRa or FLRC
  uint8_t status[5];

  snr = readPacketSNR();
  readCommand(RADIO_GET_PACKETSTATUS, status, 5) ;

  if (savedPacketType == PACKET_TYPE_LORA)
  {
    rssi = -status[0] / 2;
    if (snr < 0)
    {
      rssi =  rssi +  snr;
    }
  }

  if (savedPacketType == PACKET_TYPE_FLRC)
  {
    rssi = -status[1] / 2;
  }

  return rssi;
}


int8_t SX128XLT::readPacketSNR()
{
#ifdef SX128XDEBUG
  Serial.println(F("readPacketSNR()"));
#endif

  uint8_t status[5];
  int8_t snr;

  readCommand(RADIO_GET_PACKETSTATUS, status, 5) ;

  if ( status[1] < 128 )
  {
    snr = status[1] / 4 ;
  }
  else
  {
    snr = (( status[1] - 256 ) / 4);
  }

  return snr;
}


uint8_t SX128XLT::readRXPacketL()
{
#ifdef SX128XDEBUG
  Serial.println(F("readRXPacketL()"));
#endif

  uint8_t buffer[2];

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];
  return _RXPacketL;
}


void SX128XLT::setRx(uint16_t timeout)
{
#ifdef SX128XDEBUG
  Serial.println(F("setRx()"));
#endif

  uint8_t buffer[3];

  if (_rxtxpinmode)
  {
    rxEnable();
  }

  clearIrqStatus(IRQ_RADIO_ALL);                             //clear all interrupt flags
  buffer[0] = _PERIODBASE;                                   //use pre determined period base setting
  buffer[1] = ( uint8_t ) ((timeout >> 8 ) & 0x00FF);
  buffer[2] = ( uint8_t ) (timeout & 0x00FF);
  writeCommand(RADIO_SET_RX, buffer, 3);
}


void SX128XLT::setPeriodBase(uint8_t value)
{
#ifdef SX128XDEBUG
  Serial.println(F("setPeriodBase"));
#endif

  _PERIODBASE = value;
}


uint8_t SX128XLT::getPeriodBase()
{
#ifdef SX128XDEBUG
  Serial.println(F("getPeriodBase"));
#endif

  return _PERIODBASE;
}


void SX128XLT::setSyncWord1(uint32_t syncword)
{
#ifdef SX128XDEBUG1
  Serial.println(F("setSyncWord1()"));
#endif

  //For FLRC packet type. The SyncWord is one byte shorter and
  //the base address is shifted by one byte versus GFSK packets
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR, ( syncword >> 24 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR + 1, ( syncword >> 16 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR + 2, ( syncword >> 8 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR + 3, syncword & 0x000000FF );
}


void SX128XLT::setSyncWord2(uint32_t syncword)
{
#ifdef SX128XDEBUG1
  Serial.println(F("setSyncWord2()"));
#endif

  //For FLRC packet type. The SyncWord is one byte shorter and
  //the base address is shifted by one byte versus GFSK packets
  writeRegister( REG_FLRCSYNCWORD2_BASEADDR, ( syncword >> 24 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD2_BASEADDR + 1, ( syncword >> 16 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD2_BASEADDR + 2, ( syncword >> 8 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD2_BASEADDR + 3, syncword & 0x000000FF );
}


void SX128XLT::setSyncWord3(uint32_t syncword)
{
#ifdef SX128XDEBUG1
  Serial.println(F("setSyncWord3()"));
#endif

  //For FLRC packet type. The SyncWord is one byte shorter and
  //the base address is shifted by one byte versus GFSK packets
  writeRegister( REG_FLRCSYNCWORD3_BASEADDR, ( syncword >> 24 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD3_BASEADDR + 1, ( syncword >> 16 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD3_BASEADDR + 2, ( syncword >> 8 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD3_BASEADDR + 3, syncword & 0x000000FF );
}


void SX128XLT::setSyncWordErrors(uint8_t errors)
{
#ifdef SX128XDEBUG1
  Serial.println(F("setSyncWordErrors()"));
#endif

  writeRegister( REG_LR_SYNCWORDTOLERANCE, errors );
}


void SX128XLT::setSleep(uint8_t sleepconfig)
{
#ifdef SX128XDEBUG
  Serial.println(F("setSleep()"));
#endif

  setMode(MODE_STDBY_RC);
  checkBusy();

#ifdef USE_SPI_TRANSACTION           //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  //need to save registers to device RAM first
  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_SET_SAVECONTEXT);
  digitalWrite(_NSS, HIGH);

  checkBusy();

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_SET_SLEEP);
  SPI.transfer(sleepconfig);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
  delay(1);                            //allow time for shutdown
}


void SX128XLT::printHEXByte(uint8_t temp)
{
  if (temp < 0x10)
  {
    Serial.print(F("0"));
  }
  Serial.print(temp, HEX);
}


void SX128XLT::wake()
{
#ifdef SX128XDEBUG
  Serial.println(F("wake()"));
#endif

  digitalWrite(_NSS, LOW);
  delay(1);
  digitalWrite(_NSS, HIGH);
  delay(1);
}


int32_t SX128XLT::getFrequencyErrorRegValue()
{
#ifdef SX128XDEBUG
  Serial.println(F("getFrequencyErrorRegValue()"));
#endif

  int32_t FrequencyError;
  uint32_t regmsb, regmid, reglsb, allreg;

  setMode(MODE_STDBY_XOSC);

  regmsb = readRegister( REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB );
  regmsb = regmsb & 0x0F;               //clear bit 20 which is always set
  regmid = readRegister( REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB + 1 );
  reglsb = readRegister( REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB + 2 );

  setMode(MODE_STDBY_RC);

#ifdef LORADEBUG
  Serial.println();
  Serial.print(F("Registers "));
  Serial.print(regmsb, HEX);
  Serial.print(F(" "));
  Serial.print(regmid, HEX);
  Serial.print(F(" "));
  Serial.println(reglsb, HEX);
#endif

  allreg = (uint32_t) ( regmsb << 16 ) | ( regmid << 8 ) | reglsb;

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


int32_t SX128XLT::getFrequencyErrorHz()
{
#ifdef SX128XDEBUG
  Serial.println(F("getFrequencyErrorHz()"));
#endif

  int32_t error, regvalue;
  uint32_t bandwidth;
  float divider;

  bandwidth = returnBandwidth(savedModParam2);                   //gets the last configured bandwidth
  divider = (float) 1625000 / bandwidth;                         //data sheet says 1600000, but bandwidth is 1625000
  regvalue = getFrequencyErrorRegValue();
  error = (FREQ_ERROR_CORRECTION * regvalue) / divider;

  return error;
}


uint8_t SX128XLT::transmitAddressed(uint8_t *txbuffer, uint8_t size, char txpackettype, char txdestination, char txsource, uint32_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("transmitAddressed()"));
#endif
  uint8_t index;
  uint8_t bufferdata;

  if (size == 0)
  {
    return false;
  }

  setMode(MODE_STDBY_RC);
  checkBusy();

#ifdef USE_SPI_TRANSACTION                        //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);

  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(timeout);                                                            //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                                //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                          //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


uint8_t SX128XLT::receiveAddressed(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("receiveAddressed()"));
#endif

  uint8_t index, RXstart, RXend;
  uint16_t regdata;
  uint8_t buffer[2];

  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(timeout);

  if (!wait)
  {
    return 0;                          //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));    //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);              //ensure to stop further packet reception
  regdata = readIrqStatus();

  if ( (regdata & IRQ_HEADER_ERROR) | (regdata & IRQ_CRC_ERROR) | (regdata & IRQ_RX_TX_TIMEOUT ) | (regdata & IRQ_SYNCWORD_ERROR )) //check if any of the preceding IRQs is set
  {
    return 0;                          //packet is errored somewhere so return 0
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL > size)               //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                 //truncate packet if not enough space
  }

  RXstart = buffer[1];
  RXend = RXstart + _RXPacketL;
  checkBusy();

#ifdef USE_SPI_TRANSACTION              //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);              //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

  _RXPacketType = SPI.transfer(0);
  _RXDestination = SPI.transfer(0);
  _RXSource = SPI.transfer(0);

  for (index = RXstart; index < RXend; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;                       //so we can check for packet having enough buffer space
}


uint8_t SX128XLT::readRXPacketType()
{
#ifdef SX128XDEBUG
  Serial.println(F("readRXPacketType()"));
#endif

  return _RXPacketType;
}


uint8_t SX128XLT::readPacket(uint8_t *rxbuffer, uint8_t size)
{
#ifdef SX128XDEBUG
  Serial.println(F("readPacket()"));
#endif

  uint8_t index, regdata, RXstart, RXend;
  uint8_t buffer[2];

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL > size)                 //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                   //truncate packet if not enough space
  }

  RXstart = buffer[1];
  RXend = RXstart + _RXPacketL;
  checkBusy();

#ifdef USE_SPI_TRANSACTION               //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);               //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

  for (index = RXstart; index < RXend; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;                       //so we can check for packet having enough buffer space
}


void SX128XLT::printHEXPacket(uint8_t *buffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("printHEXPacket() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByte(buffer[index]);
    Serial.print(F(" "));
  }
}


void SX128XLT::printHEXPacket(char *buffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("printHEXPacket() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByte(buffer[index]);
    Serial.print(F(" "));
  }
}


void SX128XLT::printArrayHEX(uint8_t *buffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("printArrayHEX() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByte(buffer[index]);
    Serial.print(F(" "));
  }
}


void SX128XLT::printArrayHEX(char *buffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("printArrayHEX() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByte(buffer[index]);
    Serial.print(F(" "));
  }
}


//***********************************************************************************
//direct access SX buffer routines
//these routines read\write variables\data direct to the LoRa devices internal buffer
//***********************************************************************************

void SX128XLT::startWriteSXBuffer(uint8_t ptr)
{
#ifdef SX128XDEBUG
  Serial.println(F("startWriteSXBuffer()"));
#endif

  _TXPacketL = 0;                   //this variable used to keep track of bytes written
  setMode(MODE_STDBY_RC);
  setBufferBaseAddress(ptr, 0);     //TX,RX
  checkBusy();

#ifdef USE_SPI_TRANSACTION          //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(ptr);                //address in SX buffer to write to
  //SPI interface ready for byte to write to buffer
}


uint8_t  SX128XLT::endWriteSXBuffer()
{
#ifdef SX128XDEBUG
  Serial.println(F("endWriteSXBuffer()"));
#endif

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _TXPacketL;
}


void SX128XLT::startReadSXBuffer(uint8_t ptr)
{
#ifdef SX128XDEBUG
  Serial.println(F("startReadSXBuffer"));
#endif

  _RXPacketL = 0;

  checkBusy();

#ifdef USE_SPI_TRANSACTION             //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);               //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(ptr);
  SPI.transfer(0xFF);

  //next line would be data = SPI.transfer(0);
  //SPI interface ready for byte to read from
}


uint8_t SX128XLT::endReadSXBuffer()
{
#ifdef SX128XDEBUG
  Serial.println(F("endReadSXBuffer()"));
#endif

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}


void SX128XLT::writeUint8(uint8_t x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeUint8()"));
#endif

  SPI.transfer(x);
  _TXPacketL++;                      //increment count of bytes written
}

uint8_t SX128XLT::readUint8()
{
#ifdef SX128XDEBUG
  Serial.println(F("readUint8()"));
#endif
  uint8_t x;

  x = SPI.transfer(0);
  _RXPacketL++;                      //increment count of bytes read
  return (x);
}


void SX128XLT::writeInt8(int8_t x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeInt8()"));
#endif

  SPI.transfer(x);

  _TXPacketL++;                      //increment count of bytes written
}


int8_t SX128XLT::readInt8()
{
#ifdef SX128XDEBUG
  Serial.println(F("readInt8()"));
#endif
  int8_t x;

  x = SPI.transfer(0);

  _RXPacketL++;                      //increment count of bytes read
  return (x);
}


void SX128XLT::writeInt16(int16_t x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeInt16()"));
#endif

  SPI.transfer(lowByte(x));
  SPI.transfer(highByte(x));

  _TXPacketL = _TXPacketL + 2;        //increment count of bytes written
}


int16_t SX128XLT::readInt16()
{
#ifdef SX128XDEBUG
  Serial.println(F("readInt16()"));
#endif
  uint8_t lowbyte, highbyte;

  lowbyte = SPI.transfer(0);
  highbyte = SPI.transfer(0);

  _RXPacketL = _RXPacketL + 2;         //increment count of bytes read
  return ((highbyte << 8) + lowbyte);
}


void SX128XLT::writeUint16(uint16_t x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeUint16()"));
#endif

  SPI.transfer(lowByte(x));
  SPI.transfer(highByte(x));

  _TXPacketL = _TXPacketL + 2;         //increment count of bytes written
}


uint16_t SX128XLT::readUint16()
{
#ifdef SX128XDEBUG
  Serial.println(F("writeUint16()"));
#endif
  uint8_t lowbyte, highbyte;

  lowbyte = SPI.transfer(0);
  highbyte = SPI.transfer(0);

  _RXPacketL = _RXPacketL + 2;         //increment count of bytes read
  return ((highbyte << 8) + lowbyte);
}


void SX128XLT::writeInt32(int32_t x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeInt32()"));
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


int32_t SX128XLT::readInt32()
{
#ifdef SX128XDEBUG
  Serial.println(F("readInt32()"));
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


void SX128XLT::writeUint32(uint32_t x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeUint32()"));
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


uint32_t SX128XLT::readUint32()
{
#ifdef SX128XDEBUG
  Serial.println(F("readUint32()"));
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
  _RXPacketL = _RXPacketL + 4;          //increment count of bytes read
  return readdata.f;
}


void SX128XLT::writeFloat(float x)
{
#ifdef SX128XDEBUG
  Serial.println(F("writeFloat()"));
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


float SX128XLT::readFloat()
{
#ifdef SX128XDEBUG
  Serial.println(F("readFloat()"));
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


uint8_t SX128XLT::transmitSXBuffer(uint8_t startaddr, uint8_t length, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("transmitSXBuffer()"));
#endif

  setBufferBaseAddress(startaddr, 0);          //TX, RX
  setPayloadLength(length);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(timeout);                              //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));            //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                      //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )    //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


uint8_t SX128XLT::transmitSXBufferIRQ(uint8_t startaddr, uint8_t length, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("transmitSXBuffer()"));
#endif

  setBufferBaseAddress(startaddr, 0);          //TX, RX
  setPayloadLength(length);
  setTxParams(txpower, RAMP_TIME);
  setTx(timeout);                              //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  //0x4001   = IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT
  while (!(readIrqStatus() & 0x4001 ));        //wait for IRQs going active

  setMode(MODE_STDBY_RC);                      //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )    //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


void SX128XLT::writeBuffer(uint8_t *txbuffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("writeBuffer()"));
#endif

  uint8_t index, regdata;

  _TXPacketL = _TXPacketL + size;      //these are the number of bytes that will be added
  size--;                              //loose one byte from size, the last byte written MUST be a 0

  for (index = 0; index < size; index++)
  {
    regdata = txbuffer[index];
    SPI.transfer(regdata);
  }

  SPI.transfer(0);                     //this ensures last byte of buffer written really is a null (0)

}


uint8_t SX128XLT::receiveSXBuffer(uint8_t startaddr, uint16_t timeout, uint8_t wait )
{
#ifdef SX128XDEBUG1
  Serial.println(F("receiveSXBuffer()"));
#endif

  uint16_t regdata;
  uint8_t buffer[2];

  setMode(MODE_STDBY_RC);
  setBufferBaseAddress(0, startaddr);               //order is TX RX
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(timeout);

  if (!wait)
  {
    return 0;
  }

  while (!digitalRead(_RXDonePin));                  //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                            //ensure to stop further packet reception

  regdata = readIrqStatus();

  if ( (regdata & IRQ_HEADER_ERROR) | (regdata & IRQ_CRC_ERROR) | (regdata & IRQ_RX_TX_TIMEOUT ) | (regdata & IRQ_SYNCWORD_ERROR ))
  {
    return 0;                                        //no RX done and header valid only, could be CRC error
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  return _RXPacketL;
}


uint8_t SX128XLT::receiveSXBufferIRQ(uint8_t startaddr, uint16_t timeout, uint8_t wait )
{
#ifdef SX128XDEBUG1
  Serial.println(F("receiveSXBufferIRQ()"));
#endif

  uint8_t buffer[2];

  setMode(MODE_STDBY_RC);
  setBufferBaseAddress(0, startaddr);                //order is TX RX
  setRx(timeout);

  if (!wait)
  {
    return 0;
  }

  //0x4022   = IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR
  while (!(readIrqStatus() & 0x4022 ));              //wait for IRQs going active


  setMode(MODE_STDBY_RC);                            //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                                        //no RX done and header valid only, could be CRC error
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  return _RXPacketL;
}


uint8_t SX128XLT::readBuffer(uint8_t *rxbuffer)
{
#ifdef SX128XDEBUG1
  Serial.println(F("readBuffer()"));
#endif

  uint8_t index = 0, regdata;

  do                                     //need to find the size of the buffer first
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;           //fill the buffer.
    index++;
  } while (regdata != 0);                //keep reading until we have reached the null (0) at the buffer end
  //or exceeded size of buffer allowed
  _RXPacketL = _RXPacketL + index;       //increment count of bytes read
  return index;                          //return the actual size of the buffer, till the null (0) detected

}


uint16_t SX128XLT::CRCCCITTSX(uint8_t startadd, uint8_t endadd, uint16_t startvalue)
{
  //genrates a CRC of an area of the internal SX buffer

#ifdef SX126XDEBUG1
  Serial.println(F("CRCCCITTSX()"));
#endif

  uint16_t index, libraryCRC;
  uint8_t j;

  libraryCRC = startvalue;                           //start value for CRC16
  startReadSXBuffer(startadd);                       //begin the buffer read

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

  endReadSXBuffer();                                 //end the buffer read
  return libraryCRC;
}


uint8_t SX128XLT::getByteSXBuffer(uint8_t addr)
{
#ifdef SX128XDEBUG1
  Serial.println(F("getByteSXBuffer()"));
#endif

  uint8_t regdata;

  setMode(MODE_STDBY_RC);                     //this is needed to ensure we can read from buffer OK.
  checkBusy();

#ifdef USE_SPI_TRANSACTION                    //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                    //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(addr);
  SPI.transfer(0xFF);
  regdata = SPI.transfer(0);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return regdata;
}


void SX128XLT::printSXBufferHEX(uint8_t start, uint8_t end)
{
#ifdef SX128XDEBUG
  Serial.println(F("printSXBufferHEX()"));
#endif

  uint8_t index, regdata;


  setMode(MODE_STDBY_RC);

  checkBusy();

#ifdef USE_SPI_TRANSACTION                       //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                       //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(start);
  SPI.transfer(0xFF);

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


void SX128XLT::writeBufferChar(char *txbuffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("writeBuffer()"));
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


uint8_t SX128XLT::readBufferChar(char *rxbuffer)
{
#ifdef SX128XDEBUG1
  Serial.println(F("readBufferChar()"));
#endif

  uint8_t index = 0, regdata;

  do                                     //need to find the size of the buffer first
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;           //fill the buffer.
    index++;
  } while (regdata != 0);                //keep reading until we have reached the null (0) at the buffer end
  //or exceeded size of buffer allowed

  _RXPacketL = _RXPacketL + index;       //increment count of bytes read

  return index;                          //return the actual size of the buffer, till the null (0) detected

}


//There are two ways of setting the payload length, enabled by default is the appropriate function setPacketParams()
//To set payload length with a discovered direct register length enable the #define USEPAYLOADLENGTHREGISTER

void SX128XLT::setPayloadLength(uint8_t length)
{
#ifdef SX128XDEBUG
  Serial.println(F("setPayloadLength()"));
#endif
  if (savedPacketType == PACKET_TYPE_LORA)
  {
#ifdef USEPAYLOADLENGTHREGISTER
    //Serial.println(F(" USEPAYLOADLENGTHREGISTER "));
    writeRegister(REG_LR_PAYLOADLENGTH, length);
#else
    //Serial.println(F(" USE setPacketParams() "));
    setPacketParams(savedPacketParam1, savedPacketParam2, length, savedPacketParam4, savedPacketParam5);
#endif
  }

  if (savedPacketType == PACKET_TYPE_FLRC)
  {
#ifdef USEPAYLOADLENGTHREGISTER
    //Serial.println(F(" USEPAYLOADLENGTHREGISTER FLRC "));
    writeRegister(REG_LR_FLRCPAYLOADLENGTH, length);
#else
    //Serial.println(F(" USE setPacketParams() FLRC "));
    setPacketParams(savedPacketParam1, savedPacketParam2, savedPacketParam3, savedPacketParam4, length, savedPacketParam6, savedPacketParam7);
#endif
  }

}


void SX128XLT::setFLRCPayloadLengthReg(uint8_t length)
{
  //uses a discovered register that is the FLRC payload length for TX and RX filtering
#ifdef SX128XDEBUG
  Serial.println(F("setFLRCPayloadLengthReg()"));
#endif

  writeRegister(REG_LR_FLRCPAYLOADLENGTH, length);
}


void SX128XLT::setLoRaPayloadLengthReg(uint8_t length)
{
  //uses a discovered register that is the LoRa payload length for TX
#ifdef SX128XDEBUG
  Serial.println(F("setLoRaPayloadLengthReg()"));
#endif

  writeRegister(REG_LR_PAYLOADLENGTH, length);
}


uint8_t SX128XLT::getPacketType()
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getPacketType() "));
#endif

  return savedPacketType;
}

//*******************************************************************************
//Ranging routines
//*******************************************************************************

void SX128XLT::setRangingSlaveAddress(uint32_t address)
{
  //sets address of ranging slave
#ifdef SX128XDEBUG
  Serial.println(F("SetRangingSlaveAddress()"));
#endif

  uint8_t buffer[4];

  buffer[0] = (address >> 24u ) & 0xFFu;
  buffer[1] = (address >> 16u) & 0xFFu;
  buffer[2] = (address >>  8u) & 0xFFu;
  buffer[3] = (address & 0xFFu);
  writeRegisters(0x916, buffer, 4 );
}


void SX128XLT::setRangingMasterAddress(uint32_t address)
{
  //sets address of ranging master
#ifdef SX128XDEBUG
  Serial.println(F("SetRangingMasterAddress()"));
#endif

  uint8_t buffer[4];

  buffer[0] = (address >> 24u ) & 0xFFu;
  buffer[1] = (address >> 16u) & 0xFFu;
  buffer[2] = (address >>  8u) & 0xFFu;
  buffer[3] = (address & 0xFFu);
  writeRegisters(0x912, buffer, 4 );
}


void SX128XLT::setRangingCalibration(uint16_t cal)
{
#ifdef SX128XDEBUG
  Serial.println(F("setRangingCalibration()"));
#endif

  savedCalibration = cal;
  writeRegister( REG_LR_RANGINGRERXTXDELAYCAL, ( uint8_t )( ( cal >> 8 ) & 0xFF ) );
  writeRegister( REG_LR_RANGINGRERXTXDELAYCAL + 1, ( uint8_t )( ( cal ) & 0xFF ) );
}


void SX128XLT::setRangingRole(uint8_t role)
{
#ifdef SX128XDEBUG
  Serial.println(F("setRangingRole()"));
#endif

  uint8_t buffer[1];

  buffer[0] = role;
  writeCommand(RADIO_SET_RANGING_ROLE, buffer, 1 );
}


uint32_t SX128XLT::getRangingResultRegValue(uint8_t resultType)
{
  uint32_t valLsb = 0;

  setMode(MODE_STDBY_XOSC);
  writeRegister( 0x97F, readRegister( 0x97F ) | ( 1 << 1 ) ); // enable LORA modem clock
  writeRegister( REG_LR_RANGINGRESULTCONFIG, ( readRegister( REG_LR_RANGINGRESULTCONFIG ) & MASK_RANGINGMUXSEL ) | ( ( ( ( uint8_t )resultType ) & 0x03 ) << 4 ) );
  valLsb = ( ( (uint32_t) readRegister( REG_LR_RANGINGRESULTBASEADDR ) << 16 ) | ( (uint32_t) readRegister( REG_LR_RANGINGRESULTBASEADDR + 1 ) << 8 ) | ( readRegister( REG_LR_RANGINGRESULTBASEADDR + 2 ) ) );
  setMode(MODE_STDBY_RC);
  return valLsb;
}


double SX128XLT::getRangingDistance(uint8_t resultType, int32_t regval, float adjust)
{
  float val = 0.0;

  if (regval >= 0x800000)                  //raw reg value at low distance can goto 0x800000 which is negative, set distance to zero if this happens
  {
    regval = 0;
  }

  // Conversion from LSB to distance. For explanation on the formula, refer to Datasheet of SX1280

  switch (resultType)
  {
    case RANGING_RESULT_RAW:
      // Convert the ranging LSB to distance in meter. The theoretical conversion from register value to distance [m] is given by:
      // distance [m] = ( complement2( register ) * 150 ) / ( 2^12 * bandwidth[MHz] ) ). The API provide BW in [Hz] so the implemented
      // formula is complement2( register ) / bandwidth[Hz] * A, where A = 150 / (2^12 / 1e6) = 36621.09
      val = ( double ) regval / ( double ) returnBandwidth(savedModParam2) * 36621.09375;
      break;

    case RANGING_RESULT_AVERAGED:
    case RANGING_RESULT_DEBIASED:
    case RANGING_RESULT_FILTERED:
      Serial.print(F("??"));
      val = ( double )regval * 20.0 / 100.0;
      break;
    default:
      val = 0.0;
      break;
  }

  val = val * adjust;
  return val;
}


bool SX128XLT::setupRanging(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint32_t address, uint8_t role)
{
  //sequence is frequency, offset, spreading factor, bandwidth, coding rate, calibration, role.
#ifdef SX128XDEBUG
  Serial.println(F("setupRanging()"));
#endif

  setMode(MODE_STDBY_RC);
  setPacketType(PACKET_TYPE_RANGING);
  setModulationParams(modParam1, modParam2, modParam3);
  setPacketParams(12, LORA_PACKET_VARIABLE_LENGTH, 0, LORA_CRC_ON, LORA_IQ_NORMAL, 0, 0);
  setRfFrequency(frequency, offset);
  setRangingSlaveAddress(address);
  setRangingMasterAddress(address);
  setRangingCalibration(lookupCalibrationValue(modParam1, modParam2));
  setRangingRole(role);
  setHighSensitivity();
  return true;
}


bool SX128XLT::transmitRanging(uint32_t address, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("transmitRanging()"));
#endif

  // if ((_RXEN >= 0) || (_TXEN >= 0))
  // {
  //   return false;
  // }

  setMode(MODE_STDBY_RC);
  setRangingMasterAddress(address);
  setTxParams(txpower, RADIO_RAMP_02_US);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RANGING_MASTER_RESULT_VALID + IRQ_RANGING_MASTER_RESULT_TIMEOUT), 0, 0);
  setTx(timeout);                                           //this sends the ranging packet

  if (!wait)
  {
    return true;
  }

  while (!digitalRead(_TXDonePin));                         //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                   //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RANGING_MASTER_RESULT_VALID )   //check for timeout
  {
    return true;
  }
  else
  {
    return false;
  }
}


uint8_t SX128XLT::receiveRanging(uint32_t address, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("receiveRanging()"));
#endif

  setTxParams(txpower, RADIO_RAMP_02_US);
  setRangingSlaveAddress(address);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RANGING_SLAVE_RESPONSE_DONE + IRQ_RANGING_SLAVE_REQUEST_DISCARDED + IRQ_HEADER_ERROR), 0, 0);
  setRx(timeout);

  if (!wait)
  {
    return NO_WAIT;                                          //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));

  setMode(MODE_STDBY_RC);                                    //ensure to stop further packet reception

  if (readIrqStatus() & IRQ_RANGING_SLAVE_REQUEST_VALID)
  {
    return true;
  }
  else
  {
    return false;                                            //so we can check for packet having enough buffer space
  }
}


uint16_t SX128XLT::lookupCalibrationValue(uint8_t spreadingfactor, uint8_t bandwidth)
{
  //this looks up the calibration value from the table in SX128XLT_Definitions.hifdef SX128XDEBUG
#ifdef SX128XDEBUG
  Serial.println(F("lookupCalibrationValue()"));
#endif

  switch (bandwidth)
  {
    case LORA_BW_0400:
      savedCalibration = RNG_CALIB_0400[(spreadingfactor >> 4) - 5];
      return savedCalibration;

    case LORA_BW_0800:
      savedCalibration = RNG_CALIB_0800[(spreadingfactor >> 4) - 5];
      return savedCalibration;


    case LORA_BW_1600:
      savedCalibration = RNG_CALIB_1600[(spreadingfactor >> 4) - 5];
      return savedCalibration;

    default:
      return 0xFFFF;
  }
}


uint16_t SX128XLT::getSetCalibrationValue()
{
#ifdef SX128XDEBUG
  Serial.println(F("getCalibrationValue()"));
#endif

  return savedCalibration;
}


int16_t SX128XLT::getRangingRSSI()
{
  //Added November 2021 - see datasheet SX1280-1_V3.2, 14.5.3 Ranging RSSI
  int16_t regdata;
  regdata = readRegister(REG_RANGING_RSSI);
  regdata = regdata - 150;
  return regdata;
}

//**********************************************************************************************
// Reliable packet routines - added November 2021
// Routines assume that RX and TX buffer base addresses are set to 0 by setupLoRa(), setupFLRC()
//**********************************************************************************************

void SX128XLT::printASCIIArray(uint8_t *buffer, uint8_t size)
{
#ifdef SX128XDEBUG1
  Serial.println(F("printASCIIArray() "));
#endif

  uint8_t index;

  for (index = 0; index < size; index++)
  {
    Serial.write(buffer[index]);
  }
}


uint8_t SX128XLT::getReliableConfig(uint8_t bitread)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} getReliableConfig() "));
  Serial.println(_ReliableConfig);
#endif

  return bitRead(_ReliableConfig, bitread);
}


uint8_t SX128XLT::transmitReliable(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitRELIABLE() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
  Serial.print(F(" {RELIABLE} Payload length "));
  Serial.println(size);
#endif

  uint8_t index, tempdata;
  uint16_t payloadcrc;

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
    //payloadcrc = CRCCCITT(txbuffer, size, 0xFFFF) + 1;
  }

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);

  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                                //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                          //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }

  return _TXPacketL;
}


uint16_t SX128XLT::getTXPayloadCRC(uint8_t length)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getTXPayloadCRC() "));
#endif

  return readUint16SXBuffer(length - 2);
}


uint16_t SX128XLT::getRXPayloadCRC(uint8_t length)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getRXPayloadCRC() "));
#endif

  return readUint16SXBuffer(length - 2);
}


uint16_t SX128XLT::getRXNetworkID(uint8_t length)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getRXnetworkID() "));
#endif

  return readUint16SXBuffer(length - 4);
}


uint16_t SX128XLT::getTXNetworkID(uint8_t length)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} getTXnetworkID() "));
#endif

  return readUint16SXBuffer(length - 4);
}


void SX128XLT::printReliableStatus()
{
#ifdef SX128XDEBUGRELIABLE
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

  //0x04
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


void SX128XLT::writeUint16SXBuffer(uint8_t addr, uint16_t regdata)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} writeUint16SXBuffer() 0x"));
  Serial.print(addr, HEX);
  Serial.print(F(" 0x"));
  Serial.println(regdata, HEX);
#endif

  setMode(MODE_STDBY_RC);                 //this is needed to ensure we can write to buffer OK.
  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                //start the burst read
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(addr);
  SPI.transfer(lowByte(regdata));
  SPI.transfer(highByte(regdata));
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif
}


uint16_t SX128XLT::readUint16SXBuffer(uint8_t addr)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} readUint16SXBuffer() 0x"));
  Serial.println(addr, HEX);
#endif

  uint8_t regdatalow, regdatahigh;
  setMode(MODE_STDBY_RC);                  //this is needed to ensure we can read from buffer OK.

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                 //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(addr);
  SPI.transfer(0xFF);
  regdatalow = SPI.transfer(0);
  regdatahigh = SPI.transfer(0);
  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return (regdatalow + (regdatahigh << 8));
}

//******************************************************************************************
// Reliable packet routines - added November 2021
//******************************************************************************************


uint8_t SX128XLT::receiveReliable(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
  //Maximum total packet size is 255 bytes, so allowing for the 4 bytes appended to the end of a reliable
  //packet, the maximum payload size for LORa is 251 bytes and 117 bytes for FLRC. So to avoid overwriting
  //memory, we do need to check if the passed array is big enough to take the payload received in the packet.
  //The assumed payload length will always be 4 bytes less than the received packet length.

#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveReliable()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint8_t regdataL, regdataH;
  uint8_t index;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (size > 251 )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                                               //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));                                         //Wait for DIO1 to go high
  setMode(MODE_STDBY_RC);                                                   //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                                                               //packet is errored somewhere so return 0
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];


  if (_RXPacketL < 4)                                  //check received packet is 4 or more bytes long
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if ((_RXPacketL - 4) > size )                        //check if calculated payload size (_RXPacketL -4) fits in array
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);             //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(0);
  SPI.transfer(0xFF);

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
    //payloadcrc = CRCCCITT(rxbuffer, (_RXPacketL - 4), 0xFFFF) + 1;
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


void SX128XLT::setReliableRX(uint16_t timeout)
{
  //existing setRx() does not setup LoRa device as a receiver completly, just turns on receiver mode
  //this routine does all the required setup for receive mode
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} setReliableRX()"));
#endif

  setMode(MODE_STDBY_RC);                                                   //stops receiver
  clearIrqStatus(IRQ_RADIO_ALL);                                            //clear current interrupt flags
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);
  setRx(timeout);
}


uint8_t SX128XLT::transmitReliableAutoACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} transmitReliableAutoACK() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
  Serial.print(F(" {RELIABLE} Payload length "));
  Serial.println(size);
#endif

  uint8_t index, tempdata, RXPacketL;
  uint16_t payloadcrc;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (size > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  checkBusy();
  _TXPacketL = size + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITT(txbuffer, size, 0xFFFF);
    //payloadcrc = CRCCCITT(txbuffer, size, 0xFFFF) + 1;
  }

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                                //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                          //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }

  RXPacketL = waitReliableACK(networkID, payloadcrc, acktimeout);

  if (RXPacketL != 4)
  {
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX128XLT::waitReliableACK(uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitReliableACK()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t buffer[2];

  setReliableRX(0);
  startmS = millis();

  do
  {
    if (digitalRead(_RXDonePin))          //has a packet arrived ?
    {
      if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
      {
        setReliableRX(0);
        continue;
      }

      readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
      _RXPacketL = buffer[0];
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        return 4;                          //return value of 4 indicates valid ack
      }
      else
      {
        setReliableRX(0);
        continue;
      }
    }

  } while ( ((uint32_t) (millis() - startmS) < acktimeout));

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}


uint8_t SX128XLT::waitReliableACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{
  //overloaded version of waitReliableACK() for use when ack contains payload data
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitReliableACK()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t buffer[2];
  uint8_t regdata, index;


  if (size > 251 )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setReliableRX(0);
  startmS = millis();

  do
  {
    if (digitalRead(_RXDonePin))                         //has a packet arrived ?
    {
      regdata = readIrqStatus();

      if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
      {
        setReliableRX(0);
        continue;
      }

      readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
      _RXPacketL = buffer[0];
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        if ((_RXPacketL - 4) > size )                    //check passed buffer is big enough for payload
        {
          bitSet(_ReliableErrors, ReliableACKError);
          bitSet(_ReliableErrors, ReliableSizeError);
          return 0;
        }

        bitSet(_ReliableFlags, ReliableACKReceived);
        checkBusy();

#ifdef USE_SPI_TRANSACTION
        SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

        digitalWrite(_NSS, LOW);             //start the burst read
        SPI.transfer(RADIO_READ_BUFFER);
        SPI.transfer(0);
        SPI.transfer(0xFF);

        for (index = 0; index < (_RXPacketL - 4); index++)   //read packet into rxbuffer
        {
          regdata = SPI.transfer(0);
          rxbuffer[index] = regdata;
        }
        digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
        SPI.endTransaction();
#endif

        return _RXPacketL;                                  //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX(0);
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}



uint8_t SX128XLT::receiveReliableAutoACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait )
{
  //Maximum total packet size is 255 bytes, so allowing for the 4 bytes appended to the end of a reliable
  //packet, the maximum payload size for LORa is 251 bytes and 117 bytes for FLRC. So to avoid overwriting
  //memory, we do need to check if the passed array is big enough to take the payload received in the packet.
  //The assumed payload length will always be 4 bytes less than the received packet length.


#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveReliableAutoACK()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint8_t regdataL, regdataH, index;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (size > 251 )
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                                               //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));                                         //Wait for DIO1 to go high
  setMode(MODE_STDBY_RC);                                                   //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                                                               //packet is errored somewhere so return 0
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

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

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);                           //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(0);
  SPI.transfer(0xFF);

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
    //payloadcrc = CRCCCITT(rxbuffer, (_RXPacketL - 4), 0xFFFF) + 1;
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

  delay(ackdelay);
  _TXPacketL = sendReliableACK(RXnetworkID, payloadcrc, txpower);

  if (_TXPacketL != 4)
  {
    return 0;
  }

  return _RXPacketL;                                        //return and indicate RX OK.
}


uint8_t SX128XLT::sendReliableACK(uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} sendReliableACK()"));
#endif

  uint32_t txtimeout = 12000;                               //set TX timeout to 12 seconds, longest packet is 8.7secs
  _TXPacketL = 4;                                           //packet is networkId (2 bytes) + payloadCRC (2 bytes)
  setMode(MODE_STDBY_RC);

  //payloadcrc++;

  writeUint16SXBuffer(0, networkID);
  writeUint16SXBuffer(2, payloadcrc);

  checkBusy();
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  while (!digitalRead(_TXDonePin));

  setMode(MODE_STDBY_RC);                                 //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )               //check for timeout
  {
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return 4;                                               //TX OK so return TXpacket length
}


uint8_t SX128XLT::sendReliableACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{
  //overloaded version of sendReliableACK() for use when ack contains payload data
#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendReliableACK() "));
  Serial.print(F("buffer size "));
  Serial.println(size);
#endif

  uint32_t txtimeout = 12000;                             //set TX timeout to 12 seconds, longest packet is 8.7secs
  uint8_t bufferdata, index;

  //payloadcrc++;

  setMode(MODE_STDBY_RC);
  _TXPacketL = size + 4;
  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);                                                          //this starts the TX

  while (!digitalRead(_TXDonePin));                                //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                          //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                               //TX OK so return TXpacket length
}


uint16_t SX128XLT::CRCCCITTReliable(uint8_t startadd, uint8_t endadd, uint16_t startvalue)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} CRCCCITTReliable()"));
#endif

  //generates a CRC of bytes from the internal SX buffer, _RXPackletL and _TXPackletL are not affected

#ifdef SX128DEBUGRELIABLE
  Serial.println(F(" {RELIABLE} CRCCCITTReliable() "));
#endif

  uint16_t index, libraryCRC;
  uint8_t j, readSX;

  libraryCRC = startvalue;              //start value for CRC16
  setMode(MODE_STDBY_RC);
  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);             //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(startadd);
  SPI.transfer(0xFF);

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


void SX128XLT::setReliableConfig(uint8_t bitset)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} setReliableConfig() bit "));
  Serial.println(bitset);
#endif

  bitSet(_ReliableConfig, bitset);
  return;
}


void SX128XLT::clearReliableConfig(uint8_t bitset)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} clearReliableConfig() bit "));
  Serial.println(bitset);
#endif

  bitClear(_ReliableConfig, bitset);
  return;
}



uint8_t SX128XLT::readReliableErrors()
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} readReliableErrors()"));
#endif

  return _ReliableErrors;
}

uint8_t SX128XLT::readReliableFlags()
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} readReliableFlags()"));
#endif

  return _ReliableFlags;
}


//******************************************************************************************
// Reliable SX packet routines - added November 2021
// SX Packet routines directly read\write data\bvariables to the LoRa device internal buffer
//******************************************************************************************


uint8_t SX128XLT::transmitSXReliable(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitSXReliable() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc;

  setMode(MODE_STDBY_RC);
  checkBusy();
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
    //payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF) + 1;
  }

  writeUint16SXBuffer(startaddr + _TXPacketL - 4, networkID);
  writeUint16SXBuffer(startaddr + _TXPacketL - 2, payloadcrc);

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);                                    //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                    //Wait for pin to go high, TX finished

  setMode(MODE_STDBY_RC);                              //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )            //check for timeout
  {
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX128XLT::transmitSXReliableIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitSXReliableIRQ() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc;

  setMode(MODE_STDBY_RC);
  checkBusy();
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
    //payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF) + 1;
  }

  writeUint16SXBuffer(startaddr + _TXPacketL - 4, networkID);
  writeUint16SXBuffer(startaddr + _TXPacketL - 2, payloadcrc);
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setTx(txtimeout);                                       //this starts the TX

  if (!wait)
  {
    return _TXPacketL;
  }

  //0x4001   = IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT
  while (!(readIrqStatus() & 0x4001 ));   //wait for IRQs going active

  setMode(MODE_STDBY_RC);                                //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )              //check for timeout
  {
    return 0;
  }

  return _TXPacketL;
}


uint8_t SX128XLT::receiveSXReliable(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveSXReliable()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  setMode(MODE_STDBY_RC);
  checkBusy();
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                           //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));                     //Wait for DIO1 to go high, no timeout, RX DONE

  setMode(MODE_STDBY_RC);                               //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                                           //no RX done and header valid only, could be CRC error
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL < 4)                                   //check received packet is 4 or more bytes long
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
    //payloadcrc = CRCCCITTReliable(startaddr, (startaddr + _RXPacketL - 5), 0xFFFF) + 1;
    RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (_ReliableErrors)                                  //if there has been a reliable error return a RX fail
  {
    return 0;
  }
  return _RXPacketL;                                    //return and RX OK.
}


uint8_t SX128XLT::receiveSXReliableIRQ(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveSXReliable()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  setMode(MODE_STDBY_RC);
  checkBusy();
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                                              //not wait requested so no packet length to pass
  }

  //0x4022   = IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR
  while (!(readIrqStatus() & 0x4022 ));   //wait for IRQs going active

  setMode(MODE_STDBY_RC);                                                  //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                                                              //no RX done and header valid only, could be CRC error
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

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
    //payloadcrc = CRCCCITTReliable(startaddr, (startaddr + _RXPacketL - 5), 0xFFFF) + 1;
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

  return _RXPacketL;                                        //return and RX OK.
}


uint8_t SX128XLT::transmitSXReliableAutoACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} transmitSXReliableAutoACK() "));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint8_t RXPacketL;
  uint16_t payloadcrc;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  if (startaddr + length > 251)
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  setMode(MODE_STDBY_RC);
  checkBusy();
  _TXPacketL = length + 4;

  if (bitRead(_ReliableConfig, NoReliableCRC))
  {
    payloadcrc = 0;
  }
  else
  {
    payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF);
    //payloadcrc = CRCCCITTReliable(startaddr, startaddr + length - 1, 0xFFFF)+1;
  }

  writeUint16SXBuffer(startaddr + _TXPacketL - 4, networkID);
  writeUint16SXBuffer(startaddr + _TXPacketL - 2, payloadcrc);
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  if (!wait)
  {
    return _TXPacketL;
  }

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                        //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                  //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                //check for timeout
  {
    return 0;
  }

  RXPacketL = waitReliableACK(networkID, payloadcrc, acktimeout);

  if (RXPacketL != 4)
  {

    return 0;
  }

  return _TXPacketL;
}


uint8_t SX128XLT::receiveSXReliableAutoACK(uint8_t startaddr, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveSXReliableAutoACK()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint16_t temp1, temp2;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  setMode(MODE_STDBY_RC);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                           //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));                     //Wait for DIO1 to go high
  setMode(MODE_STDBY_RC);                               //ensure to stop further packet reception

  if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
  {
    return 0;                                           //packet is errored somewhere so return 0
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL < 4)                                   //check received packet is 4 or more bytes long
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
    //payloadcrc = CRCCCITTReliable(startaddr, (startaddr + _RXPacketL - 5), 0xFFFF)+1;
    RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
    }
  }

  if (_ReliableErrors)                                    //if there has been a reliable error return a RX fail
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


uint8_t SX128XLT::waitSXReliableACK(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitSXReliableACK()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t buffer[2];

  setReliableRX(0);

  startmS = millis();

  do
  {
    if (digitalRead(_RXDonePin))                             //has a packet arrived ?
    {
      if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
      {
        setReliableRX(0);
        continue;
      }

      readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
      _RXPacketL = buffer[0];
      RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);
      RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        return _RXPacketL;                                   //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX(0);
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}


uint8_t SX128XLT::waitSXReliableACKIRQ(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitSXReliableACKIRQ()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t buffer[2];

  setReliableRX(0);

  startmS = millis();

  do
  {
    //0x4022   = IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR
    if (readIrqStatus() & 0x4022 )
    {
      if ( readIrqStatus() & (IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR))
      {
        setReliableRX(0);
        continue;
      }

      readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
      _RXPacketL = buffer[0];
      RXnetworkID = readUint16SXBuffer(startaddr + _RXPacketL - 4);
      RXcrc = readUint16SXBuffer(startaddr + _RXPacketL - 2);

      if ( (RXnetworkID == networkID) && (RXcrc == payloadcrc))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);
        return _RXPacketL;                                 //_RXPacketL should be payload length + 4
      }
      else
      {
        setReliableRX(0);
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout) );

  bitSet(_ReliableErrors, ReliableACKError);
  return 0;
}


uint8_t SX128XLT::sendSXReliableACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendSXReliableACK() "));
#endif

  uint32_t txtimeout = 12000;                                   //set TX timeout to 12 seconds, longest packet is 8.7secs

  _TXPacketL = length + 4;                                      //packet is networkId (2 bytes) + payloadCRC (2 bytes)
  setMode(MODE_STDBY_RC);

  writeUint16SXBuffer((length + startaddr), networkID);
  writeUint16SXBuffer((length + startaddr + 2), payloadcrc);
  checkBusy();
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);                                             //start transmission

  while (!digitalRead(_TXDonePin));

  setMode(MODE_STDBY_RC);                                       //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                     //check for timeout
  {
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                            //TX OK so return TXpacket length
}


uint8_t SX128XLT::sendSXReliableACKIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendSXReliableACK() "));
#endif

  uint32_t txtimeout = 12000;                                    //set TX timeout to 12 seconds, longest packet is 8.7secs

  _TXPacketL = length + 4;                                       //packet is networkId (2 bytes) + payloadCRC (2 bytes)
  setMode(MODE_STDBY_RC);

  writeUint16SXBuffer((length + startaddr), networkID);
  writeUint16SXBuffer((length + startaddr + 2), payloadcrc);

  checkBusy();
  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setTx(txtimeout);                                             //start transmission

  //0x4001   = IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT
  while (!(readIrqStatus() & 0x4001 ));                         //wait for IRQs going active

  setMode(MODE_STDBY_RC);                                       //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                     //check for timeout
  {
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                            //TX OK so return TXpacket length
}

//***********************************************************************************
//Data Transfer functions - Added November 2021
//TX and RX base addresses assumed to be 0
//***********************************************************************************

uint8_t SX128XLT::transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} transmitDT() "));
#endif

  uint8_t index, bufferdata;
  uint16_t payloadcrc;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

#ifdef DETECTRELIABLERRORS
  if (datasize > (251 - headersize))                  //its 251 because of 4 bytes appended to packet
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }
#endif
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

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  if (!wait)
  {
    return _TXPacketL;
  }

  while (!digitalRead(_TXDonePin));                                //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);                                          //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


uint8_t SX128XLT::waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitACKDT()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t regdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  networkID = readUint16SXBuffer(_TXPacketL - 4);              //get networkID used to transmit previous packet, before next RX
  payloadCRC = readUint16SXBuffer(_TXPacketL - 2);             //get payloadCRC used to transmit previous packet, before next RX

  setReliableRX(0);
  startmS = millis();                                          //setReliableRX has a timeount, but here we want an overall timeout waiting for ACK

  do
  {
    if (digitalRead(_RXDonePin))                               //has a packet arrived ?
    {
      setMode(MODE_STDBY_RC);                                  //ensure to stop further packet reception

      //IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR = 0x4068
      if (readIrqStatus() & 0x4068)
      {
        setReliableRX(0);
        continue;
      }
      readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
      _RXPacketL = buffer[0];
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ((_RXPacketL - 4) > headersize )                       //check passed buffer is big enough for header
      {
        setReliableRX(0);
        continue;
      }

      if (!bitRead(_ReliableConfig, NoReliableCRC))
      {
        if (payloadCRC != RXcrc)
        {
          bitSet(_ReliableErrors, ReliableCRCError);
          setReliableRX(0);
          continue;
        }
      }

      if ( (RXnetworkID == networkID))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);

        checkBusy();

#ifdef USE_SPI_TRANSACTION
        SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

        digitalWrite(_NSS, LOW);                                //start the burst read
        SPI.transfer(RADIO_READ_BUFFER);
        SPI.transfer(0);
        SPI.transfer(0xFF);

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
        setReliableRX(0);
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout));

  bitSet(_ReliableErrors, ReliableACKError);
  bitSet(_ReliableErrors, ReliableTimeout);

  return 0;
}


uint8_t SX128XLT::receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveDT()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t index, payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint8_t regdataL, regdataH;
  uint8_t RXHeaderL;
  uint8_t RXDataL;
  uint8_t buffer[2];
  uint8_t RXstart;

  _ReliableErrors = 0;
  _ReliableFlags = 0;
  setMode(MODE_STDBY_RC);
  setPayloadLength(127);                                 //set for maximum packet length in FLRC mode, packets might be filtered otherwise
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                            //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));                      //Wait for DIO1 to go high, no timeout, RX DONE

  setMode(MODE_STDBY_RC);                                //ensure to stop further packet reception

  //IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR = 0x4068
  if (readIrqStatus() & 0x4068)
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.println(F(" {RELIABLE} Packet error"));
#endif
    return 0;                                            //packet is errored somewhere so return 0
  }

  RXHeaderL = getByteSXBuffer(2);
  RXDataL = getByteSXBuffer(3);

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];
  RXstart = buffer[1];

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} Header  "));
  printSXBufferHEX(0, (RXHeaderL - 1));
  Serial.println();
  Serial.print(F(" {RELIABLE} Received data payload size "));
  Serial.println(RXDataL);
  Serial.print(F(" {RELIABLE} Data payload  "));
  printSXBufferHEX(RXHeaderL, RXHeaderL + RXDataL - 1);
  Serial.println();
#endif

  if (RXHeaderL > headersize )
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} Header size error "));
    Serial.println(headersize);
#endif
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if (RXDataL > datasize )
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} Data size error "));
    Serial.println(datasize);
#endif
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} _RXPacketL  "));
  Serial.println(_RXPacketL);
  Serial.print(F(" {RELIABLE} Header  "));
  printSXBufferHEX(0, (RXHeaderL - 1));
  Serial.println();
  Serial.print(F(" {RELIABLE} Received data payload size "));
  Serial.println(RXDataL);
  Serial.print(F(" {RELIABLE} Data payload  "));
  printSXBufferHEX(RXHeaderL, RXHeaderL + RXDataL - 1);
  Serial.println();
#endif

  if (_RXPacketL < 10)                                      //check received packet is 10 or more bytes long
  {
    //Serial.println(F(" {RELIABLE} _RXPacketL < 10"));
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);             //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

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

#ifdef SX128XDEBUGRELIABLE
    Serial.println(F(" {RELIABLE} Payload CRC check enabled"));
    Serial.print(F(" {RELIABLE} payloadcrc 0x"));
    Serial.println(payloadcrc, HEX);
    Serial.print(F(" {RELIABLE} RXcrc 0x"));
    Serial.println(RXcrc, HEX);
#endif

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
#ifdef SX128XDEBUGRELIABLE
      Serial.print(F(" {RELIABLE} CRCmissmatch, received 0x"));
      Serial.println(RXcrc, HEX);
#endif
    }
  }

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} RXnetworkID 0x"));
  Serial.println(RXnetworkID, HEX);
#endif

  if (RXnetworkID != networkID)
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} NetworkID missmatch received 0x"));
    Serial.print(RXnetworkID, HEX);
    Serial.print(F(" LocalID 0x"));
    Serial.println(networkID, HEX);
#endif
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (_ReliableErrors)                                            //if there has been a reliable error return a RX fail
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} Reliable errors"));
#endif

    return 0;
  }

  return _RXPacketL;                                              //return and indicate RX OK.
}


uint8_t SX128XLT::sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendACKDT() "));
#endif

  uint32_t txtimeout = 12000;                                     //set TX timeout to 12 seconds, longest packet is 8.7secs
  uint8_t bufferdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;

  setMode(MODE_STDBY_RC);
  _TXPacketL = headersize + 4;
  networkID = readUint16SXBuffer(_RXPacketL - 4);
  payloadCRC = readUint16SXBuffer(_RXPacketL - 2);
  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  while (!digitalRead(_TXDonePin));                                          //Wait for DIO1 to go high

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                                  //check for timeout
  {
    bitSet(_ReliableFlags, ReliableTimeout);
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                                         //TX OK so return TXpacket length
}


//***********************************************************************************
//IRQ Data Transfer functions - Added November 2021
//TX and RX base addresses assumed to be 0
//***********************************************************************************



uint8_t SX128XLT::transmitDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} transmitDT() "));
#endif

  uint8_t index, bufferdata;
  uint16_t payloadcrc;

  _ReliableErrors = 0;
  _ReliableFlags = 0;

#ifdef DETECTRELIABLERRORS
  if (datasize > (251 - headersize))                  //its 251 because of 4 bytes appended to packet
  {
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }
#endif
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

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  if (!wait)
  {
    return _TXPacketL;
  }

  //while (!digitalRead(_TXDonePin));                                //Wait for DIO1 to go high

  //0x4001   = IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT
  while (!(readIrqStatus() & 0x4001 ));                            //wait for IRQs going active


  setMode(MODE_STDBY_RC);                                          //ensure we leave function with TX off

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                        //check for timeout
  {
    return 0;
  }
  else
  {
    return _TXPacketL;
  }
}


uint8_t SX128XLT::waitACKDTIRQ(uint8_t *header, uint8_t headersize, uint32_t acktimeout)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.println(F(" {RELIABLE} waitACKDT()"));
#endif

  uint16_t RXnetworkID, RXcrc;
  uint32_t startmS;
  uint8_t regdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;
  uint8_t buffer[2];

  _ReliableErrors = 0;
  _ReliableFlags = 0;

  networkID = readUint16SXBuffer(_TXPacketL - 4);              //get networkID used to transmit previous packet, before next RX
  payloadCRC = readUint16SXBuffer(_TXPacketL - 2);             //get payloadCRC used to transmit previous packet, before next RX

  setReliableRX(0);
  startmS = millis();                                          //setReliableRX has a timeount, but here we want an overall timeout waiting for ACK

  do
  {
    //0x4022   = IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR
    if (readIrqStatus() & 0x4022 )
    {
      setMode(MODE_STDBY_RC);                                  //ensure to stop further packet reception

      //IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR = 0x4068
      if (readIrqStatus() & 0x4068)
      {
        setReliableRX(0);
        continue;
      }
      readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
      _RXPacketL = buffer[0];
      RXnetworkID = readUint16SXBuffer(_RXPacketL - 4);
      RXcrc = readUint16SXBuffer(_RXPacketL - 2);

      if ((_RXPacketL - 4) > headersize )                       //check passed buffer is big enough for header
      {
        setReliableRX(0);
        continue;
      }

      if (!bitRead(_ReliableConfig, NoReliableCRC))
      {
        if (payloadCRC != RXcrc)
        {
          bitSet(_ReliableErrors, ReliableCRCError);
          setReliableRX(0);
          continue;
        }
      }

      if ( (RXnetworkID == networkID))
      {
        bitSet(_ReliableFlags, ReliableACKReceived);

        checkBusy();

#ifdef USE_SPI_TRANSACTION
        SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

        digitalWrite(_NSS, LOW);                                //start the burst read
        SPI.transfer(RADIO_READ_BUFFER);
        SPI.transfer(0);
        SPI.transfer(0xFF);

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
        setReliableRX(0);
        continue;
      }
    }
  } while ( ((uint32_t) (millis() - startmS) < acktimeout));

  bitSet(_ReliableErrors, ReliableACKError);
  bitSet(_ReliableErrors, ReliableTimeout);
  return 0;
}


uint8_t SX128XLT::receiveDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait )
{
#ifdef SX128XDEBUGRELIABLE
  Serial.println();
  Serial.println(F(" {RELIABLE} receiveDT()"));
  Serial.print(F(" {RELIABLE} _ReliableConfig "));
  Serial.println(_ReliableConfig, HEX);
#endif

  uint16_t index, payloadcrc = 0, RXcrc, RXnetworkID = 0;
  uint8_t regdataL, regdataH;
  uint8_t RXHeaderL;
  uint8_t RXDataL;
  uint8_t buffer[2];
  uint8_t RXstart;

  _ReliableErrors = 0;
  _ReliableFlags = 0;
  setMode(MODE_STDBY_RC);
  setPayloadLength(127);                                 //set for maximum packet length in FLRC mode, packets might be filtered otherwise
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR), 0, 0);
  setRx(rxtimeout);

  if (!wait)
  {
    return 0;                                            //not wait requested so no packet length to pass
  }

  //0x4022   = IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_HEADER_ERROR
  while (!(readIrqStatus() & 0x4022 ));                  //wait for IRQs going active

  setMode(MODE_STDBY_RC);                                //ensure to stop further packet reception

  //IRQ_HEADER_ERROR + IRQ_CRC_ERROR + IRQ_RX_TX_TIMEOUT + IRQ_SYNCWORD_ERROR = 0x4068
  if (readIrqStatus() & 0x4068)
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.println(F(" {RELIABLE} Packet error"));
#endif
    return 0;                                            //packet is errored somewhere so return 0
  }

  RXHeaderL = getByteSXBuffer(2);
  RXDataL = getByteSXBuffer(3);

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];
  RXstart = buffer[1];

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} Header  "));
  printSXBufferHEX(0, (RXHeaderL - 1));
  Serial.println();
  Serial.print(F(" {RELIABLE} Received data payload size "));
  Serial.println(RXDataL);
  Serial.print(F(" {RELIABLE} Data payload  "));
  printSXBufferHEX(RXHeaderL, RXHeaderL + RXDataL - 1);
  Serial.println();
#endif

  if (RXHeaderL > headersize )
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} Header size error "));
    Serial.println(headersize);
#endif
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  if (RXDataL > datasize )
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} Data size error "));
    Serial.println(datasize);
#endif
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} _RXPacketL  "));
  Serial.println(_RXPacketL);
  Serial.print(F(" {RELIABLE} Header  "));
  printSXBufferHEX(0, (RXHeaderL - 1));
  Serial.println();
  Serial.print(F(" {RELIABLE} Received data payload size "));
  Serial.println(RXDataL);
  Serial.print(F(" {RELIABLE} Data payload  "));
  printSXBufferHEX(RXHeaderL, RXHeaderL + RXDataL - 1);
  Serial.println();
#endif

  if (_RXPacketL < 10)                                      //check received packet is 10 or more bytes long
  {
    //Serial.println(F(" {RELIABLE} _RXPacketL < 10"));
    bitSet(_ReliableErrors, ReliableSizeError);
    return 0;
  }

  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);             //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

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

#ifdef SX128XDEBUGRELIABLE
    Serial.println(F(" {RELIABLE} Payload CRC check enabled"));
    Serial.print(F(" {RELIABLE} payloadcrc 0x"));
    Serial.println(payloadcrc, HEX);
    Serial.print(F(" {RELIABLE} RXcrc 0x"));
    Serial.println(RXcrc, HEX);
#endif

    if (payloadcrc != RXcrc)
    {
      bitSet(_ReliableErrors, ReliableCRCError);
#ifdef SX128XDEBUGRELIABLE
      Serial.print(F(" {RELIABLE} CRCmissmatch, received 0x"));
      Serial.println(RXcrc, HEX);
#endif
    }
  }

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} RXnetworkID 0x"));
  Serial.println(RXnetworkID, HEX);
#endif

  if (RXnetworkID != networkID)
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} NetworkID missmatch received 0x"));
    Serial.print(RXnetworkID, HEX);
    Serial.print(F(" LocalID 0x"));
    Serial.println(networkID, HEX);
#endif
    bitSet(_ReliableErrors, ReliableIDError);
  }

  if (_ReliableErrors)                                            //if there has been a reliable error return a RX fail
  {
#ifdef SX128XDEBUGRELIABLE
    Serial.print(F(" {RELIABLE} Reliable errors"));
#endif

    return 0;
  }

  return _RXPacketL;                                              //return and indicate RX OK.
}


uint8_t SX128XLT::sendACKDTIRQ(uint8_t *header, uint8_t headersize, int8_t txpower)
{

#ifdef SX128XDEBUGRELIABLE
  Serial.print(F(" {RELIABLE} sendACKDT() "));
#endif

  uint32_t txtimeout = 12000;                                     //set TX timeout to 12 seconds, longest packet is 8.7secs
  uint8_t bufferdata, index;
  uint16_t networkID;
  uint16_t payloadCRC;

  setMode(MODE_STDBY_RC);
  _TXPacketL = headersize + 4;
  networkID = readUint16SXBuffer(_RXPacketL - 4);
  payloadCRC = readUint16SXBuffer(_RXPacketL - 2);
  checkBusy();

#ifdef USE_SPI_TRANSACTION
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

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

  setPayloadLength(_TXPacketL);
  setTxParams(txpower, RAMP_TIME);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);   //set for IRQ on TX done and timeout on DIO1
  setTx(txtimeout);

  //0x4001   = IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT
  while (!(readIrqStatus() & 0x4001 ));                         //wait for IRQs going active



  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                                  //check for timeout
  {
    bitSet(_ReliableFlags, ReliableTimeout);
    return 0;
  }

  bitSet(_ReliableFlags, ReliableACKSent);
  return _TXPacketL;                                                         //TX OK so return TXpacket length
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
