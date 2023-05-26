/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 01/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

  There is a printout of the valid packets received, the packet is assumed to be in ASCII printable text,
  if its not ASCII text characters from 0x20 to 0x7F, expect weird things to happen on the Serial Monitor.
  The LED will flash for each packet received and the buzzer will sound, if fitted.

  Sample serial monitor output;

  1109s  {packet contents}  CRC,3882,RSSI,-69dBm,SNR,10dB,Length,19,Packets,1026,Errors,0,IRQreg,50

  If there is a packet error it might look like this, which is showing a CRC error,

  1189s PacketError,RSSI,-111dBm,SNR,-12dB,Length,0,Packets,1126,Errors,1,IRQreg,70,IRQ_HEADER_VALID,IRQ_CRC_ERROR,IRQ_RX_DONE

  A summary of the packet reception is sent to the OLED display as well, useful for portable applications.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>                                 //the lora device is SPI based so load the SPI library
#include <SX128XLT.h>                            //include the appropriate library   
#include "Settings.h"                            //include the setiings file, frequencies, LoRa settings etc   

SX128XLT LT;                                     //create a library class instance called LT

#include <U8x8lib.h>                                      //get library here >  https://github.com/olikraus/u8g2 
U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);    //use this line for standard 0.96" SSD1306
//U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);   //use this line for 1.3" OLED often sold as 1.3" SSD1306


uint32_t RXpacketCount;
uint32_t RXpacketErrors;
uint16_t IRQStatus;

uint8_t RXBUFFER[RXBUFFER_SIZE];                 //create the buffer that received packets are copied into
uint8_t RXPacketL;                               //stores length of packet received
int16_t  PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                               //stores signal to noise ratio of received packet


void loop()
{
  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 0, WAIT_RX); //wait for a packet to arrive with no timeout

  digitalWrite(LED1, HIGH);                      //something has happened

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);                  //buzzer on
  }

  PacketRSSI = LT.readPacketRSSI();              //read the recived RSSI value
  PacketSNR = LT.readPacketSNR();                //read the received SNR value
  IRQStatus = LT.readIrqStatus();                //read the LoRa device IRQ status register

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

  digitalWrite(LED1, LOW);                        //LED off

  Serial.println();
}


void packet_is_OK()
{
  uint16_t localCRC;

  RXpacketCount++;

  printElapsedTime();                              //print elapsed time to Serial Monitor
  Serial.print(F("  "));
  LT.printASCIIPacket(RXBUFFER, RXPacketL);        //print the packet as ASCII characters

  localCRC = LT.CRCCCITT(RXBUFFER, RXPacketL, 0xFFFF);  //calculate the CRC, this is the external CRC calculation of the RXBUFFER
  Serial.print(F(",CRC,"));                        //contents, not the LoRa device internal CRC
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
  Serial.print(RXpacketErrors);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);

  disp.clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("OK"));
  dispscreen1();
}


void packet_is_Error()
{
  printElapsedTime();                               //print elapsed time to Serial Monitor

  RXpacketErrors++;
  Serial.print(F(" PacketError"));
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL());               //get the real packet length
  Serial.print(F(",Packets,"));
  Serial.print(RXpacketCount);
  Serial.print(F(",Errors,"));
  Serial.print(RXpacketErrors);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();                            //print the names of the IRQ registers set
  disp.clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("Packet Error"));
  dispscreen1();

  delay(500);                                       //gives longer buzzer and LED falsh for error

}


void printElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}


void dispscreen1()
{
  disp.clearLine(1);
  disp.setCursor(0, 1);
  disp.print(F("RSSI    "));
  disp.print(PacketRSSI);
  disp.print(F("dBm"));
  disp.clearLine(2);
  disp.setCursor(0, 2);
  disp.print(F("SNR     "));

  if (PacketSNR > 0)
  {
    disp.print(F("+"));
  }

  disp.print(PacketSNR);
  disp.print(F("dB"));
  disp.clearLine(3);
  disp.setCursor(0, 3);
  disp.print(F("Length  "));
  disp.print(LT.readRXPacketL());
  disp.clearLine(4);
  disp.setCursor(0, 4);
  disp.print(F("Packets "));
  disp.print(RXpacketCount);
  disp.clearLine(5);
  disp.setCursor(0, 5);
  disp.print(F("Errors  "));
  disp.print(RXpacketErrors);
  disp.clearLine(6);
  disp.setCursor(0, 6);
  disp.print(F("IRQreg  "));
  disp.print(IRQStatus, HEX);
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
  Serial.println(F("33_LoRa_RSSI_Checker_With_Display Starting"));
  Serial.println();

  if (BUZZER > 0)
  {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
  }

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  disp.begin();
  disp.setFont(u8x8_font_chroma48medium8_r);

  disp.clear();
  disp.setCursor(0, 0);
  disp.print(F("Check LoRa"));
  disp.setCursor(0, 1);

  //setup hardware pins used by device, then check if device is found
  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX_EN, TX_EN, LORA_DEVICE))
  {
    disp.print(F("LoRa OK"));
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      disp.print(F("Device error"));
      led_Flash(50, 50);                                       //long fast speed LED flash indicates device error
    }
  }

  //this function call sets up the device for LoRa using the settings from settings.h
  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  Serial.println();
  LT.printModemSettings();                                     //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                                 //reads and prints the configured operting settings, useful check
  Serial.println();
  Serial.println();
  LT.printRegisters(0x900, 0x9FF);                               //print contents of device registers
  Serial.println();
  Serial.println();

  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();
}
