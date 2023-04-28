/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 06/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup.

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define RFBUSY 7                                //RFBUSY pin on LoRa device
#define DIO1 3                                  //DIO1 on LoRa device, used for RX and TX done 
#define LED1 8                                  //On board LED, high for on

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 2445000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t BandwidthBitRate = FLRC_BR_1_300_BW_1_2;  //FLRC bandwidth and bit rate, 1.3Mbs
//const uint8_t BandwidthBitRate = FLRC_BR_0_260_BW_0_3;    //FLRC 260kbps               
const uint8_t CodingRate = FLRC_CR_1_0;                   //FLRC coding rate
const uint8_t BT = RADIO_MOD_SHAPING_BT_1_0;              //FLRC BT
const uint32_t Syncword = 0x01234567;                     //FLRC uses syncword
