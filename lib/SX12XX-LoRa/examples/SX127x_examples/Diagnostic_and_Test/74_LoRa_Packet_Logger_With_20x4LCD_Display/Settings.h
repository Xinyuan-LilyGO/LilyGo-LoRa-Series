/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards based on a ATMega328 ProMini
//be sure to change the definitions to match your own setup.

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define LED1 8                                  //on board LED, high for on
#define DIO0 3                                  //DIO0 pin on LoRa device, used for RX and TX done 
#define BUZZER -1                               //pin for buzzer, on when logic high, set to -1 to turn buzzer off
#define SDCS 4                                  //CS pin for SD card 

#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using

#define DisplayAddress 0x3F                     //I2C address of PCF8574 on display, common options are 0x27 and 0x3F

//*******  Setup LoRa Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting, normally set to auto

const uint32_t RXtimeoutmS = 60000;             //RXtimeout in mS
const uint8_t RXBUFFER_SIZE = 128;              //RX buffer size

const uint8_t LCD20x4_I2C_ADDRESS = 0x3F;       //I2C address of PCF8574 controller on LCD, could be 0x27 or 0x3F
