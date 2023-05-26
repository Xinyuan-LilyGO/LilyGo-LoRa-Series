/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The is a test program for the TC74 temperature sensor using an ATmega328P based
  Arduino with an SX1278 LoRa device.

  The TC74 is read for the temperature and that is then transmitted via LoRa along with the battery voltage.
  The processor is then put to sleep using the processor watchdog timer for

  The TC74 can be tested and calibrated by placing the transmitter in a freezer and monitoring the
  temperature outside with the receiver program.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include <ProgramLT_Definitions.h>
#include "Settings.h"

#include <avr/wdt.h>                        //watchdog timer library, integral to Arduino IDE
#include <LowPower.h>                       //get the library here; https://github.com/rocketscream/Low-Power

SX127XLT LoRa;

#include <Wire.h>

uint32_t TXpacketCount;
uint8_t TXPacketL;

int8_t temperature;                         //the TC74 temperature value
uint8_t packetlength;                       //the packet length that was sent, checked against length received
uint16_t voltage;


void loop()
{
  TC74noPowersave(TC74_ADDRESS);            //turn TC74 back on
  delay(250);
  temperature = TC74read(TC74_ADDRESS);     //read the TC74
  voltage = readBatteryVoltage();           //read battery voltage

  Serial.print(F("Temperature,"));
  Serial.print(temperature);
  Serial.print(F("c Voltage,"));
  Serial.print(voltage);
  Serial.println(F("mV"));
  Serial.println();
  Serial.flush();
  TXpacketCount++;
  Serial.print(TXpacketCount);              //print the numbers of sends

  Serial.print(F(" Sending > "));

  if (sendSensorPacket())
  {
    Serial.println(F("Sent OK"));
  }
  else
  {
    Serial.print(F("Send Error - IRQreg,"));
    Serial.println(LoRa.readIrqStatus(), HEX);
  }

  Serial.print(F("Sleeping zzzz"));
  Serial.flush();                           //make sure all serial output has gone

  TC74Powersave(TC74_ADDRESS);              //turn TC74 off

  //now put the LoRa device and processor to sleep
  LoRa.setSleep(CONFIGURATION_RETENTION);   //sleep LoRa device, keeping register settings in sleep.
  sleep8seconds(2);                    //sleep Atmel processor for approx 16 seconds

  //wait a bit ................
  Serial.println(F(" - Awake !!"));         //the processor has woken up
  Serial.println();
}


uint8_t sendSensorPacket()
{
  //The SX12XX buffer is filled with variables of a known type and in a known sequence. Make sure the
  //receiver uses the same variable types and sequence to read variables out of the receive buffer.
  uint8_t len;

  LoRa.startWriteSXBuffer(0);                 //start the write packet to buffer process

  LoRa.writeUint8(Sensor1);                   //this byte defines the packet type
  LoRa.writeUint8(TXDestination);             //destination address of the packet, the receivers address
  LoRa.writeUint8(TXSource);                  //source address of this node

  /************************************************************************
    Highlighted section - this is where the actual sensor data is added to the packet
  ************************************************************************/
  LoRa.writeInt8(temperature);                //add the TC74 temperature
  LoRa.writeUint16(voltage);                  //add the battery voltage
  /************************************************************************/

  len = LoRa.endWriteSXBuffer();              //close the packet, get the length of data to be sent

  //now transmit the packet, set a timeout of 5000mS, wait for it to complete sending
  digitalWrite(LED1, HIGH);                   //turn on LED as an indicator
  TXPacketL = LoRa.transmitSXBuffer(0, (len + 2), 5000, TXpower, WAIT_TX);
  digitalWrite(LED1, LOW);                    //turn off indicator LED

  return TXPacketL;                           //TXPacketL will be 0 if there was an error sending
}


int8_t TC74read(uint8_t addr)
{

  int16_t regdata = 128;                      //max temperature is 127 degrees, so 128 indicates error.

  Wire.beginTransmission(addr);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom((int8_t) addr, 1);

  if (Wire.available())
  {
    regdata = Wire.read();
    if (regdata & 0x80)
    {
      regdata = -1 * ((regdata ^ 0xFF ) + 1);
    }
    return regdata;
  }
  else
  {
    return 128;
  }
}

void TC74noPowersave(uint8_t addr)
{
  Wire.beginTransmission(addr);
  Wire.write(0x01);              //config register
  Wire.write(0x00);              //no power save
  Wire.endTransmission();
}


void TC74Powersave(uint8_t addr)
{
  Wire.beginTransmission(addr);
  Wire.write(0x01);             //config register
  Wire.write(0x80);             //use power save
  Wire.endTransmission();
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

  analogReference(INTERNAL);
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

  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
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

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Transmitter ready"));
  Serial.println();

  Wire.begin();

  TC74read(TC74_ADDRESS);                              //do an initial sensor read
}
