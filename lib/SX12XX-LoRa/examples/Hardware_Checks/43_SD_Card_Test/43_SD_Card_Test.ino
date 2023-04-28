/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This test program has been written to check that a connected SD card adapter, Micro
  or standard, is functional. The code was tested on an Atmel ATMega328P and ATMega1284P and running the
  SD card in SPI mode. For this test it is assumed there are no other SPI devices on the SPI bus. 

  The program creates a file called LOGXXXX.TXT, where XXXX is a number that increases every time the
  program is restarted. The program opens the file and writes a line like this to the file;

  #1 Hello World!

  The file is closed and the file contents are written to the IDE serial monitor and a directory of the
  SD card printed too. The process repeats with #2 Hello World! being appended to the file next. The
  directory listing allows you to keep track of the increasing size of the logFile. Problems with reading
  or writing to the SD card should result in an ERROR message on the IDE serial monitor.

  Serial monitor baud rate is set at 115200
*******************************************************************************************************/

#include <SPI.h>
#include <SD.h>

#define SDCS 4                                 //SD card select for SPI 

char filename[] = "/LOG0000.TXT";               //filename used as base for creating root, 0000 replaced with numbers
uint16_t linenumber = 0;

File logFile;


void loop()
{
  linenumber++;

  Serial.print("Write to file > ");
  Serial.print("#");
  Serial.print(linenumber);
  Serial.println(" Hello World!");

  logFile = SD.open(filename, FILE_WRITE);
  logFile.print("#");
  logFile.print(linenumber);
  logFile.println(" Hello World!");
  logFile.close();
  dumpFile(filename);
  Serial.println();

  logFile = SD.open("/");
  logFile.rewindDirectory();
  printDirectory(logFile, 0);

  Serial.println();
  delay(1500);
}


void printDirectory(File dir, int numTabs)
{
  Serial.println("Card directory");

  while (true)
  {
    File entry =  dir.openNextFile();
    if (! entry)
    {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


bool dumpFile(char *buff)
{
  Serial.print("Print file ");

  if (SD.exists(buff))
  {
    Serial.println(buff);
  }
  else
  {
    Serial.print("ERROR ");
    Serial.print(buff);
    Serial.println(" not found - program halted");
    while (1);
    //return false;
  }

  logFile = SD.open(buff);
  logFile.seek(0);

  if (logFile)                                    //if the file is available, read from it
  {
    while (logFile.available())
    {
      Serial.write(logFile.read());
    }
    logFile.close();
    return true;
  }
  else
  {
  Serial.print("ERROR ");
  Serial.println(" dumping file ");
  Serial.print(buff);
  Serial.println(" - program halted");
  while (1);
  //return false;
  }
}


uint8_t setupSDLOG(char *buff)
{
  //creats a new filename

  uint16_t index;

  File dir;

  for (index = 1; index <= 9999; index++)
  {
    buff[4] = index / 1000 + '0';
    buff[5] = ((index % 1000) / 100) + '0';
    buff[6] = ((index % 100) / 10) + '0';
    buff[7] = index % 10 + '0' ;

    if (! SD.exists(filename))
    {
      // only open a new file if it doesn't exist
      dir = SD.open(buff, FILE_WRITE);
      break;
    }
  }

  dir.rewindDirectory();                          //stops SD.exists() command causing issues with directory listings
  dir.close();

  if (!dir)
  {
    return 0;
  }
  return index;                                   //return number of root created
}



void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print(__FILE__);
  Serial.println();

  SPI.begin();

  if (!SD.begin(SDCS))
  {
    Serial.println();
    Serial.println("ERROR Card Mount Failed - program halted");
    Serial.println();
    while (1);
  }

  Serial.println("Card Mount OK");

  logFile = SD.open("/");
  setupSDLOG(filename);
}
