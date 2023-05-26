/*
  Copyright 2019 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  17/12/19
  Revised 24/03/22
*/


#define XTAL_FREQ                                   32000000
#define FREQ_DIV                                    33554432
#define FREQ_STEP                                   0.95367431640625
#define FREQ_ERR                                    0.47683715820312
#define FREQ_ERROR_CORRECTION                       1.55  //this was measured on SX1280 for bandwidth of 400khz

#define AUTO_RX_TX_OFFSET                           2

#define CRC_IBM_SEED                                0xFFFF
#define CRC_CCITT_SEED                              0x1D0F
#define CRC_POLYNOMIAL_IBM                          0x8005
#define CRC_POLYNOMIAL_CCITT                        0x1021

//Registers - Used, tested
#define REG_LR_PAYLOADLENGTH                        0x0702
#define REG_IQ_POLARITY_SETUP                       0x0736
#define REG_LR_SYNCWORD                             0x0740
#define REG_RX_GAIN                                 0x08AC
#define REG_TX_MODULATION                           0x0889
#define REG_RFFrequency31_24                        0x088B
#define REG_RFFrequency23_16                        0x088C
#define REG_RFFrequency15_8                         0x088D
#define REG_RFFrequency7_0                          0x088E

//Registers - Not used, not tested
#define REG_LR_WHITSEEDBASEADDR_MSB                 0x06B8
#define REG_LR_WHITSEEDBASEADDR_LSB                 0x06B9
#define REG_LR_CRCSEEDBASEADDR                      0x06BC
#define REG_LR_CRCPOLYBASEADDR                      0x06BE
#define REG_LR_PACKETPARAMS                         0x0704
#define REG_LR_SYNCWORDBASEADDRESS                  0x06C0

#define REG_FREQUENCY_ERRORBASEADDR                 0x076B
#define RANDOM_NUMBER_GENERATORBASEADDR             0x0819
#define REG_OCP                                     0x08E7
#define REG_XTA_TRIM                                0x0911


#define LORA_MAC_PRIVATE_SYNCWORD                   0x1424
#define LORA_MAC_PUBLIC_SYNCWORD                    0x3444

#define PRINT_LOW_REGISTER                           0x680
#define PRINT_HIGH_REGISTER                          0x920


#define POWER_SAVE_GAIN                              0x94
#define BOOSTED_GAIN                                 0x96


//radio operatine modes
#define MODE_SLEEP                                  0x07
#define MODE_STDBY_RC                               0x00   //Device running on RC13M, set STDBY_RC mode 
#define MODE_STDBY_XOSC                             0x01   //Device running on XTAL 32MHz, set STDBY_XOSC mode
#define MODE_FS                                     0x02   // The radio is in frequency synthesis mode
#define MODE_TX                                     0x03   //TX mode
#define MODE_RX                                     0x04   //RX mode
#define MODE_RX_DC                                  0x05   //RX duty cycle mode
#define MODE_CAD                                    0x06   //RX CAD mode 

//Sleep Mode Definition
#define COLD_START 0x00
#define WARM_START  0x04
#define RTC_TIMEOUT_DISABLE  0x00
#define RTC_TIMEOUT_ENABLE   0x01

#define USE_LDO                                     0x00   //default
#define USE_DCDC                                    0x01

#define    PACKET_TYPE_GFSK                         0x00
#define    PACKET_TYPE_LORA                         0x01
#define    PACKET_TYPE_NONE                         0x0F

#define    RADIO_RAMP_10_US                         0x00
#define    RADIO_RAMP_20_US                         0x01
#define    RADIO_RAMP_40_US                         0x02
#define    RADIO_RAMP_80_US                         0x03
#define    RADIO_RAMP_200_US                        0x04
#define    RADIO_RAMP_800_US                        0x05
#define    RADIO_RAMP_1700_US                       0x06
#define    RADIO_RAMP_3400_US                       0x07

#define    LORA_CAD_01_SYMBOL                       0x00
#define    LORA_CAD_02_SYMBOL                       0x01
#define    LORA_CAD_04_SYMBOL                       0x02
#define    LORA_CAD_08_SYMBOL                       0x03
#define    LORA_CAD_16_SYMBOL                       0x04

#define    LORA_CAD_ONLY                            0x00
#define    LORA_CAD_RX                              0x01
#define    LORA_CAD_LBT                             0x10

#define    LORA_SF5                                 0x05
#define    LORA_SF6                                 0x06
#define    LORA_SF7                                 0x07
#define    LORA_SF8                                 0x08
#define    LORA_SF9                                 0x09
#define    LORA_SF10                                0x0A
#define    LORA_SF11                                0x0B
#define    LORA_SF12                                0x0C

#define    LORA_BW_500                              6      //actual 500000hz
#define    LORA_BW_250                              5      //actual 250000hz
#define    LORA_BW_125                              4      //actual 125000hz
#define    LORA_BW_062                              3      //actual  62500hz 
#define    LORA_BW_041                              10     //actual  41670hz
#define    LORA_BW_031                              2      //actual  31250hz 
#define    LORA_BW_020                              9      //actual  20830hz
#define    LORA_BW_015                              1      //actual  15630hz
#define    LORA_BW_010                              8      //actual  10420hz 
#define    LORA_BW_007                              0      //actual   7810hz

#define    LORA_CR_4_5                              0x01
#define    LORA_CR_4_6                              0x02
#define    LORA_CR_4_7                              0x03
#define    LORA_CR_4_8                              0x04

#define    WAIT_RX                                  0x01
#define    WAIT_TX                                  0x01
#define    NO_WAIT                                  0x00

#define    RADIO_PREAMBLE_DETECTOR_OFF              0x00         //!< Preamble detection length off
#define    RADIO_PREAMBLE_DETECTOR_08_BITS          0x04         //!< Preamble detection length 8 bits
#define    RADIO_PREAMBLE_DETECTOR_16_BITS          0x05         //!< Preamble detection length 16 bits
#define    RADIO_PREAMBLE_DETECTOR_24_BITS          0x06         //!< Preamble detection length 24 bits
#define    RADIO_PREAMBLE_DETECTOR_32_BITS          0x07         //!< Preamble detection length 32 bit

#define    RADIO_ADDRESSCOMP_FILT_OFF               0x00         //!< No correlator turned on i.e. do not search for SyncWord
#define    RADIO_ADDRESSCOMP_FILT_NODE              0x01
#define    RADIO_ADDRESSCOMP_FILT_NODE_BROAD        0x02

#define    RADIO_PACKET_FIXED_LENGTH                0x00         //!< The packet is known on both sides no header included in the packet
#define    RADIO_PACKET_VARIABLE_LENGTH             0x01         //!< The packet is on variable size header included

#define    RADIO_CRC_OFF                            0x01         //!< No CRC in use
#define    RADIO_CRC_1_BYTES                        0x00
#define    RADIO_CRC_2_BYTES                        0x02
#define    RADIO_CRC_1_BYTES_INV                    0x04
#define    RADIO_CRC_2_BYTES_INV                    0x06
#define    RADIO_CRC_2_BYTES_IBM                    0xF1
#define    RADIO_CRC_2_BYTES_CCIT                   0xF2

#define    RADIO_DC_FREE_OFF                        0x00
#define    RADIO_DC_FREEWHITENING                   0x01

#define    LORA_PACKET_VARIABLE_LENGTH              0x00         //!< The packet is on variable size header included
#define    LORA_PACKET_FIXED_LENGTH                 0x01         //!< The packet is known on both sides no header included in the packet
#define    LORA_PACKET_EXPLICIT                     LORA_PACKET_VARIABLE_LENGTH
#define    LORA_PACKET_IMPLICIT                     LORA_PACKET_FIXED_LENGTH

#define    LORA_CRC_ON                              0x01         //!< CRC activated
#define    LORA_CRC_OFF                             0x00         //!< CRC not used

#define    LORA_IQ_NORMAL                           0x00
#define    LORA_IQ_INVERTED                         0x01

#define    TCXO_CTRL_1_6V                           0x00
#define    TCXO_CTRL_1_7V                           0x01
#define    TCXO_CTRL_1_8V                           0x02
#define    TCXO_CTRL_2_2V                           0x03
#define    TCXO_CTRL_2_4V                           0x04
#define    TCXO_CTRL_2_7V                           0x05
#define    TCXO_CTRL_3_0V                           0x06
#define    TCXO_CTRL_3_3V                           0x07

#define    IRQ_RADIO_NONE                           0x0000
#define    IRQ_TX_DONE                              0x0001
#define    IRQ_RX_DONE                              0x0002
#define    IRQ_PREAMBLE_DETECTED                    0x0004
#define    IRQ_SYNCWORD_VALID                       0x0008
#define    IRQ_HEADER_VALID                         0x0010
#define    IRQ_HEADER_ERROR                         0x0020
#define    IRQ_CRC_ERROR                            0x0040
#define    IRQ_CAD_DONE                             0x0080
#define    IRQ_CAD_ACTIVITY_DETECTED                0x0100
#define    IRQ_RX_TX_TIMEOUT                        0x0200
#define    IRQ_TX_TIMEOUT                           0x0200
#define    IRQ_RX_TIMEOUT                           0x0200
#define    IRQ_RADIO_ALL                            0xFFFF

#define    RADIO_GET_STATUS                         0xC0
#define    RADIO_WRITE_REGISTER                     0x0D
#define    RADIO_READ_REGISTER                      0x1D
#define    RADIO_WRITE_BUFFER                       0x0E
#define    RADIO_READ_BUFFER                        0x1E
#define    RADIO_SET_SLEEP                          0x84
#define    RADIO_SET_STANDBY                        0x80
#define    RADIO_SET_FS                             0xC1
#define    RADIO_SET_TX                             0x83
#define    RADIO_SET_RX                             0x82
#define    RADIO_SET_RXDUTYCYCLE                    0x94
#define    RADIO_SET_CAD                            0xC5
#define    RADIO_SET_TXCONTINUOUSWAVE               0xD1
#define    RADIO_SET_TXCONTINUOUSPREAMBLE           0xD2
#define    RADIO_SET_PACKETTYPE                     0x8A
#define    RADIO_GET_PACKETTYPE                     0x11
#define    RADIO_SET_RFFREQUENCY                    0x86
#define    RADIO_SET_TXPARAMS                       0x8E
#define    RADIO_SET_PACONFIG                       0x95
#define    RADIO_SET_CADPARAMS                      0x88
#define    RADIO_SET_BUFFERBASEADDRESS              0x8F
#define    RADIO_SET_MODULATIONPARAMS               0x8B
#define    RADIO_SET_PACKETPARAMS                   0x8C
#define    RADIO_GET_RXBUFFERSTATUS                 0x13
#define    RADIO_GET_PACKETSTATUS                   0x14
#define    RADIO_GET_RSSIINST                       0x15
#define    RADIO_GET_STATS                          0x10
#define    RADIO_RESET_STATS                        0x00
#define    RADIO_CFG_DIOIRQ                         0x08
#define    RADIO_GET_IRQSTATUS                      0x12
#define    RADIO_CLR_IRQSTATUS                      0x02
#define    RADIO_CALIBRATE                          0x89
#define    RADIO_CALIBRATEIMAGE                     0x98
#define    RADIO_SET_REGULATORMODE                  0x96
#define    RADIO_GET_ERROR                          0x17
#define    RADIO_CLEAR_ERRORS                       0x07
#define    RADIO_SET_TCXOMODE                       0x97
#define    RADIO_SET_TXFALLBACKMODE                 0x93
#define    RADIO_SET_RFSWITCHMODE                   0x9D
#define    RADIO_SET_STOPRXTIMERONPREAMBLE          0x9F
#define    RADIO_SET_LORASYMBTIMEOUT                0xA0

//Table 13-2 SX126X Sleep modes
#define CONFIGURATION_RETENTION 0x04
#define RTC_TIMEOUT_ENABLE 0x01

#define  LDRO_OFF     0x00
#define  LDRO_ON      0x01
#define  LDRO_AUTO    0x02                       //when used causes LDRO to be automatically calculated 

#define DEVICE_SX1261  0x01
#define DEVICE_SX1262  0x00
#define DEVICE_SX1268  0x02

#define PAAUTO 0xFF
#define HPMAXAUTO 0xFF

#ifndef RAMP_TIME
#define RAMP_TIME RADIO_RAMP_40_US
#endif

#define RC64KEnable       0x01                   //calibrate RC64K clock
#define RC13MEnable       0x02                   //calibrate RC13M clock
#define PLLEnable         0x04                   //calibrate PLL
#define ADCPulseEnable    0x08                   //calibrate ADC Pulse
#define ADCBulkNEnable    0x10                   //calibrate ADC bulkN
#define ADCBulkPEnable    0x20                   //calibrate ADC bulkP
#define ImgEnable         0x40                   //calibrate image
#define ALLDevices        0x7F                   //calibrate all devices 

#define RC64K_CALIB_ERR   0x0001
#define RC13M_CALIB_ERR   0x0002
#define PLL_CALIB_ERR     0x0004
#define ADC_CALIB_ERR     0x0008
#define IMG_CALIB_ERR     0x0010
#define XOSC_START_ERR    0x0020
#define PLL_LOCK_ERR      0x0040
#define RFU               0x0080
#define PA_RAMP_ERR       0x0100

//SPI settings
#define LTspeedMaximum  8000000
#define LTdataOrder     MSBFIRST
#define LTdataMode      SPI_MODE0

//FSKRTTY Settings
#define ParityNone 0
#define ParityOdd 1
#define ParityEven 2
#define ParityZero 0xF0
#define ParityOne  0xF1

#define ToneMinuS 52                     //timed constant for ATmega328P at 8Mhz with FM Tone delayus at 0, period for half loop

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
