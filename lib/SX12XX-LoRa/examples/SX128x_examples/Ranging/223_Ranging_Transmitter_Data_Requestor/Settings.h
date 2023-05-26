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
const uint8_t LORA_DEVICE = DEVICE_SX1280;      //we need to define the device we are using
const uint8_t LED1 = 8;

//*******  Setup LoRa modem parameters here ! ***************
const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const int8_t  TXpower = 2;                      //LoRa transmit power
const uint8_t Bandwidth = LORA_BW_0800;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate


const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in transmitter
const uint8_t RequestGPSLocation = 1;           //request type number for GPS location, so receiver knows what information to supply
const uint8_t RequestRanging = 2;               //request type number for ranging

const uint32_t ACKtimeout = 1000;               //Acknowledge timeout in mS, set to 0 if ACK not used.
const uint32_t TXtimeout = 1000;                //transmit timeout in mS. If 0 return from transmit function after send.

const int8_t RangingTXPower = 10;               //Ranging transmit power used
const uint16_t  RangingTimeoutmS = 1000;        //ranging master timeout in mS, time master waits for a reply
const uint16_t  RangingUpTimemS = 2000;         //time for slave to stay in ranging listen
const uint16_t  PacketDelaymS = 0;              //forced extra delay in mS between sending packets
const uint16_t  RangingCount = 3;               //number of times ranging is carried out for each distance measurment
const float DistanceAdjustment = 1.0000;        //adjustment factor to calculated distance
const uint16_t Calibration = 11350;             //Manual Ranging calibration value
