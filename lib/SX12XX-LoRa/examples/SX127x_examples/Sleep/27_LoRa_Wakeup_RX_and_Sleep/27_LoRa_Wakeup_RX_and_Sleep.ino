/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/08/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the LoRa device need to be defined in the 'Settings.h' file also.

  When the program starts the LoRa device is setup to recieve packets. The program then waits 2 seconds
  for a packet to arrive. If a packet is received it is displayed and the LED flashes. If no packet is
  received the timeout is reported. After a packet is received or there is a timeout the processor and
  LoRa device goto sleep for 16 seconds. The sequence is then repeated.
 
  Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 9uA.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"

#include <avr/sleep.h>
#include <LowPower.h>                //get the library here; https://github.com/rocketscream/Low-Power

SX127XLT LT;

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];     //create a buffer for the received packet

uint8_t RXPacketL;                   //stores length of packet received
int16_t PacketRSSI;                  //stores RSSI of received packet
int8_t  PacketSNR;                   //stores signal to noise ratio of received packet
uint16_t IRQStatus;


void loop()
{
  
  LT.fillSXBuffer(0, 13, '#');                              //make sure the first part of LoRa FIFO is cleared, so we can tell its a fresh packet

  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, RXTIMEOUT, WAIT_RX);   //setup LoRa device for receive, timout of 2000mS
  PacketRSSI = LT.readPacketRSSI();
  PacketSNR = LT.readPacketSNR();
  IRQStatus = LT.readIrqStatus();                           //get the IRQ status

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout"));
  }
  else
  {
    //a packet has been received
    digitalWrite(LED1, HIGH);

    if (RXPacketL == 0)
    {
      packet_is_Error();
    }
    else
    {
      packet_is_OK();
    }
  }

  digitalWrite(LED1, LOW);
  Serial.println();
  Serial.println(F("Going to sleep zzzz"));
  Serial.println();
  Serial.flush();                            //make sure all serial has gone, it can wake up processor

  LT.setSleep(CONFIGURATION_RETENTION);      //preserve LoRa register settings in sleep.

  sleep8seconds(2);                          //goto sleep for 16 seconds

  led_Flash(1, 10);                          //quick LED flashes to indicate program wakeup
  Serial.println(F("Awake !!!!"));
  Serial.println();

  LT.wake();                                  
  
}


void sleep1second(uint32_t sleeps)
{
  //uses the lowpower library
  uint32_t index;

  for (index = 1; index <= sleeps; index++)
  {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);         //sleep in 1 second steps
  }
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
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
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
  led_Flash(2, 25);                             //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.println(F("27_LoRa_Wakeup_RX_and_Sleep Starting"));
  Serial.println();

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
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

  Serial.println();
  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
}
