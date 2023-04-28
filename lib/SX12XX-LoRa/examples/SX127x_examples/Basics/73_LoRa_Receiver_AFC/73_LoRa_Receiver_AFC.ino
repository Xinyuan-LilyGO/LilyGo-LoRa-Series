/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 10/06/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is demonstration of using automatic frequency adjustments (AFC) on a receiver
  to keep the frequency difference between transmitter and receiver as low as possible. The LoRa settings
  used are defined in the Settings.h file.

  If a packet is received OK, then all that is needed is a call to the LT.doAFC() function, that reads the
  last frequency error, calculates the new offset and changes the set frequency accordingly. 

  When the receiver starts the frequency error may be as large as 4000hz, when the AFC operates the error 
  should reduce to 100hz or so. The first AFC correction to run is doAFCPPM(); which based on the frequency
  error also adjusts the PPM setting. If doAFC(); were only used then as the frequency error is reduced then
  the PPM adjustment would reduce.

  Note that the maximum permitted frequency error between transmitter and receiver is 25% of the bandwidth
  in use. So at 125000hz bandwidth the maximum frequency error is 31500hz, if the bandwidth is 7800hz the
  maximum frequency error is 1950hz. Whilst the AFC functionality can keep transmitter and receiver close
  together when reception is working if the transmitter and receiver are to far apart in frequency for
  reception to work in the first place then AFC cannot operate to correct for the frequency differences.   
  
  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

#include <SPI.h>                                //the lora device is SPI based so load the SPI library
#include <SX127XLT.h>                           //include the appropriate library   
#include "Settings.h"

SX127XLT LT;                                    //create a library class instance called LT

uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into

uint8_t RXPacketL;                              //stores length of packet received
int16_t PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                              //stores signal to noise ratio (SNR) of received packet
int32_t frequencyerror;                         //frequency error of receved packet

bool FirstAFC = true;                           //used to note that AFC has been called more than once 

void loop()
{
  
  Serial.print(F("SetFrequency,"));
  Serial.print(Frequency);
  Serial.print(F("hz,Offset,"));
  Serial.print(LT.getOffset());
  Serial.print(F("hz  "));
    
  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 60000, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  PacketRSSI = LT.readPacketRSSI();              //read the received packets RSSI value
  PacketSNR = LT.readPacketSNR();                //read the received packets SNR value

  if (RXPacketL == 0)                            //if the LT.receive() function detects an error RXpacketL is 0
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
    if (FirstAFC)
    {
    LT.doAFCPPM();                               //the first time AFC is called do the PPM adjust also 
    FirstAFC = false;
    }
    else
    {
    LT.doAFC();                                  //PPM adjust has been done so now just adjust frequency  
    }
  }

  Serial.println();
}


void packet_is_OK()
{
  uint16_t IRQStatus;
  
  
  RXpacketCount++;
  IRQStatus = LT.readIrqStatus();                  //read the LoRa device IRQ status register
  frequencyerror = LT.getFrequencyErrorHz();
  printElapsedTime();                              //print elapsed time to Serial Monitor
  
  Serial.print(F("  "));
  LT.printASCIIPacket(RXBUFFER, RXPacketL);        //print the packet as ASCII characters
  Serial.print(F(","));
  Serial.print(LT.readRegister(REG_FEIMSB),HEX);
  Serial.print(F(","));
  Serial.print(LT.readRegister(REG_FEIMID),HEX);
  Serial.print(F(","));
  Serial.print(LT.readRegister(REG_FEILSB),HEX);
  Serial.print(F(",Regval,"));
  Serial.print(LT.getFrequencyErrorRegValue(),HEX);
  Serial.print(F(",FreqErrror,"));
  Serial.print(frequencyerror);
  Serial.print(F("hz,PpmCorrection,"));
  Serial.print(LT.readRegister(REG_PPMCORRECTION));
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(RXPacketL);
  Serial.print(F(",Packets,"));
  Serial.print(RXpacketCount);
  Serial.print(F(",Errors,"));
  Serial.print(errors);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register

  printElapsedTime();                               //print elapsed time to Serial Monitor

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    errors++;
    Serial.print(F(" PacketError"));
    Serial.print(F(",RSSI,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm,SNR,"));
    Serial.print(PacketSNR);
    Serial.print(F("dB,Length,"));
    Serial.print(LT.readRXPacketL());               //get the real packet length
    Serial.print(F(",Packets,"));
    Serial.print(RXpacketCount);
    Serial.print(F(",Errors,"));
    Serial.print(errors);
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();                            //print the names of the IRQ registers set
  }
}


void printElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}


void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("73_LoRa_Receiver_AFC starting"));
  Serial.println();

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

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  LT.printModemSettings();                               //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                           //reads and prints the configured operating settings, useful check
  Serial.println();
  
  Serial.print(F("Receiver ready - RXBUFFER_SIZE "));
  Serial.println(RXBUFFER_SIZE);
  Serial.println();
}



