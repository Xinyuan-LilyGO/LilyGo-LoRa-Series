/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 18/04/22

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

  The matching receiver program is 220_Reliable_Receiver_Data_RequestorIRQ. 

  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX127XLT.h>                           //include the appropriate library  
#include "Settings.h"

SX127XLT LT;                                    //create a library class instance called LT

uint8_t TXPacketL;                              //length of transmitted packet
uint8_t TXPayloadL;
uint8_t RXPayloadL;
uint8_t RXPacketL;                              //length of received acknowledge
uint16_t PayloadCRC;

uint8_t RXBUFFER[16];                           //create the buffer that received packets are copied into
uint8_t RequestType;                            //type of request to send
uint8_t RequestStation;                         //station request is aimed at

float Latitude;                                 //variables for GPS location
float Longitude;
float Altitude;
uint8_t TrackerStatus;

//#define DEBUG


void loop()
{
  
  //*****************************************************************************************
  //Request GPS location
  //*****************************************************************************************

  RequestStation = 123;

  if (sendRequestGPSLocation(RequestStation, 5))
  {
    Serial.println(F("Valid ACK received"));
#ifdef DEBUG
    packet_is_OK();
#endif
    actionRequestGPSLocation();
  }
  else
  {
    Serial.println(F("No valid ACK received"));
  }

  //*****************************************************************************************
 
  Serial.println();
  delay(5000); 
}


uint8_t sendRequestGPSLocation(uint8_t station, uint8_t sendcount)
{
  uint8_t ackrequesttype;
  uint8_t ackstation;
  uint8_t startcount;

  startcount = sendcount;

  do
  {
    Serial.print(startcount - sendcount + 1);
    Serial.print(F(" Transmit request"));
    printRequestType(RequestGPSLocation);
    Serial.print(F(" to station "));
    Serial.println(station);
    Serial.flush();

    //build the request payload
    LT.startWriteSXBuffer(0);                   //initialise SX buffer write at address 0
    LT.writeUint8(RequestGPSLocation);          //identify type of request
    LT.writeUint8(station);                     //station to reply to request
    TXPayloadL = LT.endWriteSXBuffer();         //close SX buffer write

    //now transmit the request
    digitalWrite(LED1, HIGH);
    TXPacketL = LT.transmitSXReliableIRQ(0, TXPayloadL, NetworkID, TXtimeout, TXpower, WAIT_TX);
    digitalWrite(LED1, LOW);
    PayloadCRC = LT.getTXPayloadCRC(TXPacketL);
    RXPacketL = LT.waitSXReliableACKIRQ(0, NetworkID, PayloadCRC, ACKtimeout);

    if (RXPacketL > 0)
    {
      //if waitSXReliableACK() returns > 0 then valid ack was received
      ackrequesttype = LT.getByteSXBuffer(0);
      ackstation = LT.getByteSXBuffer(1);

      if ((ackrequesttype == RequestGPSLocation) && (ackstation == station))
      {
        return RXPacketL;
      }
      delay(200);                                        //small delay between tranmission attampts
    }
    sendcount--;
  }
  while ((RXPacketL == 0) && (sendcount > 0));

  Serial.println(F("ERROR send request failed"));
  return 0;
}


void printRequestType(uint8_t type)
{
  switch (type)
  {
    case 1:
      Serial.print(F(" RequestGPSLocation"));
      break;

    default:
      Serial.print(F(" not recognised"));
      break;
  }
}


void actionRequestGPSLocation()
{
  LT.startReadSXBuffer(0);                 //initialise SX buffer write at address 0
  RequestType = LT.readUint8();            //read type of request
  RequestStation = LT.readUint8();         //who is the request reply from
  Latitude = LT.readFloat();               //read latitude
  Longitude = LT.readFloat();              //read longitude
  Altitude = LT.readFloat();               //read altitude
  TrackerStatus = LT.readUint8();          //read status byte
  RXPayloadL = LT.endReadSXBuffer();       //close SX buffer read

  Serial.print(F("RequestGPSLocation reply > "));
  Serial.print(RequestStation);
  Serial.print(F(","));
  Serial.print(Latitude, 6);
  Serial.print(F(","));
  Serial.print(Longitude, 6);
  Serial.print(F(","));
  Serial.print(Altitude, 1);
  Serial.print(F(","));
  Serial.print(TrackerStatus);
  Serial.println();
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

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Transmitter ready"));
  Serial.println();
}
