/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitiosn to match your own setup. 

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define RFBUSY 7                                //SX126X busy pin 
#define DIO1 3                                  //DIO1 on LoRa device, used for RX and TX done 
#define SW -1                                   //SW pin on Dorji devices is used to turn RF switch on\off, set to -1 if not used 

#define GPSPOWER -1                             //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE HIGH                         //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                         //logic level to turn GPS off via pin GPSPOWER 

#define RXpin A3                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                //pin number for GPS TX output from Arduino- RX into GPS

#define LED1 8                                  //On board LED, high for on
#define SupplyAD A7                             //pin for reading supply\battery voltage  
#define BATVREADON 8                            //turns on battery resistor divider, high for on, -1 if not used

const float ADMultiplier = 10.0;                //multiplier for supply volts calculation
#define DIODEMV 98                              //mV voltage drop accross diode at approx 8mA


#define LORA_DEVICE DEVICE_SX1262               //this is the device we are using 



//*******  Setup LoRa Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_062;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF12;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;          //low data rate optimisation setting

const int8_t TXpower = 10;                       //LoRa TX power

#define ThisNode '2'                             //a character that identifies this tracker

//**************************************************************************************************
// GPS Settings
//**************************************************************************************************

#define USE_SOFTSERIAL_GPS                       //need to include this if we are using softserial for GPS     
//#define HardwareSerialPort Serial1             //if using hardware serial enable this define for hardware serial port 

#define GPSBaud 9600                             //GPS Baud rate   

#define WaitGPSFixSeconds 30                     //time in seconds to wait for a new GPS fix 
#define WaitFirstGPSFixSeconds 1800              //time to seconds to wait for the first GPS fix at startup
#define Sleepsecs 5                              //seconds between transmissions, this delay is used to set overall transmission cycle time

#define echomS 2000                              //number of mS to run GPS echo at startup    


