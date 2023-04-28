/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

  The program is a matching receiver program for the '10_LoRa_Link_Test_Transmitter'. The packets received
  are displayed on the serial monitor and analysed to extract the packet data which indicates the power
  used to send the packet. A count is kept of the numbers of each power setting received. When the transmitter
  sends the test mode packet at the beginning of the sequence (displayed as 999) the running totals of the
  powers received are printed. Thus you can quickly see at what transmit power levels the reception fails.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>                                 //the lora device is SPI based so load the SPI library
#include <SX128XLT.h>                            //include the appropriate library   
#include "Settings.h"                            //include the setiings file, frequencies, LoRa settings etc   
#include <ProgramLT_Definitions.h>

SX128XLT LT;                                     //create a library class instance called LT

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];                 //create the buffer that received packets are copied into

uint8_t RXPacketL;                               //stores length of packet received
int16_t  PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                               //stores signal to noise ratio of received packet

uint32_t Test1Count[32];                         //buffer where counts of received packets are stored, -18dbm to +12dBm
uint32_t Mode1_Cycles = 0;                       //count the number of cyles received
bool updateCounts = false;                       //update counts set to tru when first TestMode1 received, at sequence start


void loop()
{
  RXPacketL = LT.receiveAddressed(RXBUFFER, RXBUFFER_SIZE, 15000, WAIT_RX); //wait for a packet to arrive with 15seconds (15000mS) timeout

  digitalWrite(LED1, HIGH);                      //something has happened

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
    delay(25);                                   //gives a slightly longer beep
    digitalWrite(BUZZER, LOW);                   //buzzer off
  }

  digitalWrite(LED1, LOW);                       //LED off

  Serial.println();
}


void packet_is_OK()
{
  uint16_t IRQStatus;

  if (BUZZER > 0)                                //turn buzzer on for a valid packet
  {
    digitalWrite(BUZZER, HIGH);
  }

  IRQStatus = LT.readIrqStatus();                //read the LoRa device IRQ status register

  RXpacketCount++;

  printElapsedTime();                            //print elapsed time to Serial Monitor
  Serial.print(F("  "));
  LT.printASCIIPacket(RXBUFFER, RXPacketL - 3);  //print the packet as ASCII characters

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

  processPacket();
}


void processPacket()
{
  int8_t lTXpower;
  uint8_t packettype;
  uint32_t temp;

  packettype = LT.readRXPacketType();                             //need to know the packet type so we can decide what to do

  if (packettype == TestPacket)
  {
    if (RXBUFFER[0] == ' ')
    {
      lTXpower = 0;
    }

    if (RXBUFFER[0] == '+')
    {
      lTXpower = ((RXBUFFER[1] - 48) * 10) +  (RXBUFFER[2] - 48);   //convert packet text to power
    }

    if (RXBUFFER[0] == '-')
    {
      lTXpower = (((RXBUFFER[1] - 48) * 10) +  (RXBUFFER[2] - 48)) * -1;  //convert packet text to power
    }

    Serial.print(F(" ("));

    if (RXBUFFER[0] != '-')
    {
      Serial.write(RXBUFFER[0]);
    }

    Serial.print(lTXpower);
    Serial.print(F("dBm)"));

    if (updateCounts)
    {
      temp = (Test1Count[lTXpower + 18]);
      Test1Count[lTXpower + 18] = temp + 1;
    }
  }

  if (packettype == TestMode1)
  {
    //this is a command to switch to TestMode1 also updates totals and logs
    updateCounts = true;
    Serial.println();
    Serial.println(F("End test sequence"));

    if (Mode1_Cycles > 0)
    {
      print_Test1Count();
    }

    Serial.println();
    Mode1_Cycles++;
  }

}


void print_Test1Count()
{
  //prints running totals of the powers of received packets
  int8_t index;
  uint32_t j;

  Serial.print(F("Test Packets "));
  Serial.println(RXpacketCount);
  Serial.print(F("Test Cycles "));
  Serial.println(Mode1_Cycles);

  Serial.println();
  for (index = 30; index >= 0; index--)
  {
    Serial.print(index - 18);
    Serial.print(F("dBm,"));
    j = Test1Count[index];
    Serial.print(j);
    Serial.print(F("  "));
  }
  Serial.println();

  Serial.print(F("CSV"));
  for (index = 30; index >= 0; index--)
  {
    Serial.print(F(","));
    j = Test1Count[index];
    Serial.print(j);
  }
  Serial.println();
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register

  printElapsedTime();                               //print elapsed time to Serial Monitor

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
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
    Serial.print(LT.readRXPacketL());               //get the real packet length
    Serial.print(F(",Packets,"));
    Serial.print(RXpacketCount);
    Serial.print(F(",Errors,"));
    Serial.print(errors);
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();                            //print the names of the IRQ registers set
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
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("20_LoRa_Link_Test_Receiver Starting"));
  Serial.println();

  if (BUZZER > 0)
  {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
  }

  //setup SPI, its external to library on purpose, so settings can be mixed and matched with other SPI devices
  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found
  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX_EN, TX_EN, LORA_DEVICE))
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

  //this function call sets up the device for LoRa using the settings from settings.h
  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  Serial.println();
  LT.printModemSettings();                                     //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                                //reads and prints the configured operting settings, useful check
  Serial.println();
  Serial.println();

  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();
}
