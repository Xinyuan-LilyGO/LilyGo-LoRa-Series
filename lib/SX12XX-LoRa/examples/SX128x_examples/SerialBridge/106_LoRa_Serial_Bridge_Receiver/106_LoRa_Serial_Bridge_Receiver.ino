/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 28/12/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This a the receiver part of a Serial bridge. The tranmitter receives data on
  the defined serial port and puts that serial data into a LoRa packet which is then transmitted. This
  matching reciever picks up the packet and displays it on the receivers serial port.

  The purpose of the bridge is to allow the serial output of a device, anothor Arduino or sensor for
  instance, to be remotely monitored, without the need for a long serial cable.

  Serial monitor baud rate should be set at 9600.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library
#include <SX128XLT.h>                           //include the appropriate library   

SX128XLT LoRa;                                  //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done  
#define LED1 8                                  //this is output 1
#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using
#define TXpower 10                              //dBm power to use for ACK  

uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint16_t RXPayloadCRC;                          //CRC of payload included in received packet
uint16_t MessageID;                             //MessageID identifies each message, 2 bytes at front of packet
uint16_t LastMessageID;                         //keep track of last received message, display messages start at 0x0100

const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const uint8_t Bandwidth = LORA_BW_1600;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint16_t NetworkID =  0x3210;             //Message NetworkID identifies each message, automatically appended to packet

const uint32_t ACKdelay1 = 10;                  //delay in mS before sending acknowledge
const uint32_t ACKdelay2 = 20;                  //delay in mS before sending acknowledge for message already received
const uint32_t RXtimeout = 60000 ;              //receive timeout in mS. set to 0 for no timeout
const uint32_t TXtimeout = 1000;                //transmit timeout in mS.
const uint8_t MaxMessageSize = 251 ;            //maximum size of serial maeesage to send, 4 bytes needed for NetworkID and payload CRC
uint8_t Message[MaxMessageSize];                //array for receiving message

#define SerialOutput Serial                     //assign serial port for outputing data
#define DebugSerial Serial                      //assign serial port for outputing debug data 

//#define DEBUG                                 //enable this define to see debug information


void loop()
{

#ifdef DEBUG
  DebugSerial.print(F("> "));
#endif

  do
  {
    RXPacketL = LoRa.receiveReliable(Message, MaxMessageSize, NetworkID, RXtimeout, WAIT_RX);

    MessageID = Message[0] + (Message[1] * 256);    //message number is first two bytes of Message array

    if (RXPacketL == 0)                             //check for a valid packet
    {
#ifdef DEBUG
      DebugSerial.print(MessageID);
      DebugSerial.print(F("E>"));
      packet_is_Error();
#endif
      break;
    }


    //if here there is a valid reliable packet

    if (MessageID > LastMessageID)                  //new message should be greater than last message
    {
      processMessage();
      delay(ACKdelay1);
      sendACK();
      break;
    }

    if (MessageID <= 256)                            //is it a control message
    {
      processControlMessage();
      break;
    }

    if (LastMessageID == MessageID)                   //have we had this message before, remember TX may miss the sent ACK ?
    {
      delay(ACKdelay2);
      sendACK();                                      //dont process message, but send ACK again
      break;                                          //and go back to receive
    }

    if (MessageID == 257)                             //this appears to be a first message, transmitter may have restarted
    {
      LastMessageID = 257;                            //reset last message number
      processMessage();
      delay(ACKdelay1);
      sendACK();                                      //dont process message, but send ACK again
      break;                                          //and go back to receive
    }

    break;                                            //and go back to receive
  }
  while (1);
}


void processMessage()
{
  uint8_t index, endpayload;

  endpayload = RXPacketL - 4;

  for (index = 2; index < endpayload; index++)
  {
    SerialOutput.write(Message[index]);
  }

#ifdef DEBUG
  DebugSerial.print(RXPacketL - 6);              //print number of characters in message
  DebugSerial.print(F(" "));
  DebugSerial.print(MessageID);                  //print message ID
  DebugSerial.print(F(" > "));
  DebugSerial.println();                         //so each message is on new line
#endif

  LastMessageID = MessageID;
}


void processControlMessage()
{
  if (MessageID == 1)
  {
    DebugSerial.println(F("Ready"));
    sendACK();
  }
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LoRa.readIrqStatus();                  //read the LoRa device IRQ status register
  DebugSerial.print(F("RXPacketL,"));
  DebugSerial.print(RXPacketL);
  DebugSerial.print(F(",IRQStatus,0x"));
  DebugSerial.println(IRQStatus, HEX);
  if (IRQStatus & IRQ_RX_TIMEOUT)                           //check for an RX timeout
  {
    DebugSerial.println(F("RXTimeout"));
  }
}


void sendACK()
{
  RXPayloadCRC = LoRa.readUint16SXBuffer(RXPacketL - 2);    //get sent payload CRC, needed for ACK
  digitalWrite(LED1, HIGH);
  LoRa.writeUint16SXBuffer(0, MessageID);
  LoRa.writeUint16SXBuffer(2, NetworkID);
  LoRa.writeUint16SXBuffer(4, RXPayloadCRC);
  LoRa.transmitSXBuffer(0, 6, TXtimeout, TXpower, WAIT_TX);
#ifdef DEBUG
  DebugSerial.print(MessageID);
  DebugSerial.println(F(" A >"));
#endif
  digitalWrite(LED1, LOW);
}


void setup()
{
  SerialOutput.begin(9600);
  DebugSerial.begin(9600);

#ifdef DEBUG
  DebugSerial.println();
  DebugSerial.println(F(__FILE__));
  DebugSerial.println();
#endif

  pinMode(LED1, OUTPUT);                            //set high for on
  SPI.begin();

  if (LoRa.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
#ifdef DEBUG
    DebugSerial.println(F("LoRa Device found"));
    delay(1000);
#endif
  }
  else
  {
    DebugSerial.println(F("No LoRa device responding"));
    while (1);
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

#ifdef DEBUG
  DebugSerial.println(F("Receiver ready"));
#endif

  LastMessageID = 0x0100;                             //start value
}
