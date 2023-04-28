/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 14/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a program that transfers a file from an Arduinos SD card to a folder on a
  PC that is connected to the Arduino via a Serial port. The transfer process uses the YModem protocol
  and the PC receives the file using the Tera Term Serial terminal program.

  This program was run on an Arduino DUE, with Serial2 used as the transfer port to the PC.

  Instructions for using the program are to be found here;

  https://stuartsprojects.github.io/2021/12/28/Arduino-to-PC-File-Transfers.html

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>
#include <SD.h>
#include "YModem.h"

File root;

const uint16_t SDCS = 30;
const uint16_t LED1 = 8;                    //LED indicator etc. on during transfers
char FileName[] = "$50SATL.JPG";            //file length 63091 bytes, file CRC 0x59CE
uint16_t transferNumber = 0;;
uint32_t filelength;
uint32_t bytestransfered;


void loop()
{
  transferNumber++;
  digitalWrite(LED1, HIGH);
  Serial.print(F("Start transfer "));
  Serial.print(transferNumber);
  Serial.print(F("  "));
  Serial.println(FileName);
  Serial.flush();
  digitalWrite(LED1, HIGH);
  
  bytestransfered = yModemSend(FileName, 1, 1);     //transfer filename with waitForReceiver and batchMode options set
  
  if (bytestransfered > 0)
  {
  Serial.print(F("YModem transfer completed "));
  Serial.print(bytestransfered);
  Serial.println(F(" bytes sent"));
  }
  else
  {
  Serial.println(F("YModem transfer FAILED"));
  }
  Serial.println();
  
  digitalWrite(LED1, LOW);
  Serial.println();
  delay(10000);
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

  Serial.begin(115200);
  Serial2.begin(115200);                          //Serial port used for file transfers
  Serial.println();
  Serial.println(F(__FILE__));
  Serial.flush();
  Serial.println("Using Serial2 for YModem comms @ 115200 baud.");
  Serial.println("Initializing SD card...");

  if (SD.begin(SDCS))
  {
    Serial.println(F("SD Card initialized."));
  }
  else
  {
    Serial.println(F("SD Card failed, or not present."));
    while (1) led_Flash(100, 25);
  }

}
