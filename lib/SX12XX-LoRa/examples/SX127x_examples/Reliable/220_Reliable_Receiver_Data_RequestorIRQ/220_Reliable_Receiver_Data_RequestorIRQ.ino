/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 17/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a demonstration of the transmission and acknowledgement of 'Reliable'
  packets to request information or operations from a remote receiver.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. The receiver needs to have the same
  NetworkID as configured for the transmitter since the receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended are a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended in the packet. The receiver is thus able to check if the payload is valid.

  The transmitter sends a request for the receiver which will respond with an acknowledge whach can include
  any data requested. The transmitted request is a total of 6 or more bytes, with one byte for the payload
  type and another byte for station the request is directed to.

  The request is for the receiver to return a set of GPS co-ordinates. This information is included in the
  acknowledge the receiver sends if there is a match for request type and station number. The orginal network
  ID and payload CRC are also returned with the acknowledge so the transmitter can verify if the packet it
  receives in reply is geniune.

  No DIO0 pin needs to be connected to the LoRa device for this program.

  The matching transmitter program is 219_Reliable_Transmitter_Data_RequestorIRQ.
  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/


#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library
#include <SX127XLT.h>                           //include the appropriate library   
#include "Settings.h"

SX127XLT LT;                                    //create a library class instance called LT

uint8_t RXBUFFER[251];                          //create the buffer that received packets are copied into
uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t TXPayloadL;                             //stores length of payload sent
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet
uint16_t StationNumber;
uint8_t RequestType;

//#define DEBUG


void loop()
{
  uint8_t requesttype;

  requesttype = waitRequest(ThisStation, 5000);                        //listen for 5000mS, will return 0 if there is no request

  switch (requesttype)
  {
    case 0:
      break;

    case RequestGPSLocation:
      actionRequestGPSLocation();
      break;

    default:
      Serial.println(F("Request not recognised"));
      break;
  }
  Serial.println();
}


uint8_t waitRequest(uint8_t station, uint32_t timeout)
{
  //wait for an incoming request, returns 0 if no request in timeout period

  Serial.println(F("Wait request"));

  RXPacketL = LT.receiveSXReliableIRQ(0, NetworkID, timeout, WAIT_RX);

  if (RXPacketL)
  {
#ifdef DEBUG
    Serial.print(F("Reliable packet received > "));
    packet_is_OK();
#endif
    RequestType = LT.getByteSXBuffer(0);
    StationNumber = LT.getByteSXBuffer(1);

    Serial.print(F("Received "));
    printRequestType(RequestType);
    Serial.print(F(" for station "));
    Serial.println(StationNumber);

    if (StationNumber == station)
    {
      return RequestType;
    }
    else
    {
      Serial.println(F("ERROR Request not for this station"));
    }
  }
  else
  {
    if (LT.readIrqStatus() & IRQ_RX_TIMEOUT)
    {
      Serial.println(F("ERROR Timeout waiting for valid request"));
    }
    else
    {
      packet_is_Error();
    }
  }
  return 0;
}


bool actionRequestGPSLocation()
{
  uint16_t RXPayloadCRC;

  RXPayloadCRC = LT.getRXPayloadCRC(RXPacketL);       //fetch received payload crc to return with ack

  LT.startWriteSXBuffer(0);                   //initialise SX buffer write at address 0
  LT.writeUint8(RequestGPSLocation);          //identify type of request
  LT.writeUint8(ThisStation);                 //who is the request reply from
  LT.writeFloat(TestLatitude);                //add latitude
  LT.writeFloat(TestLongitude);               //add longitude
  LT.writeFloat(TestAltitude);                //add altitude
  LT.writeUint8(TrackerStatus);               //add status byte
  TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

  delay(ACKdelay);

  digitalWrite(LED1, HIGH);
  LT.sendSXReliableACKIRQ(0, TXPayloadL, NetworkID, RXPayloadCRC, TXpower);
  Serial.print(F("RequestGPSLocation Replied > "));
  Serial.print(ThisStation);
  Serial.print(F(","));
  Serial.print(TestLatitude, 6);
  Serial.print(F(","));
  Serial.print(TestLongitude, 6);
  Serial.print(F(","));
  Serial.print(TestAltitude, 1);
  Serial.print(F(","));
  Serial.print(TrackerStatus);
  Serial.println();
  Serial.flush();
  digitalWrite(LED1, LOW);
  return true;
}


void printRequestType(uint8_t type)
{
  switch (type)
  {
    case RequestGPSLocation:
      Serial.print(F(" RequestGPSLocation"));
      break;

    default:
      Serial.print(F(" Request type not recognised"));
      break;
  }
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
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmitterNetworkID,0x"));
  Serial.print(LT.getRXNetworkID(RXPacketL), HEX);
  Serial.print(F(",RXPayloadCRC,0x"));
  Serial.print(LT.getRXPayloadCRC(RXPacketL), HEX);
  LT.printReliableStatus();
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;
  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println(__FILE__);
  Serial.println();

  SPI.begin();

  if (LT.begin(NSS, NRESET, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
  }
  else
  {
    Serial.println(F("ERROR No LoRa device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);   //need to be in LoRa mode to receive requests

  Serial.println(F("Receiver ready"));
  Serial.println();
}
