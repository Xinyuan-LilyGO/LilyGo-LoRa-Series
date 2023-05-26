uint8_t RXPacketType;                      //type of received packet, segment write, ACK, NACK etc
uint8_t RXPacketL;                         //length of received packet
uint16_t RXErrors;                         //count of packets received with error
uint8_t RXFlags;                           //DTflags byte in header, could be used to control actions in TX and RX
uint8_t RXHeaderL;                         //length of header
uint8_t RXDataarrayL;                      //length of data array\segment
int16_t PacketRSSI;                        //stores RSSI of received packet
int8_t  PacketSNR;                         //stores signal to noise ratio of received packet

uint16_t TXNetworkID;                      //this is used to identify a transaction, receiver must have the same DTnetworkID
uint16_t TXArrayCRC;                       //should contain CRC of data array sent
uint8_t  TXPacketL;                        //length of transmitted packet

uint16_t LocalPayloadCRC;                  //for calculating the local data array CRC
uint16_t DTLocalFileCRC;                   //CRC of file being transferred
uint16_t DTLocalArrayCRC;                  //CRC of array being transferred
uint32_t DTLocalFileLength;                //length of file to transfer
uint32_t DTLocalArrayLength;               //length of array to send
uint16_t DTSegment;                        //current segment number
uint16_t DTNumberSegments;                 //number of segments for a file transfer
uint8_t DTLastSegmentSize;                 //size of the last segment
uint16_t DTSegmentNext;                    //next segment to send\receive
uint16_t DTReceivedSegments;               //count of segments received
uint16_t DTSegmentLast;                    //last segment to send\receive
uint16_t DTSentSegments;                   //count of segments sent
uint32_t DTarraylocation;                  //a global variable giving the location in the array last written to
uint16_t DTRemoteFileCRC;                  //CRC of returned of the remote saved file
uint32_t DTRemoteFileLength;               //filelength returned of the remote saved file
uint32_t DTStartmS;                        //used for timeing transfers

uint16_t AckCount;                         //keep a track of acks that are received within timeout period
uint16_t NoAckCount;                       //keep a track of acks not received within timeout period

bool DTFileIsOpen;                         //bool to record if file open or closed
bool DTFileTransferComplete;               //bool to flag file transfer complete
