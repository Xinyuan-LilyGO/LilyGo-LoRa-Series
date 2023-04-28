/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 29/12/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This test program has been written to check that the hardware for sending AFSK RTTY on
  been connected correctly. AFSKRTTY can be used to upload packets received from a high altitude balloon
  tracker as LoRa into DL-FLDIGI running on a PC and from there uploaded to an Internet connected tracking
  system.

  The AFSK RTTY library has been tested and will work on an Arduion Pro Mini 8Mhz, Arduino DUE and ESP32
  when used with this startup command;

  startAFSKRTTY(AUDIOOUTpin, CHECKpin, 5, 1000, 7, 714, 0, 1000);

  This outputs AFSKRTTY at 200baud, 7 databits, 1 startbit, 2 stopbits, no parity, low tone 1000hz, hightone 1400hz.

  The audio comes out of the pin passed via AUDIOOUTpin and the bit timing can be checked by looking at
  the CHECKpin on a scope or analyser.

  A low pass filter consiting of a deries 47K resistor and parallel 470nF capacitor was used to reduce the output
  for feeding into a PC soundcard.

  A screenshot of the FLDIGI settings used is in the folder containing this program; 'AFSKRTTY2_DL-Fldigi_Settings.jpg'

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#include <AFSKRTTY2.h>             //AFSK RTTY library for microcontrollers without the tone() function

//Choose whichever test pattern takes your fancy
//uint8_t testBuffer[] = "0123456789* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *";
//uint8_t testBuffer[] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
uint8_t testBuffer[] = "$$$$MyFlight1,2213,14:54:37,51.48230,-3.18136,15,6,3680,23,66,3,0*2935";



const uint16_t leadinmS = 1000;      //number of ms for AFSK constant lead in tone
const uint16_t leadoutmS = 0;        //number of ms for AFSK constant lead out tone


const uint16_t LOWPERIODUS = 1000;   //actual period in uS of to give a 200baud
const uint8_t LOWCYCLES = 5;         //cycles of low frequency tone for 200baud
const uint16_t HIGHPERIODUS = 714;   //actual high period in uS to give a 200baud
const uint8_t HIGHCYCLES = 7;        //cycles of high frequency tone for 200baud
const int8_t ADJUSTUS = 0;           //uS to subtract from tone generation loop to match frequency

const int8_t AUDIOOUT = 6;           //pin used to output Audio tones
const int8_t CHECK = 8;              //this pin is toggled inside the AFSKRTTY library, high for logic 1, low for logic 0, so it can be used to check the timing.


void loop()
{
  uint8_t index;
  uint8_t chartosend;
  uint8_t len = sizeof(testBuffer) - 1;       //must NOT send null at end of buffer

  Serial.print(F("Sending AFSK RTTY "));
  Serial.flush();

  startAFSKRTTY(AUDIOOUT, CHECK, LOWCYCLES, LOWPERIODUS, HIGHCYCLES, HIGHPERIODUS, ADJUSTUS, leadinmS);

  sendAFSKRTTY(13);
  sendAFSKRTTY(10);

  for (index = 0; index < len; index++)
  {
    chartosend = testBuffer[index];
    sendAFSKRTTY(chartosend);
    Serial.write(chartosend);
  }

  sendAFSKRTTY(13);
  sendAFSKRTTY(10);

  endAFSKRTTY(AUDIOOUT, CHECK, leadoutmS);

  digitalWrite(CHECK, LOW);
  Serial.println();
  delay(1000);
}


void setup()
{
  pinMode(CHECK, OUTPUT);                                   //setup pin as output for indicator LED

  Serial.begin(9600);
  Serial.println();
  Serial.println(F("70_AFSKRTTY_Upload_Test"));
  Serial.println();
}


