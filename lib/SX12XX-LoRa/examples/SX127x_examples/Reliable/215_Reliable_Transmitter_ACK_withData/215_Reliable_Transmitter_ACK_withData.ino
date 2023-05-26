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

  With this example and the matching receiver program, 216_Reliable_Receiver_ACK_withData, the
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
#define DIO0 3                                  //DIO0 pin on LoRa device, used for sensing RX and TX done 
#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm

#define ACKtimeout 1000                         //Acknowledge timeout in mS, set to 0 if ACK not used.                      
#define TXtimeout 1000                          //transmit timeout in mS. If 0 return from transmit function after send.  

uint8_t TXPacketL;                              //length of transmitted packet
uint8_t RXPacketL;                              //length of received acknowledge
uint16_t PayloadCRC;

const uint8_t RXBUFFER_SIZE = 16;               //RX buffer size, set to max payload length of 251, or maximum expected length
uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into
uint8_t buff[] = "Hello World";                 //the payload to send

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in receiver


void loop()
{

  do
  {
    Serial.print(F("Transmit Payload > "));
    LT.printASCIIArray(buff, sizeof(buff));                      //print the payload buffer as ASCII
    Serial.println();
    Serial.flush();

    PayloadCRC = LT.CRCCCITT(buff, sizeof(buff), 0xFFFF);

    //now transmit the packet
    TXPacketL =  LT.transmitReliable(buff, sizeof(buff), NetworkID, TXtimeout, TXpower, WAIT_TX);  //will return packet length > 0 if sent OK, otherwise 0 if transmit error

    RXPacketL = LT.waitReliableACK(RXBUFFER, sizeof(RXBUFFER), NetworkID, PayloadCRC, ACKtimeout);

    if (RXPacketL > 0)
    {
      //if waitReliableACK() returns > 0 then ack was received
      packet_is_OK();
      Serial.println();
      Serial.print(F("Ack Received > "));
      LT.printASCIIPacket(RXBUFFER, RXPacketL - 5);
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


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("215_Reliable_Transmitter_ACK_withData Starting"));

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

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO); //configure frequency and LoRa settings

  Serial.println(F("Transmitter ready"));
  Serial.println();
}
