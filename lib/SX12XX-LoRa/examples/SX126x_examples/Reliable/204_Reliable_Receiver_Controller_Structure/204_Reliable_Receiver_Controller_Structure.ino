/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a basic demonstration of the receive of a 'Reliable' packet used to control
  outputs on the receiver.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. This receiver needs to have the same
  NetworkID as configured for the transmitter. The receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended is a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended to the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  This example program and the matching transmitter, 203_Reliable_Transmitter_Controller_Structure,
  receives a reliable packet from the transmitter, reads the variables from the sent data structure which
  when read remotly flashes the LED attached to the receiver on and off. Note that for the LED switching
  to be actioned the destinationNode and outputNumber received in the structure have to match.

  When using these transmit and receive examples the Arduino types should be the same as the structure
  formats can be different on some Arduinos.

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
#define LED1 8                                  //this is output 1

#define RXtimeout 60000                         //receive timeout in mS.  

uint8_t RXPacketL;                              //stores length of packet received
uint8_t PacketOK;                               //set to > 0 if packetOK
int16_t PacketRSSI;                             //stores RSSI of received packet
uint16_t LocalPayloadCRC;                       //locally calculated CRC of payload
uint16_t RXPayloadCRC;                          //CRC of payload received in packet
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint16_t thisNode = 2;                    //node number for this node

struct controllerStructure
{
  uint16_t destinationNode;
  uint8_t outputNumber;
  uint8_t onoroff;
} __attribute__((packed, aligned(1)));          //remove structure padding so there is compatibility between 8bit and 32bit Arduinos

struct controllerStructure controller1;         //define an instance called controller1 of the structure controllerStructure


void loop()
{
  PacketOK = LT.receiveReliable((uint8_t *) &controller1, sizeof(controller1), NetworkID, RXtimeout, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  RXPacketL = LT.readRXPacketL();               //get the received packet length
  PacketRSSI = LT.readPacketRSSI();             //read the received packets RSSI value

  if (PacketOK > 0)
  {
    //if the LT.receiveReliable() returns a value > 0 for PacketOK then packet was received OK
    //then only action payload if destinationNode = thisNode

    Serial.print(F("Packet received OK > "));
    LT.printHEXPacket((uint8_t *) &controller1, sizeof(controller1));
    Serial.println();

    Serial.print(F("destinationNode "));
    Serial.println(controller1.destinationNode);

    if (controller1.destinationNode == thisNode)
    {
      Serial.print(F("outputNumber "));
      Serial.println(controller1.outputNumber);
      Serial.print(F("onoroff "));
      Serial.println(controller1.onoroff);
      packet_is_OK();
      actionpayload();
    }
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

  IRQStatus = LT.readIrqStatus();                  //read the LoRa device IRQ status register
  Serial.print(F("Error "));

  if (IRQStatus & IRQ_RX_TIMEOUT)                  //check for an RX timeout
  {
    Serial.print(F(" RXTimeout "));
  }
  else
  {
    printPacketDetails();
  }
}


void actionpayload()
{
  Serial.print(F("Action payload > "));
  if (controller1.outputNumber == 1)
  {
    Serial.print(F("outputNumber "));
    Serial.print(controller1.outputNumber);
    if (controller1.onoroff)
    {
      Serial.println(F(" High"));
      digitalWrite(LED1, HIGH);
    }
    else
    {
      Serial.println(F(" Low"));
      digitalWrite(LED1, LOW);
    }
  }
  else
  {
    Serial.println(F("Not valid outputNumber"));
  }
}


void printPacketDetails()
{
  LocalPayloadCRC = LT.CRCCCITT((uint8_t *) &controller1, sizeof(controller1), 0xFFFF);  //calculate payload crc from the received RXBUFFER
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


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("204_Reliable_Receiver_Controller_Structure Starting"));
  pinMode(LED1, OUTPUT);                            //set high for on
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

  Serial.println(F("Receiver ready"));
  Serial.println();
}
