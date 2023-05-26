/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 30/12/19

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a remote control transmitter that uses a LoRa link to transmit the positions
  from a simple joystick to a remote receiver. The receiver uses the sent joystick positions to adjust the
  positions of servos. The postions of the joysticks potentiometers on the transmitter are read with the
  analogueRead() function.

  If the joystick has a switch, often made by pressing on the joystick, then this can be used to remote
  control an output on the receiver. The switch is read by an interrupt, the interrupt routine sets a flag
  byte which is read in loop().

  The program is intended as a proof of concept demonstration of how to remote control servos, the program
  is not designed as a practical remote control device for RC model cars for instance.

  It would be straight forward to make the transmitter program send packets continuously, but in most places
  in the world that would break a normal limitation of 10% duty cycle for unlicensed use. Therefore the
  program was designed to only transmit at a 10% duty cycle. Thus the fastest (lowest air time) packets are
  used, spreading factor 6 at a bandwidth of 500khz. This results in an air time for the 5 byte control
  packet of around 4mS, so there are around 25 sent per second.

  To have the transmitter program print out the values read from the joystick, comment in the line;

  //#define DEBUG

  Which is just above the loop() function. With the DEBUG enabled the transmission rate, the rate at which
  the control packets are transmitted will be slowed down.

  To reduce the risk of the receiver picking up LoRa packets from other sources, the packet sent contains a
  'TXidentity' number, valid values are 0 - 65535. The receiver must be setup with the matching identity
  number or the received packets will be ignored.

  The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file. These settings
  are not necessarily optimised for long range.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"
#include <ProgramLT_Definitions.h>

SX127XLT LT;

#include "PinChangeInterrupt.h"             //get the library here; https://github.com/NicoHood/PinChangeInterrupt

uint32_t TXpacketCount;
uint8_t TXPacketL;

uint8_t joystickX1value;                    //variable to read the value from the analog pin
uint8_t joystickY1value;                    //variable to read the value from the analog pin

volatile bool switch1flag = false;

//#define DEBUG                               //comment in thie line (remove the two // at the beggining) for debug output


void loop()
{
  uint8_t switchByte = 0xFF;

  joystickX1value = (uint8_t) (analogRead(joystickX1) / 4) ;        //read the joystick X1 pot, turn 0-1023 into 0 to 255
  joystickY1value = (uint8_t) (analogRead(joystickY1) / 4);        //read the joystick Y1 pot

  if (switch1flag)
  {
    bitClear(switchByte, 1);                       //if the switch is down clear the bit
    digitalWrite(LED1, HIGH);                      //turn on LED as switch indicator
    switch1flag = false;
  }

  if (!sendJoystickPacket(joystickX1value, joystickY1value, switchByte))
  {
    Serial.print(F("Send Error - IRQreg,"));
    Serial.print(LT.readIrqStatus(), HEX);
  }
}


uint8_t sendJoystickPacket(uint16_t X1value, uint16_t Y1value, uint8_t switches)
{
  //The SX12XX buffer is filled with variables of a known type and in a known sequence. Make sure the
  //receiver uses the same variable types and sequence to read variables out of the receive buffer.
  //uint8_t len;
  uint32_t packetStartmS, packettimemS;

  LT.startWriteSXBuffer(0);                      //start the write packet to buffer process
  LT.writeUint8(RControl1);                      //this is the packet type
  LT.writeUint8(TXIdentity);                     //this value represents the transmitter number
  LT.writeUint8(X1value);                        //this byte contains joystick pot AD X1 value to be sent
  LT.writeUint8(Y1value);                        //this byte contains joystick pot AD Y1 value to be sent
  LT.writeUint8(switches);                       //switches value
  LT.endWriteSXBuffer();                         //close the packet, thee are 5 bytes to send

  //now transmit the packet, 10 second timeout, and wait for it to complete sending
  packetStartmS = millis();
  TXPacketL = LT.transmitSXBuffer(0, PacketLength, 10000, TXpower, WAIT_TX);
  packettimemS = millis() - packetStartmS;

#ifdef DEBUG
  Serial.print(TXIdentity);
  Serial.print(F(",X1,"));
  Serial.print(joystickX1value);
  Serial.print(F(",Y1,"));
  Serial.print(joystickY1value);
  Serial.print(F(","));
  Serial.print(switches, BIN);
  Serial.print(F(","));
  Serial.print(packettimemS);
  Serial.print(F("mS"));
  Serial.println();
#endif

  digitalWrite(LED1, LOW);                         //LED off, may have been on due to switch press

  delay(packettimemS * 9);                         //delay for 9 times packet transmit time to ensure 10% duty cycle

  return TXPacketL;                                //TXPacketL will be 0 if there was an error sending
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


void attachInterrupts()
{
  if (SWITCH1  >= 0)
  {
    attachPCINT(digitalPinToPCINT(SWITCH1), wake1, FALLING);
    switch1flag = false;
  }
}


void detachInterrupts()
{
  if (SWITCH1  >= 0)
  {
    detachPCINT(digitalPinToPCINT(SWITCH1));
  }
}


void wake1()
{
  switch1flag = true;
}


void setupSwitches()
{
  if (SWITCH1  >= 0)
  {
    pinMode(SWITCH1, INPUT_PULLUP);
  }
}


void setupLoRa()
{
  //this setup is used so as the implicit packet type,LORA_PACKET_FIXED_LENGTH, is used  
  LT.setMode(MODE_STDBY_RC);                              //got to standby mode to configure device
  LT.setPacketType(PACKET_TYPE_LORA);                     //set for LoRa transmissions
  LT.setRfFrequency(Frequency, Offset);                   //set the operating frequency
  LT.calibrateImage(0);                                   //run calibration after setting frequency
  LT.setModulationParams(SpreadingFactor, Bandwidth, CodeRate, LDRO_AUTO);  //set LoRa modem parameters
  LT.setBufferBaseAddress(0x00, 0x00);                    //where in the SX buffer packets start, TX and RX
  LT.setPacketParams(8, LORA_PACKET_FIXED_LENGTH, PacketLength, LORA_CRC_ON, LORA_IQ_NORMAL);  //set packet parameters
  LT.setSyncWord(LORA_MAC_PRIVATE_SYNCWORD);              //syncword, LORA_MAC_PRIVATE_SYNCWORD = 0x12, or LORA_MAC_PUBLIC_SYNCWORD = 0x34
  LT.setHighSensitivity();                                //set for highest sensitivity at expense of slightly higher LNA current
  //This is the typical IRQ parameters set, actually excecuted in the transmit function
  LT.setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);   //set for IRQ on TX done
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  setupSwitches();

  Serial.begin(115200);

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    led_Flash(2, 125);
  }
  else
  {
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);
    }
  }

  //this function call sets up the device for LoRa using the settings from the Settings.h file
  setupLoRa();

  attachInterrupts();

  Serial.println();
  LT.printModemSettings();                                //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                           //reads and prints the configured operating settings, useful check
  Serial.println();
  Serial.println(F("35_Remote_Control_Servo_Transmitter ready"));
  Serial.println();
}

