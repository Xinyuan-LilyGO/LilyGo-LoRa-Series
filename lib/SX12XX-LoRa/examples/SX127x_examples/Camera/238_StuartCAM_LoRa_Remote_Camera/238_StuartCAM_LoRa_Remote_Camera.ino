/*******************************************************************************************************
  Based on the OV2640 Arducam programs.

  ArduCAM demo (C)2017 Lee
  Web: http://www.ArduCAM.com
 *******************************************************************************************************/

/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a demonstration program that uses LoRa to transfer an image taken with an
  OV2640 Arducam camera. The LoRa transfer is carried out using the data transfer functions of the 
  SX12XX-LoRa library. Program tested on Arduino DUE.

  The Arducam software takes an image and saves it to a SD card. The filename is then passed across to
  the LoRa library which transfers the file already on SD across to the remote receiver, which is running
  program 239_StuartCAM_LoRa_Receiver or program 234_SDfile_Transfer_Receiver.

  Serial monitor baud rate is set at 115200
*******************************************************************************************************/

#include <ArduCAM.h>                    //get library here > https://github.com/ArduCAM/Arduino
#include <Wire.h>
#include <SPI.h>
#include "memorysaver.h"                //part of Arducam library

#include <SX127XLT.h>
#include <ProgramLT_Definitions.h>
#include "Settings.h"                   //LoRa settings etc.

SX127XLT LoRa;                          //create an SX127XLT library instance called LoRa

#define ENABLEMONITOR                   //enable monitor prints
#define PRINTSEGMENTNUM                 //enable this define to print segment numbers 
//#define DEBUG                         //enable this define to print debug info for segment transfers
//#define DEBUGSD                       //enable this defien to print SD file debug info
#define ENABLEFILECRC                   //enable this define to uses and show file CRCs
//#define DISABLEPAYLOADCRC             //enable this define if you want to disable payload CRC checking

#define SDLIB                           //define SDLIB for SD.h or SDFATLIB for SDfat.h
//#define SDFATLIB

#include "DTSDlibrary.h"                //part of SX12XX library  
#include "SDtransfer.h"                 //part of SX12XX library

char FileName[13];                      //filename passed to both Arducam and LoRa routines

ArduCAM myCAM( OV2640, OV2640CS );
#include "OV2640.h"                     //include local Arducam code for OV2640 camera 


void loop()
{
  myCAMSaveToSDFile(FileName, sizeof(FileName));   //when finished FileName[] contains name of file saved on SD
  Serial.print(FileName);
  Serial.println(F(" Saved to SD card"));
  SDsendFile(FileName, sizeof(FileName));            //transfer image via LoRa
  Serial.println();
  Serial.println();
  delay(60000);                                    //wait 60 seconds
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
  pinMode(LED1, OUTPUT);                      //setup pin as output for indicator LED
  led_Flash(2, 125);                          //two quick LED flashes to indicate program start
  SDsetLED(LED1);                             //setup LED pin for data transfer indicator
  
  digitalWrite(NSS, HIGH);
  pinMode(NSS, OUTPUT);                       //set the LoRa NSS as an output
  digitalWrite(OV2640CS, HIGH);
  pinMode(OV2640CS, OUTPUT);                  //set the camera CS as an output
  digitalWrite(SDCS, HIGH);
  pinMode(SDCS, OUTPUT);                      //set the SDCS as an output
  

  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin();
  SPI.begin();

  //Initialize SD Card
  while (!SD.begin(SDCS))
  {
    Serial.println(F("SD Card Error"));
    while (1)
    {
      led_Flash(50, 50);                       //long fast speed flash indicates device error
    }
  }
  Serial.println(F("SD Card detected"));

  //Initialize camera
  setupOV2640(OV2640resolution);               //resolution choice, see Settings.h file

  //Initialise LoRa device
  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa device found"));
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("LoRa device error"));
    while (1)
    {
      led_Flash(50, 50);                       //long fast speed flash indicates device error
    }
  }

  Serial.println();

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);
  LoRa.printOperatingSettings();
  Serial.println();
  LoRa.printModemSettings();
  Serial.println();
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

}
