/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 06/04/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple program to test a Serial Bluetooth device is working. The program 
  uses software serial to send a Hello World message to a serial port. If the Bluetooth device is paired
  with an Android device you should see Hello World in an Andriod Bluetooth terminal application. 

  Bluetooth baud rate is set at 9600, an HC-06 Bluetooth device is assumed. 
  Note that not all pins on all Arduinos will work with software serial, see here;

  https://www.arduino.cc/en/Reference/softwareSerial

  Serial monitor baud rate is set at 115200.

*******************************************************************************************************/


#define BlueToothBAUD 9600   //this is the serial baud rate that will be used for the Bluetooth, a common default
#define MONITORBAUD 9600     //this is the serial baud rate that will be used for the serial monitor 

#define RXpin A3             //this is the pin that the Arduino will use to receive data from the Bluetooth
#define TXpin A2             //this is the pin that the Arduino can use to send data (commands) to the Bluetooth

#include <SoftwareSerial.h>

SoftwareSerial BlueTooth(RXpin, TXpin);

uint32_t counter;

void loop()
{
 BlueTooth.print(counter++);
 BlueTooth.println(F(" Hello World")); 
 delay(1000); 
}


void setup()
{
  BlueTooth.begin(BlueToothBAUD);
  Serial.begin(MONITORBAUD);
  Serial.println("61_Bluetooth_Test Starting");
}
