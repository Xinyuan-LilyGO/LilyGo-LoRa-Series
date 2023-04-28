/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/09/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program that blinks the 3 x WS2811 Neopixel LEDs on the DUE shield green,
  red, blue, white and off.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/


#include <Adafruit_NeoPixel.h>                  //get library here > https://github.com/adafruit/Adafruit_NeoPixel

#define NPIXEL A2                               //WS2811 on this pin
#define NUMPIXELS 1                             //How many NeoPixels are attached
#define BRIGHTNESS 50                           //LED brightness 0 to 255 

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NPIXEL, NEO_GRB + NEO_KHZ800); //get library here > https://github.com/adafruit/Adafruit_NeoPixel


void loop()
{
  Neo_FlashGreen(4, 125, 0, BRIGHTNESS);
  Neo_FlashRed(4, 125, 0, BRIGHTNESS);
  Neo_FlashBlue(4, 125, 0, BRIGHTNESS);
  Neo_FlashWhite(4, 125, 0, BRIGHTNESS);
  
  delay(2000);
}


void Neo_FlashGreen(uint16_t flashes, uint16_t delaymS, uint8_t number, uint8_t brightness)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    pixels.setPixelColor(number, pixels.Color(brightness, 0, 0)); //Green on
    pixels.show();
    delay(delaymS);
    pixels.setPixelColor(number, pixels.Color(0, 0, 0));          //all colours off
    pixels.show();
    delay(delaymS);
  }
}


void Neo_FlashRed(uint16_t flashes, uint16_t delaymS, uint8_t number, uint8_t brightness)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    pixels.setPixelColor(number, pixels.Color(0, brightness, 0)); //Red on
    pixels.show();
    delay(delaymS);
    pixels.setPixelColor(number, pixels.Color(0, 0, 0));          //all colours off
    pixels.show();
    delay(delaymS);
  }
}


void Neo_FlashBlue(uint16_t flashes, uint16_t delaymS, uint8_t number, uint8_t brightness)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    pixels.setPixelColor(number, pixels.Color(0, 0, brightness)); //Blue on
    pixels.show();
    delay(delaymS);
    pixels.setPixelColor(number, pixels.Color(0, 0, 0));          //all colours off
    pixels.show();
    delay(delaymS);
  }
}


void Neo_FlashWhite(uint16_t flashes, uint16_t delaymS, uint8_t number, uint8_t brightness)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    pixels.setPixelColor(number, pixels.Color(brightness,brightness,brightness)); //White on
    pixels.show();
    delay(delaymS);
    pixels.setPixelColor(number, pixels.Color(0, 0, 0));          //all colours off
    pixels.show();
    delay(delaymS);
  }
}



void setup()
{
  Serial.begin(9600);
  Serial.println(F("91_WS2811_Neopixel_Blink Starting"));
  Serial.println();

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));                 //all colours off
  pixels.show();
}
