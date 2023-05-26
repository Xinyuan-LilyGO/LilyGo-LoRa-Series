/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 18/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

const uint8_t NSS = 10;                         //select pin on LoRa device
const uint8_t NRESET = 9;                       //reset pin on LoRa device
const uint8_t DIO0 = 3;                         //DIO1 pin on LoRa device, used for sensing RX and TX done
const uint8_t LED1 = 8;
const uint8_t LORA_DEVICE = DEVICE_SX1278;      //we need to define the device we are using

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting, normally set to auto
const int8_t  TXpower = 2;                      //LoRa transmit power

const uint32_t ACKdelay = 200;                  //delay in mS before sending reply                      
const uint32_t RXtimeout = 10000;                //receive timeout in mS.

const uint8_t RequestGPSLocation = 1;           //request type for GPS location

//GPS co-ordinates to use for the GPS location request
const float TestLatitude  = 51.48230;           //GPS co-ordinates to use for test
const float TestLongitude  = -3.18136;          //Cardiff castle keep, used for testing purposes
const float TestAltitude = 25.5;
const uint8_t TrackerStatus = 1;                //set status bit to represent tracker GPS has fix

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t ThisStation = 123;                //the number of this station for requests and ranging
