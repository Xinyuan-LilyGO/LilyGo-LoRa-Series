/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/12/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/
/*******************************************************************************************************
  Program Operation - This is a basic demonstration of receiving a file over a serial interface. A block
  of data sent by the the transmitter is received into an array and stops when there is a timeout. The
  block is then searched and checked for start and end IDs, the filename extracted and the file data
  extracted and saved to an SD file. A CRC check is then done on the file saved to SD.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SdFat.h>
SdFat SD;
File dataFile;                                //name the file instance needed for SD library routines

char STFileName[16];

const uint32_t SerialTimeoutuS = 1000000;     //Timeout in uS before assuming message to send is complete
const uint16_t IDStart = 0X2211;              //byte sequence indicating start of block
const uint16_t IDEnd = 0X4433;                //byte sequence indicating end of block
const uint8_t FileDataBlock = 0x81;           //byte identifying block as file data

const uint32_t RXMAXBlockLength = 65536;      //max size of received data block
uint8_t DataBlock[RXMAXBlockLength];          //block array for received data

#define RXSerial Serial2                      //assign serial port for transfer here 

#define LED1 8                                //LED used to indicate transmission
#define SDCS 30                               //pin for SD card select 


void loop()
{
  receiveFile(DataBlock, RXMAXBlockLength);
  Serial.println(F("File received OK"));
  Serial.println();
  Serial.println();
  delay(5000);
}


bool receiveFile(uint8_t *buff, uint32_t maxlength)
{
  uint16_t startid;                           //for reading startID of block
  uint16_t endid;                             //for reading endID of block
  bool toobig = false;                        //for checking if there are more serial bytes received than can be stored
  uint32_t receivedBytes;                     //count of serial bytes received
  uint32_t fileLength;                        //length of file included in block
  uint32_t destinationFileBytes;              //length of destination file
  uint8_t fileNameLength;                     //length of filename in filename block
  uint16_t savedFileCRC;                      //CRC of file saved on SD
  uint32_t fileStartAddress;                  //start address in block where file data starts
  bool transferOK = false;
  uint32_t startuS;                           //used for timeout for serial input

  do
  {
    memset(buff, 0, maxlength);                 //clear entire array to 0s

    Serial.println(F("Clear serial buffer"));

    while (RXSerial.available() > 0)           //clear serial input
    {
      RXSerial.read();
    }

    receivedBytes = 0;
    Serial.println(F("Waiting start of serial input"));

    while (RXSerial.available() == 0);
    digitalWrite(LED1, HIGH);
    Serial.println(F("Started "));

    startuS = micros();

    while (((uint32_t) (micros() - startuS) < SerialTimeoutuS))
    {
      if (RXSerial.available() > 0)
      {
        if (receivedBytes >= maxlength)
        {
          toobig = true;
          break;
        }
        buff[receivedBytes] = RXSerial.read();
        startuS = micros();                                //restart timeout
        receivedBytes++;
      }
    };

    if (toobig)
    {
      Serial.println(F("File too big !"));
      return false;
    }

    digitalWrite(LED1, LOW);
    Serial.println(F("Serial timeout"));
    Serial.print(F("Serial bytes read 0x"));
    Serial.println(receivedBytes, HEX);
    Serial.print(F("Block start bytes > "));
    printarrayHEX(buff, 0, 0x20);                           //print 0x20 bytes at block start
    Serial.println();
    Serial.print(F("Block end bytes   > "));
    printarrayHEX(buff, receivedBytes - 0x20, 0x20);        //print 0x20 bytes at block end
    Serial.println();

    startid = buff[0] + (buff[1] * 256);

    if ( (startid == IDStart) && (buff[2] == FileDataBlock))
    {
      Serial.println(F("File data block start detected"));
    }
    else
    {
      Serial.println(F("ERROR - File data block start not detected"));
      continue;
    }

    endid = buff[receivedBytes - 2] + (buff[receivedBytes - 1] * 256);

    if (endid == IDEnd)
    {
      Serial.println(F("File data block end detected"));
    }
    else
    {
      Serial.println(F("ERROR - File data block end not detected"));
      continue;
    }

    fileNameLength = buff[4];
    Serial.print(F("FilenameLength 0x"));
    Serial.println(fileNameLength, HEX);

    memcpy(&fileLength, buff + fileNameLength + 5, 4);
    Serial.print(F("FileLength 0x"));
    Serial.println(fileLength, HEX);

    fileStartAddress = fileNameLength + 9;
    Serial.print(F("FileStartAddress 0x"));
    Serial.println(fileStartAddress, HEX);

    memcpy(STFileName, buff + 5, fileNameLength);

    Serial.print(F("Open "));
    Serial.print(STFileName);
    Serial.println(F(" for writing"));

    SDopenFileWrite(STFileName, 0);
    destinationFileBytes = SDwriteArrayFile(buff, fileStartAddress, fileLength);   //write array to SD file (file already open)
    SDcloseFile();

    Serial.print(F("File bytes written 0x"));
    Serial.println(destinationFileBytes, HEX);
    savedFileCRC = SDfileCRCCCITT(SDopenFileRead(STFileName));
    Serial.print(F("CRC of file written to SD 0x"));
    Serial.println(savedFileCRC, HEX);
    SDcloseFile();
    transferOK = true;
  }
  while (transferOK == false);

  return true;
}


uint32_t SDwriteArrayFile(uint8_t *buff, uint32_t startaddress, uint32_t arraysize)
{
  uint32_t index, byteswritten = 0;
  uint32_t endaddr;

  endaddr = startaddress + arraysize;

  dataFile.seek(0);

  for (index = startaddress; index < endaddr; index++)
  {
    dataFile.write(buff[index]);
    byteswritten++;
  }
  return byteswritten;
}


bool SDopenFileWrite(char *buff, uint32_t position)
{
  dataFile = SD.open(buff, FILE_WRITE);   //seems to operate as append
  dataFile.seek(position);                //seek to first position in file

  if (dataFile)
  {
    return true;
  }
  else
  {
    return false;
  }
}


uint32_t SDopenFileRead(char *buff)
{
  uint32_t filesize;

  dataFile = SD.open(buff);
  filesize = dataFile.size();
  dataFile.seek(0);
  return filesize;
}


void SDcloseFile()
{
  dataFile.close();                                   //close local file
}


uint16_t SDfileCRCCCITT(uint32_t fsize)
{
  uint32_t index;
  uint16_t CRCcalc;
  uint8_t j, filedata;

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


uint16_t arrayCRC(uint8_t *buffer, uint32_t startadd, uint32_t size, uint16_t startvalue)
{
  uint32_t index, endlocation;
  uint16_t arrayCRC;
  uint8_t j;

  arrayCRC = startvalue;                                       //start value for CRC16
  endlocation = startadd + size;

  for (index = startadd; index < endlocation; index++)
  {
    arrayCRC ^= (((uint16_t)buffer[index]) << 8);
    for (j = 0; j < 8; j++)
    {
      if (arrayCRC & 0x8000)
        arrayCRC = (arrayCRC << 1) ^ 0x1021;
      else
        arrayCRC <<= 1;
    }
  }
  return arrayCRC;
}


void printarrayHEX(uint8_t *buff, uint32_t startadd, uint32_t len)
{
  uint32_t index, endaddr;
  uint8_t buffdata;

  endaddr = startadd + len;

  for (index = startadd; index < endaddr; index++)
  {
    buffdata = buff[index];
    if (buffdata < 16)
    {
      Serial.print(F("0"));
    }
    Serial.print(buffdata, HEX);
    Serial.print(F(" "));
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

  RXSerial.begin(115200);
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
