/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 05/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program transmits a packet without using a processor buffer, the LoRa device
  internal buffer is filled direct with variables. The program is a simulation of the type of packet
  that might be sent from a GPS tracker. Note that in this example a buffer of text is part of the
  transmitted packet.

  The matching receiving program '9_LoRa_LowMemory_RXIRQ' can be used to receive and display the packet

  The contents of the packet received, and printed to serial monitor, should be;

  TR1            (buffer)      - trackerID
  51.23456       (float)       - latitude
  -3.12345       (float)       - longitude
  199            (uint16_t)    - altitude
  8              (uint8_t)     - number of satellites
  3999           (uint16_t)    - battery voltage
  -9             (int8_t)      - temperature

  Memory use on an Arduino Pro Mini;
  Sketch uses 4958 bytes (15%) of program storage space.
  Global variables use 224 bytes (10%) of dynamic memory, leaving 1824 bytes for local variables.

  This is a version of example 8_LoRa_LowMemory_TX.ino that does not require the use of the DIO1 pin to
  check for transmit done. In addition no NRESET pin is needed either, so its a program for use with a
  minimum pin count Arduino. Leave the DIO1 and NRESET pins on the LoRa device not connected.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <SX128XLT.h>
#include "Settings.h"

SX128XLT LoRa;


void loop()
{
  //The SX12XX buffer is filled with variables of a known type and order. Make sure the receiver
  //uses the same variable type and order to read variables out of the receive buffer.

  char trackerID[] = "TR1";
  float latitude = 51.23456;
  float longitude = -3.12345;
  uint16_t altitude = 199;
  uint8_t satellites = 8;
  uint16_t voltage = 3999;
  int16_t temperature = 9;
  uint8_t TXPacketL = 0;
  uint8_t BytesSent = 0;

  LoRa.startWriteSXBuffer(0);                         //start the write at SX12XX internal buffer location 0
  LoRa.writeBufferChar(trackerID, sizeof(trackerID));     //+4 bytes (3 characters plus null (0) at end)
  LoRa.writeFloat(latitude);                          //+4 = 8 bytes
  LoRa.writeFloat(longitude);                         //+4 = 12 bytes
  LoRa.writeUint16(altitude);                         //+2 = 14 bytes
  LoRa.writeUint8(satellites);                        //+1 = 15 bytes
  LoRa.writeUint16(voltage);                          //+2 = 17 bytes
  LoRa.writeInt8(temperature);                        //+1 = 18 bytes total to send
  TXPacketL = LoRa.endWriteSXBuffer();                //closes packet write and returns the length of the packet to send

  BytesSent = LoRa.transmitSXBufferIRQ(0, TXPacketL, 5000, TXpower, WAIT_TX);   //set a TX timeout of 5000mS

  if (BytesSent == 0)                               //if bytessent is 0, there has been a error
  {
    Serial.print(F("Send Error"));
  }
  else
  {
    Serial.print(BytesSent);
    Serial.print(F(" Bytes Sent"));
  }

  Serial.println();
  delay(packet_delay);
}


void setup()
{
  Serial.begin(9600);

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    while (1);
  }

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);
  Serial.flush();
}
