/*
  Copyright 2021 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 17/12/19
  New version 23/12/20
  New version, 15/07/21, Reliable packets added
*/

#define LORA_MAC_PRIVATE_SYNCWORD                   0x12
#define LORA_MAC_PUBLIC_SYNCWORD                    0x34

//radio operatine modes - see RegOpMode in register map
#define MODE_SLEEP                                  0x00
#define MODE_STDBY                                  0x01
#define MODE_STDBY_RC                               0x01

#define MODE_FSTX                                   0x02   //Frequency synthesis TX mode
#define MODE_TX                                     0x03   //TX mode
#define MODE_FSRX                                   0x04   //Frequency synthesis RX mode
#define MODE_RXCONTINUOUS                           0x05   //RX continuous mode
#define MODE_RXSINGLE                               0x06   //RX single mode

#define MODE_CAD                                    0xFF   //RX CAD mode

#define POWERSAVE                                   0xC0   //select minimum LNA gain
#define BOOSTED                                     0x38   //mode for booted, max LNA gain 

#define LNAGAING1                                   0x20   //maximum LNA gain
#define LNAGAING2                                   0x40
#define LNAGAING3                                   0x60
#define LNAGAING4                                   0x80
#define LNAGAING5                                   0xA0
#define LNAGAING6                                   0xC0   //minimum LNA gain 

#define    PACKET_TYPE_GFSK                         0x00   //regopmode setting for FSK and direct mode
#define    PACKET_TYPE_LORA                         0x80   //regopmode setting for LoRa 
#define    PACKET_TYPE_NONE                         0x0F


#define    RADIO_RAMP_10_US                         0x0F
#define    RADIO_RAMP_12_US                         0x0E
#define    RADIO_RAMP_15_US                         0x0D
#define    RADIO_RAMP_20_US                         0x0C
#define    RADIO_RAMP_25_US                         0x0B
#define    RADIO_RAMP_31_US                         0x0A
#define    RADIO_RAMP_40_US                         0x09
#define    RADIO_RAMP_50_US                         0x08
#define    RADIO_RAMP_62_US                         0x07
#define    RADIO_RAMP_100_US                        0x06
#define    RADIO_RAMP_125_US                        0x05
#define    RADIO_RAMP_250_US                        0x04
#define    RADIO_RAMP_500_US                        0x03
#define    RADIO_RAMP_1000_US                       0x02
#define    RADIO_RAMP_2000_US                       0x01
#define    RADIO_RAMP_3400_US                       0x00
#define    RADIO_RAMP_DEFAULT                       0x09

#define    OCP_TRIM_ON                              0x20
#define    OCP_TRIM_OFF                             0x00
#define    OCP_TRIM_45MA                            0x00
#define    OCP_TRIM_80MA                            0x07
#define    OCP_TRIM_100MA                           0x0B
#define    OCP_TRIM_110MA                           0x0D
#define    OCP_TRIM_120MA                           0x0F
#define    OCP_TRIM_130MA                           0x10
#define    OCP_TRIM_140MA                           0x11
#define    OCP_TRIM_150MA                           0x12

//Constant names for bandwidth settings
#define    LORA_BW_500                              144  //actual 500000hz
#define    LORA_BW_250                              128  //actual 250000hz
#define    LORA_BW_125                              112  //actual 125000hz
#define    LORA_BW_062                              96   //actual  62500hz 
#define    LORA_BW_041                              80   //actual  41670hz
#define    LORA_BW_031                              64   //actual  31250hz 
#define    LORA_BW_020                              48   //actual  20830hz
#define    LORA_BW_015                              32   //actual  15630hz
#define    LORA_BW_010                              16   //actual  10420hz 
#define    LORA_BW_007                              0    //actual   7810hz


//for SX127x
#define    LORA_SF6                                 0x06
#define    LORA_SF7                                 0x07
#define    LORA_SF8                                 0x08
#define    LORA_SF9                                 0x09
#define    LORA_SF10                                0x0A
#define    LORA_SF11                                0x0B
#define    LORA_SF12                                0x0C


#define    LORA_CR_4_5                              0x02
#define    LORA_CR_4_6                              0x04
#define    LORA_CR_4_7                              0x06
#define    LORA_CR_4_8                              0x08

#define  LDRO_OFF                                   0x00
#define  LDRO_ON                                    0x01
#define  LDRO_AUTO                                  0x02   //when set causes LDRO to be automatically calculated 

#define    WAIT_RX                                  0x01
#define    WAIT_TX                                  0x01
#define    NO_WAIT                                  0x00

#define    FREQ_STEP                                61.03515625

//These are the &/AND values for reading a parameter from a register.
//For example the Bandwidth on a SX1278 is stored in bits 7-4 and those bits are
//in effect numbers from 0 to 9, where 0 = 7800hz and 9 = 500000hz. Thus to read
//the bandwidth value you need to &/AND the contents of the register with a value
//The registers and values are different for SX1272 and the rest, so they are identified
//by _2 and _X respectivly. To read all the bits of a register, appart from the setting
//use the &/AND value with all bits inverted.

#define READ_BW_AND_2                               0xC0  //register 0x1D
#define READ_BW_AND_X                               0xF0  //register 0x1D

#define READ_CR_AND_2                               0x38  //register 0x1D
#define READ_CR_AND_X                               0x0E  //register 0x1D

#define READ_SF_AND_2                               0xF0  //register 0x1E
#define READ_SF_AND_X                               0xF0  //register 0x1E

#define READ_HASCRC_AND_2                           0x02  //register 0x1D
#define READ_HASCRC_AND_X                           0x04  //register 0x1E

#define READ_LDRO_AND_2                             0x01  //register 0x1D
#define READ_LDRO_AND_X                             0x08  //register 0x26

#define READ_AGCAUTO_AND_2                          0x04  //register 0x1E
#define READ_AGCAUTO_AND_X                          0x04  //register 0x26

#define READ_IMPLCIT_AND_2                          0x04  //register 0x1D
#define READ_IMPLCIT_AND_X                          0x01  //register 0x1D

#define READ_LNAGAIN_AND_2                          0xE0  //register 0x0C
#define READ_LNAGAIN_AND_X                          0xE0  //register 0x0C

#define READ_LNABOOSTHF_AND_2                       0x03  //register 0x0C
#define READ_LNABOOSTHF_AND_X                       0x03  //register 0x0C

#define READ_LNABOOSTLF_AND_2                       0x18  //register 0x0C
#define READ_LNABOOSTLF_AND_X                       0x18  //register 0x0C

#define READ_OPMODE_AND_2                           0x18  //register 0x01
#define READ_OPMODE_AND_X                           0x18  //register 0x01

#define READ_RANGEMODE_AND_2                        0x80  //register 0x01
#define READ_RANGEMODE_AND_X                        0x80  //register 0x01

#define    LORA_PACKET_VARIABLE_LENGTH              0x00
#define    LORA_PACKET_FIXED_LENGTH                 0x01
#define    LORA_PACKET_EXPLICIT                     LORA_PACKET_VARIABLE_LENGTH
#define    LORA_PACKET_IMPLICIT                     LORA_PACKET_FIXED_LENGTH

//for SX127x
#define    LORA_CRC_ON                              0x01  //Packet CRC is activated
#define    LORA_CRC_OFF                             0x00  //Packet CRC not used

#define    LORA_IQ_NORMAL                           0x00
#define    LORA_IQ_INVERTED                         0x40

//For SX127x - mapping of these IRQs to the SX126x and SX128x style is not easy
//the values have been fixed to these following DIOs. For instance IRQ_RX_DONE,
//IRQ_TX_DONE and IRQ_CAD_DONE can only be mapped to DIO0.
//For most applications only DIO0, DIO1 and DIO2 are connected.

#define    IRQ_RADIO_NONE                           0x00
#define    IRQ_CAD_ACTIVITY_DETECTED                0x01  //active on DIO1 
#define    IRQ_FSHS_CHANGE_CHANNEL                  0x02  //active on DIO2 
#define    IRQ_CAD_DONE                             0x04  //active on DIO0 
#define    IRQ_TX_DONE                              0x08  //active on DIO0 
#define    IRQ_HEADER_VALID                         0x10  //read from IRQ register only
#define    IRQ_CRC_ERROR                            0x20  //read from IRQ register only
#define    IRQ_RX_DONE                              0x40  //active on DIO0 
#define    IRQ_RADIO_ALL                            0xFFFF

#define    IRQ_TX_TIMEOUT                       0x0100     //so that readIrqstatus can return additional detections 
#define    IRQ_RX_TIMEOUT                       0x0200     //so that readIrqstatus can return additional detections  
#define    IRQ_NO_PACKET_CRC                    0x0400     //so that readIrqstatus can return additional detections  


#define CONFIGURATION_RETENTION 0x04                       //these have no effect in SX127x, kept for compatibility
#define RTC_TIMEOUT_ENABLE 0x01


//SX127x Register names
const uint8_t REG_FIFO = 0x00;
const uint8_t WREG_FIFO = 0x80;                             //this is the write address for the FIFO
const uint8_t REG_OPMODE = 0x01;
const uint8_t REG_FDEVLSB = 0x05;
const uint8_t REG_FRMSB = 0x06;
const uint8_t REG_FRMID = 0x07;
const uint8_t REG_FRLSB = 0x08;
const uint8_t REG_PACONFIG = 0x09;
const uint8_t REG_PARAMP = 0x0A;
const uint8_t REG_OCP = 0x0B;
const uint8_t REG_LNA = 0x0C;
const uint8_t REG_FIFOADDRPTR = 0x0D;
const uint8_t REG_FIFOTXBASEADDR = 0x0E;
const uint8_t REG_FIFORXBASEADDR = 0x0F;
const uint8_t REG_FIFORXCURRENTADDR = 0x10;
const uint8_t REG_IRQFLAGSMASK = 0x11;
const uint8_t REG_IRQFLAGS = 0x12;
const uint8_t REG_RXNBBYTES = 0x13;
const uint8_t REG_RXHEADERCNTVALUEMSB = 0x14;
const uint8_t REG_RXHEADERCNTVALUELSB = 0x15;
const uint8_t REG_RXPACKETCNTVALUEMSB = 0x16;
const uint8_t REG_RXPACKETCNTVALUELSB = 0x17;
const uint8_t REG_MODEMSTAT = 0x18;
const uint8_t REG_PKTSNRVALUE = 0x19;
const uint8_t REG_PKTRSSIVALUE = 0x1A;
const uint8_t REG_RSSIVALUE = 0x1B;
const uint8_t REG_CURRENTRSSIVALUE = 0x1B;
const uint8_t REG_HOPCHANNEL = 0x1C;
const uint8_t REG_MODEMCONFIG1 = 0x1D;
const uint8_t REG_MODEMCONFIG2 = 0x1E;
const uint8_t REG_SYMBTIMEOUTLSB = 0x1F;
const uint8_t REG_PREAMBLEMSB = 0x20;
const uint8_t REG_PREAMBLELSB = 0x21;
const uint8_t REG_PAYLOADLENGTH = 0x22;
const uint8_t REG_FIFORXBYTEADDR = 0x25;
const uint8_t REG_MODEMCONFIG3 = 0x26;
const uint8_t REG_PPMCORRECTION = 0x27;
const uint8_t REG_FEIMSB = 0x28;
const uint8_t REG_FEIMID = 0x29;
const uint8_t REG_FEILSB = 0x2A;
const uint8_t REG_LRRSSIWIDEBAND = 0x2C;
const uint8_t REG_LRTEST2F = 0x2F;
const uint8_t REG_LRTEST30 = 0x30;
const uint8_t REG_DETECTOPTIMIZE = 0x31;
const uint8_t REG_INVERTIQ = 0x33;
const uint8_t REG_LRTEST36 = 0x36;
const uint8_t REG_HIGHBWOPTIMIZE1 = 0x36;
const uint8_t REG_LRDETECTIONTHRESHOLD = 0x37;
const uint8_t REG_DETECTIONTHRESHOLD = 0x37;
const uint8_t REG_SYNCWORD = 0x39;
const uint8_t REG_LRTEST3A = 0x3A;
const uint8_t REG_HIGHBWOPTIMIZE2 = 0x3A;
const uint8_t REG_IMAGECAL = 0x3B;
const uint8_t REG_INVERTIQ2 = 0x3B;
const uint8_t REG_TEMP = 0x3C;
const uint8_t REG_DIOMAPPING1 = 0x40;
const uint8_t REG_DIOMAPPING2 = 0x41;
const uint8_t REG_VERSION = 0x42;
const uint8_t REG_PLLHOP = 0x44;
const uint8_t REG_PADAC = 0x4D;

#define PRINT_LOW_REGISTER   0x00
#define PRINT_HIGH_REGISTER  0x4F

#define DEVICE_SX1272  0x10         //for modules that use the PA_BOOST pin for RF output
#define DEVICE_SX1276  0x11         //bit 4 set indicates PA_BOOST in use
#define DEVICE_SX1277  0x12
#define DEVICE_SX1278  0x13
#define DEVICE_SX1279  0x14


#define DEVICE_SX1272_PABOOST  0x10  //for modules that use the RF_BOOST pin for RF output
#define DEVICE_SX1276_PABOOST  0x11  //bit 4 set indicates PA_BOOST in use
#define DEVICE_SX1277_PABOOST  0x12
#define DEVICE_SX1278_PABOOST  0x13
#define DEVICE_SX1279_PABOOST  0x14

//no support for SX1272 modules using RFO output, dont have one to test
#define DEVICE_SX1276_RFO   0x01     //for modules that use the RFO LF_ANT or HF_ANT pin for RF output
#define DEVICE_SX1277_RFO   0x02     //bit 4 clear indicates RFO in use
#define DEVICE_SX1278_RFO   0x03
#define DEVICE_SX1279_RFO   0x04


//power settings
#define MAXPOWER11dBm   0x00         //REG_PACONFIG = x000xxxx
#define MAXPOWER14dBm   0x50         //REG_PACONFIG = x101xxxx
#define MAXPOWER17dBm   0x70         //REG_PACONFIG = x111xxxx
#define PABOOSTON       0x80
#define PABOOSTOFF      0x00


//SPI settings
#define LTspeedMaximum  8000000
#define LTdataOrder     MSBFIRST
#define LTdataMode      SPI_MODE0

#define Deviation5khz   0x52

//FSKRTTY Settings
#define ParityNone 0
#define ParityOdd 1
#define ParityEven 2
#define ParityZero 0xF0
#define ParityOne  0xF1

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
