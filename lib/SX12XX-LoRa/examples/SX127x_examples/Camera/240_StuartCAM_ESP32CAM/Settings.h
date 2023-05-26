/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//updated pinouts for 'ESP32CAM_Long_Range_Wireless_Adapter PCB dated 271121
//Note transistor driving the White LED on pin 4, or the LED needs to be removed

#define NSS 12                //select on LoRa device
#define NRESET 15             //reset pin on LoRa device
#define SCK 4                 //SCK on SPI3
#define MISO 13               //MISO on SPI3 
#define MOSI 2                //MOSI on SPI3
#define REDLED 33             //pin number for ESP32CAM on board red LED, set logic level low for on

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
const uint32_t ACKdelaymS = 0;                  //ms delay after packet actioned and ack sent
const uint32_t ACKdelaystartendmS = 25;         //ms delay before ack sent at array start wrie and end write
const uint32_t ACKsegtimeoutmS = 75;            //mS to wait for receiving an ACK before re-trying transmit segment
const uint32_t ACKopentimeoutmS = 500;          //mS to wait for receiving an ACK before re-trying transmit file open
const uint32_t ACKclosetimeoutmS = 500;         //mS to wait for receiving an ACK before re-trying transmit file close
const uint32_t DuplicatedelaymS = 25;           //ms delay if there has been an duplicate segment or command receipt
const uint32_t NoAckCountLimit = 250;           //if no NoAckCount exceeds this value - restart transfer

const uint32_t FunctionDelaymS = 0;             //delay between functions such as open file, send segments etc
const uint32_t PacketDelaymS = 1000;            //mS delay between transmitted packets such as DTInfo etc

const uint8_t DTSegmentSize = 245;              //number of bytes in each segment or payload
const uint8_t ARDTfilenamesize = 32;            //size of filename buffer

const uint8_t StartAttempts = 2;                //number of attempts to start transfer before a fail
const uint8_t SendAttempts = 5;                 //number of attempts carrying out a process before a restart

const uint8_t HeaderSizeMax = 12;               //max size of header in bytes, minimum size is 7 bytes
const uint8_t DataSizeMax = 245;                //max size of data array in bytes
const uint16_t NetworkID = 0x3210;              //a unique identifier to go out with packet

const uint16_t SleepTimesecs = 15;              //sleep time in seconds after each TX loop
const uint32_t uS_TO_S_FACTOR = 1000000;        //Conversion factor for micro seconds to seconds
const uint8_t PicturesToTake = 1;               //number of pictures to take at each wakeup, only last is sent via LoRa
const uint32_t PictureDelaymS = 1000;           //delay in mS between pictures
const uint8_t SegmentSize = 245;                //number of bytes in each segment, 245 is maximum value for LoRa


// Pin definition for CAMERA_MODEL_AI_THINKER
// Change pin definition if you're using another ESP32 with camera module
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
