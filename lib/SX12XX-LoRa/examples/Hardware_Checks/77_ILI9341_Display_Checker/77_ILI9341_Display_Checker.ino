/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/09/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is a simple test program for the LIL9341 TFT display that comes in a
  range of sizes. The program prints a short message on each line, pauses, clears the screen, and starts
  again. Program uses Adafruit ILI9341 library.
  
  Screen write time on Arduino DUE 332mS, screen clear 268mS 
  Screen write time on Teensy 4.1 207mS, screen clear 180mS

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include "SPI.h"
#include "Adafruit_GFX.h"                      //get library here > https://github.com/adafruit/Adafruit-GFX-Library  
#include "Adafruit_ILI9341.h"                  //get library here > https://github.com/adafruit/Adafruit_ILI9341

int8_t DISPCS = 10;                            
int8_t DISPDC = 8;

//int8_t DISPCS = 43;                            //for DUE shield                            
//int8_t DISPDC = 45;                            //for DUE shield      

Adafruit_ILI9341 disp = Adafruit_ILI9341(DISPCS, DISPDC);

#define textscale 2                            //default text scale   

uint16_t writecount;
uint32_t startmS, endmS, timemS;


void loop()
{
  setCursor(0, 0);
  disp.setTextColor(ILI9341_WHITE);
  disp.setTextSize(textscale);

  writecount++;
  Serial.print(writecount);
  Serial.print(F(" Writing to display"));

  startmS = millis();
  disp.fillScreen(ILI9341_BLACK);              //clear screen
  screen1();
  endmS = millis();
  timemS = endmS - startmS;
  setCursor(8, 4);
  disp.print(timemS);
  disp.print(F("mS"));

  Serial.print(F(" - done "));
  Serial.print(timemS);
  Serial.println(F("mS"));

  delay(2000);

  startmS = millis();
  disp.fillScreen(ILI9341_BLACK);              //clear screen
  endmS = millis();
  timemS = endmS - startmS;
  setCursor(0, 0);
  disp.print(F("Screen clear "));
  disp.print(timemS);
  disp.print(F("mS"));

  delay(2000);
  
}


void setCursor(uint8_t lcol, uint8_t lrow)
{
  disp.setCursor((lcol * 6 * textscale), (lrow * 10 * textscale));
}


void screen1()
{
  setCursor(0, 0);
  disp.print(F("ILI9341_Checker"));

  setCursor(0, 1);
  disp.print(F("Line 1"));

  setCursor(0, 2);
  disp.print(F("Line 2"));

  setCursor(0, 3);
  disp.print(F("Line 3"));

  setCursor(0, 4);
  disp.print(F("Line 4"));

  setCursor(0, 5);
  disp.print(F("Line 5"));

  setCursor(0, 6);
  disp.print(F("Line 6"));

  setCursor(0, 7);
  disp.print(F("Line 7"));

  setCursor(0, 8);
  disp.print(F("Line 8"));

  setCursor(0, 9);
  disp.print(F("Line 9"));

  setCursor(0, 10);
  disp.print(F("Line 10"));

  setCursor(0, 11);
  disp.print(F("01234567890123456789012"));                         //display is 12 lines x 23 charaters
}


void setup()
{
  Serial.begin(9600);
  Serial.println(F("77_ILI9341_Display_Checker starting"));
  SPI.begin();
  disp.begin();
  //disp.setFont(Arial_16);
  disp.fillScreen(ILI9341_BLACK);
  disp.setTextColor(ILI9341_WHITE);
  disp.setRotation(1);
  disp.setTextSize(textscale);
}

