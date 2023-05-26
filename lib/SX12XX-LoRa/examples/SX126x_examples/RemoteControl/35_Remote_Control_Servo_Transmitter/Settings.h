/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


//*******  Setup hardware pin definitions here ! ***************

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitiosn to match your own setup. Some pins such as DIO2,
//DIO3, may not be in used by this sketch so they do not need to be connected and
//should be set to -1.

const int8_t NSS = 10;                          //select on LoRa device
const int8_t NRESET = 9;                        //reset on LoRa device
const int8_t RFBUSY = 7;                        //RF busy on LoRa device 
const int8_t DIO1 = 3;                          //DIO1 on LoRa device, used for RX and TX done
const int8_t DIO2 = -1;                         //DIO2 on LoRa device, normally not used so set to -1
const int8_t DIO3 = -1;                         //DIO3 on LoRa device, normally not used so set to -1
const int8_t LED1 = 8;                          //On board LED, logic high is on
const int8_t RX_EN = -1;                        //pin for RX enable, used on some SX126X devices, set to -1 if not used
const int8_t TX_EN = -1;                        //pin for TX enable, used on some SX126X devices, set to -1 if not used
const int8_t SW = -1;                           //SW pin on Dorji devices is used to turn RF switch on\off, set to -1 if not used    

#define LORA_DEVICE DEVICE_SX1262               //this is the device we are using

const int8_t joystickX1 = A2;                   //analog pin for the joystick 1 X pot
const int8_t joystickY1 = A3;                   //analog pin for the joystick 1 Y pot
const int8_t SWITCH1 = 2;                       //switch on joystick, set to -1 if not used
  
const uint32_t TXIdentity = 123 ;               //define a transmitter number, the receiver must use the same number
                                                //range is 0 to 255



//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting, normally set to auto

const uint8_t PacketLength = 5;                  //packet length is fixed 
const int8_t TXpower = 10;                       //LoRa transmit power in dBm

