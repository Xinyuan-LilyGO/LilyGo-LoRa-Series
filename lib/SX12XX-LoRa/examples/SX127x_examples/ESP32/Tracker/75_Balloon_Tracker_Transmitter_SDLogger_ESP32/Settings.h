/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 08/06/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//**************************************************************************************************
// 1) Hardware related definitions and options - specify lora board type and pins here
//**************************************************************************************************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup. 

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
#define ONE_WIRE_BUS 32                         //for DS18B20 temperature sensor
#define SupplyAD 36                             //pin for reading supply\battery voltage
#define BATVREADON 2                            //turns on battery resistor divider, high for on
#define ADMultiplier 11.65                      //Multiplier for conversion of AD reading to mV
#define SDCS 13                                 //microSD card select 

#define RXpin 17                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin 16                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER 26                             //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE LOW                          //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE HIGH                        //logic level to turn GPS off via pin GPSPOWER 

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

//**************************************************************************************************
// 2) Program Options
//**************************************************************************************************

#define ClearAllMemory                          //Clears memory of stored tracker information, counts, errors etc

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

const byte TXBUFFER_SIZE = 128;                   //defines the maximum size of the trasnmit buffer;


//**************************************************************************************************
// 4) GPS Options
//**************************************************************************************************

#define GPSBaud 9600                              //GPS Baud rate

#define HARDWARESERIALPORT Serial2                //if your using hardware serial for the GPS, define it here  

const uint16_t WaitGPSFixSeconds = 60;            //when in flight the time to wait for a new GPS fix 

#define GPS_Library <UBLOXSerialGPS.h>            //use library file for UBLOX GPS                    
//#define GPS_Library <QuectelSerialGPS.h>        //use library file for Quectel GPS


//**************************************************************************************************
// 5) FSK RTTY Settings
//**************************************************************************************************

uint32_t FrequencyShift = 500;                    //hertz frequency shift for audio  
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

const char option_SearchEnable = OptionOff;       //set to OptionOn to enable transmit of Search mode packet       
const char option_FSKRTTYEnable = OptionOff;      //set to OptionOn to enable transmit of FSKRTTY

#define option_SearchEnable_SUM (option_SearchEnable*1)
#define option_FSKRTTYEnable_SUM (option_FSKRTTYEnable*4)

const unsigned int Default_config1 = (option_SearchEnable_SUM + option_FSKRTTYEnable_SUM);
//const unsigned int Default_config1 = 0x05;      //Phew, the default config can always be set manually........
                                                  //0x05 would turn on transmit of search mode and FSKRTTY 

                                                   
//**************************************************************************************************
// 7) Memory settings - define the type of memory to use for non-Volatile storage.
//    The ESP32 version of the HAB tracker transmitter only supports the use of FRAM here. 
//**************************************************************************************************

//#define Memory_Library <FRAM_MB85RC16PNF.h>
#define Memory_Library <FRAM_FM24CL64.h>

int16_t Memory_Address = 0x50;                     //default I2C address of MB85RC16PNF and FM24CL64 FRAM

//**************************************************************************************************
// 8) HAB Flight Settings
//**************************************************************************************************

char FlightID[] = "Flight1";                       //flight ID for HAB packet

const unsigned int SleepTimesecs = 13;             //sleep time in seconds after each TX loop

const char ThisNode = '1';                         //tracker number for search packet


