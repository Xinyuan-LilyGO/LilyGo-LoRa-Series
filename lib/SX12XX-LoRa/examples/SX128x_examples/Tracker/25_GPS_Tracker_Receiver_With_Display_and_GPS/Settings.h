/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitiosn to match your own setup. Some pins such as DIO1,
//DIO2, BUZZER SWITCH1 may not be in used by this sketch so they do not need to be
//connected and should be set to -1.

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

#define RXpin A3                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER 4                              //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE HIGH                         //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                         //logic level to turn GPS off via pin GPSPOWER

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using
 

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 2445000000;           //frequency of transmissions
const int32_t Offset = 0;                        //offset frequency for calibration purposes
const uint8_t Bandwidth = LORA_BW_0200;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF12;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate

//**************************************************************************************************
// GPS Settings
//**************************************************************************************************

#define USE_SOFTSERIAL_GPS                       //need to include this if we are using softserial for GPS     
//#define HardwareSerialPort Serial1             //if using hardware serial enable this define for hardware serial port 

#define GPSBaud 9600                             //GPS Baud rate   
#define WaitGPSFixSeconds 30                     //time to wait for a new GPS fix 
#define echomS 2000                              //number of mS to run GPS echo for at startup    

#define NoRXGPSfixms 15000                       //max number of mS to allow before no local fix flagged 
#define DisplayRate 7                            //when working OK the GPS will get a new fix every second or so
                                                 //this rate defines how often the display should be updated


