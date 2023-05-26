/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 22/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini, 
//be sure to change the definitiosn to match your own setup. 

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define RFBUSY 7                                //SX128X busy pin 
#define DIO1 3                                  //DIO1 on LoRa device, used for RX and TX done 
#define RX_EN -1                                //pin for RX enable, used on some SX1280 devices, set to -1 if not used
#define TX_EN -1                                //pin for TX enable, used on some SX1280 devices, set to -1 if not used 

#define LED1 8                                  //On board LED, high for on

#define BUZZER -1                               //Buzzer if fitted, high for on. Set to -1 if not used      

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 2445000000;           //frequency of transmissions
const int32_t Offset = 0;                        //offset frequency for calibration purposes
const uint8_t Bandwidth = LORA_BW_0200;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF12;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate
