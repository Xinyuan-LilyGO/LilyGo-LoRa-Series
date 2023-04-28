/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 19/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a remote control receiver that uses a LoRa link to control the positions of
  servos sent from a remote transmitter.

  If the ttransmitter joystick has a switch, often made by pressing on the joystick, then this can be used
  to remote control an output on the receiver.

  The program is intended as a proof of concept demonstration of how to remote control servos, the program
  is not designed as a practical remote control device for RC model cars for instance.

  It would be straight forward to make the transmitter program send packets continuously, but in most places
  in the world that would break a normal limitation of 10% duty cycle for unlicensed use. Therefore the
  program was designed to only transmit at a 10% duty cycle. Thus the fastest (lowest air time) packets are
  used, spreading factor 6 at a bandwidth of 500khz. This results in an air time for the 5 byte control
  packet of around 4mS, so there are around 25 sent per second.

  To have the receiver program print out the joystick values (0-255) read from the received packet, comment
  in the line;

  //#define DEBUG

  Which is just above the loop() function. With the DEBUG enabled then there is a possibility that some
  transmitted packets will be missed. With the DEBUG line enabled to servos should also sweep to and fro 3
  times at program start-up.

  To reduce the risk of the receiver picking up LoRa packets from other sources, the packet sent contains a
  'TXidentity' number, valid values are 0 - 255. The receiver must be setup with the matching RXIdentity
  number in Settings.h or the received packets will be ignored.

  The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file. These settings
  are not necessarily optimised for long range.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#define programversion "V1.0"

#include <SPI.h>
#include <SX126XLT.h>
#include "Settings.h"
#include <ProgramLT_Definitions.h>

SX126XLT LT;

#include <Servo.h>
Servo ServoX1;                             //create the servo object
Servo ServoY1;                             //create the servo object

uint8_t joystickX1value;                   //variable to read the value from the analog pin
uint8_t joystickY1value;                   //variable to read the value from the analog pin
uint8_t RXPacketL;                         //length of received packet
uint8_t RXPacketType;                      //type of received packet

//#define DEBUG


void loop()
{
  uint16_t IRQStatus;
  
  RXPacketL = LT.receiveSXBuffer(0, 0, WAIT_RX);   //returns 0 if packet error of some sort

  while (!digitalRead(DIO1));                      //wait for DIO1 to go high

  IRQStatus = LT.readIrqStatus();

  if (LT.readIrqStatus() == (IRQ_RX_DONE + IRQ_HEADER_VALID + IRQ_PREAMBLE_DETECTED))
  {
    packet_is_OK();
  }
  else
  {
    Serial.print(F("IRQ,"));
    Serial.print(LT.readIrqStatus(),HEX);
    Serial.print(F(","));
    packet_is_Error();
  }

}


uint8_t packet_is_OK()
{
  //packet has been received, now read from the SX12xx Buffer using the same variable type and
  //order as the transmit side used.
  uint8_t TXIdentity;
  uint16_t pulseX1, pulseY1;
  uint8_t switchByte = 0xFF;              //this is the transmitted switch values, bit 0 = Switch0 etc

  LT.startReadSXBuffer(0);                //start buffer read at location 0
  RXPacketType = LT.readUint8();          //read in the packet type
  TXIdentity = LT.readUint8();            //read in the transmitter number
  joystickX1value = LT.readUint8();       //this byte contains joystick pot AD X1 value sent
  joystickY1value = LT.readUint8();       //this byte contains joystick pot AD Y1 value sent
  switchByte = LT.readUint8();            //read in the Switch values
  RXPacketL = LT.endReadSXBuffer();       //end buffer read

#ifdef DEBUG
  Serial.print(TXIdentity);
  Serial.print(F(",X1,"));
  Serial.print(joystickX1value);
  Serial.print(F(",Y1,"));
  Serial.print(joystickY1value);
  Serial.print(F(","));
  Serial.print(switchByte, BIN);
  Serial.println();
#endif


  if (RXPacketType != RControl1)
  {
    Serial.print(F("Packet type "));
    Serial.println(RXPacketType);
    led_Flash(5, 25);                      //short fast speed flash indicates wrong packet type
    return 0;
  }


  if (TXIdentity != RXIdentity)
  {
    Serial.print(F("TX"));
    Serial.print(TXIdentity);
    Serial.println(F("?"));
    return 0;
  }

  //actionServos
  pulseX1 = map(joystickX1value, 0, 255, 1000, 2000);     //scale the numbers from the joystick
  ServoX1.writeMicroseconds(pulseX1);
  pulseY1 = map(joystickY1value, 0, 255, 1000, 2000);     //scale the numbers from the joystick
  ServoY1.writeMicroseconds(pulseY1);                     //move the servo to position

  //actionOutputs
  if (!bitRead(switchByte, 1))
  {
    digitalWrite(OUTPUT1, !digitalRead(OUTPUT1));          //Toggle Output state
  }

  return RXPacketL;
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  int8_t PacketRSSI;
  IRQStatus = LT.readIrqStatus();

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout"));
  }
  else
  {
    PacketRSSI = LT.readPacketRSSI();                        //read the signal strength of the received packet
    Serial.print(F("Err,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm"));
  }
  Serial.println();
}


void setupOutputs()
{
  //configure the output pins, if a pin is defiend in 'Settings.h' as -1, its not configured, so stays as input
  if (OUTPUT1  >= 0)
  {
    pinMode(OUTPUT1, OUTPUT);
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


void sweepTest(uint8_t num)
{
  uint16_t index1, index2;
  for (index1 = 1; index1 <= num; index1++)
  {
    for (index2 = 900; index2 <= 2100; index2++)
    {
      ServoX1.writeMicroseconds(index2);
      ServoY1.writeMicroseconds(index2);
    }

    delay(1000);

    for (index2 = 2100; index2 >= 900; index2--)
    {
      ServoX1.writeMicroseconds(index2);
      ServoY1.writeMicroseconds(index2);
    }

    delay(1000);
  }
}



void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  setupOutputs();

  Serial.begin(115200);

  ServoX1.attach(pinservoX1);                     //connect pin pinservoX1 to ServoX1 object
  ServoY1.attach(pinservoY1);                     //connect pin pinservoY1 to ServoY1 object

#ifdef DEBUG
  Serial.println(F("Servo sweep test"));
  sweepTest(3);
#endif

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                          //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("36_Remote_Control_Servo_Receiver ready"));
  Serial.println();
}



