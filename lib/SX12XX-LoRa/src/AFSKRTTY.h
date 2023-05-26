/*
  Copyright 2020 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 12/05/20
*/

void startAFSKRTTY(int8_t audiopin, uint16_t freq, uint32_t afskleadinmS)
{
  tone(audiopin, freq);                       //lead in is high tone
  delay(afskleadinmS);
}


void endAFSKRTTY(int8_t audiopin)
{
  delay(500);                                 //500mS seconds of high tone to finish
  noTone(audiopin);
}


void sendAFSKRTTY(uint8_t chartosend, int8_t audiopin, int8_t checkpin, uint16_t tonelowHz, uint16_t tonehighHz, uint32_t perioduS)
//send the byte in chartosend as AFSK RTTY, assumes mark condition (idle) is already present
//Format is 7 bits, no parity and 2 stop bits
{
  uint8_t numbits;
  uint32_t startuS;

  startuS = micros();
  digitalWrite(checkpin, LOW);
  tone(audiopin, tonelowHz);

  while ( (uint32_t) (micros() - startuS) < perioduS);     //wait for start bit end

  for (numbits = 1;  numbits <= 7; numbits++)             //send 7 bits, LSB first
  {
    startuS = micros();
    if ((chartosend & 0x01) != 0)
    {
      digitalWrite(checkpin, HIGH);
      tone(audiopin, tonehighHz);
    }
    else
    {
      digitalWrite(checkpin, LOW);
      tone(audiopin, tonelowHz);                          //start 0 bit low tone
    }
    chartosend = (chartosend / 2);                        //get the next bit
    while ( (uint32_t) (micros() - startuS) < perioduS);  //wait bit period uS
  }
  perioduS = perioduS * 2;
  startuS = micros();
  digitalWrite(checkpin, HIGH);                           //start  mark condition
  tone(audiopin, tonehighHz);                             //start high tone
  while ( (uint32_t) (micros() - startuS) < perioduS);
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

