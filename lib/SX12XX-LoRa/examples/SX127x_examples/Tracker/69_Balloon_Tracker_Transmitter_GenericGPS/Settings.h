                             /*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/12/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//**************************************************************************************************
// 1) Hardware related definitions and options - specify lora board type and pins here
//**************************************************************************************************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup. 

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define DIO0 3                                  //DIO0 on LoRa device, used for RX and TX done 
#define LED1 8                                  //On board LED, high for on
#define BATVREADON 8                            //Pin that turns on the resistor divider to read battery volts
#define ONE_WIRE_BUS 4                          //for DS18B20 temperature sensor 
#define ADMultiplier 5.25                       //adjustment to convert into mV of battery voltage. for 100K\10K divider 
#define SupplyAD A0                             //Resistor divider for battery connected here 

#define RXpin A3                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER -1                             //Pin that powers GPS on\off, set to -1 if not used
#define GPSONSTATE HIGH                         //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                         //logic level to turn GPS off via pin GPSPOWER 

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

//**************************************************************************************************
// 2) Program Options
//**************************************************************************************************

//#define ClearAllMemory                          //Clears memory of stored tracker information, counts, errors etc

//**************************************************************************************************
// 3) LoRa modem settings 
//**************************************************************************************************

//LoRa Modem Parameters
const uint32_t Offset = 0;                       //offset frequency for calibration purposes

//Tracker mode
const uint32_t TrackerFrequency = 434000000;     //frequency of transmissions
const uint8_t TrackerBandwidth = LORA_BW_062;    //LoRa bandwidth
const uint8_t TrackerSpreadingFactor = LORA_SF8; //LoRa spreading factor
const uint8_t TrackerCodeRate = LORA_CR_4_5;     //LoRa coding rate
const uint8_t TrackerOptimisation = LDRO_AUTO;   //low data rate optimisation setting
const int8_t TrackerTXpower = 10;                //LoRa TX power in dBm

//Search mode
const uint32_t SearchFrequency = 434000000;      //frequency of transmissionsconst 
uint8_t SearchBandwidth = LORA_BW_062;           //LoRa bandwidth
const uint8_t SearchSpreadingFactor = LORA_SF12; //LoRa spreading factor
const uint8_t SearchCodeRate = LORA_CR_4_5;      //LoRa coding rate
const uint8_t SearchOptimisation = LDRO_AUTO;    //low data rate optimisation setting
const int8_t SearchTXpower = 10;                 //LoRa TX power in dBm

const uint16_t deviation = 10000;                //deviation in hz for FM tones
const float adjustfreq = 0.9;                    //adjustment to tone frequency 

const uint8_t TXBUFFER_SIZE = 128;               //defines the maximum size of the trasnmit buffer;


//**************************************************************************************************
// 4) GPS Options - Settings for a generic GPS, no attempt is made to put the GPS into balloon mode
//**************************************************************************************************

#define GPSBaud 9600                              //GPS Baud rate

#define USESOFTSERIALGPS                          //if your using software serial for the GPS, enable this define      

//#define USEHARDWARESERIALGPS                    //if your using hardware serial for the GPS, enable this define
#define HARDWARESERIALPORT Serial1                //if your using hardware serial for the GPS, define the port here  

const uint16_t WaitGPSFixSeconds = 60;            //when in flight the time to wait for a new GPS fix

//**************************************************************************************************
// 5) FSK RTTY Settings
//**************************************************************************************************

uint32_t FrequencyShift = 500;                    //hertz frequency shift, approx, sent at nearest 61.03515625hz step  
uint8_t NumberofPips = 4;                         //number of marker pips to send
uint16_t PipDelaymS = 1000;                       //mS between pips when carrier is off 
uint16_t PipPeriodmS = 100;                       //mS length of pip
uint16_t BaudPerioduS = 10000;                    //uS period for baud, 10000uS for 100baud 
uint16_t LeadinmS = 1000;                         //ms of leadin constant shifted carrier 


//****************************************************************************************************
// 6) Program Default Option settings - This section determines which options are on or off by default,
//    these are saved in the Default_config1 byte. These options are set in this way so that it is 
//    possible (in future program changes) to alter the options remotly. 
//**************************************************************************************************

uint8_t OptionOff = 0;
uint8_t OptionOn = 1;

const char option_SearchEnable = OptionOn;       //set to OptionOn to enable transmit of Search mode packet       
const char option_FSKRTTYEnable = OptionOn;      //set to OptionOn to enable transmit of FSKRTTY

#define option_SearchEnable_SUM (option_SearchEnable*1)
#define option_FSKRTTYEnable_SUM (option_FSKRTTYEnable*4)

const uint16_t Default_config1 = (option_SearchEnable_SUM + option_FSKRTTYEnable_SUM);
//const uint16_t Default_config1 = 0x05;      //Phew, the default config can always be set manually........
                                                  //0x05 would turn on transmit of search mode and FSKRTTY 

                                                   
//**************************************************************************************************
// 7) Memory settings - define the type of memory to use for non-Volatile storage.
//    Default is internal ATmega device EEPROM but EEPROM has a limited write endurance of 'only' 
//    100,000 writes. Since the non-Volatile memory selected is written to at each transmission loop
//    and error, its highly recommended to use one of the FRAM options, these have an endurance of
//    100,000,000,000,000 writes.   
//**************************************************************************************************

#define Memory_Library <EEPROM_Memory.h>
//#define Memory_Library <FRAM_MB85RC16PNF.h>
//#define Memory_Library <FRAM_FM24CL64.h>

int16_t Memory_Address = 0x50;                     //default I2C address of MB85RC16PNF and FM24CL64 FRAM

//**************************************************************************************************
// 8) HAB Flight Settings
//**************************************************************************************************

char FlightID[] = "Flight1";                       //flight ID for HAB packet

const uint16_t SleepTimesecs = 13;             //sleep time in seconds after each TX loop

const char ThisNode = '1';                         //tracker number for search packet


