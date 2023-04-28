/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 05/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a demonstration of using reliable packets to request data from a remote
  station. Each stations needs a number defined in 'ThisStation' and are numbered 0 to 255.


  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. The receiver needs to have the same
  NetworkID as configured for the transmitter since the receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended are a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended in the packet. The receiver is thus able to check if the payload is valid.

  The transmitter sends a payload to the receiver which is a request for the receiver to return some data.
  The payload request is just 2 bytes, the request type and the station the request is directed to.

  If the received packet is valid then a data payload together with the networkID and payload CRC are
  returned in a packet as an acknowledgement that the transmitter listens for. If the transmitter does not
  receive the acknowledgement within the ACKtimeout period, the original packet is re-transmitted until a
  valid acknowledgement is received.

  With this example and the matching transmitter program, 217_Reliable_Transmitter_Data_Requestor, the
  generation of the acknowledge by the receiver is manual and also returns data to the transmitter. This
  allows the transmitter to send a request to the receiver for it to return data. Since the acknowledge
  returns the networkID and payload CRC used in the original transmitted request, when the transmitter
  receives the acknowledge it can be very confident the data is geniune.

  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library
#include <SX128XLT.h>                           //include the appropriate library   

SX128XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 

#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using

#define ACKdelay 200                            //delay in mS before sending reply                      
#define RXtimeout 60000                         //receive timeout in mS.  
#define TXpower 2                               //dBm power to use for ACK   

const uint8_t RXBUFFER_SIZE = 251;              //RX buffer size, set to max payload length of 251, or maximum expected length
uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into

uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t TXPayloadL;                             //stores length of payload sent
int16_t PacketRSSI;                             //stores RSSI of received packet
uint16_t LocalPayloadCRC;                       //locally calculated CRC of payload
uint16_t RXPayloadCRC;                          //CRC of payload received in packet
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet
uint16_t StationNumber;                         //station number request is addressed to
uint8_t RequestType;                            //type of request, GPS location used in this example
uint8_t startaddr = 0;                          //address in SX buffer to start packet

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t ThisStation = 123;                //the number of this station
const uint8_t RequestGPSLocation = 1;           //request type for GPS location

//GPS co-ordinates to use for the GPS location request
const float TestLatitude  = 51.48230;           //GPS co-ordinates to use for test
const float TestLongitude  = -3.18136;          //Cardiff castle keep, used for testing purposes
const float TestAltitude = 25.5;


void loop()
{

  if (LT.receiveSXReliable(startaddr, NetworkID, RXtimeout, WAIT_RX))
  {
    Serial.print(F("Reliable packet received > "));
    LT.startReadSXBuffer(startaddr);               //start buffer read at location startaddr
    RequestType = LT.readUint8();                  //get the request type
    StationNumber = LT.readUint8();                //get the station number for the request
    RXPayloadL = LT.endReadSXBuffer();             //this function returns the length of the array read

    RXPacketL = LT.readRXPacketL();                //get the received packet length
    RXPayloadL = RXPacketL - 4;                    //payload length is always 4 bytes less than packet length
    PacketRSSI = LT.readPacketRSSI();              //read the received packets RSSI value

    Serial.print(F("Request "));
    Serial.print(RequestType);
    Serial.print(F(" for station "));
    Serial.println(StationNumber);

    packet_is_OK();

    if (StationNumber == ThisStation)
    {
      actionRequest();
    }
    else
    {
      Serial.print(F("Request not for this station "));
      Serial.println(StationNumber);
    }

  }
  else
  {
    packet_is_Error();
  }

  Serial.println();
}


void packet_is_OK()
{
  printPacketDetails();
  Serial.println();
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register
  Serial.print(F("Error "));

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
  {
    Serial.print(F(" RXTimeout "));
  }
  else
  {
    printPacketDetails();
  }
}


void printPacketDetails()
{
  LocalPayloadCRC = LT.CRCCCITT(RXBUFFER, RXPayloadL, 0xFFFF);  //calculate payload crc from the received RXBUFFER
  TransmitterNetworkID = LT.getRXNetworkID(RXPacketL);
  RXPayloadCRC = LT.getRXPayloadCRC(RXPacketL);

  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmitterNetworkID,0x"));
  Serial.print(TransmitterNetworkID, HEX);
  Serial.print(F(",LocalPayloadCRC,0x"));
  Serial.print(LocalPayloadCRC, HEX);
  Serial.print(F(",RXPayloadCRC,0x"));
  Serial.print(RXPayloadCRC, HEX);
  LT.printReliableStatus();
}


void actionRequest()
{
  LocalPayloadCRC = LT.getRXPayloadCRC(RXPacketL);       //fetch received payload crc

  if (RequestType == RequestGPSLocation)
  {
    LT.startWriteSXBuffer(startaddr);           //initialise SX buffer write at address startaddr
    LT.writeUint8(RequestGPSLocation);          //identify type of request
    LT.writeUint8(ThisStation);                 //who is the request reply from
    LT.writeFloat(TestLatitude);                //add latitude
    LT.writeFloat(TestLongitude);               //add longitude
    LT.writeFloat(TestAltitude);                //add altitude
    TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

    delay(ACKdelay);
    LT.sendSXReliableACK(startaddr, TXPayloadL, NetworkID, LocalPayloadCRC, TXpower);
    Serial.println(F("Request replied"));
    Serial.print(F("TXPayloadL "));
    Serial.println(TXPayloadL);
  }

}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("218_Reliable_Receiver_Data_Requestor Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    while (1);
  }

  LT.setupLoRa(2445000000, 0, LORA_SF5, LORA_BW_1600, LORA_CR_4_5);      //configure frequency and LoRa settings

  Serial.println(F("Receiver ready"));
  Serial.println();
}
