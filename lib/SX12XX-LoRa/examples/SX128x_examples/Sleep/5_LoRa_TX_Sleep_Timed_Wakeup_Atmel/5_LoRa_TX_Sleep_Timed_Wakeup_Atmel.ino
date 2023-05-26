/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program tests the sleep mode and register retention of the lora device in sleep
  mode, it assumes an Atmel ATMega328P processor is in use. The LoRa settings to use are specified in the
  'Settings.h' file.

  A packet is sent, containing the text 'Before Device Sleep' and the LoRa device and Atmel processor are put
  to sleep. The processor watchdog timer should wakeup the processor in 15 seconds (approx) and register
  values should be retained.  The device then attempts to transmit another packet 'After Device Sleep'
  without re-loading all the LoRa settings. The receiver should see 'After Device Sleep' for the first
  packet and 'After Device Sleep' for the second.

  Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 12.2uA.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <avr/wdt.h>                 //watchdog timer library, integral to Arduino IDE
#include <SPI.h>
#include <LowPower.h>                //get the library here; https://github.com/rocketscream/Low-Power

#include <SX128XLT.h>
#include "Settings.h"

SX128XLT LT;

bool SendOK;
int8_t TestPower;
uint8_t TXPacketL;


void loop()
{
  digitalWrite(LED1, HIGH);
  Serial.print(TXpower);
  Serial.print(F("dBm "));
  Serial.print(F("TestPacket1> "));
  Serial.flush();

  if (Send_Test_Packet1())
  {
    packet_is_OK();
  }
  else
  {
    packet_is_Error();
  }
  Serial.println();
  delay(packet_delay);

  LT.setSleep(CONFIGURATION_RETENTION);                        //preserve register settings in sleep.
  Serial.println(F("Sleeping zzzzz...."));
  Serial.println();
  Serial.flush();
  digitalWrite(LED1, LOW);

  sleep1second(15);                                            //goto sleep for 15 seconds

  Serial.println(F("Awake !"));
  Serial.flush();
  digitalWrite(LED1, HIGH);
  LT.wake();

  Serial.print(TXpower);
  Serial.print(F("dBm "));
  Serial.print(F("TestPacket2> "));
  Serial.flush();

  if (Send_Test_Packet2())
  {
    packet_is_OK();
  }
  else
  {
    packet_is_Error();
  }
  Serial.println();
  delay(packet_delay);
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


void packet_is_OK()
{
  Serial.print(F(" "));
  Serial.print(TXPacketL);
  Serial.print(F(" Bytes SentOK"));
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                           //get the IRQ status
  Serial.print(F("SendError,"));
  Serial.print(F("Length,"));
  Serial.print(TXPacketL);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
  digitalWrite(LED1, LOW);                                  //this leaves the LED on slightly longer for a packet error
}


bool Send_Test_Packet1()
{
  uint8_t bufffersize;

  uint8_t buff[] = "Before Device Sleep";
  TXPacketL = sizeof(buff);
  buff[TXPacketL - 1] = '*';

  if (sizeof(buff) > TXBUFFER_SIZE)              //check that defined buffer is not larger than TX_BUFFER
  {
    bufffersize = TXBUFFER_SIZE;
  }
  else
  {
    bufffersize = sizeof(buff);
  }

  TXPacketL = bufffersize;

  LT.printASCIIPacket( (uint8_t*) buff, bufffersize);
  digitalWrite(LED1, HIGH);

  if (LT.transmit( (uint8_t*) buff, TXPacketL, 10000, TXpower, WAIT_TX))
  {
    digitalWrite(LED1, LOW);
    return true;
  }
  else
  {
    return false;
  }
}


bool Send_Test_Packet2()
{
  uint8_t bufffersize;

  uint8_t buff[] = "After Device Sleep";
  TXPacketL = sizeof(buff);
  buff[TXPacketL - 1] = '*';

  if (sizeof(buff) > TXBUFFER_SIZE)              //check that defined buffer is not larger than TX_BUFFER
  {
    bufffersize = TXBUFFER_SIZE;
  }
  else
  {
    bufffersize = sizeof(buff);
  }

  TXPacketL = bufffersize;

  LT.printASCIIPacket( (uint8_t*) buff, bufffersize);
  digitalWrite(LED1, HIGH);

  if (LT.transmit( (uint8_t*) buff, TXPacketL, 10000, TXpower, WAIT_TX))
  {
    digitalWrite(LED1, LOW);
    return true;
  }
  else
  {
    return false;
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
  Serial.println(F("5_LoRa_TX_Sleep_Timed_Wakeup_Atmel Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX_EN, TX_EN, LORA_DEVICE))
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
      led_Flash(50, 50);                            //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  Serial.print(F("Transmitter ready - TXBUFFER_SIZE "));
  Serial.println(TXBUFFER_SIZE);
  Serial.println();
}

