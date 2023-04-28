/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program for receiving files transmitted by LoRa and saving them to SD card
  and then optionally transfering them to a PC using a second serial port and the Ymodem protocol. Arduino
  DUEs were used for testing the program, these have multiple hardware Serial ports.

  Progress messages on the transfer are sent to the IDE Serial monitor and printed on a connected ILI9341
  TFT display.

  For details of the PC upload process see here;

  https://stuartsprojects.github.io/2022/01/01/StuartCAM-ESP32CAM-Getting-the-Pictures-Onto-a-PC.html

  
  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/


#include <SPI.h>
#include <SX128XLT.h>
#include <ProgramLT_Definitions.h>

#define USELORA                            //enable this define to use LoRa packets
//#define USEFLRC                          //enable this define to use FLRC packets
#include "Settings.h"                      //LoRa settings etc.
SX128XLT LoRa;                             //create an SX128XLT library instance called LoRa, required by SDtransfer

//#define SDLIB                            //define SDLIB for SD.h or SDFATLIB for SDfat.h
#define SDFATLIB
#include "DTSDlibrary.h"

#include "Adafruit_GFX.h"                  //get library here > https://github.com/adafruit/Adafruit-GFX-Library  
#include "Adafruit_ILI9341.h"              //get library here > https://github.com/adafruit/Adafruit_ILI9341
Adafruit_ILI9341 disp = Adafruit_ILI9341(DISPCS, DISPDC, DISPRESET);  //for dispaly defaults, textsize and rotation, see Settings.h

#define ENABLEMONITOR                       //enable monitor prints
#define PRINTSEGMENTNUM
#define ENABLEFILECRC                       //enable this define to uses and show file CRCs
//#define DISABLEPAYLOADCRC                 //enable this define if you want to disable payload CRC checking
//#define DEBUG                             //see additional debug info
#define ENABLEPCTRANSFER                  //enable this define for YModem transfer to PC

#include "SDtransferDisplay.h"            //library of data transfer with display functions
#include "YModem.h"                       //YModem for the the save SD File

uint32_t bytestransfered;                 //bytes transfered via YModem


void loop()
{
  SDreceiveaPacketDT();

#ifdef ENABLEPCTRANSFER
  if (SDDTFileSaved)
  {
    Monitorport.println(F("File saved to SD - start YModem transfer to PC"));
    setCursor(0, 4);
    disp.print(F("Run YModem"));
    digitalWrite(LED1, HIGH);
    setCursor(0, 6);
    disp.print(F("      "));                 //clear segment number

    bytestransfered = yModemSend(SDDTfilenamebuff, 1, 1);

    if (bytestransfered > 0)
    {
      Monitorport.print(F("YModem transfer completed "));
      Monitorport.print(bytestransfered);
      Monitorport.println(F(" bytes sent"));
    }
    else
    {
      Monitorport.println(F("YModem transfer FAILED"));
    }
    Monitorport.println();

    setCursor(0, 4);
    disp.print(F("            "));
    digitalWrite(LED1, LOW);
    SDDTFileSaved = false;
    //Monitorport.println(F("YModem transfer finished"));
    //Monitorport.println();
    Monitorport.println(F("Wait for file"));
  }
#endif
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

  YModemSerial.begin(115200);

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

  disp.begin();
  disp.fillScreen(ILI9341_BLACK);
  disp.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  disp.setRotation(rotation);
  disp.setTextSize(textscale);
  setCursor(0, 0);
  disp.print(F("Waiting File"));

  SDDTSegmentNext = 0;
  SDDTFileOpened = false;
  SDDTFileSaved = false;                              //file has not been saved to SD yet

#ifdef ENABLEMONITOR
  Monitorport.println(F("SDfile transfer receiver ready"));
  Monitorport.println();
#endif
}
