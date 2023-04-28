/*****************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 17/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

#define programversion "V1.1"
#define Serial_Monitor_Baud 9600

#include <SPI.h>
#include <SX128XLT.h>
SX128XLT LT;
#include "Settings.h"

uint8_t distance_zero_count;
uint16_t calvalue, calibrationstart, calibrationend;
uint16_t rangeing_error_count, rangeing_valid_count;
uint16_t IrqStatus;
uint32_t endwaitmS, startrangingmS, range_result;
float distance;


void loop()
{
  uint16_t index;
  distance_zero_count = 0;

  for (index = calibrationstart; index <= calibrationend; index = index + 10)
  {
    digitalWrite(LED1, HIGH);
    LT.setRangingCalibration(index);
    Serial.print(F("TransmitRanging,Calibration,"));
    Serial.print(index);
    LT.transmitRanging(RangingAddress, TXtimeoutmS, TXpower, NO_WAIT);
    digitalWrite(LED1, LOW);

    endwaitmS = millis() + waittimemS;
    startrangingmS = millis();

    while (!(digitalRead(DIO1)) && (millis() < endwaitmS));           //wait for Ranging valid or timeout

    delay(10);

    IrqStatus = LT.readIrqStatus();
    Serial.print(F(",IRQ,"));
    Serial.print(IrqStatus, HEX);

    if (IrqStatus & IRQ_RANGING_MASTER_RESULT_TIMEOUT)
    {
      rangeing_error_count++;
      Serial.print(F(",  RangingTimeout!  "));
    }

    if (millis() > endwaitmS)
    {
      Serial.print(F(",ProgramTimeout"));
    }

    if (IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID)
    {
      rangeing_valid_count++;
      digitalWrite(LED1, HIGH);
      Serial.print(F(",Valid"));
      range_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);
      Serial.print(F(",RAW,"));
      Serial.print(range_result, HEX);

      distance = LT.getRangingDistance(RANGING_RESULT_RAW, range_result, 1);

      Serial.print(F(",Distance,"));
      Serial.print(distance, 1);
      Serial.print(F("m"));
      Serial.print(F(",Time,"));
      Serial.print(millis() - startrangingmS);
      Serial.print("mS");
      digitalWrite(LED1, LOW);
    }

    Serial.print(F(",OKCount,"));
    Serial.print(rangeing_valid_count);
    Serial.print(F(",ErrorCount,"));
    Serial.print(rangeing_error_count);

    if (distance == 0)
    {
      Serial.print(F(",  Distance is Zero!"));
      distance_zero_count++;
    }

    if (distance_zero_count >= 3)
    {
      delay(5000);
      break;
    }

    Serial.println();
    delay(packet_delaymS);

  }

  Serial.println();
  Serial.println();
  Serial.println();
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  unsigned int index;

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
  uint16_t Remainder;
  Serial.println();
  Serial.println();
  Serial.begin(Serial_Monitor_Baud);            //setup Serial console ouput
  Serial.println();
  Serial.println(__FILE__);
  Serial.print(F("Compiled "));
  Serial.print(__TIME__);
  Serial.print(F(" "));
  Serial.println(__DATE__);
  Serial.println(F(programversion));
  Serial.println(F("Stuart Robinson"));
  Serial.println();

  Serial.println(F("56_Ranging_Calibration_Checker Starting"));

  pinMode(LED1, OUTPUT);

  led_Flash(2, 125);

  Serial.println(F("Checking device"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX_EN, TX_EN, LORA_DEVICE))
  {
    Serial.println(F("Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                  //long fast speed flash indicates device error
    }
  }

  //The function call list below shows the complete setup for the LoRa device for ranging using the information
  //defined in the Settings.h file.
  //The 'Setup LoRa device for Ranging' list below can be replaced with a single function call, note that
  //the calibration value will be loaded automatically from the table in the library;

  //LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, RangingAddress, RangingRole);

  //***************************************************************************************************
  //Setup LoRa device for Ranging Master
  //***************************************************************************************************
  LT.setMode(MODE_STDBY_RC);
  LT.setPacketType(PACKET_TYPE_RANGING);
  LT.setModulationParams(SpreadingFactor, Bandwidth, CodeRate);
  LT.setPacketParams(12, LORA_PACKET_VARIABLE_LENGTH, 0, LORA_CRC_ON, LORA_IQ_NORMAL, 0, 0);
  LT.setRfFrequency(Frequency, Offset);
  LT.setTxParams(TXpower, RADIO_RAMP_02_US);
  LT.setRangingMasterAddress(RangingAddress);
  LT.setRangingSlaveAddress(RangingAddress);
  LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RANGING_MASTER_RESULT_VALID + IRQ_RANGING_MASTER_RESULT_TIMEOUT), 0, 0);              //set for IRQ on RX done
  LT.setRangingCalibration(LT.lookupCalibrationValue(SpreadingFactor, Bandwidth));
  LT.setRangingRole(RANGING_MASTER);
  LT.writeRegister(REG_RANGING_FILTER_WINDOW_SIZE, 8); //set up window size for ranging averaging
  LT.setHighSensitivity();
  //***************************************************************************************************

  //LT.setRangingCalibration(Calibration);               //override automatic lookup of calibration value from library table

  calvalue = LT.getSetCalibrationValue();
  Remainder = calvalue / 10;
  calibrationstart = (Remainder * 10) - 1000;
  calibrationend = (Remainder * 10) + 1000;
  Serial.print(F("CalibrationStart,"));
  Serial.print(calibrationstart);
  Serial.print(F(",CalibrationEnd,"));
  Serial.println(calibrationend);
  Serial.println();

  delay(2000);
}
