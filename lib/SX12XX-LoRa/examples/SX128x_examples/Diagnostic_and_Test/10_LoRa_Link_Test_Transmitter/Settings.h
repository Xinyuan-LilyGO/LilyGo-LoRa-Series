/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitiosn to match your own setup. Some pins such as DIO2,
//DIO3 may not be in used by this sketch so they do not need to be connected and should
//be set to -1.

#define NSS 10                      //select pin on LoRa device
#define NRESET 9                    //reset pin on LoRa device
#define LED1 8                      //on board LED, high for on
#define RFBUSY 7                    //SX128X busy pin 
#define DIO1 3                      //DIO1 pin on LoRa device, used for RX and TX done 
#define DIO2 -1                     //DIO2 pin on LoRa device, normally not used so set to -1 
#define DIO3 -1                     //DIO3 pin on LoRa device, normally not used so set to -1
#define RX_EN -1                    //pin for RX enable, used on some SX128X devices, set to -1 if not used
#define TX_EN -1                    //pin for TX enable, used on some SX128X devices, set to -1 if not used

#define LORA_DEVICE DEVICE_SX1280   //this is the device we are using


//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
#define Frequency 2445000000                     //frequency of transmissions
#define Offset 0                                 //offset frequency for calibration purposes  
#define Bandwidth LORA_BW_0400                   //LoRa bandwidth
#define SpreadingFactor LORA_SF7                 //LoRa spreading factor
#define CodeRate LORA_CR_4_5                     //LoRa coding rate

const int8_t TXpower = 10;                       //Transmit power used when sending packet starting test sequence
const int8_t start_power = 10;                   //link test starts at this transmit power, maximum +12dBm
const int8_t end_power = -18;                    //and ends at this power, minimum -18dBm
const uint8_t ThisNode = 'T';                    //this identifies the node in transmissions


#define packet_delay 250                         //mS delay between packets
#define mode_delaymS 2000                        //mS delay after sending start test sequence  
