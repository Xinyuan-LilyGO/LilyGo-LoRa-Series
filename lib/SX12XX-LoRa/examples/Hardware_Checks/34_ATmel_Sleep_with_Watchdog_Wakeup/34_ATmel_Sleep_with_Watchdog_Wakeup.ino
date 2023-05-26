/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/09/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program tests the sleep mode of an Atmel ATMega328P processor.

  At power up the flashes an LED 4 times, then turns on the LED for 5 seconds. Then the processor is put
  to sleep for 16 seconds. On wakeup the LED flashes twice, then is on for 2 seconds and the board goes to 
  sleep again. And the sequence repeats. 

  Sleep current for a 'bare bones' ATmega328 with a MCP1700 regulator @ 3.3V and using an external event
  such as a switch to wakeup from sleep should be around 2uA. Using the watchdog timer to wakeup raises
  the deep sleep current to circa 6.2uA.   
  
  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/


//basic code from here - https://www.gammon.com.au/forum/?id=11497

#include <avr/sleep.h>
#include <avr/wdt.h>

#define LED1 8                 


void loop ()
{
  Serial.println("LED Off");
  digitalWrite(LED1, LOW);
  digitalWrite(13, LOW);
  Serial.println(F("Now Sleeping... "));
  Serial.flush();
  
  sleep8seconds(2);                                  //sleep for about 16 seconds
  
  Serial.println("Awake ");
  led_Flash(2, 125);
  Serial.println("LED On ");
  digitalWrite(LED1, HIGH);
  digitalWrite(13, HIGH);
  delay(2000);
}


void sleep8seconds(uint32_t sleeps)
{
  uint32_t index;

  for (index = 1; index <= sleeps; index++)
  {
    ADCSRA = 0;                                      //disable ADC
    MCUSR = 0;                                       //clear various "reset" flags
    WDTCSR = bit (WDCE) | bit (WDE);                 //allow changes, disable reset
    WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);   //set interrupt mode and an interval, set WDIE, and 8 seconds delay
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


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;
  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(13, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    digitalWrite(13, LOW);
    delay(delaymS);
  }
}


void setup ()
{
  pinMode(LED1, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  led_Flash(4, 125);
  Serial.println("LED On ");
  digitalWrite(LED1, HIGH);
  digitalWrite(13, HIGH);
  delay(5000);
}


ISR (WDT_vect)
{
  //watchdog interrupt
  wdt_disable();  // disable watchdog
}


