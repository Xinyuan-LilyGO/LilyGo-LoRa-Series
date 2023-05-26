
# SX12XX Library

This Arduino LoRa library is I hope different to other libraries, this LoRa library has a specific emphasis on providing a large range of working examples for common applications.

There are the normal basic transmit and receive examples, but there are many additional real world examples for;

- Sending and receiving sensor data.
- Remote control of devices, turning LEDs on\off etc.
- GPS tracker transmitters and receivers with displays.
- Using joysticks and servos for remote control.
- Examples using sleep modes for low power consumption.
- High altitude balloon trackers.
- Transferring large data arrays or files.
- Sending and receiving images from ESP32CAMs.
- Sending and receiving packets where an acknowledge is required.
- Sending polling requests to remote nodes for the return of specific data.
- Link test programs to test LoRa devices and antennas.
- Packet loggers with displays.
- Transmitting data with FSK RTTY.
- Transmitting FM tones and playing tunes (Star Wars).

The coding style of the library functions is (I hope) kept simple so that its easy to understand and modify for custom applications.

There are the standard examples showing how to send and receive arrays of data and very easy to use examples that write and read data and variables direct into and from the LoRa devices internal buffer. Examples for sending and receiving structures too.

The library includes the concept of 'Reliable' packets where the library functions automatically append a payload CRC and network ID to each packet. This means the receiver can be very confident that the received packet is from a known source and in the correct sequence. Additionally the transmitter can require a valid acknowledge response from the receiver before it continues. A transmitter can send 'Reliable' poll messages so that only a specific node will respond. 

The reliable packets are used for the data transfer examples where LoRa can be used to reliably move images, files or arrays from one Arduino to another. These image or file transfers can also be forwarded from the Arduino to a folder on a PC via a local Serial YModem transfer. 

There are  examples for the distance measuring or ranging functions of the SX128X, tested up to 85km. Fast Long Range Communication (FLRC) packets are also  supported on the SX128X and these packets have an on air rate of up to 1,300,000bps.  

The Semtech LoRa devices are used to manufacture a range of LoRa modules sold by companies such as Hope, Dorji, NiceRF and others. The library does not support LoRa modules with a UART based interface such as those from Ebyte and Microchip.

<form action="https://www.paypal.com/donate" method="post" target="_top">
<input type="hidden" name="hosted_button_id" value="4EMBJU5C2UGNS" />
<input type="image" src="https://www.paypalobjects.com/en_US/GB/i/btn/btn_donateCC_LG.gif" border="0" name="submit" title="PayPal - The safer, easier way to pay online!" alt="Donate with PayPal button" />
<img alt="" border="0" src="https://www.paypal.com/en_GB/i/scr/pixel.gif" width="1" height="1" />
</form>


[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4EMBJU5C2UGNS)


## Supported Devices

The library does support the following Semtech LoRa devices; 

**SX1262,SX1262,SX1268,SX1272,SX1276,SX1277,SX1278,SX1279,SX1280,SX1281**

However, its up to module manufactures to connect the Semtech LoRa devices up as they see fit. There are several options and additional control pins that a module manufacturer can add, so just because a module uses for example an SX1262 it does not mean the library code can support that particular modules combination of pins. See the section 'Considerations for pin usage' below. 

A prime objective of the library was to allow the same program sketches to be used across the range of UHF LoRa modules (SX126x and SX127x) as well as the 2.4Ghz SX128x modules. With this library a sketch written for the SX1278 should run with very minor changes on the SX1262 or SX1280. However, whilst the SX126x and SX128x modules use the same style of internal device programming, the SX127x internal programming is completely different. The function style used for the SX126x and SX128x devices has been emulated for the SX127x.

### Warning

**The base Semtech devices that this library supports are all 3.3V logic level devices and most available modules do not include logic level conversion circuits.  So do not use modules directly with 5V logic level Arduinos unless some form of logic level conversion is implemented.** There are no specific logic level converters I could recommend. 

## SX12XX Library installation

To install the library select the 'Clone or download' button on the main Git hub page, then select 'Download Zip'. In the Arduino IDE select 'Sketch' then 'Include Library'. Next select 'Add .ZIP library' and browse to and select the ZIP file you downloaded, it's called 'SX12xx-master.zip'.

## Direct access to LoRa device internal data buffer

A conventional LoRa library normally uses a buffer of data created within the Arduino sketch to contain the data that is sent as a packet. This library has those basic functions, see the example programs 3 and 4 in the 'Basics' folder. There are examples for sending\receiving a simple character buffer ("Hello World") and for sending\receiving a data structure which can also include a character buffer.
 
An additional feature has been implemented for this library to enable variables or character data to be written direct to the LoRa devices internal buffer. This has the benefit of not requiring a memory buffer in the Arduino and also lends itself to a simple easy to understand way of sending and receiving packets. For instance this is the routine to create a packet for transmission taken from the /LowMemory examples folder;

	LT.startWriteSXBuffer(0);                         //start the write at location 0
	LT.writeBuffer(trackerID, sizeof(trackerID));     //= 13 bytes (12 characters plus null (0) at end)
	LT.writeUint32(TXpacketCount);                    //+4 = 17 bytes
	LT.writeFloat(latitude);                          //+4 = 21 bytes
	LT.writeFloat(longitude);                         //+4 = 25 bytes
	LT.writeUint16(altitude);                         //+2 = 27 bytes
	LT.writeUint8(satellites);                        //+1 = 28 bytes
	LT.writeUint16(voltage);                          //+2 = 30 bytes
	LT.writeInt8(temperature);                        //+1 = 31 bytes total to send
	len = LT.endWriteSXBuffer();

This is the matching code for the receiver;

	LT.startReadSXBuffer(0);               //start buffer read at location 0
	LT.readBuffer(receivebuffer);          //read in the character buffer
	txcount  = LT.readUint32();            //read in the TXCount
	latitude = LT.readFloat();             //read in the latitude
	longitude = LT.readFloat();            //read in the longitude
	altitude = LT.readUint16();            //read in the altitude
	satellites = LT.readUint8();           //read in the number of satellites
	voltage = LT.readUint16();             //read in the voltage
	temperature = LT.readInt8();           //read in the temperature
	RXPacketL = LT.endReadSXBuffer();
  

Clearly as with other methods of sending data the order in which the packet data is created in the transmitter has to match the order that it is read in the receiver.

## Considerations for pin usage

Pins settings and usage must be set up either at the head of a program or in the the the 'Settings.h' file that is include in some sketch folders. Program **2\_Register\_Test** is an example of a sketch that does not use a 'Settings.h' file. 
The library supports the SPI based LoRa modules and these all require that the SPI bus pins, SCK, MOSI and MISO are connected. All modules also need a NSS (chip select pin) and NRESET (reset) pin. All SX126X and SX128X devices need the RFBUSY pin to be used also. 

Of the LoRa devices DIO pins the library in standard form only uses DIO0 (SX127X) and DIO1 (SX126X and SX128X). The pin definitions for DIO1 and DIO2 (SX127x) and DIO2 and DIO3 (SX126x and SX128x) are not currently used by the library or examples so can be defined as -1 meaning they will not be configured. 

### Reduced pin usage

There are 'IRQ' library functions that do not need the use of the DIO0 (SX127X) or DIO1 (SX126X and SX128X) pins. These functions read the LoRa devices IRQ registers to check for transmit or receive completion. See examples such as **3\_LoRa\_TransmitterIRQ**. On the SX127x devices you can normally leave the NRESET pin floating, the device clears itself at power on. This can save another IO pin. There are begin() methods for these combinations.

The Dorji DRF1262 and DRF1268 modules have an SW pin which must be connected, it provides power to the antenna switch used on these modules. 

### SX126x modules with RX or TX enable pins

Some SX126x modules may have RX or TX enable pins, Ebyte E22 modules for instance. Whilst there is code in the SX126X library that was copied across from the SX128X library there were no SX126X devices using RX and TX switching available at the time this library was published, so these functions are currently not supported by the library as they have not been tested. If a user has SX126X modules that they want tested with the library, then donations of modules will be accepted.  

### SX128x modules with RX or TX enable pins

Some of the SX128x modules may also have RX or TX enable pins, such as the Ebyte E28 modules. These functions have been tested and are supported for SX128x devices. You need to define the pins RX\_EN and TX\_EN pins used, otherwise leave unused by defining them as -1.  

## Testing of library and examples

For testing the library and the example programs I used a board of my own design, which uses a 3.3V/8Mhz Arduino Pro Mini and has a socket where you can plug in a LoRa device as a Mikrobus style module. The board is small enough to be used for a GPS tracker application using the end connections for a GPS and display. See the [**Easy Pro Mini**](https://github.com/StuartsProjects/Devices/tree/master/Easy%20Pro%20Mini) folder for details. 
<br>
  

## Testing a new setup

A good first example program to test a layout and connections would be the example program in the Basics folder **2\_Register_Test**, this just does a simple register print of the LoRa device. If this program does not work, then the rest of the example programs wont either. This program is self contained, it does not need the library installed to operate. 

With an example program written and tested on this SX127x library the example should work with some minor changes with the SX126x and SX128x devices. Many of the example programs have already been tested and are working on SX126x, conversion typically takes less than a minute. 

The Settings.h file in some examples contains the settings for the LoRa device such as frequency, spreading factor, bandwidth and coding rate.  The example programs use a frequency of 434.000Mhz or 2.445Ghz for the SX128x, you will need to check if that frequency is permitted in your part of the World. The radio frequency spectrum is not a free for all, which frequencies, transmitter powers and duty cycles you are permitted to use varies by region and country. By default CRC checking is added to transmitted packets and used to check for errors on reception.

### External libraries in examples

Some of the example sketches, in particular the camera and file transfer sketches use additional Arduino libraries. Some of these such as the SD libraries are part of the Arduino IDE, others you have to download and install. Most of these additional libraries are reliable, but some, such as the SD libraries may not be with some combinations of hardware. 

## Library Functions

All of the library functions are public and can be accessed from users sketches. 

The basic functions will be described in the order that example program **3\_LoRa\_Transmitter** uses them.

**SPI.begin()**

Standard Arduino library function. Sets up SPI. The library then internally uses;

**SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode))**

before every use of the SPI and this function after it;

**SPI.endTransaction()**

The parameters used are LTspeedMaximum, LTdataOrder and LTdataMode are defined in SX12XXLT_Definitions.h as;


	LTspeedMaximum  8000000
	LTdataOrder     MSBFIRST
	LTdataMode      SPI_MODE0

The use of SPI.beginTransaction and SPI.endTransaction can be disabled by commenting out this define at the top of the relevant SX12XXLT.cpp file;

	#define USE_SPI_TRANSACTION        


**begin(NSS, NRESET, DIO0, LORA\_DEVICE)** (SX127X library)

**begin(NSS, NRESET, RFBUSY, DIO1, LORA\_DEVICE)**     (SX126X library)

**begin(NSS, NRESET, RFBUSY, DIO1, LORA\_DEVICE)**     (SX128X library)

The basic begin functions initialises the minimal hardware pins used by the device. The begin functions are slightly different for the SX127X, SX126X and SX128X libraries due to the different pins used. NSS, NRESET and DIO0 (SX127X) or DIO1 (SX126x and SX128X) are required. 

Other LoRa device pins can be used if you choose and the full list of begin functions is;


	SX126X
	bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinRXEN, int8_t pinTXEN, int8_t pinSW, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinSW, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, uint8_t device);

	SX127X    
	bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, int8_t pinDIO1, int8_t pinDIO2, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, uint8_t device);
    bool begin(int8_t pinNSS, uint8_t device);

	SX128X
	bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinRXEN, int8_t pinTXEN, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, uint8_t device);
    bool begin(int8_t pinNSS, int8_t pinRFBUSY, uint8_t device);

 Some DIOs are optional and when not used define as -1. The SX126X and SX128X devices have an RFBUSY pin. To ensure compatibility with Dorji SX1262 and SX1268 devices the SW pin needs to be defined. This pin turns on\off the antenna switch on Dorji devices. Set to -1 if not used. Some of the SX128X devices for example from eByte require TX and RX enable pins, set to -1 if your not using them.

**LoRA\_DEVICE** tells the library which actual LoRa RF IC is being used and in the case of the SX127x devices how the antenna is connected. The choices are for the SX127x part of the library are;

	DEVICE_SX1272
	DEVICE_SX1276
	DEVICE_SX1277
	DEVICE_SX1278
	DEVICE_SX1279

	DEVICE_SX1272_PABOOST
	DEVICE_SX1277_PABOOST
	DEVICE_SX1278_PABOOST
    DEVICE_SX1279_PABOOST

	DEVICE_SX1276_RFO
	DEVICE_SX1277_RFO
	DEVICE_SX1278_RFO
	DEVICE_SX1279_RFO


Note that in the above list for the SX127x part of the library only and there are new device types that allow the library to differentiate between devices that have the transmit output connected to the **PA\_BOOST** pin (which is the vast majority of SX127x devices) and those that use the **RFO LF\_ANT** or **RFO HF\_ANT** pins for RF output. These RFO connected devices are in the minority and are limited to 14dBm output. There is no library support for a SX1272 using the RFO outputs as I don't have one to test.

The old device types; DEVICE\_SX1272, DEVICE\_SX1276, DEVICE\_SX1277, DEVICE\_SX1278 and DEVICE\_SX1272 are retained for compatibility with old sketches and are the same device as those with the \_PABOOST identity.

Devices using the PABOOST output can be set to power levels from 2dBm to 20dBm, and operation at 20dBm is limited to a 1% duty cycle. Devices using the RFO outputs can be set to power levels from 0dBm to 14dBm.  

The device types for the SX126X and SX128X part of the library are;

	DEVICE_SX1261              //SX126X library
    DEVICE_SX1262              //SX126X library
    DEVICE_SX1268              //SX126X library
    DEVICE_SX1280              //SX128X library
    DEVICE_SX1281              //SX128X library

**setMode(MODE\_STDBY\_RC)**

Sets the operation mode of the LoRa device. Choices are;

	MODE_SLEEP
	MODE_STDBY
	MODE_STDBY_RC

**setPacketType(PACKET\_TYPE\_LORA)**

Set the type of packet to use, currently only LORA is supported, choices are;

	PACKET_TYPE_LORA
	PACKET_TYPE_GFSK     (not yet implemented)
	PACKET_TYPE_FLRC     (SX128X library only)
    PACKET_TYPE_RANGING  (SX128X library only)
    PACKET_TYPE_BLE      (SX128X library only) (not yet implemented)
    PACKET_TYPE_NONE

**setRfFrequency(Frequency, Offset)**

Sets the operating frequency in hertz. A calibration offset also in hertz can be used if there is a calibration value known for a particular module.

**calibrateImage(0)**

Carries out an internal device calibration, normally carried out after setting the initial operating frequency. 

**setModulationParams(SpreadingFactor, Bandwidth, CodeRate, LDRO_AUTO)**

Sets the LoRa modem parameters for Spreading factor, Bandwidth, CodeRate and Optimisation. The options are; 

    //LoRa Spreading factors
    LORA_SF5    (SX126X and SX128X libraries only)   
    LORA_SF6
    LORA_SF7
    LORA_SF8
    LORA_SF9
    LORA_SF10
    LORA_SF11
    LORA_SF12 
    
    //LoRa Bandwidths SX127X and SX126X libraries
    LORA_BW_500  //actual 500000hz
    LORA_BW_250  //actual 250000hz
    LORA_BW_125  //actual 125000hz
    LORA_BW_062  //actual  62500hz 
    LORA_BW_041  //actual  41670hz
    LORA_BW_031  //actual  31250hz 
    LORA_BW_020  //actual  20830hz
    LORA_BW_015  //actual  15630hz
    LORA_BW_010  //actual  10420hz 
    LORA_BW_007  //actual   7810hz

    //LoRa Bandwidths SX128X library
	LORA_BW_0200  //actually 203125hz
	LORA_BW_0400  //actually 406250hz
	LORA_BW_0800  //actually 812500hz
	LORA_BW_1600  //actually 1625000hz

    //LoRa Coding rates
    LORA_CR_4_5  
    LORA_CR_4_6  
    LORA_CR_4_7  
    LORA_CR_4_8

The SX126X and SX127X devices have an low data rate optimisation setting that needs to be set when the symbol time is greater than 16mS. You can manually turn it on or off or set it to LDRO\_AUTO and the library does the calculation for you.

    //Low date rate optimisation, need to be set when symbol time > 16mS
    LDRO_OFF
    LDRO_ON
    LDRO_AUTO       //automatically calculated and set 

**setBufferBaseAddress(0x00, 0x00)**

This sets the default address for the locations in the LoRa device buffer where transmitted and received packets start. The defaults of these locations are set in the transmit and receive functions, so this function is not normally required. 
                   
**setPacketParams(PreAmblelength, LORA\_PACKET\_VARIABLE\_LENGTH, PacketLength, LORA\_CRC\_ON, LORA\_IQ\_NORMAL)**

Set the packet parameters. PreAmblelength is normally 8. There is a choice of LORA\_PACKET\_VARIABLE\_LENGTH for variable length explicit packets or LORA\_PACKET\_FIXED\_LENGTH for implicit packets. PacketLength is 1 to 255, it can be set here but is normally handled within the transmitter and receiver functions. There is the option of using a packet CRC with LORA\_CRC\_ON or not using a CRC with LORA\_CRC\_OFF. IQ can be set to LORA\_IQ\_NORMAL or LORA\_IQ\_INVERTED.

**setSyncWord(LORA\_MAC\_PRIVATE\_SYNCWORD)**

You can define the syncword here, either a 8 bit value of your own choice or the standard values of LORA\_MAC\_PRIVATE\_SYNCWORD (0x12) or LORA\_MAC\_PUBLIC\_SYNCWORD (0x34). Take great care with setting your own syncwords, some values will not work and may not be compatible with other LoRa devices or can give reduced sensitivity. Best to stick to the standard PUBLIC and PRIVATE ones. There is no configurable syncword for SX128x devices.

**setHighSensitivity()**

Sets LoRa device for the highest sensitivity at expense of slightly higher LNA current. The alternative is setLowPowerReceive() for lower sensitivity with slightly reduced current. 


**setDioIrqParams(MASK, DIO0\_MASK, DIO1\_MASK, DIO2\_MASK)** (SX127X library)

**setDioIrqParams(MASK, DIO0\_MASK, DIO1\_MASK, DIO2\_MASK)** (SX126X and SX128X libraries)

Sets up the how the device responds to internal events. This function is written for the SX127X to match the style used by the SX126X and SX128X devices. MASK is applied to the IRQ settings for DIO0, DIO1 and DIO2 (SX127X) and DIO1, DIO2 and DIO3 (SX126X and SX128X), its normally set to IRQ\_RADIO\_ALL (0xFFFF). Whilst the SX127X only has an 8 bit IRQ register the library has extended the function to provide additional IRQ detections that are used for the SX126X and SX127X. 

In the case of the SX127X, the function maps the internal interrupts to the DIO0, DIO1 and DIO2 pins according to this table;

	IRQ_RADIO_NONE              0x00
	IRQ_CAD_ACTIVITY_DETECTED   0x01       //active on DIO1 
	IRQ_FSHS_CHANGE_CHANNEL     0x02       //active on DIO2 
	IRQ_CAD_DONE                0x04       //active on DIO0 
	IRQ_TX_DONE                 0x08       //active on DIO0 
	IRQ_HEADER_VALID            0x10       //read from IRQ register only
	IRQ_CRC_ERROR               0x20       //read from IRQ register only
	IRQ_RX_DONE                 0x40       //active on DIO0 
	IRQ_RADIO_ALL               0xFFFF

	IRQ_TX_TIMEOUT              0x0100     //so that readIrqstatus can return additional detections 
	IRQ_RX_TIMEOUT              0x0200     //so that readIrqstatus can return additional detections  
	IRQ_NO_PACKET_CRC           0x0400     //so that readIrqstatus can return additional detections 

The SX126X library has this table;

	IRQ_RADIO_NONE                       0x0000
	IRQ_TX_DONE                          0x0001
	IRQ_RX_DONE                          0x0002
	IRQ_PREAMBLE_DETECTED                0x0004
	IRQ_SYNCWORD_VALID                   0x0008
	IRQ_HEADER_VALID                     0x0010
	IRQ_HEADER_ERROR                     0x0020
	IRQ_CRC_ERROR                        0x0040
	IRQ_CAD_DONE                         0x0080

	IRQ_CAD_ACTIVITY_DETECTED            0x0100
	IRQ_RX_TX_TIMEOUT                    0x0200
	IRQ_TX_TIMEOUT                       0x0200
	IRQ_RX_TIMEOUT                       0x0200
	IRQ_RADIO_ALL                        0xFFFF

And the SX128X has this one;

	IRQ_RADIO_NONE                       0x0000
	IRQ_TX_DONE                          0x0001
	IRQ_RX_DONE                          0x0002
	IRQ_SYNCWORD_VALID                   0x0004
	IRQ_SYNCWORD_ERROR                   0x0008
	IRQ_HEADER_VALID                     0x0010
	IRQ_HEADER_ERROR                     0x0020
	IRQ_CRC_ERROR                        0x0040
	IRQ_RANGING_SLAVE_RESPONSE_DONE      0x0080

	IRQ_RANGING_SLAVE_REQUEST_DISCARDED  0x0100
	IRQ_RANGING_MASTER_RESULT_VALID      0x0200
	IRQ_RANGING_MASTER_RESULT_TIMEOUT    0x0400
	IRQ_RANGING_SLAVE_REQUEST_VALID      0x0800
	IRQ_CAD_DONE                         0x1000
	IRQ_CAD_ACTIVITY_DETECTED            0x2000
	IRQ_RX_TX_TIMEOUT                    0x4000
	IRQ_TX_TIMEOUT                       0x4000
	IRQ_RX_TIMEOUT                       0x4000
	IRQ_PREAMBLE_DETECTED                0x8000
	IRQ_RADIO_ALL                        0xFFFF 


**setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation)**

As an alternative to setting up the LoRa device with separate functions (as described above) you can use this function. The function first sets the Frequency of operation, the frequency is in hertz as a 32 bit unsigned integer. The actual programmed operating frequency is the sum of Frequency and Offset (also 32 bit integer).

SpreadingFactor, Bandwidth and CodeRate are the LoRa modem parameters and the choices are as given for the setModulationParams() described above.

When using setupLoRa() that library function then calls the following functions using these defaults;

	setMode(MODE_STDBY_RC)
	setPacketType(PACKET_TYPE_LORA);
	setRfFrequency(Frequency, Offset);
	calibrateImage(0);
	setModulationParams(SpreadingFactor, Bandwidth, CodeRate, LDRO_AUTO);
	setBufferBaseAddress(0x00, 0x00);
	setPacketParams(8, LORA_PACKET_VARIABLE_LENGTH, 255, LORA_CRC_ON, LORA_IQ_NORMAL);
	setSyncWord(LORA_MAC_PRIVATE_SYNCWORD);
	LORA_MAC_PUBLIC_SYNCWORD = 0x34
	setHighSensitivity();
	setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);
 

**printModemSettings()**

Prints the current modem settings to the serial monitor for diagnostic purposes. The parameters printed for lora are ; device\_type, frequency, spreading factor, bandwidth, coding\_rate, syncword, IQ\_Status, preamble\_length.

**printOperatingSettings()**

Prints the current operating settings to the serial monitor for diagnostic purposes. The settings printed are; device\_type, version\_number, packet\_mode, header\_mode, packet\_CRC\_on\_off, AGC\_auto\_on\_off, LNA\_gain, LNA\_boostHF, LNA\_boostLF.

**printRegisters(start, end)** 

Print the device registers from start address to end address. 

**printASCIIPacket(buff, length)**

Print as ASCII characters to the serial monitor the contents of the buffer name given, for the given length.

**transmit(buff, TXPacketL, timeout, TXpower, WAIT_TX)**

Transmit the contents of the buffer name given, for the given length. With a timeout in mS, with a TXpower in dBm and wait for the transmit to complete (a blocking command). To have the LoRa device start transmitting and continue as a no blocking command use NO\_WAIT. 

With **transmit** and WAIT\_TX the function returns the packet length if transmit detected no errors and 0 if errors were detected. If NO\_WAIT is used you will need to check when pin DIO0 (SX127X) or DIO1 (SX126X and SX128X) goes high indicating transmission is completed.    

**CRCCCITT(buff, TXPacketL, 0xFFFF)**

Calculates the 16 bit CRC of the buffer given for the length given. Specify the initialise value for the CRC, 0xFFFF in this case. 

**readIrqStatus()** 

Reads the value of the IRQ register flags. 

## Receiving Packets

The **4\_LoRa\_Receiver** sketch is very similar, with the following differences;

**LT.receive(RXBUFFER, RXBUFFER\_SIZE, timeout, WAIT\_RX)**

Copy the received packet into the buffer address given with a maximum buffer size. If the RXBUFFER\_SIZE is smaller than the actual received packet then the packet will be truncated. If WAIT\_RX is selected then the program will wait for the time-out period (in mS) for a packet to arrive before signalling a timeout, this is a blocking command. To have the receiver wait continuously for a packet set the timeout to 0. To use the receiver in non-blocking mode set NO\_WAIT in which case you will need to check DIO0 (SX127X) or DIO1 (SX126X and SX128X) going high to indicate a packet has arrived. 

**readPacketRSSI()** 

Read the signal strength in dBm of the received packet. 

**readPacketSNR()**

Read the signal to noise ratio in dB of the received packet. Typical values are +10dB for strong signals and -20dB for reception at the limit.  

**printIrqStatus()**

Prints to the serial monitor the interrupt flags set.


<br>
 

## Packet Addressing

LoRa is a two way technology, each device is a transceiver. Most often on a particular frequency there will be one transmitter and one receiver. However, this may not always be the case and there could be several nodes in use on the same frequency. 

In order to keep the software simple and allow for the receipt of signals from multiple receivers or directed commands to a particular node, a basic addressing scheme can be used and is implemented by some example programs, see **17\_Sensor\_Transmitter** for an example. There are library routines to send and receive packets in addressed and non-addressed format so you choose which to send. When using addressed mode regardless of the data content of the actual payload each packet sent has 3 control bytes at the beginning of the packet. In the case of the sensor example mentioned above, the use of the addressing allows the receiver to know from which sensor transmitter the packet came. 

In general the control bytes have been restricted to ASCII printable characters so that they can be shown directly on a terminal monitor. The 3 bytes are;

**Packet type**. This either describes the content of the packet, which could be a GPS location payload or is a command to do something and there is no payload. Details of the packet types defined are in the library file **ProgramLT\_Definitions.h**

**Packet Destination**. The node number that the packet is destined for.

**Packet Source**. The node number that the packet was sent from.

The destination and source packet bytes mean that node ‘2’ (could be your base station receiver) can send a command that only station ‘3’ will respond to. This command could be a reset command, a request to turn on and off certain transmission types etc. Node ‘3’ can be set-up so that it only accepts commands from a particular node.

In addressed mode the 3 control bytes are automatically stripped from each received packet.

An example of the 3 control bytes from a tracker would be;

T*2

Which means there is a test packet (T) its been sent as a broadcast (*) and its from node 2.

## Compatibility

**Library fully tested on 3.3V 8Mhz ATMega328P and ATMega1284P only**.
 
It was not the intention of this library to specifically support the large number of non-Atmel platforms. The use of platform specific hardware functions such as timers or interrupts has been avoided in the libraries, so if a hardware platform supports standard SPI and a program compiles, it should work. There are library functions that use the millis() and micros() functions. Some example programs use SoftwareSerial which is not supported on all platforms. A few programs have been tested and work on ESP32 based boards and an STM32 Xnucleo board. See the Readme for ESP32 and STM32 in the ESP32 and STM32 examples folders.


## Support

The examples do work, so if for you they do not, assume there is a problem with how you have wired the modules or that your modules are faulty or that your Arduino set-up or LoRa module is faulty or unsupported. You are best placed to diagnose these issues. **It is not practical for me to provide on-going technical support for programs that are not the library examples included.** This also applies to external  libraries used in examples, if you have problems with these libraries, contact the authors for support. 

If you find a bug, or other error in the SX12xx library or examples, and your using the ATMega328P and ATMega1284P platforms please let me know.



<br>



### Stuart Robinson

### January 2023

