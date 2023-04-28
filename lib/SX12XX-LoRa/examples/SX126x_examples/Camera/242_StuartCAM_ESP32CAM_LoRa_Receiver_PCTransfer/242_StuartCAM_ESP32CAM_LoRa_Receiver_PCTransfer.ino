/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -  This is a receiver program for an ESP32CAM board that has an SPI LoRa module set up
  on the following pins; NSS 12, NRESET 14, SCK 4, MISO 13, MOSI 2, RFBUSY 15, 3.3V VCC and GND. All other
  pins on the SX126X are not connected. The received pictures are saved to the ESP32CAMs SD card and also 
  transferred to a connected PC using the YModem protocol over the normal program upload port. Progress
  or debug messages can be seen by connection an additional serial adapter to pin 33 on the ESP32CAM.

  For details of the PC upload process see here;

  https://stuartsprojects.github.io/2022/02/05/Long-Range-Wireless-Adapter-for-ESP32CAM.html

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

#include "YModemArray.h"


void loop()
{
  uint32_t arraylength, bytestransfered;
  SDOK = false;
  Monitorport.println(F("LoRa file transfer receiver ready"));

  setupLoRaDevice();

  //if there is a successful array transfer the returned length > 0
  arraylength = ARreceiveArray(PSRAMptr, sizeof(ARDTarraysize), ReceiveTimeoutmS);

  SPI.end();

  digitalWrite(NSS, HIGH);
  digitalWrite(NRESET, HIGH);

  if (arraylength)
  {
    Monitorport.print(F("Returned picture length "));
    Monitorport.println(arraylength);
    if (initMicroSDCard())
    {
      SDOK = true;
      Monitorport.println("SD Card OK");
      Monitorport.print(ARDTfilenamebuff);
      Monitorport.println(F(" Save picture to SD card"));

      fs::FS &fs = SD_MMC;                            //save picture to microSD card
      File file = fs.open(ARDTfilenamebuff, FILE_WRITE);
      if (!file)
      {
        Monitorport.println("*********************************************");
        Monitorport.println("ERROR Failed to open SD file in writing mode");
        Monitorport.println("*********************************************");
        savedtoSDOK = false;
      }
      else
      {
        file.write(PSRAMptr, arraylength); // pointer to array and length
        Monitorport.print(ARDTfilenamebuff);
        Monitorport.println(" Saved to SD");
        savedtoSDOK = true;
      }
      file.close();
      SD_MMC.end();
    }
    else
    {
      Monitorport.println("No SD available");
    }
  }
  else
  {
    Monitorport.println(F("Error receiving picture"));
    if (ARDTArrayTimeout)
    {
      Monitorport.println(F("Timeout receiving picture"));
    }
  }
  Monitorport.println();

  if (arraylength)
  {
    Monitorport.println(F("File received - start YModem transfer to PC"));

    //bytestransfered = yModemSend(ARDTfilenamebuff, 1, 1);
    bytestransfered = yModemSend(ARDTfilenamebuff, PSRAMptr, arraylength, 1, 1);

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
  }
}


bool setupLoRaDevice()
{
  SPI.begin(SCK, MISO, MOSI, NSS);

  if (LoRa.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))
  {
    Monitorport.println(F("LoRa device found"));
  }
  else
  {
    Monitorport.println(F("LoRa Device error"));
    return false;
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

#ifdef DISABLEPAYLOADCRC
  LoRa.setReliableConfig(NoReliableCRC);
#endif

  if (LoRa.getReliableConfig(NoReliableCRC))
  {
    Monitorport.println(F("Payload CRC disabled"));
  }
  else
  {
    Monitorport.println(F("Payload CRC enabled"));
  }
  return true;
}


bool initMicroSDCard()
{
  if (!SD_MMC.begin("/sdcard", true))               //use this line for 1 bit mode, pin 2 only, 4,12,13 not used
  {
    Monitorport.println("*****************************");
    Monitorport.println("ERROR - SD Card Mount Failed");
    Monitorport.println("*****************************");
    return false;
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE)
  {
    Monitorport.println("No SD Card found");
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

  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);   //disable brownout detector
  pinMode(REDLED, OUTPUT);                       //setup pin as output for indicator LED
  led_Flash(2, 125);                             //two quick LED flashes to indicate program start
  ARsetDTLED(REDLED);                            //setup LED pin for data transfer indicator

  digitalWrite(NSS, HIGH);
  pinMode(NSS, OUTPUT);                          //disable LoRa device for now

  YModemSerial.begin(115200);
  Monitorport.begin(115200, SERIAL_8N1, RXD2, TXD2);  //monitor port, format is Monitorport.begin(baud-rate, protocol, RX pin, TX pin);
  Monitorport.println();
  Monitorport.println(__FILE__);

  if (psramInit())
  {
    Monitorport.println("PSRAM is correctly initialised");
    available_PSRAM_size = ESP.getFreePsram();
    Monitorport.println((String)"PSRAM Size available: " + available_PSRAM_size);
  }
  else
  {
    Monitorport.println("PSRAM not available");
    while (1);
  }

  Monitorport.println("Allocate array in PSRAM");
  uint8_t *byte_array = (uint8_t *) ps_malloc(ARDTarraysize * sizeof(uint8_t));
  PSRAMptr = byte_array;                              //save the pointe to byte_array to global pointer

  new_available_PSRAM_size = ESP.getFreePsram();
  Monitorport.println((String)"PSRAM Size available: " + new_available_PSRAM_size);
  Monitorport.print("PSRAM array bytes allocated: ");
  Monitorport.println(available_PSRAM_size - new_available_PSRAM_size);
  Monitorport.println();
}
