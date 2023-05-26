/***********************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
***********************************************************************************************************/

/***********************************************************************************************************
  Program operation.

  This program is for testing LCD displays that use the PCF8574 display backpack.

************************************************************************************************************/

const uint8_t DisplayAddress = 0x3F;             //I2C address of PCF8574 on display, common options are 0x27 and 0x3F

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>               //get library here > https://github.com/mathertel/LiquidCrystal_PCF8574
LiquidCrystal_PCF8574 disp(DisplayAddress);      //set the LCD address to 0x3F, could be 0x27 also

#define LED1 8                                   //on board LED, high for on


void loop()
{
  disp.setCursor(0, 0);
  disp.print(F("Line 0"));
  disp.setCursor(0, 1);
  disp.print(F("ABCDEFGHIJKLMNOP"));
  delay(2000);
  disp.clear();
  delay(1000);
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

  Wire.begin();
  Wire.beginTransmission(DisplayAddress);
  error = Wire.endTransmission();

  if (error == 0)
  {
    Serial.println("LCD found");
    disp.begin(16, 2);                   //initialize the LCD, (20, 4) for 4 line 20 character also supported
  } else
  {
    Serial.println("LCD not found.");
  }

  disp.clear();
  disp.setCursor(0, 0);
  disp.setBacklight(1);

}
