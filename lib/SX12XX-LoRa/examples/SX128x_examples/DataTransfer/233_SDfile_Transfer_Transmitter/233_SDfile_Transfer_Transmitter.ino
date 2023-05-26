/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 12/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program that transfers a file using data transfer (DT) packet functions
  from the SX128X library to send a file from the SD card on one Arduino to the SD card on another Arduino.
  Arduino DUEs were used for the test and this example transfers an JPG image.

  DT packets can be used for transfering large amounts of data in a sequence of packets or segments,
  in a reliable and resiliant way. The file open requests to the remote receiver, each segement sent and
  the remote file close will all keep transmitting until a valid acknowledge comes from the receiver.
  Use this transmitter with the matching receiver program, 234_SDfile_Transfer_Receiver.ino.

  On transmission the NetworkID and CRC of the payload are appended to the end of the packet by the library
  routines. The use of a NetworkID and CRC ensures that the receiver can validate the packet to a high degree
  of certainty.

  The transmitter sends the sequence of segments in order. If the sequence fails for some reason, the receiver
  will return a NACK packet to the transmitter requesting the segment sequence it was expecting.

  Details of the packet identifiers, header and data lengths and formats used are in the file;
  'Data transfer packet definitions.md' in the \SX128X_examples\DataTransfer\ folder.

  The transfer can be carried out using LoRa packets, max segment size (defined by DTSegmentSize) is 245 bytes
  for LoRa.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/
#define USELORA                              //enable this define to use LoRa packets
//#define USEFLRC                            //enable this define to use FLRC packets

#include <SPI.h>

#include <SX128XLT.h>
#include <ProgramLT_Definitions.h>
#include "DTSettings.h"                      //LoRa settings etc.

SX128XLT LoRa;                               //create an SX128XLT library instance called LoRa, required by SDtransfer.h

#define ENABLEMONITOR                        //enable monitor prints
#define PRINTSEGMENTNUM                      //enable this define to print segment numbers 
#define ENABLEFILECRC                        //enable this define to uses and show file CRCs
//#define DISABLEPAYLOADCRC                  //enable this define if you want to disable payload CRC checking
//#define DEBUG                              //see additional debug info 

//#define SDLIB                              //define SDLIB for SD.h or SDFATLIB for SDfat.h
#define SDFATLIB

#include <DTSDlibrary.h>                     //library of SD functions
#include <SDtransfer.h>                      //library of data transfer functions

//choice of files to send
//char FileName[] = "/$50SATL.JPG";          //file length 63091 bytes, file CRC 0x59CE
char FileName[] = "/$50SATS.JPG";        //file length 6880 bytes, file CRC 0x0281
//char FileName[] = "/$50SATT.JPG";        //file length 1068 bytes, file CRC 0x6A02


void loop()
{
  uint32_t filelength;

#ifdef ENABLEMONITOR
  Monitorport.println(F("Transfer started"));
#endif

  filelength = SDsendFile(FileName, sizeof(FileName));

  if (filelength)
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("Transfer finished"));
#endif
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("Transfer failed"));
    Monitorport.println();
#endif
  }

  delay(15000);

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
  SDsetLED(LED1);                                 //setup LED pin for data transfer indicator

#ifdef ENABLEMONITOR
  Monitorport.begin(115200);
  Monitorport.println();
  Monitorport.println(F(__FILE__));
  Monitorport.flush();
#endif

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("LoRa device error"));
#endif
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


#ifdef ENABLEMONITOR
  Monitorport.println();
  Monitorport.print(F("Initializing SD card..."));
#endif

  if (DTSD_initSD(SDCS))
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("SD Card initialized."));
#endif
  }
  else
  {
    Monitorport.println(F("SD Card failed, or not present."));
    while (1) led_Flash(100, 25);
  }

#ifdef ENABLEMONITOR
  Monitorport.println();
#endif

#ifdef DISABLEPAYLOADCRC
  LoRa.setReliableConfig(NoReliableCRC);
#endif

  if (LoRa.getReliableConfig(NoReliableCRC))
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("Payload CRC disabled"));
#endif
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("Payload CRC enabled"));
#endif
  }

  SDDTFileTransferComplete = false;

#ifdef ENABLEMONITOR
  Monitorport.println(F("SDfile transfer ready"));
  Monitorport.println();
#endif
}
