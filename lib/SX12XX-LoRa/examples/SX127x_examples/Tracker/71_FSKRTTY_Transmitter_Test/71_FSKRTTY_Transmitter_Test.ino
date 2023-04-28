/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 23/12/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a test program for using the LoRa device to transmit upper side band FSK
  RTTY. With the LoRa device in FSK direct mode, the frequency of the generated carrier is shifted up
  or down at the correct baud rate depending on whether a logic 0 or 1 is being sent. 

  The desired shift in frequency is defined in the Settings.h file as 'FrequencyShift'. When the program 
  starts the actual frequency shift will be calculated according to the discrete frequency steps the 
  LoRa device can be set to. This example uses the library function for sending FSKRTTY that is fixed at
  7 databits, 1 stop bit and no parity bit. If you want to vary these settings see the example;
  '78_FSKRTTY_Transmitter_Test_Configurable.ino'
  
  Before the actual data transmission starts you can send a series of marker pips which are short bursts 
  of up shifted carrier which will be heard as beeps in a correctly tuned receiver. These pips can aid
  in setting the receiver decode frequemcy to match the transmission. on some LoRa devices, such as the SX127x
  series there can be considerable temperature induced frequency drift. This drift can be caused by outside
  temperature changes or the RF device self heating when transmit is turned on. The duration of the pips,
  the gaps between them and the period of leadin carrier before the data starts can all be set. To send no
  pips just set the number to 0.

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#define Program_Version "V1.1"

#include <SPI.h>                                               //the lora device is SPI based                                         
#include <SX127XLT.h>                                          //include the appropriate SX12XX library  
#include "Settings.h"                                          //include the setiings file, frequencies, LoRa settings etc   

SX127XLT LT;                                                   //create a library class instance called LT

//Choose whichever test pattern takes your fancy
//uint8_t testBuffer[] = "0123456789* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *";               //This string is sent as AFSK RTTY, 7 bit, 2 Stop bit, no parity, 300 baud.

//uint8_t testBuffer[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789";               
//uint8_t testBuffer[] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
uint8_t testBuffer[] = "$$$$MyFlight1,2213,14:54:37,51.48230,-3.18136,15,6,3680,23,66,3,0*2935";

uint8_t freqShiftRegs[3];                                      //to hold returned registers that set frequency


void loop()
{
  uint8_t index;
  
  printRegisterSetup(FrequencyShift);
  Serial.println();

  LT.setupDirect(Frequency, Offset);
  LT.startFSKRTTY(FrequencyShift, NumberofPips, PipPeriodmS, PipDelaymS, LeadinmS);

  Serial.print(F("Start RTTY micros() = "));
  Serial.println(micros());
  Serial.print(F("Seconds to overflow "));
  Serial.println(((0xFFFFFFFF - micros()) / 1E6),0);
  
  LT.transmitFSKRTTY(13, BaudPerioduS, LED1);      //send carriage return
  LT.transmitFSKRTTY(10, BaudPerioduS, LED1);      //send line feed
  
  for (index = 0; index < (sizeof(testBuffer)-1); index++)
  {
    LT.transmitFSKRTTY(testBuffer[index], BaudPerioduS, LED1);
    Serial.write(testBuffer[index]);
  }
  LT.transmitFSKRTTY(13, BaudPerioduS, LED1);      //send carriage return
  LT.transmitFSKRTTY(10, BaudPerioduS, LED1);      //send line feed
    
  Serial.println();
  Serial.print(F("END RTTY micros() = "));
  Serial.println(micros());
  digitalWrite(LED1, LOW);
  Serial.println();
  Serial.println();
  
  LT.setMode(MODE_STDBY_RC);

  delay(2000);
}


void printRegisterSetup(uint32_t shift)
{
  
  uint32_t nonShiftedFreq, ShiftedFreq;
  uint32_t freqShift;
  float exactfreqShift; 

  LT.setRfFrequency(Frequency, Offset);                       //ensure base frequecy is set
  LT.getRfFrequencyRegisters(freqShiftRegs);                  //fill buffer with frequency setting registers values
  nonShiftedFreq = ( (uint32_t) freqShiftRegs[0]  << 16 ) +  ( (uint32_t) freqShiftRegs[1] << 8 ) + freqShiftRegs[2];
  Serial.print(F("NoShift Registers 0x"));
  Serial.println(nonShiftedFreq, HEX);

  LT.setRfFrequency((Frequency + shift), Offset);             //set shifted frequecy
  LT.getRfFrequencyRegisters(freqShiftRegs);                  //fill buffer with frequency setting registers values
  ShiftedFreq = ( (uint32_t) freqShiftRegs[0]  << 16 ) +  ( (uint32_t) freqShiftRegs[1] << 8 ) + freqShiftRegs[2];
  Serial.print(F("Shifted Registers 0x"));
  Serial.println(ShiftedFreq, HEX);

  freqShift = ShiftedFreq - nonShiftedFreq;
  exactfreqShift = freqShift * FREQ_STEP;
  Serial.print(F("FSKRTTY register shift "));
  Serial.println(freqShift,HEX);
  Serial.print(F("FSKRTTY frequency shift "));
  Serial.print(exactfreqShift, 8);
  Serial.println(F("hZ"));

  LT.setRfFrequency(Frequency, Offset);                       //ensure base frequecy is set
}


void printRegisterBuffer()
{
Serial.print(freqShiftRegs[0],HEX);
Serial.print(F(" "));
Serial.print(freqShiftRegs[1],HEX);
Serial.print(F(" "));
Serial.print(freqShiftRegs[2],HEX);
Serial.println();  
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
  pinMode(LED1, OUTPUT);                                   //setup pin as output for indicator LED
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("71_FSKRTTY_Transmitter_Test"));

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found
  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  LT.setupDirect(Frequency, Offset);
  Serial.print(F("Transmitter ready"));
  Serial.println();
}

