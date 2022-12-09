/*
	This is a library written for the u-blox ZED-F9P and NEO-M8P-2
	SparkFun sells these at its website: www.sparkfun.com
	Do you like this library? Help support SparkFun. Buy a board!
	https://www.sparkfun.com/products/16481
	https://www.sparkfun.com/products/15136
	https://www.sparkfun.com/products/15005
	https://www.sparkfun.com/products/15733
	https://www.sparkfun.com/products/15193
	https://www.sparkfun.com/products/15210

	Written by Nathan Seidle @ SparkFun Electronics, September 6th, 2018

	This library handles configuring and handling the responses
	from a u-blox GPS module. Works with most modules from u-blox including
	the Zed-F9P, NEO-M8P-2, NEO-M9N, ZOE-M8Q, SAM-M8Q, and many others.

	https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library

	Development environment specifics:
	Arduino IDE 1.8.5

	SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
	The MIT License (MIT)
	Copyright (c) 2016 SparkFun Electronics
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
	associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to
	do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial
	portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
	NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __u_blox_config_keys_h__
#define __u_blox_config_keys_h__

//The following consts are used to generate KEY values for the advanced protocol functions of VELGET/SET/DEL
const uint8_t VAL_SIZE_1 = 0x01; //One bit
const uint8_t VAL_SIZE_8 = 0x02;	 //One byte
const uint8_t VAL_SIZE_16 = 0x03;	 //Two bytes
const uint8_t VAL_SIZE_32 = 0x04;	 //Four bytes
const uint8_t VAL_SIZE_64 = 0x05;	 //Eight bytes

//These are the Bitfield layers definitions for the UBX-CFG-VALSET message (not to be confused with Bitfield deviceMask in UBX-CFG-CFG)
const uint8_t VAL_LAYER_RAM = (1 << 0);
const uint8_t VAL_LAYER_BBR = (1 << 1);
const uint8_t VAL_LAYER_FLASH = (1 << 2);
const uint8_t VAL_LAYER_ALL = VAL_LAYER_RAM | VAL_LAYER_BBR | VAL_LAYER_FLASH; //Not valid with getVal()

//Below are various Groups, IDs, and sizes for various settings
//These can be used to call getVal/setVal/delVal
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint8_t VAL_ID_PROT_UBX = 0x01;
const uint8_t VAL_ID_PROT_NMEA = 0x02;
const uint8_t VAL_ID_PROT_RTCM3 = 0x04;

const uint8_t VAL_GROUP_I2C = 0x51;
const uint8_t VAL_GROUP_I2COUTPROT = 0x72;
const uint8_t VAL_GROUP_UART1INPROT = 0x73;
const uint8_t VAL_GROUP_UART1OUTPROT = 0x74;
const uint8_t VAL_GROUP_UART2INPROT = 0x75;
const uint8_t VAL_GROUP_UART2OUTPROT = 0x76;
const uint8_t VAL_GROUP_USBINPROT = 0x77;
const uint8_t VAL_GROUP_USBOUTPROT = 0x78;

const uint8_t VAL_GROUP_UART_SIZE = VAL_SIZE_1; //All fields in UART group are currently 1 bit
const uint8_t VAL_GROUP_I2C_SIZE = VAL_SIZE_8;	//All fields in I2C group are currently 1 byte

const uint8_t VAL_ID_I2C_ADDRESS = 0x01;

//Below are the key values for a given configuration setting

//CFG-NMEA
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint32_t UBLOX_CFG_NMEA_PROTVER = 0x10930001;
const uint32_t UBLOX_CFG_NMEA_MAXSVS = 0x10930002;
const uint32_t UBLOX_CFG_NMEA_COMPAT = 0x10930003;
const uint32_t UBLOX_CFG_NMEA_CONSIDER = 0x10930004;
const uint32_t UBLOX_CFG_NMEA_LIMIT82 = 0x10930005;
const uint32_t UBLOX_CFG_NMEA_HIGHPREC = 0x10930006;
const uint32_t UBLOX_CFG_NMEA_SVNUMBERING = 0x20930007;
const uint32_t UBLOX_CFG_NMEA_FILT_GPS = 0x10930011;
const uint32_t UBLOX_CFG_NMEA_FILT_SBAS = 0x10930012;
const uint32_t UBLOX_CFG_NMEA_FILT_GAL = 0x10930013;
const uint32_t UBLOX_CFG_NMEA_FILT_QZSS = 0x10930015;
const uint32_t UBLOX_CFG_NMEA_FILT_GLO = 0x10930016;
const uint32_t UBLOX_CFG_NMEA_FILT_BDS = 0x10930017;
const uint32_t UBLOX_CFG_NMEA_OUT_INVFIX = 0x10930021;
const uint32_t UBLOX_CFG_NMEA_OUT_MSKFIX = 0x10930022;
const uint32_t UBLOX_CFG_NMEA_OUT_INVTIME = 0x10930023;
const uint32_t UBLOX_CFG_NMEA_OUT_INVDATE = 0x10930024;
const uint32_t UBLOX_CFG_NMEA_OUT_ONLYGPS = 0x10930025;
const uint32_t UBLOX_CFG_NMEA_OUT_FROZENCOG = 0x10930026;
const uint32_t UBLOX_CFG_NMEA_MAINTALKERID = 0x20930031;
const uint32_t UBLOX_CFG_NMEA_GSVTALKERID = 0x20930032;
const uint32_t UBLOX_CFG_NMEA_BDSTALKERID = 0x30930033;

//CFG-RATE
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint32_t UBLOX_CFG_RATE_MEAS = 0x30210001;
const uint32_t UBLOX_CFG_RATE_NAV = 0x30210002;
const uint32_t UBLOX_CFG_RATE_TIMEREF = 0x20210003;

//CFG-I2C
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint32_t UBLOX_CFG_I2C_ADDRESS = 0x20510001;
const uint32_t UBLOX_CFG_I2C_ENABLED = 0x10510003;

const uint32_t UBLOX_CFG_I2CINPROT_UBX = 0x10710001;
const uint32_t UBLOX_CFG_I2CINPROT_NMEA = 0x10710002;
const uint32_t UBLOX_CFG_I2CINPROT_RTCM3X = 0x10710004;

const uint32_t UBLOX_CFG_I2COUTPROT_UBX = 0x10720001;
const uint32_t UBLOX_CFG_I2COUTPROT_NMEA = 0x10720002;
const uint32_t UBLOX_CFG_I2COUTPROT_RTCM3X = 0x10720004;

//CFG-UART1
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint32_t UBLOX_CFG_UART1_BAUDRATE = 0x40520001;
const uint32_t UBLOX_CFG_UART1_ENABLED = 0x10520005;

const uint32_t UBLOX_CFG_UART1INPROT_UBX = 0x10730001;
const uint32_t UBLOX_CFG_UART1INPROT_NMEA = 0x10730002;
const uint32_t UBLOX_CFG_UART1INPROT_RTCM3X = 0x10730004;

const uint32_t UBLOX_CFG_UART1OUTPROT_UBX = 0x10740001;
const uint32_t UBLOX_CFG_UART1OUTPROT_NMEA = 0x10740002;
const uint32_t UBLOX_CFG_UART1OUTPROT_RTCM3X = 0x10740004;

//CFG-UART2
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint32_t UBLOX_CFG_UART2_BAUDRATE = 0x40530001;
const uint32_t UBLOX_CFG_UART2_ENABLED = 0x10530005;

const uint32_t UBLOX_CFG_UART2INPROT_UBX = 0x10750001;
const uint32_t UBLOX_CFG_UART2INPROT_NMEA = 0x10750002;
const uint32_t UBLOX_CFG_UART2INPROT_RTCM3X = 0x10750004;

const uint32_t UBLOX_CFG_UART2OUTPROT_UBX = 0x10760001;
const uint32_t UBLOX_CFG_UART2OUTPROT_NMEA = 0x10760002;
const uint32_t UBLOX_CFG_UART2OUTPROT_RTCM3X = 0x10760004;

//CFG-USB
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
const uint32_t UBLOX_CFG_USBINPROT_UBX = 0x10770001;
const uint32_t UBLOX_CFG_USBINPROT_NMEA = 0x10770002;
const uint32_t UBLOX_CFG_USBINPROT_RTCM3X = 0x10770004;

const uint32_t UBLOX_CFG_USBOUTPROT_UBX = 0x10780001;
const uint32_t UBLOX_CFG_USBOUTPROT_NMEA = 0x10780002;
const uint32_t UBLOX_CFG_USBOUTPROT_RTCM3X = 0x10780004;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// CFG-MSGOUT: Message output configuration
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// For each message and port a separate output rate (per second, per epoch) can be configured.
const uint32_t CFG_MSGOUT_NMEA_ID_DTM_I2C = 0x209100a6;				//Output rate of the NMEA-GX-DTM message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_DTM_SPI = 0x209100aa;				//Output rate of the NMEA-GX-DTM message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_DTM_UART1 = 0x209100a7;			//Output rate of the NMEA-GX-DTM message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_DTM_UART2 = 0x209100a8;			//Output rate of the NMEA-GX-DTM message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_DTM_USB = 0x209100a9;				//Output rate of the NMEA-GX-DTM message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GBS_I2C = 0x209100dd;				//Output rate of the NMEA-GX-GBS message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GBS_SPI = 0x209100e1;				//Output rate of the NMEA-GX-GBS message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GBS_UART1 = 0x209100de;			//Output rate of the NMEA-GX-GBS message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GBS_UART2 = 0x209100df;			//Output rate of the NMEA-GX-GBS message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GBS_USB = 0x209100e0;				//Output rate of the NMEA-GX-GBS message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GGA_I2C = 0x209100ba;				//Output rate of the NMEA-GX-GGA message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GGA_SPI = 0x209100be;				//Output rate of the NMEA-GX-GGA message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GGA_UART1 = 0x209100bb;			//Output rate of the NMEA-GX-GGA message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GGA_UART2 = 0x209100bc;			//Output rate of the NMEA-GX-GGA message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GGA_USB = 0x209100bd;				//Output rate of the NMEA-GX-GGA message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GLL_I2C = 0x209100c9;				//Output rate of the NMEA-GX-GLL message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GLL_SPI = 0x209100cd;				//Output rate of the NMEA-GX-GLL message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GLL_UART1 = 0x209100ca;			//Output rate of the NMEA-GX-GLL message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GLL_UART2 = 0x209100cb;			//Output rate of the NMEA-GX-GLL message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GLL_USB = 0x209100cc;				//Output rate of the NMEA-GX-GLL message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GNS_I2C = 0x209100b5;				//Output rate of the NMEA-GX-GNS message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GNS_SPI = 0x209100b9;				//Output rate of the NMEA-GX-GNS message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GNS_UART1 = 0x209100b6;			//Output rate of the NMEA-GX-GNS message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GNS_UART2 = 0x209100b7;			//Output rate of the NMEA-GX-GNS message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GNS_USB = 0x209100b8;				//Output rate of the NMEA-GX-GNS message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GRS_I2C = 0x209100ce;				//Output rate of the NMEA-GX-GRS message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GRS_SPI = 0x209100d2;				//Output rate of the NMEA-GX-GRS message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GRS_UART1 = 0x209100cf;			//Output rate of the NMEA-GX-GRS message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GRS_UART2 = 0x209100d0;			//Output rate of the NMEA-GX-GRS message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GRS_USB = 0x209100d1;				//Output rate of the NMEA-GX-GRS message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GSA_I2C = 0x209100bf;				//Output rate of the NMEA-GX-GSA message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GSA_SPI = 0x209100c3;				//Output rate of the NMEA-GX-GSA message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GSA_UART1 = 0x209100c0;			//Output rate of the NMEA-GX-GSA message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GSA_UART2 = 0x209100c1;			//Output rate of the NMEA-GX-GSA message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GSA_USB = 0x209100c2;				//Output rate of the NMEA-GX-GSA message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GST_I2C = 0x209100d3;				//Output rate of the NMEA-GX-GST message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GST_SPI = 0x209100d7;				//Output rate of the NMEA-GX-GST message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GST_UART1 = 0x209100d4;			//Output rate of the NMEA-GX-GST message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GST_UART2 = 0x209100d5;			//Output rate of the NMEA-GX-GST message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GST_USB = 0x209100d6;				//Output rate of the NMEA-GX-GST message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_GSV_I2C = 0x209100c4;				//Output rate of the NMEA-GX-GSV message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_GSV_SPI = 0x209100c8;				//Output rate of the NMEA-GX-GSV message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_GSV_UART1 = 0x209100c5;			//Output rate of the NMEA-GX-GSV message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_GSV_UART2 = 0x209100c6;			//Output rate of the NMEA-GX-GSV message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_GSV_USB = 0x209100c7;				//Output rate of the NMEA-GX-GSV message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_RLM_I2C = 0x20910400;				//Output rate of the NMEA-GX-RLM message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_RLM_SPI = 0x20910404;				//Output rate of the NMEA-GX-RLM message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_RLM_UART1 = 0x20910401;			//Output rate of the NMEA-GX-RLM message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_RLM_UART2 = 0x20910402;			//Output rate of the NMEA-GX-RLM message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_RLM_USB = 0x20910403;				//Output rate of the NMEA-GX-RLM message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_RMC_I2C = 0x209100ab;				//Output rate of the NMEA-GX-RMC message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_RMC_SPI = 0x209100af;				//Output rate of the NMEA-GX-RMC message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_RMC_UART1 = 0x209100ac;			//Output rate of the NMEA-GX-RMC message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_RMC_UART2 = 0x209100ad;			//Output rate of the NMEA-GX-RMC message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_RMC_USB = 0x209100ae;				//Output rate of the NMEA-GX-RMC message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_VLW_I2C = 0x209100e7;				//Output rate of the NMEA-GX-VLW message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_VLW_SPI = 0x209100eb;				//Output rate of the NMEA-GX-VLW message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_VLW_UART1 = 0x209100e8;			//Output rate of the NMEA-GX-VLW message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_VLW_UART2 = 0x209100e9;			//Output rate of the NMEA-GX-VLW message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_VLW_USB = 0x209100ea;				//Output rate of the NMEA-GX-VLW message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_VTG_I2C = 0x209100b0;				//Output rate of the NMEA-GX-VTG message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_VTG_SPI = 0x209100b4;				//Output rate of the NMEA-GX-VTG message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_VTG_UART1 = 0x209100b1;			//Output rate of the NMEA-GX-VTG message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_VTG_UART2 = 0x209100b2;			//Output rate of the NMEA-GX-VTG message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_VTG_USB = 0x209100b3;				//Output rate of the NMEA-GX-VTG message on port USB
const uint32_t CFG_MSGOUT_NMEA_ID_ZDA_I2C = 0x209100d8;				//Output rate of the NMEA-GX-ZDA message on port I2C
const uint32_t CFG_MSGOUT_NMEA_ID_ZDA_SPI = 0x209100dc;				//Output rate of the NMEA-GX-ZDA message on port SPI
const uint32_t CFG_MSGOUT_NMEA_ID_ZDA_UART1 = 0x209100d9;			//Output rate of the NMEA-GX-ZDA message on port UART1
const uint32_t CFG_MSGOUT_NMEA_ID_ZDA_UART2 = 0x209100da;			//Output rate of the NMEA-GX-ZDA message on port UART2
const uint32_t CFG_MSGOUT_NMEA_ID_ZDA_USB = 0x209100db;				//Output rate of the NMEA-GX-ZDA message on port USB
const uint32_t CFG_MSGOUT_PUBX_ID_POLYP_I2C = 0x209100ec;			//Output rate of the NMEA-GX-PUBX00 message on port I2C
const uint32_t CFG_MSGOUT_PUBX_ID_POLYP_SPI = 0x209100f0;			//Output rate of the NMEA-GX-PUBX00 message on port SPI
const uint32_t CFG_MSGOUT_PUBX_ID_POLYP_UART1 = 0x209100ed;		//Output rate of the NMEA-GX-PUBX00 message on port UART1
const uint32_t CFG_MSGOUT_PUBX_ID_POLYP_UART2 = 0x209100ee;	//Output rate of the NMEA-GX-PUBX00 message on port UART2
const uint32_t CFG_MSGOUT_PUBX_ID_POLYP_USB = 0x209100ef;		//Output rate of the NMEA-GX-PUBX00 message on port USB
const uint32_t CFG_MSGOUT_PUBX_ID_POLYS_I2C = 0x209100f1;			//Output rate of the NMEA-GX-PUBX03 message on port I2C
const uint32_t CFG_MSGOUT_PUBX_ID_POLYS_SPI = 0x209100f5;			//Output rate of the NMEA-GX-PUBX03 message on port SPI
const uint32_t CFG_MSGOUT_PUBX_ID_POLYS_UART1 = 0x209100f2;		//Output rate of the NMEA-GX-PUBX03 message on port UART1
const uint32_t CFG_MSGOUT_PUBX_ID_POLYS_UART2 = 0x209100f3;	//Output rate of the NMEA-GX-PUBX03 message on port UART2
const uint32_t CFG_MSGOUT_PUBX_ID_POLYS_USB = 0x209100f4;		//Output rate of the NMEA-GX-PUBX03 message on port USB
const uint32_t CFG_MSGOUT_PUBX_ID_POLYT_I2C = 0x209100f6;			//Output rate of the NMEA-GX-PUBX04 message on port I2C
const uint32_t CFG_MSGOUT_PUBX_ID_POLYT_SPI = 0x209100fa;			//Output rate of the NMEA-GX-PUBX04 message on port SPI
const uint32_t CFG_MSGOUT_PUBX_ID_POLYT_UART1 = 0x209100f7;		//Output rate of the NMEA-GX-PUBX04 message on port UART1
const uint32_t CFG_MSGOUT_PUBX_ID_POLYT_UART2 = 0x209100f8;	//Output rate of the NMEA-GX-PUBX04 message on port UART2
const uint32_t CFG_MSGOUT_PUBX_ID_POLYT_USB = 0x209100f9;		//Output rate of the NMEA-GX-PUBX04 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1005_I2C = 0x209102bd;		//Output rate of the RTCM-3X-TYPE1005 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1005_SPI = 0x209102c1;	//Output rate of the RTCM-3X-TYPE1005 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1005_UART1 = 0x209102be;//Output rate of the RTCM-3X-TYPE1005 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1005_UART2 = 0x209102bf;//Output rate of the RTCM-3X-TYPE1005 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1005_USB = 0x209102c0;	//Output rate of the RTCM-3X-TYPE1005 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1074_I2C = 0x2091035e;	//Output rate of the RTCM-3X-TYPE1074 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1074_SPI = 0x20910362;	//Output rate of the RTCM-3X-TYPE1074 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1074_UART1 = 0x2091035f;//Output rate of the RTCM-3X-TYPE1074 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1074_UART2 = 0x20910360;//Output rate of the RTCM-3X-TYPE1074 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1074_USB = 0x20910361;	//Output rate of the RTCM-3X-TYPE1074 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1077_I2C = 0x209102cc;	//Output rate of the RTCM-3X-TYPE1077 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1077_SPI = 0x209102d0;	//Output rate of the RTCM-3X-TYPE1077 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1077_UART1 = 0x209102cd;//Output rate of the RTCM-3X-TYPE1077 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1077_UART2 = 0x209102ce;//Output rate of the RTCM-3X-TYPE1077 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1077_USB = 0x209102cf;	//Output rate of the RTCM-3X-TYPE1077 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1084_I2C = 0x20910363;	//Output rate of the RTCM-3X-TYPE1084 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1084_SPI = 0x20910367;	//Output rate of the RTCM-3X-TYPE1084 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1084_UART1 = 0x20910364;//Output rate of the RTCM-3X-TYPE1084 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1084_UART2 = 0x20910365;//Output rate of the RTCM-3X-TYPE1084 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1084_USB = 0x20910366;	//Output rate of the RTCM-3X-TYPE1084 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1087_I2C = 0x209102d1;	//Output rate of the RTCM-3X-TYPE1087 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1087_SPI = 0x209102d5;	//Output rate of the RTCM-3X-TYPE1087 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1087_UART1 = 0x209102d2;//Output rate of the RTCM-3X-TYPE1087 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1087_UART2 = 0x209102d3;//Output rate of the RTCM-3X-TYPE1087 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1087_USB = 0x209102d4;	//Output rate of the RTCM-3X-TYPE1087 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1094_I2C = 0x20910368;	//Output rate of the RTCM-3X-TYPE1094 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1094_SPI = 0x2091036c;	//Output rate of the RTCM-3X-TYPE1094 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1094_UART1 = 0x20910369;//Output rate of the RTCM-3X-TYPE1094 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1094_UART2 = 0x2091036a;//Output rate of the RTCM-3X-TYPE1094 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1094_USB = 0x2091036b;	//Output rate of the RTCM-3X-TYPE1094 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1097_I2C = 0x20910318;	//Output rate of the RTCM-3X-TYPE1097 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1097_SPI = 0x2091031c;	//Output rate of the RTCM-3X-TYPE1097 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1097_UART1 = 0x20910319;//Output rate of the RTCM-3X-TYPE1097 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1097_UART2 = 0x2091031a;//Output rate of the RTCM-3X-TYPE1097 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1097_USB = 0x2091031b;	//Output rate of the RTCM-3X-TYPE1097 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1124_I2C = 0x2091036d;	//Output rate of the RTCM-3X-TYPE1124 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1124_SPI = 0x20910371;	//Output rate of the RTCM-3X-TYPE1124 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1124_UART1 = 0x2091036e;//Output rate of the RTCM-3X-TYPE1124 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1124_UART2 = 0x2091036f;//Output rate of the RTCM-3X-TYPE1124 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1124_USB = 0x20910370;	//Output rate of the RTCM-3X-TYPE1124 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1127_I2C = 0x209102d6;	//Output rate of the RTCM-3X-TYPE1127 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1127_SPI = 0x209102da;	//Output rate of the RTCM-3X-TYPE1127 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1127_UART1 = 0x209102d7;//Output rate of the RTCM-3X-TYPE1127 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1127_UART2 = 0x209102d8;//Output rate of the RTCM-3X-TYPE1127 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1127_USB = 0x209102d9;	//Output rate of the RTCM-3X-TYPE1127 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1230_I2C = 0x20910303;	//Output rate of the RTCM-3X-TYPE1230 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1230_SPI = 0x20910307;	//Output rate of the RTCM-3X-TYPE1230 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1230_UART1 = 0x20910304;//Output rate of the RTCM-3X-TYPE1230 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1230_UART2 = 0x20910305;//Output rate of the RTCM-3X-TYPE1230 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE1230_USB = 0x20910306;	//Output rate of the RTCM-3X-TYPE1230 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_0_I2C = 0x209102fe;//Output rate of the RTCM-3X-TYPE4072_0 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_0_SPI = 0x20910302;//Output rate of the RTCM-3X-TYPE4072_0 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_0_UART1 = 0x209102ff; //Output rate of the RTCM-3X-TYPE4072_0 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_0_UART2 = 0x20910300; //Output rate of the RTCM-3X-TYPE4072_0 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_0_USB = 0x20910301;//Output rate of the RTCM-3X-TYPE4072_0 message on port USB
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_1_I2C = 0x20910381;//Output rate of the RTCM-3X-TYPE4072_1 message on port I2C
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_1_SPI = 0x20910385;//Output rate of the RTCM-3X-TYPE4072_1 message on port SPI
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_1_UART1 = 0x20910382; //Output rate of the RTCM-3X-TYPE4072_1 message on port UART1
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_1_UART2 = 0x20910383; //Output rate of the RTCM-3X-TYPE4072_1 message on port UART2
const uint32_t CFG_MSGOUT_RTCM_3X_TYPE4072_1_USB = 0x20910384;//Output rate of the RTCM-3X-TYPE4072_1 message on port USB
const uint32_t CFG_MSGOUT_UBX_LOG_INFO_I2C = 0x20910259;		//Output rate of the UBX-LOG-INFO message on port I2C
const uint32_t CFG_MSGOUT_UBX_LOG_INFO_SPI = 0x2091025d;			//Output rate of the UBX-LOG-INFO message on port SPI
const uint32_t CFG_MSGOUT_UBX_LOG_INFO_UART1 = 0x2091025a;		//Output rate of the UBX-LOG-INFO message on port UART1
const uint32_t CFG_MSGOUT_UBX_LOG_INFO_UART2 = 0x2091025b;	//Output rate of the UBX-LOG-INFO message on port UART2
const uint32_t CFG_MSGOUT_UBX_LOG_INFO_USB = 0x2091025c;		//Output rate of the UBX-LOG-INFO message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_COMMS_I2C = 0x2091034f;			//Output rate of the UBX-MON-COMMS message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_COMMS_SPI = 0x20910353;			//Output rate of the UBX-MON-COMMS message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_COMMS_UART1 = 0x20910350;		//Output rate of the UBX-MON-COMMS message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_COMMS_UART2 = 0x20910351;	//Output rate of the UBX-MON-COMMS message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_COMMS_USB = 0x20910352;		//Output rate of the UBX-MON-COMMS message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_HW2_I2C = 0x209101b9;				//Output rate of the UBX-MON-HW2 message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_HW2_SPI = 0x209101bd;				//Output rate of the UBX-MON-HW2 message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_HW2_UART1 = 0x209101ba;			//Output rate of the UBX-MON-HW2 message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_HW2_UART2 = 0x209101bb;			//Output rate of the UBX-MON-HW2 message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_HW2_USB = 0x209101bc;				//Output rate of the UBX-MON-HW2 message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_HW3_I2C = 0x20910354;				//Output rate of the UBX-MON-HW3 message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_HW3_SPI = 0x20910358;				//Output rate of the UBX-MON-HW3 message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_HW3_UART1 = 0x20910355;			//Output rate of the UBX-MON-HW3 message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_HW3_UART2 = 0x20910356;			//Output rate of the UBX-MON-HW3 message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_HW3_USB = 0x20910357;				//Output rate of the UBX-MON-HW3 message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_HW_I2C = 0x209101b4;				//Output rate of the UBX-MON-HW message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_HW_SPI = 0x209101b8;				//Output rate of the UBX-MON-HW message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_HW_UART1 = 0x209101b5;			//Output rate of the UBX-MON-HW message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_HW_UART2 = 0x209101b6;			//Output rate of the UBX-MON-HW message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_HW_USB = 0x209101b7;				//Output rate of the UBX-MON-HW message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_IO_I2C = 0x209101a5;				//Output rate of the UBX-MON-IO message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_IO_SPI = 0x209101a9;				//Output rate of the UBX-MON-IO message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_IO_UART1 = 0x209101a6;			//Output rate of the UBX-MON-IO message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_IO_UART2 = 0x209101a7;			//Output rate of the UBX-MON-IO message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_IO_USB = 0x209101a8;				//Output rate of the UBX-MON-IO message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_MSGPP_I2C = 0x20910196;			//Output rate of the UBX-MON-MSGPP message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_MSGPP_SPI = 0x2091019a;			//Output rate of the UBX-MON-MSGPP message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_MSGPP_UART1 = 0x20910197;		//Output rate of the UBX-MON-MSGPP message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_MSGPP_UART2 = 0x20910198;	//Output rate of the UBX-MON-MSGPP message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_MSGPP_USB = 0x20910199;		//Output rate of the UBX-MON-MSGPP message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_RF_I2C = 0x20910359;				//Output rate of the UBX-MON-RF message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_RF_SPI = 0x2091035d;				//Output rate of the UBX-MON-RF message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_RF_UART1 = 0x2091035a;			//Output rate of the UBX-MON-RF message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_RF_UART2 = 0x2091035b;			//Output rate of the UBX-MON-RF message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_RF_USB = 0x2091035c;				// Output rate of the UBX-MON-RF message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_RXBUF_I2C = 0x209101a0;			// Output rate of the UBX-MON-RXBUF message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_RXBUF_SPI = 0x209101a4;			// Output rate of the UBX-MON-RXBUF message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_RXBUF_UART1 = 0x209101a1;		// Output rate of the UBX-MON-RXBUF message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_RXBUF_UART2 = 0x209101a2;	// Output rate of the UBX-MON-RXBUF message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_RXBUF_USB = 0x209101a3;		// Output rate of the UBX-MON-RXBUF message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_RXR_I2C = 0x20910187;				// Output rate of the UBX-MON-RXR message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_RXR_SPI = 0x2091018b;				// Output rate of the UBX-MON-RXR message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_RXR_UART1 = 0x20910188;			// Output rate of the UBX-MON-RXR message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_RXR_UART2 = 0x20910189;			// Output rate of the UBX-MON-RXR message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_RXR_USB = 0x2091018a;				// Output rate of the UBX-MON-RXR message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_SPAN_I2C = 0x2091038b;			// Output rate of the UBX-MON-SPAN message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_SPAN_SPI = 0x2091038f;			// Output rate of the UBX-MON-SPAN message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_SPAN_UART1 = 0x2091038c;		// Output rate of the UBX-MON-SPAN message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_SPAN_UART2 = 0x2091038d;	// Output rate of the UBX-MON-SPAN message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_SPAN_USB = 0x2091038e;		// Output rate of the UBX-MON-SPAN message on port USB
const uint32_t CFG_MSGOUT_UBX_MON_TXBUF_I2C = 0x2091019b;			// Output rate of the UBX-MON-TXBUF message on port I2C
const uint32_t CFG_MSGOUT_UBX_MON_TXBUF_SPI = 0x2091019f;			// Output rate of the UBX-MON-TXBUF message on port SPI
const uint32_t CFG_MSGOUT_UBX_MON_TXBUF_UART1 = 0x2091019c;		// Output rate of the UBX-MON-TXBUF message on port UART1
const uint32_t CFG_MSGOUT_UBX_MON_TXBUF_UART2 = 0x2091019d;	// Output rate of the UBX-MON-TXBUF message on port UART2
const uint32_t CFG_MSGOUT_UBX_MON_TXBUF_USB = 0x2091019e;		// Output rate of the UBX-MON-TXBUF message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_CLOCK_I2C = 0x20910065;			// Output rate of the UBX-NAV-CLOCK message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_CLOCK_SPI = 0x20910069;			// Output rate of the UBX-NAV-CLOCK message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_CLOCK_UART1 = 0x20910066;		// Output rate of the UBX-NAV-CLOCK message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_CLOCK_UART2 = 0x20910067;	// Output rate of the UBX-NAV-CLOCK message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_CLOCK_USB = 0x20910068;		// Output rate of the UBX-NAV-CLOCK message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_DOP_I2C = 0x20910038;				// Output rate of the UBX-NAV-DOP message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_DOP_SPI = 0x2091003c;				// Output rate of the UBX-NAV-DOP message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_DOP_UART1 = 0x20910039;			// Output rate of the UBX-NAV-DOP message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_DOP_UART2 = 0x2091003a;			// Output rate of the UBX-NAV-DOP message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_DOP_USB = 0x2091003b;				// Output rate of the UBX-NAV-DOP message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_EOE_I2C = 0x2091015f;				// Output rate of the UBX-NAV-EOE message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_EOE_SPI = 0x20910163;				// Output rate of the UBX-NAV-EOE message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_EOE_UART1 = 0x20910160;			// Output rate of the UBX-NAV-EOE message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_EOE_UART2 = 0x20910161;			// Output rate of the UBX-NAV-EOE message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_EOE_USB = 0x20910162;				// Output rate of the UBX-NAV-EOE message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_GEOFENCE_I2C = 0x209100a1;		// Output rate of the UBX-NAV-GEOFENCE message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_GEOFENCE_SPI = 0x209100a5;	// Output rate of the UBX-NAV-GEOFENCE message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_GEOFENCE_UART1 = 0x209100a2;// Output rate of the UBX-NAV-GEOFENCE message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_GEOFENCE_UART2 = 0x209100a3;// Output rate of the UBX-NAV-GEOFENCE message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_GEOFENCE_USB = 0x209100a4;	// Output rate of the UBX-NAV-GEOFENCE message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSECEF_I2C = 0x2091002e;// Output rate of the UBX-NAV-HPPOSECEF message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSECEF_SPI = 0x20910032;// Output rate of the UBX-NAV-HPPOSECEF message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSECEF_UART1 = 0x2091002f;// Output rate of the UBX-NAV-HPPOSECEF message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSECEF_UART2 = 0x20910030;// Output rate of the UBX-NAV-HPPOSECEF message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSECEF_USB = 0x20910031;// Output rate of the UBX-NAV-HPPOSECEF message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSLLH_I2C = 0x20910033;	// Output rate of the UBX-NAV-HPPOSLLH message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSLLH_SPI = 0x20910037;	// Output rate of the UBX-NAV-HPPOSLLH message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSLLH_UART1 = 0x20910034;// Output rate of the UBX-NAV-HPPOSLLH message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSLLH_UART2 = 0x20910035;// Output rate of the UBX-NAV-HPPOSLLH message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_HPPOSLLH_USB = 0x20910036;	// Output rate of the UBX-NAV-HPPOSLLH message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_ODO_I2C = 0x2091007e;			// Output rate of the UBX-NAV-ODO message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_ODO_SPI = 0x20910082;				// Output rate of the UBX-NAV-ODO message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_ODO_UART1 = 0x2091007f;			// Output rate of the UBX-NAV-ODO message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_ODO_UART2 = 0x20910080;			// Output rate of the UBX-NAV-ODO message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_ODO_USB = 0x20910081;				// Output rate of the UBX-NAV-ODO message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_ORB_I2C = 0x20910010;				// Output rate of the UBX-NAV-ORB message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_ORB_SPI = 0x20910014;				// Output rate of the UBX-NAV-ORB message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_ORB_UART1 = 0x20910011;			// Output rate of the UBX-NAV-ORB message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_ORB_UART2 = 0x20910012;			// Output rate of the UBX-NAV-ORB message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_ORB_USB = 0x20910013;				// Output rate of the UBX-NAV-ORB message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_POSECEF_I2C = 0x20910024;		// Output rate of the UBX-NAV-POSECEF message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_POSECEF_SPI = 0x20910028;	// Output rate of the UBX-NAV-POSECEF message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_POSECEF_UART1 = 0x20910025;// Output rate of the UBX-NAV-POSECEF message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_POSECEF_UART2 = 0x20910026;// Output rate of the UBX-NAV-POSECEF message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_POSECEF_USB = 0x20910027;	// Output rate of the UBX-NAV-POSECEF message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_POSLLH_I2C = 0x20910029;	// Output rate of the UBX-NAV-POSLLH message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_POSLLH_SPI = 0x2091002d;	// Output rate of the UBX-NAV-POSLLH message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_POSLLH_UART1 = 0x2091002a;	// Output rate of the UBX-NAV-POSLLH message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_POSLLH_UART2 = 0x2091002b;	// Output rate of the UBX-NAV-POSLLH message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_POSLLH_USB = 0x2091002c;	// Output rate of the UBX-NAV-POSLLH message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_PVT_I2C = 0x20910006;			// Output rate of the UBX-NAV-PVT message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_PVT_SPI = 0x2091000a;				// Output rate of the UBX-NAV-PVT message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_PVT_UART1 = 0x20910007;			// Output rate of the UBX-NAV-PVT message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_PVT_UART2 = 0x20910008;			// Output rate of the UBX-NAV-PVT message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_PVT_USB = 0x20910009;				// Output rate of the UBX-NAV-PVT message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_RELPOSNED_I2C = 0x2091008d;	// Output rate of the UBX-NAV-RELPOSNED message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_RELPOSNED_SPI = 0x20910091;// Output rate of the UBX-NAV-RELPOSNED message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_RELPOSNED_UART1 = 0x2091008e;// Output rate of the UBX-NAV-RELPOSNED message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_RELPOSNED_UART2 = 0x2091008f;// Output rate of the UBX-NAV-RELPOSNED message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_RELPOSNED_USB = 0x20910090;// Output rate of the UBX-NAV-RELPOSNED message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_SAT_I2C = 0x20910015;			// Output rate of the UBX-NAV-SAT message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_SAT_SPI = 0x20910019;				// Output rate of the UBX-NAV-SAT message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_SAT_UART1 = 0x20910016;			// Output rate of the UBX-NAV-SAT message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_SAT_UART2 = 0x20910017;			// Output rate of the UBX-NAV-SAT message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_SAT_USB = 0x20910018;				// Output rate of the UBX-NAV-SAT message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_SBAS_I2C = 0x2091006a;			// Output rate of the UBX-NAV-SBAS message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_SBAS_SPI = 0x2091006e;			// Output rate of the UBX-NAV-SBAS message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_SBAS_UART1 = 0x2091006b;		// Output rate of the UBX-NAV-SBAS message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_SBAS_UART2 = 0x2091006c;	// Output rate of the UBX-NAV-SBAS message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_SBAS_USB = 0x2091006d;		// Output rate of the UBX-NAV-SBAS message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_SIG_I2C = 0x20910345;				// Output rate of the UBX-NAV-SIG message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_SIG_SPI = 0x20910349;				// Output rate of the UBX-NAV-SIG message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_SIG_UART1 = 0x20910346;			// Output rate of the UBX-NAV-SIG message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_SIG_UART2 = 0x20910347;			// Output rate of the UBX-NAV-SIG message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_SIG_USB = 0x20910348;				// Output rate of the UBX-NAV-SIG message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_SLAS_I2C = 0x20910336;			// Output rate of the UBX-NAV-SLAS message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_SLAS_SPI = 0x2091033a;			// Output rate of the UBX-NAV-SLAS message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_SLAS_UART1 = 0x20910337;		// Output rate of the UBX-NAV-SLAS message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_SLAS_UART2 = 0x20910338;	// Output rate of the UBX-NAV-SLAS message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_SLAS_USB = 0x20910339;		// Output rate of the UBX-NAV-SLAS message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_STATUS_I2C = 0x2091001a;		// Output rate of the UBX-NAV-STATUS message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_STATUS_SPI = 0x2091001e;	// Output rate of the UBX-NAV-STATUS message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_STATUS_UART1 = 0x2091001b;	// Output rate of the UBX-NAV-STATUS message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_STATUS_UART2 = 0x2091001c;	// Output rate of the UBX-NAV-STATUS message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_STATUS_USB = 0x2091001d;	// Output rate of the UBX-NAV-STATUS message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_SVIN_I2C = 0x20910088;		// Output rate of the UBX-NAV-SVIN message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_SVIN_SPI = 0x2091008c;			// Output rate of the UBX-NAV-SVIN message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_SVIN_UART1 = 0x20910089;		// Output rate of the UBX-NAV-SVIN message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_SVIN_UART2 = 0x2091008a;	// Output rate of the UBX-NAV-SVIN message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_SVIN_USB = 0x2091008b;		// Output rate of the UBX-NAV-SVIN message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEBDS_I2C = 0x20910051;		// Output rate of the UBX-NAV-TIMEBDS message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEBDS_SPI = 0x20910055;	// Output rate of the UBX-NAV-TIMEBDS message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEBDS_UART1 = 0x20910052;// Output rate of the UBX-NAV-TIMEBDS message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEBDS_UART2 = 0x20910053;// Output rate of the UBX-NAV-TIMEBDS message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEBDS_USB = 0x20910054;	// Output rate of the UBX-NAV-TIMEBDS message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGAL_I2C = 0x20910056;	// Output rate of the UBX-NAV-TIMEGAL message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGAL_SPI = 0x2091005a;	// Output rate of the UBX-NAV-TIMEGAL message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGAL_UART1 = 0x20910057;// Output rate of the UBX-NAV-TIMEGAL message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGAL_UART2 = 0x20910058;// Output rate of the UBX-NAV-TIMEGAL message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGAL_USB = 0x20910059;	// Output rate of the UBX-NAV-TIMEGAL message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGLO_I2C = 0x2091004c;	// Output rate of the UBX-NAV-TIMEGLO message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGLO_SPI = 0x20910050;	// Output rate of the UBX-NAV-TIMEGLO message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGLO_UART1 = 0x2091004d;// Output rate of the UBX-NAV-TIMEGLO message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGLO_UART2 = 0x2091004e;// Output rate of the UBX-NAV-TIMEGLO message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGLO_USB = 0x2091004f;	// Output rate of the UBX-NAV-TIMEGLO message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGPS_I2C = 0x20910047;	// Output rate of the UBX-NAV-TIMEGPS message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGPS_SPI = 0x2091004b;	// Output rate of the UBX-NAV-TIMEGPS message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGPS_UART1 = 0x20910048;// Output rate of the UBX-NAV-TIMEGPS message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGPS_UART2 = 0x20910049;// Output rate of the UBX-NAV-TIMEGPS message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEGPS_USB = 0x2091004a;	// Output rate of the UBX-NAV-TIMEGPS message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMELS_I2C = 0x20910060;	// Output rate of the UBX-NAV-TIMELS message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMELS_SPI = 0x20910064;	// Output rate of the UBX-NAV-TIMELS message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMELS_UART1 = 0x20910061;	// Output rate of the UBX-NAV-TIMELS message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMELS_UART2 = 0x20910062;	// Output rate of the UBX-NAV-TIMELS message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMELS_USB = 0x20910063;	// Output rate of the UBX-NAV-TIMELS message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEQZSS_I2C = 0x20910386;	// Output rate of the UBX-NAV-TIMEQZSSmessage on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEQZSS_SPI = 0x2091038a;	// Output rate of the UBX-NAV-TIMEQZSSmessage on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEQZSS_UART1 = 0x20910387;// Output rate of the UBX-NAV-TIMEQZSSmessage on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEQZSS_UART2 = 0x20910388;// Output rate of the UBX-NAV-TIMEQZSSmessage on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEQZSS_USB = 0x20910389;	// Output rate of the UBX-NAV-TIMEQZSSmessage on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEUTC_I2C = 0x2091005b;	// Output rate of the UBX-NAV-TIMEUTC message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEUTC_SPI = 0x2091005f;	// Output rate of the UBX-NAV-TIMEUTC message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEUTC_UART1 = 0x2091005c;// Output rate of the UBX-NAV-TIMEUTC message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEUTC_UART2 = 0x2091005d;// Output rate of the UBX-NAV-TIMEUTC message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_TIMEUTC_USB = 0x2091005e;	// Output rate of the UBX-NAV-TIMEUTC message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_VELECEF_I2C = 0x2091003d;	// Output rate of the UBX-NAV-VELECEF message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_VELECEF_SPI = 0x20910041;	// Output rate of the UBX-NAV-VELECEF message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_VELECEF_UART1 = 0x2091003e;// Output rate of the UBX-NAV-VELECEF message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_VELECEF_UART2 = 0x2091003f;// Output rate of the UBX-NAV-VELECEF message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_VELECEF_USB = 0x20910040;	// Output rate of the UBX-NAV-VELECEF message on port USB
const uint32_t CFG_MSGOUT_UBX_NAV_VELNED_I2C = 0x20910042;	// Output rate of the UBX-NAV-VELNED message on port I2C
const uint32_t CFG_MSGOUT_UBX_NAV_VELNED_SPI = 0x20910046;	// Output rate of the UBX-NAV-VELNED message on port SPI
const uint32_t CFG_MSGOUT_UBX_NAV_VELNED_UART1 = 0x20910043;	// Output rate of the UBX-NAV-VELNED message on port UART1
const uint32_t CFG_MSGOUT_UBX_NAV_VELNED_UART2 = 0x20910044;	// Output rate of the UBX-NAV-VELNED message on port UART2
const uint32_t CFG_MSGOUT_UBX_NAV_VELNED_USB = 0x20910045;	// Output rate of the UBX-NAV-VELNED message on port USB
const uint32_t CFG_MSGOUT_UBX_RXM_MEASX_I2C = 0x20910204;		// Output rate of the UBX-RXM-MEASX message on port I2C
const uint32_t CFG_MSGOUT_UBX_RXM_MEASX_SPI = 0x20910208;			// Output rate of the UBX-RXM-MEASX message on port SPI
const uint32_t CFG_MSGOUT_UBX_RXM_MEASX_UART1 = 0x20910205;		// Output rate of the UBX-RXM-MEASX message on port UART1
const uint32_t CFG_MSGOUT_UBX_RXM_MEASX_UART2 = 0x20910206;	// Output rate of the UBX-RXM-MEASX message on port UART2
const uint32_t CFG_MSGOUT_UBX_RXM_MEASX_USB = 0x20910207;		// Output rate of the UBX-RXM-MEASX message on port USB
const uint32_t CFG_MSGOUT_UBX_RXM_RAWX_I2C = 0x209102a4;			// Output rate of the UBX-RXM-RAWX message on port I2C
const uint32_t CFG_MSGOUT_UBX_RXM_RAWX_SPI = 0x209102a8;			// Output rate of the UBX-RXM-RAWX message on port SPI
const uint32_t CFG_MSGOUT_UBX_RXM_RAWX_UART1 = 0x209102a5;		// Output rate of the UBX-RXM-RAWX message on port UART1
const uint32_t CFG_MSGOUT_UBX_RXM_RAWX_UART2 = 0x209102a6;	// Output rate of the UBX-RXM-RAWX message on port UART2
const uint32_t CFG_MSGOUT_UBX_RXM_RAWX_USB = 0x209102a7;		// Output rate of the UBX-RXM-RAWX message on port USB
const uint32_t CFG_MSGOUT_UBX_RXM_RLM_I2C = 0x2091025e;				// Output rate of the UBX-RXM-RLM message on port I2C
const uint32_t CFG_MSGOUT_UBX_RXM_RLM_SPI = 0x20910262;				// Output rate of the UBX-RXM-RLM message on port SPI
const uint32_t CFG_MSGOUT_UBX_RXM_RLM_UART1 = 0x2091025f;			// Output rate of the UBX-RXM-RLM message on port UART1
const uint32_t CFG_MSGOUT_UBX_RXM_RLM_UART2 = 0x20910260;			// Output rate of the UBX-RXM-RLM message on port UART2
const uint32_t CFG_MSGOUT_UBX_RXM_RLM_USB = 0x20910261;				// Output rate of the UBX-RXM-RLM message on port USB
const uint32_t CFG_MSGOUT_UBX_RXM_RTCM_I2C = 0x20910268;			// Output rate of the UBX-RXM-RTCM message on port I2C
const uint32_t CFG_MSGOUT_UBX_RXM_RTCM_SPI = 0x2091026c;			// Output rate of the UBX-RXM-RTCM message on port SPI
const uint32_t CFG_MSGOUT_UBX_RXM_RTCM_UART1 = 0x20910269;			// Output rate of the UBX-RXM-RTCM message on port UART1
const uint32_t CFG_MSGOUT_UBX_RXM_RTCM_UART2 = 0x2091026a;		// Output rate of the UBX-RXM-RTCM message on port UART2
const uint32_t CFG_MSGOUT_UBX_RXM_RTCM_USB = 0x2091026b;		// Output rate of the UBX-RXM-RTCM message on port USB
const uint32_t CFG_MSGOUT_UBX_RXM_SFRBX_I2C = 0x20910231;			// Output rate of the UBX-RXM-SFRBX message on port I2C
const uint32_t CFG_MSGOUT_UBX_RXM_SFRBX_SPI = 0x20910235;			// Output rate of the UBX-RXM-SFRBX message on port SPI
const uint32_t CFG_MSGOUT_UBX_RXM_SFRBX_UART1 = 0x20910232;		// Output rate of the UBX-RXM-SFRBX message on port UART1
const uint32_t CFG_MSGOUT_UBX_RXM_SFRBX_UART2 = 0x20910233;	// Output rate of the UBX-RXM-SFRBX message on port UART2
const uint32_t CFG_MSGOUT_UBX_RXM_SFRBX_USB = 0x20910234;		// Output rate of the UBX-RXM-SFRBX message on port USB
const uint32_t CFG_MSGOUT_UBX_TIM_TM2_I2C = 0x20910178;				// Output rate of the UBX-TIM-TM2 message on port I2C
const uint32_t CFG_MSGOUT_UBX_TIM_TM2_SPI = 0x2091017c;				// Output rate of the UBX-TIM-TM2 message on port SPI
const uint32_t CFG_MSGOUT_UBX_TIM_TM2_UART1 = 0x20910179;			// Output rate of the UBX-TIM-TM2 message on port UART1
const uint32_t CFG_MSGOUT_UBX_TIM_TM2_UART2 = 0x2091017a;			// Output rate of the UBX-TIM-TM2 message on port UART2
const uint32_t CFG_MSGOUT_UBX_TIM_TM2_USB = 0x2091017b;				// Output rate of the UBX-TIM-TM2 message on port USB
const uint32_t CFG_MSGOUT_UBX_TIM_TP_I2C = 0x2091017d;				// Output rate of the UBX-TIM-TP message on port I2C
const uint32_t CFG_MSGOUT_UBX_TIM_TP_SPI = 0x20910181;				// Output rate of the UBX-TIM-TP message on port SPI
const uint32_t CFG_MSGOUT_UBX_TIM_TP_UART1 = 0x2091017e;			// Output rate of the UBX-TIM-TP message on port UART1
const uint32_t CFG_MSGOUT_UBX_TIM_TP_UART2 = 0x2091017f;			// Output rate of the UBX-TIM-TP message on port UART2
const uint32_t CFG_MSGOUT_UBX_TIM_TP_USB = 0x20910180;				// Output rate of the UBX-TIM-TP message on port USB
const uint32_t CFG_MSGOUT_UBX_TIM_VRFY_I2C = 0x20910092;			// Output rate of the UBX-TIM-VRFY message on port I2C
const uint32_t CFG_MSGOUT_UBX_TIM_VRFY_SPI = 0x20910096;			// Output rate of the UBX-TIM-VRFY message on port SPI
const uint32_t CFG_MSGOUT_UBX_TIM_VRFY_UART1 = 0x20910093;			// Output rate of the UBX-TIM-VRFY message on port UART1
const uint32_t CFG_MSGOUT_UBX_TIM_VRFY_UART2 = 0x20910094;		// Output rate of the UBX-TIM-VRFY message on port UART2
const uint32_t CFG_MSGOUT_UBX_TIM_VRFY_USB = 0x20910095;		// Output rate of the UBX-TIM-VRFY message on port USB
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#endif