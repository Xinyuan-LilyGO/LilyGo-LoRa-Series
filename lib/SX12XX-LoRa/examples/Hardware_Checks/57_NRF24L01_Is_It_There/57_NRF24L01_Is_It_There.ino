/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 17/08/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is stand alone, it is not necessary to install a NRF2401 library.
  The program checks that the NRF2401 can be written to and read from over the SPI bus.

  The contents of the NRF2401 registers from 0x00 to 0x19 are read and printed to the serial monitor.
  If the connections are OK then the printout should look like this;

  57_NRF24L01_Is_It_There ?

  Print registers 0x00 to 0x1F
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x00  00 3F 02 03 5F 4C 27 42 00 00 E7 52 C3 C4 C5 C6
  0x10  E7 00 20 00 00 00 00 12 00 00

  If you get this output;

  Print registers 0x00 to 0x1F
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x00  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x10  00 00 00 00 00 00 00 00 00 00

  Then the CSN_PIN is not connected or defined correctly.

  Note that this is just a 'is it there' type check, the CE pin is not used or needed for this simple check.
  If the device is faulty, not present or wired incorrectly the register contents will likely be all 00s or
  FFs. The program makes no attempt to turn on the RF transmitter or receiver.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>

#define CSN_PIN 10

#define CMD_R_REGISTER      0x00
#define CMD_W_REGISTER      0x20


void loop()
{
  Serial.println(F("Print registers 0x00 to 0x19"));
  printRegisters(0, 0x19);
  Serial.println();
  delay(5000);
}


void printRegisters(uint16_t Start, uint16_t End)
{
  uint16_t Loopv1, Loopv2, endLoopv2, RegCount, RegData;

  RegCount = End - Start + 1;
  Serial.print(F("Printing "));
  Serial.print(RegCount);
  Serial.println(F(" registers"));
  Serial.println();

  Serial.print(F("Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Serial.println();

  for (Loopv1 = Start; Loopv1 <= End;)           //32 lines
  {
    Serial.print(F("0x"));
    if (Loopv1 < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print((Loopv1), HEX);                 //print the register number
    Serial.print(F("  "));

    if (RegCount < 16)
    {
      endLoopv2 = RegCount - 1;
    }
    else
    {
      endLoopv2 = 15;
    }

    for (Loopv2 = 0; Loopv2 <= endLoopv2; Loopv2++)
    {
      RegData = readRegister(Loopv1);
      RegCount--;

      if (RegData < 0x10)
      {
        Serial.print(F("0"));
      }
      Serial.print(RegData, HEX);                //print the register number
      Serial.print(F(" "));
      Loopv1++;
    }
    Serial.println();
  }
}


uint8_t readRegister(uint8_t reg)
{
  uint8_t result;

  digitalWrite(CSN_PIN, LOW);
  SPI.transfer(CMD_R_REGISTER | reg);
  result = SPI.transfer(0xff);
  digitalWrite(CSN_PIN, HIGH);
  return result;
}


void setup()
{
  Serial.begin(9600);
  pinMode(CSN_PIN, OUTPUT);
  Serial.println();
  Serial.println(F("57_NRF24L01_Is_It_There ?"));
  Serial.println();
  SPI.begin();
}
