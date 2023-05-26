/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 24/04/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
  
  These are sleep routines for Atmel processors, tested on ATmega328. When calling the sleep routines be
  sure there are no pending interrupts such as from Serial.print(), strange things can happen otherwise.
*******************************************************************************************************/



#include <avr/sleep.h>          //sleep library for Atmel processor  
#include <avr/wdt.h>            //watchdog library for Atmel processor  


//Atmel watchdog sleep times
#define sleep16mS 0x00
#define sleep32mS 0x01
#define sleep64mS 0x02
#define sleep125mS 0x03
#define sleep250mS 0x04
#define sleep500mS 0x05
#define sleep1000mS 0x06
#define sleep2000mS 0x07
#define sleep4000mS 0x20
#define sleep8000mS 0x21


void SleepSeconds(uint32_t secs);
void SleepmS(uint32_t sleeps, uint8_t numbermS);
void atmelSleepPermanent();


void SleepSeconds(uint32_t secs)
{
  //for Atmel processor only
  uint16_t sleeps8secs, sleeps1secs;

  sleeps8secs = secs >> 3;
  sleeps1secs = secs - (sleeps8secs << 3);
  SleepmS(sleeps8secs, sleep8000mS);
  SleepmS(sleeps1secs, sleep1000mS);
}



void SleepmS(uint32_t sleeps, uint8_t numbermS)
{
  //for Atmel processor only
  uint32_t index;

  for (index = 1; index <= sleeps; index++)
  {
    ADCSRA = 0;                                      //disable ADC
    MCUSR = 0;                                       //clear various "reset" flags
    WDTCSR = bit (WDCE) | bit (WDE);                 //allow changes, disable reset
    WDTCSR = bit (WDIE) + numbermS;                  //set the number of mS for sleep
    wdt_reset();                                     //pat the pet, could be a dog
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


void atmelSleepPermanent()
{
  ADCSRA = 0;                //disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();           //timed sequence follows
  sleep_enable();

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);  //turn on brown-out enable select
  MCUCR = bit (BODS);        //this must be done within 4 clock cycles of above
  interrupts ();             //guarantees next instruction executed

  sleep_cpu ();              //sleep within 3 clock cycles of above

  /* wake up here */

  sleep_disable();
}



ISR (WDT_vect)
{
  //watchdog interrupt
  wdt_disable();  // disable watchdog
}


