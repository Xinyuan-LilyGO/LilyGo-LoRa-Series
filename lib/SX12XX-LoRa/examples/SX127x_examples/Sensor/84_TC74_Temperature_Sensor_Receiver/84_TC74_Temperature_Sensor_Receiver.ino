/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors. 
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program for an ATmega328P Arduino that can be used to receive and display
  the sensor packets sent by the program 83_TC74_Temperature_Sensor_transmitter. Uses an SSD1306 or SH1107
  OLED display and SX1278 LoRa module.

   
  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"
#include <ProgramLT_Definitions.h>

SX127XLT LoRa;

uint32_t RXpacketCount;          //count of all packets received
uint32_t ValidPackets;           //count of packets received with valid data
uint32_t RXpacketErrors;         //count of all packets with errors received
bool packetisgood;

uint8_t RXPacketL;               //length of received packet
int16_t PacketRSSI;              //RSSI of received packet
int8_t  PacketSNR;               //signal to noise ratio of received packet

uint8_t RXPacketType;            //indicates type of packet sent
uint8_t RXDestination;           //destination node of packet 0 - 255   
uint8_t RXSource;                //source node of packet 0 - 255 
int8_t temperature;              //the TC74 temperature value
uint16_t voltage;                //the battery voltage value in mV


#include <U8x8lib.h>                                        //get library here >  https://github.com/olikraus/u8g2 
//U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);    //use this line for standard 0.96" SSD1306
U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);       //use this line for 1.3" OLED often sold as 1.3" SSD1306
#define DEFAULTFONT u8x8_font_chroma48medium8_r             //font for U8X8 Library


void loop()
{
  RXPacketL = LoRa.receiveSXBuffer(0, 0, WAIT_RX);   //returns 0 if packet error of some sort, no timeout set

  digitalWrite(LED1, HIGH);      //something has happened

  PacketRSSI = LoRa.readPacketRSSI();
  PacketSNR = LoRa.readPacketSNR();

  if (RXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_Received_OK();                            
  }

  digitalWrite(LED1, LOW);
  Serial.println();
}


void packet_Received_OK()
{
  RXpacketCount++;
  Serial.print(F("Packets,"));
  Serial.print(RXpacketCount);
  
  LoRa.startReadSXBuffer(0);
  
  RXPacketType = LoRa.readUint8();           //the packet type received
  RXDestination = LoRa.readUint8();          //the destination address the packet was sent to 
  RXSource = LoRa.readUint8();               //the source address, where the packet came from

  /************************************************************************
    Highlighted section - this is where the actual sensor data is read from
    the packet
  ************************************************************************/
  temperature = LoRa.readInt8();             //the TC74 temperature value
  voltage = LoRa.readUint16();               //the battery voltage value
  /************************************************************************/

  LoRa.endReadSXBuffer();

  printreceptionDetails();                   //print details of reception, RSSI etc
  Serial.println();

  Serial.println(F("Packet is OK"));
  printSensorValues();                       //print the sensor values
  Serial.println(); 
  printPacketCounts();                       //print count of valid packets and errors                  
  displayscreen1();
  Serial.println();
}


void printSensorValues()
{
  Serial.print(F("Temperature,"));
  Serial.print(temperature);
  Serial.print(F("c,Voltage,"));
  Serial.println(voltage);
  Serial.flush();
}


void printreceptionDetails()
{
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LoRa.readRXPacketL()); 
}


void printPacketCounts()
{
  Serial.print(F("OKPackets,"));
  Serial.print(RXpacketCount);
  Serial.print(F(",Errors,"));
  Serial.print(RXpacketErrors);  
}


void displayscreen1()
{
  //show sensor data on display
  disp.clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("Sensor "));
  disp.print(RXSource);
  disp.clearLine(1);
  disp.setCursor(0, 1);
  disp.print(temperature);
  disp.print(F("c"));
  disp.clearLine(2);
  disp.setCursor(0, 2);
  disp.print(voltage);
  disp.print(F("mV"));
  disp.clearLine(6);
  disp.setCursor(0, 6);
  disp.print(F("OKPackets "));
  disp.print(RXpacketCount);
  disp.setCursor(0, 7);
  disp.print(F("Errors "));
  disp.print(RXpacketErrors);
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  RXpacketErrors++;
  IRQStatus = LoRa.readIrqStatus();

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout "));
  }
  else
  {
    Serial.print(F("PacketError "));
    printreceptionDetails();
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LoRa.printIrqStatus();
    Serial.println();
    disp.clearLine(7);
    disp.setCursor(0, 7);
    disp.print(F("Errors "));
    disp.print(RXpacketErrors);
  }
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

  disp.begin();
  disp.setFont(DEFAULTFONT);
  disp.setCursor(0, 0);
  disp.print(F("Check LoRa"));
  disp.setCursor(0, 1);

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    disp.print(F("LoRa OK"));
    led_Flash(2, 125);
  }
  else
  {
    disp.print(F("Device error"));
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed flash indicates device error
    }
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Receiver ready"));
  Serial.println();
}
