/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program will receive a lora packet and relay (re-transmit) it. The receiving
  and transmitting can use different frequencies and lora settings, although in this example they are
  the same. The receiving and transmitting settings are in the 'Settings.h' file. If the relay is located
  in an advantageous position, for instance on top of a tall tree, building or in an radio controlled model
  then the range at which trackers or nodes on the ground can be received is considerably increased.
  In these circumstances the relay may listen at a long range setting using SF12 for example and then
  re-transmit back to the ground at SF7.

  For an example of the use of such a program see this report;

  How to Search 500 Square Kilometres in 10 minutes.pdf in the libraries 'Test_Reports' folder.

  Serial monitor baud rate is set at 9600.

*******************************************************************************************************/


#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"

SX126XLT LT;

uint8_t RXPacketL, TXPacketL;
int8_t PacketRSSI, PacketSNR;
uint16_t RXPacketErrors;


void loop()
{
  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  RXPacketL = LT.receiveSXBuffer(0, 0, WAIT_RX); //returns 0 if packet error of some sort, no timeout set

  digitalWrite(LED1, HIGH);                      //something has happened

  if (BUZZER > 0)                                //turn buzzer on
  {
    digitalWrite(BUZZER, HIGH);
  }

  PacketRSSI = LT.readPacketRSSI();              //read the recived RSSI value
  PacketSNR = LT.readPacketSNR();                //read the received SNR value

  if (RXPacketL == 0)                            //if the LT.receive() function detects an error, RXpacketL == 0
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);                    //buzzer off
  }

  Serial.println();
}


void packet_is_OK()
{
  //a packet has been received, so change to relay settings and transmit buffer

  Serial.print(F("PacketOK  "));
  printreceptionDetails();
  delay(packet_delay / 2);
  digitalWrite(LED1, LOW);
  delay(packet_delay / 2);

  Serial.print(F(" Retransmit"));
  LT.setupLoRa(RelayFrequency, RelayOffset, RelaySpreadingFactor, RelayBandwidth, RelayCodeRate, RelayOptimisation);
  digitalWrite(LED1, HIGH);
  TXPacketL = LT.transmitSXBuffer(0, RXPacketL, 10000, TXpower, WAIT_TX);
  Serial.print(F(" - Done"));
  digitalWrite(LED1, LOW);
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  RXPacketErrors++;
  IRQStatus = LT.readIrqStatus();

  led_Flash(5, 50);

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout "));
  }
  else
  {
    Serial.print(F("PacketError "));
    printreceptionDetails();
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();
  }
}


void printreceptionDetails()
{
  Serial.print(F("RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL());
}


void led_Flash(uint16_t flashdelay, uint16_t flashes)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {

    delay(flashdelay);
    digitalWrite(LED1, HIGH);
    delay(flashdelay);
    digitalWrite(LED1, LOW);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  Serial.begin(9600);

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                                            //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);
  Serial.print("ListenSettings,");
  LT.printModemSettings();
  Serial.println();
  LT.setupLoRa(RelayFrequency, RelayOffset, RelaySpreadingFactor, RelayBandwidth, RelayCodeRate, RelayOptimisation);
  Serial.print("RelaySettings,");
  LT.printModemSettings();
  Serial.println();
  Serial.println("Relay Ready");
}


