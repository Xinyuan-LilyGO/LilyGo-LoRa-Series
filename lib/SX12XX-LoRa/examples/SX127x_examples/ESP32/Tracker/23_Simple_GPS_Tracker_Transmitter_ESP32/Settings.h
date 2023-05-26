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


#define NSS 5                                   //select on LoRa device
#define SCK 18                                  //SCK on SPI3
#define MISO 19                                 //MISO on SPI3 
#define MOSI 23                                 //MOSI on SPI3 

#define NRESET 27                               //reset on LoRa device
#define DIO0 35                                 //DIO0 on LoRa device, used for RX and TX done 
#define DIO1 -1                                 //DIO1 on LoRa device, normally not used so set to -1
#define DIO2 -1                                 //DIO2 on LoRa device, normally not used so set to -1
#define LED1 2                                  //On board LED, high for on
#define BUZZER -1                               //Buzzer if fitted, high for on. Set to -1 if not used      
#define VCCPOWER 14                             //controls power to lora and SD card, set to -1 if not used 
#define SupplyAD 36                             //pin for reading supply\battery voltage
#define BATVREADON 25                           //turns on battery resistor divider, high for on
#define ADMultiplier 11.65                      //Multiplier for conversion of AD reading to mV

#define RXpin 17                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin 16                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER 26                             //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE LOW                          //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE HIGH                        //logic level to turn GPS off via pin GPSPOWER 
#define GPSserial Serial2                       //define GPSserial as ESP32 Serial2 

#define LORA_DEVICE DEVICE_SX1278                //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_062;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF12;      //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 10;                      //LoRa transmit power in dBm


#define ThisNode 'T'                             //a character that identifies this tracker

//**************************************************************************************************
// GPS Settings
//**************************************************************************************************

#define GPSBaud 9600                             //GPS Baud rate   

#define WaitGPSFixSeconds 30                     //time in seconds to wait for a new GPS fix 
#define WaitFirstGPSFixSeconds 1800              //time to seconds to wait for the first GPS fix at startup
#define Sleepsecs 5                              //seconds between transmissions, this delay is used to set overall transmission cycle time

#define echomS 2000                              //number of mS to run GPS echo at startup    


