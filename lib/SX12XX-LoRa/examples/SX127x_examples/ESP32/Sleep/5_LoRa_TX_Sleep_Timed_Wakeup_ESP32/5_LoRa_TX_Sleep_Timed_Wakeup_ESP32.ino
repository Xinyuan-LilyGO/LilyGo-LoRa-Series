/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 03/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program tests the sleep mode and register retention of the SX127X in sleep mode,
  it assumes an ESP32 processor module. The LoRa settings to use are specified in the 'Settings.h' file.
  To retain the program memory, so that the program restarts after sleep at the next instructions after
  going into sleep mode the ESP32 light sleep function is used.

  A packet is sent, containing the text 'Before Device Sleep' and the LoRa device and ESP32 are put to sleep.
  The ESP32 internal RTC timer should wakeup the processor in 15 seconds (approx) and lora register
  values should be retained.  The device then attempts to transmit another packet 'After Device Sleep'
  without re-loading all the lora settings. The receiver should see 'After Device Sleep' for the first
  packet and 'After Device Sleep' for the second.

  For unknown reasons the ESP32 pulses the pin used by NRESET when going into sleep modes, so the program
  turns it into an input for the duration of the sleep to avoid resetting the lora device.

  The program also has the option of using a logic pin to control the power to the lora and SD card
  devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are
  powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.

  Tested on a 'bare bones' ESP32 the current in light sleep mode was 1670uA.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"

SX127XLT LT;

boolean SendOK;
int8_t TestPower;
uint8_t TXPacketL;

/***********************************************************
  //ESP32 specific settings
***********************************************************/
#define uS_TO_S_FACTOR 1000000              //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  15                   //Time ESP32 will go to sleep (in seconds) 
RTC_DATA_ATTR int16_t bootCount = 0;
RTC_DATA_ATTR uint16_t sleepcount = 0;


void loop()
{
  Serial.print(F("Bootcount "));
  Serial.println(bootCount);
  Serial.print(F("Sleepcount "));
  Serial.println(sleepcount);

  Serial.println();
  LT.printRegisters(0x00, 0x7F);
  Serial.println();

  digitalWrite(LED1, HIGH);
  Serial.print(TXpower);
  Serial.print(F("dBm "));
  Serial.print(F("TestPacket1> "));
  Serial.flush();

  if (Send_Test_Packet1())
  {
    packet_is_OK();
  }
  else
  {
    packet_is_Error();
  }
  Serial.println();
  delay(packet_delay);

  pinMode(NRESET, INPUT);                                   //change NRESET pin to INPUT mode


  LT.setSleep(CONFIGURATION_RETENTION);                     //preserve register settings in sleep.

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println(F("Start Sleep"));
  Serial.flush();
  sleepcount++;

  esp_light_sleep_start();

  Serial.println();
  Serial.println();
  Serial.println(F("Awake !"));
  Serial.flush();
  digitalWrite(LED1, HIGH);

  LT.wake();

  digitalWrite(NRESET, HIGH);                               //set NRESET high before turning it back to an output
  pinMode(NRESET, INPUT);                                   //set NRESET as an output

  Serial.print(TXpower);
  Serial.print(F("dBm "));
  Serial.print(F("TestPacket2> "));
  Serial.flush();

  if (Send_Test_Packet2())
  {
    packet_is_OK();
  }
  else
  {
    packet_is_Error();
  }
  Serial.println();
  delay(packet_delay);
}


void packet_is_OK()
{
  Serial.print(F(" "));
  Serial.print(TXPacketL);
  Serial.print(F(" Bytes SentOK"));
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                           //get the IRQ status
  Serial.print(F("SendError,"));
  Serial.print(F("Length,"));
  Serial.print(TXPacketL);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
  digitalWrite(LED1, LOW);                                  //this leaves the LED on slightly longer for a packet error
}


bool Send_Test_Packet1()
{
  uint8_t bufffersize;

  uint8_t buff[] = "Before Device Sleep";
  TXPacketL = sizeof(buff);
  buff[TXPacketL - 1] = '*';

  if (sizeof(buff) > TXBUFFER_SIZE)                         //check that defined buffer is not larger than TX_BUFFER
  {
    bufffersize = TXBUFFER_SIZE;
  }
  else
  {
    bufffersize = sizeof(buff);
  }

  TXPacketL = bufffersize;

  LT.printASCIIPacket( (uint8_t*) buff, bufffersize);
  digitalWrite(LED1, HIGH);

  if (LT.transmit( (uint8_t*) buff, TXPacketL, 10000, TXpower, WAIT_TX))
  {
    digitalWrite(LED1, LOW);
    return true;
  }
  else
  {
    return false;
  }
}


bool Send_Test_Packet2()
{
  uint8_t bufffersize;

  uint8_t buff[] = "After Device Sleep";
  TXPacketL = sizeof(buff);
  buff[TXPacketL - 1] = '*';

  if (sizeof(buff) > TXBUFFER_SIZE)              //check that defined buffer is not larger than TX_BUFFER
  {
    bufffersize = TXBUFFER_SIZE;
  }
  else
  {
    bufffersize = sizeof(buff);
  }

  TXPacketL = bufffersize;

  LT.printASCIIPacket( (uint8_t*) buff, bufffersize);
  digitalWrite(LED1, HIGH);

  if (LT.transmit( (uint8_t*) buff, TXPacketL, 10000, TXpower, WAIT_TX))
  {
    digitalWrite(LED1, LOW);
    return true;
  }
  else
  {
    return false;
  }
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
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);                            //two quick LED flashes to indicate program start

  if (VCCPOWER >= 0)
  {
    pinMode(VCCPOWER, OUTPUT);                  //For controlling power to external devices
    digitalWrite(VCCPOWER, LOW);                //VCCOUT on. lora device on
  }

  Serial.begin(9600);
  Serial.println();
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("5_LoRa_TX_Sleep_Timed_Wakeup_ESP32 Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                            //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.print(F("Transmitter ready - TXBUFFER_SIZE "));
  Serial.println(TXBUFFER_SIZE);
  Serial.println();
}

