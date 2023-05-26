/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors. 
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - A silly program really, but does demonstrate that you can shift a carrier generated
  by the LoRa device in FSK mode fast enought to play audio tones that can be picked up on an FM UHF
  handheld receiver. The tones are not true FM but the receiver does not know that. 
  
  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>                                               //the lora device is SPI based so load the SPI library                                         
#include <SX126XLT.h>                                          //include the appropriate library  
#include "Settings.h"                                          //include the settings file, frequencies etc   
#include "pitches.h"                                           //lookup file for notes

SX126XLT LT;                                                   //create a library class instance called LT


void loop()
{
  Serial.print(TXpower);                                       //print the transmit power defined
  Serial.print(F("dBm "));
  Serial.println(F("PlayTune> "));
  Serial.println();
  Serial.flush();

  playpart1();
 
  playpart2();
 
  LT.toneFM(NOTE_F4, 250, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_GS4, 500, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_F4, 350, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_A4, 125, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_C5, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 375, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_C5, 125, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_E5, 650, deviation, adjustfreq, TXpower);
 
  delay(250);
 
  playpart2();
 
  LT.toneFM(NOTE_F4, 250, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_GS4, 500, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_F4, 375, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_C5, 125, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 500, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_F4, 375, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_C5, 125, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 650, deviation, adjustfreq, TXpower);  
 
  LT.setMode(MODE_STDBY_RC);                   //turns off carrier   
  
  Serial.println();
  
  delay(2000);                                 //have a delay between packets
}


void playpart1()
{
  LT.toneFM(NOTE_A4, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 500, deviation, adjustfreq, TXpower);    
  LT.toneFM(NOTE_A4, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_F4, 350, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_C5, 150, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_A4, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_F4, 350, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_C5, 150, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 650, deviation, adjustfreq, TXpower);
 
  delay(250);
 
  LT.toneFM(NOTE_E5, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_E5, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_E5, 500, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_F5, 350, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_C5, 150, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_GS4, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_F4, 350, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_C5, 150, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 650, deviation, adjustfreq, TXpower);
 
  delay(250);
}

 
void playpart2()
{
  LT.toneFM(NOTE_A5, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 300, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A4, 150, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_A5, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_GS5, 325, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_G5, 175, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_FS5, 125, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_F5, 125, deviation, adjustfreq, TXpower);    
  LT.toneFM(NOTE_FS5, 250, deviation, adjustfreq, TXpower);
 
  delay(250);
 
  LT.toneFM(NOTE_AS4, 250, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_DS5, 500, deviation, adjustfreq, TXpower);
  LT.toneFM(NOTE_F5, 325, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_CS5, 175, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_C5, 125, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_AS4, 125, deviation, adjustfreq, TXpower);  
  LT.toneFM(NOTE_C5, 250, deviation, adjustfreq, TXpower);  
 
  delay(500);
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
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("59_Play_Star_Wars_Tune Starting"));

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found
  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA_DEVICE))
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
  
  LT.setupDirect(Frequency, Offset);
  Serial.print(F("Tone Transmitter ready"));
  Serial.println();
}

