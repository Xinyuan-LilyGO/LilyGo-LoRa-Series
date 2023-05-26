//SX1280LT Includes


//*************************************************************
// LoRa Modem Settings
//*************************************************************
//
//LoRa spreading factors
#define LORA_SF5      0x50
#define LORA_SF6      0x60
#define LORA_SF7      0x70
#define LORA_SF8      0x80
#define LORA_SF9      0x90
#define LORA_SF10     0xA0
#define LORA_SF11     0xB0
#define LORA_SF12     0xC0

//LoRa bandwidths
#define LORA_BW_0200  0x34               //actually 203125hz
#define LORA_BW_0400  0x26               //actually 406250hz
#define LORA_BW_0800  0x18               //actually 812500hz
#define LORA_BW_1600  0x0A               //actually 1625000hz

//LoRa coding rates
#define LORA_CR_4_5   0x01
#define LORA_CR_4_6   0x02
#define LORA_CR_4_7   0x03
#define LORA_CR_4_8   0x04

//March 2020  A new interleaving scheme has been implemented to increase robustness to
//burst interference and/or strong Doppler events.
//Note: There is a limitation on maximum payload length for LORA_CR_LI_4_8. Payload length should
//not exceed 253 bytes if CRC is enabled.
#define LORA_CR_LI_4_5   0x05
#define LORA_CR_LI_4_6   0x06
#define LORA_CR_LI_4_8   0x07

//LoRa CAD settings
#define LORA_CAD_01_SYMBOL                0x00
#define LORA_CAD_02_SYMBOL                0x20
#define LORA_CAD_04_SYMBOL                0x40
#define LORA_CAD_08_SYMBOL                0x60
#define LORA_CAD_16_SYMBOL                0x80

//LoRa Header Types
#define LORA_PACKET_VARIABLE_LENGTH       0x00
#define LORA_PACKET_FIXED_LENGTH          0x80
#define LORA_PACKET_EXPLICIT              LORA_PACKET_VARIABLE_LENGTH
#define LORA_PACKET_IMPLICIT              LORA_PACKET_FIXED_LENGTH

//LoRa packet CRC settings
#define LORA_CRC_ON                       0x20
#define LORA_CRC_OFF                      0x00

//LoRa IQ Setings
#define LORA_IQ_NORMAL                    0x40
#define LORA_IQ_INVERTED                  0x00


#define FREQ_STEP                         198.364
#define FREQ_ERROR_CORRECTION             1.55




//*************************************************************
// SX1280 Interrupt flags
//*************************************************************

#define    IRQ_RADIO_NONE                       0x0000
#define    IRQ_TX_DONE                          0x0001
#define    IRQ_RX_DONE                          0x0002
#define    IRQ_SYNCWORD_VALID                   0x0004
#define    IRQ_SYNCWORD_ERROR                   0x0008
#define    IRQ_HEADER_VALID                     0x0010
#define    IRQ_HEADER_ERROR                     0x0020
#define    IRQ_CRC_ERROR                        0x0040
#define    IRQ_RANGING_SLAVE_RESPONSE_DONE      0x0080

#define    IRQ_RANGING_SLAVE_REQUEST_DISCARDED  0x0100
#define    IRQ_RANGING_MASTER_RESULT_VALID      0x0200
#define    IRQ_RANGING_MASTER_RESULT_TIMEOUT    0x0400
#define    IRQ_RANGING_SLAVE_REQUEST_VALID      0x0800
#define    IRQ_CAD_DONE                         0x1000
#define    IRQ_CAD_ACTIVITY_DETECTED            0x2000
#define    IRQ_RX_TX_TIMEOUT                    0x4000
#define    IRQ_TX_TIMEOUT                       0x4000
#define    IRQ_RX_TIMEOUT                       0x4000
#define    IRQ_PREAMBLE_DETECTED                0x8000
#define    IRQ_RADIO_ALL                        0xFFFF


//*************************************************************
// SX1280 Commands
//*************************************************************

#define RADIO_GET_PACKETTYPE              0x03
#define RADIO_GET_IRQSTATUS               0x15
#define RADIO_GET_RXBUFFERSTATUS          0x17
#define RADIO_WRITE_REGISTER              0x18
#define RADIO_READ_REGISTER               0x19
#define RADIO_WRITE_BUFFER                0x1A
#define RADIO_READ_BUFFER                 0x1B
#define RADIO_GET_PACKETSTATUS            0x1D
#define RADIO_GET_RSSIINST                0x1F
#define RADIO_SET_STANDBY                 0x80
#define RADIO_SET_RX                      0x82
#define RADIO_SET_TX                      0x83
#define RADIO_SET_SLEEP                   0x84
#define RADIO_SET_RFFREQUENCY             0x86
#define RADIO_SET_CADPARAMS               0x88
#define RADIO_CALIBRATE                   0x89
#define RADIO_SET_PACKETTYPE              0x8A
#define RADIO_SET_MODULATIONPARAMS        0x8B
#define RADIO_SET_PACKETPARAMS            0x8C
#define RADIO_SET_DIOIRQPARAMS            0x8D
#define RADIO_SET_TXPARAMS                0x8E
#define RADIO_SET_BUFFERBASEADDRESS       0x8F
#define RADIO_SET_RXDUTYCYCLE             0x94
#define RADIO_SET_REGULATORMODE           0x96
#define RADIO_CLR_IRQSTATUS               0x97
#define RADIO_SET_AUTOTX                  0x98
#define RADIO_SET_LONGPREAMBLE            0x9B
#define RADIO_SET_UARTSPEED               0x9D
#define RADIO_SET_AUTOFS                  0x9E
#define RADIO_SET_RANGING_ROLE            0xA3
#define RADIO_GET_STATUS                  0xC0
#define RADIO_SET_FS                      0xC1
#define RADIO_SET_CAD                     0xC5
#define RADIO_SET_TXCONTINUOUSWAVE        0xD1
#define RADIO_SET_TXCONTINUOUSPREAMBLE    0xD2
#define RADIO_SET_SAVECONTEXT             0xD5


//*************************************************************
// SX1280 Registers
//*************************************************************

#define REG_LNA_REGIME                    0x0891
#define REG_LR_PAYLOADLENGTH              0x901
#define REG_LR_PACKETPARAMS               0x903

#define REG_RFFrequency23_16              0x906
#define REG_RFFrequency15_8               0x907
#define REG_RFFrequency7_0                0x908

#define REG_FLRC_RFFrequency23_16         0x9A3          //found by experiment
#define REG_FLRC_RFFrequency15_8          0x9A4
#define REG_FLRC_RFFrequency7_0           0x9A5

#define REG_RANGING_FILTER_WINDOW_SIZE    0x091E
#define REG_LR_DEVICERANGINGADDR          0x0916
#define REG_LR_DEVICERANGINGADDR          0x0916
#define REG_LR_RANGINGRESULTCONFIG        0x0924
#define REG_LR_RANGINGRERXTXDELAYCAL      0x092C
#define REG_LR_RANGINGIDCHECKLENGTH       0x0931
#define REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB 0x954
#define REG_LR_ESTIMATED_FREQUENCY_ERROR_MID 0x955
#define REG_LR_ESTIMATED_FREQUENCY_ERROR_LSB 0x956
#define REG_LR_RANGINGRESULTBASEADDR      0x0961
#define REG_RANGING_RSSI                  0x0964
#define REG_LR_FLRCPAYLOADLENGTH          0x09C3         //found by experiment
#define REG_LR_SYNCWORDTOLERANCE          0x09CD
#define REG_LR_SYNCWORDBASEADDRESS1       0x09CE
#define REG_FLRCSYNCWORD1_BASEADDR        0x09CF
#define REG_LR_SYNCWORDBASEADDRESS2       0x09D3
#define REG_FLRCSYNCWORD2_BASEADDR        0x09D4
#define REG_LR_SYNCWORDBASEADDRESS3       0x09D8
#define REG_FLRCSYNCWORD3_BASEADDR        0x09D9

#define REG_RANGING_RSSI                  0x0964

#define REG_LR_ESTIMATED_FREQUENCY_ERROR_MASK       0x0FFFFF

//SX1280 Packet Types
#define PACKET_TYPE_GFSK                  0x00
#define PACKET_TYPE_LORA                  0x01
#define PACKET_TYPE_RANGING               0x02
#define PACKET_TYPE_FLRC                  0x03
#define PACKET_TYPE_BLE                   0x04

//SX1280 Standby modes
#define MODE_STDBY_RC                     0x00
#define MODE_STDBY_XOSC                   0x01

//TX and RX timeout based periods
#define PERIODBASE_15_US                   0x00
#define PERIODBASE_62_US                   0x01
#define PERIODBASE_01_MS                   0x02
#define PERIODBASE_04_MS                   0x03

//TX ramp periods
#define RADIO_RAMP_02_US                  0x00
#define RADIO_RAMP_04_US                  0x20
#define RADIO_RAMP_06_US                  0x40
#define RADIO_RAMP_08_US                  0x60
#define RADIO_RAMP_10_US                  0x80
#define RADIO_RAMP_12_US                  0xA0
#define RADIO_RAMP_16_US                  0xC0
#define RADIO_RAMP_20_US                  0xE0

//SX1280 Power settings
#define USE_LDO   0x00
#define USE_DCDC  0x01


//*************************************************************
//SX1280 Ranging settings
//*************************************************************

#define RANGING_IDCHECK_LENGTH_08_BITS  0x00
#define RANGING_IDCHECK_LENGTH_16_BITS  0x01
#define RANGING_IDCHECK_LENGTH_24_BITS  0x02
#define RANGING_IDCHECK_LENGTH_32_BITS  0x03

#define RANGING_RESULT_RAW              0x00
#define RANGING_RESULT_AVERAGED         0x01
#define RANGING_RESULT_DEBIASED         0x02
#define RANGING_RESULT_FILTERED         0x03


#define MASK_RANGINGMUXSEL              0xCF

#define RANGING_SLAVE                   0x00
#define RANGING_MASTER                  0x01


//*************************************************************
//GFSK  modem settings
//*************************************************************

#define GFS_BLE_BR_2_000_BW_2_4  0x04
#define GFS_BLE_BR_1_600_BW_2_4  0x28
#define GFS_BLE_BR_1_000_BW_2_4  0x4C
#define GFS_BLE_BR_1_000_BW_1_2  0x45
#define GFS_BLE_BR_0_800_BW_2_4  0x70
#define GFS_BLE_BR_0_800_BW_1_2  0x69
#define GFS_BLE_BR_0_500_BW_1_2  0x8D
#define GFS_BLE_BR_0_500_BW_0_6  0x86
#define GFS_BLE_BR_0_400_BW_1_2  0xB1
#define GFS_BLE_BR_0_400_BW_0_6  0xAA
#define GFS_BLE_BR_0_250_BW_0_6  0xCE
#define GFS_BLE_BR_0_250_BW_0_3  0xC7
#define GFS_BLE_BR_0_125_BW_0_3  0xEF

#define    GFS_BLE_MOD_IND_0_35  0
#define    GFS_BLE_MOD_IND_0_50  1
#define    GFS_BLE_MOD_IND_0_75  2
#define    GFS_BLE_MOD_IND_1_00  3
#define    GFS_BLE_MOD_IND_1_25  4
#define    GFS_BLE_MOD_IND_1_50  5
#define    GFS_BLE_MOD_IND_1_75  6
#define    GFS_BLE_MOD_IND_2_00  7
#define    GFS_BLE_MOD_IND_2_25  8
#define    GFS_BLE_MOD_IND_2_50  9
#define    GFS_BLE_MOD_IND_2_75  10
#define    GFS_BLE_MOD_IND_3_00  11
#define    GFS_BLE_MOD_IND_3_25  12
#define    GFS_BLE_MOD_IND_3_50  13
#define    GFS_BLE_MOD_IND_3_75  14
#define    GFS_BLE_MOD_IND_4_00  15

#define    PREAMBLE_LENGTH_04_BITS        0x00      //4 bits
#define    PREAMBLE_LENGTH_08_BITS        0x10      //8 bits
#define    PREAMBLE_LENGTH_12_BITS        0x20      //12 bits
#define    PREAMBLE_LENGTH_16_BITS        0x30      //16 bits
#define    PREAMBLE_LENGTH_20_BITS        0x40      //20 bits
#define    PREAMBLE_LENGTH_24_BITS        0x50      //24 bits
#define    PREAMBLE_LENGTH_28_BITS        0x60      //28 bits
#define    PREAMBLE_LENGTH_32_BITS        0x70      //32 bits

#define    GFS_SYNCWORD_LENGTH_1_BYTE     0x00      //Sync word length 1 byte
#define    GFS_SYNCWORD_LENGTH_2_BYTE     0x02      //Sync word length 2 bytes
#define    GFS_SYNCWORD_LENGTH_3_BYTE     0x04      //Sync word length 3 bytes
#define    GFS_SYNCWORD_LENGTH_4_BYTE     0x06      //Sync word length 4 bytes
#define    GFS_SYNCWORD_LENGTH_5_BYTE     0x08      //Sync word length 5 bytes  

#define    RADIO_RX_MATCH_SYNCWORD_OFF    0x00      //no search for SyncWord
#define    RADIO_RX_MATCH_SYNCWORD_1      0x10
#define    RADIO_RX_MATCH_SYNCWORD_2      0x20
#define    RADIO_RX_MATCH_SYNCWORD_1_2    0x30
#define    RADIO_RX_MATCH_SYNCWORD_3      0x40
#define    RADIO_RX_MATCH_SYNCWORD_1_3    0x50
#define    RADIO_RX_MATCH_SYNCWORD_2_3    0x60
#define    RADIO_RX_MATCH_SYNCWORD_1_2_3  0x70

#define    RADIO_PACKET_FIXED_LENGTH      0x00      //The packet is fixed length, klnown on both RX and TX, no header
#define    RADIO_PACKET_VARIABLE_LENGTH   0x20      //The packet is variable size, header included

#define RADIO_CRC_OFF      0x00
#define RADIO_CRC_1_BYTES  0x10
#define RADIO_CRC_2_BYTES  0x20
#define RADIO_CRC_3_BYTES  0x30

#define   RADIO_WHITENING_ON   0x00
#define   RADIO_WHITENING_OFF  0x08

//End GFSK ****************************************************



//*************************************************************
//FLRC  modem settings
//*************************************************************

#define FLRC_SYNC_NOSYNC         0x00
#define FLRC_SYNC_WORD_LEN_P32S  0x04

#define RX_DISABLE_SYNC_WORD       0x00
#define RX_MATCH_SYNC_WORD_1       0x10
#define RX_MATCH_SYNC_WORD_2       0x20
#define RX_MATCH_SYNC_WORD_1_2     0x30
#define RX_MATCH_SYNC_WORD_3       0x40
#define RX_MATCH_SYNC_WORD_1_3     0x50
#define RX_MATCH_SYNC_WORD_2_3     0x60
#define RX_MATCH_SYNC_WORD_1_2_3   0x70

#define FLRC_BR_1_300_BW_1_2 0x45   //1.3Mbs  
#define FLRC_BR_1_000_BW_1_2 0x69   //1.04Mbs 
#define FLRC_BR_0_650_BW_0_6 0x86   //0.65Mbs
#define FLRC_BR_0_520_BW_0_6 0xAA   //0.52Mbs
#define FLRC_BR_0_325_BW_0_3 0xC7   //0.325Mbs
#define FLRC_BR_0_260_BW_0_3 0xEB   //0.26Mbs

#define FLRC_CR_1_2  0x00           //coding rate 1:2
#define FLRC_CR_3_4  0x02           //coding rate 3:4
#define FLRC_CR_1_0  0x04           //coding rate 1 

#define BT_DIS  0x00                //No filtering
#define BT_1    0x10                //1
#define BT_0_5  0x20                //0.5

#define RADIO_MOD_SHAPING_BT_OFF  0x00
#define RADIO_MOD_SHAPING_BT_1_0  0x10
#define RADIO_MOD_SHAPING_BT_0_5  0x20


//Table 13-45: PacketStatus2 in FLRC Packet
#define PacketCtrlBusy  0x01
#define PacketReceived  0x02
#define HeaderReceived  0x04
#define AbortError      0x08
#define CrcError        0x10
#define LengthError     0x20
#define SyncError       0x40
#define Reserved        0x80


//Table 13-46: PacketStatus3 in FLRC Packet
#define PktSent    0x01
#define rxpiderr   0x08
#define rx_no_ack  0x10

//FLRC default packetparamns
#define FLRC_Default_AGCPreambleLength  PREAMBLE_LENGTH_32_BITS       //packetParam1
#define FLRC_Default_SyncWordLength     FLRC_SYNC_WORD_LEN_P32S       //packetParam2
#define FLRC_Default_SyncWordMatch      RADIO_RX_MATCH_SYNCWORD_1     //packetParam3
#define FLRC_Default_PacketType         RADIO_PACKET_VARIABLE_LENGTH  //packetParam4
#define FLRC_Default_PayloadLength      BUFFER_SIZE_DEFAULT           //packetParam5
#define FLRC_Default_CrcLength          RADIO_CRC_3_BYTES             //packetParam6
#define FLRC_Default_Whitening          RADIO_WHITENING_OFF           //packetParam7


//Table 11-15 Sleep modes
#define RETAIN_INSTRUCTION_RAM   0x04
#define RETAIN_DATABUFFER        0x02
#define RETAIN_DATA_RAM          0x01
#define CONFIGURATION_RETENTION  0x01         //included for libray compatibility
#define RETAIN_None              0x00


#ifndef RAMP_TIME
#define RAMP_TIME RADIO_RAMP_02_US
#endif

#ifndef PERIODBASE
#define PERIODBASE PERIOBASE_01_MS
#endif

#ifndef PERIODBASE_COUNT_15_8
#define PERIODBASE_COUNT_15_8 0
#endif

#ifndef PERIODBASE_COUNT_7_0
#define PERIODBASE_COUNT_7_0 0
#endif


#define DEVICE_SX1280  0x20
#define DEVICE_SX1281  0x21


//SPI settings
#define LTspeedMaximum  8000000
#define LTdataOrder     MSBFIRST
#define LTdataMode      SPI_MODE0

#define    RANGING_VALID                            0x03
#define    RANGING_TIMEOUT                          0x02
#define    WAIT_RX                                  0x01
#define    WAIT_TX                                  0x01
#define    NO_WAIT                                  0x00

#define CalibrationSF10BW400                        10180     //calibration value for ranging, SF10, BW400
#define CalibrationSF5BW1600                        13100     //calibration value for ranging, SF5, BW1600


const uint16_t RNG_CALIB_0400[] = { 10260,  10244,  10228,  10212,  10196,  10180  };   //SF5 to SF10
const uint16_t RNG_CALIB_0800[] = { 11380,  11370,  11360,  11350,  11340,  11330  };
const uint16_t RNG_CALIB_1600[] = { 13100,  13160,  13220,  13280,  13340,  13400  };


//These are the bit numbers which when set indicate reliable errors, variable _ReliableErrors
#define ReliableCRCError 0x00             //bit number set in _ReliableErrors when there is a reliable CRC missmatch
#define ReliableIDError  0x01             //bit number set in _ReliableErrors when there is a NetworkID missmatch
#define ReliableSizeError 0x02            //bit number set in _ReliableErrors when there is a size error for packet
#define ReliableACKError 0x03             //bit number set in _ReliableErrors when there is a ACK error
#define ReliableTimeout 0x04              //bit number set in _ReliableErrors when there is a timeout error
#define SegmentSequenceError 0x05         //bit number set in _ReliableErrors when there is a segment sequence error
#define FileError 0x06                    //bit number set in _ReliableErrors when there ia a file (SD) error

//These are the bit numbers which when set indicate reliable status flags, variable _ReliableFlags
#define ReliableACKSent 0x00              //bit number set in _ReliableFlags when there is a ACK sent
#define ReliableACKReceived 0x01          //bit number set in _ReliableFlags when there is a ACK received

//These are the bit numbers which when set indicate reliable configuration, variable _ReliableConfig
#define NoReliableCRC 0x00                //bit number set in _ReliableConfig when reliable CRC is not used
#define NoAutoACK 0x01                    //bit number set in _ReliableConfig when ACK is not used 


//number of bits to use to match ranging slave address
#define bits8   0x00
#define bits16  0x40 
#define bits24  0x80
#define bits32  0xC0 


/*
  MIT license

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions
  of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/
