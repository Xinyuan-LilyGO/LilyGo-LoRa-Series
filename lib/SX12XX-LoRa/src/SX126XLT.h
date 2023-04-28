/*
  Copyright 2019 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  17/12/19
  Revised 24/03/22
*/


/*
****************************************************************************
  To Do:


****************************************************************************
*/


#ifndef SX126XLT_h
#define SX126XLT_h

#include "Arduino.h"
#include <SX126XLT_Definitions.h>


class SX126XLT  {
  public:

    SX126XLT();

    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinRXEN, int8_t pinTXEN, int8_t pinSW, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinSW, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, uint8_t device);

    void checkBusy();
    void writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size );
    void readCommand( uint8_t Opcode, uint8_t *buffer, uint16_t size );
    void writeRegisters( uint16_t address, uint8_t *buffer, uint16_t size );
    void writeRegister( uint16_t address, uint8_t value );
    void readRegisters( uint16_t address, uint8_t *buffer, uint16_t size );
    uint8_t readRegister( uint16_t address );
    void resetDevice();
    bool checkDevice();
    void setupLoRa(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint8_t modParam4);
    void setMode(uint8_t modeconfig);
    void setRegulatorMode(uint8_t mode);

    void setPaConfig(uint8_t dutycycle, uint8_t hpMax, uint8_t device);
    void setDIO3AsTCXOCtrl(uint8_t tcxoVoltage);
    void calibrateDevice(uint8_t devices);
    void calibrateImage(uint32_t freq);
    void setDIO2AsRfSwitchCtrl();
    void setPacketType(uint8_t PacketType);
    void setRfFrequency( uint32_t frequency, int32_t offset );
    void setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint8_t  modParam4);
    void set_REGTXMODULATION();
    uint8_t returnOptimisation(uint8_t SpreadingFactor, uint8_t Bandwidth);
    uint32_t returnBandwidth(uint8_t BWregvalue);
    float calcSymbolTime(float Bandwidth, uint8_t SpreadingFactor);
    void setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);
    void setPacketParams(uint16_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5);
    void setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask );
    void setHighSensitivity();
    void setLowPowerRX();
    void setSyncWord(uint16_t syncword);
    void printModemSettings();
    void printDevice();
    uint32_t getFreqInt();                     //this reads the SX126x registers to get the current frequency
    uint8_t getLoRaSF();

    uint8_t getLoRaCodingRate();
    uint8_t getOptimisation();

    uint16_t getSyncWord();
    uint8_t getInvertIQ();
    uint16_t getPreamble();
    void printOperatingSettings();
    uint8_t getHeaderMode();
    uint8_t getLNAgain();
    void printRegisters(uint16_t Start, uint16_t End);
    void printASCIIPacket(uint8_t *buff, uint8_t tsize);
    uint8_t transmit(uint8_t *txbuffer, uint8_t size, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t transmitIRQ(uint8_t *txbuffer, uint8_t size, uint16_t timeout, int8_t txpower, uint8_t wait);
    void setTxParams(int8_t TXpower, uint8_t RampTime);
    void setTx(uint32_t timeout);
    void clearIrqStatus( uint16_t irq );
    uint16_t readIrqStatus();
    uint16_t CRCCCITT(uint8_t *buffer, uint32_t size, uint16_t start);
    uint8_t receive(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait);
    uint8_t receiveIRQ(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait);
    int16_t readPacketRSSI();
    int8_t readPacketSNR();
    uint8_t readRXPacketL();
    void setRx(uint32_t timeout);

    void printIrqStatus();
    void rxEnable();
    void txEnable();
    bool config();

    /***************************************************************************
      //Start direct access SX buffer routines
    ***************************************************************************/

    void startWriteSXBuffer(uint8_t ptr);
    uint8_t endWriteSXBuffer();
    void startReadSXBuffer(uint8_t ptr);
    uint8_t endReadSXBuffer();

    void writeUint8(uint8_t x);
    uint8_t readUint8();

    void writeInt8(int8_t x);
    int8_t readInt8();

    void writeInt16(int16_t x);
    int16_t readInt16();

    void writeUint16(uint16_t x);
    uint16_t readUint16();

    void writeInt32(int32_t x);
    int32_t readInt32();

    void writeUint32(uint32_t x);
    uint32_t readUint32();

    void writeFloat(float x);
    float readFloat();

    uint8_t transmitSXBuffer(uint8_t startaddr, uint8_t length, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    void writeBuffer(uint8_t *txbuffer, uint8_t size);
    void writeBufferChar(char *txbuffer, uint8_t size);
    uint8_t receiveSXBuffer(uint8_t startaddr, uint32_t rxtimeout, uint8_t wait);
    uint8_t readBuffer(uint8_t *rxbuffer);
    uint8_t readBufferChar(char *rxbuffer);
    void setupDirect(uint32_t frequency, int32_t offset);
    void toneFM(uint16_t frequency, uint32_t length, uint32_t deviation, float adjust, uint8_t txpower);
    void setTXDirect();
    uint8_t getByteSXBuffer(uint8_t addr);
    void printSXBufferHEX(uint8_t start, uint8_t end);
    int32_t getFrequencyErrorHz();
    int32_t getFrequencyErrorRegValue();
    void printHEXByte(uint8_t temp);
    uint8_t transmitAddressed(uint8_t *txbuffer, uint8_t size, char txpackettype, char txdestination, char txsource, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t readRXPacketType();
    uint8_t readRXDestination();
    uint8_t readRXSource();
    uint8_t receiveAddressed(uint8_t *rxbuffer, uint8_t size, uint32_t rxtimeout, uint8_t wait);
    void clearDeviceErrors();
    void printDeviceErrors();
    void printHEXPacket(uint8_t *buffer, uint8_t size);
    void printHEXByte0x(uint8_t temp);
    uint8_t readsavedModParam1();     //spreading factor
    uint8_t readsavedModParam2();     //bandwidth
    uint8_t readsavedModParam3();     //code rate
    uint8_t readsavedModParam4();     //optimisation
    uint8_t readsavedPower();
    uint8_t getPacketMode();
    uint8_t readsavedPacketParam1();  //preamble
    uint8_t readsavedPacketParam2();  //header type
    uint8_t readsavedPacketParam3();  //packet length
    uint8_t readsavedPacketParam4();  //CRC
    uint8_t readsavedPacketParam5();  //IQ
    uint8_t getOpmode();
    uint8_t getCRCMode();
    void fillSXBuffer(uint8_t startaddress, uint8_t size, uint8_t character);
    uint8_t readPacket(uint8_t *rxbuffer, uint8_t size);
    void writeByteSXBuffer(uint8_t addr, uint8_t regdata);
    void printSXBufferASCII(uint8_t start, uint8_t end);
    void startFSKRTTY(uint32_t freqshift, uint8_t pips, uint16_t pipPeriodmS, uint16_t pipDelaymS, uint16_t leadinmS);
    void transmitFSKRTTY(uint8_t chartosend, uint8_t databits, uint8_t stopbits, uint8_t parity, uint16_t baudPerioduS, int8_t pin);
    void transmitFSKRTTY(uint8_t chartosend, uint16_t baudPerioduS, int8_t pin);
    void printRTTYregisters();
    void endFSKRTTY();
    void getRfFrequencyRegisters(uint8_t *buff);
    void setRfFrequencyDirect(uint8_t high, uint8_t midhigh, uint8_t midlow, uint8_t low);


    /***************************************************************************
      //End direct access SX buffer routines
    ***************************************************************************/

    uint16_t CRCCCITTSX(uint8_t startadd, uint8_t endadd, uint16_t startvalue);
    void setSleep(uint8_t sleepconfig);
    void wake();


    //**********************************************************************************************
    // Reliable packet routines - added November 2021
    // Routines assume that RX and TX buffer base addresses are set to 0 by setupLoRa()
    //**********************************************************************************************

    void printASCIIArray(uint8_t *buffer, uint8_t size);
    uint8_t getReliableConfig(uint8_t bitread);
    void printReliableStatus();
    uint8_t transmitReliable(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint16_t getTXPayloadCRC(uint8_t length);
    uint16_t readUint16SXBuffer(uint8_t addr);
    void setPayloadLength(uint8_t length);
    uint8_t receiveReliable(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint16_t getRXNetworkID(uint8_t length);
    uint16_t getRXPayloadCRC(uint8_t length);
    uint8_t transmitSXReliable(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXReliable(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint16_t CRCCCITTReliable(uint8_t startadd, uint8_t endadd, uint16_t startvalue);
    void writeUint16SXBuffer(uint8_t addr, uint16_t regdata);
    uint8_t transmitReliableAutoACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t waitReliableACK(uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    uint8_t waitReliableACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    void setReliableRX(uint16_t timeout);
    uint8_t receiveReliableAutoACK(uint8_t *rxbuffer, uint8_t size, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendReliableACK(uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t sendReliableACK(uint8_t *txbuffer, uint8_t size, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t transmitSXReliableAutoACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXReliableAutoACK(uint8_t startaddr, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait );
    uint8_t waitSXReliableACK(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    uint8_t sendSXReliableACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t transmitSXReliableIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t waitSXReliableACKIRQ(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    uint8_t receiveSXReliableIRQ(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendSXReliableACKIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t transmitSXBufferIRQ(uint8_t startaddr, uint8_t length, uint16_t timeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXBufferIRQ(uint8_t startaddr, uint16_t timeout, uint8_t wait );
    void setReliableConfig(uint8_t bitset);
    void clearReliableConfig(uint8_t bitset);

    //***********************************************************************************
    //Data Transfer functions - Added December 2021
    //TX and RX base addresses assumed to be 0
    //***********************************************************************************

    uint8_t transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower);
    uint8_t receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout);
    uint16_t getTXNetworkID(uint8_t length);
    uint8_t readReliableErrors();
    uint8_t readReliableFlags();
    uint8_t sendACKDTIRQ(uint8_t *header, uint8_t headersize, int8_t txpower);
    uint8_t waitACKDTIRQ(uint8_t *header, uint8_t headersize, uint32_t acktimeout);
    uint8_t receiveDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t transmitDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);


  private:

    int8_t _NSS, _NRESET, _RFBUSY, _DIO1, _DIO2, _DIO3, _SW;
    int8_t _RXEN, _TXEN;
    uint8_t _RXPacketL;             //length of packet received
    uint8_t _RXPacketType;          //type number of received packet
    uint8_t _RXDestination;         //destination address of received packet
    uint8_t _RXSource;              //source address of received packet
    int8_t  _TXPacketL;             //length of packet transmitted
    uint8_t _RXcount;               //used to keep track of the bytes read from SX126X buffer during readFloat() etc
    uint8_t _TXcount;               //used to keep track of the bytes written to SX126X buffer during writeFloat() etc
    uint8_t _RXBufferPointer;       //pointer to first byte of packet in buffer
    uint8_t _OperatingMode;         //current operating mode
    bool _rxtxpinmode = false;      //set to true if RX and TX enable pin mode is used.
    uint8_t _Device;                //saved device type
    uint8_t _TXDonePin;             //the pin that will indicate TX done
    uint8_t _RXDonePin;             //the pin that will indicate RX done

    uint32_t savedFrequency;
    int32_t savedOffset;
    uint8_t  savedPacketType;
    uint8_t  savedRegulatorMode;

    uint8_t  savedModParam1, savedModParam2, savedModParam3, savedModParam4;
    uint16_t savedPacketParam1;
    uint8_t  savedPacketParam2, savedPacketParam3, savedPacketParam4, savedPacketParam5;
    uint16_t savedIrqMask, savedDio1Mask, savedDio2Mask, savedDio3Mask;
    int8_t   savedTXPower;

    uint32_t savedFrequencyReg;
    uint8_t _freqregH, _freqregMH, _freqregML, _freqregL; //the registers values for the set frequency
    uint8_t _ShiftfreqregH, _ShiftfreqregMH, _ShiftfreqregML, _ShiftfreqregL;  //register values for shifted frequency, used in FSK

    uint8_t _ReliableErrors;        //Reliable status byte
    uint8_t _ReliableFlags;         //Reliable flags byte
    uint8_t _ReliableConfig;        //Reliable config byte

};
#endif
