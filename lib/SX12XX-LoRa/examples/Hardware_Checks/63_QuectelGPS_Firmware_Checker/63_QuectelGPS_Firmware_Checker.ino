/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 08/09/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program sends a command to a Quectel GPS to return its firmware revision. 
  The output from the GPS is copied to the Serial Monitor.
  
  If you get no data displayed on the serial monitor, the most likely cause is that you have the receive
  data pin into the Arduino (RX) pin connected incorrectly.

  If the data displayed on the serial terminal appears to be random text with odd symbols its very
  likely you have the GPS serial baud rate set incorrectly.

  Note that not all pins on all Arduinos will work with software serial, see here;

  https://www.arduino.cc/en/Reference/softwareSerial

  Serial monitor baud rate is set at 115200.

*******************************************************************************************************/


#define GPSBAUD 9600          //this is the serial baud rate that will be used for the GPS, a common default
#define MONITORBAUD 115200    //this is the serial baud rate that will be used for the serial monitor 

#define RXpin A3              //this is the pin that the Arduino will use to receive data from the GPS
#define TXpin A2              //this is the pin that the Arduino can use to send data (commands) to the GPS - not used
#define GPSPOWER 4            //When high this pin turns GPS on
#define LED1 8                //when high this pin turns LED on

#include <SoftwareSerial.h>

SoftwareSerial GPS(RXpin, TXpin);

void loop()
{
  while (GPS.available())
  {
    Serial.write(GPS.read());
  }
}


void setup()
{
  pinMode(GPSPOWER, OUTPUT);
  digitalWrite(GPSPOWER, HIGH);

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, HIGH);
  
  GPS.begin(GPSBAUD);
  Serial.begin(MONITORBAUD);
  Serial.println("63_QuectelGPS_Firmware_Checker");
  delay(2000);
  GPS.println("$PMTK605*31");
 
}
