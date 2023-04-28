/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 10/03/22

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

  The transmitter sends a payload to the receiver which is a request for the receiver to return some data.
  The payload request is just 2 bytes, the request type and the station the request is directed to.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  If the received packet is valid then a data payload together with the networkID and payload CRC are
  returned in a packet as an acknowledgement that the transmitter listens for. If the transmitter does not
  receive the acknowledgement within the ACKtimeout period, the original packet is re-transmitted until a
  valid acknowledgement is received.

  With this example and the matching receiver program, 220_Reliable_Receiver_Data_RequestorIRQ, the
  generation of the acknowledge by the receiver is manual and also returns data to the transmitter. This
  allows the transmitter to send a request to the receiver for it to return data. Since the acknowledge
  returns the networkID and payload CRC used in the original transmitted request, when the transmitter
  receives the acknowledge it can be very confident the data is geniune.


  This is a version of example 217_Reliable_Transmitter_Data_Requestor.ino that does not require the use
  of the DIO1 pin to check for transmit done. In addition no NRESET pin is needed either, so its a program
  for use with a minimum pin count Arduino. Leave the DIO1 and NRESET pins on the LoRa device not
  connected.


  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX126XLT.h>                           //include the appropriate library  

SX126XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device

#define LORA_DEVICE DEVICE_SX1262               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm

#define ACKtimeout 1000                         //Acknowledge timeout in mS, set to 0 if ACK not used.                      
#define TXtimeout 1000                          //transmit timeout in mS. If 0 return from transmit function after send.  

uint8_t TXPacketL;                              //length of transmitted packet
uint8_t TXPayloadL;
uint8_t RXPayloadL;
uint8_t RXPacketL;                              //length of received acknowledge
uint16_t PayloadCRC;
uint8_t RequestStation;

const uint8_t RXBUFFER_SIZE = 16;               //RX buffer size, set to max payload length of 251, or maximum expected length
uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into
uint8_t RequestType;
uint8_t Station;
uint8_t buff[] = "Hello World";                 //the payload to send

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t RequestGPSLocation = 1;           //request number for GPS location, so receiver knows what information to supply


float Latitude;
float Longitude;
float Altitude;
uint8_t startaddr = 0;                          //address in SX buffer to start packet


void loop()
{

  do
  {
    RequestStation = 123;                       //the number of station to reply to request

    Serial.print(F("Transmit Request "));
    Serial.flush();

    //build the request payload
    LT.startWriteSXBuffer(startaddr);           //initialise SX buffer write at address 0
    LT.writeUint8(RequestGPSLocation);          //identify type of packet
    LT.writeUint8(RequestStation);              //station to reply to request
    TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

    PayloadCRC = LT.CRCCCITT(buff, sizeof(buff), 0xFFFF);

    //now transmit the request
    TXPacketL = LT.transmitSXReliableIRQ(startaddr, TXPayloadL, NetworkID, TXtimeout, TXpower, WAIT_TX);

    PayloadCRC = LT.getTXPayloadCRC(TXPacketL);

    RXPacketL = LT.waitSXReliableACKIRQ(startaddr, NetworkID, PayloadCRC, ACKtimeout);

    if (RXPacketL > 0)
    {
      //if waitReliableACK() returns > 0 then ack was received
      packet_is_OK();
      Serial.println();
      Serial.print(F("Reply Received > "));
      actionReply();
      Serial.println();
    }
    else
    {
      //if transmitReliable() returns 0 there was an error
      packet_is_Error();
      Serial.println();
      Serial.println(F("No Ack Received"));
      Serial.println();
    }

    delay(200);                                        //small delay between tranmission attampts
  }
  while (RXPacketL == 0);

  Serial.println();
  delay(5000);                                         //have a delay between packets
}


void packet_is_OK()
{
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmittedPayloadCRC,0x"));        //print CRC of transmitted packet
  Serial.print(PayloadCRC, HEX);
}


void packet_is_Error()
{
  Serial.print(F("SendError"));
  LT.printIrqStatus();                                 //prints the text of which IRQs set
  LT.printReliableStatus();                            //print the reliable status
}


void actionReply()
{

  LT.startReadSXBuffer(startaddr);         //initialise SX buffer write at address startaddr
  RequestType = LT.readUint8();            //identify type of request
  Station = LT.readUint8();                //who is the request reply from
  Latitude = LT.readFloat();               //read latitude
  Longitude = LT.readFloat();              //read longitude
  Altitude = LT.readFloat();               //read altitude
  RXPayloadL = LT.endReadSXBuffer();       //close SX buffer read


  if (RXPayloadL != (RXPacketL - 4))
  {
    Serial.print(F("Packet wrong size: "));
    Serial.print(RXPacketL);
    Serial.print(F(" expected "));
    Serial.print(RXPayloadL + 4);
    return;
  }


  if (RequestType == RequestGPSLocation)
  {
    if ((RequestType == RequestGPSLocation) && (Station == RequestStation))
    {
      Serial.print(F("Valid Reply > "));        //print CRC of transmitted packet
      Serial.print(Station);
      Serial.print(F(","));
      Serial.print(Latitude, 6);
      Serial.print(F(","));
      Serial.print(Longitude, 6);
      Serial.print(F(","));
      Serial.print(Altitude, 1);
      Serial.println();
    }
    else
    {
      Serial.print(F("Not Valid Station "));
      Serial.print(Station);
      Serial.println();
    }
  }
  else
  {
    Serial.print(F("Not Valid Request Reply "));
    Serial.print(RequestType);
    Serial.println();
  }

}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("219_Reliable_Transmitter_Data_RequestorIRQ Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    while (1);
  }

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO);      //configure frequency and LoRa settings

  Serial.println(F("Transmitter ready"));
  Serial.println();
}
