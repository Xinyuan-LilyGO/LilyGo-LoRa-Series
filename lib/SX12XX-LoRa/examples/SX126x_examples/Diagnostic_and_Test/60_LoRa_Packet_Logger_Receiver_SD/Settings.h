/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 02/04/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, An ATmeag1284P shield base for my BBF modules. 
//Be sure to change the definitions to match your own setup. Some pins such as DIO2, DIO3, BUZZER may not
//be in used by this sketch so they do not need to be connected and should be included and be set to -1.

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define LED1 8                                  //on board LED, high for on
#define RFBUSY 7                                //SX126X busy pin 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for RX and TX done 
#define SW -1                                   //SW pin on Dorji devices is used to turn RF switch on\off, set to -1 if not used    
#define BUZZER 4                                //pin for buzzer, on when logic high
#define SDCS 30                                 //CS pin for SD card     

#define LORA_DEVICE DEVICE_SX1262               //we need to define the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 2;                       //LoRa TX power

#define packet_delay 1000                       //mS delay between packets


#define RXBUFFER_SIZE 255                        //RX buffer size 
