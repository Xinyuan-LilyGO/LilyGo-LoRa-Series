/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a basic demonstration of the transmission of a 'Reliable' packet used to
  control outputs on a remote receiver.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. This receiver needs to have the same
  NetworkID as configured for the transmitter. The receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended is a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended to the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  This example program and the matching receiver, 206_Reliable_Receiver_Controller_ArrayRW, transmits a
  array that contains variables which when read by the remote receiver flash an LED attached to
  the receiver on and off. The variables are writen direct to the array using a library file arrayRW.h.

  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX126XLT.h>                           //include the appropriate library  
#include <arrayRW.h>                            //routines for reading and writing varaibles to array

SX126XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //LED used to indicate transmission
#define LORA_DEVICE DEVICE_SX1262               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm

#define TXtimeout 1000                          //transmit timeout in mS. 

uint8_t controlarray[4];                        //the array payload to send
uint8_t controlarraysize;
uint16_t PayloadCRC;
uint8_t TXPacketL;

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in receiver

uint16_t destinationNode = 2;                   //node number we are controlling, 0 to 65535
uint8_t outputNumber = 1;                       //output number on node we are controlling
uint8_t onoroff = 0;                            //set to 0 to set remote output off, 1 to set it on


void loop()
{

  beginarrayRW(controlarray, 0);                //start writing to controlarray array at location 0
  arrayWriteUint16(destinationNode);
  arrayWriteUint8(outputNumber);
  arrayWriteUint8(onoroff);                     //0 for off, 1 for on
  controlarraysize = endarrayRW() + 1;          //this function returns the length of the array to send, i.e. the packet payload

  Serial.print(F("Transmit Array > "));
  LT.printHEXPacket(controlarray, controlarraysize);    //print the sent array as HEX
  Serial.println();
  Serial.flush();

  //now transmit the packet
  digitalWrite(LED1, HIGH);                     //LED on to indicate transmit
  TXPacketL = LT.transmitReliable(controlarray, controlarraysize, NetworkID, TXtimeout, TXpower, WAIT_TX);

  if (TXPacketL)
  {
    //if transmitReliable() returns > 0 then transmit was OK
    PayloadCRC = LT.getTXPayloadCRC(TXPacketL);          //read the actual transmitted CRC from the LoRa device
    packet_is_OK();
    Serial.println();
  }
  else
  {
    //if transmitReliable() returns 0 there was an error
    packet_is_Error();
    Serial.println();
  }

  digitalWrite(LED1, LOW);
  Serial.println();
  delay(5000);                                 //have a delay between packets

  if (onoroff == 0)                            //toggle the on/off status
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
  Serial.println(F("205_Reliable_Transmitter_Controller_ArrayRW Starting"));

  pinMode(LED1, OUTPUT);

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

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO);

  Serial.println(F("Transmitter ready"));
  Serial.println();
}
