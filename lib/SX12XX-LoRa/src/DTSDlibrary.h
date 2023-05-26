/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 08/11/21

  This code is supplied as is, it is up to the user of the program to decide if the program is suitable
  for the intended purpose and free from errors.
*******************************************************************************************************/

#ifdef SDFATLIB
#include <SdFat.h>
SdFat SD;
File dataFile;                                   //name the file instance needed for SD library routines
#endif

#ifdef SDLIB
#include <SD.h>
File dataFile;                                   //name the file instance needed for SD library routines
#endif

#ifndef Monitorport
#define Monitorport Serial                       //output to Serial if no other port defined 
#endif


bool DTSD_dumpFileASCII(char *buff);
bool DTSD_dumpFileHEX(char *buff);
bool DTSD_dumpSegmentHEX(uint8_t segmentsize);
bool DTSD_initSD(uint8_t CSpin);
uint32_t DTSD_getFileSize(char *buff);
void DTSD_printDirectory();
uint32_t DTSD_openFileRead(char *buff);
uint16_t DTSD_getNumberSegments(uint32_t filesize, uint8_t segmentsize);
uint8_t DTSD_getLastSegmentSize(uint32_t filesize, uint8_t segmentsize);
bool DTSD_openNewFileWrite(char *buff);
bool DTSD_openFileWrite(char *buff, uint32_t position);
uint8_t DTSD_readFileSegment(uint8_t *buff, uint8_t segmentsize);
uint8_t DTSD_writeSegmentFile(uint8_t *buff, uint8_t segmentsize);
void DTSD_seekFileLocation(uint32_t position);
uint16_t DTSD_createFile(char *buff);
uint16_t DTSD_fileCRCCCITT();
void DTSD_fileFlush();
void DTSD_closeFile();
void printDirectorySD(File dir, int numTabs);


bool DTSD_dumpFileASCII(char *buff)
{

  File dataFile = SD.open(buff);                  //open the test file note that only one file can be open at a time,

  if (dataFile)                                   //if the file is available, read from it
  {
    while (dataFile.available())
    {
      Monitorport.write(dataFile.read());
    }
    dataFile.close();
    return true;
  }
  else
  {
    return false;
  }
}


bool DTSD_dumpFileHEX(char *buff)
{
  //Note, this function will return true if the SD card is removed.
  uint16_t Loopv1, Loopv2;
  uint8_t fileData;
  uint32_t filesize;

  if (!SD.exists(buff))
  {
    return false;
  }

  dataFile = SD.open(buff);
  filesize = dataFile.size();
  filesize--;                                      //file data locations are from 0 to (filesize -1);
  Monitorport.print(F("Lcn    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Monitorport.println();

  if (dataFile)                                    //if the file is available, read from it
  {
    while (dataFile.available())
    {
      for (Loopv1 = 0; Loopv1 <= filesize;)
      {
        Monitorport.print(F("0x"));
        if (Loopv1 < 0x10)
        {
          Monitorport.print(F("0"));
        }
        Monitorport.print((Loopv1), HEX);
        Monitorport.print(F("  "));
        for (Loopv2 = 0; Loopv2 <= 15; Loopv2++)
        {
          fileData = dataFile.read();
          if (fileData < 0x10)
          {
            Monitorport.print(F("0"));
          }
          Monitorport.print(fileData, HEX);
          Monitorport.print(F(" "));
          Loopv1++;
        }
        Monitorport.println();
      }
    }
    dataFile.close();
    return true;
  }
  else
  {
    Monitorport.println(F("File not available"));
    return false;
  }
}


bool DTSD_initSD(uint8_t CSpin)
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


uint32_t DTSD_getFileSize(char *buff)
{
  uint32_t filesize;

  if (!SD.exists(buff))
  {
    return 0;
  }

  dataFile = SD.open(buff);
  filesize = dataFile.size();
  dataFile.close();
  return filesize;
}


#ifdef SDFATLIB
void DTSD_printDirectory()
{
  dataFile = SD.open("/");
  Monitorport.println(F("Card directory"));
  SD.ls("/", LS_R);
}
#endif


#ifdef SDLIB
void DTSD_printDirectory()
{
  dataFile = SD.open("/");

  printDirectorySD(dataFile, 0);

  Monitorport.println();
}


void printDirectorySD(File dir, int numTabs)
{

  while (true)
  {
    File entry =  dir.openNextFile();

    if (! entry)
    {
      //no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++)
    {
      Monitorport.print('\t');
    }

    Monitorport.print(entry.name());

    if (entry.isDirectory())
    {
      Monitorport.println("/");
      printDirectorySD(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Monitorport.print("\t\t");
      Monitorport.println(entry.size(), DEC);
    }
    entry.close();
  }
}
#endif


bool DTSD_dumpSegmentHEX(uint8_t segmentsize)
{
  uint16_t Loopv1, Loopv2;
  uint8_t fileData;

  Monitorport.print(F("Print segment of "));
  Monitorport.print(segmentsize);
  Monitorport.println(F(" bytes"));
  Monitorport.print(F("Lcn    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Monitorport.println();

  if (dataFile)                                    //if the file is available, read from it
  {
    for (Loopv1 = 0; Loopv1 < segmentsize;)
    {
      Monitorport.print(F("0x"));
      if (Loopv1 < 0x10)
      {
        Monitorport.print(F("0"));
      }
      Monitorport.print((Loopv1), HEX);
      Monitorport.print(F("  "));
      for (Loopv2 = 0; Loopv2 <= 15; Loopv2++)
      {
        //stop printing if all of segment has been printed
        if (Loopv1 < segmentsize)
        {
          fileData = dataFile.read();
          if (fileData < 0x10)
          {
            Monitorport.print(F("0"));
          }
          Monitorport.print(fileData, HEX);
          Monitorport.print(F(" "));
          Loopv1++;
        }
      }
      Monitorport.println();
    }
    return true;
  }
  else
  {
    return false;
  }
}


uint32_t DTSD_openFileRead2(char *buff)
{
  uint32_t filesize;

  dataFile = SD.open(buff);
  filesize = dataFile.size();
  dataFile.seek(0);
  return filesize;
}


uint32_t DTSD_openFileRead(char *buff)
{
  uint32_t filesize;

  if (SD.exists(buff))
  {
    //Monitorport.println(F("File exists"));
    dataFile = SD.open(buff);
    filesize = dataFile.size();
    dataFile.seek(0);
    return filesize;
  }
  else
  {
    //Monitorport.println(F("File does not exist"));
    return 0;
  }
}


uint16_t DTSD_getNumberSegments(uint32_t filesize, uint8_t segmentsize)
{
  uint16_t segments;
  segments = filesize / segmentsize;

  if ((filesize % segmentsize) > 0)
  {
    segments++;
  }
  return segments;
}


uint8_t DTSD_getLastSegmentSize(uint32_t filesize, uint8_t segmentsize)
{
  uint8_t lastsize;

  lastsize = filesize % segmentsize;
  if (lastsize == 0)
  {
    lastsize = segmentsize;
  }
  return lastsize;
}


bool DTSD_openNewFileWrite(char *buff)
{
  if (SD.exists(buff))
  {
    //Monitorport.print(buff);
    //Monitorport.println(F(" File exists - deleting"));
    SD.remove(buff);
  }

  if (dataFile = SD.open(buff, FILE_WRITE))
  {
    //Monitorport.print(buff);
    //Monitorport.println(F(" SD File opened"));
    return true;
  }
  else
  {
    //Monitorport.print(buff);
    //Monitorport.println(F(" ERROR opening file"));
    return false;
  }
}


bool DTSD_openFileWrite(char *buff, uint32_t position)
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


uint8_t DTSD_readFileSegment(uint8_t *buff, uint8_t segmentsize)
{
  uint8_t index = 0;
  uint8_t fileData;

  while (index < segmentsize)
  {
    fileData = (uint8_t) dataFile.read();
    buff[index] = fileData;
    index++;
  };

  if (index == segmentsize)
  {
    return segmentsize;            //if all written return segment size
  }
  else
  {
    return index - 1;               //if not all written return number bytes read
  }
}


uint8_t DTSD_writeSegmentFile(uint8_t *buff, uint8_t segmentsize)
{
  uint8_t index, byteswritten = 0;

  for (index = 0; index < segmentsize; index++)
  {
    dataFile.write(buff[index]);
    byteswritten++;
  }
  return byteswritten;
}


void DTSD_seekFileLocation(uint32_t position)
{
  dataFile.seek(position);                       //seek to position in file
  return;
}


uint16_t DTSD_createFile(char *buff)
{
  //creats a new filename use this definition as the base;
  //char filename[] = "/SD0000.txt";     //filename used as base for creating logfile, 0000 replaced with numbers
  //the 0000 in the filename is replaced with the next number avaialable

  uint16_t index;

  for (index = 1; index <= 9999; index++) {
    buff[3] = index / 1000 + '0';
    buff[4] = ((index % 1000) / 100) + '0';
    buff[5] = ((index % 100) / 10) + '0';
    buff[6] = index % 10 + '0' ;
    if (! SD.exists(buff))
    {
      // only open a new file if it doesn't exist
      dataFile = SD.open(buff, FILE_WRITE);
      break;
    }
  }

  if (!dataFile)
  {
    return 0;
  }

  return index;                                      //return number of logfile created
}


uint16_t DTSD_fileCRCCCITT(uint32_t fsize)
{
  uint32_t index;
  uint16_t CRCcalc;
  uint8_t j, filedata;

  CRCcalc = 0xFFFF;                                  //start value for CRC16

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


void DTSD_fileFlush()
{
  dataFile.flush();
}


void DTSD_closeFile()
{
  dataFile.close();                                   //close local file
}
