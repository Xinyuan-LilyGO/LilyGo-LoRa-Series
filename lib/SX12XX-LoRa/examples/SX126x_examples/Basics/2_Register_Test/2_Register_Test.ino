/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 06/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors. 
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is stand alone, it is not necessary to install the SX12XX-LoRa library
  to use it. This test program is for the SX126X LoRa devices.

  The program checks that a SX126X LoRa device can be accessed by doing a test register write and read.
  If there is no device found a message is printed on the serial monitor. The contents of the registers
  from 0x00 to 0x7F are printed, there is a copy of a typical printout below. Note that the read back
  changed frequency may be slightly different to the programmed frequency, there is a rounding error due
  to the use of floats to calculate the frequency.

  The Arduino pin numbers that the NSS and NRESET pins on the LoRa device are connected to must be
  specified in the hardware definitions section below. The LoRa device type in use, SX1261, SX1262,
  or SX1268 must be specified also.

  Typical printout;

  2_Register_Test Starting
  Reset device
  LoRa Device found
  Reset device
  Registers at reset
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x800  00 00 00 00 01 07 20 1E 00 10 19 04 0F FF 0F FF 
  0x810  10 00 10 00 10 00 10 00 00 00 00 00 00 00 00 00 
  0x820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x830  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x840  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x850  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x860  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x870  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x880  03 00 00 5F 10 08 00 00 08 05 00 39 30 00 00 0C 
  0x890  00 00 00 00 00 0F 0A 07 10 00 26 01 01 53 06 07 
  0x8A0  10 00 AA 20 5A 04 F0 00 56 56 54 43 94 20 40 00 
  0x8B0  00 83 11 00 01 04 0A 4C 14 0A 2F 01 6B FF FF 00 
  0x8C0  00 A0 20 00 00 00 AC 00 1C 00 00 AB 05 30 00 00 
  0x8D0  0C 14 14 40 06 00 00 10 C8 00 00 00 00 00 31 39 
  0x8E0  90 39 0C 04 40 20 1C 18 03 00 05 04 03 02 01 01 
  0x8F0  00 00 00 00 30 00 00 00 00 00 00 00 00 00 00 00 
  0x900  30 00 00 00 00 00 00 00 00 00 00 00 24 04 47 04 
  0x910  14 12 12 04 00 03 0A 00 15 35 09 00 02 1F 5F 08 
  0x920  01 04 05 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x930  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x940  00 07 00 03 02 00 10 0E 0D 0C 03 04 03 70 0C 00 
  0x950  00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 
  0x960  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x970  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x980  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x990  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x9A0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x9B0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x9C0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x9D0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x9E0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
  0x9F0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 


  Frequency at reset 915000000
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x880  03 00 00 5F 10 08 00 00 08 05 00 39 30 00 00 0C 
  Change Frequency 434100000
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x880  03 00 00 5F 10 08 00 00 08 05 00 1B 21 99 A0 0C 
  Changed Frequency 434100000

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

const uint16_t REG_RFFrequency31_24 = 0x088B;
const uint16_t REG_RFFrequency23_16 = 0x088C;
const uint16_t REG_RFFrequency15_8 = 0x088D;
const uint16_t REG_RFFrequency7_0 = 0x088E;
const uint8_t RADIO_WRITE_REGISTER = 0x0D;
const uint8_t RADIO_READ_REGISTER = 0x1D;
const uint8_t RADIO_SET_RFFREQUENCY = 0x86;

const uint8_t DEVICE_SX1261 = 0x01;
const uint8_t DEVICE_SX1262 = 0x00;
const uint8_t DEVICE_SX1268 = 0x02;

//*********  Setup hardware definitions here ! *****************

//These are the pin definitions for one of the Tracker boards, be sure to change them to match your
//own setup. You will also need to connect up the pins for the SPI bus, which on an Arduino Pro Mini are
//SCK pin 13, MISO pin 12, and MOSI pin 11.

#define NSS 10                                  //SX126X device select
#define NRESET 9                                //SX126X reset pin
#define RFBUSY 7                                //SX126X busy pin 
#define LED1 8                                  //for on board LED, put high for on
#define SW -1                                   //SW pin on Dorji devices is used to turn RF switch on\off, set to -1 if not used     
#define LORA_DEVICE DEVICE_SX1262               //define the device, DEVICE_SX1261, DEVICE_SX1262 or DEVICE_SX1268   

//**************************************************************/


#include <SPI.h>


void setup()
{
  Serial.begin(9600);
  Serial.println(F("2_Register_Test Starting"));

  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //The begin function setups the hardware pins used by device and then checks if device is found
  //the DIO1, DIO2 and DIO3 are not used in this example so are set to -1

  if (begin(NSS, NRESET, RFBUSY, -1, -1, -1, SW, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
  }
  else
  {
    Serial.println(F("No device responding"));
  }
}


void loop()
{
  uint32_t frequency;
  resetDevice(LORA_DEVICE);                  //reset the device
  Serial.println(F("Registers at reset"));   //show the all registers following a reset
  printRegisters(0x800, 0x9FF);
  Serial.println();
  Serial.println();

  frequency = getFreqInt();                  //read the set frequency following a reset
  Serial.print(F("Frequency at reset "));
  Serial.println(frequency);
  printRegisters(0x0880, 0x088F);            //show the registers before the frequency change
  setRfFrequency(434100000, 0);              //change the frequency at reset, in hertz
  frequency = getFreqInt();                  //read back the changed frequency
  Serial.print(F("Change Frequency "));
  Serial.println(frequency);                 //print the changed frequency, did the write work (allow for rounding errors) ?
  printRegisters(0x0880, 0x088F);            //show the registers after frequency change
  frequency = getFreqInt();                  //read the set frequency following a reset
  Serial.print(F("Changed Frequency "));
  Serial.println(frequency);

  Serial.println();
  Serial.println();
  delay(5000);
}


void readRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{

  uint16_t index;
  uint8_t addr_l, addr_h;

  addr_h = address >> 8;
  addr_l = address & 0x00FF;
  checkBusy();

  digitalWrite(NSS, LOW);
  SPI.transfer(RADIO_READ_REGISTER);
  SPI.transfer(addr_h);               //MSB
  SPI.transfer(addr_l);               //LSB
  SPI.transfer(0xFF);
  for (index = 0; index < size; index++)
  {
    *(buffer + index) = SPI.transfer(0xFF);
  }

  digitalWrite(NSS, HIGH);

  checkBusy();
}


uint8_t readRegister(uint16_t address)
{
  uint8_t data;

  readRegisters(address, &data, 1);
  return data;
}


void writeRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
  uint8_t addr_l, addr_h;
  uint8_t i;

  addr_l = address & 0xff;
  addr_h = address >> 8;
  checkBusy();

  digitalWrite(NSS, LOW);
  SPI.transfer(RADIO_WRITE_REGISTER);
  SPI.transfer(addr_h);   //MSB
  SPI.transfer(addr_l);   //LSB

  for (i = 0; i < size; i++)
  {
    SPI.transfer(buffer[i]);
  }

  digitalWrite(NSS, HIGH);

  checkBusy();
}


void writeRegister(uint16_t address, uint8_t value)
{
  writeRegisters( address, &value, 1 );
}


uint32_t getFreqInt()
{
  //get the current set device frequency from registers, return as long integer
  uint8_t MsbH, MsbL, Mid, Lsb;
  uint32_t uinttemp;
  float floattemp;
  MsbH = readRegister(REG_RFFrequency31_24);
  MsbL = readRegister(REG_RFFrequency23_16);
  Mid = readRegister(REG_RFFrequency15_8);
  Lsb = readRegister(REG_RFFrequency7_0);
  floattemp = ( (MsbH * 0x1000000ul) + (MsbL * 0x10000ul) + (Mid * 0x100ul) + Lsb);
  floattemp = ((floattemp * 0.95367431640625) / 1000000ul);
  uinttemp = (uint32_t)(floattemp * 1000000);
  return uinttemp;
}


void printRegisters(uint16_t Start, uint16_t End)
{
  //prints the contents of SX126x registers to serial monitor

  uint16_t Loopv1, Loopv2, RegData;

  Serial.print(F("Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F"));
  Serial.println();

  for (Loopv1 = Start; Loopv1 <= End;)           //32 lines
  {
    Serial.print(F("0x"));
    Serial.print((Loopv1), HEX);                 //print the register number
    Serial.print(F("  "));
    for (Loopv2 = 0; Loopv2 <= 15; Loopv2++)
    {
      RegData = readRegister(Loopv1);
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


void setRfFrequency( uint32_t frequency, int32_t offset )
{
  //Note RF_Freq = freq_reg*32M/(2^25)-----> freq_reg = (RF_Freq * (2^25))/32

  uint8_t Rf_Freq[4];

  frequency = frequency + offset;

  frequency = ( uint32_t )( ( double )frequency / ( double )0.95367431640625 );

  checkBusy();

  Rf_Freq[0] = (frequency >> 24) & 0xFF; //MSB
  Rf_Freq[1] = (frequency >> 16) & 0xFF;
  Rf_Freq[2] = (frequency >> 8) & 0xFF;
  Rf_Freq[3] = frequency & 0xFF;//LSB

  writeCommand(RADIO_SET_RFFREQUENCY, Rf_Freq, 4);
}


void checkBusy()
{
  uint8_t busy_timeout_cnt;
  busy_timeout_cnt = 0;

  while (digitalRead(RFBUSY))
  {
    delay(1);
    busy_timeout_cnt++;

    if (busy_timeout_cnt > 10) //wait 10mS for busy to complete
    {
      busy_timeout_cnt = 0;
      Serial.println(F("ERROR - Busy Timeout!"));
      break;
    }
  }
}


void resetDevice(uint8_t device)
{
  if ( (device == DEVICE_SX1261) | (device ==  DEVICE_SX1262) | (device == DEVICE_SX1268) )
  {
    Serial.println(F("Reset device"));
    delay(10);
    digitalWrite(NRESET, LOW);
    delay(2);
    digitalWrite(NRESET, HIGH);
    delay(25);
    checkBusy();
  }
}



bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3, int8_t pinSW, uint8_t device)
{
  pinMode(pinNSS, OUTPUT);
  digitalWrite(pinNSS, HIGH);
  pinMode(pinNRESET, OUTPUT);
  digitalWrite(pinNRESET, LOW);
  pinMode(pinRFBUSY, INPUT);

  if (pinDIO1 >= 0)
  {
    pinMode( pinDIO1, INPUT);
  }

  if (pinDIO2 >= 0)
  {
    pinMode(pinDIO2, INPUT);
  }

  if (pinDIO3 >= 0)
  {
    pinMode(pinDIO3, INPUT);
  }

  if (pinSW >= 0)
  {
    pinMode(pinSW, OUTPUT);                     //Dorji devices have an SW pin that needs to be set high to power antenna switch
    digitalWrite(pinSW, HIGH);
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
  Regdata1 = readRegister(0x88e);               //low byte of frequency setting
  writeRegister(0x88e, (Regdata1 + 1));
  Regdata2 = readRegister(0x88e);               //read changed value back
  writeRegister(0x88e, Regdata1);               //restore register to original value

  if (Regdata2 == (Regdata1 + 1))
  {
    return true;
  }
  else
  {
    return false;
  }
}


void writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size)
{
  uint8_t index;
  checkBusy();

  digitalWrite(NSS, LOW);
  SPI.transfer(Opcode);

  for (index = 0; index < size; index++)
  {
    SPI.transfer(buffer[index]);
  }
  digitalWrite(NSS, HIGH);

  checkBusy();
}


