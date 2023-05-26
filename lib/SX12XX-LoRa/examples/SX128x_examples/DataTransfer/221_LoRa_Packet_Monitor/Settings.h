/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 06/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup.

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //RFBUSY pin on LoRa device
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //LED used to indicate transmission

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

const uint32_t Frequency = 2445000000;                     //frequency of transmissions
const uint32_t Offset = 0;                                 //offset frequency for calibration purposes

//*******  Setup LoRa modem parameters here ! ***************
const uint8_t Bandwidth = LORA_BW_1600;                    //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;                  //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;                      //LoRa coding rate

const uint8_t BytesToPrint = 16;                           //number of bytes of packet to print
