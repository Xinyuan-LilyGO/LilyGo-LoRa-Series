/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 12/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program that simulates the transfer of a file using data transfer (DT)
  packet functions from the SX128X library. No SD cards are needed for the simulation. The file length
  used to simulate the transfer is defined by DTFileSize in the DTSettings.h file. Use with matching
  receiver program 232_Data_Transfer_Test_Receiver.ino

  DT packets can be used for transfering large amounts of data in a sequence of packets or segments,
  in a reliable and resiliant way. The file open requests to the remote receiver, each segement sent and
  the remote file close will all keep transmitting until a valid acknowledge comes from the receiver.
 
  On transmission the NetworkID and CRC of the payload are appended to the end of the packet by the library
  routines. The use of a NetworkID and CRC ensures that the receiver can validate the packet to a high degree
  of certainty.

  The transmitter sends the sequence of segments in order. If the sequence fails for some reason, the receiver
  will return a NACK packet to the transmitter requesting the segment sequence it was expecting.

  Details of the packet identifiers, header and data lengths and formats used are in the file;
  'Data transfer packet definitions.md' in the \SX128X_examples\DataTransfer\ folder.

  The transfer can be carried out using LoRa packets, max segment size (defined by DTSegmentSize) is 245 bytes
  for LoRa, 117 is maximum value for FLRC.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/
#define USELORA                              //enable this define to use LoRa packets
//#define USEFLRC                            //enable this define to use FLRC packets

#include <SPI.h>

#include <SX128XLT.h>
#include <ProgramLT_Definitions.h>
#include "DTSettings.h"                      //LoRa settings etc.
#include <arrayRW.h>

SX128XLT LoRa;                               //create an SX128XLT library instance called LoRa

#define PRINTSEGMENTNUM                      //enable this define to print segment numbers 

//#define DEBUG
//#define DISABLEPAYLOADCRC                  //enable this define if you want to disable payload CRC checking

#include "DTLibrarySIM.h"

char DTFileName[] = "/Simulate.JPG";         //file name to simulate sending


void loop()
{
  Serial.println(("Transfer started"));

  do
  {
    DTStartmS = millis();

    //opens the local file to send and sets up transfer parameters
    if (startFileTransfer(DTFileName, sizeof(DTFileName), DTSendAttempts))
    {
      Serial.print(DTFileName);
      Serial.println(F(" opened OK on remote"));
      printLocalFileDetails();
      Serial.println();
      NoAckCount = 0;
    }
    else
    {
      Serial.print(DTFileName);
      Serial.println(F("  Error opening remote file - restart transfer"));
      DTFileTransferComplete = false;
      continue;
    }

    delay(packetdelaymS);

    if (!sendSegments())
    {
      Serial.println();
      Serial.println(F("**********************************************************"));
      Serial.println(F("Error - Segment write with no file open - Restart received"));
      Serial.println(F("**********************************************************"));
      Serial.println();
      continue;
    }

    if (endFileTransfer(DTFileName, sizeof(DTFileName)))         //send command to close remote file
    {
      DTSendmS = millis() - DTStartmS;                  //record time taken for transfer
      Serial.print(DTFileName);
      Serial.println(F(" closed OK on remote"));
      beginarrayRW(DTheader, 4);
      DTDestinationFileLength = arrayReadUint32();
      Serial.print(F("Acknowledged remote destination file length "));
      Serial.println(DTDestinationFileLength);
      DTFileTransferComplete = true;
    }
    else
    {
      DTFileTransferComplete = false;
      Serial.println(F("ERROR send close remote destination file failed - program halted"));
    }
  }
  while (!DTFileTransferComplete);


  Serial.print(F("NoAckCount "));
  Serial.println( NoAckCount);
  Serial.println();

  DTsendSecs = (float) DTSendmS / 1000;
  Serial.print(F("Transmit time "));
  Serial.print(DTsendSecs, 3);
  Serial.println(F("secs"));
  Serial.print(F("Transmit rate "));
  Serial.print( (DTFileSize * 8) / (DTsendSecs), 0 );
  Serial.println(F("bps"));
  Serial.println(("Transfer finished"));

  Serial.println(("Program halted"));
  while (1);
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
  pinMode(LED1, OUTPUT);                          //setup pin as output for indicator LED
  led_Flash(2, 125);                              //two quick LED flashes to indicate program start
  setDTLED(LED1);                                 //setup LED pin for data transfer indicator

  Serial.begin(115200);
  Serial.println();
  Serial.println(F(__FILE__));
  Serial.flush();

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("LoRa device error"));
    while (1)
    {
      led_Flash(50, 50);                          //long fast speed flash indicates device error
    }
  }

#ifdef USELORA
  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);
  Serial.println(F("Using LoRa packets"));
#endif

#ifdef USEFLRC
  LoRa.setupFLRC(Frequency, Offset, BandwidthBitRate, CodingRate, BT, Syncword);
  Serial.println(F("Using FLRC packets"));
#endif

  LoRa.printOperatingSettings();
  Serial.println();
  LoRa.printModemSettings();
  Serial.println();

#ifdef DISABLEPAYLOADCRC
  LoRa.setReliableConfig(NoReliableCRC);
#endif

  if (LoRa.getReliableConfig(NoReliableCRC))
  {
    Serial.println(F("Payload CRC disabled"));
  }
  else
  {
    Serial.println(F("Payload CRC enabled"));
  }

  DTFileTransferComplete = false;

  Serial.println(F("File transfer ready"));
  Serial.println();
}
