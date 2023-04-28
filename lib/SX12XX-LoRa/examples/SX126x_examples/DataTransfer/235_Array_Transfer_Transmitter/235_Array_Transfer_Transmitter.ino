/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 10/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This is a simulation test program for the use of a data transfer (DT) packet to send
  the contents of a memory array (DTsendarray) on one Arduino to the SD card as a file on another Arduino,
  Arduino DUEs were used for the test. For this example the memory array is first loaded from an SD file.

  DT packets can be used for transfering large amounts of data in a sequence of packets or segments,
  in a reliable and resiliant way. The remote file open request, the segements sent and the remote file close
  will be transmitted until a valid acknowledge comes from the receiver. Use with the matching receiver
  program, 234_SDfile_Transfer_Receiver.ino or 236_SDfile_Transfer_ReceiverIRQ.ino.

  Each DT packet contains a variable length header array and a variable length data array as the payload.
  On transmission the NetworkID and CRC of the payload are appended to the end of the packet by the library
  routines. The use of a NetworkID and CRC ensures that the receiver can validate the packet to a high degree
  of certainty. The receiver will not accept packets that dont have the appropriate NetworkID or payload CRC
  at the end of the packet.

  Details of the packet identifiers, header and data lengths and formats used are in the file
  Data_transfer_packet_definitions.md in the \SX126X_examples\DataTransfer\ folder.

  Serial monitor baud rate is set at 115200.
*******************************************************************************************************/

#include <SPI.h>

#include <SX126XLT.h>
#include <ProgramLT_Definitions.h>
#include "Settings.h"                            //LoRa settings etc.
#include <arrayRW.h>
#include "Variables.h"

SX126XLT LoRa;                                   //create an SX126XLT library instance called LoRa

#include <SdFat.h>
SdFat SD;
File dataFile;                                   //name the file instance needed for SD library routines

uint8_t DTheader[16];                            //header array
uint8_t DTdata[245];                             //data/segment array
uint8_t DTsendarray[0x10000];                    //create a global array to hold data to transfer
uint8_t *ptrDTsendarray;                         //create a global pointer to the array to send, so all functions have access

//choice of files to send
char DTfilenamebuff[] = "/$50SATS.JPG";      //file length 6880 bytes, file CRC 0x0281
//char DTfilenamebuff[] = "/$50SATT.JPG";    //file length 1068 bytes, file CRC 0x6A02


//#define DEBUG                              //enable define to see more detail for data transfer operation
//#define DEBUGSDLIB                         //enable define to see more detail for SD operation
//#define DISABLEPAYLOADCRC                  //enable this define if you want to disable payload CRC checking


void loop()
{

  DTLocalArrayLength = moveFileArray(DTfilenamebuff, DTsendarray, sizeof(DTsendarray));         //move the file to a global array to be sent

  if (DTLocalArrayLength == 0)
  {
    Serial.println("ERROR moving local file to array - program halted");
    while (1);
  }

  doArrayTransfer(DTsendarray);
  Serial.println("Array transfer complete - program halted");
  while (1);
}


void printSeconds()
{
  float secs;
  secs = ( (float) millis() / 1000);
  Serial.print(secs, 3);
  Serial.print(F(" "));
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


//**********************************
// Start Code for transfer of array
//**********************************

bool doArrayTransfer(uint8_t *ptrarray)
{
  ptrDTsendarray = ptrarray;                                                 //set the global ptr for the  the array to send
  DTLocalArrayCRC = LoRa.CRCCCITT(ptrDTsendarray, DTLocalArrayLength, 0xFFFF);

  DTStartmS = millis();
  do
  {
    if (startArrayTransfer(DTfilenamebuff, sizeof(DTfilenamebuff)))          //sends the DTsendarray to a remote filename
    {
      Serial.print(DTfilenamebuff);
      Serial.println(" opened OK on remote");
      DTNumberSegments = getNumberSegments(DTLocalArrayLength, DTSegmentSize);
      DTLastSegmentSize = getLastSegmentSize(DTLocalArrayLength, DTSegmentSize);
      printLocalFileDetails();
      Serial.println();
      NoAckCount = 0;
    }
    else
    {
      Serial.print(DTfilenamebuff);
      Serial.println("Error opening remote file - restart transfer");
      DTFileTransferComplete = false;
      continue;
    }

    delay(packetdelaymS);

    if (!sendSegments())
    {
      Serial.println();
      Serial.println(F("**********************************************************"));
      Serial.println(F("Error - Segment write with no file open - Restart received"));
      Serial.println(F("**********************************************************"));
      Serial.println();
      continue;
    }

    if (endArrayTransfer(DTfilenamebuff, sizeof(DTfilenamebuff)))        //send command to close remote file
    {
      //the header returned from file close contains a 16bit CRC of the file saved on the remotes SD
      Serial.print(DTfilenamebuff);
      Serial.println(" closed OK on remote");
      beginarrayRW(DTheader, 4);
      DTRemoteFileLength = arrayReadUint32();
      DTRemoteFileCRC = arrayReadUint16();
      Serial.print(F("Acknowledged remote file length "));
      Serial.println(DTRemoteFileLength);
      Serial.print(F("Acknowledged remote file CRC 0x"));
      Serial.println(DTRemoteFileCRC, HEX);
    }
    else
    {
      DTFileTransferComplete = false;
      Serial.println(F("ERROR send close remote file failed - program halted"));
    }

    Serial.print(F("NoAckCount "));
    Serial.println( NoAckCount);
    Serial.print(F("Total file transmit time "));
    Serial.print(millis() -  DTStartmS);
    Serial.println(F("mS"));
    DTFileTransferComplete = true;
  }
  while (!DTFileTransferComplete);

  return true;
}


uint16_t getNumberSegments(uint32_t arraysize, uint8_t segmentsize)
{
  uint16_t segments;
  segments = arraysize / segmentsize;
  if ((arraysize % segmentsize) > 0)
  {
    segments++;
  }
  return segments;
}


uint8_t getLastSegmentSize(uint32_t arraysize, uint8_t segmentsize)
{
  uint8_t lastsize;
  lastsize = arraysize % segmentsize;
  if (lastsize == 0)
  {
    lastsize = segmentsize;
  }
  return lastsize;
}


bool startArrayTransfer(char *buff, uint8_t filenamesize)
{
  uint8_t ValidACK;

  Serial.print(F("Start Array transfer "));
  Serial.print(DTLocalArrayLength);
  Serial.println(F(" bytes"));

  build_DTFileOpenHeader(DTheader, DTFileOpenHeaderL, filenamesize, DTLocalArrayLength, DTLocalArrayCRC, DTSegmentSize);
  LocalPayloadCRC = LoRa.CRCCCITT((uint8_t *) buff, filenamesize, 0xFFFF);

  do
  {
    Serial.println(F("Transmit open remote file request"));
    digitalWrite(LED1, HIGH);
    TXPacketL = LoRa.transmitDT(DTheader, DTFileOpenHeaderL, (uint8_t *) buff, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    digitalWrite(LED1, LOW);
    TXNetworkID = LoRa.getTXNetworkID(TXPacketL);          //get the networkID appended to packet
    TXArrayCRC = LoRa.getTXPayloadCRC(TXPacketL);          //get the payload CRC appended to packet

#ifdef DEBUG
    Serial.print(F("TXNetworkID,0x"));
    Serial.print(TXNetworkID, HEX);               //get the NetworkID of the packet just sent, its placed at the packet end
    Serial.print(F(",TXarrayCRC,0x"));
    Serial.println(TXArrayCRC, HEX);              //get the CRC of the data array just sent, its placed at the packet end
    Serial.println();
#endif

    if (TXPacketL == 0)                           //if there has been a send and ack error, TXPacketL returns as 0
    {
      Serial.println(F("Transmit error"));
    }

    Serial.print(F("Wait ACK "));
    ValidACK = LoRa.waitACKDT(DTheader, DTFileOpenHeaderL, ACKtimeoutDTmS);
    RXPacketType = DTheader[0];

    if ((ValidACK > 0) && (RXPacketType == DTFileOpenACK))
    {
      Serial.println(F(" - Valid ACK received"));
#ifdef DEBUG
      printPacketHex();
#endif
    }
    else
    {
      NoAckCount++;
      Serial.println(F("No Valid ACK received"));
#ifdef DEBUG
      printACKdetail();
      Serial.print(F("ACKPacket "));
      printPacketHex();
#endif
      Serial.println();
    }
    Serial.println();
  }
  while (ValidACK == 0);

  return true;
}


void build_DTFileOpenHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize)
{
  //this builds the header buffer for the filename passed

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTFileOpen);         //byte 0, write the packet type
  arrayWriteUint8(0);                  //byte 1, initial DTflags byte, not used here
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(filelength);        //byte 4,5,6,7, write the file length
  arrayWriteUint16(filecrc);           //byte 8, 9, write dataarray (filename) CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


bool sendSegments()
{
  //start the array transfer at segment 0
  DTSegment = 0;
  DTSentSegments = 0;

  DTarraylocation = 0;                      //ensure at first position in array

  while (DTSegment < (DTNumberSegments - 1))
  {
    printSeconds();

    if (sendArraySegment(DTSegment, DTSegmentSize))
    {
      Serial.println();
      //DTSegment++;
      DTSentSegments++;
    }
    else
    {
      Serial.println(F("ERROR in sendArraySegment"));
      Serial.println();
      return false;
    }

    delay(packetdelaymS);
  };

  Serial.println("Last segment");

  if (!sendArraySegment(DTSegment, DTLastSegmentSize))
  {
    Serial.println(F("ERROR in sendArraySegment"));
    return false;
  }

  return true;
}


bool sendArraySegment(uint16_t segnum, uint8_t segmentsize)
{
  uint8_t ValidACK;
  uint8_t index;
  uint8_t tempdata;

  for (index = 0; index < segmentsize; index++)
  {
    tempdata = ptrDTsendarray[DTarraylocation];
    DTdata[index] = tempdata;
    DTarraylocation++;
  }

  build_DTSegmentHeader(DTheader, DTSegmentWriteHeaderL, segmentsize, segnum);

  Serial.print(F("Send Segment,"));
  Serial.print(segnum);
  Serial.print(F(" "));
  printheader(DTheader, DTSegmentWriteHeaderL);
  Serial.println();

  do
  {
    digitalWrite(LED1, HIGH);
    TXPacketL = LoRa.transmitDT(DTheader, DTSegmentWriteHeaderL, (uint8_t *) DTdata, segmentsize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    digitalWrite(LED1, LOW);

    if (TXPacketL == 0)                                        //if there has been a send TXPacketL returns as 0
    {
      Serial.println(F("Transmit error"));
    }

    ValidACK = LoRa.waitACKDT(DTheader, DTSegmentWriteHeaderL, ACKtimeoutDTmS);
    RXPacketType = DTheader[0];

    if (ValidACK > 0)
    {
      if (RXPacketType == DTSegmentWriteNACK)
      {
        DTSegment = DTheader[4] +  (DTheader[5] << 8);                 //load what the segment should be
        RXHeaderL = DTheader[2];
        Serial.println();
        Serial.println(F("************************************"));
        Serial.print(F("Received restart request at segment "));
        Serial.println(DTSegment);
        printheader(DTheader, RXHeaderL);
        Serial.println();
        Serial.print(F("Seek to file location "));
        Serial.println(DTSegment * DTSegmentSize);
        Serial.println(F("************************************"));
        Serial.println();
        Serial.flush();
        DTarraylocation  = (DTSegment * DTSegmentSize);
      }

      //ack is valid, segment was acknowledged if here

      if (RXPacketType == DTStartNACK)
      {
        Serial.println(F("Received DTStartNACK "));
        return false;
      }

      if (RXPacketType == DTSegmentWriteACK)
      {
        readACKHeader();
        AckCount++;
        printPacketDetails();
        DTSegment++;                  //increase value for next segment
        return true;
      }
    }
    else
    {
      NoAckCount++;
      Serial.print(F("Error No Ack "));
      Serial.print(F("NoAckCount,"));
      Serial.print(NoAckCount);
      LoRa.printReliableStatus();
      Serial.println();
    }
  } while (ValidACK == 0);

  return true;
}


void build_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum)
{
  //this builds the header buffer for the a segment transmit
  beginarrayRW(header, 0);                //start writing to array at location 0
  arrayWriteUint8(DTSegmentWrite);        //write the packet type
  arrayWriteUint8(0);                     //initial DTflags byte, not used here
  arrayWriteUint8(headersize);            //write length of header
  arrayWriteUint8(datalen);               //write length of data array
  arrayWriteUint16(segnum);               //write the DTsegment number
  endarrayRW();
}


void printPacketDetails()
{
  PacketRSSI = LoRa.readPacketRSSI();
  PacketSNR = LoRa.readPacketSNR();
  Serial.print(F("AckCount,"));
  Serial.print(AckCount);
  Serial.print(F(",NoAckCount,"));
  Serial.print(NoAckCount);
  Serial.print(F(",AckRSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,AckSNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB"));
  Serial.println();
}


void printLocalFileDetails()
{
  Serial.print(DTfilenamebuff);
  Serial.print(F(" LocalFilelength is "));
  Serial.print(DTLocalFileLength);
  Serial.println(F(" bytes"));
  Serial.print(DTfilenamebuff);
  Serial.print(F(" Array to send CRC is 0x"));
  Serial.println(DTLocalArrayCRC, HEX);
  Serial.print(F("DTSegmentSize "));
  Serial.println(DTSegmentSize);
  Serial.print(F("Number Segments "));
  Serial.println(DTNumberSegments);
  Serial.print(F("DTLastSegmentSize "));
  Serial.println(DTLastSegmentSize);
}


void printPacketHex()
{
  RXPacketL = LoRa.readRXPacketL();
  Serial.print(RXPacketL);
  Serial.print(F(" bytes > "));
  if (RXPacketL > 0)
  {
    LoRa.printSXBufferHEX(0, RXPacketL - 1);
  }
}


void printACKdetail()
{
  Serial.print(F("ACKDetail"));
  Serial.print(F(",RXNetworkID,0x"));
  Serial.print(LoRa.getRXNetworkID(RXPacketL), HEX);
  Serial.print(F(",RXPayloadCRC,0x"));
  Serial.print(LoRa.getRXPayloadCRC(RXPacketL), HEX);
  Serial.print(F(",RXPacketL,"));
  Serial.print(RXPacketL);
  Serial.print(F(" "));
  LoRa.printReliableStatus();
  Serial.println();
}


void printdata(uint8_t *dataarray, uint8_t arraysize)
{
  Serial.print(F("DataBytes,"));
  Serial.print(arraysize);
  Serial.print(F("  "));
  printarrayHEX((uint8_t *) dataarray, arraysize);
}


void printheader(uint8_t *hdr, uint8_t hdrsize)
{
  Serial.print(F("HeaderBytes,"));
  Serial.print(hdrsize);
  Serial.print(F(" "));
  printarrayHEX(hdr, hdrsize);
}


void readACKHeader()
{
  //the first 6 bytes of the segment write header contain the important stuff, so load it up
  //so we can decide what to do next.
  beginarrayRW(DTheader, 0);                      //start buffer read at location 0
  RXPacketType = arrayReadUint8();                //load the packet type
  RXFlags = arrayReadUint8();                     //initial DTflags byte, not used here
  RXHeaderL = arrayReadUint8();                   //load the header length
  RXDataarrayL = arrayReadUint8();                //load the datalength
  DTSegment = arrayReadUint16();                  //load the segment number
}


bool endArrayTransfer(char *buff, uint8_t filenamesize)
{
  uint8_t ValidACK;

  Serial.print(F("End file transfer "));
  Serial.println(buff);
  DTSD_closeFile();
  build_DTFileCloseHeader(DTheader, DTFileCloseHeaderL, filenamesize, DTLocalArrayLength, DTLocalArrayCRC, DTSegmentSize);

  do
  {
    Serial.println(F("Transmit close remote file request"));
    digitalWrite(LED1, HIGH);
    TXPacketL = LoRa.transmitDT(DTheader, DTFileCloseHeaderL, (uint8_t *) buff, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    digitalWrite(LED1, LOW);
    TXNetworkID = LoRa.getTXNetworkID(TXPacketL);
    TXArrayCRC = LoRa.getTXPayloadCRC(TXPacketL);

#ifdef DEBUG
    Serial.print(F("TXNetworkID,0x"));
    Serial.print(TXNetworkID, HEX);               //get the NetworkID of the packet just sent, its placed at the packet end
    Serial.print(F(",TXarrayCRC,0x"));
    Serial.println(TXArrayCRC, HEX);              //get the CRC of the data array just sent, its placed at the packet end
    Serial.println();
#endif

    if (TXPacketL == 0)                                //if there has been a send and ack error, RXPacketL returns as 0
    {
      Serial.println(F("Transmit error"));
    }

    Serial.print(F("Wait ACK "));
    ValidACK = LoRa.waitACKDT(DTheader, DTFileCloseHeaderL, ACKtimeoutDTmS);
    RXPacketType = DTheader[0];

    if ((ValidACK > 0) && (RXPacketType == DTFileCloseACK))
    {
      Serial.println(F(" - Valid ACK received"));
#ifdef DEBUG
      printPacketHex();
#endif
    }
    else
    {
      NoAckCount++;
      Serial.println(F("No Valid ACK received"));
      printACKdetail();
#ifdef DEBUG
      Serial.print(F("ACKPacket "));
      printPacketHex();
#endif
      Serial.println();
    }
    Serial.println();
  }
  while (ValidACK == 0);

  return true;
}


void build_DTFileCloseHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize)
{
  //this builds the header buffer for the filename passed

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTFileClose);         //byte 0, write the packet type
  arrayWriteUint8(0);                  //byte 1, initial DTflags byte, not used here
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(filelength);        //byte 4,5,6,7, write the file length
  arrayWriteUint16(filecrc);           //byte 8, 9, write dataarray (filename) CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


//*********************************
// Start Code for dealing with SD
//*********************************

uint32_t moveFileArray(char *filenamebuff, uint8_t *buff, uint32_t buffsize)
{
  uint32_t index;

  ptrDTsendarray = buff;                                         //assign passed array ptr to global ptr
  DTLocalFileLength = DTSD_getFileSize(filenamebuff);            //get the file length

  if (DTLocalFileLength == 0)
  {
    Serial.print(F("Error - opening local file "));
    Serial.println(filenamebuff);
    return 0;
  }

  if (DTLocalFileLength > buffsize)
  {
    Serial.println(filenamebuff);
    Serial.print(F("Error - file length of "));
    Serial.print(DTLocalFileLength);
    Serial.print(F(" bytes exceeds array length of "));
    Serial.print(buffsize);
    Serial.println(F(" bytes"));
    return 0;
  }

  DTSD_openFileRead(filenamebuff);
  Serial.print(F("Opened local file "));
  Serial.print(filenamebuff);
  Serial.print(F(" "));
  Serial.print(DTLocalFileLength);
  Serial.println(F(" bytes"));
  DTLocalFileCRC = DTSD_fileCRCCCITT(DTLocalFileLength);                   //get file CRC from position 0 to end
  Serial.print(F("DTLocalFileCRC 0x"));
  Serial.println(DTLocalFileCRC,HEX);

  //now tranfer SD file to global array
  dataFile.seek(0);                                                        //ensure at first position in file
  for (index = 0; index < DTLocalFileLength; index++)
  {
    buff[index] = dataFile.read();
  }

  Serial.println(F("DTsendarray loaded from SD"));
  Serial.print(F("Last written location "));
  Serial.println(index);
  Serial.print(F("First 16 bytes of array to send "));
  LoRa.printHEXPacket(buff, 16);
  Serial.println();
  DTarraylocation = 0;
  return DTLocalFileLength;
}


bool DTSD_initSD(uint8_t CSpin)
{
  if (SD.begin(CSpin))
  {
    return true;
  }
  else
  {
    return false;
  }
}


void DTSD_printDirectory()
{
  dataFile = SD.open("/");
  Serial.println("Card directory");
  SD.ls("/", LS_R);
}


uint32_t DTSD_openFileRead(char *buff)
{
  uint32_t filesize;
  dataFile = SD.open(buff);
  filesize = dataFile.size();
  dataFile.seek(0);
  return filesize;
}


void DTSD_closeFile()
{
  dataFile.close();                                      //close local file
}


uint16_t DTSD_fileCRCCCITT(uint32_t fsize)
{
  //does a CRC calculation on the file open via dataFile
  uint32_t index;
  uint16_t CRCcalc;
  uint8_t j, filedata;

  CRCcalc = 0xFFFF;                                //start value for CRC16
  dataFile.seek(0);                                //be sure at start of file position

  for (index = 0; index < fsize; index++)
  {
    filedata = dataFile.read();
#ifdef DEBUGSDLIB
    Serial.print(F(" 0x"));
    Serial.print(filedata, HEX);
#endif
    CRCcalc ^= (((uint16_t) filedata ) << 8);
    for (j = 0; j < 8; j++)
    {
      if (CRCcalc & 0x8000)
        CRCcalc = (CRCcalc << 1) ^ 0x1021;
      else
        CRCcalc <<= 1;
    }
  }

#ifdef DEBUGSDLIB
  Serial.print(F(" {DEBUGSDLIB} "));
  Serial.print(index);
  Serial.print(F(" Bytes checked - CRC "));
  Serial.println(CRCcalc, HEX);
#endif

  return CRCcalc;
}


uint16_t DTSD_getNumberSegments(uint32_t filesize, uint8_t segmentsize)
{
  uint16_t segments;
  segments = filesize / segmentsize;
  if ((filesize % segmentsize) > 0)
  {
    segments++;
  }
  return segments;
}


uint8_t DTSD_getLastSegmentSize(uint32_t filesize, uint8_t segmentsize)
{
  uint8_t lastsize;
  lastsize = filesize % segmentsize;
  if (lastsize == 0)
  {
    lastsize = segmentsize;
  }
  return lastsize;
}


void DTSD_seekFileLocation(uint32_t position)
{
  dataFile.seek(position);                       //seek to position in file
  return;
}


uint32_t DTSD_getFileSize(char *buff)
{
  uint32_t filesize;

  if (!SD.exists(buff))
  {
    return 0;
  }

  dataFile = SD.open(buff);
  filesize = dataFile.size();
  dataFile.close();
  return filesize;
}

//*******************************
// End Code for dealing with SD
//*******************************


void setup()
{
  pinMode(LED1, OUTPUT);                          //setup pin as output for indicator LED
  led_Flash(2, 125);                              //two quick LED flashes to indicate program start

  Serial.begin(115200);

  while (!Serial);                                // wait for serial port to connect. Needed for native USB

  Serial.println();
  Serial.println(F("235_Array_Transfer_Transmitter starting"));
  Serial.flush();

  SPI.begin();

  if (LoRa.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
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

  LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.print(F("Initializing SD card..."));

  if (DTSD_initSD(SDCS))
  {
    Serial.println(F("Card initialized."));
  }
  else
  {
    Serial.println(F("Card failed, or not present."));
    while (1) led_Flash(100, 25);
  }

  Serial.println();
  DTSD_printDirectory();
  Serial.println();
  Serial.println();
  Serial.println(F("Array transfer ready"));
  Serial.println();

#ifdef DISABLEPAYLOADCRC
  LoRa.setReliableConfig(NoReliableCRC);
#endif

  if (LoRa.getReliableConfig(NoReliableCRC))
  {
    Serial.println(F("Payload CRC disabled"));
  }
  else
  {
    Serial.println(F("Payload CRC enabled"));
  }

  DTFileTransferComplete = false;

}
