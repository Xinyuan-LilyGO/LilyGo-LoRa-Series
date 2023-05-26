/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 05/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple demonstration of the receipt of a 'Reliable' packet.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. This receiver needs to have the same
  NetworkID as configured for the transmitter. The receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended are a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended to the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  This example program and the matching transmitter, 207_Reliable_SXTransmitter_Controller reads a
  series of variables direct from the packet received in the LoRa devices internal buffer, it is not necessary
  to read variables to a memory array first. The receiver then uses these variables as the control information
  to flash a LED attached to the receiver on and off.

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
#define LED1 8                                  //this is output 1

#define RXtimeout 60000                         //receive timeout in mS.  

uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t PacketOK;                               //set to > 0 if packetOK
int16_t PacketRSSI;                             //stores RSSI of received packet
uint16_t LocalPayloadCRC;                       //locally calculated CRC of payload
uint16_t RXPayloadCRC;                          //CRC of payload received in packet
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter

uint16_t destinationNode;                       //node number we are controlling, 0 to 65535
const uint16_t thisNode = 2;                    //node number for this node
uint8_t outputNumber = 1;                       //output number on node we are controlling
uint8_t onoroff = 1;                            //set to 0 to set remote output off, 1 to set it on
uint8_t startaddr = 0;                          //address in SX buffer to start packet

void loop()
{
  PacketOK = LT.receiveSXReliable(startaddr, NetworkID, RXtimeout, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  RXPacketL = LT.readRXPacketL();                //get the received packet length, get this before reading packet
  RXPayloadL = RXPacketL - 4;
  PacketRSSI = LT.readPacketRSSI();              //read the received packets RSSI value

  LT.startReadSXBuffer(startaddr);               //start buffer read at location 0
  destinationNode = LT.readUint16();             //load the destination node
  outputNumber = LT.readUint8();                 //load the output number
  onoroff = LT.readUint8();                      //0 for off, 1 for on
  RXPayloadL = LT.endReadSXBuffer();             //this function returns the length of the array read

  if (PacketOK > 0)
  {
    //if the LT.receiveReliable() returns a value > 0 for PacketOK then packet was received OK
    //then only action payload if destinationNode = thisNode

    Serial.print(F("Payload received OK > "));
    LT.printSXBufferHEX(startaddr, startaddr + RXPayloadL - 1);
    Serial.println();

    Serial.print(F("destinationNode "));
    Serial.println(destinationNode);

    if (destinationNode == thisNode)
    {
      Serial.print(F("outputNumber "));
      Serial.println(outputNumber);
      Serial.print(F("onoroff "));
      Serial.println(onoroff);
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


void actionpayload()
{
  Serial.print(F("Action payload > "));
  if (outputNumber == 1)
  {
    Serial.print(F("outputNumber "));
    Serial.print(outputNumber);
    if (onoroff)
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
  LocalPayloadCRC = LT.CRCCCITTSX(startaddr, startaddr + RXPayloadL - 1, 0xFFFF);  //calculate payload crc from the received payload in LoRa device buffer
  TransmitterNetworkID = LT.getRXNetworkID(RXPacketL);
  RXPayloadCRC = LT.getRXPayloadCRC(RXPacketL);                //used the saved packet value to retrieve the payload CRC

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
  Serial.println(F("208_Reliable_SXReceiver_Controller Starting"));
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

  LT.setupLoRa(2445000000, 0, LORA_SF7, LORA_BW_0400, LORA_CR_4_5);

  Serial.println(F("Receiver ready"));
  Serial.println();

  //enable the following line if you want to disable payload CRC checking
  //LT.setReliableConfig(NoReliableCRC);                 //disable payload CRC check
}
