/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 04/06/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This test program has been written to check that a connected SD card adapter, Micro
  or standard, is functional. 

  When the program runs it will attempts to create a file that is next in sequence to Log0000.txt, thus
  if this is the first time the program has run on the SD card it will create file Log0001.txt. If the
  file Log0001.txt exists it will create Log0002.txt etc. This ensures that everytime the program starts
  it creates a new file for testing.

  Next the program checks if the Logxxxx.txt file exists and if so attempts to delete it.

  Then the program starts a loop. First the same file is again opened for append, it will be created if it
  does not exist and then the line '1 Hello World' is writtent to the file. The file is closed and the
  contents of the file are dumped to the serial monitor. The loop restarts, and this time the line
  '2 Hello World' is appended to the file.

  As the program progresses the file will grow in size and after 4 iterrations of the open,write,close
  and dump loop the file dump on serial monitor will look like this

  1 Hello World
  2 Hello World
  3 Hello World
  4 Hello World

  This file dump will grow if you let the program run. If an error with the SD card is detected at any
  time the LED will rapid flash continuously and the message 'X Card failed, or not present' is printed
  to serial monitor. The number X will allow you to check the program listing for where the error occured.
  
  1 Card failed = SD card did not initialise
  2 Card failed = Could nt setup logFile for new name 
  3 Card failed = Could not open file for append
  4 Card failed = Failure to dump file to serial monitor

  Note: At the time of writing, 04/06/20, the function that the dumpFile() routine uses to check if an SD
  card is still present, SD.exists(filename), does not work on the SD.h file included in the current Expressif
  core for Arduino. If the SD card is removed, the SD.exists(filename) function returns true so the following
  dump of the file locks up the ESP32.  

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/
#define Program_Version "V1.1"


#include <SD.h>
#include <SPI.h>

//pin definitions. You also need to connect the card SCK to pin 18, MISO to pin 19 and MOSI to pin 23
#define LED1 2                          //pin number for LED
#define SDCS  13                        //pin number for device select on SD card module

File logFile;

char filename[] = "/LOG0000.TXT";       //filename used as base for creating logfile, 0000 replaced with numbers

uint16_t linecount;


void loop()
{
  Serial.println();
  Serial.println("Initializing SD card");

  if (!SD.begin(SDCS))
  {
    cardFail(1);
  }

  Serial.println("Card initialized");

  logFile = SD.open("/");
  Serial.println("Card directory");
  Serial.println();
  printDirectory(logFile, 0);
  Serial.println();
  Serial.println();

  if (!setupSDLOG(filename))                            //setup logfile name for writing
  {
  cardFail(2);  
  }
  
  Serial.print(F("logFile name is "));
  Serial.println(filename);

  if (SD.exists(filename))
  {
    Serial.print(filename);
    Serial.println(" exists - delete");
    SD.remove(filename);
  }

  if (!SD.exists(filename))
  {
    Serial.print(filename);
    Serial.println(" does not exist");
  }

  while (1)
  {
    logFile = SD.open(filename, FILE_APPEND);

    if (!logFile)
    {
      cardFail(3);
    }

    linecount++;
    logFile.print(linecount);
    logFile.println(" Hello World");
    logFile.close();

    Serial.println();
    Serial.print("Dump file ");
    Serial.println(filename);
    Serial.println();

    digitalWrite(LED1, HIGH);

    if (dumpFile(filename))
    {
      Serial.println();
      Serial.println("Finished File Dump");
    }
    else
    {
      cardFail(4);
    }

    digitalWrite(LED1, LOW);
    delay(2000);
  }
}


void printDirectory(File dir, int numTabs)
{
  while (true)
  {

    File entry =  dir.openNextFile();
    if (! entry)
    {
      //no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
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
      //files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


bool dumpFile(char *buf)
{
  //Note, this function will return true if the SD card is remove. See note at program start.
  if (SD.exists(buf))
  {
  Serial.print(buf);
  Serial.println(" found");  
  }
  else
  {
  Serial.print(filename);
  Serial.println(" not found");
  return false;  
  }
    
  logFile = SD.open(buf);

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


uint8_t setupSDLOG(char *buf)
{
  //creats a new filename

  uint16_t index;

  for (index = 1; index <= 9999; index++) {
    buf[4] = index / 1000 + '0';
    buf[5] = ((index % 1000) / 100) + '0';
    buf[6] = ((index % 100) / 10) + '0';
    buf[7] = index % 10 + '0' ;
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logFile = SD.open(buf, FILE_WRITE);
      break;
    }
  }

  if (!logFile)
  {
    return 0;
  }

  return index;                                   //return number of logfile created
}


void cardFail(uint8_t num)
{
  while (1)
  {
    Serial.print(num);                                //so we can tell where card failed
    Serial.println(" Card failed, or not present");
    led_Flash(100, 25);
  }
}


void led_Flash(unsigned int flashes, unsigned int delaymS)
{
  unsigned int index;

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
  pinMode(LED1, OUTPUT);                                //for PCB LED
  led_Flash(4, 125);

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("43_SD_Card_Test Starting"));
}


