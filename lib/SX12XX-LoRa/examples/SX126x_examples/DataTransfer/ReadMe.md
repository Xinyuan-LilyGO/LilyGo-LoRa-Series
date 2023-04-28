Several of the functions needed to make the data transfers work were already incorporated in the reliable packets functions, so I just had to add the library functions to transmit and receive the packets that consisted of a configurable header array and a data array. 

The additional functions needed were;

    uint8_t transmitDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t txtimeout, int8_t txpower, uint8_t wait);
    uint8_t receiveDT(uint8_t *header, uint8_t headersize, uint8_t *dataarray, uint8_t size, uint16_t networkID, uint32_t rxtimeout, uint8_t wait );
    uint8_t sendACKDT(uint8_t *header, uint8_t headersize, int8_t txpower);
    uint8_t waitACKDT(uint8_t *header, uint8_t headersize, uint32_t acktimeout);
  
The various sketch functions required such as sending an openfile command to the receiver, sending the the segments of the file and a close file command, would use the above core library functions with various different header formats. The detail of passing the file names across (for the receiver to save to SD with), calculating segment sizes and dealing with the segment sequencing would be dealt with at the Arduino sketch level where it would be easier to see exactly what is going on during a transfer. Perhaps at a later date it might be possible to move more of the required sketch code into a library function. 


## Example Programs

A simple test program was needed so that you could check a particular set of LoRa modem parameters to see how reliable they were at a chosen distance for sending the large packets. This is the purpose of the first two example sketches, '**231\_Data\_Transfer\_Test\_Transmitter.ino**' and '**232\_Data\_Transfer\_Test\_Receiver.ino**'

The transmitter sends a test segment of size defined by DTSegmentSize in the Settings.h file where the LoRa modem settings can also be defined.

The test program does implement a check on the segment sequence. If the receiver has just had segment 10, then it next expects segment 11. If something goes wrong and say segment 12 appears next then the receiver recognises this and sends a NACK packet back to the transmitter to restart the sequence from number 11. You can test this recovery at any time by resetting the transmitter. 

The program **221\_LoRa\_DTPacket\_Monitor.ino** can be used to monitor the progress of the test transmitter, be sure to use the same LoRa modem settings as the test transmitter and receiver. 

## File Transfer

The purpose of the Data Transfer functions in the SX12XX Library is for applications such as moving files from one Arduino to another over a LoRa link. There are no guarantees in radio frequency reception of data packets, it's inevitable that some will be missed. Thus the data transfer functions need to deal with this as well as coping with packets from possible foreign sources or missed segments in the file. A single bit error in a graphic image for instance can render the image unreadable. 

The examples **233\_SDfile\_Transfer\_Transmitter.ino** and **234\_SDfile\_Transfer\_Receiver.ino** transfer a choice of files; $50SATL.JPG 63091 bytes, $50SATS.JPG 6880 bytes and $50SATT.JPG  1068 bytes) from the SD card on the transmitter Arduino to the SD card on another receiver Arduino. Arduino DUEs were used for testing these examples. The JPG image files above are located in the examples\SX126x_examples\DataTransfer folder and will need to be copied to the SD card on the transmitter. 

<br>
<p align="center">
  <img width="250"  src="/$50SATL.jpg">
</p>
<br>

The transmitter starts the transfer by requesting that the file name chosen is opened on the remote receiver, the transmitter will keep sending this open file request till it succeeds. Then the actual segment\data transfer is started and each segment will be transmitted until it is accepted. If there is a segment sequence error at the receiver then the transmitter is notified and the transmission of segments restarts at the correct location. The last segment sent can be a different size to those in the main body of the file transfer. The transmitter then sends a file close request and the receiver then returns the file length and CRC of the file now on it's SD card data back to the transmitter. This is a further integrity check that the data has been transferred correctly.   

The transfer could be organised in such a way that the segment  transmissions were blind, with no acknowledge, but that would then require the receiver to keep track of missed segments and later request re-transmission. There are some LoRa set-ups that use SSDV to transfer images from cameras, the image is processed and spilt into blocks and a part image can be displayed even if there are some blocks missing. However, the data transfer method described here has no processing that is dependant on an image or file type, it just treats the image or file as a string of bytes. 

## Transfer a memory array

Example **235\_Array\_Transfer\_Transmitter.ino** is a version of **233\_SDfile\_Transfer\_Transmitter.ino** that demonstrates sending a memory array (DTsendarray) from a transmitter to a receiver that then saves the received array onto a file on an SD card. The DTsendarray is first populated with data from a file /$50SATS.JPG or /$50SATT.JPG by the transmitter. In this example the array is then sent as a sequence of segments, similar to the way a file would be read from SD and sent.


##Fine tuning for performance

The speed of the data transfers is mainly dependant on the LoRa settings used, higher\faster data rates come from using a lower spreading factor 
and a higher bandwidth, although of course the higher the data rate the shorter the distance covered. 

There are two program parameters in the example sketches that you may need to adjust. When the receiver has picked up a packet from the transmitter there is a programmable delay before the acknowledge is sent. This is the ACKdelaymS parameter. If the transmitter is particularly slow in changing from transmitting a packet and being ready to pick up the start of the acknowledge then it might miss it. Due to the delays in the receiver of writing a segment to SD an ACKdelaymS of 0 will likely work, but increase it if the transmitter is missing a lot of the acknowledge packets. 

A second parameter to adjust is the ACKtimeoutDTmS, and this is the period the transmitter waits for a valid acknowledge before sending the packet again. This time-out needs to be long enough to receive an acknowledge, but not too long or every missed acknowledge could slow down the transfer as it waits for the time-out period before re-transmitting. 


## Achieved data rates

With the segment length set at maximum for LoRa, 245 bytes, and LoRa settings of spreading factor 5, bandwidth 1600khz and coding rate 4:5  the 63019 byte $50SATL.JPG file took 4.54 seconds to transfer to the SD card on the receiver Arduino. That is an achieved data rate of;

(63091 * 8) / 4.54 = **111,173bps**. 

The equivalent data sheet on air rate for the LoRa settings used is **203kbps**.  

The Data Transfer examples will be found in the \examples\SX126x_examples\DataTransfer folder of the [**SX12XX-LoRa Library**](https://github.com/StuartsProjects/SX12XX-LoRa).

## Duty Cycle

Whilst the described routines work well enough for SX126x in the UHF 433Mhz band in a lot of places in the World your limited to 10% duty cycle, so sending images continuously is not legal. However the library functions described here are for the 2.4Ghz SX128X library, and at 2.4Ghz there are few duty cycle restrictions. 

<br>


**Stuart Robinson**

**November 2021**