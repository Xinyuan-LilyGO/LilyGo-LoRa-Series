/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program to receive images sent via LoRa using program
  238_StuartCAM_LoRa_Remote_Camera which uses an OV2640 Arducam camera to take pictures. 
  
  The received images\files are saved onto an SD card. Arduino DUEs were used for this test. 

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>

#include <SX126XLT.h>
#include <ProgramLT_Definitions.h>
#include "Settings.h"                       //LoRa settings etc.

SX126XLT LoRa;                              //create an SX126XLT library instance called LoRa, required by SDtransfer.h

//#define SDLIB                             //define SDLIB for SD.h or SDFATLIB for SDfat.h
#define SDFATLIB

#define ENABLEMONITOR                       //enable monitor prints
#define PRINTSEGMENTNUM
#define ENABLEFILECRC                       //enable this define to uses and show file CRCs
//#define DISABLEPAYLOADCRC                 //enable this define if you want to disable payload CRC checking
//#define DEBUG                             //see additional debug info

#include <DTSDlibrary.h>                    //library of SD functions
#include <SDtransfer.h>                     //library of data transfer functions


void loop()
{
  SDreceiveaPacketDT();
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
  pinMode(LED1, OUTPUT);                       //setup pin as output for indicator LED
  led_Flash(2, 125);                           //two quick LED flashes to indicate program start
  SDsetLED(LED1);                              //setup LED pin for data transfer indicator

#ifdef ENABLEMONITOR
  Monitorport.begin(115200);
  Monitorport.println();
  Monitorport.println(F(__FILE__));
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

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

#ifdef ENABLEMONITOR
  Monitorport.println();
  Monitorport.print(F("Initializing SD card..."));
#endif

  if (DTSD_initSD(SDCS))
  {
    Monitorport.println(F("SD Card initialized."));
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("SD Card failed, or not present."));
#endif
    while (1) led_Flash(100, 50);
  }

#ifdef ENABLEMONITOR
  Monitorport.println();
#endif

#ifdef DISABLEPAYLOADCRC
  LoRa.setReliableConfig(NoReliableCRC);
#endif

  if (LoRa.getReliableConfig(NoReliableCRC))
  {
    Monitorport.println(F("Payload CRC disabled"));
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("Payload CRC enabled"));
#endif
  }

  SDDTSegmentNext = 0;
  SDDTFileOpened = false;

#ifdef ENABLEMONITOR
  Monitorport.println(F("SDfile transfer receiver ready"));
  Monitorport.println();
#endif
}
