/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

  There is a printout of the valid packets received in HEX format. Thus the program can be used to receive
  and record non-ASCII packets. The LED will flash for each packet received.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"

SX126XLT LT;

uint8_t RXPacketL;                                //stores length of packet received
int16_t  PacketRSSI;                              //stores RSSI of received packet
int8_t  PacketSNR;                                //stores signal to noise ratio of received packet


void loop()
{
  RXPacketL = LT.receiveSXBuffer(0, 60000, WAIT_RX); //returns 0 if packet error of some sort, timeout set at 60secs\60000mS

  digitalWrite(LED1, HIGH);                       //something has happened

  printSeconds();

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


void packet_is_OK()
{
  printReceptionDetails();
  Serial.print(RXPacketL);
  Serial.print(F(" bytes > "));
  LT.printSXBufferHEX(0, (RXPacketL - 1));
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                     //get the IRQ status

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    Serial.print(F(" PacketError"));
    printReceptionDetails();
    Serial.print(F("  Length,"));
    Serial.print(LT.readRXPacketL());                  //get the real packet length
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
  }
}


void printReceptionDetails()
{
  Serial.print(F(" RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB  "));
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


void printSeconds()
{
  float secs;

  secs = ( (float) millis() / 1000);
  Serial.print(secs, 3);
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  Serial.begin(115200);

  Serial.println(F("221_LoRa_Packet_Monitor Starting"));

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    while (1)
    {
      led_Flash(50, 50);
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  LT.printModemSettings();
  Serial.println();
  LT.printOperatingSettings();
  Serial.println();
  Serial.print(F("Packet monitor ready"));
  Serial.println();
}
