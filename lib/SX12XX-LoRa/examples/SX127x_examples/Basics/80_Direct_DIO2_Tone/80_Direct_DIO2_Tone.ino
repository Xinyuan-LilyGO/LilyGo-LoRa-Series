/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 23/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors. 
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - Transmits a FM tone using the LoRa device that can be picked up on an FM UHF 
  handheld receiver. The tones are generated in direct mode by applying a pulse using the Arduino tone()
  function (not supported by all Arduinos) to the DIO2 pin.
  
  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>                                               //the lora device is SPI based so load the SPI library                                         
#include <SX127XLT.h>                                          //include the appropriate library  
#include "Settings.h"                                          //include the setiings file, frequencies, LoRa settings etc   

SX127XLT LoRa;                                                 //create a library class instance called LoRa


void loop()
{
  Serial.print(TXpower);                                       //print the transmit power defined
  Serial.print(F("dBm "));
  Serial.println(F("PlayTones> "));
  Serial.println();

  LoRa.setupDirect(Frequency, Offset);
  LoRa.setTXDirect();                                          //turn on transmit
  LoRa.setTxParams(TXpower, RADIO_RAMP_DEFAULT);               //set TX power
  LoRa.writeRegister(REG_FDEVLSB, deviation);                  //set the deviation  
  LoRa.writeRegister(REG_PLLHOP, 0xAD);                        //set fast hop mode, needed for fast changes of frequency

  
  while(1)                                                     //repeat forever
  {
  digitalWrite(LED1, HIGH);
  tone(DIO2, 1000);                                            //pin, frequency, durationmS 
  delay(1000);
  digitalWrite(LED1, LOW);
  noTone(DIO2);
  delay(1000);
  };
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
  pinMode(LED1, OUTPUT);                                   //setup pin as output for indicator LED
  pinMode(6, OUTPUT);
  
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("80_Direct_DIO2_Tone Starting"));

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found
  if (LoRa.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  Serial.print(F("Tone Transmitter ready"));
  Serial.println();
}

