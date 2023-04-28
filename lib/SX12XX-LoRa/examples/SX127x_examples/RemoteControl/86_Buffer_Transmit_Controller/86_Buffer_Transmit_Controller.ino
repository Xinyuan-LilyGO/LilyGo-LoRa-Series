/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/09/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a LoRa transmitter that transmits a buffer containing text to control a
  remote device by sending 'LEDOn' to turn the LED on and 'LEDOff' to turn the LED off.

  Sample Serial Monitor output;

  86_Buffer_Transmit_Controller Starting
  LoRa Device found
  Transmitter ready
  Send Packet> LEDOn
  Send Packet> LEDOff

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#include <SPI.h>                                //the lora device is SPI based so load the SPI library                                         
#include <SX127XLT.h>                           //include the appropriate library  

SX127XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define DIO0 3                                  //DIO0 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //pin for LED

#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using
#define TXpower 10                              //LoRa transmit power in dBm
#define TXtimeout 10000                         //transmit timeout in mS

uint8_t TXPacketL;

uint8_t onpacket[] = "LEDOn";                   //send this to turn LED on
uint8_t offpacket[] = "LEDOff";                 //send this to turn LED off

#include <SPI.h>
#include <LoRa.h>


void loop()
{
  //************************************
  //LED on
  //************************************

  Serial.print(F("Send Packet> ("));
  Serial.print(sizeof(onpacket));
  Serial.print(F("chars) "));

  LT.printASCIIPacket(onpacket, sizeof(onpacket));                           //print the buffer (the sent packet) as ASCII

  digitalWrite(LED1, HIGH);
  LT.transmit(onpacket, sizeof(onpacket), TXtimeout, TXpower, WAIT_TX);      //will return packet length sent if OK, otherwise 0 if transmit error

  Serial.println();
  delay(5000);                                                               //have a delay between packets

  //************************************
  //LED off
  //************************************

  Serial.print(F("Send Packet> ("));
  Serial.print(sizeof(offpacket));
  Serial.print(F("chars) "));
  LT.printASCIIPacket(offpacket, sizeof(offpacket));                         //print the buffer (the sent packet) as ASCII

  digitalWrite(LED1, LOW);
  LT.transmit(offpacket, sizeof(offpacket), TXtimeout, TXpower, WAIT_TX);    //will return packet length sent if OK, otherwise 0 if transmit error

  Serial.println();
  delay(5000);                                                               //have a delay between packets
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("86_Buffer_Transmit_Controller Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                                     //long fast speed LED flash indicates device error
    }
  }

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO); //configure frequency and LoRa settings

  Serial.print(F("Transmitter ready"));
  Serial.println();
}


