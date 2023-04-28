/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program transmits a LoRa packet without using a processor buffer, the LoRa
  devices internal buffer is filled directly with variables.

  The sensor used is a BME280. The pressure, humidity, and temperature are read and transmitted. There
  is also a 16bit value of battery mV (simulated) and and a 8 bit status value at the packet end.

  Although the LoRa packet transmitted and received has its own internal CRC error checking, you could
  still receive packets of the same length from another source. If this valid packet were to be used
  to recover the sensor values, you could be reading rubbish. To reduce the risk of this, when the packet
  is transmitted the CRC value of the actual sensor data is calculated and sent out with the packet.
  This CRC value is read by the receiver and used to check that the received CRC matches the supposed
  sensor data in the packet. As an additional check there is some addressing information at the beginning
  of the packet which is also checked for validity. Thus we can be relatively confident when reading the
  received packet that its genuine and from this transmitter. The packet is built and sent in the
  sendSensorPacket() function, there is a 'highlighted section' where the actual sensor data is added to
  the packet.

  Between readings the LoRa device, BME280 sensor, and Atmel microcontroller are put to sleep in units of
  8 seconds using the Atmel processor internal watchdog.

  The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file.

  There is also an option of using a logic pin to turn the resistor divider used to read battery voltage on
  and off. This reduces current used in sleep mode. To use the feature set the define for pin BATVREADON
  in 'Settings.h' to the pin used. If not using the feature set the pin number to -1.

  The Atmel watchdog timer is a viable option for a very low current sensor node. A 'bare bones' ATmega328P
  with regulator and LoRa device has a sleep current of 6.6uA, add the LoRa devices and BME280 sensor
  module and the average sleep current only rises to 6.8uA.

  One of these transmitter programs is running on a long term test with a 150mAh battery, to see how long
  the battery actually lasts.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"
#include <ProgramLT_Definitions.h>

#include <avr/wdt.h>                        //watchdog timer library, integral to Arduino IDE
#include <LowPower.h>                       //get the library here; https://github.com/rocketscream/Low-Power

SX126XLT LT;

#include <Seeed_BME280.h>                   //get library here; https://github.com/Seeed-Studio/Grove_BME280
BME280 bme280;                              //create an instance of the BME280 senosor
#include <Wire.h>

uint32_t TXpacketCount;
uint8_t TXPacketL;

float temperature;                          //the BME280 temperature value
float pressure;                             //the BME280 pressure value
uint16_t humidity;                          //the BME280 humididty value
uint16_t voltage;                           //the battery voltage value
uint8_t statusbyte;                         //a status byte, not currently used
uint16_t CRCvalue;                          //the CRC value of the packet data up to this point
uint8_t packetlength;                       //the packet length that was sent, checked against length received


void loop()
{
  TXpacketCount++;
  Serial.print(TXpacketCount);              //print the numbers of sends
  Serial.print(F(" Sending > "));

  readSensors();                            //read the sensor values
  printSensorValues();                      //print the sensor values

  if (sendSensorPacket())
  {
    Serial.println(F("SentOK"));
  }
  else
  {
    Serial.print(F("Send Error - IRQreg,"));
    Serial.println(LT.readIrqStatus(), HEX);
  }

  Serial.print(F("Sleeping zzzz"));
  Serial.flush();                           //make sure all serial output has gone

  //now put the sensor, LoRa device and processor to sleep
  sleepBME280();                            //sleep the BME280
  LT.setSleep(CONFIGURATION_RETENTION);     //sleep LoRa device, keeping register settings in sleep.
  sleep8seconds(sleeps);                    //sleep Atmel processor in units of approx 8 seconds

  //wait a bit ................
  Serial.println(F(" - Awake !!"));         //the processor has woken up
  Serial.println();
  
  LT.wake();
  normalBME280();                           //BME280 sensor to normal mode
}


uint8_t sendSensorPacket()
{
  //The SX12XX buffer is filled with variables of a known type and in a known sequence. Make sure the
  //receiver uses the same variable types and sequence to read variables out of the receive buffer.
  uint8_t len;

  LT.startWriteSXBuffer(0);                 //start the write packet to buffer process

  LT.writeUint8(Sensor1);                   //this byte defines the packet type
  LT.writeUint8('B');                       //this byte identifies the destination node of the packet
  LT.writeUint8(1);                         //this byte identifies the source node of the packet

  /************************************************************************
    Highlighted section - this is where the actual sensor data is added to the packet
  ************************************************************************/
  LT.writeFloat(temperature);               //add the BME280 temperature
  LT.writeFloat(pressure);                  //add the BME280 pressure
  LT.writeUint16(humidity);                 //add the BME280 humididty
  LT.writeUint16(voltage);                  //add the battery voltage
  LT.writeUint8(statusbyte);                //add the status byte
  /************************************************************************/

  len = LT.endWriteSXBuffer();              //close the packet, get the length of data to be sent

  addPacketErrorCheck(len);                 //add the additional CRC error checking to the packet end

  //now transmit the packet, set a timeout of 5000mS, wait for it to complete sending
  digitalWrite(LED1, HIGH);                 //turn on LED as an indicator
  TXPacketL = LT.transmitSXBuffer(0, (len + 2), 5000, TXpower, WAIT_TX);
  digitalWrite(LED1, LOW);                  //turn off indicator LED

  return TXPacketL;                         //TXPacketL will be 0 if there was an error sending
}


void addPacketErrorCheck(uint8_t len)
{
  //calculate the CRC of packet sensor data
  CRCvalue = LT.CRCCCITTSX(3, (len - 1), 0xFFFF);

  Serial.print(F("Calculated CRC value "));
  Serial.println(CRCvalue, HEX);

  LT.startWriteSXBuffer(len);                //start the write packet again at location of CRC, past end of sensor data
  LT.writeUint16(CRCvalue);                  //add the actual CRC value
  LT.endWriteSXBuffer();                     //close the packet
}


void readSensors()
{
  //read the sensor values into the global variables
  temperature = bme280.getTemperature();
  pressure = bme280.getPressure();
  humidity = bme280.getHumidity();

  if (BATVREADON >= 0)
  {
    voltage = readBatteryVoltage();            //read resistor divider across battery
  }
  else
  {
    voltage = 9999;                            //set a default value
  }
  statusbyte = 0x55;                         //manually set this for now, its a test
}


void printSensorValues()
{
  Serial.print(F("Temperature,"));
  Serial.print(temperature, 1);
  Serial.print(F("c,Pressure,"));
  Serial.print(pressure, 0);
  Serial.print(F("Pa,Humidity,"));
  Serial.print(humidity);
  Serial.print(F("%,Voltage,"));
  Serial.print(voltage);
  Serial.print(F("mV,Status,"));
  Serial.print(statusbyte, HEX);
  Serial.print(F("  "));
  Serial.flush();
}


void sleep8seconds(uint32_t sleeps)
{
  //uses the lowpower library
  uint32_t index;

  for (index = 1; index <= sleeps; index++)
  {
    //sleep 8 seconds
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
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


void writeBME280reg(byte reg, byte regvalue)
{
  //write a register value to the BME280
  Wire.beginTransmission((uint8_t) BME280_ADDRESS);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)regvalue);
  Wire.endTransmission();
}


uint16_t readBatteryVoltage()
{
  //relies on 1V1 internal reference and 91K & 11K resistor divider
  //returns supply in mV @ 10mV per AD bit read
  uint16_t temp;
  uint16_t volts = 0;
  byte index;

  if (BATVREADON >= 0)
  {
    digitalWrite(BATVREADON, HIGH);               //turn on MOSFET connecting resitor divider in circuit
  }

  analogReference(INTERNAL1V1);
  temp = analogRead(BATTERYAD);

  for (index = 0; index <= 4; index++)          //sample AD 5 times
  {
    temp = analogRead(BATTERYAD);
    volts = volts + temp;
  }
  volts = ((volts / 5) * ADMultiplier) + DIODEMV;

  if (BATVREADON >= 0)
  {
    digitalWrite(BATVREADON, LOW);               //turn off MOSFET connecting resitor divider in circuit
  }

  return volts;
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

  if (BATVREADON >= 0)
  {
    pinMode(BATVREADON, OUTPUT);
  }

  Serial.begin(9600);

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                    //long fast speed flash indicates LoRa device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  if (!bme280.init())
  {
    Serial.println("BME280 Device error!");
    led_Flash(100, 15);                     //long very fast speed flash indicates BME280 device error
  }

  Serial.println(F("Transmitter ready"));
  Serial.println();

  readSensors();                            //do an initial sensor read
}

