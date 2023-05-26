/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 01/02/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation -  This is a version of data tranfer functions that will transfer an array in memory
  across to the receiver which may receive the array directly or store the received segments to an SD
  card. These array transfer functions assume that ultimatly the array will be saved as a file somewhere,
  so a filename needs to be provided, even when it may not be used.

  These functions expect the calling sketch to create an instance called LoRa, so that SX12XX library
  functions are called like this; LoRa.getARTXNetworkID().

  To receive the array use a program from the SX12XX LoRa library such as 234_SDfile_Transfer_Receiver or
  239_StuartCAM_LoRa_Receiver.

  Updated 18/01/22 to allow the port for Monitorport debug prints to be changed, all prints now goto
  Monitorport. The actual port for prints is defined like this;

  #define Monitorport Serialx

*******************************************************************************************************/

//110122 added local function ARprintArrayHEX(uint8_t *buff, uint32_t len)
//110122 Replace printpacketHEX() with ARprintArrayHEX() which used  LoRa.printSXBufferHEX() and Monitorport.print()
//180122 preface all variables and functions with AR so that functions can be used with SD transfer functions
//180122 allow the port for Monitorport debug prints to be changed
//010222 Added ARsendDTInfo functions

//so that Monitorport prints default to the primary Monitorport port of Monitorport
#ifndef Monitorport
#define Monitorport Serial
#endif

#define ARUNUSED(v) (void) (v)               //add ARUNUSED(variable); to avoid compiler warnings 

//#define DEBUG                              //enable this define to show data transfer debug info
#include <arrayRW.h>                         //part of SX12XX library


//Variables used on transmitter and receiver
uint8_t ARRXPacketL;                         //length of received packet
uint8_t ARRXPacketType;                      //type of received packet, segment write, ACK, NACK etc
uint8_t ARRXHeaderL;                         //length of header
int16_t ARPacketRSSI;                        //stores RSSI of received packet
int8_t  ARPacketSNR;                         //stores signal to noise ratio of received packet
uint16_t ARAckCount;                         //keep a count of acks that are received within timeout period
uint16_t ARNoAckCount;                       //keep a count of acks not received within timeout period
uint16_t ARDTSourceArrayCRC;                 //CRC returned of the remote received array
uint32_t ARDTSourceArrayLength;              //length of file at source\transmitter
uint16_t ARDTDestinationArrayCRC;            //CRC of complete array received
uint32_t ARDTDestinationArrayLength;         //length of file\array written on the destination\receiver
uint32_t ARDTStartmS;                        //used for timeing transfers
uint16_t ARDTSegment = 0;                    //current segment number
char ARDTfilenamebuff[ARDTfilenamesize];     //global buffer to store filename
uint8_t ARDTheader[16];                      //header array
uint8_t ARDTdata[245];                       //data/segment array
uint8_t ARDTflags = 0;                       //Flags byte used to pass status information between nodes
int8_t ARDTLED = -1;                         //pin number for indicator LED, if -1 then not used
uint16_t ARDTErrors;                         //used for tracking errors in the transfer process
bool ARDTArrayTransferComplete;              //bool to flag array transfer complete
uint32_t ARDTSendmS;                         //used for timing transfers
float ARDTsendSecs;                          //seconds to transfer a file
uint16_t ARDTNumberSegments;                 //number of segments for a file transfer
uint8_t ARDTLastSegmentSize;                 //size of the last segment
uint16_t ARLocalPayloadCRC;                  //for calculating the local data array CRC
uint8_t  ARTXPacketL;                        //length of transmitted packet
uint16_t ARTXNetworkID;                      //this is used to store the 'network' number, receiver must have the same
uint16_t ARTXArrayCRC;                       //should contain CRC of data array transmitted
uint16_t ARDTSentSegments;                   //count of segments sent

//Receive mode only variables
uint16_t ARRXErrors;                         //count of packets received with error
uint8_t ARRXFlags;                           //ARDTflags byte in header, could be used to control actions in TX and RX
uint8_t ARRXDataarrayL;                      //length of data array\segment
bool ARDTArrayStarted;                       //bool to flag when array write has started
bool ARDTArrayEnded;                         //bool to flag when array write has finished
bool ARDTArrayTimeout;                       //set true when there is a timeout waiting for transfer
uint16_t ARDTSegmentNext;                    //next segment expected
uint16_t ARDTReceivedSegments;               //count of segments received
uint16_t ARDTSegmentLast;                    //last segment processed

//A global pointer to the array and a variable for its length and current location are used so that all routines
//have access to the array to send without constantly passing the array pointer and variables between functions.
uint8_t *ptrARsendArray;                     //create a global pointer to the array to send, so all functions have access
uint8_t *ptrARreceivearray;                  //create a global pointer to the array to receive into, so all functions have access
uint32_t ARArrayLength;                      //length of array to send or receive
uint32_t ARarraylocation;                    //a global variable giving the location in the array last used

//Transmitter mode functions
bool ARsendArray(uint8_t *ptrarray, uint32_t arraylength, char *filename, uint8_t namelength);
bool ARstartArrayTransfer(char *buff, uint8_t filenamesize);
bool ARsendSegments();
bool ARsendArraySegment(uint16_t segnum, uint8_t segmentsize);
bool ARendArrayTransfer(char *buff, uint8_t filenamesize);
void ARbuild_DTArrayStartHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t arraylength, uint16_t arraycrc, uint8_t segsize);
void ARbuild_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum);
void ARbuild_DTArrayEndHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t arraylength, uint16_t arraycrc, uint8_t segsize);
void ARprintLocalArrayDetails();
void ARprintSeconds();
void ARprintAckBrief();
void ARprintAckReception();
void ARprintACKdetail();
void ARprintdata(uint8_t *dataarray, uint8_t arraysize);
uint16_t ARgetNumberSegments(uint32_t arraysize, uint8_t segmentsize);
uint8_t ARgetLastSegmentSize(uint32_t arraysize, uint8_t segmentsize);
bool ARsendDTInfo();
void ARbuild_DTInfoHeader(uint8_t *header, uint8_t headersize, uint8_t datalen);

//Receiver mode functions
uint32_t ARreceiveArray(uint8_t *arraychar, uint32_t length, uint32_t receivetimeout);
bool ARreceivePacketDT();
void ARreadHeaderDT();
bool ARprocessPacket(uint8_t packettype);
bool ARprocessSegmentWrite();
bool ARprocessArrayStart(uint8_t *buff, uint8_t filenamesize);
bool ARprocessArrayEnd();
void ARprintSourceArrayDetails();
void ARprintDestinationArrayDetails();
uint16_t ARarrayCRC(uint8_t *buffer, uint16_t size, uint16_t startvalue);

//Common functions
void ARsetDTLED(int8_t pinnumber);
void ARprintheader(uint8_t *hdr, uint8_t hdrsize);
void ARprintArrayHEX(uint8_t *buff, uint32_t len);
void ARprintReliableStatus();
void ARprintPacketDetails();

//bit numbers used by ATDTErrors (16bits) and RXErrors (first 8bits)
const uint8_t ARNoFileSave = 0;              //bit number of ATDTErrors to set when no file save, to SD for example
const uint8_t ARNothingToSend = 1;           //bit number of ATDTErrors to set when nothing to send or unable to send image\file
const uint8_t ARNoCamera = 1;                //bit number of ATDTErrors to set when camera fails
const uint8_t ARSendFile = 2;                //bit number of ATDTErrors to set when file SD file image\file send fail
const uint8_t ARSendArray = 2;               //bit number of ATDTErrors to set when file array image\file send fail
const uint8_t ARNoACKlimit = 3;              //bit number of ATDTErrors to set when NoACK limit reached
const uint8_t ARSendPacket = 4;              //bit number of ATDTErrors to set when sending a packet fails or there is no ack

const uint8_t ARStartTransfer = 11;          //bit number of ATDTErrors to set when StartTransfer fails
const uint8_t ARSendSegments = 12;           //bit number of ATDTErrors to set when SendSegments function fails
const uint8_t ARSendSegment = 13;            //bit number of ATDTErrors to set when sending a single Segment send fails
const uint8_t AROpeningFile = 14;            //bit number of ATDTErrors to set when opening file fails
const uint8_t ARendTransfer = 15;            //bit number of ATDTErrors to set when end transfer fails


//************************************************
//Transmit mode functions
//************************************************

bool ARsendArray(uint8_t *ptrarray, uint32_t arraylength, char *filename, uint8_t namelength)
{
  //This routine allows the array transfer to be run with a function call of ARsendArray().

  uint8_t localattempts = 0;
  memcpy(ARDTfilenamebuff, filename, namelength);    //copy the name of destination file into global filename array for use outside this function
  ptrARsendArray = ptrarray;                         //set global pointer to array pointer passed
  ARArrayLength = arraylength;                       // the length of array to send

  ARDTSourceArrayCRC = 0;
  ARDTSourceArrayLength = 0;
  ARDTDestinationArrayCRC = 0;
  ARDTDestinationArrayLength = 0;

  do
  {
    localattempts++;
    ARNoAckCount = 0;
    ARDTStartmS = millis();

#ifdef ENABLEMONITOR
    Monitorport.print(F("Send array attempt "));
    Monitorport.println(localattempts);
#endif

    if (ARstartArrayTransfer(filename, namelength))
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Array write started OK on remote"));
      ARprintLocalArrayDetails();
#endif
    }
    else
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("*************************"));
      Monitorport.println(F("ERROR writing to remote array"));
      Monitorport.println(F("Restarting transfer"));
      Monitorport.println(F("*************************"));
#endif

      ARDTArrayTransferComplete = false;
      continue;
    }

    delay(FunctionDelaymS);

    if (!ARsendSegments())
    {
#ifdef ENABLEMONITOR
      Monitorport.println();
      Monitorport.println(F("************************"));
      Monitorport.println(F("ERROR in sendSegments()"));
      Monitorport.println(F("Restarting transfer"));
      Monitorport.println(F("***********************"));
      Monitorport.println();
#endif

      ARDTArrayTransferComplete = false;
      continue;
    }

    delay(FunctionDelaymS);

    if (ARendArrayTransfer(filename, namelength))        //send command to end remote array write
    {
      ARDTSendmS = millis() - ARDTStartmS;                   //record time taken for transfer
      beginarrayRW(ARDTheader, 4);
      ARDTDestinationArrayLength = arrayReadUint32();

#ifdef ENABLEMONITOR
      Monitorport.println(F("Array write ended OK on remote"));
      Monitorport.print(F("Acknowledged remote destination file length "));
      Monitorport.println(ARDTDestinationArrayLength);
#endif

      if (ARDTDestinationArrayLength != ARDTSourceArrayLength)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("************************************************"));
        Monitorport.println(F("ERROR destination array and local array lengths do not match"));
        Monitorport.println(F("Restarting transfer"));
        Monitorport.println(F("************************************************"));
#endif

        ARDTArrayTransferComplete = false;
        continue;
      }
      else
      {

#ifdef ENABLEMONITOR
        Monitorport.println(F("Destination array and local array lengths match"));
#endif
      }
#ifdef ENABLEARRAYCRC
      ARDTDestinationArrayCRC = arrayReadUint16();

#ifdef ENABLEMONITOR
      Monitorport.print(F("Acknowledged destination array CRC 0x"));
      Monitorport.println(ARDTDestinationArrayCRC, HEX);
#endif

#endif
      if (ARDTDestinationArrayCRC != ARDTSourceArrayCRC)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("*********************************************"));
        Monitorport.println(F("ERROR destination array and local array CRCs do not match"));
        Monitorport.println(F("Restarting transfer"));
        Monitorport.println(F("*********************************************"));
#endif

        ARDTArrayTransferComplete = false;
        continue;
      }
      else
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("Destination array and local array CRCs match"));
#endif
      }
      ARDTArrayTransferComplete = true;
    }
    else
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("******************************"));
      Monitorport.println(F("ERROR ending destination array write"));
      Monitorport.println(F("Restarting transfer"));
      Monitorport.println(F("******************************"));
#endif

      ARDTArrayTransferComplete = false;
      continue;
    }
  }
  while ((!ARDTArrayTransferComplete) && (localattempts < StartAttempts));

  if (localattempts == StartAttempts)
  {
    bitSet(ARDTErrors, ARSendArray);
    return false;
  }

  ARDTsendSecs = (float) ARDTSendmS / 1000;

#ifdef ENABLEMONITOR
  Monitorport.print(F("ARNoAckCount "));
  Monitorport.println(ARNoAckCount);
  Monitorport.print(F("Transmit time "));
  Monitorport.print(ARDTsendSecs, 3);
  Monitorport.println(F("secs"));
  Monitorport.print(F("Transmit rate "));
  Monitorport.print( (ARDTDestinationArrayLength * 8) / (ARDTsendSecs), 0 );
  Monitorport.println(F("bps"));
  Monitorport.println(("Transfer finished"));
#endif

  return true;
}


bool ARstartArrayTransfer(char *buff, uint8_t filenamesize)
{
  //Start transfer of array to remote array or file
  uint8_t ValidACK;
  uint8_t localattempts = 0;

#ifdef ENABLEMONITOR
  Monitorport.print(F("Start array transfer for "));
  Monitorport.println(buff);
#endif

  ARDTSourceArrayLength = ARArrayLength;

  if (ARDTSourceArrayLength == 0)
  {
#ifdef ENABLEMONITOR
    Monitorport.print(F("Error - array 0 bytes "));
    Monitorport.println(buff);
#endif

    return false;
  }

#ifdef ENABLEARRAYCRC
  ARDTSourceArrayCRC = LoRa.CRCCCITT((uint8_t *) ptrARsendArray, ARArrayLength, 0xFFFF);       //get array CRC from position 0 to end
#endif

  ARDTNumberSegments = ARgetNumberSegments(ARDTSourceArrayLength, SegmentSize);
  ARDTLastSegmentSize = ARgetLastSegmentSize(ARDTSourceArrayLength, SegmentSize);
  ARbuild_DTArrayStartHeader(ARDTheader, DTArrayStartHeaderL, filenamesize, ARDTSourceArrayLength, ARDTSourceArrayCRC, SegmentSize);
  ARLocalPayloadCRC = LoRa.CRCCCITT((uint8_t *) buff, filenamesize, 0xFFFF);

  do
  {
    localattempts++;

#ifdef ENABLEMONITOR
    Monitorport.println(F("Send open remote file request"));
#endif

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }

    ARTXPacketL = LoRa.transmitDT(ARDTheader, DTArrayStartHeaderL, (uint8_t *) buff, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }

    ARTXNetworkID = LoRa.getTXNetworkID(ARTXPacketL);     //get the networkID appended to packet
    ARTXArrayCRC = LoRa.getTXPayloadCRC(ARTXPacketL);     //get the payload CRC thats was appended to packet

#ifdef ENABLEMONITOR
    Monitorport.print(F("Send attempt "));
    Monitorport.println(localattempts);

    if (ARTXPacketL == 0)                               //if there has been a send and ack error, ARTXPacketL returns as 0
    {
      Monitorport.println(F("Transmit error"));
    }
#endif

    ValidACK = LoRa.waitACKDT(ARDTheader, DTArrayStartHeaderL, ACKopentimeoutmS);
    ARRXPacketType = ARDTheader[0];

    if ((ValidACK > 0) && (ARRXPacketType == DTArrayStartACK))
    {
#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.println(F("Valid ACK > "));
      ARprintArrayHEX(ARDTheader, ValidACK);                   //ValidACK is packet length
#endif
#endif
    }
    else
    {
      ARNoAckCount++;

#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.println(F("NoACK"));
#endif
#endif

      if (ARNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
        Monitorport.println();
#endif

        bitSet(ARDTErrors, ARNoACKlimit);
        return false;
      }

    }
  }
  while ((ValidACK == 0) && (localattempts < SendAttempts));

  if (localattempts == SendAttempts)
  {
    bitSet(ARDTErrors, ARStartTransfer);
    return false;
  }

  return true;

}


bool ARsendSegments()
{
  //Start the array transfer at segment 0
  ARDTSegment = 0;
  ARDTSentSegments = 0;

  ARarraylocation = 0;                      //start at first position in array

  while (ARDTSegment < (ARDTNumberSegments - 1))
  {
#ifdef ENABLEMONITOR
#ifdef DEBUG
    ARprintSeconds();
#endif
#endif

    if (ARsendArraySegment(ARDTSegment, SegmentSize))
    {
      ARDTSentSegments++;
    }
    else
    {
      return false;
    }
    delay(FunctionDelaymS);
  };

#ifdef ENABLEMONITOR
  Monitorport.println(F("Last segment"));
#endif

  if (!ARsendArraySegment(ARDTSegment, ARDTLastSegmentSize))
  {
    return false;
  }

  return true;
}


bool ARsendArraySegment(uint16_t segnum, uint8_t segmentsize)
{
  //Send array segment as payload in a data transfer packet

  uint8_t ValidACK;
  uint8_t index;
  uint8_t tempdata;
  uint8_t localattempts = 0;

  for (index = 0; index < segmentsize; index++)
  {
    tempdata = ptrARsendArray[ARarraylocation];
    ARDTdata[index] = tempdata;
    ARarraylocation++;
  }

  ARbuild_DTSegmentHeader(ARDTheader, DTSegmentWriteHeaderL, segmentsize, segnum);

#ifdef ENABLEMONITOR
#ifdef PRINTSEGMENTNUM
  Monitorport.print(segnum);
#endif
#ifdef DEBUG
  Monitorport.print(F(" "));
  ARprintheader(ARDTheader, DTSegmentWriteHeaderL);
  Monitorport.print(F(" "));
  ARprintdata(ARDTdata, segmentsize);                           //print segment size of data array only
#endif
  Monitorport.println();
#endif

  do
  {
    localattempts++;
    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }

    ARTXPacketL = LoRa.transmitDT(ARDTheader, DTSegmentWriteHeaderL, (uint8_t *) ARDTdata, segmentsize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);
    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }

    if (ARTXPacketL == 0)                                     //if there has been an error ARTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
    }

    ValidACK = LoRa.waitACKDT(ARDTheader, DTSegmentWriteHeaderL, ACKsegtimeoutmS);
    ARRXPacketType = ARDTheader[0];

    if (ValidACK > 0)
    {
      if (ARRXPacketType == DTSegmentWriteNACK)
      {
        ARDTSegment = ARDTheader[4] +  (ARDTheader[5] << 8);      //load what the segment number should be
        ARRXHeaderL = ARDTheader[2];
        ARarraylocation = ARDTSegment * SegmentSize;

#ifdef ENABLEMONITOR
        Monitorport.println();
        Monitorport.println(F("************************************"));
        Monitorport.print(F("Received restart request at segment "));
        Monitorport.println(ARDTSegment);
#ifdef DEBUG
        ARprintheader(ARDTheader, ARRXHeaderL);
#endif
        Monitorport.println();
        Monitorport.print(F("Seek to array location "));
        Monitorport.println(ARDTSegment * SegmentSize);
        Monitorport.println(F("************************************"));
        Monitorport.println();
        Monitorport.flush();
#endif
      }

      if (ARRXPacketType == DTStartNACK)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("Received restart request"));
#endif

        return false;
      }

      //ack is valid, segment was acknowledged if here

      if (ARRXPacketType == DTSegmentWriteACK)
      {
        ARAckCount++;
        ARDTSegment++;                  //increase value for next segment
        return true;
      }
    }
    else
    {
      ARNoAckCount++;

#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.println(F("NoACK"));
#endif
#endif

      if (ARNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
#endif

        return false;
      }
    }
  } while ((ValidACK == 0) && (localattempts < SendAttempts)) ;

  if (localattempts == SendAttempts)
  {
    bitSet(ARDTErrors, ARSendSegment);
    return false;
  }

  return true;
}


bool ARendArrayTransfer(char *buff, uint8_t filenamesize)
{
  //End array transfer

  uint8_t ValidACK;
  uint8_t localattempts = 0;

  ARbuild_DTArrayEndHeader(ARDTheader, DTArrayEndHeaderL, filenamesize, ARDTSourceArrayLength, ARDTSourceArrayCRC, SegmentSize);

  do
  {
    localattempts++;
#ifdef ENABLEMONITOR
    Monitorport.println(F("Send end array write"));
#endif

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }

    ARTXPacketL = LoRa.transmitDT(ARDTheader, DTArrayEndHeaderL, (uint8_t *) buff, filenamesize, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }

    ARTXNetworkID = LoRa.getTXNetworkID(ARTXPacketL);
    ARTXArrayCRC = LoRa.getTXPayloadCRC(ARTXPacketL);

    if (ARTXPacketL == 0)                           //if there has been a send and ack error, ARARTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
    }

    ValidACK = LoRa.waitACKDT(ARDTheader, DTArrayEndHeaderL, ACKclosetimeoutmS);
    ARRXPacketType = ARDTheader[0];

    if ((ValidACK > 0) && (ARRXPacketType == DTArrayEndACK))
    {
#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.print(F("ACK header "));
      ARprintArrayHEX(ARDTheader, ValidACK);                   //ValidACK is packet length
      Monitorport.println();
#endif
#endif
    }
    else
    {
      ARNoAckCount++;

#ifdef ENABLEMONITOR
#ifdef DEBUG
      Monitorport.println(F("NoACK"));
#endif
#endif

      if (ARNoAckCount > NoAckCountLimit)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("ERROR NoACK limit reached"));
#endif
        return false;
      }

#ifdef ENABLEMONITOR
#ifdef DEBUG
      ARprintArrayHEX(ARDTheader, ValidACK);                   //ValidACK is packet length
#endif
      Monitorport.println();
#endif
    }
  }
  while ((ValidACK == 0) && (localattempts < SendAttempts)) ;


  if (localattempts == SendAttempts)
  {
    bitSet(ARDTErrors, ARSendSegment);
    return false;
  }
  return true;
}


void ARbuild_DTArrayStartHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t arraylength, uint16_t arraycrc, uint8_t segsize)
{
  //This builds the header buffer for the filename receiver might save the array as

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTArrayStart);       //byte 0, write the packet type
  arrayWriteUint8(ARDTflags);            //byte 1, ARDTflags byte
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(arraylength);       //byte 4,5,6,7, write the array length
  arrayWriteUint16(arraycrc);          //byte 8, 9, write array CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


void ARbuild_DTSegmentHeader(uint8_t *header, uint8_t headersize, uint8_t datalen, uint16_t segnum)
{
  //This builds the header buffer for a segment transmit

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTSegmentWrite);     //write the packet type
  arrayWriteUint8(ARDTflags);            //initial ARDTflags byte
  arrayWriteUint8(headersize);         //write length of header
  arrayWriteUint8(datalen);            //write length of data array
  arrayWriteUint16(segnum);            //write the DTsegment number
  endarrayRW();
}


void ARbuild_DTArrayEndHeader(uint8_t *header, uint8_t headersize, uint8_t datalength, uint32_t arraylength, uint16_t arraycrc, uint8_t segsize)
{
  //This builds the header buffer for the end remote array write

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTArrayEnd);         //byte 0, write the packet type
  arrayWriteUint8(ARDTflags);            //byte 1, initial ARDTflags byte
  arrayWriteUint8(headersize);         //byte 2, write length of header
  arrayWriteUint8(datalength);         //byte 3, write length of dataarray
  arrayWriteUint32(arraylength);       //byte 4,5,6,7, write the array length
  arrayWriteUint16(arraycrc);          //byte 8, 9, write array CRC
  arrayWriteUint8(segsize);            //byte 10, segment size
  arrayWriteUint8(0);                  //byte 11, unused byte
  endarrayRW();
}


void ARprintLocalArrayDetails()
{
#ifdef ENABLEMONITOR
  Monitorport.print(F("Source array length "));
  Monitorport.print(ARDTSourceArrayLength);
  Monitorport.println(F(" bytes"));
#ifdef ENABLEARRAYCRC
  Monitorport.print(F("Source array CRC is 0x"));
  Monitorport.println(ARDTSourceArrayCRC, HEX);
#endif
  Monitorport.print(F("Segment Size "));
  Monitorport.println(SegmentSize);
  Monitorport.print(F("Number segments "));
  Monitorport.println(ARDTNumberSegments);
  Monitorport.print(F("Last segment size "));
  Monitorport.println(ARDTLastSegmentSize);
#endif
}


void ARprintSeconds()
{
#ifdef ENABLEMONITOR
  float secs;
  secs = ( (float) millis() / 1000);
  Monitorport.print(secs, 2);
  Monitorport.print(F(" "));
#endif
}


void ARprintAckBrief()
{
#ifdef ENABLEMONITOR
  ARPacketRSSI = LoRa.readPacketRSSI();
  Monitorport.print(F(",AckRSSI,"));
  Monitorport.print(ARPacketRSSI);
  Monitorport.print(F("dBm"));
#endif
}


void ARprintAckReception()
{
#ifdef ENABLEMONITOR
  ARPacketRSSI = LoRa.readPacketRSSI();
  ARPacketSNR = LoRa.readPacketSNR();
  Monitorport.print(F("ARAckCount,"));
  Monitorport.print(ARAckCount);
  Monitorport.print(F(",ARNoAckCount,"));
  Monitorport.print(ARNoAckCount);
  Monitorport.print(F(",AckRSSI,"));
  Monitorport.print(ARPacketRSSI);
  Monitorport.print(F("dBm,AckSNR,"));
  Monitorport.print(ARPacketSNR);
  Monitorport.print(F("dB"));
  Monitorport.println();
#endif
}


void ARprintACKdetail()
{
#ifdef ENABLEMONITOR
  Monitorport.print(F("ACKDetail"));
  Monitorport.print(F(",RXNetworkID,0x"));
  Monitorport.print(LoRa.getRXNetworkID(ARRXPacketL), HEX);
  Monitorport.print(F(",RXPayloadCRC,0x"));
  Monitorport.print(LoRa.getRXPayloadCRC(ARRXPacketL), HEX);
  Monitorport.print(F(",ARRXPacketL,"));
  Monitorport.print(ARRXPacketL);
  Monitorport.print(F(" "));
  ARprintReliableStatus();
  Monitorport.println();
#endif
}


void ARprintdata(uint8_t *dataarray, uint8_t arraysize)
{
  ARUNUSED(dataarray);
  ARUNUSED(arraysize);

#ifdef ENABLEMONITOR
  Monitorport.print(F("DataBytes,"));
  Monitorport.print(arraysize);
  Monitorport.print(F("  "));
  ARprintArrayHEX((uint8_t *) dataarray, 16);             //There is a lot of data to print so only print first 16 bytes
#endif
}


uint16_t ARgetNumberSegments(uint32_t arraysize, uint8_t segmentsize)
{
  uint16_t segments;
  segments = arraysize / segmentsize;

  if ((arraysize % segmentsize) > 0)
  {
    segments++;
  }
  return segments;
}


uint8_t ARgetLastSegmentSize(uint32_t arraysize, uint8_t segmentsize)
{
  uint8_t lastsize;

  lastsize = arraysize % segmentsize;
  if (lastsize == 0)
  {
    lastsize = segmentsize;
  }
  return lastsize;
}


bool ARsendDTInfo()
{
  //Send array info packet, for this implmentation its really only the flags in ARDTflags that is sent

  uint8_t ValidACK = 0;
  uint8_t localattempts = 0;

  ARbuild_DTInfoHeader(ARDTheader, DTInfoHeaderL, 0);

  do
  {
    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }

    localattempts++;
#ifdef ENABLEMONITOR
    Monitorport.print(F("Send DTInfo packet attempt "));
    Monitorport.println(localattempts);
#endif
    ARTXPacketL = LoRa.transmitDT(ARDTheader, DTInfoHeaderL, (uint8_t *) ARDTdata, 0, NetworkID, TXtimeoutmS, TXpower,  WAIT_TX);

    if (ARTXPacketL == 0)                                         //if there has been an error ARTXPacketL returns as 0
    {
#ifdef ENABLEMONITOR
      Monitorport.println(F("Transmit error"));
#endif
      continue;
    }

    ValidACK = LoRa.waitACKDT(ARDTheader, DTInfoHeaderL, ACKsegtimeoutmS);

    if (ValidACK > 0)
    {
      //ack is a valid relaible packet
      ARRXPacketType = ARDTheader[0];
#ifdef ENABLEMONITOR
      Monitorport.print(F("ACK Packet type 0x"));
      Monitorport.println(ARRXPacketType, HEX);
#endif

      if (ARRXPacketType == DTInfoACK)
      {
#ifdef ENABLEMONITOR
        Monitorport.println(F("DTInfoACK received"));
#endif
        ARAckCount++;
        ARRXPacketType = ARDTheader[0];
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
      ARNoAckCount++;
#ifdef ENABLEMONITOR
      Monitorport.println(F("No valid ACK received "));
#endif

      if (ARNoAckCount > NoAckCountLimit)
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
    bitSet(ARDTErrors, ARSendPacket);
    return false;
  }
  return true;
}


void ARbuild_DTInfoHeader(uint8_t *header, uint8_t headersize, uint8_t datalen)
{
  //This builds the header buffer for a info only header, faults problems etc

  beginarrayRW(header, 0);             //start writing to array at location 0
  arrayWriteUint8(DTInfo);             //write the packet type
  arrayWriteUint8(ARDTflags);          //ARDTflags byte
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

uint32_t ARreceiveArray(uint8_t *ptrarray, uint32_t length, uint32_t receivetimeout)
{
  //returns 0 if no ARDTArrayEnded set, returns length of array if received
  uint32_t startmS = millis();

  ptrARreceivearray = ptrarray;                        //set global pointer to array pointer passed
  ARArrayLength = length;
  ARDTArrayTimeout = false;
  ARDTArrayEnded = false;
  ARDTDestinationArrayLength = 0;

  do
  {
    if (ARreceivePacketDT())
    {
      startmS = millis();
    }

    if (ARDTArrayEnded)                                    //has the end array transfer been received ?
    {
      return ARDTDestinationArrayLength;
    }
  }
  while (((uint32_t) (millis() - startmS) < receivetimeout ));


  if (ARDTArrayEnded)                                    //has the end array transfer been received ?
  {
    return ARDTDestinationArrayLength;
  }
  else
  {
    ARDTArrayTimeout = true;
    return 0;
  }

}


bool ARreceivePacketDT()
{
  //Receive data transfer packets

  ARRXPacketType = 0;
  ARRXPacketL = LoRa.receiveDT(ARDTheader, HeaderSizeMax, (uint8_t *) ARDTdata, DataSizeMax, NetworkID, RXtimeoutmS, WAIT_RX);

  if (ARDTLED >= 0)
  {
    digitalWrite(ARDTLED, HIGH);
  }

#ifdef ENABLEMONITOR
#ifdef DEBUG
  ARprintSeconds();
#endif
#endif
  if (ARRXPacketL > 0)
  {
    //if the LoRa.receiveDT() returns a value > 0 for ARRXPacketL then packet was received OK
    //then only action payload if destinationNode = thisNode
    ARreadHeaderDT();                        //get the basic header details into global variables ARRXPacketType etc
    ARprocessPacket(ARRXPacketType);         //process and act on the packet
    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }
    return true;
  }
  else
  {
    //if the LoRa.receiveDT() function detects an error RXOK is 0

    uint16_t IRQStatus = LoRa.readIrqStatus();

    if (IRQStatus & IRQ_RX_TIMEOUT)
    {
      Monitorport.println(F("RX Timeout"));
    }
    else
    {
      ARRXErrors++;

#ifdef ENABLEMONITOR
      Monitorport.print(F("PacketError"));
      ARprintPacketDetails();
      ARprintReliableStatus();
      Monitorport.print(F("IRQreg,0x"));
      Monitorport.println(LoRa.readIrqStatus(), HEX);
      Monitorport.println();
#endif
    }

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }
    return false;
  }
}


void ARreadHeaderDT()
{
  //The first 6 bytes of the header contain the important stuff, so load it up
  //so we can decide what to do next.
  beginarrayRW(ARDTheader, 0);                      //start buffer read at location 0
  ARRXPacketType = arrayReadUint8();                //load the packet type
  ARRXFlags = arrayReadUint8();                     //ARDTflags byte
  ARRXHeaderL = arrayReadUint8();                   //load the header length
  ARRXDataarrayL = arrayReadUint8();                //load the datalength
  ARDTSegment = arrayReadUint16();                  //load the segment number
}


bool ARprocessPacket(uint8_t packettype)
{
  //Decide what to do with an incoming packet

  if (packettype == DTSegmentWrite)
  {
    ARprocessSegmentWrite();
    return true;
  }

  if (packettype == DTArrayStart)
  {
    ARprocessArrayStart(ARDTdata, ARRXDataarrayL);       //ARDTdata contains the filename
    return true;
  }

  if (packettype == DTArrayEnd)
  {
    ARprocessArrayEnd();
    return true;
  }

  return true;
}


bool ARprocessSegmentWrite()
{
  //There is a request to write a segment to array on receiver
  //checks that the sequence of segment writes is correct

  uint8_t index, byteswritten = 0;

  if (!ARDTArrayStarted)
  {
    //something is wrong, have received a request to write a segment but there is no array
    //write in progresss so need to reject the segment write with a restart NACK

#ifdef ENABLEMONITOR
    Monitorport.println();
    Monitorport.println(F("*************************************************************"));
    Monitorport.println(F("Error - Segment write with no array write started - send NACK"));
    Monitorport.println(F("*************************************************************"));
    Monitorport.println();
#endif

    ARDTheader[0] = DTStartNACK;
    delay(ACKdelaymS);
    delay(DuplicatedelaymS);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }

    LoRa.sendACKDT(ARDTheader, DTStartHeaderL, TXpower);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }
    return false;
  }

  if (ARDTSegment == ARDTSegmentNext)
  {
    //segment to write is as expected

    for (index = 0; index < ARRXDataarrayL; index++)
    {
      ptrARreceivearray[ARarraylocation] = ARDTdata[index];
      ARarraylocation++;
      byteswritten++;
    }

#ifdef ENABLEMONITOR
#ifdef PRINTSEGMENTNUM
    Monitorport.println(ARDTSegment);
#endif
#endif

    ARDTheader[0] = DTSegmentWriteACK;
    delay(ACKdelaymS);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }

    LoRa.sendACKDT(ARDTheader, DTSegmentWriteHeaderL, TXpower);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }
    ARDTReceivedSegments++;
    ARDTSegmentLast = ARDTSegment;                  //so we can tell if sequece has been received twice
    ARDTSegmentNext = ARDTSegment + 1;
    return true;
  }

  if (ARDTSegment == ARDTSegmentLast)
  {
#ifdef ENABLEMONITOR
#ifdef DEBUG
    Monitorport.print(F("ERROR segment "));
    Monitorport.print(ARDTSegment);
    Monitorport.println(F(" already received "));
#endif
#endif

    ARDTheader[0] = DTSegmentWriteACK;
    delay(DuplicatedelaymS);
    delay(ACKdelaymS);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }
    LoRa.sendACKDT(ARDTheader, DTSegmentWriteHeaderL, TXpower);

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }
    return true;
  }

  if (ARDTSegment != ARDTSegmentNext )
  {
    ARDTheader[0] = DTSegmentWriteNACK;
    ARDTheader[4] = lowByte(ARDTSegmentNext);
    ARDTheader[5] = highByte(ARDTSegmentNext);
    delay(ACKdelaymS);
    delay(DuplicatedelaymS);                   //add an extra delay here to stop repeated segment sends

#ifdef ENABLEMONITOR
    Monitorport.print(F(" ERROR Received Segment "));
    Monitorport.print(ARDTSegment);
    Monitorport.print(F(" expected "));
    Monitorport.print(ARDTSegmentNext);
    Monitorport.print(F(" "));
    Monitorport.print(F(" Send NACK for segment "));
    Monitorport.print(ARDTSegmentNext);
    Monitorport.println();
    Monitorport.println();
    Monitorport.println(F("*****************************************"));
    Monitorport.print(F("Transmit restart request for segment "));
    Monitorport.println(ARDTSegmentNext);
#ifdef DEBUG
    ARprintheader(ARDTheader, ARRXHeaderL);
#endif
    Monitorport.println();
    Monitorport.println(F("*****************************************"));
    Monitorport.println();
    Monitorport.flush();
#endif

    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, HIGH);
    }
    LoRa.sendACKDT(ARDTheader, DTSegmentWriteHeaderL, TXpower);
    if (ARDTLED >= 0)
    {
      digitalWrite(ARDTLED, LOW);
    }
    return false;
  }

  return true;
}


bool ARprocessArrayStart(uint8_t *buff, uint8_t filenamesize)
{
  //There is a request to start writing to a local array on receiver
  //The transmitter will have passed a filename to be used when saving the array to SD
  //or for use when transferring the array elsewhere

  ARDTArrayStarted = false;                         //until this function completes the Array transfer has not started
  ARDTArrayEnded = false;                           //and it cannot hve completed either ......
  ARarraylocation = 0;
  beginarrayRW(ARDTheader, 4);                      //start buffer read at location 4
  ARDTSourceArrayLength = arrayReadUint32();        //load the array length being sent
  ARDTSourceArrayCRC = arrayReadUint16();           //load the CRC of the array being sent
  memset(ARDTfilenamebuff, 0, ARDTfilenamesize);      //clear ARDTfilenamebuff to all 0s
  memcpy(ARDTfilenamebuff, buff, filenamesize);     //copy received ARDTdata into ARDTfilenamebuff, the array should have a destination filename

#ifdef ENABLEMONITOR
  Monitorport.print((char*) ARDTfilenamebuff);
  Monitorport.println(F(" Array start write request"));
#ifdef DEBUG
  Monitorport.print(F("Header > "));
  ARprintArrayHEX(ARDTheader, 16);
#endif
  Monitorport.println();
  ARprintSourceArrayDetails();

  if bitRead(ARRXFlags, ARNoFileSave)
  {
    Monitorport.println(F("Remote did not save file to SD"));
  }
#endif
  ARDTStartmS = millis();
  delay(ACKdelaystartendmS);

  ARDTheader[0] = DTArrayStartACK;                    //set the ACK packet type

  if (ARDTLED >= 0)
  {
    digitalWrite(ARDTLED, HIGH);
  }
  LoRa.sendACKDT(ARDTheader, DTArrayStartHeaderL, TXpower);
  if (ARDTLED >= 0)
  {
    digitalWrite(ARDTLED, LOW);
  }
  ARDTSegmentNext = 0;                               //after a\rray write start open, segment 0 is next

  ARDTArrayStarted = true;

  return true;
}


bool ARprocessArrayEnd()
{
  // There is a request to end writing to an array on receiver
#ifdef ENABLEMONITOR
  Monitorport.print((char*) ARDTfilenamebuff);
  Monitorport.println(F(" end array write request"));
#endif

  if (ARDTArrayStarted)                                   //check if array write had been started, end it if it is
  {
    ARDTArrayStarted = false;
    ARDTDestinationArrayLength = ARarraylocation;

#ifdef ENABLEMONITOR
    Monitorport.print(F("ARDTDestinationArrayLength "));
    Monitorport.println(ARDTDestinationArrayLength);
#ifdef ENABLEARRAYCRC
    ARDTDestinationArrayCRC = ARarrayCRC(ptrARreceivearray, ARDTDestinationArrayLength, 0xFFFF);
    Monitorport.print(F("Destination arrayCRC 0x"));
    Monitorport.println(ARDTDestinationArrayCRC, HEX);
#endif
#endif

    beginarrayRW(ARDTheader, 4);                       //start writing to array at location 12
    arrayWriteUint32(ARDTDestinationArrayLength);       //write array length of array just written just written to ACK header
    arrayWriteUint16(ARDTDestinationArrayCRC);          //write CRC of array just written to ACK header

#ifdef ENABLEMONITOR
    Monitorport.println(F("Array write ended"));
    Monitorport.print(F("Transfer time "));
    Monitorport.print(millis() - ARDTStartmS);
    Monitorport.print(F("mS"));
    Monitorport.println();
    ARprintDestinationArrayDetails();
#endif
  }
  else
  {
#ifdef ENABLEMONITOR
    Monitorport.println(F("Array write already ended"));
#endif
    delay(DuplicatedelaymS);
  }

  delay(ACKdelaystartendmS);
  ARDTheader[0] = DTArrayEndACK;

  if (ARDTLED >= 0)
  {
    digitalWrite(ARDTLED, HIGH);
  }

  LoRa.sendACKDT(ARDTheader, DTArrayEndHeaderL, TXpower);

  if (ARDTLED >= 0)
  {
    digitalWrite(ARDTLED, LOW);
  }

  ARDTArrayEnded = true;
  return true;
}


uint16_t ARarrayCRC(uint8_t *buffer, uint16_t size, uint16_t startvalue)
{
  uint16_t index, libraryCRC;
  uint8_t j;

  libraryCRC = startvalue;                                  //start value for CRC16

  for (index = 0; index < size; index++)
  {
    libraryCRC ^= (((uint16_t)buffer[index]) << 8);
    for (j = 0; j < 8; j++)
    {
      if (libraryCRC & 0x8000)
        libraryCRC = (libraryCRC << 1) ^ 0x1021;
      else
        libraryCRC <<= 1;
    }
  }

  return libraryCRC;
}


void ARprintSourceArrayDetails()
{
#ifdef ENABLEMONITOR
  Monitorport.print(ARDTfilenamebuff);
  Monitorport.print(F(" Source array length is "));
  Monitorport.print(ARDTSourceArrayLength);
  Monitorport.println(F(" bytes"));
#ifdef ENABLEARRAYCRC
  Monitorport.print(F("Source array CRC is 0x"));
  Monitorport.println(ARDTSourceArrayCRC, HEX);
#endif
#endif
}


void ARprintDestinationArrayDetails()
{
#ifdef ENABLEMONITOR
  Monitorport.print(F("Destination array length "));
  Monitorport.print(ARDTDestinationArrayLength);
  Monitorport.println(F(" bytes"));
  Monitorport.print(F("Source array length "));
  Monitorport.print(ARDTSourceArrayLength);
  Monitorport.println(F(" bytes"));

  if (ARDTDestinationArrayLength != ARDTSourceArrayLength)
  {
    Monitorport.println(F("ERROR - array lengths do not match"));
  }
  else
  {
    Monitorport.println(F("Array lengths match"));
  }

#ifdef ENABLEARRAYCRC
  Monitorport.print(F("Destination array CRC is 0x"));
  Monitorport.println(ARDTDestinationArrayCRC, HEX);
  Monitorport.print(F("Source array CRC is 0x"));
  Monitorport.println(ARDTSourceArrayCRC, HEX);

  if (ARDTDestinationArrayCRC != ARDTSourceArrayCRC)
  {
    Monitorport.println(F("ERROR - array CRCs do not match"));
  }
  else
  {
    Monitorport.println(F("Array CRCs match"));
  }
#endif
#endif
}


//************************************************
//Common functions
//************************************************

void ARprintArrayHEX(uint8_t *buff, uint32_t len)
{
  ARUNUSED(buff);
  ARUNUSED(len);

#ifdef ENABLEMONITOR
  uint32_t index, buffdata;

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


void ARsetDTLED(int8_t pinnumber)
{
  //give the data transfer routines an LED to flash
  if (pinnumber >= 0)
  {
    ARDTLED = pinnumber;
    pinMode(pinnumber, OUTPUT);
  }
}


void ARprintheader(uint8_t *hdr, uint8_t hdrsize)
{
  ARUNUSED(hdr);
  ARUNUSED(hdrsize);

#ifdef ENABLEMONITOR
  Monitorport.print(F("HeaderBytes,"));
  Monitorport.print(hdrsize);
  Monitorport.print(F(" "));
  ARprintArrayHEX(hdr, hdrsize);
#endif
}


void ARprintPacketDetails()
{
#ifdef ENABLEMONITOR
#ifdef DEBUG
  ARPacketRSSI = LoRa.readPacketRSSI();
  ARPacketSNR = LoRa.readPacketSNR();
  Monitorport.print(F(" RSSI,"));
  Monitorport.print(ARPacketRSSI);
  Monitorport.print(F("dBm"));
  Monitorport.print(F(",SNR,"));
  Monitorport.print(ARPacketSNR);
  Monitorport.print(F("dBm,RXOKCount,"));
  Monitorport.print(ARDTReceivedSegments);
  Monitorport.print(F(",RXErrs,"));
  Monitorport.print(ARRXErrors);
  Monitorport.print(F(" RX"));
  ARprintheader(ARDTheader, ARRXHeaderL);
#endif
#endif
}


void ARprintReliableStatus()
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
