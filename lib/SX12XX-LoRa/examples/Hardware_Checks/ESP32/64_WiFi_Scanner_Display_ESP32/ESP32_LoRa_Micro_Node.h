/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/01/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of the Tracker boards, the ESP32_Micro_Node, be sure to change
//them to match your own setup. You will also need to connect up the pins for the SPI bus, which on the 
//ESP32_Micro_Node are SCK on pin 18, MISO on pin 19 and MOSI on pin 23. Some pins such as DIO1, DIO2 and
//BUZZER may not be in used by this sketch so they do not need to be connected and should be set to -1.


#define SWITCH1 0                    //pin number for switch, the boot switch also 
#define LED1 2                       //pin number for LED
#define NSS 5                        //pin number where the NSS line for the LoRa device is connected
#define SDCS  13                     //ESP32 pin number for device select on SD card module
#define VCCPOWER 14                  //pin controls power to external devices
#define DIO2 15                      //pin number for DIO2 pin on LoRa device
#define TonePin 15                   //pin number for LoRa radio tone generation, connects to LoRa device pin DIO2
#define GPSTX 16                     //pin number for TX output from Arduino - RX into GPS
#define GPSRX 17                     //pin number for RX input into Arduino - TX from GPS
#define BATREAD 25                   //pin that switches on supply measure resistor devider
#define GPSPOWER 26                  //pin controls power to external devices
#define NRESET 27                    //pin where LoRa device reset line is connected
#define ONEWIREBUS 33                //pin for one wire bus devices  
#define DIO1 34                      //pin connected to DIO1 on LoRa device
#define DIO0 35                      //pin number for DIO0 pin on LoRa device
#define SupplyAD 36                  //pin for reading supply voltage

#define ADMultiplier 10              //multiplier for supply volts calculation, default


