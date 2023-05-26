/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 12/05/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


//**************************************************************************************************
// 1) Hardware related definitions and options - specify lora board type and pins here
//**************************************************************************************************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitiosn to match your own setup. 

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define DIO1 3                                  //DIO1 on LoRa device, used for RX and TX done 
#define RFBUSY 7                                //busy pin on LoRa device
#define LED1 8                                  //On board LED, high for on
#define SWITCH1 2                               //if pin shorted to ground, switch is active

#define RXpin A3                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER -1                             //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE HIGH                         //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                         //logic level to turn GPS off via pin GPSPOWER

#define AUDIOOUT 4                              //pin used to output Audio tones for HAB packet upload 
#define CHECK -1                                //this pin is toggled inside the AFSKRTTY library, high for logic 1, low for logic 0, so it can be used to check the timing.

#define LORA_DEVICE DEVICE_SX1262               //this is the LoRa device we are using

//**************************************************************************************************
// 2) Program Options
//**************************************************************************************************



//**************************************************************************************************
// 3) LoRa modem settings 
//**************************************************************************************************

const uint32_t Offset = 0;                             //offset frequency for calibration purposes

//Tracker mode
const uint32_t TrackerFrequency = 434000000;           //frequency of transmissions
const uint8_t TrackerBandwidth = LORA_BW_062;          //LoRa bandwidth
const uint8_t TrackerSpreadingFactor = LORA_SF8;       //LoRa spreading factor
const uint8_t TrackerCodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t TrackerOptimisation = LDRO_AUTO;         //low data rate optimisation setting
const int8_t TrackerTXpower = 10;                      //LoRa TX power in dBm
const uint8_t TrackerMode = 1;                         //used for receiver to tell whatmode it is in

//Search mode
const uint32_t SearchFrequency = 434000000;            //frequency of transmissions
const uint8_t SearchBandwidth = LORA_BW_062;;          //LoRa bandwidth
const uint8_t SearchSpreadingFactor = LORA_SF12;       //LoRa spreading factor
const uint8_t SearchCodeRate = LORA_CR_4_5;            //LoRa coding rate
const uint8_t SearchOptimisation = LDRO_AUTO;          //low data rate optimisation setting
const int8_t SearchTXpower = 10;                       //LoRa TX power in dBm
const uint8_t SearchMode = 2;                          //used for receiver to tell whatmode it is in

const uint8_t RXBUFFER_SIZE = 128;                     //RX buffer size

//**************************************************************************************************
// 4) GPS Options
//**************************************************************************************************

const uint16_t GPSBaud = 9600;                   //GPS Baud rate

#define USESOFTSERIALGPS                         //need to include this if we are using softserial for GPS     
//#define HARDWARESERIALPORT Serial1             //if using hardware serial enable this define for hardware serial port 

const uint16_t WaitGPSFixSeconds = 30;           //time to wait for a new GPS fix 

const uint16_t NoRXGPSfixms = 15000;             //max number of mS to allow before no local fix flagged 
const uint8_t DisplayRate = 7;                   //when working OK the GPS will get a new fix every second or so
                                                 //this rate defines how often the display should be updated


//**************************************************************************************************
// 6) AFSK RTTY Settings - For PC upload into Dl-Fldigi in HAB mode. 
//    Sent at 300baud, 7 bit, no parity, 2 stop bits.
//    Shift 500hz, low tone 800hz, high tone 1300hz. 
//**************************************************************************************************

//#define UPLOADHABPACKET                          //comment in define to output HAB packet as AFSKRTTY for PC upload

const uint16_t AFSKRTTYperiod = 3333;            //period in uS for 1 bit at chosen baud rate, e.g. 10000 for 100baud, 3333 for 300baud
const uint16_t leadinmS = 500;                   //number of ms for AFSK constant lead in tone
const uint16_t tonehighHz = 1300;                //high tone in Hertz 
const uint16_t tonelowHz = 800;                  //low tone in Hertz   



