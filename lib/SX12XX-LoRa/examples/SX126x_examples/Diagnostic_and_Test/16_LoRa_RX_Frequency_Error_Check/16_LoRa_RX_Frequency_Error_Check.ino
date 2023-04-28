/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program can be used to check the frequency error between a pair of LoRa 
  devices, a transmitter and receiver. This receiver measures the frequecy error between the receivers
  centre frequency and the centre frequency of the transmitted packet. The frequency difference is shown
  for each packet and an average over 10 received packets reported. Any transmitter program can be used
  to give this program something to listen to, including example program '3_LoRa_Transmit'. 

  Checked for correct operation with lora bandwidths of 500000hz, 125000 and 7800hz. At higher bandwidths
  the reported frequency errors can be within 10-20hz at minimum bandwidth, 7800hz, the reported frequency
  can be circa 100hz out.

  Note: Semtech appear to have stated that the frequency error function that this example uses, is not
  supported for SX126X, for reasons that have not been given, so use at your own risk.   

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/


#define Program_Version "V1.0"

#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"

SX126XLT LT;

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];                 //a buffer is needed to receive packets

uint8_t RXPacketL;                               //stores length of packet received
int8_t  PacketRSSI;                              //stores RSSI of received packet
int8_t  PacketSNR;                               //stores signal to noise ratio of received packet
int32_t totalHzError = 0;                        //used to keep a running total of hZ error for averaging


void loop()
{

  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 0, WAIT_RX); //wait for a packet to arrive

  digitalWrite(LED1, HIGH);                      //something has happened

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
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();

  RXpacketCount++;
  Serial.print(F("PacketOK > "));
  Serial.print(F(" RSSI,"));
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
  printFrequencyError();
}


void printFrequencyError()
{
  int32_t hertzerror, regdata;
  regdata = LT.getFrequencyErrorRegValue();
  hertzerror = LT.getFrequencyErrorHz();
  Serial.print(F("ErrorRegValue,"));
  Serial.print(regdata, HEX);
  Serial.print(F("  PacketHertzError,"));
  Serial.print(hertzerror);
  Serial.println(F("hz"));

  totalHzError = totalHzError + hertzerror;

  if (RXpacketCount == 10)
  {
    Serial.print(F("******** AverageHertzerror "));
    Serial.print((totalHzError / 10));
    Serial.println(F("hz"));
    RXpacketCount = 0;
    totalHzError = 0;
    delay(5000);
  }
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
  errors++;
  Serial.print(F("PacketError,RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL());                  //get the real packet length
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
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
  Serial.println(F("16_LoRa_RX_Frequency_Error_Check Starting"));
  Serial.println();

  SPI.begin();

   if (LT.begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);
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

  Serial.println(F("Receiver ready"));
  Serial.println();
}



