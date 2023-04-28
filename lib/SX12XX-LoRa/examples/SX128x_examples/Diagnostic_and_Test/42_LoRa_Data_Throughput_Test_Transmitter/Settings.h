/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 26/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup. Some pins such as DIO1,
//DIO2, BUZZER are not used by this sketch so they do not need to be connected and
//should be set to -1.

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //RFBUSY pin on LoRa device   
#define LED1 8                                  //on board LED, high for on
#define DIO1 3                                  //DIO1 pin on LoRa device, used for RX and TX done 

#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using


//*******  Setup LoRa Parameters Here ! ***************

//LoRa Modem Parameters
#define Frequency 2445000000                    //frequency of transmissions
#define Offset 0                                //offset frequency for calibration purposes  

#define Bandwidth LORA_BW_0400                  //LoRa bandwidth
#define SpreadingFactor LORA_SF7                //LoRa spreading factor
#define CodeRate LORA_CR_4_5                    //LoRa coding rate

const int8_t TXpower = 10;                      //LoRa transmit power in dBm


//*******  Setup packet parameters Here ! ***************
const uint8_t numberPackets = 50;               //number of packets to send in transmit loop
const uint8_t TXPacketL = 16;                   //length of packet to send
const bool waitforACK = true;                   //set to true to have transmit wait for ack before continuing
