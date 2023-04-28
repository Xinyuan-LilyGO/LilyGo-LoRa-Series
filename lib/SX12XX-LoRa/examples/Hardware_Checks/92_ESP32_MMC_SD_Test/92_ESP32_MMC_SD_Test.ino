/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 07/04/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This test program has been written to check that a connected SD card adapter, Micro
  or standard, is functional. The code is for an ESP32 running the SD card in MMC mode. Tested on a 
  NodeMCU ESP32. 

  The program creates a file called LOGXXXX.TXT, where XXXX is a number that increases every time the
  program is restarted. The program opens the file and writes a line like this to the file;

  #1 Hello World!

  The file is closed and the file contents are written to the IDE serial monitor and a directory of the
  SD card printed too. The process repeats with #2 Hello World! being appended to the file next. The
  directory listing allows you to keep track of the increasing size of the logFile. Problems with reading
  or writing to the SD card should result in an ERROR message on the IDE serial monitor.

  Connect the SD card to the following pins:

  SD Card | ESP32
  CMD       15   (MOSI for SPI mode)
  VSS       GND
  VDD       3.3V
  CLK       14   (SCK for SPI mode)
  VSS       GND
  D0        2    (MISO for SPI mode) (add 1K pull up after flashing)

  Note that GPIO2 may need a 1K pullup resistor added after flashing or the card will fail to initialise.

  If you get unreliable results, try adding more input_pullup lines for pins 4,12,13,15.

  See here for more information;

  https://www.reddit.com/r/esp32/comments/d71es9/a_breakdown_of_my_experience_trying_to_talk_to_an/

  Serial monitor baud rate is set at 115200
*******************************************************************************************************/


#include <SD_MMC.h>

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

  logFile = SD_MMC.open(filename, FILE_APPEND);
  logFile.print("#");
  logFile.print(linenumber);
  logFile.println(" Hello World!");
  logFile.close();
  dumpFile(filename);
  Serial.println();

  logFile = SD_MMC.open("/");
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


bool dumpFile(char *buf)
{
  Serial.print("Print file ");

  if (SD_MMC.exists(buf))
  {
    Serial.println(buf);
  }
  else
  {
    Serial.print("ERROR ");
    Serial.print(buf);
    Serial.println(" not found");
    return false;
  }

  logFile = SD_MMC.open(buf);
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
  return false;
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

    if (! SD_MMC.exists(filename))
    {
      // only open a new file if it doesn't exist
      dir = SD_MMC.open(buff, FILE_WRITE);
      break;
    }
  }

  dir.rewindDirectory();                          //stops SD_MMC.exists() command causing issues with directory listings
  dir.close();

  if (!dir)
  {
    return 0;
  }
  return index;                                   //return number of root created
}



void setup()
{
  pinMode(2, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print(__FILE__);
  Serial.println();

  if (!SD_MMC.begin("/sdcard", true))
  {
    Serial.println();
    Serial.println("ERROR Card Mount Failed - program halted");
    Serial.println();
    while (1);
  }

  Serial.println("Card Mount OK");

  logFile = SD_MMC.open("/");
  setupSDLOG(filename);
}
