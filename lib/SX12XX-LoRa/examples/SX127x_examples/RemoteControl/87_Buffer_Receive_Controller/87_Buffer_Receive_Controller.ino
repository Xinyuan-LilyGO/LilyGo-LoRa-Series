/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/09/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a LoRa receiver that listens for packets that are text based commands used
  when received to turn a LED on or off. When 'LEDOn' is received the LED is turned on and its turned off
  when 'LEDOff' is received.

  Sample Serial Monitor output;

  87_Buffer_Receive_Controller Starting

  LoRa Device found
  Receiver ready
  2s Received packet > LEDOn,RSSI,-65dBm,SNR,6dB,Length,5 Turn LED on
  3s Received packet > LEDOff,RSSI,-62dBm,SNR,5dB,Length,6 Turn LED off

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/


#include <SPI.h>                                //the lora device is SPI based so load the SPI library
#include <SX127XLT.h>                           //include the appropriate library   

SX127XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define DIO0 3                                  //DIO0 pin on LoRa device, used for RX and TX done 
#define LED1 8                                  //pin for LED

#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using
#define RXtimeout 60000                         //receive timeout in mS

char RXBUFFER[16];                              //create the buffer that received packets are copied into

uint8_t RXPacketL;                              //stores length of packet received
int16_t PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                              //stores signal to noise ratio (SNR) of received packet

char onpacket[] = "LEDOn";                      //send this to turn LED on
char offpacket[] = "LEDOff";                    //send this to turn LED off


void loop()
{
  RXPacketL = LT.receive( (uint8_t*) RXBUFFER, sizeof(RXBUFFER), RXtimeout, WAIT_RX); //wait for a packet to arrive with timeout

  PacketRSSI = LT.readPacketRSSI();             //read the received packets RSSI value
  PacketSNR = LT.readPacketSNR();               //read the received packets SNR value

  if (RXPacketL == 0)                           //if the LT.receive() function detects an error RXpacketL is 0
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();

    if (strncmp((char*) RXBUFFER, onpacket, strlen(onpacket)) == 0)
    {
      Serial.print(F(" Turn LED on"));
      digitalWrite(LED1, HIGH);
    }

    if ( strncmp( (char*) RXBUFFER, offpacket, strlen(offpacket)) == 0)
    {
      Serial.print(F(" Turn LED off"));
      digitalWrite(LED1, LOW);
    }

  }
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


void printElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}


void packet_is_OK()
{
  printElapsedTime();                              //print elapsed time to Serial Monitor
  Serial.print(F(" Received packet > "));
  LT.printASCIIPacket((uint8_t*)RXBUFFER, RXPacketL);        //print the packet as ASCII characters

  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(RXPacketL);
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register

  printElapsedTime();                               //print elapsed time to Serial Monitor
  Serial.print(F(" Packet error > "));

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    Serial.print(F(" RSSI,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm,SNR,"));
    Serial.print(PacketSNR);
    Serial.print(F("dB,Length,"));
    Serial.print(LT.readRXPacketL());               //get the real packet length
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();                            //print the names of the IRQ registers set
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("87_Buffer_Receive_Controller Starting"));
  Serial.println();

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
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
      led_Flash(50, 50);                                                     //long fast speed LED flash indicates device error
    }
  }

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO);   //configure frequency and LoRa settings

  Serial.println(F("Receiver ready"));
}

