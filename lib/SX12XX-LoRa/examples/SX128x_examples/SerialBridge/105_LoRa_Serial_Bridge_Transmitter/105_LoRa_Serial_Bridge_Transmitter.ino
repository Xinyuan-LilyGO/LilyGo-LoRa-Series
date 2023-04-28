/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 28/12/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This the transmitter part of a Serial bridge. This tranmitter receives data on
  the defined serial port and puts that serial data into a LoRa packet which is then transmitted. A
  matching reciever picks up the packet and displays it on the remote Arduinos serial port.

  The purpose of the bridge is to allow the serial output of a device, anothor Arduino or sensor for
  instance, to be remotely monitored, without the need for a long serial cable.

  Serial monitor baud rate should be set at 9600.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX128XLT.h>                           //include the appropriate library  

SX128XLT LoRa;                                  //create a library class instance called LoRa

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done
#define LED1 8                                  //indicator LED
#define LORA_DEVICE DEVICE_SX1280               //we need to define the LoRa device we are using
#define TXpower 10                              //LoRa transmit power in dBm

uint8_t TXPacketL;                              //length of transmitted packet
uint8_t RXPacketL;                              //length of received acknowledge
uint16_t PayloadCRC;
uint16_t MessageID;                             //MessageID identifies each message, 2 bytes at front of packet
uint8_t receivedBytes;                          //count of serial bytes received
uint8_t Attempts;                               //number of times to send packet waiting for acknowledge
uint32_t startuS;                               //used for timeout for serial input
const uint8_t MaxMessageSize = 251;             //max size of array to send with received serial
uint8_t Message[MaxMessageSize];                //array for received serial data

const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const uint8_t Bandwidth = LORA_BW_1600;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint16_t NetworkID = 0x3210;              //a unique identifier to go out with packet = 0x3210;

uint32_t SerialTimeoutuS;                       //Timeout in uS before assuming message to send is complete
const uint32_t TimeoutCharacters = 10;          //number of characters at specified baud rate that are a serial timeout
const uint8_t MaxAttempts = 4;                  //Maximum attempts to send message and receive acknowledge
const uint32_t ACKtimeoutmS = 50;               //Acknowledge timeout in mS
const uint32_t TXtimeoutmS = 1000;              //transmit timeout in mS.

#define SerialInput Serial                      //assign serial port for reading data
#define DebugSerial Serial                      //assign serial port for debug output 
#define SerialInputBaud 9600                    //baud rate for the serial 

//#define DEBUG                                 //enable define to see debug information
#define SENDREADY                               //enable define to see Ready control message at start


void loop()
{
#ifdef DEBUG
  DebugSerial.println(F("> "));
#endif

  Message[0] = lowByte(MessageID);
  Message[1] = highByte(MessageID);
  MessageID++;                                  //ready for next message

  receivedBytes = 2;

  while (SerialInput.available() == 0);         //wait for serial data to be available

  digitalWrite(LED1, HIGH);
  startuS = micros();

  while (((uint32_t) (micros() - startuS) < SerialTimeoutuS))
  {
    if (SerialInput.available() > 0)
    {
      if (receivedBytes >= MaxMessageSize)
      {
        break;
      }
      Message[receivedBytes] = SerialInput.read();
      startuS = micros();                        //restart timeout
      receivedBytes++;
    }
  };

  //the only exits from the serial collection loop above are if there is a timeout, or the maximum
  //message size is reached.

  Attempts = 0;

  do
  {
#ifdef DEBUG
    uint8_t index;
    DebugSerial.print(receivedBytes);
    DebugSerial.print(F(" "));
    DebugSerial.print(MessageID);
    DebugSerial.print(F(" > "));                                   //flag on monitor for transmitting

    for (index = 2; index < receivedBytes; index++)
    {
      DebugSerial.write(Message[index]);
    }
    DebugSerial.println();
#endif

    TXPacketL = LoRa.transmitReliable(Message, receivedBytes, NetworkID, TXtimeoutmS, TXpower, WAIT_TX);
    PayloadCRC = LoRa.readUint16SXBuffer(TXPacketL - 2);           //need the payload CRC to check for valid ACK
    RXPacketL = LoRa.waitReliableACK(NetworkID, PayloadCRC, ACKtimeoutmS);
    Attempts++;
  }
  while ((RXPacketL == 0) && (Attempts <= MaxAttempts));

#ifdef DEBUG
  if (RXPacketL == 0)
  {
    Serial.println(F("NA>"));
  }
#endif
}


void setup()
{
  pinMode(LED1, OUTPUT);

  DebugSerial.begin(9600);
  SerialInput.begin(SerialInputBaud);

#ifdef DEBUG
  DebugSerial.println();
  Serial.println(F(__FILE__));
  DebugSerial.println();
#endif

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
#ifdef DEBUG
    DebugSerial.println(F("LoRa Device found"));
#endif
  }
  else
  {
    DebugSerial.println(F("No LoRa device responding"));
    while (1);
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

#ifdef SENDREADY
  //Send a ready packet so remorte can check its working, send control message 0x0001
  Message[0] = 1;
  Message[1] = 0;
  Message[2] = 'R';
  TXPacketL = LoRa.transmitReliable(Message, 3, NetworkID, TXtimeoutmS, TXpower, WAIT_TX);
  delay(1000);
#endif

  //now calculate timeout in microseconds based on baud rate and number of characters, assuming a 11bit byte

  SerialTimeoutuS = ((1000000 / SerialInputBaud) * 11) * TimeoutCharacters;

#ifdef DEBUG
  DebugSerial.print(F("SerialTimeoutuS "));
  DebugSerial.println(SerialTimeoutuS);
  DebugSerial.println(F("Clear serial buffer"));
#endif

  while (SerialInput.available() > 0)           //clear serial input
  {
    SerialInput.read();
  }

#ifdef DEBUG
  DebugSerial.println(F("Waiting start of serial input"));
#endif

  MessageID = 257;                              //first message to send is 0x0101
}
