/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/
/*

****************************************************************************************************************
  Program operation - The program flashes a LED connected to the pin defined by LED1, and puts the ESP32 to
  light_sleep for a period determined by TIME_TO_SLEEP (in seconds).

  The program also has the option of using a logic pin to control the power to the lora.SD card and DS18B20  
  devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are
  powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.

  Current in light_sleep mode was 1500uA
****************************************************************************************************************
*/


#define VCCPOWER 14                         //when low supplies VCC power to external devices. Set to -1 if not used
#define LED1 2                              //On board LED, high for on


#define uS_TO_S_FACTOR 1000000              //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  15                   //Time ESP32 will go to sleep (in seconds) 

RTC_DATA_ATTR int16_t bootCount = 0;
RTC_DATA_ATTR uint16_t sleepcount = 0;


void loop()
{
  Serial.print(F("Bootcount "));
  Serial.println(bootCount);
  Serial.print(F("Sleepcount "));
  Serial.println(sleepcount);
  Serial.println(F("LED Flash"));
  led_Flash(4, 125);
  Serial.println(F("LED On"));
  digitalWrite(LED1, HIGH);
  delay(2500);
  Serial.println(F("LED Off"));
  digitalWrite(LED1, LOW);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println(F("Start Sleep"));
  Serial.flush();
  sleepcount++;
  esp_light_sleep_start();
  Serial.println();
  Serial.println();
  Serial.println(F("Awake !"));
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
  Serial.println(F("50_LightSleep_Timed_Wakeup_ESP32 - Starting"));

  if (bootCount == 0) //Run this only the first time
  {
    bootCount = bootCount + 1;
  }

  pinMode(LED1, OUTPUT);                      //for PCB LED

  if (VCCPOWER >= 0)
  {
    pinMode(VCCPOWER, OUTPUT);                  //For controlling power to external devices
    digitalWrite(VCCPOWER, HIGH);               //VCCOUT off, lora device and SD card off
  }

}

