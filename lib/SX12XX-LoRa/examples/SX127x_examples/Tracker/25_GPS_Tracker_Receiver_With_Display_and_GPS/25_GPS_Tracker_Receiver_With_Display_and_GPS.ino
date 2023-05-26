/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 28/12/20
  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
Program Operation -  This program is an example of a functional GPS tracker receiver using lora. 
It is capable of picking up the trackers location packets from many kilometres away with only basic antennas. 

The program receives the location packets from the remote tracker transmitter and writes them on an OLED
display and also prints the information to the Arduino IDE serial monitor. The program can read a locally
attached GPS and when that has a fix, will display the distance and direction to the remote tracker. 

The program writes direct to the lora devices internal buffer, no memory buffer is used. The lora settings
are configured in the Settings.h file.

The receiver recognises two types of tracker packet, the one from the matching program '23_GPS_Tracker_Transmitter'
(LocationPacket, 27 bytes) which causes these fields to be printed to the serial monitor;
  
Latitude, Longitude, Altitude, Satellites, HDOP, TrackerStatusByte, GPS Fixtime, Battery mV, Distance, Direction,
Distance, Direction, PacketRSSI, PacketSNR, NumberPackets, PacketLength, IRQRegister.

This is a long packet which at the long range LoRa settings takes just over 3 seconds to transmit. 

The receiver also recognises a much shorter location only packet (LocationBinaryPacket, 11 bytes) and when
received this is printed to the serial monitor;

Latitude, Longitude, Altitude, TrackerStatusByte, Distance, Direction, PacketRSSI, PacketSNR, NumberPackets,
PacketLength, IRQRegister.

Most of the tracker information (for both types of packet) is shown on the OLED display. If there has been a
tracker transmitter GPS fix the number\identifier of that tracker is shown on row 0 right of screen and if there
is a recent local (receiver) GPS fix an 'R' is displayed  row 1 right of screen.

When the tracker transmitter starts up or is reset its sends a power up message containing the battery voltage
which is shown on the OLED and printer to the serial monitor.

The program has the option of using a pin to control the power to the GPS, if the GPS module being used has this
feature. To use the option change the define in Settings.h; 

'#define GPSPOWER -1' from -1 to the pin number being used. Also set the GPSONSTATE and GPSOFFSTATE defines to
the appropriate logic levels.

The program by default uses software serial to read the GPS, you can use hardware serial by commenting out this
line in the Settings.h file;

#define USE_SOFTSERIAL_GPS

And then defining the hardware serial port you are using, which defaults to Serial1.

Serial monitor baud rate is set at 115200.
*******************************************************************************************************/


#define Program_Version "V1.2"

#include <SPI.h>
#include <SX127XLT.h>
SX127XLT LT;

#include "Settings.h"
#include <ProgramLT_Definitions.h>

#include <U8x8lib.h>                                        //https://github.com/olikraus/u8g2 
U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);      //standard 0.96" SSD1306
//U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);     //1.3" OLED often sold as 1.3" SSD1306


#include <TinyGPS++.h>                                      //http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;                                            //create the TinyGPS++ object

#ifdef USESOFTSERIALGPS
#include <SoftwareSerial.h>
SoftwareSerial GPSserial(RXpin, TXpin);
#endif

#ifdef USEHARDWARESERIALGPS
#define GPSserial HARDWARESERIALPORT
#endif


uint32_t RXpacketCount;        //count of received packets
uint8_t RXPacketL;             //length of received packet
int8_t  PacketRSSI;            //signal strength (RSSI) dBm of received packet
int8_t  PacketSNR;             //signal to noise ratio (SNR) dB of received packet
uint8_t PacketType;            //for packet addressing, identifies packet type
uint8_t Destination;           //for packet addressing, identifies the destination (receiving) node
uint8_t Source;                //for packet addressing, identifies the source (transmiting) node
uint8_t TXStatus;              //status byte from tracker transmitter
uint8_t TXSats;                //number of sattelites in use
float TXLat;                   //latitude
float TXLon;                   //longitude
float TXAlt;                   //altitude
float RXLat;                   //latitude
float RXLon;                   //longitude
float RXAlt;                   //altitude
uint32_t TXHdop;               //HDOP, indication of fix quality, horizontal dilution of precision, low is good
uint32_t TXGPSFixTime;         //time in mS for fix
uint16_t TXVolts;              //supply\battery voltage
uint16_t RXVolts;              //supply\battery voltage
float TXdistance;              //calculated distance to tracker
uint16_t TXdirection;          //calculated direction to tracker
uint16_t RXerrors;
uint32_t TXupTimemS;           //up time of TX in mS 

uint32_t LastRXGPSfixCheck;    //used to record the time of the last GPS fix

bool TXLocation = false;       //set to true when at least one tracker location packet has been received
bool RXGPSfix = false;         //set to true if the local GPS has a recent fix

uint8_t FixCount = DisplayRate;  //used to keep track of number of GPS fixes before display updated


void loop()
{
  RXPacketL = LT.receiveSXBuffer(0, 0, NO_WAIT);   //returns 0 if packet error of some sort

  while (!digitalRead(DIO0))
  {
    readGPS();                                     //If the DIO pin is low, no packet arrived, so read the GPS
  }

  //something has happened in receiver
  digitalWrite(LED1, HIGH);

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, HIGH);
  }

  RXPacketL = LT.readRXPacketL();
  PacketRSSI = LT.readPacketRSSI();
  PacketSNR = LT.readPacketSNR();


  if (RXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_is_OK();
  }

  digitalWrite(LED1, LOW);

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);
  }
  Serial.println();
}


void readGPS()
{
  if (GPSserial.available() > 0)
  {
    gps.encode(GPSserial.read());
  }

  if ( (uint32_t) (millis() - LastRXGPSfixCheck) > NoRXGPSfixms)
  {
    RXGPSfix = false;
    LastRXGPSfixCheck = millis();
    dispscreen1();
  }

  if (gps.location.isUpdated() && gps.altitude.isUpdated())
  {
    RXGPSfix = true;
    RXLat = gps.location.lat();
    RXLon = gps.location.lng();
    RXAlt = gps.altitude.meters();
    printRXLocation();
    LastRXGPSfixCheck = millis();

    if ( FixCount == 1)                           //update screen when FIX count counts down from DisplayRate to 1
    {
      FixCount = DisplayRate;
      dispscreen1();
    }
    FixCount--;
  }
}


bool readTXStatus(byte bitnum)
{
  return bitRead(TXStatus, bitnum);
}


void printRXLocation()
{
  Serial.print(F("LocalGPS "));
  Serial.print(RXLat, 5);
  Serial.print(F(","));
  Serial.print(RXLon, 5);
  Serial.print(F(","));
  Serial.print(RXAlt, 1);
  Serial.println();
}


void readPacketAddressing()
{
  LT.startReadSXBuffer(0);
  PacketType = LT.readUint8();
  Destination = LT.readUint8();
  Source = LT.readUint8();
  LT.endReadSXBuffer();
}


void packet_is_OK()
{
  //uint16_t IRQStatus;
  float tempfloat;

  RXpacketCount++;

  readPacketAddressing();

  if (PacketType == PowerUp)
  {
    LT.startReadSXBuffer(0);
    LT.readUint8();                                      //read byte from SXBuffer, not used
    LT.readUint8();                                      //read byte from SXBuffer, not used
    LT.readUint8();                                      //read byte from SXBuffer, not used
    TXVolts = LT.readUint16();                           //read tracker transmitter voltage
    LT.endReadSXBuffer();
    Serial.print(F("Tracker Powerup - Battery "));
    Serial.print(TXVolts);
    Serial.println(F("mV"));
    dispscreen2();
  }

  if (PacketType == LocationPacket)
  {
    //packet has been received, now read from the SX12XX FIFO in the correct order.
    Serial.print(F("LocationPacket "));
    TXLocation = true;
    LT.startReadSXBuffer(0);                //start the read of received packet
    PacketType = LT.readUint8();            //read in the PacketType
    Destination = LT.readUint8();           //read in the Packet destination address
    Source = LT.readUint8();                //read in the Packet source address
    TXLat = LT.readFloat();                 //read in the tracker latitude
    TXLon = LT.readFloat();                 //read in the tracker longitude
    TXAlt = LT.readFloat();                 //read in the tracker altitude
    TXSats = LT.readUint8();                //read in the satellites in use by tracker GPS
    TXHdop = LT.readUint32();               //read in the HDOP of tracker GPS
    TXStatus = LT.readUint8();              //read in the tracker status byte
    TXGPSFixTime = LT.readUint32();         //read in the last fix time of tracker GPS
    TXVolts = LT.readUint16();              //read in the tracker supply\battery volts
    TXupTimemS = LT.readUint32();           //read in the TX uptime in mS
    RXPacketL = LT.endReadSXBuffer();       //end the read of received packet


    if (RXGPSfix)                           //if there has been a local GPS fix do the distance and direction calculation
    {
      TXdirection = (int16_t) TinyGPSPlus::courseTo(RXLat, RXLon, TXLat, TXLon);
      TXdistance = TinyGPSPlus::distanceBetween(RXLat, RXLon, TXLat, TXLon);
    }
    else
    {
      TXdistance = 0;
      TXdirection = 0;
    }

    Serial.write(PacketType);
    Serial.write(Destination);
    Serial.write(Source);
    Serial.print(F(","));
    Serial.print(TXLat, 6);
    Serial.print(F(","));
    Serial.print(TXLon, 6);
    Serial.print(F(","));
    Serial.print(TXAlt, 1);
    Serial.print(F(","));
    Serial.print(TXSats);
    Serial.print(F(","));

    tempfloat = ( (float) TXHdop / 100);           //need to convert Hdop read from GPS as uint32_t to a float for display
    Serial.print(tempfloat, 2);

    Serial.print(F(","));
    Serial.print(TXStatus);
    Serial.print(F(","));

    Serial.print(TXGPSFixTime);
    Serial.print(F("mS,"));
    Serial.print(TXVolts);
    Serial.print(F("mV,"));
    Serial.print((TXupTimemS/1000));
    Serial.print(F("s,"));

    Serial.print(TXdistance, 0);
    Serial.print(F("m,"));
    Serial.print(TXdirection);
    Serial.print(F("d"));
    printpacketDetails();
    dispscreen1();                                  //and show the packet detail it on screen
    return;
  }


  if (PacketType == LocationBinaryPacket)
  {
    //packet from locator has been received, now read from the SX12XX FIFO in the correct order.
    TXLocation = true;
    Serial.print(F("LocationBinaryPacket "));
    LT.startReadSXBuffer(0);
    PacketType = LT.readUint8();
    Destination = LT.readUint8();
    Source = LT.readUint8();
    TXLat = LT.readFloat();
    TXLon = LT.readFloat();
    TXAlt = LT.readInt16();
    TXStatus = LT.readUint8();
    RXPacketL = LT.endReadSXBuffer();

    if (RXGPSfix)                           //if there has been a local GPS fix do the distance and direction calculation
    {
      TXdirection = (int16_t) TinyGPSPlus::courseTo(RXLat, RXLon, TXLat, TXLon);
      TXdistance = TinyGPSPlus::distanceBetween(RXLat, RXLon, TXLat, TXLon);
    }
    else
    {
      TXdistance = 0;
      TXdirection = 0;
    }

    Serial.write(PacketType);
    Serial.write(Destination);
    Serial.write(Source);
    Serial.print(F(","));
    Serial.print(TXLat, 6);
    Serial.print(F(","));
    Serial.print(TXLon, 6);
    Serial.print(F(","));
    Serial.print(TXAlt, 0);
    Serial.print(F("m,"));
    Serial.print(TXStatus);
    Serial.print(F(","));
    Serial.print(TXdistance, 0);
    Serial.print(F("m,"));
    Serial.print(TXdirection);
    Serial.print(F("d"));
    printpacketDetails();
    dispscreen1();
    return;
  }

  if (PacketType == NoFix)
  {
    Serial.print(F("No tracker GPS fix"));
  }
}


void printpacketDetails()
{
  uint16_t IRQStatus;
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Packets,"));
  Serial.print(RXpacketCount);

  Serial.print(F(",Length,"));
  Serial.print(RXPacketL);
  IRQStatus = LT.readIrqStatus();
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  if (BUZZER >= 0)
  {
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
  }

  IRQStatus = LT.readIrqStatus();                    //get the IRQ status
  RXerrors++;
  Serial.print(F("PacketError,RSSI"));

  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);

  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL());                  //get the real packet length
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
  LT.printIrqStatus();
  digitalWrite(LED1, LOW);

  if (BUZZER >= 0)
  {
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
  }
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


void dispscreen1()
{
  //show received packet data on display
  float tempfloat;
  disp.clearLine(0);
  disp.setCursor(0, 0);
  disp.print(TXLat, 6);

  disp.setCursor(14, 0);
  
  if (readTXStatus(GPSFix))
  {
   disp.print(F("T"));
   disp.write(Source);
  }
  else
  {
   disp.print(F("T?"));  
  }
  
  disp.clearLine(1);
  disp.setCursor(0, 1);
  disp.print(TXLon, 6);

  disp.setCursor(14, 1);
  if (RXGPSfix)
  {
   disp.print(F("RG"));
  }
  else
  {
   disp.print(F("R?"));  
  }
  
  disp.clearLine(2);
  disp.setCursor(0, 2);
  disp.print(TXAlt,0);
  disp.print(F("m"));
  disp.clearLine(3);
  disp.setCursor(0, 3);

  disp.print(F("RSSI "));
  disp.print(PacketRSSI);
  disp.print(F("dBm"));
  disp.clearLine(4);
  disp.setCursor(0, 4);
  disp.print(F("SNR  "));

  if (PacketSNR > 0)
  {
    disp.print(F("+"));
  }

  if (PacketSNR == 0)
  {
    disp.print(F(" "));
  }

  if (PacketSNR < 0)
  {
    disp.print(F("-"));
  }

  disp.print(PacketSNR);
  disp.print(F("dB"));

  if (PacketType == LocationPacket)
  {
    disp.clearLine(5);
    disp.setCursor(0, 5);
    tempfloat = ((float) TXVolts / 1000);
    disp.print(F("Batt "));
    disp.print(tempfloat, 2);
    disp.print(F("v"));
  }

  disp.clearLine(6);
  disp.setCursor(0, 6);
  disp.print(F("Packets "));
  disp.print(RXpacketCount);

  disp.clearLine(7);

  if (RXGPSfix && TXLocation)           //only display distance and direction if have received tracker packet and have local GPS fix
  {
    disp.clearLine(7);
    disp.setCursor(0, 7);
    disp.print(TXdistance, 0);
    disp.print(F("m "));
    disp.print(TXdirection);
    disp.print(F("d"));
  }
}


void dispscreen2()
{
  //show tracker powerup data on display
  float tempfloat;
  disp.clear();
  disp.setCursor(0, 0);
  disp.print(F("Tracker Powerup"));
  disp.setCursor(0, 1);
  disp.print(F("Battery "));
  tempfloat = ((float) TXVolts / 1000);
  disp.print(tempfloat, 2);
  disp.print(F("v"));
}


void GPSTest()
{
  uint32_t startmS;
  startmS = millis();

  while ( (uint32_t) (millis() - startmS) < 2000)       //allows for millis() overflow
  {
    if (GPSserial.available() > 0)
    {
     Serial.write(GPSserial.read());
    }
  }
  Serial.println();
  Serial.println();
  Serial.flush();
}


void setup()
{
  pinMode(LED1, OUTPUT);                        //setup pin as output for indicator LED
  led_Flash(2, 125);                            //two quick LED flashes to indicate program start

  Serial.begin(115200);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();

  Serial.println(F("25_GPS_Tracker_Receiver_With_Display_and_GPS Starting"));

  if (BUZZER >= 0)
  {
    pinMode(BUZZER, OUTPUT);
  }

  SPI.begin();

  disp.begin();
  disp.setFont(u8x8_font_chroma48medium8_r);

  Serial.print(F("Checking LoRa device - "));         //Initialize LoRa
  disp.setCursor(0, 0);

  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("Receiver ready"));
    disp.print(F("Receiver ready"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("No LoRa device responding"));
    disp.print(F("No LoRa device"));
    while (1)
    {
      led_Flash(50, 50);                                            //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  Serial.println(F("Startup GPS check"));
  
  if (GPSPOWER >= 0)
  {
    pinMode(GPSPOWER, OUTPUT);
    digitalWrite(GPSPOWER, GPSONSTATE);                         //power up GPS
  }

  GPSserial.begin(GPSBaud);
  GPSTest();
  
  Serial.println();
  Serial.println();

  Serial.println(F("Receiver ready"));
  Serial.println();
}
