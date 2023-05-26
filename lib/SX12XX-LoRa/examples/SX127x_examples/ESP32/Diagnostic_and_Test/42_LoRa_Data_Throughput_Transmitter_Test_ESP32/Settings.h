/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 26/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the an ESP32 shield base.

#define SCK 18                                  //SCK on SPI3
#define MISO 19                                 //MISO on SPI3 
#define MOSI 23                                 //MOSI on SPI3 

#define NSS 5                                   //select pin on LoRa device
#define NRESET 27                               //reset pin on LoRa device
#define RFBUSY 25                               //busy line
#define DIO1 35                                 //DIO1 pin on LoRa device, used for RX and TX done 

#define LED1 2                                  //on board LED, high for on


#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using


//*******  Setup LoRa Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_500;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting, normally set to auto

const int8_t TXpower = 14;                      //LoRa transmit power in dBm


//*******  Setup packet parameters Here ! ***************
const uint8_t numberPackets = 100;              //number of packets to send in transmit loop
const uint8_t TXPacketL = 255;                  //length of packet to send  
const bool waitforACK = false;                  //set to true to have transmit wait for ack before continuing


