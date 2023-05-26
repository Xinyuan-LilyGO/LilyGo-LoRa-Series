/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup.

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define LED1 8                                  //on board LED, high for on
#define DIO0 3                                  //DIO0 pin on LoRa device, used for RX and TX done 

#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using


//*******  Setup Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint32_t tone_lengthmS = 3000;            //tone length in mS
const uint16_t tone_frequencyHz = 500;          //tone frequency in hertz
const float adjustfreq = 0.9;                   //adjustment to tone frequency
const uint16_t deviation = 10000;               //frequency deviation of RF output carrier
const int8_t transmit_power = 2;                //LoRa transmit power in dBm
const uint32_t transmission_delaymS = 2000;     //mS delay between transmissions
