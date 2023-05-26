/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program that transmits a long LoRa packets lasting about 5 seconds that
  can be used to measure the frequency and power of the transmission using external equipment. The bandwidth
  of the transmission is only 10khz, so a frequency counter should give reasonable average result.

  The LoRa settings to use, including transmit powwer, are specified in the 'Settings.h' file.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"

SX127XLT LT;

uint8_t TXPacketL;
uint32_t TXPacketCount;


void loop()
{
  Serial.print(F("Sending "));
  Serial.print(TXpower);
  Serial.print(F("dBm Packet > "));
  Serial.flush();

  uint8_t buff[] = "Hello World!1234Hello World!1234";             //use a longish packet to measure
  TXPacketL = sizeof(buff);

  LT.printASCIIPacket(buff, TXPacketL);                            //print the packet

  digitalWrite(LED1, HIGH);
  if (LT.transmit(buff, TXPacketL, 10000, TXpower, WAIT_TX))       //will return packet length sent if OK, otherwise 0
  {
    TXPacketCount++;
    packet_is_OK();
    digitalWrite(LED1, LOW);
  }
  else
  {
    packet_is_Error();
    digitalWrite(LED1, LOW);
  }

  Serial.println();
  delay(packet_delay);
}


void packet_is_OK()
{
  Serial.print(F(" "));
  Serial.print(TXPacketL);
  Serial.print(F(" Bytes SentOK"));
  Serial.print(F(" PacketsSent "));
  Serial.print(TXPacketCount);
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                   //get the IRQ status
  Serial.print(F("SendError,"));
  Serial.print(F("Length,"));
  Serial.print(TXPacketL);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
  digitalWrite(LED1, LOW);                          //this leaves the LED on slightly longer for a packet send error
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

  Serial.println(F("13_Frequency_and_Power_Check_TX Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
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
      led_Flash(50, 50);                           //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.print(F("Transmitter ready - TXBUFFER_SIZE "));
  Serial.println(TXBUFFER_SIZE);

  LT.printModemSettings();
  Serial.println();
  Serial.println();
}

