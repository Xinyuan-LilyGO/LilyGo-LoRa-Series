/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/01/19

  This programs is supplied as is, it is up to the user of the program to decide if the programs are
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program blinks an LED connected the pin number defined below. The blinks
  should be close to one per second. messages are sent to the Serial Monitor also.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define LED1 2                                 //pin number for LED, set logic level high for on

#define Program_Version "V1.0"

uint16_t seconds;                              //used to display time elapsed on Serial Monitor

void loop()
{
  Serial.print(seconds);
  Serial.println(F(" Seconds"));               //this message should print on console at close to once per second
  seconds++;
  digitalWrite(LED1, HIGH);
  delay(100);
  digitalWrite(LED1, LOW);
  delay(890);                                  //should give approx 1 second flash
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  //general purpose routine for flashing LED as indicator
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);                  //LED on
    delay(delaymS);
    digitalWrite(LED1, LOW);                   //LED off
    delay(delaymS);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);                       //setup pin as output for indicator LED
  led_Flash(2, 125);                           //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("1_LED_Blink_ESP32 Starting"));
}

