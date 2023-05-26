/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 10/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a basic demonstration of the transmission and acknowledgement of a 'Reliable'
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

  If the received packet is valid then the networkID and payload CRC are returned in a 4 byte packet as an
  acknowledgement that the transmitter listens for. If the transmitter does not receive the acknowledgement
  within the ACKtimeout period, the original packet is re-transmitted until a valid acknowledgement is
  received. This program should be used with the matching receiver program, 212_Reliable_SXReceiver_AutoACK.

  This example writes the payload direct to the LoRa SX devices internal packet buffer, no intermediate array
  or memory is needed. The payload is a simpel remote control packet directed to a specific receiver,

  The packet is destined for a specific receiver (destinationNode). A particular output (outputNumber) is turned
  on and off.

  Serial monitor baud rate should be set at 115200.

  *******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX126XLT.h>                           //include the appropriate library  

SX126XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LORA_DEVICE DEVICE_SX1262               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm

#define ACKtimeout 1000                         //Acknowledge timeout in mS                      
#define TXtimeout 1000                          //transmit timeout in mS. If 0 return from transmit function after send.  

uint16_t PayloadCRC;                            //CRC of the payload
uint8_t TXPacketL;                              //length of transmitted packet
uint8_t TXPayloadL;                             //this is the payload length sent

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in receiver

uint16_t destinationNode = 2;                   //node number we are controlling, 0 to 65535
uint8_t outputNumber = 1;                       //output number on node we are controlling
uint8_t onoroff = 0;                            //set to 0 to set remote output off, 1 to set it on
uint8_t startaddr = 0;                          //address in SX buffer to start packet

void loop()
{

  //keep transmitting the packet until an ACK is received
  do
  {
    LT.startWriteSXBuffer(startaddr);             //start the write at SX12XX internal buffer location 0
    LT.writeUint16(destinationNode);              //destination node for packet
    LT.writeUint8(outputNumber);                  //output number on receiver
    LT.writeUint8(onoroff);                       //0 for off, 1 for on
    TXPayloadL = LT.endWriteSXBuffer();           //closes packet write and returns the length of the payload to send

    Serial.print(F("Transmit SX buffer > "));
    LT.printSXBufferHEX(startaddr, startaddr + TXPayloadL - 1);       //print the sent SX array as HEX
    Serial.println();
    Serial.flush();

    TXPacketL = LT.transmitSXReliableAutoACK(startaddr, TXPayloadL, NetworkID, ACKtimeout, TXtimeout, TXpower, WAIT_TX);

    if (TXPacketL > 0)
    {
      //if transmitSXReliableAutoACK() returns > 0 then transmit and ack was OK
      PayloadCRC = LT.getTXPayloadCRC(TXPacketL);                    //read the actual transmitted CRC from the LoRa device buffer
      packet_is_OK();
      Serial.println();
    }
    else
    {
      //if transmitReliable() returns 0 there was an error
      packet_is_Error();
      Serial.println();
    }
    delay(500);                                        //small delay between tranmission attampts
  }
  while (TXPacketL == 0);

  Serial.println();
  delay(1000);                                         //have a delay between packets

  if (onoroff == 0)                                    //toggle the on/off status
  {
    onoroff = 1;
  }
  else
  {
    onoroff = 0;
  }

}


void packet_is_OK()
{
  Serial.println(F("Packet acknowledged"));
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmittedPayloadCRC,0x"));        //print CRC of transmitted packet
  Serial.print(PayloadCRC, HEX);
}


void packet_is_Error()
{
  Serial.print(F("No Packet acknowledge"));
  LT.printIrqStatus();                                 //prints the text of which IRQs set
  LT.printReliableStatus();                            //print the reliable status
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("211_Reliable_SXTransmitter_AutoACK Starting"));

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

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO); //configure frequency and LoRa settings

  Serial.println(F("Transmitter ready"));
  Serial.println();
}
