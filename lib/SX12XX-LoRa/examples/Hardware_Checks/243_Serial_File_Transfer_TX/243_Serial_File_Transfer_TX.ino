/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/12/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a basic demonstration of transmitting a file from an SD card over a serial
  interface to another Arduino.

  A block of data sent by the the transmitter is received into an array on the connected Arduino and stops
  when there is a timeout. The block\array is then searched for start and end IDs, the filename extracted
  and the file data payload (the file contents) extracted saved to an SD file.

  The transmitted block contains the data payload (file contents) CRC which is checked against the block
  data data and the file saved on SD.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SdFat.h>
SdFat SD;
File dataFile;                                     //name the file instance needed for SD library routines

const uint8_t FileDataBlock = 0x81;                //byte identifying file data block
const uint16_t IDStart = 0X2211;                   //byte sequence indicating start of block
const uint16_t IDEnd = 0X4433;                     //byte sequence indicating end of block

#define LED1 8
#define SDCS 40
#define TXSerial Serial2                           //assign serial port for transfer here 


void loop()
{
  //name of file to send
  char SourceFileName[] = "/$50SATL.JPG";          //file length 63091/0xF673 bytes, file CRC 0x597E

  transferFile(SourceFileName, sizeof(SourceFileName));

  Serial.print(("Program halted"));
  Serial.println();
  Serial.println();
  while (1);
}


bool transferFile(char *buff, uint8_t namelength)
{
  uint32_t index;
  uint32_t filesize;
  uint16_t sourcefileCRC;
  uint32_t filebytecount = 0;
  float sendSecs;
  uint32_t StartmS, SendmS;

  Serial.print(("Serial Transfer started "));
  Serial.println(buff);

  filesize = SDopenFileRead(buff);
  Serial.print(("Filesize 0x"));
  Serial.println(filesize, HEX);
  Serial.print(F("Filename length 0x"));
  Serial.println(namelength, HEX);
  sourcefileCRC = SDfileCRCCCITT(filesize);
  Serial.print(F("SourcefileCRC 0x"));
  Serial.println(sourcefileCRC, HEX);

  StartmS = millis();

  Serial.println(F("Writing header"));
  TXSerial.write(lowByte(IDStart));
  TXSerial.write(highByte(IDStart));
  TXSerial.write(FileDataBlock);                           //identify as file data block
  TXSerial.write(0x01);                                    //write the flags byte, not currently used
  TXSerial.write(namelength);
  TXSerial.print(buff);
  TXSerial.write((uint8_t) 0x00);                          //put null on end of filename
  TXSerial.write(filesize & 0x000000FF);
  TXSerial.write(filesize >> 8);
  TXSerial.write(filesize >> 16);
  TXSerial.write(filesize >> 24);;

  Serial.println(F("Writing file data"));

  dataFile.seek(0);                                        //start at file beginning
  for (index = 0; index < filesize; index++)
  {
    TXSerial.write(dataFile.read());
    filebytecount++;
  }

  TXSerial.write(lowByte(sourcefileCRC));                  //send the data payload (file) CRC
  TXSerial.write(highByte(sourcefileCRC));
  TXSerial.write(lowByte(IDEnd));                          //send the block end marker
  TXSerial.write(highByte(IDEnd));
  SendmS = millis() - StartmS;
  sendSecs = (float) SendmS / 1000;

  Serial.print(("Filebytes sent 0x"));
  Serial.println(filebytecount, HEX);

  Serial.print(F("Transmit time "));
  Serial.print(sendSecs, 3);
  Serial.println(F(" seconds"));

  return true;
}


uint16_t SDfileCRCCCITT(uint32_t fsize)
{
  uint32_t index;
  uint16_t CRCcalc;
  uint8_t j, filedata;

  filedata = dataFile.seek(0);                        //start at file beginning
  CRCcalc = 0xFFFF;                                   //start value for CRC16

  for (index = 0; index < fsize; index++)
  {
    filedata = dataFile.read();
    CRCcalc ^= (((uint16_t) filedata ) << 8);
    for (j = 0; j < 8; j++)
    {
      if (CRCcalc & 0x8000)
        CRCcalc = (CRCcalc << 1) ^ 0x1021;
      else
        CRCcalc <<= 1;
    }
  }
  return CRCcalc;
}


uint32_t SDopenFileRead(char *buff)
{
  uint32_t filesize;

  dataFile = SD.open(buff);
  filesize = dataFile.size();
  dataFile.seek(0);
  return filesize;
}


bool SDinitSD(uint8_t CSpin)
{
  if (SD.begin(CSpin))
  {
    return true;
  }
  else
  {
    return false;
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
  pinMode(LED1, OUTPUT);                          //setup pin as output for indicator LED
  led_Flash(2, 125);                              //two quick LED flashes to indicate program start

  TXSerial.begin(115200);
  Serial.begin(115200);
  Serial.println();
  Serial.println(F(__FILE__));
  Serial.flush();

  Serial.print(F("Initializing SD card..."));

  if (SDinitSD(SDCS))
  {
    Serial.println(F("SD Card initialized."));
  }
  else
  {
    Serial.println(F("SD Card failed, or not present."));
    while (1) led_Flash(100, 25);
  }

  Serial.println(F("Serial file transfer ready"));
  Serial.println();
}
