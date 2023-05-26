/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/12/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program listens for incoming packets using the LoRa settings in the 'Settings.h'
  file. The pins to access the LoRa device need to be defined in the 'Settings.h' file also.

  There is a printout to serial monitor of the hex values of a valid valid packet received. The LED will
  flash for each packet received and the buzzer will sound, if fitted.

  The 128x64 OLED will display the number of packets received OK, those with errors and the RSSI and SNR of
  the last received packet. At startup the display will show the set frequency, LoRa spreading factor, LoRa
  bandwidth and the SD logfile name used, if an SD card is detected.

  Sample serial monitor output;

  122s  RSSI,-48dBm,SNR,9dB,Length,23,Packets,1,Errors,0,IRQreg,50  48 65 6C .........

  If there is a packet error it might look like this, which is showing a CRC error,

  127s PacketError, RSSI,-116dBm,SNR,-11dB,Length,23,Packets,5,Errors,1,IRQreg,70,IRQ_HEADER_VALID,IRQ_CRC_ERROR,IRQ_RX_DONE

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                   //the LoRa device is SPI based so load the SPI library
#include <SX127XLT.h>                              //include the appropriate LoRa library   
#include "Settings.h"                              //include the settings file, frequencies, LoRa settings etc   
SX127XLT LoRa;                                     //create a library class instance called LoRa

#include <TimeLib.h>                               //get the library here > https://github.com/PaulStoffregen/Time
time_t recordtime;                                 //used to record the current time

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>                 //get library here > https://github.com/mathertel/LiquidCrystal_PCF8574
LiquidCrystal_PCF8574 disp(DisplayAddress);        //set the LCD address to 0x3F, could be 0x27 also

#include <SdFat.h>                                 //get the library here > https://github.com/greiman/SdFat
SdFat SD;
File logFile;
char filename[] = "Log000.txt";                    //base name for logfile
bool SD_Found = false;                             //variable set if SD card found at program startup
uint8_t lognumber;

uint32_t RXpacketCount;
uint32_t RXpacketErrors;
uint16_t IRQStatus;

uint8_t RXBUFFER[RXBUFFER_SIZE];                   //create the buffer that received packets are copied into
uint8_t RXPacketL;                                 //stores length of packet received
int16_t PacketRSSI;                                //stores RSSI of received packet
int8_t  PacketSNR;                                 //stores signal to noise ratio of received packet


void loop()
{
  RXPacketL = LoRa.receive(RXBUFFER, RXBUFFER_SIZE, RXtimeoutmS, WAIT_RX);

  digitalWrite(LED1, HIGH);                        //something has happened, so LED on

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);                    //buzzer on
  }

  PacketRSSI = LoRa.readPacketRSSI();              //read the recived RSSI value
  PacketSNR = LoRa.readPacketSNR();                //read the received SNR value
  IRQStatus = LoRa.readIrqStatus();                //read the LoRa device IRQ status register

  printElapsedTime();                              //print seconds to monitor

  if (RXPacketL == 0)                              //if the LoRa.receive() function detects an error, RXpacketL == 0
  {
    packet_is_Error();

    if (SD_Found)
    {
      printElapsedTimeSD();
      packet_is_ErrorSD();
    }

  }
  else
  {
    packet_is_OK();

    if (SD_Found)
    {
      printElapsedTimeSD();
      packet_is_OKSD();
    }

  }

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);                    //buzzer off
  }

  digitalWrite(LED1, LOW);                        //LED off

}


void packet_is_OK()
{
  RXpacketCount++;

  Serial.print(F(" RSSI,"));
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
  Serial.print(F("  "));
  printHEXPacket(RXBUFFER, RXPacketL);              //print the HEX values of packet
  Serial.println();
  clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("Received OK "));
  dispscreen1();
}


void printHEXPacket(uint8_t *buffer, uint8_t size)
{
  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByte(buffer[index]);
    Serial.print(F(" "));
  }
}


void printHEXByte(uint8_t temp)
{
  if (temp < 0x10)
  {
    Serial.print(F("0"));
  }
  Serial.print(temp, HEX);
}


void packet_is_Error()
{
  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.println(F(" RXTimeout"));
    clearLine(0);
    disp.setCursor(0, 0);
    disp.print(F("RXTimeout"));
    return;
  }

  RXpacketErrors++;
  Serial.print(F(" PacketError"));
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LoRa.readRXPacketL());               //get the real packet length
  Serial.print(F(",Packets,"));
  Serial.print(RXpacketCount);
  Serial.print(F(",Errors,"));
  Serial.print(RXpacketErrors);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LoRa.printIrqStatus();                            //print the names of the IRQ registers set
  clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("Packet Error"));
  dispscreen1();
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
  disp.print(F("Len "));
  disp.print(RXPacketL);
  clearLine(1);
  disp.setCursor(0, 1);
  disp.print(F("OK "));
  disp.print(RXpacketCount);
  disp.print(F(" Errors "));
  disp.print(RXpacketErrors);
  clearLine(2);
  disp.setCursor(0, 2);
  disp.print(F("RSSI"));
  disp.print(PacketRSSI);
  disp.print(F("dBm "));

  disp.print(F("SNR"));

  if (PacketSNR > 0)
  {
    disp.print(F("+"));
  }

  disp.print(PacketSNR);
  disp.print(F("dB"));
}


void dispscreen2()
{
  disp.clear();
  disp.setCursor(0, 0);
  disp.print(F("Frequency "));
  disp.print(Frequency);
  disp.setCursor(0, 1);
  disp.print(F("Bandwidth "));
  disp.print(LoRa.returnBandwidth(Bandwidth));
  disp.setCursor(0, 2);
  disp.print(F("SpreadingFactor "));
  disp.print(SpreadingFactor);

  disp.setCursor(0, 3);

  if (SD_Found)
  {
    disp.print(F("Log "));
    disp.print(filename);
  }
  else
  {
    disp.print(F("No SD card"));
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


//******************************************************
//SD logger routines
//******************************************************

void printElapsedTimeSD()
{
  float seconds;
  seconds = millis() / 1000;
  logFile.print(seconds, 0);
  logFile.print(F("s"));
}


void printModemSettingsSD()
{
  uint8_t regdata;
  uint8_t sf;
  uint32_t bandwidth;
  uint8_t cr;
  uint8_t opt;
  uint16_t syncword;
  uint8_t  invertIQ;
  uint16_t preamble;
  uint32_t freqint;

  if (LORA_DEVICE == DEVICE_SX1272)
  {
    regdata = (LoRa.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_2);
  }
  else
  {
    regdata = (LoRa.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_X);
  }

  //get al the data frome the lora device in one go to avoid swapping
  //devices on the SPI bus all the time

  if (LORA_DEVICE == DEVICE_SX1272)
  {
    regdata = (LoRa.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_2);
  }
  else
  {
    regdata = (LoRa.readRegister(REG_MODEMCONFIG1) & READ_BW_AND_X);
  }

  bandwidth = LoRa.returnBandwidth(regdata);
  freqint = LoRa.getFreqInt();
  sf = LoRa.getLoRaSF();
  cr = LoRa.getLoRaCodingRate();
  opt = LoRa.getOptimisation();
  syncword = LoRa.getSyncWord();
  invertIQ = LoRa.getInvertIQ();
  preamble = LoRa.getPreamble();

  printDeviceSD();
  logFile.print(F(", "));
  logFile.print(freqint);
  logFile.print(F("hz, SF"));
  logFile.print(sf);

  logFile.print(F(", BW"));
  logFile.print(bandwidth);

  logFile.print(F(", CR4: "));
  logFile.print(cr);
  logFile.print(F(", LDRO_"));

  if (opt)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(", SyncWord_0x"));
  logFile.print(syncword, HEX);

  if (invertIQ == LORA_IQ_INVERTED)
  {
    logFile.print(F(", IQInverted"));
  }
  else
  {
    logFile.print(F(", IQNormal"));
  }
  logFile.print(F(", Preamble_"));
  logFile.print(preamble);
  logFile.flush();
}


void printOperatingSettingsSD()
{
  //get al the data frome the lora device in one go to avoid swapping
  //devices on the SPI bus all the time

  uint8_t ver = LoRa.getVersion();
  uint8_t pm = LoRa.getPacketMode();
  uint8_t hm = LoRa.getHeaderMode();
  uint8_t crcm = LoRa.getCRCMode();
  uint8_t agc = LoRa.getAGC();
  uint8_t lnag = LoRa.getLNAgain();
  uint8_t boosthf = LoRa.getLNAboostHF();
  uint8_t boostlf = LoRa.getLNAboostLF();
  uint8_t opmode = LoRa.getOpmode();

  printDeviceSD();
  logFile.print(F(", "));

  printOperatingModeSD(opmode);

  logFile.print(F(", Version_"));
  logFile.print(ver, HEX);

  logFile.print(F(", PacketMode_"));

  if (pm)
  {
    logFile.print(F("LoRa"));
  }
  else
  {
    logFile.print(F("FSK"));
  }

  if (hm)
  {
    logFile.print(F(", Implicit"));
  }
  else
  {
    logFile.print(F(", Explicit"));
  }

  logFile.print(F(", CRC_"));
  if (crcm)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(", AGCauto_"));
  if (agc)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(", LNAgain_"));
  logFile.print(lnag);

  logFile.print(F(", LNAboostHF_"));
  if (boosthf)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }

  logFile.print(F(", LNAboostLF_"));
  if (boostlf)
  {
    logFile.print(F("On"));
  }
  else
  {
    logFile.print(F("Off"));
  }
  logFile.flush();
}


void printOperatingModeSD(uint8_t opmode)
{
  switch (opmode)
  {
    case 0:
      logFile.print(F("SLEEP"));
      break;

    case 1:
      logFile.print(F("STDBY"));
      break;

    case 2:
      logFile.print(F("FSTX"));
      break;

    case 3:
      logFile.print(F("TX"));
      break;

    case 4:
      logFile.print(F("FSRX"));
      break;

    case 5:
      logFile.print(F("RXCONTINUOUS"));
      break;

    case 6:
      logFile.print(F("RXSINGLE"));
      break;

    case 7:
      logFile.print(F("CAD"));
      break;

    default:
      logFile.print(F("NOIDEA"));
      break;
  }
}


uint8_t setup_SDLOG()
{
  //checks if the SD card is present and can be initialised
  //returns log number, 1-99, if OK, 0 if not

  uint8_t i;

  Serial.print(F("SD card..."));

  if (!SD.begin(SDCS))
  {
    Serial.println(F("ERROR SD card fail"));
    Serial.println();
    SD_Found = false;
    return 0;
  }

  Serial.print(F("Initialized OK - "));
  SD_Found = true;

  for (i = 1; i < 100; i++)
  {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename))
    {
      // only open a new file if it doesn't exist
      logFile = SD.open(filename, FILE_WRITE);
      break;
    }
  }

  Serial.print(F("Writing to "));
  Serial.println(filename);

  return i;
}


void printDeviceSD()
{

  switch (LORA_DEVICE)
  {
    case DEVICE_SX1272:
      logFile.print(F("SX1272"));
      break;

    case DEVICE_SX1276:
      logFile.print(F("SX1276"));
      break;

    case DEVICE_SX1277:
      logFile.print(F("SX1277"));
      break;

    case DEVICE_SX1278:
      logFile.print(F("SX1278"));
      break;

    case DEVICE_SX1279:
      logFile.print(F("SX1279"));
      break;

    default:
      logFile.print(F("Unknown Device"));
  }
}


void printHEXPacketSD(uint8_t *buffer, uint8_t size)
{
  uint8_t index;

  for (index = 0; index < size; index++)
  {
    printHEXByteSD(buffer[index]);
    logFile.print(F(" "));
  }
}


void printHEXByteSD(uint8_t temp)
{
  if (temp < 0x10)
  {
    logFile.print(F("0"));
  }
  logFile.print(temp, HEX);
}


void packet_is_OKSD()
{
  IRQStatus = LoRa.readIrqStatus();

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

  logFile.print(F("RSSI,"));
  logFile.print(PacketRSSI);
  logFile.print(F("dBm,SNR,"));
  logFile.print(PacketSNR);
  logFile.print(F("dB,Length,"));
  logFile.print(RXPacketL);
  logFile.print(F(",Packets,"));
  logFile.print(RXpacketCount);
  logFile.print(F(",Errors,"));
  logFile.print(RXpacketErrors);
  logFile.print(F(",IRQreg,"));
  logFile.print(IRQStatus, HEX);
  logFile.print(F(",FreqErrror,"));
  logFile.print(LoRa.getFrequencyErrorHz());
  logFile.print(F("hz  "));
  printHEXPacketSD(RXBUFFER, RXPacketL);
  logFile.println();
  logFile.flush();
}


void packet_is_ErrorSD()
{
  IRQStatus = LoRa.readIrqStatus();                    //get the IRQ status
  RXPacketL = LoRa.readRXPacketL();                    //get the real packet length

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    logFile.println(F(" RXTimeout"));
  }
  else
  {
    logFile.print(F(" PacketError"));
    logFile.print(F(",RSSI,"));
    logFile.print(PacketRSSI);
    logFile.print(F("dBm,SNR,"));
    logFile.print(PacketSNR);
    logFile.print(F("dB,Length,"));
    logFile.print(RXPacketL);
    logFile.print(F(",Packets,"));
    logFile.print(RXpacketCount);
    logFile.print(F(",Errors,"));
    logFile.print(RXpacketErrors);
    logFile.print(F(",IRQreg,"));
    logFile.println(IRQStatus, HEX);
  }
  logFile.flush();
}


void printDigitsSD(int8_t digits)
{
  //utility function for digital clock display: prints preceding colon and leading 0
  logFile.print(F(": "));
  if (digits < 10)
    logFile.print('0');
  logFile.print(digits);
}


void printtimeSD()
{
  logFile.print(hour(recordtime));
  printDigitsSD(minute(recordtime));
  printDigitsSD(second(recordtime));
}


void clearLine(uint8_t linenum)
{
  disp.setCursor(0, linenum);
  disp.print(F("                    "));
}


void setup()
{
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);                            //two quick LED flashes to indicate program start

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("74_LoRa_Packet_Logger_With_20x4LCD_Display"));
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

  Wire.begin();
  disp.begin(20, 4);                       //initialize the lcd for 20 chars 4 lines, turn on backlight
  disp.setBacklight(1);                    //backlight on
  disp.setCursor(0, 0);
  disp.print(F("Check LoRa"));

  //setup hardware pins used by device, then check if device is found
  if (LoRa.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    disp.setCursor(0, 1);
    disp.print(F("LoRa OK"));
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);
  }
  else
  {
    disp.print(F("No device responding"));
    Serial.println(F("No device responding"));
    while (1)
    {
      disp.print(F("Device error"));
      led_Flash(50, 50);                                       //long fast speed LED flash indicates device error
    }
  }

  lognumber = setup_SDLOG() ;                                  //setup SD card
  Serial.print(F("Lognumber "));
  Serial.println(lognumber);

  //this function call sets up the device for LoRa using the settings from settings.h
  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  LoRa.printModemSettings();                                   //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LoRa.printOperatingSettings();                               //reads and prints the configured operating settings, useful check
  Serial.println();
  Serial.println();

  dispscreen2();

  if (SD_Found)
  {
    printModemSettingsSD();
    logFile.println();
    printOperatingSettingsSD();
    logFile.println();
    logFile.println();
    logFile.flush();
  }

  delay(2000);
  clearLine(0);
  clearLine(1);
  clearLine(2);
  disp.setCursor(0, 0);
  disp.print(F("Receiver ready"));

  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();
}
