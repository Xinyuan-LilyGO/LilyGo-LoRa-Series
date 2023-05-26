#ifndef SX128XLT_h
#define SX128XLT_h

#include "Arduino.h"
#include <SX128XLT_Definitions.h>


//check if (size > 251 ) for receive reliable

class SX128XLT  {

  public:

    SX128XLT();

    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinRFBUSY, uint8_t device);

    void rxEnable();
    void txEnable();


    void checkBusy();
    bool config();
    void readRegisters( uint16_t address, uint8_t *buffer, uint16_t size );
    uint8_t readRegister( uint16_t address );
    void writeRegisters( uint16_t address, uint8_t *buffer, uint16_t size );
    void writeRegister( uint16_t address, uint8_t value );
    void writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size );
    void readCommand( uint8_t Opcode, uint8_t *buffer, uint16_t size );
    void resetDevice();
    bool checkDevice();
    void setupLoRa(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3);
    void setMode(uint8_t modeconfig);
    void setRegulatorMode(uint8_t mode);
    void setPacketType(uint8_t PacketType);
    void setRfFrequency( uint32_t frequency, int32_t offset );
    void setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);
    void setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3);
    void setPacketParams(uint8_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5, uint8_t packetParam6, uint8_t packetParam7);
    void setPacketParams(uint8_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5);
    void setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask );
    void setHighSensitivity();
    void setLowPowerRX();
    void printModemSettings();
    void printDevice();
    uint32_t getFreqInt();
    uint8_t getLoRaSF();
    uint32_t returnBandwidth(uint8_t data);
    uint8_t getLoRaCodingRate();
    uint8_t getInvertIQ();
    uint16_t getPreamble();
    void printOperatingSettings();
    uint8_t getLNAgain();
    void printRegisters(uint16_t Start, uint16_t End);
    void printASCIIPacket(uint8_t *buff, uint8_t tsize);
    uint8_t transmit(uint8_t *txbuffer, uint8_t size, uint16_t timeout, int8_t txpower, uint8_t wait);
    uint8_t transmitIRQ(uint8_t *txbuffer, uint8_t size, uint16_t timeout, int8_t txpower, uint8_t wait);
    void setTxParams(int8_t TXpower, uint8_t RampTime);
    void setTx(uint16_t timeout);
    void clearIrqStatus( uint16_t irq );
    uint16_t readIrqStatus();
    void printIrqStatus();
    uint16_t CRCCCITT(uint8_t *buffer, uint32_t size, uint16_t start);
    uint8_t receive(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait);
    uint8_t receiveIRQ(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait);
    int16_t readPacketRSSI2();
    int16_t readPacketRSSI();
    int8_t readPacketSNR();
    uint8_t readRXPacketL();
    void setRx(uint16_t timeout);
    void setSyncWord1(uint32_t syncword);
    void setSyncWord2(uint32_t syncword);
    void setSyncWord3(uint32_t syncword);
    void setSyncWordErrors(uint8_t errors);
    void setSleep(uint8_t sleepconfig);
    uint16_t CRCCCITTSX(uint8_t startadd, uint8_t endadd, uint16_t startvalue);
    uint8_t getByteSXBuffer(uint8_t addr);
    int32_t getFrequencyErrorRegValue();
    int32_t getFrequencyErrorHz();
    void printHEXByte(uint8_t temp);
    void wake();
    uint8_t transmitAddressed(uint8_t *txbuffer, uint8_t size, char txpackettype, char txdestination, char txsource, uint32_t timeout, int8_t txpower, uint8_t wait);
    uint8_t receiveAddressed(uint8_t *rxbuffer, uint8_t size, uint16_t timeout, uint8_t wait);
    uint8_t readRXPacketType();
    uint8_t readPacket(uint8_t *rxbuffer, uint8_t size);
    void setPeriodBase(uint8_t value);
    uint8_t getPeriodBase();


    //***************************************************************************
    //Start direct access SX buffer routines
    //***************************************************************************
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

    uint8_t transmitSXBuffer(uint8_t startaddr, uint8_t length, uint16_t timeout, int8_t txpower, uint8_t wait);
    uint8_t transmitSXBufferIRQ(uint8_t startaddr, uint8_t length, uint16_t timeout, int8_t txpower, uint8_t wait);
    void writeBuffer(uint8_t *txbuffer, uint8_t size);
    //void writeBuffer(uint8_t *txbuffer, uint8_t startaddr, uint8_t size);
    uint8_t receiveSXBuffer(uint8_t startaddr, uint16_t timeout, uint8_t wait);
    uint8_t receiveSXBufferIRQ(uint8_t startaddr, uint16_t timeout, uint8_t wait );
    uint8_t readBuffer(uint8_t *rxbuffer);
    //uint8_t readBuffer(uint8_t *rxbuffer, uint8_t startaddr, uint8_t len);
    void printSXBufferHEX(uint8_t start, uint8_t end);
    void writeBufferChar(char *txbuffer, uint8_t size);
    uint8_t readBufferChar(char *rxbuffer);


    //***************************************************************************
    //Start ranging routines
    //***************************************************************************
    void setRangingSlaveAddress(uint32_t address);
    void setRangingMasterAddress(uint32_t address);
    void setRangingCalibration(uint16_t cal);
    void setRangingRole(uint8_t role);
    double getRangingDistance(uint8_t resultType, int32_t regval, float adjust);
    uint32_t getRangingResultRegValue(uint8_t resultType);
    int32_t complement2( uint32_t num, uint8_t bitCnt );
    bool setupRanging(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint32_t address, uint8_t role);
    bool transmitRanging(uint32_t address, uint16_t timeout, int8_t txpower, uint8_t wait);
    uint8_t receiveRanging(uint32_t address, uint16_t timeout, int8_t txpower, uint8_t wait);
    uint16_t lookupCalibrationValue(uint8_t spreadingfactor, uint8_t bandwidth);
    uint16_t getSetCalibrationValue();
    int16_t getRangingRSSI();


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

    void printASCIIArray(uint8_t *buffer, uint8_t size);
    uint8_t getReliableConfig(uint8_t bitread);

    uint16_t getTXPayloadCRC(uint8_t length);
    void printReliableStatus();
    uint16_t getRXPayloadCRC(uint8_t length);
    uint16_t getRXNetworkID(uint8_t length);
    uint16_t getTXNetworkID(uint8_t length);
    void writeUint16SXBuffer(uint8_t addr, uint16_t regdata);
    uint16_t readUint16SXBuffer(uint8_t addr);

    void printHEXPacket(uint8_t *buffer, uint8_t size);
    void printHEXPacket(char *buffer, uint8_t size);
    void printArrayHEX(uint8_t *buffer, uint8_t size);
    void printArrayHEX(char *buffer, uint8_t size);
    void setReliableConfig(uint8_t bitset);
    void clearReliableConfig(uint8_t bitset);

    uint8_t getPacketType();
    void setReliableRX(uint16_t timeout);

    void setupFLRC(uint32_t frequency, int32_t offset, uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3, uint32_t syncword);
    void setFLRCPayloadLengthReg(uint8_t length);
    void setLoRaPayloadLengthReg(uint8_t length);
    void setPayloadLength(uint8_t length);
    uint16_t CRCCCITTReliable(uint8_t startadd, uint8_t endadd, uint16_t startvalue);

    uint8_t transmitSXReliable(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t transmitSXReliableIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXReliable(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t receiveSXReliableIRQ(uint8_t startaddr, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t transmitSXReliableAutoACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint32_t acktimeout, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveSXReliableAutoACK(uint8_t startaddr, uint16_t networkID, uint32_t ackdelay, int8_t txpower, uint32_t rxtimeout, uint8_t wait );
    uint8_t waitSXReliableACK(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    uint8_t waitSXReliableACKIRQ(uint8_t startaddr, uint16_t networkID, uint16_t payloadcrc, uint32_t acktimeout);
    uint8_t sendSXReliableACK(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t sendSXReliableACKIRQ(uint8_t startaddr, uint8_t length, uint16_t networkID, uint16_t payloadcrc, int8_t txpower);
    uint8_t readReliableErrors();
    uint8_t readReliableFlags();

    //*******************************************************************************
    //Data Transfer functions
    //*******************************************************************************
    uint8_t transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout);
    uint8_t receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower);

    uint8_t transmitDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t waitACKDTIRQ(uint8_t *header, uint8_t headersize, uint32_t acktimeout);
    uint8_t receiveDTIRQ(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t datasize, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendACKDTIRQ(uint8_t *header, uint8_t headersize, int8_t txpower);


  private:

    int8_t _NSS, _NRESET, _RFBUSY, _DIO1, _DIO2, _DIO3;
    int8_t _RXEN, _TXEN;
    uint8_t _RXPacketL;             //length of packet received
    uint8_t _RXPacketType;          //type number of received packet
    uint8_t _RXDestination;         //destination address of received packet
    uint8_t _RXSource;              //source address of received packet
    //int8_t  _PacketRSSI;          //RSSI of received packet - removed November 2021, not used
    //int8_t  _PacketSNR;           //signal to noise ratio of received packet - removed November 2021, not used
    int8_t  _TXPacketL;             //transmitted packet length
    uint8_t _RXcount;               //used to keep track of the bytes read from SX1280 buffer during readFloat() etc
    uint8_t _TXcount;               //used to keep track of the bytes written to SX1280 buffer during writeFloat() etc
    uint8_t _OperatingMode;         //current operating mode
    bool _rxtxpinmode = false;      //set to true if RX and TX pin mode is used.

    uint8_t _Device;                //saved device type
    uint8_t _TXDonePin;             //the pin that will indicate TX done
    uint8_t _RXDonePin;             //the pin that will indicate RX done
    uint8_t _PERIODBASE = PERIODBASE_01_MS;

    uint8_t  savedRegulatorMode;
    uint8_t  savedPacketType;
    uint32_t savedFrequency;
    int32_t  savedOffset;
    uint8_t  savedModParam1, savedModParam2, savedModParam3; //sequence is spreading factor, bandwidth, coding rate
    uint8_t  savedPacketParam1, savedPacketParam2, savedPacketParam3, savedPacketParam4, savedPacketParam5, savedPacketParam6, savedPacketParam7;
    uint16_t savedIrqMask, savedDio1Mask, savedDio2Mask, savedDio3Mask;
    int8_t   savedTXPower;
    uint16_t savedCalibration;
    uint32_t savedFrequencyReg;

    uint8_t _ReliableErrors;        //Reliable status byte
    uint8_t _ReliableFlags;         //Reliable flags byte
    uint8_t _ReliableConfig;        //Reliable config byte

};
#endif
