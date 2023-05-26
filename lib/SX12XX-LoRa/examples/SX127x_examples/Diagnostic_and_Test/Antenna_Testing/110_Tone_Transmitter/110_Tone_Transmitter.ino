/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program operation

  This test program has been written to enable simple checks to be made on the operation of antennas.
  The transmitter can sends a simple FM compatble audio tone, this can be used by a matching receiver to
  measure the strength of signal transmitted. The measurement device could be a low cost SDR, or for field
  or portable use an RF Explorer or a tracker board with display setup as an RSSI meter. The tone can be
  heard on a UHF handheld covering the frequency used or viewed and measured on a low cost SDR.

  Do not forget to fit an antenna to the LoRa device, you can destroy it if you use it without an antenna.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                 //the lora device is SPI based so load the SPI library
#include <SX127XLT.h>                            //include the appropriate library   
#include "Settings.h"                            //include the setiings file, frequencies, LoRa settings etc   

SX127XLT LoRa;                                   //create a library class instance called LoRa

uint32_t testloop = 0;


void loop()
{
  testloop++;

  Serial.print(F("Test Loop "));
  Serial.println(testloop);
  Serial.println(F("Transmit Tone "));
  Serial.flush();

  digitalWrite(LED1, HIGH);
  LoRa.toneFM(tone_frequencyHz, tone_lengthmS, deviation, adjustfreq, transmit_power);
  digitalWrite(LED1, LOW);

  Serial.println();
  delay(transmission_delaymS);
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
  pinMode(LED1, OUTPUT);		                    //setup pin for LED
  led_Flash(2, 125);                            //two quick LED flashes to indicate program start

  Serial.begin(115200);                         //setup Serial console ouput
  Serial.println();
  Serial.println(__FILE__);
  Serial.println();

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  LoRa.setupDirect(Frequency, Offset);
  Serial.println(F("Tone Transmitter ready"));
  Serial.println();
}
