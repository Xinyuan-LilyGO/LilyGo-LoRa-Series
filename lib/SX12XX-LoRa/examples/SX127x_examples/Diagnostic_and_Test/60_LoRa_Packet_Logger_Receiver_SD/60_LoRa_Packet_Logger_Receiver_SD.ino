/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 02/04/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

  There is a printout and save to SD card of the valid packets received in HEX format. Thus the program
  can be used to receive and record non-ASCII packets. The LED will flash for each packet received and
  the buzzer will sound, if fitted. The measured frequency difference between the frequency used by the
  transmitter and the frequency used by the receiver is shown. If this frequency difference gets to 25%
  of the set LoRa bandwidth, packet reception will fail. The displayed error can be reduced by using the
  'offset' setting in the 'Settings.h' file.

  There will be a limit to how fast the logger can receive packets, mainly caused by the delay in writing
  to SD card, so at high packet rates, packets will be lost.   

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>

#include <SdFat.h>                              //https://github.com/greiman/SdFat
SdFat SD;
File logFile;
char filename[] = "Log000.txt";

bool SD_Found = false;                          //set if SD card found at program startup
uint8_t lognumber;


#include <SX127XLT.h>
SX127XLT LT;

#include "Settings.h"
#include <TimeLib.h>                             //get the library here; https://github.com/PaulStoffregen/Time

uint32_t RXpacketCount;                          //count of good packets
uint32_t errors;                                 //count of packet errors
uint8_t RXPacketL;                               //stores length of packet received
int16_t PacketRSSI;                              //stores RSSI of received packet
int8_t  PacketSNR;                               //stores signal to noise ratio of received packet
uint16_t IRQStatus;                              //used to read the IRQ status
int32_t FreqErrror;                              //frequency error of received packet, in hz


time_t recordtime;                               //used to record the current time, preventing displayed rollover on printing

uint8_t RXBUFFER[RXBUFFER_SIZE];                 //create the buffer that received packets are copied into

#include "SD_Logger_Library.h"


void loop()
{
  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 60000, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  digitalWrite(LED1, HIGH);                       //something has happened
  recordtime = now();                             //stop the time to be displayed rolling over
  printtime();
  printtimeSD();

  PacketRSSI = LT.readPacketRSSI();
  PacketSNR = LT.readPacketSNR();
  FreqErrror = LT.getFrequencyErrorHz();
  IRQStatus = LT.readIrqStatus();

  if (RXPacketL == 0)
  {
    packet_is_Error();
    packet_is_ErrorSD();
  }
  else
  {
    packet_is_OK();
    packet_is_OKSD();
  }

  digitalWrite(LED1, LOW);

  if (BUZZER > 0)
  {
    delay(50);                                   //lets have a slightly longer beep
    digitalWrite(BUZZER, LOW);
  }

  Serial.println();
}


void packet_is_OK()
{

  RXpacketCount++;

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

  Serial.print(F(" FreqErrror,"));
  Serial.print(FreqErrror);
  Serial.print(F("hz  "));

  LT.printHEXPacket(RXBUFFER, RXPacketL);

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
}


void packet_is_Error()
{
  RXPacketL = LT.readRXPacketL();                    //get the real packet length

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    errors++;
    Serial.print(F(" PacketError"));
    Serial.print(F(",RSSI,"));
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


void printDigits(int8_t digits)
{
  //utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(F(":"));
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void printtime()
{
  Serial.print(hour(recordtime));
  printDigits(minute(recordtime));
  printDigits(second(recordtime));
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  Serial.begin(9600);
  Serial.println();
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("60_LoRa_Packet_Logger_Receiver_SD Starting"));
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

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("lora device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);
    }
  }

  lognumber = setup_SDLOG() ;                   //setup SD card
  Serial.print(F("Lognumber "));
  Serial.println(lognumber);

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  LT.printModemSettings();
  Serial.println();
  printModemSettingsSD();
  logFile.println();
  LT.printOperatingSettings();
  Serial.println();
  printOperatingSettingsSD();
  logFile.println();
  printtime();
  Serial.print(F("Receiver ready"));
  Serial.println();
}

