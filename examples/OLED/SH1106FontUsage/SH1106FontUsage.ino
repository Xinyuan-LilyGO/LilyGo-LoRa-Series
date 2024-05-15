/*

  FontUsage.ino

  How to overwrite previous text with a new text.
  How to avoid the buffer clear command.

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


  Usually, a sequence like this is used:
  u8g2->clearBuffer();                   // clear the internal memory
  u8g2->setFont(u8g2_font_ncenB08_tr);   // choose a suitable font
  u8g2->setCursor(0,20)              // set write position
  u8g2->print("Hello World!");           // write something to the internal memory
  u8g2->sendBuffer();                    // transfer internal memory to the display

  In order to speed up the display content rendering for any modified text,
  we could drop the clearBuffer command and just overwrite the previous string:

  u8g2->setFont(u8g2_font_ncenB08_tr);   // choose a suitable font
  u8g2->setCursor(0,20)              // set write position
  u8g2->print("hello there");            // write something to the internal memory
  u8g2->sendBuffer();                    // transfer internal memory to the display

  This will only work if:
  - Background is drawn
  - All gylphs have the same height
  Additionally some extra spaces are required of the new text is shorter than the
  previous one.

  To draw the background: use font mode 0
  To ensure that all glyphs have the same height, use h or m fonts.

  u8g2->setFontMode(0);              // write solid glyphs
  u8g2->setFont(u8g2_font_ncenB08_hr);   // choose a suitable h font
  u8g2->setCursor(0,20)              // set write position
  u8g2->print("hello there  ");          // use extra spaces here
  u8g2->sendBuffer();                    // transfer internal memory to the display


*/

#include <Arduino.h>
#include "LoRaBoards.h"


#define INFO_SCREEN_DELAY 3000

/*
  Linear Congruential Generator (LCG)
  z = (a*z + c) % m;
  m = 256 (8 Bit)

  for period:
  a-1: dividable by 2
  a-1: multiple of 4
  c: not dividable by 2

  c = 17
  a-1 = 64 --> a = 65
*/
uint8_t z = 127;    // start value
uint32_t lcg_rnd(void)
{
    z = (uint8_t)((uint16_t)65 * (uint16_t)z + (uint16_t)17);
    return (uint32_t)z;
}

void setup(void)
{
    setupBoards();
    if (!u8g2) {
        Serial.println("No find SH1106 display!Please check whether the connection is normal");
        while (1);
    }
}

void draw(int is_blank)
{
    int i, j;
    int n;
    char s[4];

    for ( j = 0; j < 20; j++ ) {
        // random number
        n = lcg_rnd();

        // random string
        for ( i = 0; i < 3; i++ ) {
            s[i] = lcg_rnd() >> 3;
            if ( s[i] < 16 )
                s[i] += 'a';
            else
                s[i] += 'A';
        }
        s[3] = '\0';

        // print number
        u8g2->setCursor(0, 15);
        u8g2->print("Number: ");
        if ( is_blank )
            u8g2->print("       ");
        u8g2->setCursor(70, 15);
        u8g2->print(n);


        // print string
        u8g2->setCursor(0, 30);
        u8g2->print("Text: ");
        u8g2->setCursor(70, 30);
        u8g2->print(s);
        if ( is_blank )
            u8g2->print("        ");

        // make the result visible
        u8g2->sendBuffer();

        // delay, so that the user can see the result
        delay(200);
    }
}

void draw_m1_t()
{
    u8g2->clearBuffer();

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);

    u8g2->setCursor(0, 15);
    u8g2->print(F("setFontMode(1);"));
    u8g2->setCursor(0, 30);
    u8g2->print(F("setFont(..._tr);"));
    u8g2->setCursor(0, 55);
    u8g2->print(F("Very Bad"));

    u8g2->sendBuffer();
    delay(INFO_SCREEN_DELAY);

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);
    u8g2->clearBuffer();                   // clear the internal memory once
    draw(0);
}

void draw_m0_t()
{
    u8g2->clearBuffer();

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);

    u8g2->setCursor(0, 15);
    u8g2->print(F("setFontMode(0);"));
    u8g2->setCursor(0, 30);
    u8g2->print(F("setFont(.._tr);"));
    u8g2->setCursor(0, 55);
    u8g2->print(F("Wrong"));

    u8g2->sendBuffer();
    delay(INFO_SCREEN_DELAY);

    u8g2->setFontMode(0);
    u8g2->setFont(u8g2_font_cu12_tr);
    u8g2->clearBuffer();                   // clear the internal memory once
    draw(0);
}

void draw_m1_h()
{
    u8g2->clearBuffer();

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);

    u8g2->setCursor(0, 15);
    u8g2->print(F("setFontMode(0);"));
    u8g2->setCursor(0, 30);
    u8g2->print(F("setFont(.._hr);"));
    u8g2->setCursor(0, 55);
    u8g2->print(F("Still bad"));

    u8g2->sendBuffer();
    delay(INFO_SCREEN_DELAY);

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_hr);
    u8g2->clearBuffer();                   // clear the internal memory once
    draw(0);
}

void draw_m0_h()
{
    u8g2->clearBuffer();

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);

    u8g2->setCursor(0, 15);
    u8g2->print(F("setFontMode(0);"));
    u8g2->setCursor(0, 30);
    u8g2->print(F("setFont(.._hr);"));
    u8g2->setCursor(0, 55);
    u8g2->print(F("Almost ok"));

    u8g2->sendBuffer();
    delay(INFO_SCREEN_DELAY);

    u8g2->setFontMode(0);
    u8g2->setFont(u8g2_font_cu12_hr);
    u8g2->clearBuffer();                   // clear the internal memory once
    draw(0);
}

void draw_m0_h_with_extra_blank()
{
    u8g2->clearBuffer();

    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);

    u8g2->setCursor(0, 15);
    u8g2->print(F("setFontMode(0);"));
    u8g2->setCursor(0, 30);
    u8g2->print(F("setFont(.._hr);"));
    u8g2->setCursor(0, 55);
    u8g2->print(F("Extra blank --> Ok"));

    u8g2->sendBuffer();
    delay(INFO_SCREEN_DELAY);

    u8g2->setFontMode(0);
    u8g2->setFont(u8g2_font_cu12_hr);
    u8g2->clearBuffer();                   // clear the internal memory once
    draw(1);
}


void loop(void)
{

    // This problem applies only to full buffer mode
    u8g2->clearBuffer();
    u8g2->setFontMode(1);
    u8g2->setFont(u8g2_font_cu12_tr);
    u8g2->setCursor(0, 15);
    u8g2->print(F("Problems with"));
    u8g2->setCursor(0, 30);
    u8g2->print(F("full buffer mode"));
    u8g2->setCursor(0, 45);
    u8g2->print(F("and skipped clear."));
    u8g2->sendBuffer();
    delay(INFO_SCREEN_DELAY);


    draw_m1_t();          // fontmode 1, t font --> wrong
    draw_m1_h();          // fontmode 1, h font --> wrong
    draw_m0_t();          // fontmode 0, t font --> wrong
    draw_m0_h();          // fontmode 1, h font --> ok
    draw_m0_h_with_extra_blank(); // fontmode 1, h font with extra blank --> correct
    delay(1000);
}

