/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is a simple test program for the SSD1306 and SH1106 OLEDs running on
  a 3.3V 8Mhz bare bones ATmega328P. The program prints a short message on each line, pauses, clears the
  screen, turns off the screem, puts the processor to sleep and starts again. Sleep current with display
  off is circa 8uA.

  OLED address defaults to 0x3C.

  Screen write on 8Mhz Atmel 196mS.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <avr/sleep.h>
#include <avr/wdt.h>


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
  disp.setPowerSave(1);                              //power save display, turns off, sleep current circa 8uA
  sleep1seconds(2);                                  //sleep in units of 1 second 
  disp.setPowerSave(0);                              //display back to normal
}


void sleep1seconds(uint32_t sleeps)
{
  uint32_t index;

  for (index = 1; index <= sleeps; index++)
  {
    ADCSRA = 0;                                      //disable ADC
    MCUSR = 0;                                       //clear various "reset" flags
    WDTCSR = bit (WDCE) | bit (WDE);                 //allow changes, disable reset
    WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1);   //set interrupt mode and an interval, set WDIE, and 1 seconds sleep
    wdt_reset();                                     //pat the dog
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    noInterrupts ();                                 //timed sequence follows
    sleep_enable();
    MCUCR = bit (BODS) | bit (BODSE);                //turn off brown-out enable in software
    MCUCR = bit (BODS);
    interrupts ();                                   //guarantees next instruction executed

    sleep_cpu ();
    //awake here
    sleep_disable();                                 //cancel sleep as a precaution
  }
}

ISR (WDT_vect)
{
  //watchdog interrupt
  wdt_disable();  // disable watchdog
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

