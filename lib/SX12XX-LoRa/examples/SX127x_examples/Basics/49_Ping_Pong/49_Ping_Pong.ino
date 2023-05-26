/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 02/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a simple demonstration of using one sketch to send and receive LoRa packets.

  The program first transmits the contents of the buff[] array then waits with a 2 second timeout to receive
  a packet. If a packet is received from another other Arduino it is displayed on the serial monitor and the
  program repeats.

  If one Arduino is setup with the transmitted buffer\array as Ping;

  uint8_t buff[] = "Ping";

  and another Arduino is setup to send Pong;

  uint8_t buff[] = "Pong";

  Then the serial monitor should display Ping Pong messages.

  Serial monitor baud rate is set at 9600
*******************************************************************************************************/

#include <SPI.h>                                //the lora device is SPI based so load the SPI library                                         
#include <SX127XLT.h>                           //include the appropriate library  

SX127XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define DIO0 3                                  //DIO0 pin on LoRa device, used for sensing RX and TX done 
#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using
#define TXpower 10                              //LoRa transmit power in dBm

uint8_t RXPacketL;                              //used to store the received packet length
uint8_t RXBUFFER[16];                           //create the buffer that received packets are copied into

uint8_t buff[] = "Ping";                        //the message to send, set one Arduino to Ping, the other to Pong
//uint8_t buff[] = "Pong";                      //the message to send, set one Arduino to Ping, the other to Pong

void loop()
{
  Serial.print(F("Transmit Packet > "));
  Serial.flush();

  LT.printASCIIPacket(buff, sizeof(buff));                        //print the buffer (the sent packet) as ASCII

  //Transmit a packet
  if (!LT.transmit(buff, sizeof(buff), 10000, TXpower, WAIT_TX))  //will return 0 if transmit error
  {
    Serial.print(F("Transmit failed"));
    LT.printIrqStatus();                                          //print IRQ status, indicates why packet transmit fail
    Serial.println();
  }

  Serial.println();
  Serial.print(F("Listen for packet > "));

  //Receive a packet with timeout
  if (LT.receive(RXBUFFER, sizeof(RXBUFFER), 2000, WAIT_RX))      //wait for a packet to arrive with 2 second (2000mS) timeout
  {
    RXPacketL = LT.readRegister(REG_RXNBBYTES);
    LT.printASCIIPacket(RXBUFFER, RXPacketL);                     //print the packet as ASCII characters
    Serial.println();
  }
  else
  {
    Serial.print(F("Receive failed"));
    LT.printIrqStatus();                                          //print IRQ status, why did packet receive fail
    Serial.println();
  }

  Serial.println();
  delay(500);                                                     //have a delay between packets
}


void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("49_Ping_Pong Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1);
  }

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO); //configure frequency and LoRa settings

  Serial.print(F("Ping_Pong ready"));
  Serial.println();
  Serial.println();
}
