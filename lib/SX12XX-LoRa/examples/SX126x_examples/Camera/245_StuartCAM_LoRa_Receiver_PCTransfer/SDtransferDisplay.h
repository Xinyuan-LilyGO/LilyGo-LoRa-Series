/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 20/03/22

  The functions expect the calling sketch to create an instance called LoRa, so that functions
  are called like this; LoRa.getSDTXNetworkID().

  This code is supplied as is, it is up to the user of the program to decide if the program is suitable
  for the intended purpose and free from errors.

  There is a copy of this file in the SX12XX-LoRa library \src folder, but the file can be copied to the
  sketch folder and used locally. In this way its possible to carry out custom modifications.

  Extensive use is made of #defines to allow the monotoring and debug prints to serial monitor to be
  turned off, this is to allow for the circumstance where the primary serial port is in use for serial
  file transfers to a PC or similar. To see the serial prints you need to have this set of #defines;

  #define Monitorport Serial
  #define ENABLEMONITOR

*******************************************************************************************************/
/*
  ToDo:

*/

//110122 added local function printArrayHEX(uint8_t *buff, uint32_t len)
//130122 Made variable and function names unique so that the array transfer routines can be used in the same program
//130122 Converted all Serial prints to Monitorport.print() format
//140322 added #ifdef ENABLEMONITOR to serial prints


#define SDUNUSED(v) (void) (v)               //add SDUNUSED(variable); to avoid compiler warnings 


#ifndef Monitorport
#define Monitorport Serial
#endif

#include <arrayRW.h>
//#define DEBUG                              //enable this define to print additional debug info for segment transfers

uint8_t SDRXPacketL;                         //length of received packet
uint8_t SDRXPacketType;                      //type of received packet, segment write, ACK, NACK etc
uint8_t SDRXHeaderL;                         //length of header
int16_t SDPacketRSSI;                        //stores RSSI of received packet
int8_t  SDPacketSNR;                         //stores signal to noise ratio of received packet
uint16_t SDAckCount;                         //keep a count of acks that are received within timeout period
uint16_t SDNoAckCount;                       //keep a count of acks not received within timeout period
uint16_t SDDTDestinationFileCRC;             //CRC of complete file received
uint32_t SDDTDestinationFileLength;          //length of file written on the destination\receiver
uint16_t SDDTSourceFileCRC;                  //CRC returned of the remote saved file
uint32_t SDDTSourceFileLength;               //length of file at source\transmitter
uint32_t SDDTStartmS;                        //used for timeing transfers
uint16_t SDDTSegment = 0;                    //current segment number
char SDDTfilenamebuff[Maxfilenamesize];      //global buffer to store current filename
uint8_t SDDTheader[16];                      //header array
uint8_t SDDTdata[245];                       //data/segment array
uint8_t SDDTflags = 0;                       //Flags byte used to pass status information between nodes
int SDDTLED = -1;                            //pin number for indicator LED, if -1 then not used
uint16_t SDDTErrors;                         //used for tracking errors in the transfer process

uint16_t SDTXNetworkID;                      //this is used to store the 'network' number from packet received, receiver must have the same networkID
uint16_t SDTXArrayCRC;                       //should contain CRC of data array transmitted
uint8_t  SDTXPacketL;                        //length of transmitted packet
uint16_t SDLocalPayloadCRC;                  //for calculating the local data array CRC
uint8_t SDDTLastSegmentSize;                 //size of the last segment
uint16_t SDDTNumberSegments;                 //number of segments for a file transfer
uint16_t SDDTSentSegments;                   //count of segments sent
bool SDDTFileTransferComplete;               //bool to flag file transfer complete
uint32_t SDDTSendmS;                         //used for timing transfers
float SDDTsendSecs;                          //seconds to transfer a file

uint16_t SDRXErrors;                         //count of packets received with error
uint8_t SDRXFlags;                           //SDDTflags byte in header, could be used to control actions in TX and RX
uint8_t SDRXDataarrayL;                      //length of data array\segment
bool SDDTFileOpened;                         //bool to flag when file has been opened
bool SDDTFileSaved;                         //bool to flag when file has been saved to SD
uint16_t SDDTSegmentNext;                    //next segment expected
uint16_t SDDTReceivedSegments;               //count of segments received
uint16_t SDDTSegmentLast;                    //last segment processed


//Transmitter mode functions
uint32_t SDsendFile(char *filename, uint8_t namelength);
bool SDstartFileTransfer(char *filename, uint8_t filenamesize);
bool SDsendSegments();
bool SDsendFileSegment(uint16_t segnum, uint8_t segmentsize);
bool SDendFileTransfer(char *filename, uint8_t filenamesize);
void SDbuild_DTFileOpenHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize);
void SDbuild_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum);
void SDbuild_DTFileCloseHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize);
void SDprintLocalFileDetails();
void SDprintSeconds();
void SDprintAckBrief();
void SDprintAckReception();
void SDprintACKdetail();
void SDprintdata(uint8_t *dataarray, uint8_t arraysize);
void SDprintPacketHex();
bool SDsendDTInfo();
void SDbuild_DTInfoHeader(uint8_t *header, uint8_t headersize, uint8_t datalen);

//Receiver mode functions
bool SDreceiveaPacketDT();
void SDreadHeaderDT();
bool SDprocessPacket(uint8_t packettype);
void SDprintPacketDetails();
bool SDprocessSegmentWrite();
bool SDprocessFileOpen(uint8_t *filename, uint8_t filenamesize);
bool SDprocessFileClose();
void SDprintPacketRSSI();
void SDprintSourceFileDetails();
void SDprintDestinationFileDetails();

//Common functions
void SDsetLED(int8_t pinnumber);
void SDprintheader(uint8_t *header, uint8_t headersize);
void SDprintReliableStatus();
void setCursor(uint8_t lcol, uint8_t lrow);

//bit numbers used by SDDTErrors (16bits) and RXErrors (first 8bits)
const uint8_t SDNoFileSave = 0;              //bit number of SDDTErrors to set when no file save, to SD for example
const uint8_t SDNothingToSend = 1;           //bit number of SDDTErrors to set when nothing to send or unable to send image\file
const uint8_t SDNoCamera = 1;                //bit number of SDDTErrors to set when camera fails
const uint8_t SDSendFile = 2;                //bit number of SDDTErrors to set when file SD file image\file send fail
const uint8_t SDSendArray = 2;               //bit number of SDDTErrors to set when file array image\file send fail
const uint8_t SDNoACKlimit = 3;              //bit number of SDDTErrors to set when NoACK limit reached
const uint8_t SDSendPacket = 4;              //bit number of SDDTErrors to set when sending a packet fails or there is no ack

const uint8_t SDStartTransfer = 11;          //bit number of SDDTErrors to set when StartTransfer fails
const uint8_t SDSendSegments = 12;           //bit number of SDDTErrors to set when SendSegments function fails
const uint8_t SDSendSegment = 13;            //bit number of SDDTErrors to set when sending a single Segment send fails
const uint8_t SDOpeningFile = 14;            //bit number of SDDTErrors to set when opening file fails
const uint8_t SDendTransfer = 15;            //bit number of SDDTErrors to set when end transfer fails



//************************************************
//Transmit mode functions
//************************************************

uint32_t SDsendFile(char *filename, uint8_t namelength)
{
  //This routine allows the file transfer to be run with a function call of sendFile(filename, sizeof(filename));
  memcpy(SDDTfilenamebuff, filename, namelength);  //copy the name of file into global filename array for use outside this function

  uint8_t localattempts = 0;

  SDDTErrors = 0;                                  //clear all error flags
  SDDTDestinationFileCRC = 0;
  SDDTSourceFileCRC = 0;
  SDDTDestinationFileLength = 0;
  SDDTSourceFileLength = 0;

  do
  {
    localattempts++;
    SDNoAckCount = 0;
    SDDTStartmS = millis();

#ifdef ENABLEMONITOR
    Monitorport.print(F("Send file attempt "));
    Monitorport.println(localattempts);
#endif

    //opens the local file to send and sets up transfer parameters
    if (SDstartFileTransfer(filename, namelength))
    {
#ifdef ENABLEMONITOR
      Monitorport.print(filename);
      Monitorport.println(F(" opened OK on remote"));
      SDprintLocalFileDetails();
#endif
    }
    else
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("********************"));
      Monitorport.println(filename);
      Monitorport.println(F("ERROR opening file"));
      Monitorport.println(F("Restarting transfer"));
      Monitorport.println(F("********************"));
#endif
      SDDTFileTransferComplete = false;
      delay(2000);
      continue;
    }

    delay(FunctionDelaymS);

    if (!SDsendSegments())
    {
#ifdef ENABLEMONITOR
      Monitorport.println();
      Monitorport.println(F("************************"));
      Monitorport.println(filename);
      Monitorport.println(F("ERROR in SDsendSegments()"));
      Monitorport.println(F("Restarting transfer"));
      Monitorport.println(F("***********************"));
      Monitorport.println();
#endif
      SDDTFileTransferComplete = false;
      continue;
    }

    delay(FunctionDelaymS);

    if (SDendFileTransfer(filename, namelength))             //send command to close remote file
    {
      SDDTSendmS = millis() - SDDTStartmS;                   //record time taken for transfer
      beginarrayRW(SDDTheader, 4);
      SDDTDestinationFileLength = arrayReadUint32();
#ifdef ENABLEMONITOR
      Monitorport.print(filename);
      Monitorport.println(F(" closed OK on remote"));
      Monitorport.print(F("Acknowledged remote destination file length "));
      Monitorport.println(SDDTDestinationFileLength);
#endif

      if (SDDTDestinationFileLength != SDDTSourceFileLength)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("*******************************"));
        Monitorport.println(filename);
        Monitorport.println(F("ERROR file lengths do not match"));
        Monitorport.println(F("Restarting transfer"));
        Monitorport.println(F("*******************************"));
#endif
        SDDTFileTransferComplete = false;
        continue;
      }
      else
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("File lengths match"));
#endif
      }

#ifdef ENABLEFILECRC
      SDDTDestinationFileCRC = arrayReadUint16();
#ifdef ENABLEMONITOR
      Monitorport.print(F("Acknowledged remote destination file CRC 0x"));
      Monitorport.println(SDDTDestinationFileCRC, HEX);
#endif

      if (SDDTDestinationFileCRC != SDDTSourceFileCRC)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("****************************"));
        Monitorport.println(filename);
        Monitorport.println(F("ERROR file CRCs do not match"));
        Monitorport.println(F("Restarting transfer"));
        Monitorport.println(F("****************************"));
#endif
        SDDTFileTransferComplete = false;
        continue;
      }
      else
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("File CRCs match"));
#endif
      }
#endif
      //end of ENABLEFILECRC
      SDDTFileTransferComplete = true;
    }
    else
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("******************************"));
      Monitorport.println(filename);
      Monitorport.println(F("ERROR close remote file failed"));
      Monitorport.println(F("Restarting transfer"));
      Monitorport.println(F("******************************"));
#endif
      SDDTFileTransferComplete = false;
      continue;
    }
  }
  while ((!SDDTFileTransferComplete) && (localattempts < StartAttempts));

  SDDTsendSecs = (float) SDDTSendmS / 1000;

#ifdef ENABLEMONITOR
  Monitorport.print(F("StartAttempts "));
  Monitorport.println(localattempts);
  Monitorport.print(F("SDNoAckCount "));
  Monitorport.println(SDNoAckCount);
  Monitorport.print(F("Transmit time "));
  Monitorport.print(SDDTsendSecs, 3);
  Monitorport.println(F("secs"));
  Monitorport.print(F("Transmit rate "));
  Monitorport.print( (SDDTDestinationFileLength * 8) / (SDDTsendSecs), 0 );
  Monitorport.println(F("bps"));
#endif

  if (localattempts == StartAttempts)
  {
    bitSet(SDDTErrors, SDSendFile);
    return 0;
  }

  return SDDTDestinationFileLength;
}


bool SDstartFileTransfer(char *filename, uint8_t filenamesize)
{
  //Start file transfer, open local file first then remote file.

  uint8_t ValidACK;
  uint8_t localattempts = 0;

#ifdef ENABLEMONITOR
  Monitorport.print(F("Start file transfer for "));
  Monitorport.println(filename);
#endif
  SDDTSourceFileLength = DTSD_openFileRead(filename);                   //get the file length

  if (SDDTSourceFileLength == 0)
  {
#ifdef ENABLEMONITOR
    Monitorport.print(F("Error - opening file"));
    Monitorport.println(filename);
#endif
    bitSet(SDDTErrors, SDOpeningFile);
    return false;
  }

#ifdef ENABLEFILECRC
  SDDTSourceFileCRC = DTSD_fileCRCCCITT(SDDTSourceFileLength);        //get file CRC from position 0 to end
#endif

  SDDTNumberSegments = DTSD_getNumberSegments(SDDTSourceFileLength, SegmentSize);
  SDDTLastSegmentSize = DTSD_getLastSegmentSize(SDDTSourceFileLength, SegmentSize);
  SDbuild_DTFileOpenHeader(SDDTheader, DTFileOpenHeaderL, filenamesize, SDDTSourceFileLength, SDDTSourceFileCRC, SegmentSize);
  SDLocalPayloadCRC = LoRa.CRCCCITT((uint8_t *) filename, filenamesize, 0xFFFF);

  do
  {
    localattempts++;
#ifdef ENABLEMONITOR
    Monitorport.println(F("Send open remote file request"));
#endif

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }

    SDTXPacketL = LoRa.transmitDT(SDDTheader, DTFileOpenHeaderL, (uint8_t *) filename, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }

#ifdef ENABLEMONITOR
#ifdef DEBUG
    SDTXNetworkID = LoRa.getTXNetworkID(SDTXPacketL);     //get the networkID appended to packet
    SDTXArrayCRC = LoRa.getTXPayloadCRC(SDTXPacketL);     //get the payload CRC appended to packet
    Monitorport.print(F("Send attempt "));
    Monitorport.println(localattempts);
    Monitorport.print(F("SDTXNetworkID,0x"));
    Monitorport.println(SDTXNetworkID, HEX);
    Monitorport.print(F("SDTXArrayCRC,0x"));
    Monitorport.println(SDTXArrayCRC, HEX);
#endif
#endif

    //Monitorport.println();
    //LoRa.printSXBufferHEX(0, 254);
    //Monitorport.println();
    //Monitorport.println();

    if (SDTXPacketL == 0)                                 //if there has been a send and ack error, SDTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
    }

    ValidACK = LoRa.waitACKDT(SDDTheader, DTFileOpenHeaderL, ACKopentimeoutmS);
    SDRXPacketType = SDDTheader[0];

    if ((ValidACK > 0) && (SDRXPacketType == DTFileOpenACK))
    {
#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.println(F(" Valid ACK "));
#endif
#endif
    }
    else
    {
      SDNoAckCount++;
#ifdef ENABLEMONITOR
      Monitorport.println(F("NoACK"));
#ifdef DEBUG
      SDprintACKdetail();
      Monitorport.print(F("  ACKPacket "));
      SDprintPacketHex();
#endif
#endif
      if (SDNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
#endif
        bitSet(SDDTErrors, SDNoACKlimit);
        return false;
      }
#ifdef ENABLEMONITOR
      Monitorport.println();
#endif
    }
  }
  while ((ValidACK == 0) && (localattempts < SendAttempts));

  if (localattempts == SendAttempts)
  {
    bitSet(SDDTErrors, SDStartTransfer);
    return false;
  }

  return true;
}


bool SDsendSegments()
{
  //Start the file transfer at segment 0
  SDDTSegment = 0;
  SDDTSentSegments = 0;

  dataFile.seek(0);                       //ensure at first position in file

  while (SDDTSegment < (SDDTNumberSegments - 1))
  {
#ifdef ENABLEMONITOR
#ifdef DEBUG
    SDprintSeconds();
#endif
#endif

    if (SDsendFileSegment(SDDTSegment, SegmentSize))
    {
      SDDTSentSegments++;
    }
    else
    {
      bitSet(SDDTErrors, SDSendSegment);
      return false;
    }
    delay(FunctionDelaymS);
  };

#ifdef ENABLEMONITOR
  Monitorport.println(F("Last segment"));
#endif

  if (!SDsendFileSegment(SDDTSegment, SDDTLastSegmentSize))
  {
    bitSet(SDDTErrors, SDSendSegment);
    return false;
  }

  return true;
}


bool SDsendFileSegment(uint16_t segnum, uint8_t segmentsize)
{
  //Send file segment as payload in a DT packet

  uint8_t ValidACK;
  uint8_t localattempts = 0;

  DTSD_readFileSegment(SDDTdata, segmentsize);
  SDbuild_DTSegmentHeader(SDDTheader, DTSegmentWriteHeaderL, segmentsize, segnum);

#ifdef ENABLEMONITOR

#ifdef PRINTSEGMENTNUM
  Monitorport.println(segnum);
#endif

#ifdef DEBUG
  SDprintheader(SDDTheader, DTSegmentWriteHeaderL);
  Monitorport.print(F(" "));
  SDprintdata(SDDTdata, 16);                                  //print first 16 bytes data array
#endif

#endif

  do
  {
    localattempts++;

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }

    SDTXPacketL = LoRa.transmitDT(SDDTheader, DTSegmentWriteHeaderL, (uint8_t *) SDDTdata, segmentsize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }

    if (SDTXPacketL == 0)                                     //if there has been an error SDTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
    }

    ValidACK = LoRa.waitACKDT(SDDTheader, DTSegmentWriteHeaderL, ACKsegtimeoutmS);
    SDRXPacketType = SDDTheader[0];

    if (ValidACK > 0)
    {
      if (SDRXPacketType == DTSegmentWriteNACK)
      {
        SDDTSegment = SDDTheader[4] +  (SDDTheader[5] << 8);      //load what the segment number should be
        SDRXHeaderL = SDDTheader[2];
        DTSD_seekFileLocation(SDDTSegment * SegmentSize);
#ifdef ENABLEMONITOR
        Monitorport.println();
        Monitorport.println(F("************************************"));
        Monitorport.print(F("Received restart request at segment "));
        Monitorport.println(SDDTSegment);
        SDprintheader(SDDTheader, SDRXHeaderL);
        Monitorport.println();
        Monitorport.print(F("Seek to file location "));
        Monitorport.println(SDDTSegment * SegmentSize);
        Monitorport.println(F("************************************"));
        Monitorport.println();
        Monitorport.flush();
#endif

      }

      //ack is valid, segment was acknowledged if here

      if (SDRXPacketType == DTStartNACK)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("Received restart request"));
#endif
        return false;
      }

      if (SDRXPacketType == DTSegmentWriteACK)
      {
        SDAckCount++;
        SDDTSegment++;                  //increase value for next segment
#ifdef ENABLEMONITOR
#ifdef DEBUG
        SDprintAckBrief();
#endif
#endif
        return true;
      }
    }
    else
    {
      SDNoAckCount++;
#ifdef ENABLEMONITOR
      Monitorport.println(F("NoACK"));
#endif

      if (SDNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
#endif
        bitSet(SDDTErrors, SDNoACKlimit);
        return false;
      }
    }
  } while ((ValidACK == 0) && (localattempts < SendAttempts)) ;


  if (localattempts == SendAttempts)
  {
    bitSet(SDDTErrors, SDSendSegment);
    return 0;
  }

  return true;
}


bool SDendFileTransfer(char *filename, uint8_t filenamesize)
{
  //End file transfer, close local file first then remote file

  uint8_t ValidACK;
  uint8_t localattempts = 0;

  DTSD_closeFile();
  SDbuild_DTFileCloseHeader(SDDTheader, DTFileCloseHeaderL, filenamesize, SDDTSourceFileLength, SDDTSourceFileCRC, SegmentSize);

  do
  {
    localattempts++;
#ifdef ENABLEMONITOR
    Monitorport.println(F("Send close remote file"));
#endif

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }
    SDTXPacketL = LoRa.transmitDT(SDDTheader, DTFileCloseHeaderL, (uint8_t *) filename, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }

    SDTXNetworkID = LoRa.getTXNetworkID(SDTXPacketL);
    SDTXArrayCRC = LoRa.getTXPayloadCRC(SDTXPacketL);

#ifdef ENABLEMONITOR
#ifdef DEBUG
    Monitorport.print(F("SDTXNetworkID,0x"));
    Monitorport.print(SDTXNetworkID, HEX);               //get the NetworkID of the packet just sent, its placed at the packet end
    Monitorport.print(F(",SDTXArrayCRC,0x"));
    Monitorport.println(SDTXArrayCRC, HEX);              //get the CRC of the data array just sent, its placed at the packet end
    Monitorport.println();
#endif
#endif

    if (SDTXPacketL == 0)                           //if there has been a send and ack error, SDTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
    }

    ValidACK = LoRa.waitACKDT(SDDTheader, DTFileCloseHeaderL, ACKclosetimeoutmS);
    SDRXPacketType = SDDTheader[0];

    if ((ValidACK > 0) && (SDRXPacketType == DTFileCloseACK))
    {
#ifdef ENABLEMONITOR
#ifdef DEBUG
      SDprintPacketHex();
#endif
#endif
    }
    else
    {
      SDNoAckCount++;
#ifdef ENABLEMONITOR
      Monitorport.println(F("NoACK"));
#endif
      if (SDNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
#endif
        bitSet(SDDTErrors, SDNoACKlimit);
        return false;
      }
#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.println();
      Monitorport.print(F("  ACKPacket "));
      SDprintPacketHex();
      Monitorport.println();
#endif
#endif
    }
  }
  while ((ValidACK == 0) && (localattempts < SendAttempts));

  if (localattempts == SendAttempts)
  {
    bitSet(SDDTErrors, SDendTransfer);
    return 0;
  }

  return true;
}


void SDbuild_DTFileOpenHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize)
{
  //This builds the header buffer for the filename to send

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTFileOpen);         //byte 0, write the packet type
  arrayWriteUint8(SDDTflags);            //byte 1, SDDTflags byte
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(filelength);        //byte 4,5,6,7, write the file length
  arrayWriteUint16(filecrc);           //byte 8, 9, write file CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


void SDbuild_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum)
{
  //This builds the header buffer for a segment transmit

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTSegmentWrite);     //write the packet type
  arrayWriteUint8(SDDTflags);            //SDDTflags byte
  arrayWriteUint8(headersize);         //write length of header
  arrayWriteUint8(datalen);            //write length of data array
  arrayWriteUint16(segnum);            //write the DTsegment number
  endarrayRW();
}


void SDbuild_DTFileCloseHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize)
{
  //This builds the header buffer for the filename passed

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTFileClose);        //byte 0, write the packet type
  arrayWriteUint8(SDDTflags);            //byte 1, SDDTflags byte
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(filelength);        //byte 4,5,6,7, write the file length
  arrayWriteUint16(filecrc);           //byte 8, 9, write file CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


void SDprintLocalFileDetails()
{
#ifdef ENABLEMONITOR
  Monitorport.print(F("Source file length "));
  Monitorport.print(SDDTSourceFileLength);
  Monitorport.println(F(" bytes"));
#ifdef ENABLEFILECRC
  Monitorport.print(F("Source file CRC is 0x"));
  Monitorport.println(SDDTSourceFileCRC, HEX);
#endif
  Monitorport.print(F("Segment Size "));
  Monitorport.println(SegmentSize);
  Monitorport.print(F("Number segments "));
  Monitorport.println(SDDTNumberSegments);
  Monitorport.print(F("Last segment size "));
  Monitorport.println(SDDTLastSegmentSize);
#endif
}


void SDprintSeconds()
{
#ifdef ENABLEMONITOR
  float secs;
  secs = ( (float) millis() / 1000);
  Monitorport.print(secs, 2);
  Monitorport.print(F(" "));
#endif
}


void SDprintAckBrief()
{
#ifdef ENABLEMONITOR
  SDPacketRSSI = LoRa.readPacketRSSI();
  Monitorport.print(F(",AckRSSI,"));
  Monitorport.print(SDPacketRSSI);
  Monitorport.println(F("dBm"));
#endif
}


void SDprintAckReception()
{
#ifdef ENABLEMONITOR
  SDPacketRSSI = LoRa.readPacketRSSI();
  SDPacketSNR = LoRa.readPacketSNR();
  Monitorport.print(F("SDAckCount,"));
  Monitorport.print(SDAckCount);
  Monitorport.print(F(",SDNoAckCount,"));
  Monitorport.print(SDNoAckCount);
  Monitorport.print(F(",AckRSSI,"));
  Monitorport.print(SDPacketRSSI);
  Monitorport.print(F("dBm,AckSNR,"));
  Monitorport.print(SDPacketSNR);
  Monitorport.print(F("dB"));
  Monitorport.println();
#endif
}


void SDprintACKdetail()
{
#ifdef ENABLEMONITOR
  Monitorport.print(F("ACKDetail"));
  Monitorport.print(F(",RXNetworkID,0x"));
  Monitorport.print(LoRa.getRXNetworkID(SDRXPacketL), HEX);
  Monitorport.print(F(",RXPayloadCRC,0x"));
  Monitorport.print(LoRa.getRXPayloadCRC(SDRXPacketL), HEX);
  Monitorport.print(F(",SDRXPacketL,"));
  Monitorport.print(SDRXPacketL);
  Monitorport.print(F(" "));
  SDprintReliableStatus();
  Monitorport.println();
#endif
}


void SDprintdata(uint8_t *dataarray, uint8_t arraysize)
{
  SDUNUSED(dataarray);           //to prevent a compiler warning
  SDUNUSED(arraysize);           //to prevent a compiler warning
#ifdef ENABLEMONITOR
  Monitorport.print(F("DataBytes,"));
  Monitorport.print(arraysize);
  Monitorport.print(F("  "));
  printarrayHEX((uint8_t *) dataarray, 16);             //There is a lot of data to print so only print first 16 bytes
#endif
}


bool SDsendDTInfo()
{
  //Send array info packet, for this implmentation its really only the flags in ARDTflags that is sent

  uint8_t ValidACK = 0;
  uint8_t localattempts = 0;

  SDbuild_DTInfoHeader(SDDTheader, DTInfoHeaderL, 0);

  do
  {
    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }

    localattempts++;
#ifdef ENABLEMONITOR
    Monitorport.print(F("Send DTInfo packet attempt "));
    Monitorport.println(localattempts);
#endif
    SDTXPacketL = LoRa.transmitDT(SDDTheader, DTInfoHeaderL, (uint8_t *) SDDTdata, 0, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);

    if (SDTXPacketL == 0)                                         //if there has been an error ARTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
      continue;
    }

    ValidACK = LoRa.waitACKDT(SDDTheader, DTInfoHeaderL, ACKsegtimeoutmS);

    if (ValidACK > 0)
    {
      //ack is a valid reliable packet
      SDRXPacketType = SDDTheader[0];
#ifdef ENABLEMONITOR
      Monitorport.print(F("ACK Packet type 0x"));
      Monitorport.println(SDRXPacketType, HEX);
#endif

      if (SDRXPacketType == DTInfoACK)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("DTInfoACK received"));
#endif
        SDAckCount++;
        SDRXPacketType = SDDTheader[0];
        return true;
      }
      else
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("DTInfoACK not received"));
#endif
        return false;
      }

    }
    else
    {
      SDNoAckCount++;
#ifdef ENABLEMONITOR
      Monitorport.println(F("No valid ACK received "));
#endif

      if (SDNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
#endif
        return false;
      }
    }
    delay(PacketDelaymS);
  }
  while ((ValidACK == 0) && (localattempts < SendAttempts));

  if (localattempts == SendAttempts)
  {
    bitSet(SDDTErrors, SDSendPacket);
    return false;
  }
  return true;
}


void SDbuild_DTInfoHeader(uint8_t *header, uint8_t headersize, uint8_t datalen)
{
  //This builds the header buffer for a info only header, faults problems etc

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTInfo);             //write the packet type
  arrayWriteUint8(SDDTflags);          //ARDTflags byte
  arrayWriteUint8(headersize);         //write length of header
  arrayWriteUint8(datalen);            //write length of data array
  arrayWriteUint8(0);                  //unused
  arrayWriteUint8(0);                  //unused
  arrayWriteUint8(0);                  //unused
  arrayWriteUint8(0);                  //unused
  endarrayRW();
}


//************************************************
//Receiver mode  functions
//************************************************


bool SDreceiveaPacketDT()
{
  //Receive Data transfer packets

  SDRXPacketType = 0;
  SDRXPacketL = LoRa.receiveDT(SDDTheader, HeaderSizeMax, (uint8_t *) SDDTdata, DataSizeMax, NetworkID, RXtimeoutmS, WAIT_RX);

  if (SDDTLED >= 0)
  {
    digitalWrite(SDDTLED, HIGH);
  }

#ifdef DEBUG
  SDprintSeconds();
#endif

  if (SDRXPacketL > 0)
  {
    //if the LT.receiveDT() returns a value > 0 for SDRXPacketL then packet was received OK
    SDreadHeaderDT();                        //get the basic header details into global variables SDRXPacketType etc
    SDprocessPacket(SDRXPacketType);         //process and act on the packet
    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }
    return true;
  }
  else
  {
    //if the LoRa.receiveDT() function detects an error RXOK is 0
    uint16_t IRQStatus = LoRa.readIrqStatus();

    if (IRQStatus & IRQ_RX_TIMEOUT)
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("RX Timeout"));
#endif
    }
    else
    {
      SDRXErrors++;
#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.print(F("PacketError"));
      SDprintPacketDetails();
      SDprintReliableStatus();
      Monitorport.println();
#endif
#endif

      if (SDDTLED >= 0)
      {
        digitalWrite(SDDTLED, LOW);
      }

    }
  }
  return false;
}


void SDreadHeaderDT()
{
  //The first 6 bytes of the header contain the important stuff, so load it up
  //so we can decide what to do next.
  beginarrayRW(SDDTheader, 0);                      //start buffer read at location 0
  SDRXPacketType = arrayReadUint8();                //load the packet type
  SDRXFlags = arrayReadUint8();                     //SDDTflags byte
  SDRXHeaderL = arrayReadUint8();                   //load the header length
  SDRXDataarrayL = arrayReadUint8();                //load the datalength
  SDDTSegment = arrayReadUint16();                  //load the segment number
}


bool SDprocessPacket(uint8_t packettype)
{
  //Decide what to do with an incoming packet

  if (packettype == DTSegmentWrite)
  {
    SDprocessSegmentWrite();
    return true;
  }

  if (packettype == DTFileOpen)
  {
    SDprocessFileOpen(SDDTdata, SDRXDataarrayL);
    return true;
  }

  if (packettype == DTFileClose)
  {
    SDprocessFileClose();
    return true;
  }

  return true;
}


void SDprintPacketDetails()
{
#ifdef ENABLEMONITOR
  SDPacketRSSI = LoRa.readPacketRSSI();
  SDPacketSNR = LoRa.readPacketSNR();
  Monitorport.print(F(" RSSI,"));
  Monitorport.print(SDPacketRSSI);
  Monitorport.print(F("dBm"));

#ifdef DEBUG
  Monitorport.print(F(",SNR,"));
  Monitorport.print(SDPacketSNR);
  Monitorport.print(F("dBm,RXOKCount,"));
  Monitorport.print(SDDTReceivedSegments);
  Monitorport.print(F(",RXErrs,"));
  Monitorport.print(SDRXErrors);
  Monitorport.print(F(" RX"));
  SDprintheader(SDDTheader, SDRXHeaderL);
#endif
#endif
}


bool SDprocessSegmentWrite()
{
  //There is a request to write a segment to file on receiver
  //checks that the sequence of segment writes is correct

  if (!SDDTFileOpened)
  {
    //something is wrong, have received a request to write a segment but there is no file opened
    //need to reject the segment write with a restart NACK
#ifdef ENABLEMONITOR
    Monitorport.println();
    Monitorport.println(F("***************************************************"));
    Monitorport.println(F("Error - Segment write with no file open - send NACK"));
    Monitorport.println(F("***************************************************"));
    Monitorport.println();
#endif
    SDDTheader[0] = DTStartNACK;
    delay(ACKdelaymS);
    delay(DuplicatedelaymS);

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }
    LoRa.sendACKDT(SDDTheader, DTStartHeaderL, TXpower);
    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }
    return false;
  }

  if (SDDTSegment == SDDTSegmentNext)
  {
    DTSD_writeSegmentFile(SDDTdata, SDRXDataarrayL);

#ifdef ENABLEMONITOR
#ifdef PRINTSEGMENTNUM
    //Monitorport.print(F("Segment,"));
    Monitorport.println(SDDTSegment);
    setCursor(0, 6);
    disp.print(SDDTSegment);
#endif

#ifdef DEBUG
    Monitorport.print(F("Bytes,"));
    Monitorport.print(SDRXDataarrayL);
    SDprintPacketRSSI();
    Monitorport.println(F(" SendACK"));
#endif
#endif
    SDDTheader[0] = DTSegmentWriteACK;
    delay(ACKdelaymS);

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }
    LoRa.sendACKDT(SDDTheader, DTSegmentWriteHeaderL, TXpower);

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }
    SDDTReceivedSegments++;
    SDDTSegmentLast = SDDTSegment;                  //so we can tell if sequece has been received twice
    SDDTSegmentNext = SDDTSegment + 1;
    return true;
  }

  if (SDDTSegment == SDDTSegmentLast)
  {
#ifdef ENABLEMONITOR
    Monitorport.print(F("ERROR segment "));
    Monitorport.print(SDDTSegment);
    Monitorport.println(F(" already received "));
    delay(DuplicatedelaymS);
#ifdef DEBUG
    SDprintPacketDetails();
    SDprintPacketRSSI();
#endif
#endif
    SDDTheader[0] = DTSegmentWriteACK;
    delay(ACKdelaymS);

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }
    LoRa.sendACKDT(SDDTheader, DTSegmentWriteHeaderL, TXpower);

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }
    return true;
  }

  if (SDDTSegment != SDDTSegmentNext )
  {
    SDDTheader[0] = DTSegmentWriteNACK;
    SDDTheader[4] = lowByte(SDDTSegmentNext);
    SDDTheader[5] = highByte(SDDTSegmentNext);
    delay(ACKdelaymS);
    delay(DuplicatedelaymS);                   //add an extra delay here to stop repeated segment sends

#ifdef ENABLEMONITOR
    Monitorport.print(F(" ERROR Received Segment "));
    Monitorport.print(SDDTSegment);
    Monitorport.print(F(" expected "));
    Monitorport.print(SDDTSegmentNext);
    Monitorport.print(F(" "));

#ifdef DEBUG
    SDprintPacketDetails();
    SDprintPacketRSSI();
#endif

    Monitorport.print(F(" Send NACK for segment "));
    Monitorport.print(SDDTSegmentNext);
    Monitorport.println();
    Monitorport.println();
    Monitorport.println(F("*****************************************"));
    Monitorport.print(F("Transmit restart request for segment "));
    Monitorport.println(SDDTSegmentNext);
    SDprintheader(SDDTheader, SDRXHeaderL);
    Monitorport.println();
    Monitorport.println(F("*****************************************"));
    Monitorport.println();
    Monitorport.flush();
#endif

    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, HIGH);
    }
    LoRa.sendACKDT(SDDTheader, DTSegmentWriteHeaderL, TXpower);
    if (SDDTLED >= 0)
    {
      digitalWrite(SDDTLED, LOW);
    }
    return false;
  }

  return true;
}


bool SDprocessFileOpen(uint8_t *filename, uint8_t filenamesize)
{
  //There is a request to open local file on receiver

  SDDTDestinationFileCRC = 0;                          //CRC of complete file received
  SDDTDestinationFileLength = 0;                       //length of file written on the destination\receiver

  beginarrayRW(SDDTheader, 4);                         //start buffer read at location 4
  SDDTSourceFileLength = arrayReadUint32();            //load the file length of the remote file being sent
  SDDTSourceFileCRC = arrayReadUint16();               //load the CRC of the source file being sent
  memset(SDDTfilenamebuff, 0, Maxfilenamesize);        //clear SDDTfilenamebuff to all 0s
  memcpy(SDDTfilenamebuff, filename, filenamesize);    //copy received SDDTdata into SDDTfilenamebuff

#ifdef ENABLEMONITOR
  Monitorport.print((char*) SDDTfilenamebuff);
  Monitorport.print(F(" SD File Open request"));
  Monitorport.println();

#ifdef DEBUG
  SDTXNetworkID = LoRa.getTXNetworkID(SDRXPacketL);     //get the networkID appended to packet
  SDTXArrayCRC = LoRa.getTXPayloadCRC(SDRXPacketL);     //get the payload CRC appended to packet
  Monitorport.print(F("SDTXNetworkID,0x"));
  Monitorport.println(SDTXNetworkID, HEX);
  Monitorport.print(F("SDTXArrayCRC,0x"));
  Monitorport.println(SDTXArrayCRC, HEX);
#endif

  SDprintSourceFileDetails();

  if bitRead(SDRXFlags, SDNoFileSave)
  {
    Monitorport.println(F("Remote did not save file to SD"));
  }
#endif

  if (DTSD_openNewFileWrite(SDDTfilenamebuff))      //open file for write at beginning, delete if it exists
  {
#ifdef ENABLEMONITOR
    Monitorport.print((char*) SDDTfilenamebuff);
    Monitorport.println(F(" DT File Opened OK"));
    Monitorport.println(F("Waiting File"));
#endif
    SDDTSegmentNext = 0;                            //since file is opened the next sequence should be the first
    SDDTFileOpened = true;
    SDDTStartmS = millis();
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.print((char*) SDDTfilenamebuff);
    Monitorport.println(F(" File Open fail"));
#endif
    SDDTFileOpened = false;
    return false;
  }

  setCursor(0, 0);
  disp.print(F("Receiving File  "));
  setCursor(0, 1);
  disp.print(F("             "));                    //clear previous filename
  setCursor(0, 1);
  disp.print(SDDTfilenamebuff);
  setCursor(0, 2);
  disp.print(F("            "));                     //clear previous file size
  setCursor(0, 2);
  disp.print(SDDTSourceFileLength);
  disp.print(F(" bytes"));
  setCursor(0, 3);
  disp.print(F("          "));                       //clear transfer time
  setCursor(0, 5);
  disp.print(F("    "));

  SDDTStartmS = millis();
  delay(ACKdelaymS);

#ifdef ENABLEMONITOR
#ifdef DEBUG
  Monitorport.println(F("Sending ACK"));
#endif
#endif

  SDDTheader[0] = DTFileOpenACK;                    //set the ACK packet type

  if (SDDTLED >= 0)
  {
    digitalWrite(SDDTLED, HIGH);
  }
  LoRa.sendACKDT(SDDTheader, DTFileOpenHeaderL, TXpower);
  if (SDDTLED >= 0)
  {
    digitalWrite(SDDTLED, LOW);
  }
  SDDTSegmentNext = 0;                               //after file open, segment 0 is next

  return true;
}


bool SDprocessFileClose()
{
  // There is a request to close a file on SD of receiver

#ifdef ENABLEMONITOR
  Monitorport.print((char*) SDDTfilenamebuff);
  Monitorport.println(F(" File close request"));
#endif

  SDDTFileSaved = false;

  if (SDDTFileOpened)                                     //check if file has been opened, close it if it is
  {
    if (SD.exists(SDDTfilenamebuff))                      //check if file exists
    {
      DTSD_closeFile();

#ifdef ENABLEMONITOR
      Monitorport.print(F("Transfer time "));
      Monitorport.print(millis() - SDDTStartmS);
      Monitorport.print(F("mS"));
      Monitorport.println();
      Monitorport.println(F("File closed"));
#endif

      setCursor(0, 0);
      disp.print(F("File Received   "));
      setCursor(0, 3);
      disp.print(F("          "));
      setCursor(0, 3);
      disp.print(millis() - SDDTStartmS);
      disp.print(F(" mS"));

      SDDTFileOpened = false;
      SDDTDestinationFileLength = DTSD_openFileRead(SDDTfilenamebuff);

#ifdef ENABLEFILECRC
      SDDTDestinationFileCRC = DTSD_fileCRCCCITT(SDDTDestinationFileLength);
#endif

      beginarrayRW(SDDTheader, 4);                       //start writing to array at location 12
      arrayWriteUint32(SDDTDestinationFileLength);       //write file length of file just written just written to ACK header
      arrayWriteUint16(SDDTDestinationFileCRC);          //write CRC of file just written to ACK header

#ifdef ENABLEMONITOR
      SDprintDestinationFileDetails();
#endif
    }
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("File already closed"));
#endif
    delay(DuplicatedelaymS);
  }

  delay(ACKdelaymS);
#ifdef ENABLEMONITOR
#ifdef DEBUG
  Monitorport.println(F("Sending ACK"));
#endif
#endif
  SDDTheader[0] = DTFileCloseACK;

  if (SDDTLED >= 0)
  {
    digitalWrite(SDDTLED, HIGH);
  }
  LoRa.sendACKDT(SDDTheader, DTFileCloseHeaderL, TXpower);
  if (SDDTLED >= 0)
  {
    digitalWrite(SDDTLED, LOW);
  }
#ifdef ENABLEMONITOR
  Monitorport.println();
#endif

  SDDTFileSaved = true;
  return true;
}


void SDprintPacketRSSI()
{
#ifdef ENABLEMONITOR
  SDPacketRSSI = LoRa.readPacketRSSI();
  Monitorport.print(F(" RSSI,"));
  Monitorport.print(SDPacketRSSI);
  Monitorport.print(F("dBm"));
#endif
}


void SDprintSourceFileDetails()
{

#ifdef ENABLEMONITOR
  Monitorport.print(F("Source file length is "));
  Monitorport.print(SDDTSourceFileLength);
  Monitorport.println(F(" bytes"));
#ifdef ENABLEFILECRC
  Monitorport.print(F("Source file CRC is 0x"));
  Monitorport.println(SDDTSourceFileCRC, HEX);
#endif
#endif
}


void SDprintDestinationFileDetails()
{
#ifdef ENABLEMONITOR
  Monitorport.print(F("Destination file length "));
  Monitorport.print(SDDTDestinationFileLength);
  Monitorport.println(F(" bytes"));
  if (SDDTDestinationFileLength != SDDTSourceFileLength)
  {
    Monitorport.println(F("ERROR - file lengths do not match"));
  }
  else
  {
    Monitorport.println(F("File lengths match"));
  }

#ifdef ENABLEFILECRC
  Monitorport.print(F("Destination file CRC is 0x"));
  Monitorport.println(SDDTDestinationFileCRC, HEX);
  if (SDDTDestinationFileCRC != SDDTSourceFileCRC)
  {
    Monitorport.println(F("ERROR - file CRCs do not match"));
  }
  else
  {
    Monitorport.println(F("File CRCs match"));
  }
#endif

#endif
}

//************************************************
//Common functions
//************************************************

void SDsetLED(int8_t pinnumber)
{
  if (pinnumber >= 0)
  {
    SDDTLED = pinnumber;
    pinMode(pinnumber, OUTPUT);
  }
}


void SDprintheader(uint8_t *header, uint8_t headersize)
{
  SDUNUSED(header);               //to prevent a compiler warning
  SDUNUSED(headersize);           //to prevent a compiler warning

#ifdef ENABLEMONITOR
  Monitorport.print(F("HeaderBytes,"));
  Monitorport.print(headersize);
  Monitorport.print(F(" "));
  printarrayHEX(header, headersize);
#endif
}


void printArrayHEX(uint8_t *buff, uint32_t len)
{
  SDUNUSED(buff);           //to prevent a compiler warning
  SDUNUSED(len);           //to prevent a compiler warning

#ifdef ENABLEMONITOR
  uint8_t index, buffdata;
  for (index = 0; index < len; index++)
  {
    buffdata = buff[index];
    if (buffdata < 16)
    {
      Monitorport.print(F("0"));
    }
    Monitorport.print(buffdata, HEX);
    Monitorport.print(F(" "));
  }
#endif
}


void SDprintReliableStatus()
{

#ifdef ENABLEMONITOR

  uint8_t reliableErrors = LoRa.readReliableErrors();
  uint8_t reliableFlags = LoRa.readReliableFlags();

  if (bitRead(reliableErrors, ReliableCRCError))
  {
    Monitorport.print(F(",ReliableCRCError"));
  }

  if (bitRead(reliableErrors, ReliableIDError))
  {
    Monitorport.print(F(",ReliableIDError"));
  }

  if (bitRead(reliableErrors, ReliableSizeError))
  {
    Monitorport.print(F(",ReliableSizeError"));
  }

  if (bitRead(reliableErrors, ReliableACKError))
  {
    Monitorport.print(F(",NoReliableACK"));
  }

  if (bitRead(reliableErrors, ReliableTimeout))
  {
    Monitorport.print(F(",ReliableTimeout"));
  }

  if (bitRead(reliableFlags, ReliableACKSent))
  {
    Monitorport.print(F(",ACKsent"));
  }

  if (bitRead(reliableFlags, ReliableACKReceived))
  {
    Monitorport.print(F(",ACKreceived"));
  }
#endif
}


void SDprintPacketHex()
{
#ifdef ENABLEMONITOR
  uint8_t packetlen = LoRa.readRXPacketL();
  Monitorport.print(packetlen);
  Monitorport.print(F(" bytes > "));
  if (packetlen > 0)
  {
    LoRa.printSXBufferHEX(0, packetlen - 1);
  }
#endif
}


void setCursor(uint8_t lcol, uint8_t lrow)
{
  disp.setCursor((lcol * 6 * textscale), (lrow * 9 * textscale));
}
