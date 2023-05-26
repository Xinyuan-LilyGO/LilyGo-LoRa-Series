/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


/*******************************************************************************************************
  Program Operation - This is a basic demonstration of the transmission of a 'Reliable' packet used to
  control outputs on a remote receiver.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. This receiver needs to have the same
  NetworkID as configured for the transmitter. The receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended is a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended to the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  This example program and the matching receiver, 204_Reliable_Receiver_Controller_Structure, transmits a
  data structure that contains variables which when read by the remote receiver flashes an LED attached to
  the receiver on and off.

  When using these transmit and receive examples the Arduino types should be the same as the structure
  formats can be different on some Arduinos.

  Serial monitor baud rate should be set at 115200.
*******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX126XLT.h>                           //include the appropriate library  

SX126XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //LED used to indicate transmission 
#define LORA_DEVICE DEVICE_SX1262               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm
uint8_t TXPacketL;

#define TXtimeout 1000                          //transmit timeout in mS. If 0 return from transmit function after send.  

struct controllerStructure
{
  uint16_t destinationNode;
  uint8_t outputNumber;
  uint8_t onoroff;
} __attribute__((packed, aligned(1)));          //remove structure padding so there is compatibility between 8bit and 32bit Arduinos

struct controllerStructure controller1;         //define an instance called controller1 of the structure controllerStructure
uint16_t PayloadCRC;
uint8_t TXPayloadL;
const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in receiver


void loop()
{

  controller1.destinationNode = 2;              //node number we are controlling, 0 to 65535
  controller1.outputNumber = 1;                 //output number on node we are controlling

  if (controller1.onoroff == 0)                 //toggle the on/off status
  {
    controller1.onoroff = 1;
  }
  else
  {
    controller1.onoroff = 0;
  }

  Serial.print(F("Transmit Structure > "));
  TXPayloadL = sizeof(controller1);
  LT.printHEXPacket((uint8_t *) &controller1, sizeof(controller1));       //print the sent array as HEX
  Serial.println();
  Serial.flush();

  //now transmit the packet
  digitalWrite(LED1, HIGH);                                               //LED on to indicate transmit

  TXPacketL = LT.transmitReliable((uint8_t *) &controller1, sizeof(controller1), NetworkID, TXtimeout, TXpower, WAIT_TX);  //will return packet length > 0 if sent OK, otherwise 0 if transmit error

  if (TXPacketL > 0)
  {
    //if transmitReliable() returns > 0 then transmit was OK
    PayloadCRC = LT.getTXPayloadCRC(TXPacketL);                 //read the actual transmitted CRC from the LoRa device buffer
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
  delay(5000);                                                   //have a delay between packets
}


void packet_is_OK()
{
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmittedPayloadCRC,0x"));                  //print CRC of transmitted packet
  Serial.print(PayloadCRC, HEX);
}


void packet_is_Error()
{
  Serial.print(F("SendError"));
  LT.printIrqStatus();                                 //prints the text of which IRQs set
  LT.printReliableStatus();                            //print the reliable status
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("203_Reliable_Transmitter_Controller_Structure Starting"));

  pinMode(LED1, OUTPUT);

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    while (1);
  }

  LT.setupLoRa(434000000, 0, LORA_SF7, LORA_BW_125, LORA_CR_4_5, LDRO_AUTO);

  Serial.println(F("Transmitter ready"));
  Serial.println();

  //enable the following line if you want to disable payload CRC checking
  //LT.setReliableConfig(NoReliableCRC);                 //disable payload CRC check

}
