/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 13/09/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a demonstration of the transmission and acknowledgement of a 'Reliable'
  packet. 

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. The receiver needs to have the same
  NetworkID as configured for the transmitter since the receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended are a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended in the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  If the received packet is valid then a data payload together with the networkID and payload CRC are
  returned in a packet as an acknowledgement that the transmitter listens for. If the transmitter does not
  receive the acknowledgement within the ACKtimeout period, the original packet is re-transmitted until a
  valid acknowledgement is received. 
  
  With this example and the matching transmitter program, 215_Reliable_Transmitter_ACK_withData, the
  generation of the acknowledge by the receiver is manual and also returns data to the transmitter. This
  allows the transmitter to send a request to the receiver for it to return data. Since the acknowledge
  returns the networkID and payload CRC used in the original transmitted request, when the transmitter
  receives the acknowledge it can be very confident the data is geniune. 

  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library
#include <SX127XLT.h>                           //include the appropriate library   

SX127XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define DIO0 3                                  //DIO0 pin on LoRa device, used for RX and TX done 
#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using

#define ACKdelay 200                            //delay in mS before sending acknowledge, not used here so set to 0                      
#define RXtimeout 60000                         //receive timeout in mS.  
#define TXpower 2                               //dBm power to use for ACK   

const uint8_t RXBUFFER_SIZE = 251;              //RX buffer size, set to max payload length of 251, or maximum expected length
uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into

uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t PacketOK;                               //set to > 0 if packetOK
int16_t PacketRSSI;                             //stores RSSI of received packet
uint16_t LocalPayloadCRC;                       //locally calculated CRC of payload
uint16_t RXPayloadCRC;                          //CRC of payload received in packet
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter


void loop()
{
  PacketOK = LT.receiveReliable(RXBUFFER, RXBUFFER_SIZE, NetworkID, RXtimeout, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  RXPacketL = LT.readRXPacketL();               //get the received packet length
  RXPayloadL = RXPacketL - 4;                   //payload length is always 4 bytes less than packet length
  PacketRSSI = LT.readPacketRSSI();             //read the received packets RSSI value

  if (PacketOK > 0)
  {
    //if the LT.receiveReliable() returns a value > 0 for PacketOK then packet was received OK
  Serial.print(F("Payload received OK > "));
  LT.printASCIIPacket(RXBUFFER, RXPayloadL);
  Serial.println();
  packet_is_OK();
  sendACK();
  Serial.println();
  }
  else
  {
    //if the LT.receiveReliable() function detects an error PacketOK is 0
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


void sendACK()
{
 uint8_t buff[] = "Goodbye";                                          //the ack payload to send
 LocalPayloadCRC = LT.CRCCCITTReliable(0, RXPayloadL - 1, 0xFFFF);    //calculate received payload crc 
 delay(ACKdelay);
 LT.sendReliableACK(buff, sizeof(buff), NetworkID, LocalPayloadCRC, TXpower);
 Serial.print(F("Ack sent > "));
 LT.printASCIIPacket(buff, sizeof(buff));
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("216_Reliable_Receiver_ACK_withData Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    while (1);
  }

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO);   //configure frequency and LoRa settings

  Serial.println(F("Receiver ready"));
  Serial.println();
}
