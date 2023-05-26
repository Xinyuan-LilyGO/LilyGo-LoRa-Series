/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 12/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/
#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //RFBUSY pin on LoRa device
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //LED used to indicate transmission

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using


//*******  Setup LoRa Test Parameters Here ! ***************

const uint32_t Frequency = 2445000000;                     //frequency of transmissions
const uint32_t Offset = 0;                                 //offset frequency for calibration purposes
const int8_t  TXpower = 10;                                //LoRa transmit power

//*******  Setup LoRa modem parameters here ! ***************
const uint8_t Bandwidth = LORA_BW_1600;                    //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;                  //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;                      //LoRa coding rate

//*******  Setup FLRC modem parameters here ! ***************
const uint8_t BandwidthBitRate = FLRC_BR_1_300_BW_1_2;     //FLRC bandwidth and bit rate, 1.3Mbs
//const uint8_t BandwidthBitRate = FLRC_BR_0_260_BW_0_3;   //FLRC 260kbps
const uint8_t CodingRate = FLRC_CR_1_0;                    //FLRC coding rate
const uint8_t BT = RADIO_MOD_SHAPING_BT_1_0;               //FLRC BT
const uint32_t Syncword = 0x01234567;                      //FLRC uses syncword


const uint32_t TXtimeoutmS = 5000;              //mS to wait for TX to complete
const uint32_t RXtimeoutmS = 60000;             //mS to wait for receiving a packet
const uint32_t ACKdelaymS = 0;                  //ms delay after packet actioned and ack sent
const uint32_t ACKsegtimeoutmS = 75;            //mS to wait for receiving an ACK before re-trying transmit segment
const uint32_t ACKopentimeoutmS = 250;          //mS to wait for receiving an ACK before re-trying transmit file open
const uint32_t ACKclosetimeoutmS = 250;         //mS to wait for receiving an ACK before re-trying transmit file close
const uint32_t NoAckCountLimit = 250;           //if no NoAckCount exceeds this value - restart transfer
const uint32_t DuplicatedelaymS = 10;           //ms delay if there has been an duplicate segment or command receipt
const uint32_t packetdelaymS = 0;               //mS delay between transmitted packets

const uint8_t HeaderSizeMax = 12;               //max size of header in bytes, minimum size is 7 bytes
const uint8_t DataSizeMax = 245;                //max size of data array in bytes
const uint8_t DTfilenamesize = 32;              //size of DTfilename buffer

const uint16_t NetworkID = 0x3210;              //a unique identifier to go out with packet
const uint8_t DTSendAttempts = 10;              //number of attempts sending a packet before a restart

const uint32_t DTFileSize = 65535;              //size of file to simulate

#ifdef USELORA
  const uint8_t DTSegmentSize = 245;            //number of bytes in each segment 245 is maximum value for LoRa
#endif

#ifdef USEFLRC
  const uint8_t DTSegmentSize = 117;            //number of bytes in each segment 117 is maximum value for FLRC
#endif
