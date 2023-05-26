## ESP32 - SX12XX Library Example Programs

Having originally tested the SX127X part of the library on a ATMega328P based micro controller I decided to check that the library was code compatible with another popular processor that can be programmed in the Arduino IDE, the ESP32. 

Unfortunately there is frequently a misunderstanding about what an 'ESP32' is. The ESP32 is a single surface mountable module to which you need to add various components to make a usable board. However the many different types of 'ESP32' boards may use different types of components and connections which could conflict with the pins used in the library examples here. It is not practical to test the examples on all possible permutations of 'ESP32' assembled boards, there are over 60 different boards supported by the Arduino IDE and I would go poor buying them all.

Thus the ESP32 examples presented here have been tested against a known standard which is an actual ESP32 Wroom module with no additional hardware, apart from the components shown in the schematic below. It is possible that the examples will not work against your particular 'ESP32' assembled board. If the example programs do not work for you, it is not an issue with the library but a difference between the reference hardware and your particular setup. I am not in a position to assist in resolving issues with particular ESP32 board versions. 

![Picture 1](/pictures/ESP32_Bare_Bones_Schematic.jpg)


As well as testing against the bare bones ESP32 schematic shown above, some of the examples will have been tested on a small portable unit I built which follows the same principles of the bare bones schematic. This PCB was developed to create a PCB that could be used for a small ESP32 based GPS tracker node, initially for use on The Things Network. The board has a GPS, I2C SSD1306 OLED and RFM98 lora device. There are options for a micro SD card, DS18B20 temperature sensor and a I2C FRAM. With it all assembled, the node consumes around 31uA in deep sleep with all devices connected. The 'Micro\_Node' contains additional circuitry to power off devices such as the lora module and GPS. Its highly unlikely that a standard ESP32 board will achieve a sleep current anywhere near the 'Micro_Node', this unit is pictured below;

![Picture 1](/pictures/ESP32_Micro_Node.jpg)


### ESP32 Deep Sleep

The ESP32 does some things with I\O pins when going into deep sleep that you might not expect. There are functions in this SX12XX library for putting the LoRa device into deep sleep, whilst still preserving register settings. The current then taken by the lora device is circa 0.5uA. However these functions will likely not work directly with most ESP32 boards due to the way the ESP32 handles the I\O pins in deep sleep mode.

When designing a board where a very low deep sleep current it is important, you need to build the design in stages, checking the deep sleep current after adding each component. Debugging a high deep sleep current on a fully assembled board can be from extremely difficult to impossible. Achieving a low deep sleep current for particular ESP32 boards is well outside the scope of this SX12xx library. 

<br>

**3\_LoRa\_Transmitter and 4\_LoRa\_Receiver example programs**

These example programs work  when used with the ESP32 'bare bones' schematic shown earlier and the 'ESP32 Dev Module' board type selected in the Arduino IDE. 

The pins used for SPI were; 

	SCK 18
	MISO 19
	MOSI 23
	NSS 5
	NRESET 27
    RFBUSY 25     (SX126X and SX128X devices only)
	DIOX 35       (DIOX is DIO0 on the SX127X devices and DIO1 on SX126X and SX128X devices)

In the example programs for ESP32, you can start the SPI with;

SPI.begin();

And the default SPI pin outs for the ESP32 module shown above will be used. 

Alternatively you can uses this format to start SPI;

SPI.begin(SCK, MISO, MOSI, NSS);

And the pin definitions will be taken from those specified in the 'Settings.h' file. If you change these pin allocations from the defaults given you will need to be sure they are valid for your particular ESP32 board. 


The SX12XX example programs are specifically written for and and tested on ATmega processors such as 328,1284 and 2560. However most will work, with minor modification, on the ESP32. 

This is a run through of the changes that were needed to have the tracker receiver examples; **25\_GPS\_Tracker\_Receiver\_With\_Display\_and\_GPS**,  written for the ATmega328P run on the ESP32 bare bones type boards described above. 

The original SX12XX tracker program uses the SPI interface to talk to the lora device, I2C to talk to the OLED display and software serial to talk to the GPS. 

The pins used for SPI were; 

	SCK 18
	MISO 19
	MOSI 23
	NSS 5
	NRESET 27
	RFBUSY 25     (SX126X and SX128X devices only)
	DIOX 35       (DIOX is DIO0 on the SX127X devices and DIO1 on SX126X and SX128X devices)

The ESP32 I2C pin connections were;

	SDA 21
	SCL 22

There is no need for software serial on the ESP32 as it has an available hardware serial port. These are the pin connections used;

	GPSTX 16  //this is data out from the GPS into the ESP32
	GPSRX 17  //this is data out from the ESP32 into the GPS

The other pins used were;

	LED1 2       //On board indicator LED, logic high for on
	GPSPOWER 26  //Pin that controls power to GPS, set to -1 if not used

The only software changes required were to change the lines in the Settings.h file from;

	#define USE_SOFTSERIAL_GPS                      
	#define HardwareSerialPort Serial2

to;

	//#define USE_SOFTSERIAL_GPS                      
	#define HardwareSerialPort Serial2

This removes the definition USE\_SOFTSERIAL\_GPS from the sketch and the effect of this change is then to remove these two lines from the Sketch;

	#include <SoftwareSerial.h>
	SoftwareSerial GPSserial(RXpin, Txpin);

And include this one;

	#define GPSserial HardwareSerialPort

Which sets the commands to read data from the GPS such as GPSserial.read() to be in effect Serial2.read(), which is the ESP32 hardware serial port used. 


**Note:** The provided lora settings (in the Settings.h file) may not be optimised for long distance. See the 'What is LoRa' document for information on how LoRa settings affect range. 


### Stuart Robinson

### April 2020

