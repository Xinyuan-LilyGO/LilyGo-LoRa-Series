/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simple test for a Bosch BME280 sensor. Readings are sent to the serial
  monitor. The Seeed library assumes the BME280 is at address 0x76. It can be changed to 0x77 by an edit
  in the Seeed_BME280.h librsry file. 
  
  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <Seeed_BME280.h>                   //get library here; https://github.com/Seeed-Studio/Grove_BME280
BME280 bme280;                              //create an instance of the BME280 senosor

#define BME280_REGISTER_CONTROL 0xF4        //BME280 register number for power control

#include <Wire.h>

#define LED1 8                              //on board LED, high for on

float temperature;                          //the BME280 temperature value
float pressure;                             //the BME280 pressure value
uint16_t humidity;                          //the BME280 humididty value


void loop()
{
  Serial.print(F("Reading > "));

  readSensors();                            //read the sensor values
  printSensorValues();                      //print the sensor values
  Serial.println();

  sleepBME280();
  delay(500);
  normalBME280();                           //BME280 sensor to normal mode
}


void readSensors()
{
  //read the sensor values into the global variables
  temperature = bme280.getTemperature();
  pressure = bme280.getPressure();
  humidity = bme280.getHumidity();

}


void printSensorValues()
{
  Serial.print(F("Temperature,"));
  Serial.print(temperature, 1);
  Serial.print(F("c,Pressure,"));
  Serial.print(pressure, 0);
  Serial.print(F("Pa,Humidity,"));
  Serial.print(humidity);
  Serial.print(F("%"));
  Serial.print(F("  "));
  Serial.flush();
}


void sleepBME280()
{
  //write this register value to BME280 to put it to sleep
  writeBME280reg(BME280_REGISTER_CONTROL, B01111100);
}


void normalBME280()
{
  //write this register value to BME280 to put it to read mode
  writeBME280reg(BME280_REGISTER_CONTROL, B01111111);
}


void writeBME280reg(uint8_t reg, uint8_t regvalue)
{
  //write a register value to the BME280
  Wire.beginTransmission((uint8_t) BME280_ADDRESS);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)regvalue);
  Wire.endTransmission();
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
  pinMode(LED1, OUTPUT);                      //for PCB LED
  led_Flash(2, 125);

  Serial.begin(9600);
  Serial.println();
  Serial.println(F("81_BME280_Test starting"));

  bme280.init();
  Serial.println(F("Initialised BME280"));
  Serial.println();

  readSensors();                              //do an initial sensor read
}

