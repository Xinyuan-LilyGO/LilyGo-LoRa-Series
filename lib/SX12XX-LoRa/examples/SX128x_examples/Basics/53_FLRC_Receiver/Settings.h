/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/09/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup. Some pins such as DIO2,
//DIO3, BUZZER may not be in used by this sketch so they do not need to be
//connected and should be included and be set to -1.

#define NSS 10
#define RFBUSY 7
#define NRESET 9
#define LED1 8
#define DIO1 3
#define RX_EN -1                //pin for RX enable, used on some SX1280 devices, set to -1 if not used
#define TX_EN -1                //pin for TX enable, used on some SX1280 devices, set to -1 if not used                        


#define LORA_DEVICE DEVICE_SX1280                         //we need to define the device we are using  

//FLRC Modem Parameters
const uint32_t Frequency = 2445000000;                    //frequency of transmissions
const int32_t Offset = 0;                                 //offset frequency for calibration purposes

const uint8_t BandwidthBitRate = FLRC_BR_1_300_BW_1_2;    //FLRC bandwidth and bit rate, 1.3Mbs
const uint8_t CodingRate = FLRC_CR_1_0;                   //FLRC coding rate
const uint8_t BT = RADIO_MOD_SHAPING_BT_1_0;              //FLRC BT
const uint32_t Syncword = 0x01234567;                     //FLRC uses syncword
const int8_t TXpower  = 0;                                //power for transmissions in dBm

const uint16_t packet_delay = 1000;                       //mS delay between packets

#define RXBUFFER_SIZE 127                                 //Max RX buffer size  
