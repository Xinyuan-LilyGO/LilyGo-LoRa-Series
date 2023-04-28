/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

#define NSS 12                //select on LoRa device
#define NRESET 14             //reset pin on LoRa device
#define RFBUSY 15             //busy pin on LoRa device 
#define SCK 4                 //SCK on SPI3
#define MISO 13               //MISO on SPI3 
#define MOSI 2                //MOSI on SPI3
#define REDLED 33             //pin number for ESP32CAM on board red LED, set logic level low for on

#define RXD2 40               //RX pin for monitor port, not used 
#define TXD2 33               //TX pin for monitor port

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using

#define YModemSerial Serial
#define Monitorport Serial2

const uint32_t Frequency = 2445000000;          //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes
const int8_t  TXpower = 10;                     //LoRa transmit power

//*******  Setup LoRa modem parameters here ! ***************
const uint8_t Bandwidth = LORA_BW_1600;         //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate

//*******  Setup FLRC modem parameters here ! ***************
const uint8_t BandwidthBitRate = FLRC_BR_1_300_BW_1_2;     //FLRC bandwidth and bit rate, 1.3Mbs
//const uint8_t BandwidthBitRate = FLRC_BR_0_260_BW_0_3;   //FLRC 260kbps
const uint8_t CodingRate = FLRC_CR_1_0;                    //FLRC coding rate
const uint8_t BT = RADIO_MOD_SHAPING_BT_1_0;               //FLRC BT
const uint32_t Syncword = 0x01234567;                      //FLRC uses syncword

const uint32_t TXtimeoutmS = 5000;              //mS to wait for TX to complete
const uint32_t RXtimeoutmS = 3000000;           //mS to wait for receiving a packet
const uint32_t ACKdelaymS = 0;                  //ms delay after general packet actioned and ack sent
const uint32_t ACKdelaystartendmS = 25;         //ms delay before ack sent at array start wrie and end write
const uint32_t ACKsegtimeoutmS = 75;            //mS to wait for receiving an ACK before re-trying transmit segment
const uint32_t ACKopentimeoutmS = 250;          //mS to wait for receiving an ACK before re-trying transmit file open
const uint32_t ACKclosetimeoutmS = 250;         //mS to wait for receiving an ACK before re-trying transmit file close
const uint32_t DuplicatedelaymS = 25;           //ms delay if there has been an duplicate segment or command receipt
const uint32_t NoAckCountLimit = 250;           //if no NoAckCount exceeds this value - restart transfer

const uint32_t FunctionDelaymS = 0;             //delay between functions such as open file, send segments etc
const uint32_t PacketDelaymS = 1000;            //mS delay between transmitted packets such as DTInfo etc

const uint32_t ReceiveTimeoutmS = 60000;        //mS waiting for array transfer before timeout
const uint8_t HeaderSizeMax = 12;               //max size of header in bytes, minimum size is 6 bytes
const uint8_t DataSizeMax = 245;                //max size of data array in bytes
const uint8_t ARDTfilenamesize = 32;            //size of DTfilename buffer used by array transfer functions
const uint32_t ARDTarraysize = 0x20000;         //maximum file\array size to receive
const uint16_t NetworkID = 0x3210;              //a unique identifier to go out with packet

const uint8_t StartAttempts = 2;                //number of attempts to start transfer before a fail
const uint8_t SendAttempts = 5;                 //number of attempts carrying out a process before a restart

#ifdef USELORA
const uint8_t SegmentSize = 245;                //number of bytes in each segment, 245 is maximum value for LoRa
#endif

#ifdef USEFLRC
const uint8_t SegmentSize = 117;                //number of bytes in each segment, 117 is maximum value for FLRC
#endif
