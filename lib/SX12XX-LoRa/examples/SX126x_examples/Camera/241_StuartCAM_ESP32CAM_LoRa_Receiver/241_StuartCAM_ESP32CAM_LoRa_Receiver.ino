/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -  This is a receiver program for an ESP32CAM board that has an SPI LoRa module set up
  on the following pins; NSS 12, NRESET 14, SCK 4, MISO 13, MOSI 2, RFBUSY 15, 3.3V VCC and GND. All other
  pins on the SX126X are not connected. The received pictures are saved to the ESP32CAMs SD card.

  Note that the white LED on pin 4 or the transistor controlling it need to be removed so that the LoRa
  device can properly use pin 4.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>
#include "FS.h"                            //SD Card ESP32
#include "SD_MMC.h"                        //SD Card ESP32
#include "soc/soc.h"                       //disable brownout problems
#include "soc/rtc_cntl_reg.h"              //disable brownout problems
#include "driver/rtc_io.h"
#include <SX126XLT.h>                      //SX12XX-LoRa library
#include <ProgramLT_Definitions.h>         //part of SX12XX-LoRa library
#include "Settings.h"                      //LoRa settings etc.

#define ENABLEMONITOR                      //enable this define to monitor data transfer information, needed for ARtransferIRQ.h
#define ENABLEARRAYCRC                     //enable this define to check and print CRC of sent array                   
#define PRINTSEGMENTNUM                    //enable this define to print segment numbers during data transfer
//#define DISABLEPAYLOADCRC                //enable this define if you want to disable payload CRC checking
//#define DEBUG                            //enable more detail of transfer progress


SX126XLT LoRa;                             //create an SX126XLT library instance called LoRa
#include <ARtransferIRQ.h>

uint8_t *PSRAMptr;                         //create a global pointer to the array to send, so all functions have access
bool SDOK;
bool savedtoSDOK;


void loop()
{
  uint32_t arraylength;
  SDOK = false;
  Serial.println(F("LoRa file transfer receiver ready"));
  setupLoRaDevice();

  //if there is a successful array transfer the returned length > 0
  arraylength = ARreceiveArray(PSRAMptr, sizeof(ARDTarraysize), ReceiveTimeoutmS);

  SPI.end();

  digitalWrite(NSS, HIGH);
  digitalWrite(NRESET, HIGH);

  if (arraylength)
  {
    Serial.print(F("Returned picture length "));
    Serial.println(arraylength);
    if (initMicroSDCard())
    {
      SDOK = true;
      Serial.println("SD Card OK");
      Serial.print(ARDTfilenamebuff);
      Serial.println(F(" Save picture to SD card"));

      fs::FS &fs = SD_MMC;                            //save picture to microSD card
      File file = fs.open(ARDTfilenamebuff, FILE_WRITE);
      if (!file)
      {
        Serial.println("*********************************************");
        Serial.println("ERROR Failed to open SD file in writing mode");
        Serial.println("*********************************************");
        savedtoSDOK = false;
      }
      else
      {
        file.write(PSRAMptr, arraylength); // pointer to array and length
        Serial.print(ARDTfilenamebuff);
        Serial.println(" Saved to SD");
        savedtoSDOK = true;
      }
      file.close();
      SD_MMC.end();
    }
    else
    {
      Serial.println("No SD available");
    }
  }
  else
  {
    Serial.println(F("Error receiving picture"));
    if (ARDTArrayTimeout)
    {
      Serial.println(F("Timeout receiving picture"));
    }
  }
  Serial.println();
}


bool setupLoRaDevice()
{
  SPI.begin(SCK, MISO, MOSI, NSS);

  if (LoRa.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))
  {
    Serial.println(F("LoRa device found"));
  }
  else
  {
    Serial.println(F("LoRa Device error"));
    return false;
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

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
  return true;
}


bool initMicroSDCard()
{
  if (!SD_MMC.begin("/sdcard", true))               //use this line for 1 bit mode, pin 2 only, 4,12,13 not used
  {
    Serial.println("*****************************");
    Serial.println("ERROR - SD Card Mount Failed");
    Serial.println("*****************************");
    return false;
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD Card found");
    return false;
  }
  return true;
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;
  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(REDLED, HIGH);
    delay(delaymS);
    digitalWrite(REDLED, LOW);
    delay(delaymS);
  }
}


void setup()
{
  uint32_t available_PSRAM_size;
  uint32_t new_available_PSRAM_size;

  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);      //disable brownout detector
  pinMode(REDLED, OUTPUT);                       //setup pin as output for indicator LED
  led_Flash(2, 125);                             //two quick LED flashes to indicate program start
  ARsetDTLED(REDLED);                            //setup LED pin for data transfer indicator

  digitalWrite(NSS, HIGH);
  pinMode(NSS, OUTPUT);                          //disable LoRa device for now

  Serial.begin(115200);  
  Serial.println();
  Serial.println(__FILE__);

  if (psramInit())
  {
    Serial.println("PSRAM is correctly initialised");
    available_PSRAM_size = ESP.getFreePsram();
    Serial.println((String)"PSRAM Size available: " + available_PSRAM_size);
  }
  else
  {
    Serial.println("PSRAM not available");
    while (1);
  }

  Serial.println("Allocate array in PSRAM");
  uint8_t *byte_array = (uint8_t *) ps_malloc(ARDTarraysize * sizeof(uint8_t));
  PSRAMptr = byte_array;                              //save the pointe to byte_array to global pointer

  new_available_PSRAM_size = ESP.getFreePsram();
  Serial.println((String)"PSRAM Size available: " + new_available_PSRAM_size);
  Serial.print("PSRAM array bytes allocated: ");
  Serial.println(available_PSRAM_size - new_available_PSRAM_size);
  Serial.println();
}
