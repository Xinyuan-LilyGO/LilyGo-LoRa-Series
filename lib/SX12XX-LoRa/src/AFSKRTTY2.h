/*
  Copyright 2020 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 18/12/20
*/


/*
  This is the AFSK RTTY send library specifically for microcontrollers that do not have a tone() function
  such as Arduino DUE and ESP32. Tones are generated here by manually toggling the selected tone pin with
  a suitable delay between toggles.

  The baud rate and tone frequecies used can be adjusted by usin the startAFSKRTTY() command.

  The library has been tested and will work on an Arduion Pro Mini 8Mhz, Arduino DUE and ESP32 and when
  used with this startup command;

  startAFSKRTTY(AUDIOOUTpin, CHECKpin, 5, 1000, 7, 714, 0, 1000);

  This outputs AFSKRTTY at 100baud, 7bit, no parity, 1stop bit, low tone 1000hz, hightone 1400hz.

  The audio comes out of the pin passed via AUDIOOUTpin and the bit timing can be checked by looking at
  the CHECKpin on a scope or analyser.

  A low pass filter consiting of a 47K resistor and 470nF capacitor was used to redice the output for
  feeding into a PC soundcard.

*/



#define AFSKRTTY_UNUSED(v) (void) (v)       //add AFSKRTTY_UNUSED(variable); in functions to avoid compiler warnings 

int8_t _audiopin, _checkpin, _adjust;
uint8_t _lowcycles, _highcycles;
uint32_t _lowperioduS, _highperioduS;

void startAFSKRTTY(int8_t audiopin, int8_t checkpin, uint8_t lowcycles, uint16_t lowperioduS, uint8_t highcycles, uint16_t highperioduS, int16_t adjust, uint16_t leadinmS);
void sendAFSKRTTY(uint8_t chartosend);
void toneHigh();
void toneLow();


void startAFSKRTTY(int8_t audiopin, int8_t checkpin, uint8_t lowcycles, uint16_t lowperioduS, uint8_t highcycles, uint16_t highperioduS, int16_t adjust, uint16_t leadinmS)
{
  uint32_t startmS;
  _audiopin = audiopin;
  _checkpin = checkpin;
  _lowperioduS = (lowperioduS / 2) + adjust;           //period passed is equal to frequency, but delays are in two halfs
  _highperioduS = (highperioduS / 2) + adjust;
  _lowcycles = lowcycles;
  _highcycles = highcycles;
  _adjust = adjust;

  if (audiopin > 0)
  {
    pinMode(audiopin, OUTPUT);
  }

  if (checkpin > 0)
  {
    pinMode(checkpin, OUTPUT);
  }

  startmS = millis();

  while ( (uint32_t) (millis() - startmS) < leadinmS)       //allows for millis() overflow
  {
    digitalWrite(audiopin, HIGH);
    delayMicroseconds(_highperioduS);
    digitalWrite(audiopin, LOW);
    delayMicroseconds(_highperioduS);
  }
}


void endAFSKRTTY(int8_t audiopin, int8_t checkpin, uint16_t leadoutmS)
{
  uint32_t startmS;

  startmS = millis();

  while ( (uint32_t) (millis() - startmS) < leadoutmS)       //allows for millis() overflow
  {
    digitalWrite(audiopin, HIGH);
    delayMicroseconds(_highperioduS);
    digitalWrite(audiopin, LOW);
    delayMicroseconds(_highperioduS);
  }

  if (audiopin > 0)
  {
    pinMode(audiopin, INPUT);
  }

  if (checkpin > 0)
  {
    pinMode(checkpin, INPUT);
  }
}


void toneHigh()
{
  uint8_t index;

  for (index = 1; index <= _highcycles; index++)
  {
    digitalWrite(_audiopin, HIGH);
    delayMicroseconds(_highperioduS);
    digitalWrite(_audiopin, LOW);
    delayMicroseconds(_highperioduS);
  }

}


void toneLow()
{
  uint8_t index;

  for (index = 1; index <= _lowcycles; index++)
  {
    digitalWrite(_audiopin, HIGH);
    delayMicroseconds(_lowperioduS);
    digitalWrite(_audiopin, LOW);
    delayMicroseconds(_lowperioduS);
  }

}



void sendAFSKRTTY(uint8_t chartosend)
//send the byte in chartosend as AFSK RTTY, assumes mark condition (idle) is already present
//Format is 7 bits, no parity and 2 stop bits
{

  uint8_t numbits;
  digitalWrite(_checkpin, LOW);
  toneLow();

  for (numbits = 1;  numbits <= 7; numbits++) //send 7 bits, LSB first
  {
    if ((chartosend & 0x01) != 0)
    {
      digitalWrite(_checkpin, HIGH);
      toneHigh();
    }
    else
    {
      digitalWrite(_checkpin, LOW);
      toneLow();
    }
    chartosend = (chartosend / 2);            //get the next bit
  }
  digitalWrite(_checkpin, HIGH);               //start  mark condition
  toneHigh();
  toneHigh();
}


/*
  MIT license

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions
  of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

