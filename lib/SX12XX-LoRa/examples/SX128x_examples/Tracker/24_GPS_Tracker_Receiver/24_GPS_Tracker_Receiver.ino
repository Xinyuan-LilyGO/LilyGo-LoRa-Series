/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 22/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is an basic receiver for the '23_Simple_GPS_Tracker_Transmitter' program.
  The program reads the received packet from the tracker transmitter and displays the results on
  the serial monitor. The LoRa and frequency settings provided in the Settings.h file must
  match those used by the transmitter.

  The program receives direct from the LoRa devices internal buffer.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.1"

#include <SPI.h>
#include <SX128XLT.h>

SX128XLT LT;

#include "Settings.h"
#include <ProgramLT_Definitions.h>


uint32_t RXpacketCount;        //count of received packets

uint8_t RXPacketL;             //length of received packet
int16_t PacketRSSI;            //RSSI of received packet
int8_t PacketSNR;              //signal to noise ratio of received packet
uint8_t PacketType;            //for packet addressing, identifies packet type
uint8_t Destination;           //for packet addressing, identifies the destination (receiving) node
uint8_t Source;                //for packet addressing, identifies the source (transmiting) node
uint8_t TXStatus;              //A status byte
float TXLat;                   //latitude
float TXLon;                   //longitude
float TXAlt;                   //altitude
uint32_t TXHdop;               //HDOP, indication of fix quality, horizontal dilution of precision, low is good
uint32_t TXGPSFixTime;         //time in mS for fix
uint16_t TXVolts;              //supply\battery voltage
uint8_t TXSats;                //number of sattelites in use
uint32_t TXupTimemS;           //up time of TX in mS


void loop()
{
  RXPacketL = LT.receiveSXBuffer(0, 0, WAIT_RX);       //returns 0 if packet error of some sort

  digitalWrite(LED1, HIGH);

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

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


void readPacketAddressing()
{
  //the transmitter is using packet addressing, so read in the details
  LT.startReadSXBuffer(0);
  PacketType = LT.readUint8();
  Destination = LT.readUint8();
  Source = LT.readUint8();
  LT.endReadSXBuffer();
}


void packet_is_OK()
{
  float tempHdop;

  RXpacketCount++;
  Serial.print(F("Packet OK > "));

  readPacketAddressing();

  if (PacketType == PowerUp)
  {
    LT.startReadSXBuffer(0);
    LT.readUint8();                              //read byte from FIFO, not used
    LT.readUint8();                              //read byte from FIFO, not used
    LT.readUint8();                              //read byte from FIFO, not used
    TXVolts = LT.readUint16();
    LT.endReadSXBuffer();
    Serial.print(F("Tracker transmitter powerup - battery "));
    Serial.print(TXVolts);
    Serial.print(F("mV"));
  }


  if (PacketType == LocationPacket)
  {
    //packet has been received, now read from the SX12XX FIFO in the correct order.
    Serial.print(F("LocationPacket "));
    LT.startReadSXBuffer(0);
    PacketType = LT.readUint8();
    Destination = LT.readUint8();
    Source = LT.readUint8();
    TXLat = LT.readFloat();
    TXLon = LT.readFloat();
    TXAlt = LT.readFloat();
    TXSats = LT.readUint8();
    TXHdop = LT.readUint32();
    TXStatus = LT.readUint8();
    TXGPSFixTime = LT.readUint32();
    TXVolts = LT.readUint16();
    TXupTimemS = LT.readUint32();
    RXPacketL = LT.endReadSXBuffer();

    tempHdop = ( (float) TXHdop / 100);           //need to convert Hdop read from GPS as uint32_t to a float for display

    Serial.write(PacketType);
    Serial.write(Destination);
    Serial.write(Source);
    Serial.print(F(","));
    Serial.print(TXLat, 5);
    Serial.print(F(","));
    Serial.print(TXLon, 5);
    Serial.print(F(","));
    Serial.print(TXAlt, 1);
    Serial.print(F("m,"));
    Serial.print(TXSats);
    Serial.print(F(","));
    Serial.print(tempHdop, 2);
    Serial.print(F(","));
    Serial.print(TXStatus);
    Serial.print(F(","));
    Serial.print(TXGPSFixTime);
    Serial.print(F("mS,"));
    Serial.print(TXVolts);
    Serial.print(F("mV,"));
    Serial.print((TXupTimemS / 1000));
    Serial.print(F("s,"));
    printpacketDetails();
    return;
  }

  if (PacketType == LocationBinaryPacket)
  {
    //packet from locator has been received, now read from the SX12XX FIFO in the correct order.
    Serial.print(F("LocationBinaryPacket "));
    LT.startReadSXBuffer(0);
    PacketType = LT.readUint8();
    Destination = LT.readUint8();
    Source = LT.readUint8();
    TXLat = LT.readFloat();
    TXLon = LT.readFloat();
    TXAlt = LT.readInt16();
    TXStatus = LT.readUint8();
    RXPacketL = LT.endReadSXBuffer();

    tempHdop = ( (float) TXHdop / 100);           //need to convert Hdop read from GPS as uint32_t to a float for display

    Serial.write(PacketType);
    Serial.write(Destination);
    Serial.write(Source);
    Serial.print(F(","));
    Serial.print(TXLat, 5);
    Serial.print(F(","));
    Serial.print(TXLon, 5);
    Serial.print(F(","));
    Serial.print(TXAlt, 0);
    Serial.print(F("m,"));
    Serial.print(TXStatus);
    printpacketDetails();
    return;
  }

  if (PacketType == NoFix)
  {
    Serial.print(F("No Tracker GPS fix "));
    printpacketDetails();
    return;
  }

}


void printpacketDetails()
{
  uint16_t IRQStatus;
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Packets,"));
  Serial.print(RXpacketCount);

  Serial.print(F(",Length,"));
  Serial.print(RXPacketL);
  IRQStatus = LT.readIrqStatus();
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);

}


void packet_is_Error()
{
  uint16_t IRQStatus;

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
  }

  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
  Serial.print(F("PacketError,RSSI"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);

  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL());                  //get the real packet length
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
  digitalWrite(LED1, LOW);

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
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
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("24_GPS_Tracker_Receiver Starting"));

  if (BUZZER >= 0)
  {
    pinMode(BUZZER, OUTPUT);
    Serial.println(F("BUZZER Enabled"));
  }
  else
  {
    Serial.println(F("BUZZER Not Enabled"));
  }

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, RX_EN, TX_EN, LORA_DEVICE))
  {
    Serial.println(F("LoRa device found"));
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                             //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  Serial.println();
  LT.printModemSettings();                           //reads and prints the configured LoRa settings, useful check
  Serial.println();

  Serial.println(F("Receiver ready"));
  Serial.println();
}
