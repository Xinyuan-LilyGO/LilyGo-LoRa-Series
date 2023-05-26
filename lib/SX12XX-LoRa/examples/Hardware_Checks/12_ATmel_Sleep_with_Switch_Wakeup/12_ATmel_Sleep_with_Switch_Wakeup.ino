/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 26/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program tests the deep sleep mode and wakeup with a switch of an Atmel 328P or
  1284P processor. The program starts, flashes the LED and then puts the processor into permanent sleep.
  It can be woken up with a switch press. Used as a base test routine for checking the sleep current of
  a board.

  Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 1.7uA with a 3.3V MCP1700
  regulator being used.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <avr/sleep.h>

#define LED1 8                                  //on board LED, high for on
#define SWITCH1 2                               //switch used to wake processor up, switch pin connected to 
                                                //ground to activate. Define as -1 if switch not used.

uint32_t sleeps;
                                                

void loop()
{
  digitalWrite(LED1, HIGH);
  delay(2000);
  sleeps++;
  Serial.print(sleeps);
  Serial.println(F(" Sleeping zzzzz...."));
  Serial.println();
  Serial.flush();                              //make sure serial out buffer is empty
  digitalWrite(LED1, LOW);

  attachInterrupt(digitalPinToInterrupt(SWITCH1), wakeUp, FALLING);   //This is a hardware interrupt

  sleep_permanent();                           //goto sleep till woken up by switch press
  
  detachInterrupt(digitalPinToInterrupt(SWITCH1));
  
  Serial.println(F("Awake !"));
  digitalWrite(LED1, HIGH);
}


void sleep_permanent()
{
  ADCSRA = 0;                         //disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();                   //timed sequence follows
  sleep_enable();

  //turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);  //turn on brown-out enable select
  MCUCR = bit (BODS);                //this must be done within 4 clock cycles of above
  interrupts ();                     //guarantees next instruction executed

  sleep_cpu ();                      //sleep within 3 clock cycles of above

  /* wake up here */

  sleep_disable();
}


void wakeUp()
{
  //handler for the interrupt
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
  pinMode(SWITCH1, INPUT_PULLUP);              //setup switch pin, ground to activate
  pinMode(LED1, OUTPUT);                       //setup pin as output for indicator LED
  led_Flash(2, 125);                           //two quick LED flashes to indicate program start

  pinMode(SWITCH1, INPUT_PULLUP);              //setup switch pin, connect to ground to activate

  Serial.begin(9600);
  Serial.println();
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("12_Atmel_Sleep_with_Switch_Wakeup Starting"));


}
