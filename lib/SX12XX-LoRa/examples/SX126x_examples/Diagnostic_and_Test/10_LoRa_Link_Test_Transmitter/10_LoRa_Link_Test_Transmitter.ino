/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 01/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program that can be used to test the effectiveness of a LoRa link or its
  attached antennas. Simulations of antenna performance are no substitute for real world tests and this
  simple program allows both long distance link performance to be evaluated and antenna performance to be
  compared.

  The program sends short test packets that reduce in power by 1dBm at a time. The start power is defined
  by start_power and the end power is defined by end_power (see Settings.h file). Once the end_power point
  is reached, the program pauses a short while and starts the transmit sequence again at start_power.
  The packet sent contains the power used to send the packet. By listening for the packets with the basic
  LoRa receive program (4_LoRa_Receiver) you can see the reception results, which should look something
  like this;

  11s  1*T+05,CRC,80B8,RSSI,-73dBm,SNR,9dB,Length,6,Packets,9,Errors,0,IRQreg,50
  12s  1*T+04,CRC,9099,RSSI,-74dBm,SNR,9dB,Length,6,Packets,10,Errors,0,IRQreg,50
  14s  1*T+03,CRC,E07E,RSSI,-75dBm,SNR,9dB,Length,6,Packets,11,Errors,0,IRQreg,50

  Above shows 3 packets received, the first at +05dBm (+05 in printout), the second at 4dBm (+04 in
  printout) and the third at 3dBm (+03) in printout.

  If it is arranged so that reception of packets fails halfway through the sequence by attenuating either the
  transmitter (with an SMA attenuator for instance) or the receiver (by placing it in a tin perhaps) then
  if you swap transmitter antennas you can see the dBm difference in reception, which will be the dBm difference
  (gain) of the antenna.

  To start the sequence a packet is sent with the number 999, when received it looks like this;

  T*1999

  This received packet could be used for the RX program to be able to print totals etc.

  LoRa settings to use for the link test are specified in the 'Settings.h' file.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>
#include <SX126XLT.h>
#include <ProgramLT_Definitions.h>
#include "Settings.h"

SX126XLT LT;

int8_t TestPower;
uint8_t TXPacketL;

void loop()
{
  Serial.println(F("Start Test Sequence"));
  Serial.print(TXpower);
  Serial.print(F("dBm "));
  Serial.print(F("Start Packet> "));

  SendTest1ModePacket();

  Serial.println();

  for (TestPower = start_power; TestPower >= end_power; TestPower--)
  {
    Serial.print(TestPower);
    Serial.print(F("dBm "));
    Serial.print(F("Test Packet> "));
    Serial.flush();
    SendTestPacket(TestPower);
    Serial.println();
    delay(packet_delay);
  }

  Serial.println(F("Finished Test Sequence"));
  Serial.println();
}


void SendTestPacket(int8_t lpower)
{
  //build and send the test packet in addressed form, 3 bytes will be added to begining of packet
  int8_t temppower;
  uint8_t buff[3];                            //the packet is built in this buffer
  TXPacketL = sizeof(buff);

  if (lpower < 0)
  {
    buff[0] = '-';
  }
  else
  {
    buff[0] = '+';
  }

  if (TestPower == 0)
  {
    buff[0] = ' ';
  }

  temppower = TestPower;

  if (temppower < 0)
  {
    temppower = -temppower;
  }

  if (temppower > 19)
  {
    buff[1] = '2';
    buff[2] = ((temppower - 20) + 0x30);
  }
  else if (temppower > 9)
  {
    buff[1] = '1';
    buff[2] = ((temppower - 10) + 0x30);
  }
  else
  {
    buff[1] = '0';
    buff[2] = (temppower + 0x30);
  }

  LT.printASCIIPacket(buff, sizeof(buff));

  digitalWrite(LED1, HIGH);
  TXPacketL = LT.transmitAddressed(buff, sizeof(buff), TestPacket, Broadcast, ThisNode, 5000, lpower, WAIT_TX);
  digitalWrite(LED1, LOW);

  if (TXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }
}


void SendTest1ModePacket()
{
  //used to allow an RX to recognise the start off the sequence and possibly print totals

  uint8_t buff[3];                    //the packet is built in this buffer

  buff[0] = '9';
  buff[1] = '9';
  buff[2] = '9';
  TXPacketL = sizeof(buff);

  LT.printASCIIPacket(buff, sizeof(buff));

  digitalWrite(LED1, HIGH);
  TXPacketL = LT.transmitAddressed(buff, sizeof(buff), TestMode1, Broadcast, ThisNode, 5000, start_power, WAIT_TX);
  delay(mode_delaymS);                //longer delay, so that the start test sequence is obvious
  digitalWrite(LED1, LOW);

  if (TXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }
}


void packet_is_OK()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
  Serial.print(F(" "));
  Serial.print(TXPacketL);
  Serial.print(F(" Bytes SentOK"));
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
  Serial.print(F(" SendError,"));
  Serial.print(F("Length,"));
  Serial.print(TXPacketL);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
  delay(packet_delay);                                     //change LED flash so packet error visible
  delay(packet_delay);
  digitalWrite(LED1, HIGH);
  delay(packet_delay);
  delay(packet_delay);
  digitalWrite(LED1, LOW);
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

  Serial.println(F("10_LoRa_Link_Test_Transmitter Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA_DEVICE))
  {
    Serial.println(F("Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                            //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  LT.printModemSettings();                                    //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                                //reads and prints the configured operting settings, useful check
  Serial.println();
  Serial.println();
  LT.printRegisters(0x800, 0x9FF);                            //print contents of device registers
  Serial.println();
  Serial.println();

  Serial.print(F("Transmitter ready"));
  Serial.println();

}
