/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/01/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program reads a single DS18B20 temperature sensor and prints the result to the
  serial monitor.

  The program also has the option of using a logic pin to control the power to the lora and SD card
  devices, which can save power in sleep mode. If the hardware is fitted to your board then these devices are
  assumed to be powered on by setting the VCCPOWER pin low. If your board does not have this feature set
  VCCPOWER to -1.

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/


#define programversion "V1.0"

#define ONE_WIRE_BUS 4                            //pin the DS18B20 is connected to
#define LED1 8                                    //LED, on when reading temperature  


#include <OneWire.h>                              //get library here > https://github.com/PaulStoffregen/OneWire  
OneWire oneWire(ONE_WIRE_BUS);                    //create instance of OneWire library

#include <DallasTemperature.h>                    //get library here > https://github.com/milesburton/Arduino-Temperature-Control-Library
DallasTemperature sensor(&oneWire);               //create instance of dallas library


void loop()
{
  float DS18B20temperature;

  digitalWrite(LED1, HIGH);
  sensor.requestTemperatures();
  digitalWrite(LED1, LOW);

  DS18B20temperature = sensor.getTempCByIndex(0);
  Serial.print(F("DS18B20 Temperature "));
  Serial.print(DS18B20temperature, 2);
  Serial.println(F("c"));
  delay(5000);
}


void led_Flash(unsigned int flashes, unsigned int delaymS)
{
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
  Serial.begin(9600);
  Serial.println();
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(programversion));
  Serial.println();
  Serial.println("48_DS18B20_Test Starting");

  pinMode(LED1, OUTPUT);

  led_Flash(4, 125);                          //one second of flashes

  sensor.begin();
  sensor.requestTemperatures();               //do a null temperature read
}



