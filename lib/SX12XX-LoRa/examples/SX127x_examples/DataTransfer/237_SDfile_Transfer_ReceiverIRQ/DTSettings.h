/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/02/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define LED1 8                                  //LED used to indicate transmission
#define SDCS 30

#define Monitorport Serial                      //Port where serial prints go

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

//*******  Setup LoRa modem parameters here ! ***************
const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_500;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting, normally set to auto

const int8_t TXpower = 10;                      //LoRa transmit power in dBm

const uint32_t TXtimeoutmS = 5000;              //mS to wait for TX to complete
const uint32_t RXtimeoutmS = 60000;             //mS to wait for receiving a packet
const uint32_t ACKdelaymS = 10;                 //ms delay after packet actioned and ack sent
const uint32_t ACKsegtimeoutmS = 75;            //mS to wait for receiving an ACK before re-trying transmit segment
const uint32_t ACKopentimeoutmS = 250;          //mS to wait for receiving an ACK before re-trying transmit file open
const uint32_t ACKclosetimeoutmS = 250;         //mS to wait for receiving an ACK before re-trying transmit file close
const uint32_t DuplicatedelaymS = 10;           //ms delay if there has been an duplicate segment or command receipt
const uint32_t NoAckCountLimit = 250;           //if no NoAckCount exceeds this value - restart transfer
const uint32_t FunctionDelaymS = 0;             //delay between functions such as open file, send segments etc
const uint32_t PacketDelaymS = 1000;            //mS delay between transmitted packets such as DTInfo etc

const uint8_t HeaderSizeMax = 12;               //max size of header in bytes, minimum size is 7 bytes
const uint8_t DataSizeMax = 245;                //max size of data array in bytes
const uint8_t Maxfilenamesize = 32;              //size of DTfilename buffer

const uint16_t NetworkID = 0x3210;              //a unique identifier to go out with packet
const uint8_t SendAttempts = 10;                //number of attempts sending a packet or attempting a process before a restart of transfer
const uint8_t StartAttempts = 10;               //number of attempts sending the file

const uint8_t SegmentSize = 245;                //number of bytes in each segment, 245 is maximum value for LoRa
