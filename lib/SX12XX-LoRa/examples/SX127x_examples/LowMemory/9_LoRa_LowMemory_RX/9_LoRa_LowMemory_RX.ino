/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 05/11/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program receives a packet without using a memory buffer, the LoRa device
  internal buffer is read direct for variables. The program is a simulation of the type of packet
  that might be received from a GPS tracker. Note that in this example a buffer of text is part of the
  received packet.

  The matching transmitter program '8_LoRa_LowMemory_TX' is used to transmit the packet.

  The contents of the packet received, and printed to serial monitor, should be;

  TR1            (buffer)      - trackerID
  51.23456       (float)       - latitude
  -3.12345       (float)       - longitude
  199            (uint16_t)    - altitude
  8              (uint8_t)     - number of satellites
  3999           (uint16_t)    - battery voltage
  -9             (int8_t)      - temperature

  
  
  Memory use on an Arduino Pro Mini;
  
  Sketch uses 6290 bytes (19%) of program storage space. 
  Global variables use 237 bytes (11%) of dynamic memory, leaving 1811 bytes for local variables. 
 

  Serial monitor baud rate is set at 9600.

*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"

SX127XLT LoRa;


void loop()
{
  uint8_t RXPacketL;
  
  RXPacketL = LoRa.receiveSXBuffer(0, 0, WAIT_RX);       //returns 0 if packet error of some sort, no timeout

  if (RXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }

  Serial.println();
}


uint8_t packet_is_OK()
{
  char receivebuffer[4];                 //create receive buffer, make sure this is big enough for buffer sent !!!
  float latitude;
  float longitude;
  uint16_t altitude;
  uint8_t satellites;
  uint16_t voltage;
  int8_t temperature;
  uint8_t RXPacketL;
  static uint8_t RXpacketCount;
  
  //packet has been received, now read from the SX12xx Buffer using the same variable type and
  //order as the transmit side used.

  RXpacketCount++;
  Serial.print(RXpacketCount);
  Serial.print(F("  "));

  LoRa.startReadSXBuffer(0);               //start buffer read at location 0
  LoRa.readBufferChar(receivebuffer);      //read in the character buffer
  latitude = LoRa.readFloat();             //read in the latitude
  longitude = LoRa.readFloat();            //read in the longitude
  altitude = LoRa.readUint16();            //read in the altitude
  satellites = LoRa.readUint8();           //read in the number of satellites
  voltage = LoRa.readUint16();             //read in the voltage
  temperature = LoRa.readInt8();           //read in the temperature
  RXPacketL = LoRa.endReadSXBuffer();      //finish packet read, get received packet length 

  Serial.print(receivebuffer);           //print the received character buffer
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
  printpacketDetails();
  return RXPacketL;
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LoRa.readIrqStatus();

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout"));
  }
  else
  {
    Serial.print(F("PacketError"));
    printpacketDetails();
    Serial.print(F("IRQreg,"));
    Serial.print(IRQStatus, HEX);
  }
}


void printpacketDetails()
{
  int16_t PacketRSSI;              //RSSI of received packet
  int8_t  PacketSNR;               //signal to noise ratio of received packet
  
  PacketRSSI = LoRa.readPacketRSSI();
  PacketSNR = LoRa.readPacketSNR();
  
  Serial.print(F("  RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB"));
}


void setup()
{
  Serial.begin(9600);

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("Device OK"));
  }
  else
  {
    Serial.println(F("Device error"));
    while (1);
  }

  Serial.flush();
  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);
}



