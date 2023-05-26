/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -  This program is an example of a basic GPS tracker. The program reads the GPS,
  waits for an updated fix and transmits location and altitude, number of satellites in view, the HDOP
  value, the fix time of the GPS and the battery voltage. This transmitter can be also be used to
  investigate GPS performance. At startup there should be a couple of seconds of recognisable text from
  the GPS printed to the serial monitor. If you see garbage or funny characters its likley the GPS baud
  rate is wrong. If the transmitter is turned on from cold, the receiver will pick up the cold fix time,
  which is an indication of GPS performance. The GPS will be powered on for around 4 seconds before the
  timing of the fix starts. Outside with a good view of the sky most GPSs should produce a fix in around
  45 seconds. The number of satellites and HDOP are good indications to how well a GPS is working.

  The program writes direct to the LoRa devices internal buffer, no memory buffer is used.

  The LoRa settings are configured in the Settings.h file.

  The program has the option of using a pin to control the power to the GPS (GPSPOWER), if the GPS module
  or board being used has this feature. To not use this feature set the define for GPSPOWER in the
  Settings.h file to '#define GPSPOWER -1'. Also set the GPSONSTATE and GPSOFFSTATE to the appropriate logic
  levels.

  There is also an option of using a logic pin to turn the resistor divider used to read battery voltage on
  and off. This reduces current used in sleep mode. To use the feature set the define for pin BATVREADON
  in 'Settings.h' to the pin used. If not using the feature set the pin number to -1.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.1"
#define authorname "Stuart Robinson"

#include <SPI.h>
#include <SX128XLT.h>

#include "Settings.h"
#include <ProgramLT_Definitions.h>

SX128XLT LT;

#include <TinyGPS++.h>                             //get library here > http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                   //create the TinyGPS++ object


#ifdef USE_SOFTSERIAL_GPS
#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);
#else
#define GPSserial HardwareSerialPort               //hardware serial port (eg Serial1) is configured in the Settings.h file
#endif


uint8_t TXStatus = 0;                              //used to store current status flag bits of Tracker transmitter (TX)
uint8_t TXPacketL;                                 //length of LoRa packet (TX)
float TXLat;                                       //Latitude from GPS on Tracker transmitter (TX)
float TXLon;                                       //Longitude from GPS on Tracker transmitter (TX)
float TXAlt;                                       //Altitude from GPS on Tracker transmitter (TX)
uint8_t TXSats;                                    //number of GPS satellites seen (TX)
uint32_t TXHdop;                                   //HDOP from GPS on Tracker transmitter (TX)
uint16_t TXVolts;                                  //Volts (battery) level on Tracker transmitter (TX)
uint32_t TXGPSFixTime;                             //GPS fix time in hot fix mode of GPS on Tracker transmitter (TX)
uint32_t TXPacketCount, TXErrorsCount;             //keep count of OK packets and send errors


void loop()
{

  if (gpsWaitFix(WaitGPSFixSeconds))
  {
    sendLocation(TXLat, TXLon, TXAlt, TXHdop, TXGPSFixTime);
    Serial.println();
    Serial.print(F("Waiting "));
    Serial.print(Sleepsecs);
    Serial.println(F("s"));
    delay(Sleepsecs * 1000);                        //this sleep is used to set overall transmission cycle time
  }
  else
  {
    send_Command(NoFix);                            //send notification of no GPS fix.
  }
}


bool gpsWaitFix(uint32_t waitSecs)
{
  //waits a specified number of seconds for a fix, returns true for good fix
  uint32_t endwaitmS, GPSonTime;
  bool GPSfix = false;
  float tempfloat;
  uint8_t GPSchar;

  GPSonTime = millis();
  GPSserial.begin(9600);                         //start GPSserial

  Serial.print(F("Wait GPS Fix "));
  Serial.print(waitSecs);
  Serial.println(F("s"));

  endwaitmS = millis() + (waitSecs * 1000);

  while (millis() < endwaitmS)
  {
    if (GPSserial.available() > 0)
    {
      GPSchar = GPSserial.read();
      gps.encode(GPSchar);
    }

    if (gps.location.isUpdated() && gps.altitude.isUpdated())
    {
      GPSfix = true;
      Serial.print(F("Have GPS Fix "));
      TXGPSFixTime = millis() - GPSonTime;
      Serial.print(TXGPSFixTime);
      Serial.println(F("mS"));

      TXLat = gps.location.lat();
      TXLon = gps.location.lng();
      TXAlt = gps.altitude.meters();
      TXSats = gps.satellites.value();
      TXHdop = gps.hdop.value();
      tempfloat = ( (float) TXHdop / 100);

      Serial.print(TXLat, 5);
      Serial.print(F(","));
      Serial.print(TXLon, 5);
      Serial.print(F(","));
      Serial.print(TXAlt, 1);
      Serial.print(F(","));
      Serial.print(TXSats);
      Serial.print(F(","));
      Serial.print(tempfloat, 2);
      Serial.println();

      break;                                  //exit while loop reading GPS
    }
  }

  //if here then there has either been a fix or no fix and a timeout

  if (GPSfix)
  {
    setStatusByte(GPSFix, 1);                 //set status bit to flag a GPS fix
  }
  else
  {
    setStatusByte(GPSFix, 0);                 //set status bit to flag no fix
    Serial.println();
    Serial.println(F("Timeout - No GPSFix"));
    Serial.println();
    GPSfix = false;
  }

  GPSserial.end();                            //serial RX interrupts interfere with SPI, so stop GPSserial
  return GPSfix;
}


void sendLocation(float Lat, float Lon, float Alt, uint32_t Hdop, uint32_t fixtime)
{
  uint8_t len;
  uint16_t IRQStatus;

  Serial.print(F("Send Location"));

  TXVolts = readSupplyVoltage();              //get the latest supply\battery volts

  LT.startWriteSXBuffer(0);                   //initialise buffer write at address 0
  LT.writeUint8(LocationPacket);              //indentify type of packet
  LT.writeUint8(Broadcast);                   //who is the packet sent too
  LT.writeUint8(ThisNode);                    //tells receiver where is packet from
  LT.writeFloat(Lat);                         //add latitude
  LT.writeFloat(Lon);                         //add longitude
  LT.writeFloat(Alt);                         //add altitude
  LT.writeUint8(TXSats);                      //add number of satellites
  LT.writeUint32(Hdop);                       //add hdop
  LT.writeUint8(TXStatus);                    //add tracker status
  LT.writeUint32(fixtime);                    //add GPS fix time in mS
  LT.writeUint16(TXVolts);                    //add tracker supply volts
  LT.writeUint32(millis());                   //add uptime in mS
  len = LT.endWriteSXBuffer();                //close buffer write

  digitalWrite(LED1, HIGH);
  TXPacketL = LT.transmitSXBuffer(0, len, 10000, TXpower, WAIT_TX);
  digitalWrite(LED1, LOW);

  if (TXPacketL)
  {
    TXPacketCount++;
    Serial.println(F(" - Done "));
    Serial.print(F("SentOK,"));
    Serial.print(TXPacketCount);
    Serial.print(F(",Errors,"));
    Serial.println(TXErrorsCount);
  }
  else
  {
    //if here there was an error transmitting packet
    TXErrorsCount++;
    IRQStatus = LT.readIrqStatus();                  //read the the interrupt register
    Serial.print(F(" SendError,"));
    Serial.print(F("Length,"));
    Serial.print(TXPacketL);                         //print transmitted packet length
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);                    //print IRQ status
    LT.printIrqStatus();                             //prints the text of which IRQs set
    Serial.println();
  }
}


void setStatusByte(uint8_t bitnum, uint8_t bitval)
{
  //program the status byte

  if (bitval == 0)
  {
    bitClear(TXStatus, bitnum);
  }
  else
  {
    bitSet(TXStatus, bitnum);
  }
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  //flash LED to show tracker is alive
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void send_Command(char cmd)
{
  bool SendOK;
  uint8_t len;

  Serial.print(F("Send Cmd "));
  Serial.write(cmd);

  LT.startWriteSXBuffer(0);
  LT.writeUint8(cmd);                         //packet addressing used indentify type of packet
  LT.writeUint8(Broadcast);                   //who is the packet sent to
  LT.writeUint8(ThisNode);                    //where is packet from
  LT.writeUint16(TXVolts);
  len = LT.endWriteSXBuffer();

  digitalWrite(LED1, HIGH);
  SendOK = LT.transmitSXBuffer(0, len, 10000, TXpower, WAIT_TX);   //timeout set at 10 seconds
  digitalWrite(LED1, LOW);

  if (SendOK)
  {
    Serial.println(F(" - Done"));
  }
  else
  {
    Serial.println(F(" - Error"));
  }
}


uint16_t readSupplyVoltage()
{
  //relies on 1V internal reference and 91K & 11K resistor divider
  //returns supply in mV @ 10mV per AD bit read
  uint16_t temp;
  uint16_t voltage = 0;
  uint8_t index;

  if (BATVREADON >= 0)
  {
    digitalWrite(BATVREADON, HIGH);            //turn on MOSFET connecting resitor divider in circuit
  }

  analogReference(INTERNAL);
  temp = analogRead(SupplyAD);

  for (index = 0; index <= 4; index++)         //sample AD 5 times
  {
    temp = analogRead(SupplyAD);
    voltage = voltage + temp;
  }

  if (BATVREADON >= 0)
  {
    digitalWrite(BATVREADON, LOW);             //turn off MOSFET connecting resitor divider in circuit
  }


  voltage = ((voltage / 5) * ADMultiplier) + DIODEMV;
  return voltage;
}


void GPSON()
{
  if (GPSPOWER >= 0)
  {
    digitalWrite(GPSPOWER, GPSONSTATE);                         //power up GPS
  }
}


void GPSOFF()
{
  if (GPSPOWER)
  {
    digitalWrite(GPSPOWER, GPSOFFSTATE);                        //power off GPS
  }
}


void setup()
{
  uint32_t endmS;

  if (GPSPOWER >= 0)
  {
    pinMode(GPSPOWER, OUTPUT);
    GPSON();
  }

  if (BATVREADON >= 0)
  {
    pinMode(BATVREADON, OUTPUT);
  }

  pinMode(LED1, OUTPUT);                                      //setup pin as output for indicator LED
  led_Flash(2, 125);                                          //two quick LED flashes to indicate program start

  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("23_GPS_Tracker_Transmitter Starting"));

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, RX_EN, TX_EN, LORA_DEVICE))
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
      led_Flash(50, 50);                          //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  Serial.println();
  LT.printModemSettings();                         //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                    //reads and prints the configured operating settings, useful check
  Serial.println();

  TXVolts = readSupplyVoltage();

  Serial.print(F("Supply "));
  Serial.print(TXVolts);
  Serial.println(F("mV"));

  send_Command(PowerUp);                           //send power up command, includes supply mV

  Serial.println(F("Startup GPS check"));

  GPSserial.begin(9600);

  endmS = millis() + echomS;

  while (millis() < endmS)
  {
    while (GPSserial.available() > 0)
      Serial.write(GPSserial.read());
  }
  Serial.println();
  Serial.println();

  Serial.println(F("Wait for first GPS fix"));
  gpsWaitFix(WaitFirstGPSFixSeconds);

  sendLocation(TXLat, TXLon, TXAlt, TXHdop, TXGPSFixTime);
}
