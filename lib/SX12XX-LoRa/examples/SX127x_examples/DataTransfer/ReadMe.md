Several of the functions needed to make the data transfers work were already incorporated in the reliable packets functions, so I just had to add the library functions to transmit and receive the packets that consisted of a configurable header array and a data array. 

The additional functions needed were;

    uint8_t transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower);
    uint8_t waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout);
  
The various sketch functions required such as sending an openfile command to the receiver, sending the the segments of the file and a close file command, would use the above core library functions with various different header formats. The detail of passing the file names across (for the receiver to save to SD with), calculating segment sizes and dealing with the segment sequencing would be dealt with at the Arduino sketch level where it would be easier to see exactly what is going on during a transfer. Perhaps at a later date it might be possible to move more of the required sketch code into a library function. 


## Example Programs

A simple test program was needed so that you could check a particular set of LoRa modem parameters to see how reliable they were at a chosen distance for sending the large packets. This is the purpose of the first two example sketches, '**231\_Data\_Transfer\_Test\_Transmitter.ino**' and '**232\_Data\_Transfer\_Test\_Receiver.ino**'

The transmitter sends a test segment of size defined by DTSegmentSize in the DTSettings.h file where the LoRa modem settings can also be defined.

The test program does implement a check on the segment sequence. If the receiver has just had segment 10, then it next expects segment 11. If something goes wrong and say segment 12 appears next then the receiver recognises this and sends a NACK packet back to the transmitter to restart the sequence from number 11. You can test this recovery at any time by resetting the transmitter. 

The program **221\_LoRa\_DTPacket\_Monitor.ino** can be used to monitor the progress of the test transmitter, be sure to use the same LoRa modem settings as the test transmitter and receiver. 

## File Transfer

The purpose of the Data Transfer functions in the SX12XX Library is for applications such as moving files from one Arduino to another over a LoRa link. There are no guarantees in radio frequency reception of data packets, it's inevitable that some will be missed. Thus the data transfer functions need to deal with this as well as coping with packets from possible foreign sources or missed segments in the file. A single bit error in a graphic image for instance can render the image unreadable. 

The examples **233\_SDfile\_Transfer\_Transmitter.ino** and **234\_SDfile\_Transfer\_Receiver.ino** transfer a choice of files; $50SATL.JPG 63091 bytes, $50SATS.JPG 6880 bytes and $50SATT.JPG  1068 bytes) from the SD card on the transmitter Arduino to the SD card on another receiver Arduino. Arduino DUEs were used for testing these examples. The JPG image files above are located in the examples\SX127x_examples\DataTransfer folder and will need to be copied to the SD card on the transmitter. 

<br>
<p align="center">
  <img width="250"  src="/$50SATL.jpg">
</p>
<br>

The transmitter starts the transfer by requesting that the file name chosen is opened on the remote receiver, the transmitter will keep sending this open file request till it succeeds. Then the actual segment\data transfer is started and each segment will be transmitted until it is accepted. If there is a segment sequence error at the receiver then the transmitter is notified and the transmission of segments restarts at the correct location. The last segment sent can be a different size to those in the main body of the file transfer. The transmitter then sends a file close request and the receiver then returns the file length and CRC of the file now on it's SD card data back to the transmitter. This is a further integrity check that the data has been transferred correctly.   

The transfer could be organised in such a way that the segment  transmissions were blind, with no acknowledge, but that would then require the receiver to keep track of missed segments and later request re-transmission. There are some LoRa set-ups that use SSDV to transfer images from cameras, the image is processed and spilt into blocks and a part image can be displayed even if there are some blocks missing. However, the data transfer method described here has no processing that is dependant on an image or file type, it just treats the image or file as a string of bytes. 

The overhead of the send and ack process used is not that significant,   using Arduino DUEs and LoRa settings of SF7 and bandwidth 500khz, the 63091 byte $50SATL.JPG file took 29.9 seconds to transfer.

Examples **236\_SDfile\_Transfer\_TransmitterIRQ.ino** and **237\_SDfile\_Transfer\_ReceiverIRQ.ino** perform the same function as example programs 233 and 234 respectively. The difference is that examples 235 and 236 do not require the connection of micro controller IO pins to DIO0 and NRESET on the SX127x. Instead of reading DIO0 to detect TXdone or RXdone the IRQ register of the SX127 is read for the same information.

## Transfer a memory array

Example **235\_Array\_Transfer\_Transmitter.ino** is a version of **233\_SDfile\_Transfer\_Transmitter.ino** that demonstrates sending a memory array (DTsendarray) from a transmitter to a receiver that then saves the received array onto a file on an SD card. The DTsendarray is first populated with data from a file /$50SATS.JPG or /$50SATT.JPG by the transmitter. In this example the array is then sent as a sequence of segments, similar to the way a file would be read from SD and sent.


##Performance tuning

The example programs contain some parameters that need to be optimised for a particular set of LoRa parameters depending on the on-air rate. Consider the sequence of sending a segment;

1. The transmitter reads the next segment from SD card.
2. The segment is loaded into the LoRa devices buffer.
3. The segment is transmitted as a packet.
4. When the transmission completes, the transmitter switches the LoRa device to listening for the acknowledge. 
5. The transmitter waits a predefined time for the acknowledge.
6. If the acknowledge is received within the time allowed the transmitter starts the process again for the next segment.
7. If there is no valid acknowledge received before the time-out, the segment is re-transmitted.

The data transfer process has to allow for the acknowledge from the receiver not being picked up by the transmitter. In the normal sequence there is a delay at the receiver whilst the received segment is saved to the SD card. Whilst this save to SD card is taking place the transmitter normally has time to swap across from transmit to receive and be ready to pick up the acknowledge. If the acknowledge is not received due to a packet error caused by interference for instance, when the transmitter re-sends the same segment, the receiver recognises this and does not save the segment to SD card. Thus there is only a small delay before the second acknowledge is sent out. Its possible in these circumstances that the transmitter is not actually ready to receive when the acknowledge starts to send, and its missed again, and again, and again.

To get out of this potentially stuck situation there are several parameters that can be changed in the DTSettings.h file for optimum performance.   

	const uint32_t ACKsegtimeoutmS = 75;           //mS to wait for receiving an ACK before re-trying transmit segment
	const uint32_t ACKopentimeoutmS = 250;         //mS to wait for receiving an ACK before re-trying transmit file open
	const uint32_t ACKclosetimeoutmS = 250;        //mS to wait for receiving an ACK before re-trying transmit file close
	const uint32_t DuplicatedelaymS = 10;           //ms delay if there has been an duplicate segment or command receipt
	const uint32_t NoAckCountLimit = 250;           //if no NoAckCount exceeds this value - restart transfer 

### ACKsegtimeoutmS

This is the mS to wait for the transmitter to receive an ACK before re-trying the segment transmit. For maximum throughput this time should be as short as possible. A long time-out here, say 1000ms, would slow the data transfer considerably if there are significant numbers of packet errors with the reception of the acknowledges. A time-out of 75mS works reliably for the faster 200kbps+ on-air data rates but would need to be increased for slower on-air data rates. 

For slower LoRa on-air rates the ACKsegtimeoutmS time of 75mS, might not be enough time for the acknowledge to be received so the transmitter might keep complaining it has received no acknowledge. The receiver might also keep saying it has already received a particular segment. If in doubt increase the ACKsegtimeoutmS to a much bigger figure, 1 second maybe, then keep cutting it down until problems start to appear and then go back and increase the value a bit.  

### ACKopentimeoutmS

This is the mS to wait for the transmitter to receive an ACK before re-trying the file open request to the receiver again. The receiver might take a few 100s of mS to open the requested file on SD, so increase this acknowledge time-out if there are repeated requests sent by the transmitter to open a new file. 


### ACKclosetimeoutmS

This is the mS to wait for the transmitter to receive an ACK before re-trying the fileclose request to the receiver again. The receiver might take a few 100s of milliseconds to close the requested file on SD, so increase this ack timeout if there are repeated requests sent by the transmitter to close the file. Closing a file is the end of the data transfer.

### DuplicatedelaymS

This is an additional delay that can be used to compensate for the quicker ack response of the receiver when a duplicate segment write is sent, see discussion above. Also applies to duplicate file open or close requests. If a repeated sequence of no ack received messages are reported by the transmitter try adding a few 10s of mS extra delay with this parameter.  

### NoAckCountLimit

From the start of the data transfer a running count is kept of how many transmissions are sent but no acknowledge is received. If the receiver gets stuck somehow then the number of missed acknowledges can increase quickly, depending on the time-out settings. If the number of missed acknowledges reaches the NoAckCountLimit set then the data transfer is restarted. This limit is intended as a backstop to prevent the transmitter endlessly sending segments or requests.  


## Achieved data rates

With the segment length set at maximum, 245 bytes, the 63019 byte $50SATL.JPG file took 29.98 seconds to transfer to the SD card on the receiver Arduino. That is an achieved data rate of;

(63091 * 8) / 29.98 = **16,833bps**. 

The equivalent data sheet on air rate for the LoRa settings used, spreading factor 7, bandwidth 500khz and coding rate 4:5 is **21,875bps**. 
The Data Transfer examples will be found in the \examples\SX127x_examples\DataTransfer folder of the [**SX12XX-LoRa Library**](https://github.com/StuartsProjects/SX12XX-LoRa).

## Conclusions

Well, the data transfers work and they are not too slow either. What distances the file\image transfer might work at is an interesting question. The examples described above all use spreading factor 7, bandwidth 500khz and coding rate 4:5, for an on air data rate of 21,875bps. Way back in 2015 I had fast LoRa communications working from my shed to a high altitude balloon that was 100km+ distant. The settings were similar to those used above, spreading factor 7, bandwidth 500khz but coding rate 4:8 and that equates to an on-air rate of 13,671bps. When those 100km+ settings are used with the data transfer examples the 63019 byte image file took 47.3 seconds to transfer. 

In theory the 21,875bps data rate of the SX127x at spreading factor 7 could be improved to 37,500bps by using spreading factor 6, but for the SX127x that requires fixed length packets, which is  not so easy to arrange. However the SX126x devices can use variable packet lengths and at a lower spreading factor of 5 and that would push the on-air rate to around 63,500bps. 

## Duty Cycle

Whilst the described routines work well enough for SX127x in the 433Mhz band in a lot of places in the World your limited to 10% duty cycle, so sending images continuously is not legal. However the library functions described should are available in the 2.4Ghz SX128X library, and at 2.4Ghz there are few duty cycle restrictions. In addition the SX128x devices have a LoRa on-air rate of 203kbps so some 9 times faster than the SX127x examples above. 

<br>


**Stuart Robinson**

**November 2021**