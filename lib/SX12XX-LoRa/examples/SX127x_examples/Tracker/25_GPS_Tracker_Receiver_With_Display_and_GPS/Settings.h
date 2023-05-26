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

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define DIO0 3                                  //DIO0 on LoRa device, used for RX and TX done 
#define LED1 8                                  //On board LED, high for on
#define BUZZER -1                               //Buzzer if fitted, high for on. Set to -1 if not used      

#define RXpin A3                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER 4                              //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE LOW                          //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE HIGH                        //logic level to turn GPS off via pin GPSPOWER

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 10;                      //LoRa transmit power in dBm


//**************************************************************************************************
// GPS Settings
//**************************************************************************************************

#define USESOFTSERIALGPS                         //if your using software serial for the GPS, enable this define     
//#define USEHARDWARESERIALGPS                   //if your using hardware serial for the GPS, enable this define
//#define HARDWARESERIALPORT Serial2             //if your using hardware serial for the GPS, define the port here  

#define GPSBaud 9600                             //GPS Baud rate   

#define NoRXGPSfixms 15000                       //max number of mS to allow before no local GPS fix flagged 
#define DisplayRate 7                            //when working OK the GPS will get a new fix every second or so
                                                 //this rate defines how often the display should be updated
