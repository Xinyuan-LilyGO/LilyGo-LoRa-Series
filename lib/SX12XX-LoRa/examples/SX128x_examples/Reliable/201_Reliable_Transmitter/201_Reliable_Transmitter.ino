/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 25/09/21

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a basic demonstration of the transmission of a 'Reliable' packet.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. The receiver needs to have the same
  NetworkID as configured for the transmitter since the receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended are a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended in the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  It is possible to use the 'NetworkID' to direct the packet to specific receivers.

  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX128XLT.h>                           //include the appropriate library  

SX128XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //RFBUSY pin on LoRa device
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //LED used to indicate transmission
#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm
#define TXtimeout 5000                          //transmit timeout in mS. If 0 return from transmit function after send.  

uint8_t buff[] = "Hello World";                 //the payload to send
uint16_t PayloadCRC;
uint8_t TXPayloadL;                             //this is the payload length sent
uint8_t TXPacketL;

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in receiver


void loop()
{
  Serial.print(F("Transmit Payload > "));
  TXPayloadL = sizeof(buff);
  LT.printASCIIArray(buff, TXPayloadL);         //print the payload buffer as ASCII
  Serial.println();

  if (LT.getReliableConfig(NoReliableCRC))
  {
    Serial.println(F("Payload CRC check disabled"));
  }
  Serial.flush();

  //now transmit the packet
  digitalWrite(LED1, HIGH);                                            //LED on to indicate transmit
  TXPacketL = LT.transmitReliable(buff, TXPayloadL, NetworkID, TXtimeout, TXpower, WAIT_TX);  //will return packet length > 0 if sent OK, otherwise 0 if transmit error

  if (TXPacketL > 0)
  {
    //if transmitReliable() returns > 0 then transmit was OK
    PayloadCRC = LT.getTXPayloadCRC(TXPacketL);                        //read the actual transmitted CRC from the LoRa device buffer
    packet_is_OK();
    Serial.println();
  }
  else
  {
    //if transmitReliable() returns 0 there was an error
    packet_is_Error();
    Serial.println();
  }

  digitalWrite(LED1, LOW);
  Serial.println();
  delay(5000);                    //have a delay between packets
}


void packet_is_OK()
{
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmittedPayloadCRC,0x"));       //print CRC of transmitted payload
  Serial.print(PayloadCRC, HEX);
}


void packet_is_Error()
{
  Serial.print(F("SendError"));
  LT.printIrqStatus();                                //prints the text of which IRQs set
  LT.printReliableStatus();                           //print the reliable status
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
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("201_Basic_Reliable_Transmitter Starting"));

  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    do
    {
      digitalWrite(LED1, HIGH);
      delay(50);
      digitalWrite(LED1, LOW);
      delay(50);
    } while (1);
  }

  LT.setupLoRa(2445000000, 0, LORA_SF7, LORA_BW_0400, LORA_CR_4_5);

  Serial.println();
  LT.printModemSettings();                               //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                           //reads and prints the configured operating settings, useful check
  Serial.println();
  Serial.println();
  LT.printRegisters(0x900, 0x9FF);                       //print contents of device registers, normally 0x900 to 0x9FF
  Serial.println();
  Serial.println(F("Transmitter ready"));
  Serial.println();

  //enable the following line if you want to disable payload CRC checking
  //LT.setReliableConfig(NoReliableCRC);                   //disable payload CRC check
}
