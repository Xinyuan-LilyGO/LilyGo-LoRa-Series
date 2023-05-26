/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 17/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

// *******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini, 
//be sure to change the definitiosn to match your own setup. 

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define DIO0 3                                  //DIO0 on LoRa device, used for RX and TX done 
#define LED1 8                                  //On board LED, high for on

#define BATVREADON 8                            //when high turns on the resistor divider to measure voltage, -1 if not used
#define BATTERYAD A0                            //Resitor divider for battery connected here, -1 if not used
#define ADMultiplier 10.00                      //adjustment to convert AD value read into mV of battery voltage
#define DIODEMV 98                              //mV voltage drop accross diode @ low idle current


#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using


// *******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF12;      //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 10;                      //LoRa transmit power in dBm

const uint8_t TC74_ADDRESS = 0x4C;              //I2C bus address of TC74


// *******  Setup node addressing here ! ***************

const uint8_t TXPacketType = Sensor1;           //the packet type sent
const uint8_t TXDestination = 'B';              //the destination address of the receiver where the packet is being sent to 
const uint8_t TXSource = 2;                     //the source address, the address of this node, where the packet came from
 
