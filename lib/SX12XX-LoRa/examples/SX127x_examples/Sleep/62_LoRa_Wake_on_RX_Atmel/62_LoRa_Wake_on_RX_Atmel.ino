/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/06/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

  When the program starts the LoRa device is setup to recieve packets with pin DIO0 set to go high when a
  packet arrives. The receiver remains powered (it cannot receive otherwise) and the processor
  (Atmel ATMega328P or 1284P) is put to sleep. When pin DIO0 does go high, indicating a packet is received,
  the processor wakes up and prints the packet. It then goes back to sleep.

  There is a printout of the valid packets received, these are assumed to be in ASCII printable text.
  The LED will flash for each packet received and the buzzer will sound,if fitted.

  Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 13.07mA.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.1"

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include "PinChangeInterrupt.h"      //get the library here; https://github.com/NicoHood/PinChangeInterrupt

#include <AtmelSleep.h>

SX127XLT LT;

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];     //create a buffer for the received packet

uint8_t RXPacketL;                   //stores length of packet received
int16_t PacketRSSI;                  //stores RSSI of received packet
int8_t  PacketSNR;                   //stores signal to noise ratio of received packet


void loop()
{
  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 0, NO_WAIT);   //setup LoRa device for receive with no timeout

  Serial.println(F("Waiting for RX - Sleeping"));
  Serial.flush();

  attachInterrupt(digitalPinToInterrupt(DIO0), wakeUp, HIGH);

  atmelSleepPermanent();                                         //sleep the processor

  detachInterrupt(digitalPinToInterrupt(DIO0));

  //something has happened ?
  Serial.println(F("Awake"));
  digitalWrite(LED1, HIGH);

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

  RXPacketL = LT.readPacket(RXBUFFER, RXBUFFER_SIZE);            //now read in the received packet to the RX buffer

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

  if (BUZZER > 0)
  {

    digitalWrite(BUZZER, LOW);
  }
  Serial.println();
}


void wakeUp()
{
  //handler for the interrupt
}


void packet_is_OK()
{
  uint16_t IRQStatus, localCRC;

  IRQStatus = LT.readIrqStatus();
  RXpacketCount++;

  RXPacketL = LT.readPacket(RXBUFFER, RXBUFFER_SIZE);            //now read in the received packet to the RX buffer

  Serial.print(F("Packet> "));
  LT.printASCIIPacket(RXBUFFER, RXPacketL);

  localCRC = LT.CRCCCITT(RXBUFFER, RXPacketL, 0xFFFF);
  Serial.print(F("  CRC,"));
  Serial.print(localCRC, HEX);
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(RXPacketL);
  Serial.print(F(",Packets,"));
  Serial.print(RXpacketCount);
  Serial.print(F(",Errors,"));
  Serial.print(errors);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  Serial.println();
  led_Flash(2, 125);                            //LED flash for approx 10 seconds
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                //get the IRQ status

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.println(F("RXTimeout"));
  }
  else
  {
    errors++;
    Serial.print(F("PacketError"));
    Serial.print(F(",RSSI,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm,SNR,"));
    Serial.print(PacketSNR);
    Serial.print(F("dB,Length,"));
    Serial.print(LT.readRXPacketL());                 //get the real packet length
    Serial.print(F(",Packets,"));
    Serial.print(RXpacketCount);
    Serial.print(F(",Errors,"));
    Serial.print(errors);
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();
    Serial.println();
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
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);                            //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("62_LoRa_Wake_on_RX_Atmel Starting"));

  if (BUZZER > 0)
  {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
  }

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("Radio Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();
}

