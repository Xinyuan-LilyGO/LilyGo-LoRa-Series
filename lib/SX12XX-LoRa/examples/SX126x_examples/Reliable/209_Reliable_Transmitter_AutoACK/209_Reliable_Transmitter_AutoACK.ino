/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 10/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a basic demonstration of the transmission and acknowledgement of a 'Reliable'
  packet.

  A reliable packet has 4 bytes automatically appended to the end of the buffer\array that is the data
  payload. The first two bytes appended are a 16bit 'NetworkID'. The receiver needs to have the same
  NetworkID as configured for the transmitter since the receiver program uses the NetworkID to check that
  the received packet is from a known source.  The third and fourth bytes appended are a 16 bit CRC of
  the payload. The receiver will carry out its own CRC check on the received payload and can then verify
  this against the CRC appended in the packet. The receiver is thus able to check if the payload is valid.

  For a packet to be accepted by the receiver, the networkID and payload CRC appended to the packet by the
  transmitter need to match those from the receiver which gives a high level of assurance that the packet
  is valid.

  If the received packet is valid then the networkID and payload CRC are returned in a 4 byte packet as an
  acknowledgement that the transmitter listens for. If the transmitter does not receive the acknowledgement
  within the ACKtimeout period, the original packet is re-transmitted until a valid acknowledgement is
  received. This program should be used with the matching receiver program, 210_Reliable_Receiver_AutoACK.

  The program will attempt to transmit the packet and have it acknowledged by the receiver a number of times
  as defined by constant TXattempts. If there is no acknowledge withing this time it will be reported.

  It is possible to use the 'NetworkID' to direct the packet to specific receivers.

  Serial monitor baud rate should be set at 115200.

  *******************************************************************************************************/

#include <SPI.h>                                //the LoRa device is SPI based so load the SPI library                                         
#include <SX126XLT.h>                           //include the appropriate library  

SX126XLT LT;                                    //create a library class instance called LT

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //busy pin on LoRa device 
#define DIO1 3                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LORA_DEVICE DEVICE_SX1262               //we need to define the device we are using
#define TXpower 2                               //LoRa transmit power in dBm

#define ACKtimeout 1000                         //Acknowledge timeout in mS                      
#define TXtimeout 1000                          //transmit timeout in mS. If 0 return from transmit function after send.  
#define TXattempts 10                           //number of times to attempt to TX and get an Ack before failing  

uint8_t buff[] = "Hello World";                 //the payload to send
uint16_t PayloadCRC;
uint8_t TXPacketL;

const uint16_t NetworkID = 0x3210;              //NetworkID identifies this connection, needs to match value in receiver


void loop()
{

  //keep transmitting the packet until an ACK is received
  uint8_t attempts = TXattempts;

  do
  {
    Serial.print(F("Transmit Payload > "));
    LT.printASCIIArray(buff, sizeof(buff));     //print the payload buffer as ASCII
    Serial.println();
    Serial.flush();

    Serial.print(F("Send attempt "));
    Serial.println(TXattempts - attempts + 1);

    TXPacketL = LT.transmitReliableAutoACK(buff, sizeof(buff), NetworkID, ACKtimeout, TXtimeout, TXpower, WAIT_TX);
    attempts--;

    if (TXPacketL > 0)
    {
      //if transmitReliable() returns > 0 then transmit and ack was OK
      PayloadCRC = LT.getTXPayloadCRC(TXPacketL);                        //read the actual transmitted CRC from the LoRa device buffer
      packet_is_OK();
      Serial.println();
    }
    else
    {
      //if transmitReliableAutoACK() returns 0 there was an error, timeout etc
      packet_is_Error();
      Serial.println();
    }
    delay(500);                                        //small delay between tranmission attampts
  }
  while ((TXPacketL == 0) && (attempts != 0));

  if (TXPacketL > 0)
  {
    Serial.println(F("Packet acknowledged"));
  }

  if (attempts == 0)
  {
    Serial.print(F("No acknowledge after "));
    Serial.print(TXattempts);
    Serial.print(F(" attempts"));
  }

  Serial.println();
  delay(5000);                                         //have a delay between packets
}


void packet_is_OK()
{
  Serial.print(F("LocalNetworkID,0x"));
  Serial.print(NetworkID, HEX);
  Serial.print(F(",TransmittedPayloadCRC,0x"));        //print CRC of transmitted packet
  Serial.print(PayloadCRC, HEX);
}


void packet_is_Error()
{
  Serial.print(F("No Packet acknowledge"));
  LT.printIrqStatus();                                 //prints the text of which IRQs set
  LT.printReliableStatus();                            //print the reliable status
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("209_Reliable_Transmitter_AutoACK Starting"));

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
}
