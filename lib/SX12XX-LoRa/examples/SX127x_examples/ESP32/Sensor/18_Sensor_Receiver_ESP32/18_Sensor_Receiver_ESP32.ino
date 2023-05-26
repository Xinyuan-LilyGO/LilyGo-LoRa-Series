/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/01/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program receives a LoRa packet without using a processor buffer, the LoRa devices
  internal buffer is read direct for the received sensor data. 
  
  The sensor used in the matching '17_Sensor_Transmiter' program is a BME280 and the pressure, humidity,
  and temperature are being and received. There is also a 16bit value of battery mV and and a 8 bit status
  value at the end of the packet.

  When a packet is received its printed and assuming the packet is validated, the sensor results are printed
  to the serial monitor and screen.

  For the sensor data to be accepted as valid the folowing need to match;

  The 16bit CRC on the received sensor data must match the CRC value transmitted with the packet.
  The packet must start with a byte that matches the packet type sent, 'Sensor1'
  The RXdestination byte in the packet must match this node ID of this receiver node, defined by 'This_Node'

  In total thats 16 + 8 + 8  = 32bits of checking, so a 1:4294967296 chance (approx) that an invalid
  packet is acted on and erroneous values displayed.

  The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include <SX127XLT.h>
#include "Settings.h"
#include <ProgramLT_Definitions.h>

SX127XLT LT;

#include <U8x8lib.h>                                        //get library here >  https://github.com/olikraus/u8g2 
U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);      //use this line for standard 0.96" SSD1306
//U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);     //use this line for 1.3" OLED often sold as 1.3" SSD1306

uint32_t RXpacketCount;          //count of all packets received
uint32_t ValidPackets;           //count of packets received with valid data
uint32_t RXpacketErrors;         //count of all packets with errors received
bool packetisgood;

uint8_t RXPacketL;               //length of received packet
int8_t  PacketRSSI;              //RSSI of received packet
int8_t  PacketSNR;               //signal to noise ratio of received packet

uint8_t RXPacketType;
uint8_t RXDestination;
uint8_t RXSource;
float temperature;               //the BME280 temperature value
float pressure;                  //the BME280 pressure value
uint16_t humidity;               //the BME280 humididty value
uint16_t voltage;                //the battery voltage value
uint8_t statusbyte;              //a status byte, not currently used
uint16_t TXCRCvalue;             //the CRC value of the packet data as transmitted


void loop()
{
  RXPacketL = LT.receiveSXBuffer(0, 0, WAIT_RX);   //returns 0 if packet error of some sort, no timeout set

  digitalWrite(LED1, HIGH);      //something has happened

  PacketRSSI = LT.readPacketRSSI();
  PacketSNR = LT.readPacketSNR();

  if (RXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_Received_OK();                            //its a valid packet LoRa wise, but it might not be a packet we want
  }

  digitalWrite(LED1, LOW);
  Serial.println();
  //SPI.endTransaction();
}


void packet_Received_OK()
{
  //a LoRa packet has been received, which has passed the internal LoRa checks, including CRC, but it could be from
  //an unknown source, so we need to check that its actually a sensor packet we are expecting, and has valid sensor data

  uint8_t len;
  uint8_t contenterrors;                 //keep a count of errors found in packet

  RXpacketCount++;
  Serial.print(RXpacketCount);
  Serial.print(F(",PacketsReceived,"));

  LT.startReadSXBuffer(0);
  RXPacketType = LT.readUint8();
  RXDestination = LT.readUint8();
  RXSource = LT.readUint8();

  /************************************************************************
    Highlighted section - this is where the actual sensor data is read from
    the packet
  ************************************************************************/
  temperature = LT.readFloat();             //the BME280 temperature value
  pressure = LT.readFloat();                //the BME280 pressure value
  humidity = LT.readUint16();               //the BME280 humididty value
  voltage = LT.readUint16();                //the battery voltage value
  statusbyte = LT.readUint8();              //a status byte, not currently used
  /************************************************************************/

  len = LT.endReadSXBuffer();                //note len is bytes read from packet, may be different to actual packet length

  printreceptionDetails();                   //print details of reception, RSSI etc
  Serial.println();

  contenterrors = checkPacketValid(len);     //pass length of packet to check routine 

  if (contenterrors == 0)
  {
    Serial.println(F("  Packet is good"));
    ValidPackets++;
    printSensorValues();                     //print the sensor values
    Serial.println();
    printPacketCounts();                     //print count of valid packets and errors                  
    displayscreen1();
    Serial.println();
  }
  else
  {
  Serial.println(F("  Packet is not valid"));
  RXpacketErrors++;
  disp.clearLine(7);
  disp.setCursor(0, 7);
  disp.print(F("Errors "));
  disp.print(RXpacketErrors);  
  }
}


uint8_t checkPacketValid(uint8_t len)
{
  //this function checks if the packet is valid and will be displayed
  
  uint8_t errors = 0;

  if (RXPacketType != Sensor1)                        //is it a Sensor1 type packet
  {
    errors++;
  }

  if (RXDestination != This_Node)                     //was the packet sent to this receiver node ? 
  {
    errors++;
  }

  if (!checkCRCvalue(len))                            //is the sent CRC value of sensor data valid ? 
  {
    errors++;
  }

  Serial.println();
  Serial.print(F("Error Check Count = "));
  Serial.print(errors);
  return errors;
}


bool checkCRCvalue(uint8_t len)
{
  uint16_t CRCSensorData;
  //uint8_t msb, lsb;

  CRCSensorData = LT.CRCCCITTSX(3, (len-1), 0xFFFF);    //calculate the CRC of packet sensor data

  Serial.print(F("(CRC of Received sensor data "));
  Serial.print(CRCSensorData, HEX);
  Serial.print(F(")"  ));

  //SPI.endTransaction();

  TXCRCvalue = ((LT.getByteSXBuffer(17) << 8) + (LT.getByteSXBuffer(16)));

  Serial.print(F("(CRC transmitted "));
  Serial.print(TXCRCvalue, HEX);
  Serial.print(F(")"  ));

  if (TXCRCvalue != CRCSensorData)
  {
    Serial.print(F(" Sensor Data Not Valid"));
    return false;
  }
  else
  {
    Serial.print(F(" Sensor Data is Valid"));
    return true;
  }

}


void printSensorValues()
{
  Serial.print(F("Temperature,"));
  Serial.print(temperature, 1);
  Serial.print(F("c,Pressure,"));
  Serial.print(pressure, 0);
  Serial.print(F("Pa,Humidity,"));
  Serial.print(humidity);
  Serial.print(F("%,Voltage,"));
  Serial.print(voltage);
  Serial.print(F("mV,Status,"));
  Serial.print(statusbyte, HEX);
  Serial.print(F(",CRC,"));
  Serial.print(TXCRCvalue, HEX);
  Serial.flush();
}


void printreceptionDetails()
{
  Serial.print(F("RSSI"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL()); 
}


void printPacketCounts()
{
  Serial.print(F("ValidPackets,"));
  Serial.print(ValidPackets);
  Serial.print(F(",Errors,"));
  Serial.print(RXpacketErrors);  
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  RXpacketErrors++;
  IRQStatus = LT.readIrqStatus();

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout "));
  }
  else
  {
    Serial.print(F("PacketError "));
    printreceptionDetails();
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();
    Serial.println();
    disp.clearLine(7);
    disp.setCursor(0, 7);
    disp.print(F("Errors "));
    disp.print(RXpacketErrors);
  }
}


void displayscreen1()
{
  //show sensor data on display
  disp.clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("Sensor "));
  disp.print(RXSource);
  disp.clearLine(1);
  disp.setCursor(0, 1);
  disp.print(temperature, 1);
  disp.print(F("c"));
  disp.clearLine(2);
  disp.setCursor(0, 2);
  disp.print(pressure, 0);
  disp.print(F("Pa"));
  disp.clearLine(3);
  disp.setCursor(0, 3);
  disp.print(humidity);
  disp.print(F("%"));
  disp.clearLine(4);
  disp.setCursor(0, 4);
  disp.print(voltage);
  disp.print(F("mV"));
  disp.clearLine(6);
  disp.setCursor(0, 6);
  disp.print(F("ValidPkts "));
  disp.print(ValidPackets);
  disp.setCursor(0, 7);
  disp.print(F("Errors "));
  disp.print(RXpacketErrors);
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
  pinMode(VCCPOWER, OUTPUT);                    //this pin switches power for external devices, lora and SD card
  digitalWrite(VCCPOWER, LOW);                  //turn power on 
  
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  Serial.begin(9600);

  disp.begin();
  disp.setFont(u8x8_font_chroma48medium8_r);

  disp.clear();
  disp.setCursor(0, 0);
  disp.print(F("Check LoRa"));
  disp.setCursor(0, 1);

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    disp.print(F("LoRa OK"));
    led_Flash(2, 125);
  }
  else
  {
    disp.print(F("Device error"));
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Receiver ready"));
  Serial.println();
}

