/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/01/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This test program has been written to check that a connected SD card adapter, Micro
  or standard, is funtional with the FS functions. To use the program first copy the file (in this programs
  directory) called testfile.txt to the root directory of the SD card.

  When the program runs it will attempt to open 'testfile.txt' and spool the contents to the Arduino IDE
  serial monitor.   The testfile is part of the source code for the Apollo 11 Lunar Lander navigation and
  guidance computer. There are LED flashes at power up or reset, then at start of every loop of the test.
  The LED is on whilst the testfile is being read. If the LED flashes very rapidly then there is a problem
  accessing the SD card.

  The program also has the option of using a logic pin to control the power to the lora and SD card
  devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are
  powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#define Program_Version "V1.0"
#include "ESP32_LoRa_Micro_Node.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"


void loop()
{
  Serial.println(F("LED Flash"));
  Serial.println();
  led_Flash(2, 50);
  readFile(SD, "/testfile.txt");
  delay(1000);
}


void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println();
    Serial.println("Failed to open file for reading");
    Serial.println();
    return;
  }

  Serial.print("Read from file: ");
  digitalWrite(LED1, HIGH);
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  digitalWrite(LED1, LOW);
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
  pinMode(LED1, OUTPUT);                                 //for PCB LED
  led_Flash(4, 125);

  if (VCCPOWER >= 0)
  {
    pinMode(VCCPOWER, OUTPUT);                  //For controlling power to external devices
    digitalWrite(VCCPOWER, LOW);                //VCCOUT on. lora device on
  }

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("44_SD_Card_Test_With_FS_ESP32 Starting"));
  Serial.print("Initializing SD card...");

  if (!SD.begin(SDCS)) {
    Serial.println("Card failed, or not present.");
    led_Flash(100, 25);
    return;                                              //loop if no card found
  }

  Serial.println("Card initialized.");
}


