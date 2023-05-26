/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 04/11/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a receiver program that can be used to test the throughput of a LoRa
  transmitter. The matching program '42_LoRa_Data_Throughput_Test_Transmitter' is setup to send packets
  that require an acknowledgement before sending the next packet. This will slow down the effective
  throughput. Make sure the lora settings in the 'Settings.h' file match those used in the transmitter.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>                                 //the lora device is SPI based so load the SPI library
#include <SX128XLT.h>                            //include the appropriate library   
#include <ProgramLT_Definitions.h>
#include "Settings.h"                            //include the setiings file, frequencies, LoRa settings etc   

SX128XLT LT;                                     //create a library class instance called LT

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[255];                           //create the buffer that received packets are copied into

uint8_t RXPacketL;                               //stores length of packet received
uint8_t TXPacketL;                               //stores length of packet sent
int16_t PacketRSSI;                              //stores RSSI of received packet
int8_t  PacketSNR;                               //stores signal to noise ratio (SNR) of received packet

uint8_t PacketType;                              //for packet addressing, identifies packet type
uint32_t packetCheck;

void loop()
{
  RXPacketL = LT.receive(RXBUFFER, 255, 60000, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  digitalWrite(LED1, HIGH);                      //something has happened

  if (RXPacketL == 0)                            //if the LT.receive() function detects an error, RXpacketL is 0
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }

  digitalWrite(LED1, LOW);                       //LED off

  Serial.println();
}


void packet_is_OK()
{
  RXpacketCount++;
  Serial.print(RXBUFFER[1]);
  Serial.print(F(" RX"));
  packetCheck = ( (uint32_t) RXBUFFER[4] << 24) + ( (uint32_t) RXBUFFER[3] << 16) + ( (uint32_t) RXBUFFER[2] << 8 ) + (uint32_t) RXBUFFER[1];
  Serial.print(F(",SendACK"));
  sendAck(packetCheck);
}


void sendAck(uint32_t num)
{
  //acknowledge the packet received
  uint8_t len;

  LT.startWriteSXBuffer(0);                   //initialise buffer write at address 0
  LT.writeUint8(ACK);                         //identify type of packet
  LT.writeUint32(num);                        //send the packet check, bytes 1 to 5 of packet
  len = LT.endWriteSXBuffer();                //close buffer write

  digitalWrite(LED1, HIGH);
  TXPacketL = LT.transmitSXBuffer(0, len, 10000, TXpower, WAIT_TX);
  digitalWrite(LED1, LOW);
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();               //read the LoRa device IRQ status register

  if (IRQStatus & IRQ_RX_TIMEOUT)               //check for an RX timeout
  {
    Serial.print(F(",RXTimeout"));
  }
  else
  {
    errors++;
    PacketRSSI = LT.readPacketRSSI();            //read the recived RSSI value
    Serial.print(F("Error"));
    Serial.print(F(",RSSI,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm,Len,"));
    Serial.print(LT.readRXPacketL());            //get the device packet length
  }
}


void printElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
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
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("43_LoRa_Data_Throughput_Acknowledge_Receiver Starting"));
  Serial.println();

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
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
      led_Flash(50, 50);                                       //long fast speed LED flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  Serial.println();
  LT.printModemSettings();                                     //reads and prints the configured LoRa settings, useful check
  Serial.println();
  Serial.println();

  Serial.print(F("Receiver ready"));
  Serial.println();
}
