/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 08/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program receives a packet without using a processor buffer, the LoRa device
  internal buffer is read direct and copied to variables. The program is a simulation of the type of packet
  that might be received from a GPS tracker. Note that in this example a buffer of text is part of the
  received packet, this does need a processor buffer which is filled with data from the LoRa device internal
  buffer, if you don't need to send and receive text then the uint8_t receivebuffer[32]; definition can be
  ommited.

  The contents of the packet received, and printed to serial monitor, should be;

  "Tracker1" (buffer)      - trackerID
  1+             (uint32_t)    - packet count
  51.23456       (float)       - latitude
  -3.12345       (float)       - longitude
  199            (uint16_t)    - altitude
  8              (uint8_t)     - number of satellites
  3999           (uint16_t)    - battery voltage
  -9             (int8_t)      - temperature

  Serial monitor baud rate is set at 9600.

*******************************************************************************************************/

#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"

SX126XLT LT;

uint32_t RXpacketCount;
uint16_t errors;

uint8_t RXPacketL;               //length of received packet
int8_t  PacketRSSI;              //RSSI of received packet
int8_t  PacketSNR;               //signal to noise ratio of received packet


void loop()
{

  RXPacketL = LT.receiveSXBuffer(0, 0, WAIT_RX);       //returns 0 if packet error of some sort, no timeout

  digitalWrite(LED1, HIGH);      //something has happened

  PacketRSSI = LT.readPacketRSSI();
  PacketSNR = LT.readPacketSNR();

  if (RXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }

  digitalWrite(LED1, LOW);
  Serial.println();
}


uint8_t packet_is_OK()
{
  float latitude, longitude;
  uint16_t altitude, voltage;
  uint8_t satellites;
  int8_t temperature;
  uint32_t txcount;

  uint8_t receivebuffer[16];            //create receive buffer, make sure this is big enough for buffer sent !!!

  //packet has been received, now read from the SX12xx Buffer using the same variable type and
  //order as the transmit side used.

  RXpacketCount++;
  Serial.print(RXpacketCount);
  Serial.print(F("  "));

  LT.startReadSXBuffer(0);               //start buffer read at location 0
  LT.readBuffer(receivebuffer);          //read in the character buffer
  txcount  = LT.readUint32();            //read in the TXCount
  latitude = LT.readFloat();             //read in the latitude
  longitude = LT.readFloat();            //read in the longitude
  altitude = LT.readUint16();            //read in the altitude
  satellites = LT.readUint8();           //read in the number of satellites
  voltage = LT.readUint16();             //read in the voltage
  temperature = LT.readInt8();           //read in the temperature
  RXPacketL = LT.endReadSXBuffer();

  Serial.print((char*)receivebuffer);    //print the received buffer, cast to char needed
  Serial.print(F(","));
  Serial.print(txcount);
  Serial.print(F(","));
  Serial.print(latitude, 5);
  Serial.print(F(","));
  Serial.print(longitude, 5);
  Serial.print(F(","));
  Serial.print(altitude);
  Serial.print(F("m,"));
  Serial.print(satellites);
  Serial.print(F("sats,"));
  Serial.print(voltage);
  Serial.print(F("mV,"));
  Serial.print(temperature);
  Serial.print(F("c "));
  Serial.print(F("  RSSI"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB"));
  return RXPacketL;
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout"));
  }
  else
  {
    errors++;
    Serial.print(F("PacketError"));
    printpacketDetails();
    Serial.print(F("IRQreg,"));
    Serial.print(IRQStatus, HEX);
  }
}


void printpacketDetails()
{
  Serial.print(F("  RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB"));
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
      led_Flash(50, 50);                                            //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Receiver ready"));
  Serial.println();
}



