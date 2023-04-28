/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

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

  Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 12.26mA.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"

#include <avr/sleep.h>
#include "PinChangeInterrupt.h"      //get the library here; https://github.com/NicoHood/PinChangeInterrupt

SX127XLT LT;

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];     //create a buffer for the received packet

uint8_t RXPacketL;                   //stores length of packet received
int16_t PacketRSSI;                  //stores RSSI of received packet
int8_t  PacketSNR;                   //stores signal to noise ratio of received packet


void loop()
{
  LT.fillSXBuffer(0, 13, '#');               //make sure the first part of FIFO is cleared, so we can tell its a fresh packet

  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 0, NO_WAIT);   //setup LoRa device for receive and continue

  //receive is setup

  Serial.println(F("Going to sleep zzzz"));
  Serial.println();
  Serial.flush();                            //make sure all serial has gone, it can wake up processor

  sleep_permanent();                         //put processor to sleep, with LoRa device listening, should
                                             //wakeup when DIO0 goes high 

  Serial.println(F("Awake !!!!"));
  digitalWrite(LED1, HIGH);                  //something has happened ?

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

  RXPacketL = LT.readPacket(RXBUFFER, RXBUFFER_SIZE);   //now read in the received packet to the RX buffer

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


void sleep_permanent()
{
  LT.clearIrqStatus(IRQ_RADIO_ALL);                     //ensure the DIO0 low is cleared, otherwise there could be an immediate wakeup
  attachPCINT(digitalPinToPCINT(DIO0), wakeUp, HIGH);   //This is a hardware interrupt, the LoRa device is set for DIOo goes high on RXdone
  ADCSRA = 0;                                           //disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();                                      //timed sequence follows
  sleep_enable();

  MCUCR = bit (BODS) | bit (BODSE);                     //turn on brown-out enable select
  MCUCR = bit (BODS);                                   //this must be done within 4 clock cycles of above
  interrupts ();                                        //guarantees next instruction executed

  sleep_cpu ();                                         //sleep within 3 clock cycles of above

  /* wake up here */

  sleep_disable();

  detachPCINT(digitalPinToPCINT(DIO0));
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
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                    //get the IRQ status

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout"));
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
  Serial.println(F("6_LoRa_RX_and_Sleep_Atmel Starting"));
  Serial.println();

  if (BUZZER > 0)
  {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
  }

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
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

  Serial.println();
  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();

}

