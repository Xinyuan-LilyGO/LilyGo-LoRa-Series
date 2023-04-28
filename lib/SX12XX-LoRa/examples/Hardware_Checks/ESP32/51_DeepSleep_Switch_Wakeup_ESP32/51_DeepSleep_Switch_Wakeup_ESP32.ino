/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
Program Operation - The program flashes a LED connected to the pin defined by LED1, and puts the ESP32
to deep_sleep. Pressing BOOT switch should wake up the ESP32 from sleep.

Only the specific RTC IO pins can be used as a source for external wakeup. 
These are pins: 0,2,4,12-15,25-27,32-39.

Current in deep_sleep for a bare bones ESP32 with regulator and no other devices was 27uA.

Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define LED1 2                              //pin number for LED
#define SWITCH1 0                           //pin number wakeup switch

                     
RTC_DATA_ATTR int16_t bootCount = 0;
RTC_DATA_ATTR uint16_t sleepcount = 0;


void loop()
{
  Serial.print(F("Bootcount "));
  Serial.println(bootCount);
  Serial.print(F("Sleepcount "));
  Serial.println(sleepcount);
  Serial.println(F("LED Flash"));
  led_Flash(4,125);
  Serial.println(F("LED On"));
  digitalWrite(LED1, HIGH);
  delay(2500);
  Serial.println(F("LED Off"));
  digitalWrite(LED1, LOW);
  
  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);      //wakeup on pin GPIO0 going low 
  
  Serial.println(F("Start Sleep"));
  Serial.flush();
  sleepcount++;
  esp_deep_sleep_start();
  Serial.println();
  Serial.println();
  Serial.println(F("Awake ?"));                     //should not really see this, deep sleep wakeup causes reset ....
}


void led_Flash(unsigned int flashes, unsigned int delaymS)
{
  //flash LED to show tracker is alive
  unsigned int index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, LOW);
    delay(delaymS);
    digitalWrite(LED1, HIGH);
    delay(delaymS);
  }
}


void setup()
{
  Serial.begin(9600);                         //setup Serial console ouput
  Serial.println();
  Serial.println();
  Serial.println(F("51_DeepSleep_Timed_Wakeup_ESP32 - Starting"));

  if(bootCount == 0) //Run this only the first time
  {
      bootCount = bootCount+1;
  }
  
  pinMode(LED1, OUTPUT);                      //for PCB LED                              
  pinMode(SWITCH1, INPUT_PULLUP);             //for wakeup switch 

}

