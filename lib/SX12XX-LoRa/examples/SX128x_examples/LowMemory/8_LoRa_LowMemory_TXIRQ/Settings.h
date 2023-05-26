/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 18/09/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup.

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device
#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using

//LoRa Modem Parameters
const uint32_t Frequency = 2445000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_0400;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate

const int8_t TXpower = 10;                      //LoRa TX power in dBm
const uint16_t packet_delay = 1000;             //mS delay between packets
