/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of the Tracker boards, the ESP32_Micro_Node, be sure to change
//them to match your own setup. You will also need to connect up the pins for the SPI bus, which on the 
//ESP32_Micro_Node are SCK on pin 18, MISO on pin 19 and MOSI on pin 23. Some pins such as DIO1, DIO2 and
//BUZZER may not be in used by this sketch so they do not need to be connected and should be set to -1.

#define NSS 5                                   //select pin on LoRa device
#define SCK 18                                  //SCK on SPI3
#define MISO 19                                 //MISO on SPI3 
#define MOSI 23                                 //MOSI on SPI3 

#define NRESET 27                               //reset pin on LoRa device
#define LED1 2                                  //on board LED, high for on
#define DIO0 35                                 //DIO0 pin on LoRa device, used for RX and TX done 
#define DIO1 -1                                 //DIO1 pin on LoRa device, normally not used so set to -1 
#define DIO2 -1                                 //DIO2 pin on LoRa device, normally not used so set to -1
#define BUZZER -1                               //pin for buzzer, set to -1 if not used 
#define VCCPOWER 14                             //pin controls power to external devices
#define BATTERYAD 36                            //pin for reading supply voltage
#define BATVREADON 25                           //when high turns on the resistor divider to measure voltage, -1 if not used
#define ADMultiplier 11.65                      //Multiplier for conversion of AD reading to mV

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 14;                      //LoRa transmit power in dBm

#define BME280_ADDRESS  0x76                    //I2C bus address of BME280 
#define BME280_REGISTER_CONTROL 0xF4            //BME280 register number for power control

const uint16_t TIME_TO_SLEEP = 16;              //Time ESP32 will go to sleep (in seconds)
