/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - - 10/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

uint8_t RXPacketL;                         //length of received packet
uint8_t RXPacketType;                      //type of received packet, segment write, ACK, NACK etc
uint16_t RXErrors;                         //count of packets received with error
uint8_t RXFlags;                           //DTflags byte in header, could be used to control actions in TX and RX
uint8_t RXHeaderL;                         //length of header
uint8_t RXDataarrayL;                      //length of data array\segment
int16_t PacketRSSI;                        //stores RSSI of received packet
int8_t  PacketSNR;                         //stores signal to noise ratio of received packet

uint16_t TXNetworkID;                      //this is used to store the 'network' number, receiver must have the same networkID
uint16_t TXArrayCRC;                       //should contain CRC of data array sent
uint8_t  TXPacketL;                        //length of transmitted packet

uint16_t AckCount;                         //keep a count of acks that are received within timeout period
uint16_t NoAckCount;                       //keep a count of acks not received within timeout period
uint16_t LocalPayloadCRC;                  //for calculating the local data array CRC

uint16_t DTDestinationFileCRC;             //CRC of file received
uint16_t DTSourceFileCRC;                  //CRC of returned of the remote saved file

uint32_t DTDestinationFileLength;          //length of file written on this destination
uint32_t DTSourceFileLength;               //length of file at source

uint32_t DTStartmS;                        //used for timeing transfers
bool DTFileOpened;                         //bool to record when file has been opened

uint16_t DTSegment = 0;                    //current segment number
uint16_t DTSegmentNext;                    //next segment expected
uint16_t DTReceivedSegments;               //count of segments received
uint16_t DTSegmentLast;                    //last segment processed
uint8_t DTLastSegmentSize;                 //size of the last segment
uint16_t DTNumberSegments;                 //number of segments for a file transfer
uint16_t DTSentSegments;                   //count of segments sent

bool DTFileTransferComplete;               //bool to flag file transfer complete
uint32_t DTSendmS;                         //used for timing transfers
float DTsendSecs;                          //seconds to transfer a file

char DTfilenamebuff[DTfilenamesize];

int DTLED = -1;                            //pin number for indicator LED, if -1 then not used

bool sendFile(char *DTFileName, uint8_t namelength);
bool sendFileSegment(uint16_t segnum, uint8_t segmentsize);
bool startFileTransfer(char *buff, uint8_t filenamesize, uint8_t attempts);
bool endFileTransfer(char *buff, uint8_t filenamesize);
void build_DTFileOpenHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize);
void build_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum);
void build_DTFileCloseHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize);
void printLocalFileDetails();
bool sendSegments();
void printheader(uint8_t *hdr, uint8_t hdrsize);
void printSeconds();
void printAckBrief();
void printAckDetails();
void printdata(uint8_t *dataarray, uint8_t arraysize);
void printACKdetail();
void printPacketHex();
void printPacketRSSI();
void printPacketDetails();
void readHeaderDT();
void printSourceFileDetails();
void printDestinationFileDetails();
bool processFileClose();
bool processFileOpen(uint8_t *buff, uint8_t filenamesize);
bool processSegmentWrite();
bool processPacket(uint8_t packettype);
bool receiveaPacketDT();
uint16_t getNumberSegments(uint32_t filesize, uint8_t segmentsize);
uint8_t getLastSegmentSize(uint32_t filesize, uint8_t segmentsize);
void setDTLED(int8_t pinnumber);

uint8_t DTheader[16];                        //header array
uint8_t DTdata[245];                         //data/segment array

bool sendFile(char *DTFileName, uint8_t namelength)
{
  //**************************************************************************************************************
  // Start filesend process
  // This routine allows the file transfer to be run with a function call of sendFile(FileName, sizeof(FileName));
  //**************************************************************************************************************

  do
  {

    DTStartmS = millis();

    //opens the local file to send and sets up transfer parameters
    if (startFileTransfer(DTFileName, namelength, DTSendAttempts))
    {
      Serial.print(DTFileName);
      Serial.println(F(" opened OK on remote"));
      printLocalFileDetails();
      Serial.println();
      NoAckCount = 0;
    }
    else
    {
      Serial.print(DTFileName);
      Serial.println(F("  Error opening remote file - restart transfer"));
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

    if (endFileTransfer(DTFileName, namelength))         //send command to close remote file
    {
      DTSendmS = millis() - DTStartmS;                  //record time taken for transfer
      Serial.print(DTFileName);
      Serial.println(F(" closed OK on remote"));
      beginarrayRW(DTheader, 4);
      DTDestinationFileLength = arrayReadUint32();
      Serial.print(F("Acknowledged remote destination file length "));
      Serial.println(DTDestinationFileLength);
      if (DTDestinationFileLength != DTSourceFileLength)
      {
        Serial.println(F("ERROR - file lengths do not match"));
      }
      else
      {
        Serial.println(F("File lengths match"));
      }
      DTFileTransferComplete = true;
    }
    else
    {
      DTFileTransferComplete = false;
      Serial.println(F("ERROR send close remote destination file failed - program halted"));
    }
  }
  while (!DTFileTransferComplete);

  Serial.print(F("NoAckCount "));
  Serial.println( NoAckCount);
  Serial.println();
  DTsendSecs = (float) DTSendmS / 1000;
  Serial.print(F("Transmit time "));
  Serial.print(DTsendSecs, 3);
  Serial.println(F("secs"));
  Serial.print(F("Transmit rate "));
  Serial.print( (DTDestinationFileLength * 8) / (DTsendSecs), 0 );
  Serial.println(F("bps"));
  Serial.println(("Transfer finished"));
  return true;
}


bool sendFileSegment(uint16_t segnum, uint8_t segmentsize)
{
  //****************************************************************
  //Send file segment as payload in a packet
  //****************************************************************

  uint8_t ValidACK;

  build_DTSegmentHeader(DTheader, DTSegmentWriteHeaderL, segmentsize, segnum);

#ifdef PRINTSEGMENTNUM
  //Serial.print(F("Segment,"));
  Serial.println(segnum);
#endif

#ifdef DEBUG
  printheader(DTheader, DTSegmentWriteHeaderL);
  Serial.print(F(" "));
  printdata(DTdata, segmentsize);                         //print segment size of data array only
#endif

  do
  {
    digitalWrite(DTLED, HIGH);
    TXPacketL = LoRa.transmitDT(DTheader, DTSegmentWriteHeaderL, (uint8_t *) DTdata, segmentsize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    digitalWrite(DTLED, LOW);

    if (TXPacketL == 0)                                    //if there has been an error TXPacketL returns as 0
    {
      Serial.println(F("Transmit error"));
    }

    ValidACK = LoRa.waitACKDT(DTheader, DTSegmentWriteHeaderL, ACKsegtimeoutmS);
    RXPacketType = DTheader[0];

    if (ValidACK > 0)
    {
      if (RXPacketType == DTSegmentWriteNACK)
      {
        DTSegment = DTheader[4] +  (DTheader[5] << 8);      //load what the segment number should be
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
      }

      //ack is valid, segment was acknowledged if here

      if (RXPacketType == DTStartNACK)
      {
        Serial.println(F("Received DTStartNACK "));
        return false;
      }

      if (RXPacketType == DTSegmentWriteACK)
      {
        AckCount++;
#ifdef DEBUG
        printAckBrief();
        //printAckDetails()
#endif
        DTSegment++;                  //increase value for next segment
        return true;
      }
    }
    else
    {
      NoAckCount++;
      Serial.print(F("Error no ACK received "));
      Serial.println(NoAckCount);
      if (NoAckCount > NoAckCountLimit)
      {
        Serial.println(F("ERROR NoACK limit reached"));
        return false;
      }
    }
  } while (ValidACK == 0);

  return true;
}


bool startFileTransfer(char *buff, uint8_t filenamesize, uint8_t attempts)
{
  //*********************************************************************
  //Start file transfer, simulate open local file first then remote file.
  //*********************************************************************

  uint8_t ValidACK;

  Serial.print(F("Start file transfer "));
  Serial.println(buff);

  DTSourceFileLength = DTFileSize;                             //get the file length

  if (DTSourceFileLength == 0)
  {
    Serial.print(F("Error opening local file "));
    Serial.println(buff);
    return false;
  }

  DTNumberSegments = getNumberSegments(DTSourceFileLength, DTSegmentSize);
  DTLastSegmentSize = getLastSegmentSize(DTSourceFileLength, DTSegmentSize);

  build_DTFileOpenHeader(DTheader, DTFileOpenHeaderL, filenamesize, DTSourceFileLength, DTSourceFileCRC, DTSegmentSize);
  LocalPayloadCRC = LoRa.CRCCCITT((uint8_t *) buff, filenamesize, 0xFFFF);

  do
  {
    Serial.println(F("Send open remote file request"));
    digitalWrite(DTLED, HIGH);
    TXPacketL = LoRa.transmitDT(DTheader, DTFileOpenHeaderL, (uint8_t *) buff, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    digitalWrite(DTLED, LOW);

#ifdef DEBUG
    Serial.print(F("Send attempt "));
    Serial.println(DTSendAttempts - attempts + 1);
#endif

    attempts--;
    TXNetworkID = LoRa.getTXNetworkID(TXPacketL);     //get the networkID appended to packet
    TXArrayCRC = LoRa.getTXPayloadCRC(TXPacketL);     //get the payload CRC appended to packet

#ifdef DEBUG
    Serial.print(F("TXNetworkID,0x"));
    Serial.print(TXNetworkID, HEX);                   //get the NetworkID of the packet just sent, its placed at the packet end
    Serial.print(F(",TXarrayCRC,0x"));
    Serial.println(TXArrayCRC, HEX);                  //get the CRC of the data array just sent, its placed at the packet end
    Serial.println();
#endif

    if (TXPacketL == 0)                               //if there has been a send and ack error, TXPacketL returns as 0
    {
      Serial.println(F("Transmit error"));
    }

    Serial.print(F("Wait ACK "));
    ValidACK = LoRa.waitACKDT(DTheader, DTFileOpenHeaderL, ACKopentimeoutmS);
    RXPacketType = DTheader[0];

    if ((ValidACK > 0) && (RXPacketType == DTFileOpenACK))
    {
#ifdef DEBUG
      printPacketHex();
#endif
    }
    else
    {
      NoAckCount++;
      Serial.println(F("No ACK received"));
      Serial.println(NoAckCount);
      if (NoAckCount > NoAckCountLimit)
      {
        Serial.println(F("ERROR NoACK limit reached"));
        return false;
      }

#ifdef DEBUG
      printACKdetail();
      Serial.print(F("ACKPacket "));
      printPacketHex();
#endif
      Serial.println();
    }
    Serial.println();
  }
  while ((ValidACK == 0) && (attempts != 0));

  if (attempts == 0)
  {
    return false;
  }

  return true;
}


bool endFileTransfer(char *buff, uint8_t filenamesize)
{
  //****************************************************************
  //End file transfer, close local file first then remote file.
  //****************************************************************

  uint8_t ValidACK;

  build_DTFileCloseHeader(DTheader, DTFileCloseHeaderL, filenamesize, DTSourceFileLength, DTSourceFileCRC, DTSegmentSize);

  do
  {
    printSeconds();
    Serial.println(F("Send close remote file"));

    digitalWrite(DTLED, HIGH);
    TXPacketL = LoRa.transmitDT(DTheader, DTFileCloseHeaderL, (uint8_t *) buff, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    digitalWrite(DTLED, LOW);

    TXNetworkID = LoRa.getTXNetworkID(TXPacketL);
    TXArrayCRC = LoRa.getTXPayloadCRC(TXPacketL);

#ifdef DEBUG
    Serial.print(F("TXNetworkID,0x"));
    Serial.print(TXNetworkID, HEX);               //get the NetworkID of the packet just sent, its placed at the packet end
    Serial.print(F(",TXarrayCRC,0x"));
    Serial.println(TXArrayCRC, HEX);              //get the CRC of the data array just sent, its placed at the packet end
    Serial.println();
#endif

    if (TXPacketL == 0)                           //if there has been a send and ack error, RXPacketL returns as 0
    {
      Serial.println(F("Transmit error"));
    }

    ValidACK = LoRa.waitACKDT(DTheader, DTFileCloseHeaderL, ACKclosetimeoutmS);
    RXPacketType = DTheader[0];

    if ((ValidACK > 0) && (RXPacketType == DTFileCloseACK))
    {
#ifdef DEBUG
      printPacketHex();
#endif
    }
    else
    {
      NoAckCount++;
      Serial.print(F("Error no ACK received "));
      Serial.println(NoAckCount);
      if (NoAckCount > NoAckCountLimit)
      {
        Serial.println(F("ERROR NoACK limit reached"));
        return false;
      }

#ifdef DEBUG
      Serial.println();
      Serial.print(F("ACKPacket "));
      printPacketHex();
      Serial.println();
#endif
    }
  }
  while (ValidACK == 0);

  return true;
}


void build_DTFileOpenHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize)
{
  //this builds the header buffer for the filename to send

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTFileOpen);         //byte 0, write the packet type
  arrayWriteUint8(0);                  //byte 1, initial DTflags byte, not used here
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(filelength);        //byte 4,5,6,7, write the file length
  arrayWriteUint16(filecrc);           //byte 8, 9, write file CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


void build_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum)
{
  //this builds the header buffer for a segment transmit

  beginarrayRW(header, 0);                //start writing to array at location 0
  arrayWriteUint8(DTSegmentWrite);        //write the packet type
  arrayWriteUint8(0);                     //initial DTflags byte, not used here
  arrayWriteUint8(headersize);            //write length of header
  arrayWriteUint8(datalen);               //write length of data array
  arrayWriteUint16(segnum);               //write the DTsegment number
  endarrayRW();
}


void build_DTFileCloseHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t filelength, uint16_t filecrc, uint8_t segsize)
{
  //this builds the header buffer for the filename passed

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTFileClose);        //byte 0, write the packet type
  arrayWriteUint8(0);                  //byte 1, initial DTflags byte, not used here
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(filelength);        //byte 4,5,6,7, write the file length
  arrayWriteUint16(filecrc);           //byte 8, 9, write file CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


void printLocalFileDetails()
{
  Serial.print(F("Source file length is "));
  Serial.print(DTSourceFileLength);
  Serial.println(F(" bytes"));
  Serial.print(F("Segment Size "));
  Serial.println(DTSegmentSize);
  Serial.print(F("Number segments "));
  Serial.println(DTNumberSegments);
  Serial.print(F("Last segment size "));
  Serial.println(DTLastSegmentSize);
}


bool sendSegments()
{
  //start the file transfer at segment 0
  DTSegment = 0;
  DTSentSegments = 0;

  while (DTSegment < (DTNumberSegments - 1))
  {
#ifdef DEBUG
    printSeconds();
#endif

    if (sendFileSegment(DTSegment, DTSegmentSize))
    {
      DTSentSegments++;
    }
    else
    {
      Serial.println(F("ERROR in sendFileSegment"));
      Serial.println();
      return false;
    }

    delay(packetdelaymS);
  };

  printSeconds();
  Serial.println(F("Last segment"));

  if (!sendFileSegment(DTSegment, DTLastSegmentSize))
  {
    Serial.println(F("ERROR in sendFileSegment"));
    return false;
  }

  return true;
}


void printheader(uint8_t *hdr, uint8_t hdrsize)
{
  Serial.print(F("HeaderBytes,"));
  Serial.print(hdrsize);
  Serial.print(F(" "));
  printarrayHEX(hdr, hdrsize);
}


void printSeconds()
{
  float secs;
  secs = ( (float) millis() / 1000);
  Serial.print(secs, 2);
  Serial.print(F(" "));
}


void printdata(uint8_t *dataarray, uint8_t arraysize)
{
  Serial.print(F("DataBytes,"));
  Serial.print(arraysize);
  Serial.print(F("  "));
  printarrayHEX((uint8_t *) dataarray, 16);             //There is a lot of data to print so only print first 16 bytes
}


void printAckDetails()
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


void printAckBrief()
{
  PacketRSSI = LoRa.readPacketRSSI();
  Serial.print(F(",AckRSSI,"));
  Serial.print(PacketRSSI);
  Serial.println(F("dBm"));
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


void printPacketDetails()
{
  PacketRSSI = LoRa.readPacketRSSI();
  PacketSNR = LoRa.readPacketSNR();
  Serial.print(F(" RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm"));

#ifdef DEBUG
  Serial.print(F(",SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dBm,RXOKCount,"));
  Serial.print(DTReceivedSegments);
  Serial.print(F(",RXErrs,"));
  Serial.print(RXErrors);
  Serial.print(F(" RX"));
  printheader(DTheader, RXHeaderL);
#endif
}


void printPacketRSSI()
{
  PacketRSSI = LoRa.readPacketRSSI();
  Serial.print(F(" RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm"));
}

void readHeaderDT()
{
  //the first 6 bytes of the header contain the important stuff, so load it up
  //so we can decide what to do next.
  beginarrayRW(DTheader, 0);                      //start buffer read at location 0
  RXPacketType = arrayReadUint8();                //load the packet type
  RXFlags = arrayReadUint8();                     //initial DTflags byte, not used here
  RXHeaderL = arrayReadUint8();                   //load the header length
  RXDataarrayL = arrayReadUint8();                //load the datalength
  DTSegment = arrayReadUint16();                  //load the segment number
}


void printSourceFileDetails()
{
  Serial.print(DTfilenamebuff);
  Serial.print(F(" Source file length is "));
  Serial.print(DTSourceFileLength);
  Serial.println(F(" bytes"));
  Serial.print(DTfilenamebuff);
}


void printDestinationFileDetails()
{
  Serial.print(F("Destination file length is "));
  Serial.print(DTDestinationFileLength);
  Serial.println(F(" bytes"));
  if (DTDestinationFileLength != DTSourceFileLength)
  {
    Serial.println(F("ERROR - file lengths do not match"));
  }
  else
  {
    Serial.println(F("File lengths match"));
  }
}


bool processFileClose()
{
  //***********************************************************************************************
  // Code for closing local SD file and sending request to remote
  //***********************************************************************************************

  Serial.print(F("File close for "));
  Serial.println((char*) DTfilenamebuff);

  if (DTFileOpened)                                     //check if file has been opened, close it if it is
  {
    if (1)                                              //simulate file exists
    {

      Serial.print(F("Transfer time "));
      Serial.print(millis() - DTStartmS);
      Serial.print(F("mS"));
      Serial.println();
      Serial.println(F("File closed"));
      Serial.println();

      DTFileOpened = false;
      DTDestinationFileLength = DTFileSize;
      beginarrayRW(DTheader, 4);                       //start writing to array at location 12
      arrayWriteUint32(DTDestinationFileLength);       //write file length of file just written just written to ACK header
      arrayWriteUint16(DTDestinationFileCRC);          //write CRC of file just written to ACK header

      printDestinationFileDetails();
    }
  }

  delay(ACKdelaymS);
#ifdef DEBUG
  Serial.println(F("Sending ACK"));
#endif
  DTheader[0] = DTFileCloseACK;

  digitalWrite(DTLED, HIGH);
  LoRa.sendACKDT(DTheader, DTFileCloseHeaderL, TXpower);
  digitalWrite(DTLED, LOW);

  Serial.println();
  Serial.println();
  return true;
}


bool processFileOpen(uint8_t *buff, uint8_t filenamesize)
{
  //*************************************************************************
  // Code for simulating opening local SD file and sending request to remote
  //*************************************************************************

  beginarrayRW(DTheader, 4);                      //start buffer read at location 4
  DTSourceFileLength = arrayReadUint32();         //load the file length of the remote file being sent
  DTSourceFileCRC = arrayReadUint16();            //load the CRC of the source file being sent

  memset(DTfilenamebuff, 0, DTfilenamesize);      //clear DTfilenamebuff to all 0s
  memcpy(DTfilenamebuff, buff, filenamesize);     //copy received DTdata into DTfilenamebuff

  Serial.print(F("File Open request for "));
  Serial.print((char*) DTfilenamebuff);
  Serial.println();
  printSourceFileDetails();

  if (1)                                          //simulate open file for write at beginning, delete if it exists
  {
    Serial.print((char*) DTfilenamebuff);
    Serial.println(F(" File Opened OK"));
    Serial.println(F("Waiting transfer"));
    DTSegmentNext = 0;                            //since file is opened the next sequence should be the first
    DTFileOpened = true;
    DTStartmS = millis();
  }
  else
  {
    Serial.print((char*) DTfilenamebuff);
    Serial.println(F(" File Open fail"));
    DTFileOpened = false;
    return false;
  }

  DTStartmS = millis();
  delay(ACKdelaymS);
#ifdef DEBUG
  Serial.println(F("Sending ACK"));
#endif
  DTheader[0] = DTFileOpenACK;                //set the ACK packet type
  digitalWrite(DTLED, HIGH);
  LoRa.sendACKDT(DTheader, DTFileOpenHeaderL, TXpower);
  digitalWrite(DTLED, LOW);
  DTSegmentNext = 0;                          //after file open, segment 0 is next

  return true;
}


bool processSegmentWrite()
{
  //***********************************************************************************************
  // Code for dealing with segment writes   - checks that the sequence of segment writes is correct
  //***********************************************************************************************

  if (!DTFileOpened)
  {
    //something is wrong, have received a request to write a segment but there is no file opened
    //need to reject the segment write with a restart NACK
    Serial.println();
    Serial.println(F("***************************************************"));
    Serial.println(F("Error - Segment write with no file open - send NACK"));
    Serial.println(F("***************************************************"));
    Serial.println();
    DTheader[0] = DTStartNACK;
    delay(ACKdelaymS);
    digitalWrite(DTLED, HIGH);
    LoRa.sendACKDT(DTheader, DTStartHeaderL, TXpower);
    digitalWrite(DTLED, LOW);
    return false;
  }

  if (DTSegment == DTSegmentNext)
  {

#ifdef PRINTSEGMENTNUM
    //Serial.print(F("Segment,"));
    Serial.println(DTSegment);
#endif

#ifdef DEBUG
    Serial.print(F(",Bytes,"));
    Serial.print(RXDataarrayL);
    //printPacketDetails();
    printPacketRSSI();
    Serial.println(F(" SendACK"));
#endif

    DTheader[0] = DTSegmentWriteACK;
    delay(ACKdelaymS);
    digitalWrite(DTLED, HIGH);
    LoRa.sendACKDT(DTheader, DTSegmentWriteHeaderL, TXpower);
    digitalWrite(DTLED, LOW);
    DTReceivedSegments++;
    DTSegmentLast = DTSegment;                  //so we can tell if sequece has been received twice
    DTSegmentNext = DTSegment + 1;
    return true;
  }

  if (DTSegment == DTSegmentLast)
  {
    Serial.print(F("ERROR segment "));
    Serial.print(DTSegment);
    Serial.print(F(" already received "));
    //printPacketDetails();
    printPacketRSSI();
    DTheader[0] = DTSegmentWriteACK;
    Serial.print(F(" Send ACK"));
    delay(ACKdelaymS);
    delay(DuplicatedelaymS);
    digitalWrite(DTLED, HIGH);
    LoRa.sendACKDT(DTheader, DTSegmentWriteHeaderL, TXpower);
    digitalWrite(DTLED, LOW);
    return true;
  }

  if (DTSegment != DTSegmentNext )
  {
    Serial.print(F(" ERROR Received Segment "));
    Serial.print(DTSegment);
    Serial.print(F(" expected "));
    Serial.print(DTSegmentNext);
    Serial.print(F(" "));
    //printPacketDetails();
    printPacketRSSI();
    DTheader[0] = DTSegmentWriteNACK;
    DTheader[4] = lowByte(DTSegmentNext);
    DTheader[5] = highByte(DTSegmentNext);

    Serial.print(F(" Send NACK for segment "));
    Serial.print(DTSegmentNext);
    delay(ACKdelaymS);
    delay(DuplicatedelaymS);
    digitalWrite(DTLED, HIGH);

    Serial.println();
    Serial.println();
    Serial.println(F("*****************************************"));
    Serial.print(F("Transmit restart request for segment "));
    Serial.println(DTSegmentNext);
    printheader(DTheader, RXHeaderL);
    Serial.println();
    Serial.println(F("*****************************************"));
    Serial.println();
    Serial.flush();

    LoRa.sendACKDT(DTheader, DTSegmentWriteHeaderL, TXpower);
    digitalWrite(DTLED, LOW);
    return false;
  }

  return true;
}


bool processPacket(uint8_t packettype)
{
  //***********************************************************************************************
  // Decide what to do with an incoming packet
  //***********************************************************************************************

  if (packettype == DTSegmentWrite)
  {
    processSegmentWrite();
    return true;
  }

  if (packettype == DTFileOpen)
  {
    processFileOpen(DTdata, RXDataarrayL);
    return true;
  }

  if (packettype == DTFileClose)
  {
    processFileClose();
    return true;
  }

  return true;
}


bool receiveaPacketDT()
{
  //******************************
  //Receive Data transfer packets
  //******************************

  RXPacketType = 0;

  RXPacketL = LoRa.receiveDT(DTheader, HeaderSizeMax, (uint8_t *) DTdata, DataSizeMax, NetworkID, RXtimeoutmS, WAIT_RX);
  digitalWrite(DTLED, HIGH);

#ifdef DEBUG
  printSeconds();
#endif

  if (RXPacketL > 0)
  {
    //if the LT.receiveDT() returns a value > 0 for RXPacketL then packet was received OK
    //then only action payload if destinationNode = thisNode
    readHeaderDT();                      //get the basic header details into global variables RXPacketType etc
    processPacket(RXPacketType);         //process and act on the packet
    digitalWrite(DTLED, LOW);
    return true;
  }
  else
  {
    //if the LoRa.receiveDT() function detects an error RXOK is 0
    Serial.print(F("PacketError"));
    RXErrors++;
    printPacketDetails();
#ifdef DEBUG
    LoRa.printReliableStatus();
    LoRa.printIrqStatus();
#endif
    Serial.println();
    digitalWrite(DTLED, LOW);
    return false;
  }
}


uint16_t getNumberSegments(uint32_t filesize, uint8_t segmentsize)
{
  uint16_t segments;
  segments = filesize / segmentsize;
  if ((filesize % segmentsize) > 0)
  {
    segments++;
  }
  return segments;
}


uint8_t getLastSegmentSize(uint32_t filesize, uint8_t segmentsize)
{
  uint8_t lastsize;
  lastsize = filesize % segmentsize;
  if (lastsize == 0)
  {
    lastsize = segmentsize;
  }
  return lastsize;
}


void setDTLED(int8_t pinnumber)
{
  if (pinnumber >= 0)
  {
    DTLED = pinnumber;
    pinMode(pinnumber, OUTPUT);
  }
}
