// Code below taken from https://gist.github.com/zonque/0ae2dc8cedbcdbd9b933
// file xymodem-mini.c

// MarkD modifcations
// - uses 128 byte packets for low RAM devices
// - supports batch upload of several files at once

//Revised code 02/01/22, changed;
//sprintf(spfBuff, "%ld", numBytesStillToSend); to sprintf(spfBuff, "%lu", numBytesStillToSend);
//uint32_t numBytesStillToSend = 0; to long unsigned int numBytesStillToSend = 0;
//numBytesThisPacket = min( numBytesStillToSend, sizeof(yPacket.payload)); to numBytesThisPacket = min( (uint32_t) numBytesStillToSend, sizeof(yPacket.payload));

//Revised code 05/01/22, changed;
//Organised YModem transfer to operate from previously filled memor array rather then and SD file

//Revised code 28/01/22 to stop transfer hanging if PC receiving YModem stops responding

#define X_SOH 0x01
#define X_STX 0x02
#define X_ACK 0x06
#define X_NAK 0x15
#define X_EOT 0x04

const uint32_t transfertimeoutmS = 5000;
const uint8_t ackerrorlimit = 16;


struct yModemPacket {
  uint8_t start;
  uint8_t block;
  uint8_t block_neg;
  uint8_t payload[128];
  uint16_t crc;
} __attribute__((packed));

#define CRC_POLY 0x1021

static uint16_t crc_update(uint16_t crc_in, int incr)
{
  uint16_t _xor = crc_in >> 15;
  uint16_t _out = crc_in << 1;

  if (incr)
    _out++;

  if (_xor)
    _out ^= CRC_POLY;

  return _out;
}


static uint16_t crc16(const uint8_t *data, uint16_t size)
{
  uint16_t crc, i;

  for (crc = 0; size > 0; size--, data++)
    for (i = 0x80; i; i >>= 1)
      crc = crc_update(crc, *data & i);

  for (i = 0; i < 16; i++)
    crc = crc_update(crc, 0);

  return crc;
}


static uint16_t swap16(uint16_t in)
{
  return (in >> 8) | ((in & 0xff) << 8);
}

// Main YModem code.
// filename is pointer to null terminated string
// set waitForReceiver to 1 so that the upload begins when TeraTerm is ready
// set batchMode to 0 when sending 1 file
// set batchMode to 1 for each file sent apart from the last one.


static uint32_t yModemSend(const char *filename, uint8_t *arraydata, uint32_t arraysize, int waitForReceiver, int batchMode )
{
  //*filename is pointer to filename
  //*arraydata is pointer to the transferred array
  //arraysize is the lengrth of data to transfer

  //uint32_t numBytesStillToSend = 0;
  long unsigned int numBytesStillToSend = 0;
  uint16_t numBytesThisPacket = 0;
  uint8_t  skip_payload;
  uint8_t  doNextBlock;
  uint8_t  answer = 0;
  char     spfBuff[16];
  struct   yModemPacket yPacket;
  uint32_t index;
  uint32_t arraybytescopied = 0;
  uint32_t startmS;

  uint8_t ackerrors = 0;

  if (arraysize > ARDTarraysize)
  {
    Monitorport.println("Array too big");
    return 0;
  }

  // get the size of the file and convert to an ASCII representation for header packet
  numBytesStillToSend = arraysize;
  sprintf(spfBuff, "%lu", numBytesStillToSend);

  // wait here for the receiving device to respond
  if (waitForReceiver)
  {
    Monitorport.println("Waiting for receiver ping ...");
    while ( YModemSerial.available() ) YModemSerial.read();

    startmS = millis();

    do
    {
      if (YModemSerial.available()) answer = YModemSerial.read();
    }
    while ((answer != 'C') && ((uint32_t) (millis() - startmS) < transfertimeoutmS));
  }

  if (answer != 'C')
  {
    Monitorport.println("Timeout starting YModem transfer");
    return 0;
  }
  else
  {
    Monitorport.println("done");
  }

  Monitorport.print("Ymodem Sending ");
  Monitorport.println(filename);
  Monitorport.print(spfBuff);
  Monitorport.println(" bytes");
  yPacket.start = X_SOH;
  yPacket.block = 0;

  // copy the filename into the payload - fill remainder of payload with 0x00
  strncpy((char *) yPacket.payload, filename, sizeof(yPacket.payload));

  // insert the file size in bytes as ASCII after the NULL of the filename string
  strcpy( (char *)(yPacket.payload) + strlen(filename) + 1 , spfBuff );                  //spfBuff is file size ?

  // first pass - don't read any file data as it will overwrite the file details packet
  skip_payload = 1;

  while (numBytesStillToSend > 0)
  {
    doNextBlock = 0;

    // if this isn't the 1st pass, then read a block of up to 128 bytes from the file
    if (skip_payload == 0)
    {
      numBytesThisPacket = min( (uint32_t) numBytesStillToSend, sizeof(yPacket.payload));

      //now fill yPacket.payload with numBytesThisPacket from arraydata
      for (index = 0; index < numBytesThisPacket; index++)
      {
        yPacket.payload[index] = arraydata[arraybytescopied];
        arraybytescopied++;
      }

      if (numBytesThisPacket < sizeof(yPacket.payload))
      {
        //pad out the rest of the payload block with 0x1A
        memset(yPacket.payload + numBytesThisPacket, 0x1A, sizeof(yPacket.payload) - numBytesThisPacket);
      }
    }

    yPacket.block_neg = 0xff - yPacket.block;

    //calculate and insert the CRC16 checksum into the packet
    yPacket.crc = swap16(crc16(yPacket.payload, sizeof(yPacket.payload)));

    //send the whole packet to the receiver - will block here
    YModemSerial.write( (uint8_t*) &yPacket, sizeof(yPacket));

    // wait for the receiver to send back a response to the packet
    startmS = millis();
    while ((!YModemSerial.available()) && ((uint32_t) (millis() - startmS) < transfertimeoutmS));
    if (!YModemSerial.available())
    {
      Monitorport.println("Timeout waiting YModem response");
      return 0;
    }

    answer = YModemSerial.read();

    switch (answer)
    {
      case X_NAK:
        //something went wrong - send the same packet again?
        Monitorport.print("N");
        ackerrors++;
        break;

      case X_ACK:
        //got ACK to move to the next block of data
        Monitorport.print(".");
        doNextBlock = 1;
        break;

      default:
        //unknown response
        Monitorport.print("?");
        ackerrors++;
        break;
    }

    if (ackerrors >= ackerrorlimit)
    {
      Monitorport.println("Ack error limit reached");
      return 0;
    }

    // need to handle the 'C' response after the initial file details packet has been sent
    if (skip_payload == 1)
    {
      startmS = millis();
      while ((!YModemSerial.available()) && ((uint32_t) (millis() - startmS) < transfertimeoutmS));
      if (!YModemSerial.available())
      {
        Monitorport.println("Timeout waiting YModem response");
        return 0;
      }

      answer = YModemSerial.read();
      if (answer == 'C')
      {
        // good - start sending the data in the next transmission
        skip_payload = 0;
      }
      else
      {
        // error? send the file details packet again?
        doNextBlock = 0;
      }
    }

    // move on to the next block of data
    if (doNextBlock == 1)
    {
      yPacket.block++;
      numBytesStillToSend = numBytesStillToSend - numBytesThisPacket;
    }
  }

  // all done - send the end of transmission code
  YModemSerial.write(X_EOT);

  // need to send EOT again for YMODEM
  startmS = millis();
  while ((!YModemSerial.available()) && ((uint32_t) (millis() - startmS) < transfertimeoutmS));
  if (!YModemSerial.available())
  {
    Monitorport.println("Timeout waiting YModem response");
    return 0;
  }

  answer = YModemSerial.read();
  YModemSerial.write(X_EOT);

  if (batchMode == 0)
  {
    //and then a packet full of NULL seems to terminate the process
    //and make TeraTerm close the receive dialog box
    yPacket.block = 0;
    yPacket.block_neg = 0xff - yPacket.block;
    memset(yPacket.payload, 0x00, sizeof(yPacket.payload) );
    yPacket.crc = swap16(crc16(yPacket.payload, sizeof(yPacket.payload)));
    YModemSerial.write( (uint8_t*)&yPacket, sizeof(yPacket));
  }

  Monitorport.println("done");
  return arraybytescopied;
}
