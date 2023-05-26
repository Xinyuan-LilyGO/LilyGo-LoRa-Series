/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/09/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is a simple test program for the SSD1306 and SH1106 OLEDs. The program
  prints a short message on each line, pauses, clears the screen, turns off the screen, waits a while and
  starts again. 
  
  OLED address defaults to 0x3C.

  Screen write on 8Mhz Atmel 196mS.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <U8x8lib.h>                                        //get library here >  https://github.com/olikraus/u8g2 
//U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);    //use this line for standard 0.96" SSD1306
U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);       //use this line for 1.3" OLED often sold as 1.3" SSD1306

#define DEFAULTFONT u8x8_font_chroma48medium8_r             //font for U8X8 Library


uint16_t writecount;
uint32_t startwritemS, endwritemS, timemS;


void loop()
{
  writecount++;
  Serial.print(writecount);
  Serial.print(F(" Writing to display"));

  startwritemS = millis();
  disp.clear();
  screen1();
  endwritemS = millis();
  timemS = endwritemS - startwritemS;
  disp.setCursor(8, 4);
  disp.print(timemS);
  disp.print(F("mS"));

  Serial.print(F(" - done "));
  Serial.print(timemS);
  Serial.println(F("mS"));

  delay(2000);
  Serial.println(F("PowerSave display"));
  Serial.flush();                                    //pending serial output affects sleep mode
  disp.setPowerSave(1);                              //power save display
  delay(5000);
  disp.setPowerSave(0);                              //display back to normal
}


void screen1()
{
  disp.setCursor(0, 0);
  disp.print(F("Hello World !"));
  disp.setCursor(0, 1);
  disp.print(F("Line 1"));
  disp.setCursor(0, 2);
  disp.print(F("Line 2"));
  disp.setCursor(0, 3);
  disp.print(F("Line 3"));
  disp.setCursor(0, 4);
  disp.print(F("Line 4"));
  disp.setCursor(0, 5);
  disp.print(F("Line 5"));
  disp.setCursor(0, 6);
  disp.print(F("Line 6"));
  disp.setCursor(0, 7);
  disp.print(F("0123456789012345"));                         //display is 8 lines x 16 charaters when using the
}


void setup()
{
  Serial.begin(9600);
  Serial.println(F("31_SSD1306_SH1106_OLED_Checker starting"));
  disp.begin();
  disp.setFont(DEFAULTFONT);
}
