/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is a remote control transmitter. When one of four switches are made
  (shorted to ground) a packet is transmitted with single byte indicating the state of Switch0 as bit 0,
  Switch1 as bit 1 and Switch2 as bit 2. To prevent false triggering at the receiver the packet contains a
  32 bit number called the TXIdentity which in this example is set to 1234554321. The receiver will only
  act on, change the state of the outputs, if the identity set in the receiver matches that of the
  transmitter. The chance of a false trigger is fairly remote.

  Between switch presses the LoRa device and Atmel microcontroller are put to sleep. A switch press wakes
  up the processor from sleep, the switches are read and a packet sent. On a 'bare bones' Arduino setup
  the transmitter has a sleep current of approx 2.2uA, so it's ideal for a battery powered remote control
  with a potential range of many kilometres.

  The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file. These settings
  are not necessarily optimised for long range.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"
#include <ProgramLT_Definitions.h>

#include <avr/wdt.h>                        //watchdog timer library, integral to Arduino IDE
#include <avr/sleep.h>                      //watchdog timer library, integral to Arduino IDE
#include "PinChangeInterrupt.h"             //get the library here; https://github.com/NicoHood/PinChangeInterrupt

SX127XLT LT;

uint32_t TXpacketCount;
uint8_t TXPacketL;

volatile bool switch0flag = false;
volatile bool switch1flag = false;
volatile bool switch2flag = false;
volatile bool switch3flag = false;

void loop()
{
  uint8_t switches;

  digitalWrite(LED1, LOW);                  //turn off indicator LED
  Serial.print(F("Sleeping zzzz"));
  Serial.flush();                           //make sure all serial output has gone

  LT.setSleep(CONFIGURATION_RETENTION);     //sleep LoRa device, keeping register settings in sleep.
  sleep_permanent();                        //sleep Atmel processor permanently for switch wakeup only

  digitalWrite(LED1, HIGH);

  Serial.println(F(" - Awake !!"));         //the processor has woken up
  switches = readSwitches();                //read the state of the switches

  TXpacketCount++;
  Serial.print(TXpacketCount);              //print the numbers of sends
  Serial.print(F(" Sending > "));

  Serial.print(switches, BIN);

  if (sendSwitchPacket(switches))
  {
    Serial.println(F("  SentOK"));
  }
  else
  {
    Serial.print(F("Send Error - IRQreg,"));
    Serial.print(LT.readIrqStatus(), HEX);
  }

  Serial.println();
  delay(500);
}


uint8_t sendSwitchPacket(uint8_t switches)
{
  //The SX12XX buffer is filled with variables of a known type and in a known sequence. Make sure the
  //receiver uses the same variable types and sequence to read variables out of the receive buffer.
  uint8_t len;

  LT.startWriteSXBuffer(0);                 //start the write packet to buffer process
  LT.writeUint8(RControl1);                 //this byte identifies the type of packet
  LT.writeUint32(TXIdentity);               //this 32bit integer defines the Identity of the transmiter
  LT.writeUint8(switches);                  //this byte contains the 8 switch values to be sent
  len = LT.endWriteSXBuffer();              //close the packet, get the length of data to be sent

  //now transmit the packet, 10 second timeout, and wait for it to complete sending
  TXPacketL = LT.transmitSXBuffer(0, len, 10000, TXpower, WAIT_TX);

  return TXPacketL;                         //TXPacketL will be 0 if there was an error sending
}


void sleep_permanent()
{
  attachInterrupts();

  ADCSRA = 0;                //disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();           //timed sequence follows
  sleep_enable();

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);  //turn on brown-out enable select
  MCUCR = bit (BODS);        //this must be done within 4 clock cycles of above
  interrupts ();             //guarantees next instruction executed

  sleep_cpu ();              //sleep within 3 clock cycles of above

  /* wake up here */

  sleep_disable();

  detachInterrupts();
}


void attachInterrupts()
{
  if (SWITCH0  >= 0)
  {
    attachPCINT(digitalPinToPCINT(SWITCH0), wake0, FALLING);
    switch0flag = false;
  }

  if (SWITCH1  >= 0)
  {
    attachPCINT(digitalPinToPCINT(SWITCH1), wake1, FALLING);
    switch1flag = false;
  }

  if (SWITCH2  >= 0)
  {
    attachPCINT(digitalPinToPCINT(SWITCH2), wake2, FALLING);
    switch2flag = false;
  }

  if (SWITCH3  >= 0)
  {
    attachPCINT(digitalPinToPCINT(SWITCH3), wake3, FALLING);
    switch3flag = false;
  }

}


void detachInterrupts()
{
  if (SWITCH0  >= 0)
  {
    detachPCINT(digitalPinToPCINT(SWITCH0));
  }

  if (SWITCH1  >= 0)
  {
    detachPCINT(digitalPinToPCINT(SWITCH1));
  }

  if (SWITCH2  >= 0)
  {
    detachPCINT(digitalPinToPCINT(SWITCH2));
  }

  if (SWITCH3  >= 0)
  {
    detachPCINT(digitalPinToPCINT(SWITCH3));
  }

}




void wake0()
{
  switch0flag = true;
}


void wake1()
{
  switch1flag = true;
}


void wake2()
{
  switch2flag = true;
}


void wake3()
{
  switch3flag = true;
}


uint8_t readSwitches()
{
  uint8_t switchByte = 0xFF;                  //start assuming all switches off

  if (switch0flag)
  {
    bitClear(switchByte, 0);                  //if the flag is set clear the bit
    Serial.println(F("SWITCH0 pressed"));
  }

  if (switch1flag)
  {
    bitClear(switchByte, 1);                  //if the flag is set clear the bit
    Serial.println(F("SWITCH1 pressed"));
  }

  if (switch2flag)
  {
    bitClear(switchByte, 2);                  //if the flag is set clear the bit
    Serial.println(F("SWITCH2 pressed"));
  }


  if (switch3flag)
  {
    bitClear(switchByte, 3);                  //if the flag is set clear the bit
    Serial.println(F("SWITCH3 pressed"));
  }

  return switchByte;
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


void setupSwitches()
{
  if (SWITCH0  >= 0)
  {
    pinMode(SWITCH0, INPUT_PULLUP);
  }

  if (SWITCH1  >= 0)
  {
    pinMode(SWITCH1, INPUT_PULLUP);
  }

  if (SWITCH2  >= 0)
  {
    pinMode(SWITCH2, INPUT_PULLUP);
  }

  if (SWITCH3  >= 0)
  {
    pinMode(SWITCH3, INPUT_PULLUP);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  setupSwitches();

  Serial.begin(9600);

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                    //long fast speed flash indicates LoRa device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Transmitter ready"));
  Serial.println();

}

