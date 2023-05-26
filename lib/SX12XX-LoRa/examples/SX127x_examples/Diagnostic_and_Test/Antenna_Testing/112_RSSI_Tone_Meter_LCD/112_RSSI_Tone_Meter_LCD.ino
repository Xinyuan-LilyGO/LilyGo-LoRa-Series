/***********************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
***********************************************************************************************************/

/***********************************************************************************************************
  Program operation.

  This program using the LoRa device as an RSSI meter, the signal level seen is printed to serial terminal
  and printed to a display. The LoRa device is put into receive mode so that the read of current RSSI works.

  Do not forget to fit an antenna to the LoRa device, you can destroy it if you use it without and antenna
************************************************************************************************************/


#include <SPI.h>                                 //the LoRa device is SPI based so load the SPI library
#include <SX127XLT.h>                            //include the appropriate library   
#include "Settings.h"                            //include the setiings file, frequencies, LoRa settings etc   

SX127XLT LoRa;                                   //create a library class instance called LoRa

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>               //get library here > https://github.com/mathertel/LiquidCrystal_PCF8574
LiquidCrystal_PCF8574 disp(DisplayAddress);      //set the LCD address to 0x3F, could be 0x27 also


void loop()
{
  uint8_t index;
  float floatrssi = 0;
  int16_t temprssi = 0;

  LoRa.receiveSXBuffer(0, 60000, NO_WAIT);         //turn on receiver so current RSSI reading works

  for (index = 1; index <= 25; index++)
  {
    temprssi = temprssi + getCurrentRSSI();
    delay(10);
  }

  floatrssi = (float) temprssi / 25 ;

  Serial.print(floatrssi, 1);
  Serial.println(F("dBm"));
  displayscreen2(floatrssi);
}


void displayscreen1()
{
  disp.setCursor(0, 0);
  disp.print(F("RSSI Meter"));
}


void displayscreen2(float averagerssi)
{
  disp.setCursor(0, 0);
  disp.print("       ");
  disp.setCursor(0, 0);
  disp.print(averagerssi, 1);
}


int16_t getCurrentRSSI()
{
  int16_t CurrentRSSI;                                  //RSSI of received packet

  if (Frequency < 779000000)                            //779Mhz is lower frequency limit for SX1279 on band1 (HF Port)
  {
    CurrentRSSI = -164 + LoRa.readRegister(REG_RSSIVALUE);
  }
  else
  {
    CurrentRSSI = -157 + LoRa.readRegister(REG_RSSIVALUE);
  }

  return CurrentRSSI;
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
  int16_t error;
  pinMode(LED1, OUTPUT);		                     //setup pin for PCB LED
  led_Flash(2, 125);                             //two quick LED flashes to indicate program start

  Serial.begin(115200);                          //setup Serial console ouput
  Serial.println(F(__FILE__));
  Serial.println();

  SPI.begin();				                           //initialize SPI

  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                           //two further quick LED flashes to indicate device found
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
  Serial.print(F("FM Tone "));
  digitalWrite(LED1, HIGH);
  LoRa.toneFM(1000, 1000, 10000, 0.9, 2);
  digitalWrite(LED1, LOW);
  Serial.println();

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.print("Probing for PCF8574 on address 0x");
  Serial.println(DisplayAddress, HEX);

  Wire.begin();
  Wire.beginTransmission(DisplayAddress);
  error = Wire.endTransmission();

  if (error == 0)
  {
    Serial.println("LCD found");
    disp.begin(16, 2);                   //initialize the LCD
  } else
  {
    Serial.println("LCD not found.");
  }

  disp.clear();
  disp.setCursor(0, 0);
  disp.setBacklight(1);
  displayscreen1();
  delay(1000);
  disp.clear();

  Serial.println(F("RSSI Tone Meter Starting"));
}
