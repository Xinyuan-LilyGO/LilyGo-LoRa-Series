/*******************************************************************************************************

  Programs for Arduino - Copyright of the author Stuart Robinson - 30/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is stand alone, it is not necessary to install the SX12XX-LoRa library
  to use it.

  The program checks that a SX127X LoRa device can be accessed by doing a test register write and read.
  If there is no device found a message is printed on the serial monitor. The contents of the registers
  from 0x00 to 0x7F are printed, there is a copy of a typical printout below. Note that the read back
  changed frequency may be slightly different to the programmed frequency, there is a rounding error due
  to the use of floats to calculate the frequency.

  The Arduino pin numbers that the NSS and NRESET pins on the LoRa device are connected to must be
  specified in the hardware definitions section below. The LoRa device type in use, SX1272, SX1276,
  SX1277, SX1278 or SX1279 must be specified also.

  Typical printout;

  2_Register_Test Starting
  SX1276-79 Selected
  LoRa Device found
  Device version 0x12

  Frequency at reset 434000000
  Registers at reset
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x00  00 09 1A 0B 00 52 6C 80 00 4F 09 2B 20 08 02 0A
  0x10  FF 6F 15 0B 28 0C 12 47 32 3E 00 00 00 00 00 40
  0x20  00 00 00 00 05 00 03 93 55 55 55 55 55 55 55 55
  0x30  90 40 40 00 00 0F 00 00 00 F5 20 82 00 02 80 40
  0x40  00 00 12 24 2D 00 03 00 04 23 00 09 05 84 32 2B
  0x50  14 00 00 12 00 00 00 0F E0 00 0C 00 08 00 5C 78
  0x60  00 19 0C 4B CC 0F 01 20 04 47 AF 3F CF 00 53 0B
  0x70  D0 01 10 00 00 00 00 00 00 00 00 00 00 00 00 00

  Changed Frequency 434099968
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x00  00 09 1A 0B 00 52 6C 86 66 4F 09 2B 20 08 02 0A
  0x10  FF 6F 15 0B 28 0C 12 47 32 3E 00 00 00 00 00 40
  0x20  00 00 00 00 05 00 03 93 55 55 55 55 55 55 55 55
  0x30  90 40 40 00 00 0F 00 00 00 F5 20 82 00 02 80 40
  0x40  00 00 12 24 2D 00 03 00 04 23 00 09 05 84 32 2B
  0x50  14 00 00 12 00 00 00 0F E0 00 0C 00 08 00 5C 78
  0x60  00 19 0C 4B CC 0F 01 20 04 47 AF 3F CF 00 53 0B
  0x70  D0 01 10 00 00 00 00 00 00 00 00 00 00 00 00 00

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

const uint8_t REG_FRMSB = 0x06;                 //register number for setting setting and reading frequency, high byte
const uint8_t REG_FRMID = 0x07;                 //register number for setting setting and reading frequency, mid byte
const uint8_t REG_FRLSB = 0x08;                 //register number for setting setting and reading frequency, low byte
const uint8_t REG_VERSION = 0x42;               //register containg version number of device

const uint8_t DEVICE_SX1272 = 0x10;             //SX1272
const uint8_t DEVICE_SX1276 = 0x11;             //SX1276
const uint8_t DEVICE_SX1277 = 0x12;             //SX1277
const uint8_t DEVICE_SX1278 = 0x13;             //SX1278
const uint8_t DEVICE_SX1279 = 0x14;             //SX1279

//*********  Setup hardware definitions here ! *****************

//These are the pin definitions for one of the Tracker boards, be sure to change them to match your
//own setup. You will also need to connect up the pins for the SPI bus, which on an Arduino Pro Mini are
//SCK pin 13, MISO pin 12, and MOSI pin 11.

#define NSS 10                                  //SX127X device select
#define NRESET 9                                //SX127X reset pin
#define DIO0 -1                                 //DIO0 pin on LoRa device, not used here so set to -1 
#define DIO1 -1                                 //DIO1 pin on LoRa device, normally not used so set to -1 
#define DIO2 -1                                 //DIO2 pin on LoRa device, normally not used so set to -1

#define LORA_DEVICE DEVICE_SX1278               //defines the type of LoRa device used, needed for correct program operation

//**************************************************************/


#include <SPI.h>


void setup()
{
  Serial.begin(9600);
  Serial.println(F("2_Register_Test_STM32 Starting"));

  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //The begin function setups the hardware pins used by device and then checks if device is found
  //the DIO0, DIO1 and DIO2 pins are not used in this example so are set to -1
  //the LT.begin fuction can define the pins and device type directly in this way (for SX1278);
  //LT.begin(10, 9, -1, -1, -1, DEVICE_SX1278)

  if (begin(NSS, NRESET, -1, -1, -1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
  }
  else
  {
    Serial.println(F("No device responding"));
  }

  Serial.print(F("Device version 0x"));
  uint8_t deviceversion = readRegister(REG_VERSION);
  if (deviceversion < 0x10)
  {
    Serial.print(F("0"));
  }
  Serial.println(deviceversion, HEX);
}


void loop()
{
  uint32_t frequency;

  frequency = getFreqInt();                  //read the set frequency following a reset
  Serial.print(F("Frequency at reset "));
  Serial.println(frequency);

  Serial.println(F("Registers at reset"));   //show the all registers following a reset
  printRegisters(0x00, 0x7F);

  Serial.println();
  Serial.println();

  setRfFrequency(434100000, 0);              //change the frequency at reset, in hertz
  frequency = getFreqInt();                  //read back the changed frequency
  Serial.print(F("Changed Frequency "));
  Serial.println(frequency);                 //print the changed frequency, did the write work (allow for rounding errors) ?
  printRegisters(0x00, 0x7F);                //show the registers after frequency change
  Serial.println();
  delay(5000);
  resetDevice(LORA_DEVICE);                  //reset the device and start again
}


uint8_t readRegister(uint8_t address)
{
  uint8_t regdata;
  digitalWrite(NSS, LOW);                    //set NSS low
  SPI.transfer(address & 0x7F);              //mask address for read
  regdata = SPI.transfer(0);                 //read the byte
  digitalWrite(NSS, HIGH);                   //set NSS high
  return regdata;
}


void writeRegister(uint8_t address, uint8_t value)
{
  digitalWrite(NSS, LOW);                    //set NSS low
  SPI.transfer(address | 0x80);              //mask address for write
  SPI.transfer(value);                       //write the byte
  digitalWrite(NSS, HIGH);                   //set NSS high
}


uint32_t getFreqInt()
{
  //get the current set LoRa device frequency, return as long integer

  uint8_t Msb, Mid, Lsb;
  uint32_t uinttemp;
  float floattemp;
  Msb = readRegister(REG_FRMSB);
  Mid = readRegister(REG_FRMID);
  Lsb = readRegister(REG_FRLSB);
  floattemp = ((Msb * 0x10000ul) + (Mid * 0x100ul) + Lsb);
  floattemp = ((floattemp * 61.03515625) / 1000000ul);
  uinttemp = (uint32_t)(floattemp * 1000000);
  return uinttemp;
}


void printRegisters(uint16_t Start, uint16_t End)
{
  //prints the contents of SX127x registers to serial monitor

  uint16_t Loopv1, Loopv2, RegData;

  Serial.print(F("Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Serial.println();

  for (Loopv1 = Start; Loopv1 <= End;)
  {
    Serial.print(F("0x"));
    if (Loopv1 < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print((Loopv1), HEX);
    Serial.print(F("  "));
    for (Loopv2 = 0; Loopv2 <= 15; Loopv2++)
    {
      RegData = readRegister(Loopv1);
      if (RegData < 0x10)
      {
        Serial.print(F("0"));
      }
      Serial.print(RegData, HEX);
      Serial.print(F(" "));
      Loopv1++;
    }
    Serial.println();
  }
}


void setRfFrequency(uint64_t freq64, int32_t offset)
{
  freq64 = freq64 + offset;
  freq64 = ((uint64_t)freq64 << 19) / 32000000;
  writeRegister(REG_FRMSB, (uint8_t)(freq64 >> 16));
  writeRegister(REG_FRMID, (uint8_t)(freq64 >> 8));
  writeRegister(REG_FRLSB, (uint8_t)(freq64 >> 0));
}


void resetDevice(uint8_t device)
{
  if (device == DEVICE_SX1272)
  {
    digitalWrite(NRESET, HIGH);
    delay(2);
    digitalWrite(NRESET, LOW);
    delay(20);
    Serial.println(F("SX1272 Selected"));
  }
  else
  {
    digitalWrite(NRESET, LOW);
    delay(2);
    digitalWrite(NRESET, HIGH);
    delay(20);
    Serial.println(F("SX1276-79 Selected"));
  }
}


bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinDIO0, int8_t pinDIO1, int8_t pinDIO2, uint8_t device)
{
  pinMode(pinNSS, OUTPUT);
  digitalWrite(pinNSS, HIGH);
  pinMode(pinNRESET, OUTPUT);
  digitalWrite(pinNRESET, LOW);

  if (pinDIO0 >= 0)
  {
    pinMode( pinDIO0, INPUT);
  }

  if (pinDIO1 >= 0)
  {
    pinMode( pinDIO1,  INPUT);
  }

  if (pinDIO2 >= 0)
  {
    pinMode( pinDIO2,  INPUT);
  }

  resetDevice(device);

  if (checkDevice())
  {
    return true;
  }

  return false;
}


bool checkDevice()
{
  //check there is a device out there, writes a register and reads back

  uint8_t Regdata1, Regdata2;
  Regdata1 = readRegister(REG_FRMID);               //low byte of frequency setting
  writeRegister(REG_FRMID, (Regdata1 + 1));
  Regdata2 = readRegister(REG_FRMID);               //read changed value back
  writeRegister(REG_FRMID, Regdata1);               //restore register to original value

  if (Regdata2 == (Regdata1 + 1))
  {
    return true;
  }
  else
  {
    return false;
  }
}



