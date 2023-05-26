/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 18/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

const uint8_t NSS = 10;                         //select pin on LoRa device
const uint8_t NRESET = 9;                       //reset pin on LoRa device
const uint8_t RFBUSY = 7;                       //busy pin on LoRa device
const uint8_t LED1 = 8;
const uint8_t LORA_DEVICE = DEVICE_SX1280;      //we need to define the device we are using

//*******  Setup LoRa modem parameters here ! ***************
const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const int8_t  TXpower = 10;                      //LoRa transmit power
const int8_t RangingTXPower = 10;               //Transmit power used for ranging
const uint8_t Bandwidth = LORA_BW_1600;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint32_t ACKdelay = 200;                  //delay in mS before sending reply                      
const uint32_t RXtimeout = 5000;                //receive timeout in mS.

const uint8_t RequestGPSLocation = 1;           //request type for GPS location

//GPS co-ordinates to use for the GPS location request
const float TestLatitude  = 51.48230;           //GPS co-ordinates to use for test
const float TestLongitude  = -3.18136;          //Cardiff castle keep, used for testing purposes
const float TestAltitude = 25.5;
const uint8_t TrackerStatus = 1;                //set status bit to represent tracker GPS has fix

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t ThisStation = 123;                //the number of this station for requests and ranging
