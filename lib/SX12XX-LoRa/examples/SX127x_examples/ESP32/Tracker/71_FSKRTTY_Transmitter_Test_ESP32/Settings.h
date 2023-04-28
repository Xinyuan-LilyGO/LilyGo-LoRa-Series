/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 23/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions needed. You will also need to connect up the pins for the SPI bus, which
//are SCK on pin 18, MISO on pin 19 and MOSI on pin 23.

#define NSS 5                                   //select pin on LoRa device
#define NRESET 27                               //reset pin on LoRa device
#define LED1 2                                  //on board LED, high for on
#define DIO0 35                                 //DIO0 pin on LoRa device, used for RX and TX done 

#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using

//*******  Setup Direct Modem Parameters Here ! ***************

const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const uint16_t deviation = 10000;               //deviation, total frequency shift low to high
const float adjustfreq = 0.9;                   //adjustment to tone frequency 

const int8_t TXpower = 10;                      //LoRa transmit power in dBm


//*******  Setup FSKRTTY Settings here ! ***************

uint32_t FrequencyShift = 400;                  //hertz frequency shift, approx, in 61.03515625hz steps   
uint8_t NumberofPips = 2;                       //number of marker pips to send
uint16_t PipDelaymS = 500;                      //mS between pips, carrier off 
uint16_t PipPeriodmS = 100;                     //mS length of pip
uint16_t BaudPerioduS = 10000;                  //uS period for baud, 10000uS for 100baud 
uint16_t LeadinmS = 1000;                       //ms of leadin, shifted carrier 
uint8_t DataBits = 7;                           //number of databits, normally 7 or 8
uint8_t StopBits = 2;                           //number of stopbits, normally 1 or 2 
uint8_t Parity = ParityNone;                    //parity on data bits, ParityNone, ParityOdd, ParityEven, ParityZero, ParityOne 



