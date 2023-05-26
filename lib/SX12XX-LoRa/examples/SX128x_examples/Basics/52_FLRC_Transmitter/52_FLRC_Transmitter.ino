/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/09/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a test transmitter for the Fast Long Range Communication (FLRC) mode
  introduced in the SX128X devices. A packet containing ASCII text is sent according to the frequency and
  FLRC settings specified in the 'Settings.h' file. The pins to access the SX128X device need to be defined
  in the 'Settings.h' file also.

  The details of the packet sent and any errors are shown on the Serial Monitor, together with the transmit
  power used, the packet length and the CRC of the packet. The matching receive program, '53_FLRC_Receiver'
  can be used to check the packets are being sent correctly, the frequency and FLRC settings (in Settings.h)
  must be the same for the Transmit and Receive program. Sample Serial Monitor output;

  10dBm Packet> {packet contents*}  BytesSent,23  CRC,DAAB  TransmitTime,54mS  PacketsSent,1

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#include <SPI.h>                                               //the SX128X device is SPI based so load the SPI library                                         
#include <SX128XLT.h>                                          //include the appropriate library  
#include "Settings.h"                                          //include the setiings file, frequencies, LoRa settings etc   

SX128XLT LT;                                                   //create a library class instance called LT

uint8_t TXPacketL;
uint32_t TXPacketCount, startmS, endmS;

uint8_t buff[] = "Hello World 1234567890";

void loop()
{
  Serial.print(TXpower);                                       //print the transmit power defined
  Serial.print(F("dBm "));
  Serial.print(F("Packet> "));
  Serial.flush();

  TXPacketL = sizeof(buff);                                    //set TXPacketL to length of array
  buff[TXPacketL - 1] = '*';                                   //replace null character at buffer end so its visible on reciver

  LT.printASCIIPacket(buff, TXPacketL);                        //print the buffer (the sent packet) as ASCII

  digitalWrite(LED1, HIGH);
  startmS =  millis();                                         //start transmit timer

  TXPacketL = LT.transmit(buff, TXPacketL, 10000, TXpower, WAIT_TX);  //will return 0 if transmit fails, timeout 10 seconds

  if (TXPacketL > 0)
  {
    endmS = millis();                                          //packet sent, note end time
    TXPacketCount++;
    packet_is_OK();
  }
  else
  {
    packet_is_Error();                                 //transmit packet returned 0, so there was an error
  }

  digitalWrite(LED1, LOW);
  Serial.println();
  delay(packet_delay);                                 //have a delay between packets
}


void packet_is_OK()
{
  //if here packet has been sent OK
  uint16_t localCRC;

  Serial.print(F("  BytesSent,"));
  Serial.print(TXPacketL);                             //print transmitted packet length
  localCRC = LT.CRCCCITT(buff, TXPacketL, 0xFFFF);
  Serial.print(F("  CRC,"));
  Serial.print(localCRC, HEX);                              //print CRC of sent packet
  Serial.print(F("  TransmitTime,"));
  Serial.print(endmS - startmS);                       //print transmit time of packet
  Serial.print(F("mS"));
  Serial.print(F("  PacketsSent,"));
  Serial.print(TXPacketCount);                         //print total of packets sent OK
}


void packet_is_Error()
{
  //if here there was an error transmitting packet
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                  //read the the interrupt register
  Serial.print(F(" SendError,"));
  Serial.print(F("Length,"));
  Serial.print(TXPacketL);                         //print transmitted packet length
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);                    //print IRQ status
  LT.printIrqStatus();                             //prints the text of which IRQs set
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
  pinMode(LED1, OUTPUT);                                   //setup pin as output for indicator LED
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.println(F("52_FLRC_Transmitter Starting"));

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found
  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, RX_EN, TX_EN, LORA_DEVICE))
  {
    Serial.println(F("FLRC Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
    delay(1000);
  }
  else
  {
    Serial.println(F("No FLRC device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  LT.setupFLRC(Frequency, Offset, BandwidthBitRate, CodingRate, BT, Syncword);

  //The full details of the setupFLRC function call above are listed below
  //***************************************************************************************************
  //Setup FLRC
  //***************************************************************************************************
  //LT.setMode(MODE_STDBY_RC);
  //LT.setRegulatorMode(USE_LDO);
  //LT.setPacketType(PACKET_TYPE_FLRC);
  //LT.setRfFrequency(Frequency, Offset);
  //LT.setBufferBaseAddress(0, 0);
  //LT.setModulationParams(BandwidthBitRate, CodingRate, BT);
  //LT.setPacketParams(PREAMBLE_LENGTH_32_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 127, RADIO_CRC_3_BYTES, RADIO_WHITENING_OFF);
  //LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);              //set for IRQ on TX done and timeout on DIO1
  //LT.setSyncWord1(Syncword);
  //***************************************************************************************************

  Serial.println();
  LT.printModemSettings();                               //reads and prints the configured modem settings, useful check
  Serial.println();
  LT.printOperatingSettings();                           //reads and prints the configured operating settings, useful check
  Serial.println();
  Serial.println();
  LT.printRegisters(0x900, 0x9FF);                       //print contents of device registers
  Serial.println();
  Serial.println();

  Serial.print(F("Transmitter ready"));
  Serial.println();
}
