/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program demonstrates the transmitting of a structure as a LoRa packet. The
  contents of the structure are the same as in the '8_LoRa_LowMemory_TX' program. The packet sent is
  typical of what might be sent from a GPS tracker.

  The structure type is defined as trackerPacket and an instance called location1 is created. The struture
  which includes a character array (text) is filled with values and transmitted.

  The matching receiving program '15_LoRa_RX_Structure' can be used to receive and display the packet,
  though the program '9_LoRa_LowMemory_RX' should receive it as well, since the contents are the same.

  Note that the structure definition and variable order (including the buffer size) used in the transmitter
  need to match those used in the receiver.

  The contents of the packet transmitted should be;

  "tracker1"     (buffer)      - trackerID
  1+             (uint32_t)    - packet count
  51.23456       (float)       - latitude
  -3.12345       (float)       - longitude
  199            (uint16_t)    - altitude
  8              (uint8_t)     - number of satellites
  3999           (uint16_t)    - battery voltage
  -9             (int8_t)      - temperature

  Good luck.

  Serial monitor baud rate is set at 9600.

*******************************************************************************************************/

#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"

SX126XLT LT;

uint32_t TXpacketCount = 1;
uint32_t startmS, endmS;

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

struct trackerPacket location1;                        //define an instance called location1 of the structure trackerPacket


void loop()
{

  //fill the defined structure with values
  uint8_t buff[] = "tracker1";                         //create the contents to be of location1.trackerID
  memcpy (&location1.trackerID, &buff, sizeof(buff));  //copy the contents of buff[] into the structure
  location1.txcount = TXpacketCount;
  location1.latitude = 51.23456;
  location1.longitude = -3.12345;
  location1.altitude = 199;
  location1.satellites = 8;
  location1.voltage = 3999;
  location1.temperature = -9;

  digitalWrite(LED1, HIGH);
  startmS = millis();

  if (LT.transmit((uint8_t *) &location1, sizeof(location1), 0, TXpower, WAIT_TX))  //will return packet length sent if OK, otherwise 0
  {
    endmS = millis();
    digitalWrite(LED1, LOW);
    TXpacketCount++;
    Serial.print(TXpacketCount);
    Serial.print(F(" "));
    Serial.print(sizeof(location1));
    Serial.print(F(" Bytes Sent"));
    Serial.print(F(" "));
    Serial.print(endmS - startmS);
    Serial.print(F("mS"));
  }
  else
  {
    Serial.print(F("Send Error - IRQreg,"));
    Serial.print(LT.readIrqStatus(), HEX);
  }

  digitalWrite(LED1, LOW);
  Serial.println();
  delay(packet_delay);
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
  pinMode(LED1, OUTPUT);                       //setup pin as output for indicator LED
  led_Flash(2, 125);                           //two quick LED flashes to indicate program start

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
      led_Flash(50, 50);                        //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Transmitter ready"));
  Serial.println();
}

