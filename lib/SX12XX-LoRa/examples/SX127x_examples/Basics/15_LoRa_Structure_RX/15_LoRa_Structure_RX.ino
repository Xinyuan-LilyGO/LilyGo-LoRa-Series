/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 17/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program demonstrates the receiving of a structure as a LoRa packet. The packet
  sent is typical of what might be sent from a GPS tracker.

  The structure type is defined as trackerPacket and an instance called location1 is created. The structure
  includes a character array (text).

  The matching receiving program is '15_LoRa_RX_Structure' can be used to receive and display the packet,
  though the program '9_LoRa_LowMemory_RX' should receive it as well, since the packet contents are the same.

  Not that the structure definition and variable order (including the buffer size) used in the transmitter
  need to match those used in the receiver. Good luck.

  The contents of the packet received, and printed to serial monitor, should be;

  "tracker1"     (buffer)      - trackerID
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
#include <SX127XLT.h>
#include "Settings.h"

SX127XLT LT;

uint8_t RXPacketL;               //stores length of packet received
uint32_t RXpacketCount;          //count of received packets
int16_t PacketRSSI;              //RSSI of received packet
int8_t PacketSNR;                //signal to noise ratio of received packet
uint32_t errors;                 //count of packet errors


struct trackerPacket
{
  uint8_t trackerID[13];
  uint32_t txcount;
  float latitude;
  float longitude;
  uint16_t altitude;
  uint8_t satellites;
  uint16_t voltage;
  int8_t temperature;
};

struct trackerPacket location1;                 //define an instance called location1 of the structure trackerPacket


void loop()
{
  RXPacketL = LT.receive( (uint8_t *) &location1, sizeof(location1), 0, WAIT_RX); //wait for a packet to arrive with no timeout

  digitalWrite(LED1, HIGH);                     //something has happened, what I wonder ?

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


void printlocation1()
{
  uint8_t buff[13];                                    //define a buffer to receive a copy from the structure
  memcpy (&buff, &location1.trackerID, sizeof(buff));  //copy the contents of buffer in struture to buff[]

  //now print the contents of the structure
  Serial.print((char*) buff);                          //cast to a char type for printing
  Serial.print(F(","));
  Serial.print(location1.txcount);
  Serial.print(F(","));
  Serial.print(location1.latitude, 5);
  Serial.print(F(","));
  Serial.print(location1.longitude, 5);
  Serial.print(F(","));
  Serial.print(location1.altitude);
  Serial.print(F("m,"));
  Serial.print(location1.satellites);
  Serial.print(F("sats,"));
  Serial.print(location1.voltage);
  Serial.print(F("mV,"));
  Serial.print(location1.temperature);
  Serial.print(F("c "));
}


void packet_is_OK()
{
  RXpacketCount++;
  Serial.print(RXpacketCount);
  Serial.print(F("  "));
  printlocation1();
  printpacketDetails();
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


void setup(void)
{
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);                            //two quick LED flashes to indicate program start

  Serial.begin(9600);

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.print(F("Receiver ready"));
  Serial.println();
  Serial.println();
}

