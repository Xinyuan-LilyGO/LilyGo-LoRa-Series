/*
  Copyright 2021 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 17/12/19
  New version 23/12/20, add support for PA_BOOST
  New version, 24/08/21, Reliable packets added
  New version, 19/09/21, Data Transfer packets added, support for no DIO0 operation added
  21/11/21, new begin() function added for NSS and NRESET
*/


/*
****************************************************************************
  To Do:


****************************************************************************
*/



#ifndef SX127XLT_h
#define SX127XLT_h

#include <Arduino.h>
#include <SX127XLT_Definitions.h>


class SX127XLT
{

  public:

    SX127XLT();

    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, int8_t pinDIO1, int8_t pinDIO2, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, uint8_t device);
    bool begin(int8_t pinNSS, uint8_t device);
    void resetDevice();
    void setMode(uint8_t modeconfig);
    void setSleep(uint8_t sleepconfig);
    bool checkDevice();
    void wake();
    void calibrateImage(uint8_t null);
    uint16_t CRCCCITT(uint8_t *buffer, uint32_t size, uint16_t startvalue);
    uint16_t CRCCCITTSX(uint8_t startadd, uint8_t endadd, uint16_t startvalue);
    void setDevice(uint8_t type);
    void printDevice();
    uint8_t getOperatingMode();
    bool isReceiveDone();
    bool isTransmitDone();
    void writeRegister( uint8_t address, uint8_t value );
    uint8_t readRegister( uint8_t address );
    void printRegisters(uint16_t start, uint16_t end);
    void printRegister(uint8_t reg);
    void printOperatingMode();
    void printOperatingSettings();
    void setTxParams(int8_t txPower, uint8_t rampTime);
    void setPacketParams(uint16_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5);
    void setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint8_t  modParam4);
    void setRfFrequency(uint64_t freq64, int32_t offset);
    uint32_t getFreqInt();
    int32_t getFrequencyErrorRegValue();
    int32_t getFrequencyErrorHz();
    void setTx(uint32_t timeout);
    void setRx(uint32_t timeout);
    bool readTXIRQ();
    bool readRXIRQ();
    void setLowPowerReceive();
    void setHighSensitivity();
    void setRXGain(uint8_t config);
    uint8_t getAGC();
    uint8_t getLNAgain();
    uint8_t getCRCMode();
    uint8_t getHeaderMode();
    uint8_t getLNAboostHF();
    uint8_t getLNAboostLF();
    uint8_t getOpmode();
    uint8_t getPacketMode();
    uint8_t readRXPacketL();
    uint8_t readTXPacketL();
    int16_t readPacketRSSI();
    int16_t readCurrentRSSI();
    int8_t readPacketSNR();
    bool readPacketCRCError();
    bool readPacketHeaderValid();
    uint8_t packetOK();
    uint8_t readRXPacketType();
    uint8_t readRXDestination();
    uint8_t readRXSource();
    void setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);
    void setPacketType(uint8_t PacketType);
    void clearIrqStatus( uint16_t irqMask );
    uint16_t readIrqStatus();
    void setDioIrqParams(uint16_t irqMask, uint16_t dio0Mask, uint16_t dio1Mask, uint16_t dio2Mask );
    void printIrqStatus();
    void printASCIIPacket(uint8_t *buff, uint8_t tsize);
    void printHEXPacket(uint8_t *buff, uint8_t tsize);
    void printASCIIorHEX(uint8_t temp);
    void printHEXByte(uint8_t temp);
    void printHEXByte0x(uint8_t temp);
    bool isRXdone();
    bool isTXdone();
    bool isRXdoneIRQ();
    bool isTXdoneIRQ();
    void setTXDonePin(uint8_t pin);
    void setRXDonePin(uint8_t pin);

    //*******************************************************************************
    //Packet Read and Write Routines
    //*******************************************************************************

    uint8_t receive(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait);
    uint8_t receiveIRQ(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait);
    uint8_t receiveAddressed(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait);
    uint8_t readPacket(uint8_t *rxbuffer, uint8_t size);
    uint8_t readPacketAddressed(uint8_t *rxbuffer, uint8_t size);
    uint8_t transmit(uint8_t *txbuffer, uint8_t size, uint32_t txtimeout, int8_t txPower, uint8_t wait);
    uint8_t transmitIRQ(uint8_t *txbuffer, uint8_t size, uint32_t txtimeout, int8_t txPower, uint8_t wait);
    uint8_t transmitAddressed(uint8_t *txbuffer, uint8_t size, char txpackettype, char txdestination, char txsource, uint32_t txtimeout, int8_t txpower, uint8_t wait);

    //*******************************************************************************
    //LoRa specific routines
    //*******************************************************************************

    void setupLoRa(uint32_t Frequency, int32_t Offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint8_t modParam4);
    void setupLoRa(uint32_t Frequency);
    uint8_t getLoRaSF();
    uint8_t getLoRaCodingRate();
    uint8_t getOptimisation();
    uint8_t getSyncWord();
    uint8_t getInvertIQ();
    uint8_t getVersion();
    uint16_t getPreamble();
    uint32_t returnBandwidth(uint8_t BWregvalue);
    uint8_t returnOptimisation(uint8_t SpreadingFactor, uint8_t Bandwidth);
    float calcSymbolTime(float Bandwidth, uint8_t SpreadingFactor);
    void printModemSettings();
    void setSyncWord(uint8_t syncword);
    void setTXDirect();
    void setupDirect(uint32_t frequency, int32_t offset);
    void toneFM(uint16_t frequency, uint32_t length, uint32_t deviation, float adjust, int8_t txpower);
    int8_t getDeviceTemperature();
    void fskCarrierOn(int8_t txpower);
    void fskCarrierOff();
    void setRfFrequencyDirect(uint8_t high, uint8_t mid, uint8_t low);
    void getRfFrequencyRegisters(uint8_t *buff);
    void startFSKRTTY(uint32_t freqshift, uint8_t pips, uint16_t pipDelaymS, uint16_t pipPeriodmS, uint16_t leadinmS);
    void transmitFSKRTTY(uint8_t chartosend, uint8_t databits, uint8_t stopbits, uint8_t parity, uint16_t baudPerioduS, int8_t pin);
    void transmitFSKRTTY(uint8_t chartosend, uint16_t baudPerioduS, int8_t pin);
    void printRTTYregisters();
    void endFSKRTTY();
    void doAFC();
    void doAFCPPM();
    uint8_t getPPM();
    int32_t getOffset();
    void printOCPTRIM();
    uint8_t readBufferbytes(uint8_t *rxbuffer, uint8_t size);
    uint8_t writeBufferbytes(uint8_t *txbuffer, uint8_t size);
    void setDevicePABOOST();
    void setDeviceRFO();

    //*******************************************************************************
    //Read Write SX12xxx Buffer commands, this is the buffer internal to the SX12xxxx
    //*******************************************************************************

    uint8_t receiveSXBuffer(uint8_t startaddr, uint32_t rxtimeout, uint8_t wait);
    uint8_t transmitSXBuffer(uint8_t startaddr, uint8_t length, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXBufferIRQ(uint8_t startaddr, uint32_t rxtimeout, uint8_t wait);
    uint8_t transmitSXBufferIRQ(uint8_t startaddr, uint8_t length, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    void printSXBufferHEX(uint8_t start, uint8_t end);
    void printSXBufferASCII(uint8_t start, uint8_t end);
    void fillSXBuffer(uint8_t startaddress, uint8_t size, uint8_t character);
    uint8_t getByteSXBuffer(uint8_t addr);
    void writeByteSXBuffer(uint8_t addr, uint8_t regdata);
    void startWriteSXBuffer(uint8_t ptr);
    uint8_t endWriteSXBuffer();
    void startReadSXBuffer(uint8_t ptr);
    uint8_t endReadSXBuffer();

    void writeUint8(uint8_t x);
    uint8_t readUint8();

    void writeInt8(int8_t x);
    int8_t readInt8();

    void writeChar(char x);
    char readChar();

    void writeUint16(uint16_t x);
    uint16_t readUint16();

    void writeInt16(int16_t x);
    int16_t readInt16();

    void writeUint32(uint32_t x);
    uint32_t readUint32();

    void writeInt32(int32_t x);
    int32_t readInt32();

    void writeFloat(float x);
    float readFloat();

    void writeBuffer(uint8_t *txbuffer, uint8_t size);
    void writeBufferChar(char *txbuffer, uint8_t size);
    void writeBufferChar(char *txbuffer);
    uint8_t readBuffer(uint8_t *rxbuffer);               //reads buffer till a null 0x00 is reached
    uint8_t readBuffer(uint8_t *rxbuffer, uint8_t size);
    uint8_t readBufferChar(char *rxbuffer);

    //*******************************************************************************
    //Reliable RX\TX routines
    //*******************************************************************************

    uint8_t transmitReliable(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveReliable(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );

    uint8_t transmitReliableAutoACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveReliableAutoACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait );

    uint8_t sendReliableACK(uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t sendReliableACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);

    uint8_t waitReliableACK(uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    uint8_t waitReliableACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);

    uint8_t transmitSXReliable(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t transmitSXReliableAutoACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXReliable(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t receiveSXReliableAutoACK(uint8_t startaddr, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendSXReliableACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t waitSXReliableACK(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);

    uint8_t transmitSXReliableIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXReliableIRQ(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendSXReliableACKIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t waitSXReliableACKIRQ(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);

    void writeUint16SXBuffer(uint8_t addr, uint16_t regdata);
    uint16_t readUint16SXBuffer(uint8_t addr);

    uint8_t readReliableErrors();
    uint8_t readReliableFlags();
    uint8_t readReliableConfig();

    void setReliableConfig(uint8_t bitset);
    void clearReliableConfig(uint8_t bitclear);
    uint8_t getReliableConfig(uint8_t bitread);

    void printReliableConfig();
    void printReliableStatus();
    void setReliableRX();

    uint16_t CRCCCITTReliable(uint8_t startadd, uint8_t endadd, uint16_t startvalue);
    void writeArray(uint8_t *txbuffer, uint8_t size);
    void printASCIIArray(uint8_t *buffer, uint8_t size);

    uint16_t getRXPayloadCRC(uint8_t length);          //cannot rely on _RXPacketL, since some SX reads adjust it
    uint16_t getTXPayloadCRC(uint8_t length);          //cannot rely on _TXPacketL, since some SX reads adjust it
    uint16_t getRXNetworkID(uint8_t length);           //cannot rely on _RXPacketL, since some SX reads adjust it
    uint16_t getTXNetworkID(uint8_t length);           //cannot rely on _TXPacketL, since some SX reads adjust it

    //*******************************************************************************
    //Data Array - File Transfer functions
    //*******************************************************************************

    uint8_t transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower);
    uint8_t waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout);

    uint8_t transmitDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendACKDTIRQ(uint8_t *header, uint8_t headersize, int8_t txpower);
    uint8_t waitACKDTIRQ(uint8_t *header, uint8_t headersize, uint32_t acktimeout);

    //*******************************************************************************
    //RX\TX Enable routines - Not yet tested as of 02/12/19
    //*******************************************************************************

    void rxtxInit(int8_t pinRXEN, int8_t pinTXEN);  //not used on current SX127x modules
    void rxEnable();                                //not used on current SX127x modules
    void txEnable();                                //not used on current SX127x modules

    //*******************************************************************************
    //Library variables
    //*******************************************************************************

  private:

    int8_t _NSS, _NRESET, _DIO0, _DIO1, _DIO2;
    uint8_t _RXPacketL;             //length of packet received
    uint8_t _RXPacketType;          //type number of received packet
    uint8_t _RXDestination;         //destination address of received packet
    uint8_t _RXSource;              //source address of received packet
    uint8_t  _TXPacketL;            //length of transmitted packet
    uint16_t _IRQmsb;               //for setting additional flags
    uint8_t _Device;                //saved device type
    int8_t _TXDonePin;              //the pin that will indicate TX done
    int8_t _RXDonePin;              //the pin that will indicate RX done
    uint8_t _UseCRC;                //when packet parameters are set this flag is set if CRC on packets in use
    int8_t _RXEN, _TXEN;            //for modules that have RX TX pin switching
    uint8_t _PACKET_TYPE;           //used to save the set packet type
    uint8_t _freqregH, _freqregM, _freqregL;                 //the registers values for the set frequency
    uint8_t _ShiftfreqregH, _ShiftfreqregM, _ShiftfreqregL;  //register values for shifted frequency, used in FSK RTTY etc
    uint32_t _savedFrequency;       //when setRfFrequency() is used the set frequency is saved
    int32_t _savedOffset;           //when setRfFrequency() is used the set offset is saved
    uint8_t _ReliableErrors;        //Reliable status byte
    uint8_t _ReliableFlags;         //Reliable flags byte
    uint8_t _ReliableConfig;        //Reliable config byte

};
#endif


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
