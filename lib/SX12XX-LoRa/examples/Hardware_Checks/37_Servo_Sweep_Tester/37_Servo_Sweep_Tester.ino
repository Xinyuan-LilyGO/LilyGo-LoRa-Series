/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program sweeps two servos from one end of their travel to the other. Useful to
  check servos are connected correctly and working.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/


#define pinservoX1 2                              //pin for controlling servo X 1 
#define pinservoY1 4                              //pin for controlling servo Y 1 


#define LED1 8

#include <Servo.h>

Servo ServoX1;                                     //create the servo object
Servo ServoY1;                                     //create the servo object


void loop()
{
  uint16_t index;

  for (index = 900; index <= 2100; index++)
  {
    //Serial.print(F("Microseconds1 "));
    //Serial.println(index);
    ServoX1.writeMicroseconds(index);
    ServoY1.writeMicroseconds(index);
  }

  delay(1000);

  for (index = 2100; index >= 900; index--)
  {
    //Serial.print(F("Microseconds1 "));
    //Serial.println(index);
    ServoX1.writeMicroseconds(index);
    ServoY1.writeMicroseconds(index);
  }

  delay(1000);
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
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);


  Serial.begin(9600);
  ServoX1.attach(pinservoX1);                     //using pin servoX for servo object
  ServoY1.attach(pinservoY1);                     //using pin servoX for servo object

  Serial.println(F("Servo sweep test starting"));
}
