/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/04/22

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
#define DIO0 3                                  //DIO0 on LoRa device, used for RX and TX done 
#define LED1 8                                  //On board LED, high for on
#define SWITCH1 2                               //if pin shorted to ground, switch is active

#define RXpin A3                                //pin number for GPS RX input into Arduino - TX from GPS
#define TXpin A2                                //pin number for GPS TX output from Arduino- RX into GPS

#define GPSPOWER -1                             //Pin that controls power to GPS, set to -1 if not used
#define GPSONSTATE HIGH                         //logic level to turn GPS on via pin GPSPOWER 
#define GPSOFFSTATE LOW                         //logic level to turn GPS off via pin GPSPOWER

#define AUDIOOUT 6                              //pin used to output Audio tones for HAB packet upload 
#define CHECK 8                                 //this pin is toggled inside the AFSKRTTY library, high for logic 1, low for logic 0, so it can be used to check the timing.

#define LORA_DEVICE DEVICE_SX1278               //this is the LoRa device we are using

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

#define USESOFTSERIALGPS                          //if your using software serial for the GPS, enable this define     
//#define USEHARDWARESERIALGPS                    //if your using hardware serial for the GPS, enable this define
#define HARDWARESERIALPORT Serial1                //if using hardware serial enable this define for hardware serial port 

const uint16_t WaitGPSFixSeconds = 30;           //time to wait for a new GPS fix

const uint16_t NoRXGPSfixms = 15000;             //max number of mS to allow before no local fix flagged
const uint8_t DisplayRate = 7;                   //when working OK the GPS will get a new fix every second or so
//this rate defines how often the display should be updated


//**************************************************************************************************
// 6) AFSK RTTY Settings - For PC upload into Dl-Fldigi in HAB mode.
//    Sent at 200baud, 7 bit, no parity, 2 stop bits.
//    Shift circa 360hz, low tone 1000hz, high tone 1400hz (measured on an Arduino Due)
//    See screenshot 'AFSKRTTY2_DL-Fldigi_Settings.jpg' in this folder for the settings used
//
//**************************************************************************************************

#define UPLOADHABPACKET              //comment in define to output HAB packet as AFSKRTTY for PC upload

const uint16_t leadinmS = 2000;      //number of ms for AFSK constant lead in tone
const uint16_t leadoutmS = 0;        //number of ms for AFSK constant lead out tone


const uint16_t LOWPERIODUS = 1000;   //actual period in uS of to give a 200baud
const uint8_t LOWCYCLES = 5;         //cycles of low frequency tone for 200baud
const uint16_t HIGHPERIODUS = 714;   //actual high period in uS to give a 200baud
const uint8_t HIGHCYCLES = 7;        //cycles of high frequency tone for 200baud
const int8_t ADJUSTUS = 0;           //uS to subtract from tone generation loop to match frequency
