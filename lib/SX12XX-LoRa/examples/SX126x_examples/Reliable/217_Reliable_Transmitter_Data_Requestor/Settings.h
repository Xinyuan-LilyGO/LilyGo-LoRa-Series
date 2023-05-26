/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 18/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

const uint8_t NSS = 10;                         //select pin on LoRa device
const uint8_t NRESET = 9;                       //reset pin on LoRa device
const uint8_t RFBUSY = 7;                       //busy pin on LoRa device
const uint8_t DIO1 = 3;                         //DIO1 pin on LoRa device, used for sensing RX and TX done
const uint8_t LORA_DEVICE = DEVICE_SX1262;      //we need to define the device we are using
const uint8_t LED1 = 8;

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting, normally set to auto
const int8_t TXpower = 10;                      //LoRa transmit power in dBm

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t RequestGPSLocation = 1;           //request type number for GPS location, so receiver knows what information to supply

const uint32_t ACKtimeout = 1000;               //Acknowledge timeout in mS, set to 0 if ACK not used.
const uint32_t TXtimeout = 1000;                //transmit timeout in mS. If 0 return from transmit function after send.
