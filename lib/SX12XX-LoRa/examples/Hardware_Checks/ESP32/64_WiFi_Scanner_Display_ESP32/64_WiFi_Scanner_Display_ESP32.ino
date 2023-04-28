/*******************************************************************************************************
  lora Programs for Arduino - Copyright of the author Stuart Robinson - 20/01/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - When the ESP32 turns on the WiFi function, there is a short high current pulse that
  can cause the ESP32 brownout detect to operate.

  This test program at startup flashes an LED, leaves it on and then starts the WiFi. If the Wifi initiates
  a brownout, you will see the LED flash again. The LED stays on when scanning, the program reports the
  networks found to the serial console and displays them on an attached SSD1306 OLED.

  Thus if you see the LED continually doing short bursts of flashing the turn on\off the WiFi is causing
  the ESP32 to reset. There will also be a message on the serial monitor that the brownout detector operated.

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#include "WiFi.h"
#define LED1 2                                          //Arduino pin number for LED, when high LED should be on.

#include <U8x8lib.h>                                    //get library here >  https://github.com/olikraus/u8g2 
//U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);  //use this line for standard 0.96" SSD1306
U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE); //use this line for 1.3" OLED often sold as 1.3" SSD1306


void loop()
{
  Serial.println("Set WiFi to Station mode");           //Set WiFi to station mode
  Serial.flush();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Setup done");
  Serial.flush();

  digitalWrite(LED1, HIGH);
  Serial.println("WiFi scan start");
  Serial.flush();
  int n = WiFi.scanNetworks();                          //WiFi.scanNetworks will return the number of networks found
  digitalWrite(LED1, LOW);
  delay(500);
  disp.clear();
  disp.setCursor(0, 0);

  if (n == 0) {
    Serial.println("No WiFi");
    disp.println("No WiFi");
  } else {
    Serial.print(n);
    disp.print(n);
    Serial.println(" WiFi found");
    disp.println(" WiFi found");
    led_Flash(n, 500);

    if (n > 16)                                          //only want to display first 16 networks
    {
      n = 16;
    }

    for (int i = 0; i < n; ++i) {
      //Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));

      if (i > 7)
      { disp.clearLine(i - 8);
        disp.setCursor(0, i - 8);
      }
      else
      {
        disp.clearLine(i);
        disp.setCursor(0, i);
      }

      disp.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      if (i == 7)                       //check if 8 lines have already been sent to display
      {
        delay(2000);                    //leave last 8 on display for a while
        disp.clear();
      }
    }
  }
  Serial.println();
  disp.println();


  // Wait a bit before scanning again
  delay(5000);
}


void led_Flash(unsigned int flashes, unsigned int delaymS)
{
  //flash LED
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
  pinMode(LED1, OUTPUT);
  led_Flash(5, 50);
  digitalWrite(LED1, LOW);
  delay(1000);
  Serial.begin(9600);

  disp.begin();
  disp.setFont(u8x8_font_chroma48medium8_r);
  disp.clear();
  disp.setCursor(0, 0);
  disp.print(F("Scanner Ready"));
}
