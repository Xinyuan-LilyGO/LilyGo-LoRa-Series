/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple terminal to allow you to use a Serial terminal program, such as Teraterm
  or CoolTerm to talk to a serial device such as a Bluetooth module connected to your Arduino. 

  Note that not all pins on all Arduinos will work with software serial, see here;

  https://www.arduino.cc/en/Reference/softwareSerial

  Serial monitor baud rate is set at 9600.

*******************************************************************************************************/


#define DEVICEBAUD 9600     //serial baud rate that will be used for the device conncted to your Arduino
#define MONITORBAUD 9600    //serial baud rate that will be used for the serial monitor 

#define RXpin A3            //this is the pin that the Arduino will use to receive data from the serial device
#define TXpin A2            //this is the pin that the Arduino can use to send data (commands) to the serial device


#include <SoftwareSerial.h>

SoftwareSerial device(RXpin, TXpin);

void loop()
{
  while (device.available())
  {
    Serial.write(device.read());
  }

  while (Serial.available())
  {
    device.write(Serial.read());
  }
  
}


void setup()
{
  device.begin(DEVICEBAUD);
  Serial.begin(MONITORBAUD);
  Serial.println("79_Serial_Terminal Starting");

  delay(1000);
 
}
