## SX12XX Library Example Programs


For the majority of the program examples you will need to define the pins used plus the frequency and the LoRa settings used in the Settings.h file. The default provided settings may not be optimised for long distance. See the 'What is LoRa' document for information on how LoRa settings affect range. 

Some of the examples use sleep mode on the processor and LoRa device to save power. Typical sleep currents may be mentioned in the description of the program. In most all cases a 'bare bones' Atmel based Arduino has been used to measure the sleep currents and you may not get even close to the quoted figures using standard Arduinos such as Pro Minis or similar. Optimising particular Arduino boards for low power sleep is outside of the scope of this library and examples.

  


#### 1\_LED\_Blink &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Basics folder)

This program blinks an LED connected the pin number defined by LED1.The pin 13 LED, fitted to some Arduinos is blinked as well. The blinks should be close to one per second. Messages are sent to the Serial Monitor also.

#### 2\_Register\_Test &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Basics folder)
This program is stand alone, it is not necessary to install the SX12XX-LoRa library to use it. 

The program checks that a lora device can be accessed by doing a test register write and read. If there is no device found a message is printed on the serial monitor. The contents of the registers from 0x00 to 0x7F are printed, there is a copy of a typical printout below. Note that the read back changed frequency may be different to the programmed frequency, there is a rounding error due to the use of floats to calculate the frequency. 

	SX1276-79 Selected
	LoRa Device found
	Device version 0x12
	Frequency at reset 434000000
	Registers at reset
	Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	0x00  00 09 1A 0B 00 52 6C 80 00 4F 09 2B 20 08 02 0A 
	0x10  FF 70 15 0B 28 0C 12 47 32 3E 00 00 00 00 00 40 
	0x20  00 00 00 00 05 00 03 93 55 55 55 55 55 55 55 55 
	0x30  90 40 40 00 00 0F 00 00 00 F5 20 82 04 02 80 40 
	0x40  00 00 12 24 2D 00 03 00 04 23 00 09 05 84 32 2B 
	0x50  14 00 00 10 00 00 00 0F E0 00 0C 04 06 00 5C 78 
	0x60  00 19 0C 4B CC 0D FD 20 04 47 AF 3F F2 3F D9 0B 
	0x70  D0 01 10 00 00 00 00 00 00 00 00 00 00 00 00 00 


	Changed Frequency 434099968
	Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	0x00  00 09 1A 0B 00 52 6C 86 66 4F 09 2B 20 08 02 0A 
	0x10  FF 70 15 0B 28 0C 12 47 32 3E 00 00 00 00 00 40 
	0x20  00 00 00 00 05 00 03 93 55 55 55 55 55 55 55 55 
	0x30  90 40 40 00 00 0F 00 00 00 F5 20 82 04 02 80 40 
	0x40  00 00 12 24 2D 00 03 00 04 23 00 09 05 84 32 2B 
	0x50  14 00 00 10 00 00 00 0F E0 00 0C 04 06 00 5C 78 
	0x60  00 19 0C 4B CC 0D FD 20 04 47 AF 3F F2 3F D9 0B 
	0x70  D0 01 10 00 00 00 00 00 00 00 00 00 00 00 00 00 


#### 3\_LoRa\_Transmitter &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Basics folder)
This is a minimum setup LoRa test transmitter. A packet containing the ASCII text "Hello World 1234567890" is sent using the frequency and LoRa settings specified in the LT.setupLoRa() command. The pins to access the lora device need to be defined at the top of the program also.

The details of the packet sent and any errors are shown on the Arduino IDE Serial Monitor, together with   the transmit power used and the packet length. The matching receiver program, '4\_LoRa\_Receiver' can be used
to check the packets are being sent correctly, the frequency and LoRa settings (in the LT.setupLoRa()   commands) must be the same for the transmitter and receiver programs. Sample Serial Monitor output;

10dBm Packet> Hello World 1234567890  BytesSent,23  PacketsSent,6

For an example of a more detailed configuration for a transmitter, see program 103\_LoRa\_Transmitter.

Serial monitor baud rate is set at 9600

#### 4\_LoRa\_Receiver &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Basics folder)

This is a minimum setup LoRa test receiver. The program listens for incoming packets using the frequency and LoRa settings in the LT.setupLoRa() command. The pins to access the lora device need to be defined at the top of the program also.

There is a printout on the Arduino IDE serial monitor of the valid packets received, the packet is assumed to be in ASCII printable text, if it's not ASCII text characters from 0x20 to 0x7F, expect weird things to happen on the Serial Monitor. Sample serial monitor output;

8s  Hello World  1234567890,RSSI,-44dBm,SNR,9dB,Length,23,Packets,7,Errors,0,IRQreg,50

If there is a packet error it might look like this, which is showing a CRC error;
  
137s PacketError,RSSI,-89dBm,SNR,-8dB,Length,23,Packets,37,Errors,2,IRQreg,70,IRQ\_HEADER\_VALID,IRQ\_CRC\_ERROR,IRQ\_RX\_DONE

If there are no packets received in a 10 second period then you should see a message like this;

112s RXTimeout

For an example of a more detailed configuration for a receiver, see program 104\_LoRa\_Receiver.

Serial monitor baud rate is set at 9600.
<br>

#### 5\_LoRa\_TX\_Sleep\_Timed\_Wakeup\_Atmel &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Sleep folder) 

This program tests the sleep mode and register retention of the lora device in sleep mode, it assumes an Atmel ATMega328P processor is in use. The LoRa settings to use are specified in the 'Settings.h' file.

A packet is sent, containing the text 'Before Device Sleep' and the LoRa device and Atmel processor are put to sleep. The processor watchdog timer should wakeup the processor in 15 seconds (approx) and register values should be retained.  The device then attempts to transmit another packet 'After Device Sleep' without re-loading all the LoRa settings. The receiver should see 'After Device Sleep' for the first packet and 'After Device Sleep' for the second.

Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 6.5uA.


#### 6\_LoRa\_RX\_and\_Sleep\_Atmel &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Sleep folder) 


The program listens for incoming packets using the LoRa settings in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

When the program starts the LoRa device is set-up to receive packets with pin DIO0 set to go high when a packet arrives. The receiver remains powered (it cannot receive otherwise) and the processor (Atmel ATMega328P or 1284P) is put to sleep. When pin DIO0 does go high, indicating a packet is received, the processor wakes up and prints the packet. It then goes back to sleep.

There is a printout of the valid packets received, these are assumed to be in ASCII printable text. The LED will flash for each packet received and the buzzer will sound,if fitted.

Tested on a 'bare bones' ATmega328P board, the current in sleep mode was 12.26mA.

#### 7\_LoRa\_TX\_Sleep\_Switch\_Wakeup\_Atmel &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Sleep folder) 


This program tests the sleep mode and register retention of the lora device in sleep mode, it assumes an Atmel ATMega328P processor is in use. The LoRa settings to use are specified in the 'Settings.h' file.

A packet is sent, containing the text 'Before Device Sleep' and the lora device and Atmel processor are put to sleep. The processor should remain asleep until the pin defined by SWITCH1 in the Settings.h file is connected to ground and the LoRa device register values should be retained.  The LoRa device then attempts to transmit another packet 'After Device Sleep' without re-loading all the LoRa settings. The receiver should see 'After Device Sleep' for the first packet and 'After Device Sleep' for the second.

Tested on a bare bones ATmega328P board, the current in sleep mode was 2.4uA.

#### 8\_LoRa\_LowMemory\_TX &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (LowMemory folder)

The program transmits a packet without using a processor buffer, the LoRa device internal buffer is filled direct with variables. The program is a simulation of the type of packet that might be sent from a GPS tracker. Note that in this example a buffer of text is part of the transmitted packet, this does need a processor buffer which is used to fill the LoRa device internal buffer, if you don't need to transmit text then the uint8_t trackerID[] = "tracker1"; definition can be omitted.

The matching receiving program '9\_LoRa\_LowMemory\_RX' can be used to receive and display the packet, though the program  '15\_LoRa\_RX\_Structure' should receive it as well, since the packet contents are the same.

The contents of the packet received, and printed to serial monitor, should be;
  
"tracker1"     (buffer)      - trackerID  
1+             (uint32\_t)   - packet count     
51.23456       (float)       - latitude   
-3.12345       (float)       - longitude  
199            (uint16\_t)   - altitude  
8              (uint8\_t)    - number of satellites   
3999           (uint16\_t)   - battery voltage 
-9             (int8_t)      - temperature

#### 9\_LoRa\_LowMemory\_RX &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (LowMemory folder)

The program receives a packet without using a processor buffer, the LoRa device internal buffer is read direct and copied to variables. The program is a simulation of the type of packet that might be received from a GPS tracker. Note that in this example a buffer of text is part of the   received packet, this does need a processor buffer which is filled with data from the LoRa device internal buffer, if you don't need to send and receive text then the uint8_t receivebuffer[32]; definition can be   omitted. 

The contents of the packet received, and printed to serial monitor, should be;
  
"tracker1" (buffer)      - trackerID  
1+             (uint32\_t)   - packet count     
51.23456       (float)       - latitude   
-3.12345       (float)       - longitude  
199            (uint16\_t)   - altitude  
8              (uint8\_t)    - number of satellites   
3999           (uint16\_t)   - battery voltage 
-9             (int8_t)      - temperature


#### 10\_LoRa\_Link\_Test\_Transmitter &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)

This is a program that can save you a great deal of time if your testing the effectiveness of a LoRa links or attached antennas. Simulations of antenna performance are no substitute for real world tests and this simple program allows both long distance link performance to be evaluated and antenna performance to be compared.

The program sends short test packets that reduce in power by 1dBm at a time. The start power is defined by start\_power and the end power is defined by end\_power (see Settings.h file). Once the end\_power point is reached, the program pauses a short while and starts the transmit sequence again at start\_power. The packet sent contains the power used to send the packet. By listening for the packets with the basic LoRa receive program (4\_LoRa\_Receiver) you can see the reception results, which should look something like this;

	11s  1*T+05,CRC,80B8,RSSI,-03dBm,SNR,9dB,Length,6, Packets,9,Errors,0,IRQreg,50
	12s  1*T+04,CRC,9099,RSSI,-74dBm,SNR,9dB,Length,6, Packets,10,Errors,0,IRQreg,50
	14s  1*T+03,CRC,E07E,RSSI,-75dBm,SNR,9dB,Length,6, Packets,11,Errors,0,IRQreg,50

Above shows 3 packets received, the first at +05dBm (+05 in printout), the second at 4dBm (+04 in printout) and the third at 3dBm (+03) in printout.

If it is arranged so that reception of packets fails halfway through the sequence by attenuating either the transmitter (with an SMA attenuator for instance) or the receiver (by placing it in a tin perhaps) then if you swap transmitter antennas you can see the dBm difference in reception, which will be the dBm difference (gain) of the antenna.

To start the sequence a packet is sent with the number 999, when received it looks like this;

T*1999

This received packet could be used for the RX program to be able to print totals etc.

LoRa settings to use for the link test are specified in the 'Settings.h' file.


#### 11\_LoRa\_Packet\_Logger\_Receiver &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)


This is a useful packet logger program. It listens for incoming packets using the LoRa settings in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

There is a printout of the valid packets received in HEX format. Thus the program can be used to receive and record non-ASCII packets. The LED will flash for each packet received and the buzzer will sound, if fitted. The measured frequency difference between the frequency used by the transmitter and the frequency used by the receiver is shown. If this frequency difference gets to 25% of the set LoRa bandwidth, packet reception will fail. The displayed error can be reduced by using the 'offset' setting in the 'Settings.h' file. 

#### 12\_ATmel\_Sleep\_with\_Switch\_Wakeup &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware_Checks folder) 

This program tests the deep sleep mode and wakeup with a switch of an Atmel 328P or 1284P processor. The program starts, flashes the LED and then puts the processor into permanent sleep. It can be woken up with a switch press. Used as a base test routine for checking the sleep current of a board.

Tested on an 'bare bones' ATmega328P board, the current in sleep mode was 1.7uA with a 3.3V MCP1700 regulator being used.


#### 13\_Frequency\_and\_Power\_Check\_TX &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)

This is a program that transmits a long LoRa packets lasting about 5 seconds that can be used to measure the frequency and power of the transmission using external equipment. The bandwidth of the transmission is only 10khz, so a frequency counter should give reasonable average result.

The LoRa settings to use, including transmit power, are specified in the 'Settings.h' file.


#### 14\_LoRa\_Structure\_TX &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Basics folder)

This program demonstrates the transmitting of a structure as a LoRa packet. The contents of the structure are the same as in the '8\_LoRa\_LowMemory\_TX' program. The packet sent is typical of what might be sent from a GPS tracker. The structure type is defined as trackerPacket and an instance called location1 is created. The structure which includes a character array (text) is filled with values and transmitted.

The matching receiving program '15\_LoRa\_RX\_Structure' can be used to  receive and display the packet, though the program '9\_LoRa\_LowMemory\_RX' should receive it as well, since the contents are the same.

Note that the structure definition and variable order (including the buffer size) used in the transmitter need to match those used in the receiver. 

The contents of the packet transmitted should be;
  
"tracker1" (buffer)      - trackerID  
1+             (uint32\_t)   - packet count     
51.23456       (float)       - latitude   
-3.12345       (float)       - longitude  
199            (uint16\_t)   - altitude  
8              (uint8\_t)    - number of satellites   
3999           (uint16\_t)   - battery voltage 
-9             (int8_t)      - temperature

#### 15\_LoRa\_Structure\_RX &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Basics folder)


This program demonstrates the receiving of a structure as a LoRa packet. The packet sent is typical of what might be sent from a GPS tracker.

The structure type is defined as trackerPacket and an instance called location1 is created. The structure includes a character array (text).

The matching receiving program is '15\_LoRa\_RX\_Structure' can be used to receive and display the packet, though the program '9\_LoRa\_LowMemory\_RX' should receive it as well, since the packet contents are the same.

Not that the structure definition and variable order (including the buffer size) used in the transmitter need to match those used in the receiver. Good luck.

The contents of the packet received, and printed to serial monitor, should be;
  
"tracker1" (buffer)      - trackerID  
1+             (uint32\_t)   - packet count     
51.23456       (float)       - latitude   
-3.12345       (float)       - longitude  
199            (uint16\_t)   - altitude  
8              (uint8\_t)    - number of satellites   
3999           (uint16\_t)   - battery voltage 
-9             (int8_t)      - temperature


#### 16\_LoRa\_RX\_Frequency\_Error\_Check &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)

This program can be used to check the frequency error between a pair of LoRa devices, a transmitter and receiver. This receiver measures the frequecy error between the receivers  centre frequency and the centre frequency of the transmitted packet. The frequency difference is shown
for each packet and an average over 10 received packets reported. Any transmitter program can be used to give this program something to listen to, including example program '3\_LoRa\_Transmitter'.  


#### 17\_Sensor\_Transmitter &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Sensor folder)

The program transmits a LoRa packet without using a processor buffer, the LoRa devices internal buffer is filled directly with variables. 
  
The sensor used is a BME280. The pressure, humidity, and temperature are read and transmitted. There is also a 16bit value of battery mV (simulated) and and a 8 bit status value at the packet end.

Although the LoRa packet transmitted and received has its own internal CRC error checking, you could still receive packets of the same length from another source. If this valid packet were to be used to recover the sensor values, you could be reading rubbish. To reduce the risk of this, when the packet is transmitted the CRC value of the actual sensor data is calculated and sent out with the packet. This CRC value is read by the receiver and used to check that the received CRC matches the supposed
sensor data in the packet. As an additional check there is some addressing information at the beginning of the packet which is also checked for validity. Thus we can be relatively confident when reading the
received packet that its genuine and from this transmitter. The packet is built and sent in the sendSensorPacket() function, there is a 'highlighted section' where the actual sensor data is added to the packet.

Between readings the LoRa device, BME280 sensor, and Atmel micro controller are put to sleep in units of 8 seconds using the Atmel processor internal watchdog.

The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file. The Atmel watchdog timer is a viable option for a very low current sensor node. A bare bones ATmega328P with regulator and LoRa device has a sleep current of 6.6uA, add the LoRa devices and BME280 sensor module and the average sleep current only rises to 6.8uA.

One of these transmitter programs is running on a long term test with a 175mAh battery, to see how long the battery actually lasts.


#### 18\_Sensor\_Receiver &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Sensor folder)

The program receives a LoRa packet without using a processor buffer, the LoRa devices internal buffer is read direct for the received sensor data. 
  
The sensor used in the matching '17\_Sensor\_Transmitter' program is a BME280 and the pressure, humidity, and temperature are being and received. There is also a 16bit value of battery mV and and a 8 bit status value at the end of the packet.

When the program starts, the LoRa device is set-up to set the DIO0 pin high when a packet is received, the Atmel processor is then put to sleep and will wake up when a packet is received. When a packet is received, its printed and assuming the packet is validated, the sensor results are printed to the serial monitor and screen. Between readings the sensor transmitter is put to sleep in units of 8 seconds using the Atmel
processor internal watchdog.

For the sensor data to be accepted as valid the flowing need to match;

The 16bit CRC on the received sensor data must match the CRC value transmitted with the packet. 
The packet must start with a byte that matches the packet type sent, 'Sensor1'
The RXdestination byte in the packet must match this node ID of this receiver node, defined by 'This_Node'

In total that's 16 + 8 + 8  = 32bits of checking, so a 1:4294967296 chance (approx) that an invalid packet is acted on and erroneous values displayed.

The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file.

With a standard Arduino Pro Mini and SSD1306 display the current consumption was 20.25mA with the display and 16.6mA without the display. 


#### 20\_LoRa\_Link\_Test\_Receiver &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)

The program listens for incoming packets using the LoRa settings in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

The program is a matching receiver program for the '10\_LoRa\_Link\_Test\_TX'. The packets received are displayed on the serial monitor and analysed to extract the packet data which indicates the power used to send the   packet. A count is kept of the numbers of each power setting received. When the transmitter sends the test mode packet at the beginning of the sequence (displayed as 999) the running totals of the powers received are printed. Thus you can quickly see at what transmit power levels the reception fails. 


#### 21\_On\_Off\_Transmitter &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Remote Control folder)

This program is a remote control transmitter. When one of four switches are made (shorted to ground) a packet is transmitted with single byte indicating the state of Switch0 as bit 0, Switch1 as bit 1 and Switch2 as bit 2. To prevent false triggering at the receiver the packet contains a
32 bit number called the TXIdentity which in this example is set to 1234554321. The receiver will only act on, change the state of the outputs, if the identity set in the receiver matches that of the  transmitter. The chance of a false trigger is fairly remote.

Between switch presses the LoRa device and Atmel micro controller are put to sleep. A switch press wakes up the processor from sleep, the switches are read and a packet sent. On a 'bare bones' Arduino set-up the transmitter has a sleep current of approx 2.2uA, so it's ideal for a battery powered remote control with a potential range of many kilometres.


The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file.


#### 22\_On\_Off\_Receiver &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Remote Control folder)

This program is a remote control receiver. When a packet is received an 8 bit byte (SwitchByte) is read and the four outputs (defined in Settings.h) are toggled according to the bits set in this byte. If the Switch1 byte has bit 0 cleared, then OUTPUT0 is toggled. If the Switch1 byte has bit 1 cleared, then OUTPUT1 is toggled. If the Switch1 byte has bit 2 cleared, then OUTPUT2 is toggled. 
  
To prevent false triggering at the receiver the packet also contains a 32 bit number called the TXIdentity which in this example is set to 1234554321. The receiver will only act on, change the state of the outputs, if the identity set in the receiver matches that of the transmitter. The chance of a false trigger is fairly remote.

The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file.


#### 23\_GPS\_Tracker\_Transmitter &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Tracker folder) 

This program is an example of a basic GPS tracker. The program reads the GPS, waits for an updated fix and transmits location and altitude, number of satellites in view, the HDOP value, the fix time of the GPS and the battery voltage. This transmitter can be also be used to investigate GPS performance. At start-up there should be a couple of seconds of  recognisable text from the GPS printed to the serial monitor. If you see garbage or funny characters its likely the GPS baud rate is wrong. If the transmitter is turned on from cold, the receiver will pick up the cold fix time, which is an indication of GPS performance. The GPS will be powered on for around 4 seconds before the timing of the fix starts. Outside with a good view of the sky most GPSs should produce a fix in around 45 seconds. The number of satellites and HDOP are good indications to how well a GPS is working. 

The program writes direct to the LoRa devices internal buffer, no memory buffer is used.
  
The LoRa settings are configured in the Settings.h file.

The program has the option of using a pin to control the power to the GPS, if the GPS module being used has this feature. To use the option change the define in the Settings.h file; '#define GPSPOWER -1' from -1 to the pin number being used. Also set the GPSONSTATE and GPSOFFSTATE to the appropriate logic levels.


#### 24\_GPS\_Tracker\_Receiver  &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Tracker folder) 

This program is an basic receiver for the '23\_Simple\_GPS\_Tracker\_Transmitter' program. 

The program reads the received packet from the tracker transmitter and displays the results on the serial monitor. The LoRa and frequency settings provided in the Settings.h file must match those used by the transmitter. 

The program receives direct from the LoRa devices internal buffer.


#### 25\_GPS\_Tracker\_Receiver\_with\_Display\_and\_GPS  &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Tracker folder) 

This program is an example of a basic portable GPS tracker receiver. The program receives the location packets from the remote tracker and displays them on an OLED display. The program also reads a local GPS and when that has a fix, will display the distance and direction to the remote tracker.
  
The program writes direct to the LoRa devices internal buffer, no memory buffer is used.

The LoRa settings are configured in the Settings.h file.

The received information is printed to screen in this order top to bottom;

Latitude, Longitude, Altitude, HDOP, GPS Fixtime, Tracker battery mV, Number of received packets, Distance and direction to tracker, if local GPS fix. In addition if there is a recent tracker transmitter GPS fix a 'T' is shown on line 0 right of screen and if there is a recent local (receiver) GPS fix a 'R' is displayed line 1 right of screen.
  
The received information is printed to the Serial Monitor as CSV data in this order;
  
Packet Address information, Latitude, Longitude, Altitude, Satellites in use, HDOP, TX status byte, GPS Fixtime, Tracker battery mV, Number of received packets, Distance and direction to tracker, if local GPS fix. 

The program has the option of using a pin to control the power to the GPS, if the GPS module being used has this feature. To use the option change the define in Settings.h; '#define GPSPOWER -1' from -1 to the pin number being used. Also set the GPSONSTATE and GPSOFFSTATE to the appropriate logic levels.

The program by default uses software serial to read the GPS, you can use hardware serial by commenting out this line in the Settings.h file;

 #define USE\_SOFTSERIAL\_GPS

And then defining the hardware serial port you are using, which defaults to Serial1. 

#### 26\_GPS\_Echo  &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Hardware_Checks folder) 

This is a simple program to test a GPS. It reads characters from the GPS using software serial and sends them (echoes) to the IDE serial monitor. If your ever having problems with a GPS (or just think you are) use this program first.

If you get no data displayed on the serial monitor, the most likely cause is that you have the receive data pin into the Arduino (RX) pin connected incorrectly.

If the data displayed on the serial terminal appears to be random text with odd symbols its very likely you have the GPS serial baud rate set incorrectly.

Note that not all pins on all Arduinos will work with software serial, see here;

https://www.arduino.cc/en/Reference/softwareSerial

Serial monitor baud rate is set at 115200.


#### 28\_GPS\_Checker  &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Hardware_Checks folder) 

This program is a portable GPS checker and display. At start-up the program starts checking the data coming from the GPS for a valid fix. It checks for 5 seconds and if there is no fix, prints a message on the serial monitor. During this time the data coming from the GPS is copied to the serial monitor also. 
  
When the program detects that the GPS has a fix, it prints the Latitude, Longitude, Altitude, Number of satellites in use and the HDOP value to the serial monitor.

Serial monitor baud rate is set at 115200, GPS baud rate to 9600, both are configured in setup().


#### 29\_GPS\_Checker\_With\_Display  &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Hardware_Checks folder) 

This program is a GPS checker with a display output. It uses an SSD1306 or SH1106 128x64 I2C OLED display. At start-up the program starts checking the data coming from the GPS for a valid fix. It reads the GPS for 5 seconds and if there is no fix, prints a message on the serial monitor and updates the seconds without a fix on the display. During this time the data coming from the GPS is copied to the serial monitor also.

When the program detects that the GPS has a fix, it prints the Latitude, Longitude, Altitude, Number of satellites in use, the HDOP value, time and date to the serial monitor. If the I2C OLED display is attached that is updated as well. Display is assumed to be on I2C address 0x3C.


#### 30\_I2C\_Scanner &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Hardware_Checks folder)

The program scans the I2C bus and displays the addresses of any devices found. Useful first check when using I2C devices.


#### 31\_SSD1306\_OLED\_Checker &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)

This program is a simple test program for the SSD1306 and SH1106 OLEDs. The program prints a short message on each line, pauses, clears the screen, and starts again. 

OLED address is defined as 0x3C.

#### 33\_LoRa\_RSSI\_Checker\_With\_Display &emsp; &emsp; &emsp;  &emsp; &emsp; &emsp; (Diagnostics and Test folder)

The program listens for incoming packets using the LoRa settings in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also. 

There is a printout of the valid packets received, the packet is assumed to be in ASCII printable text, if its not ASCII text characters from 0x20 to 0x7F, expect weird things to happen on the Serial Monitor. The LED will flash for each packet received and the buzzer will sound, if fitted.

Sample serial monitor output;

1109s  {packet contents}  CRC,3882,RSSI,-69dBm,SNR,10dB,Length,19,Packets,1026,Errors,0,IRQreg,50

If there is a packet error it might look like this, which is showing a CRC error,

1189s PacketError,RSSI,-111dBm,SNR,-12dB,Length,0,Packets,1126,Errors,1,IRQreg,70,IRQ\_HEADER\_VALID,IRQ\_CRC\_ERROR,IRQ\_RX\_DONE

A summary of the packet reception is sent to the OLED display as well, useful for portable applications. 


#### 34\_ATmel\_Sleep\_with\_Watchdog\_Wakeup &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware_Checks folder) 

This program tests the sleep mode of an Atmel ATMega328P processor.

At power up the flashes an LED 4 times, then turns on the LED for 5 seconds. Then the processor is put to sleep for 8 seconds. On wakeup the LED flashes twice, then is on for 5 seconds and the board goes to sleep again. And the sequence repeats. 

Sleep current for a 'bare bones' ATmega328 with a MCP1700 regulator @ 3.3V and using an external event such as a switch to wakeup from sleep should be around 2uA. Using the watchdog timer to wakeup raises the deep sleep current to circa 6.2uA.   

#### 35\_Remote\_Control\_Servo\_Transmitter &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Remote Control folder) 

This is a remote control transmitter that uses a LoRa link to transmit the positions from a simple joystick to a remote receiver. The receiver uses the sent joystick positions to adjust the positions of servos. The positions of the joysticks potentiometers on the transmitter are read with the analogueRead() function.

If the joystick has a switch, often made by pressing on the joystick, then this can be used to remote control an output on the receiver. The switch is read by an interrupt, the interrupt routine sets a flag byte which is read in loop().

The program is intended as a proof of concept demonstration of how to remote control servos, the program is not designed as a practical remote control device for RC model cars for instance.

It would be straight forward to make the transmitter program send packets continuously, but in most places in the world that would break a normal limitation of 10% duty cycle for unlicensed use. Therefore the program was designed to only transmit at a 10% duty cycle. Thus the fastest (lowest air time) packets are used, spreading factor 6 at a bandwidth of 500khz. This results in an air time for the 5 byte control packet of around 4mS, so there are around 25 sent per second.

To have the transmitter program print out the values read from the joystick, comment in the line;

//#define DEBUG

Which is just above the loop() function. With the DEBUG enabled the transmission rate, the rate at which the control packets are transmitted will be slowed down.

To reduce the risk of the receiver picking up LoRa packets from other sources, the packet sent contains a 'TXidentity' number, valid values are 0 - 255. The receiver must be set-up with the matching identity number or the received packets will be ignored.

The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file. These settings are not necessarily optimised for long range.


#### 36\_Remote\_Control\_Servo\_Receiver &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Remote Control folder) 

This is a remote control receiver that uses a LoRa link to control the positions of servos sent from a remote transmitter.  

If the transmitter joystick has a switch, often made by pressing on the joystick, then this can be used to remote control an output on the receiver. 

The program is intended as a proof of concept demonstration of how to remote control servos, the program is not designed as a practical remote control device for RC model cars for instance.

It would be straight forward to make the transmitter program send packets continuously, but in most places in the world that would break a normal limitation of 10% duty cycle for unlicensed use. Therefore the program was designed to only transmit at a 10% duty cycle. Thus the fastest (lowest air time) packets are used, spreading factor 6 at a bandwidth of 500khz. This results in an air time for the 5 byte control packet of around 4mS, so there are around 25 sent per second.  
To have the receiver program print out the joystick values (0-255) read from the received packet, comment in the line;

//#define DEBUG

Which is just above the loop() function. With the DEBUG enabled then there is a possibility that some transmitted packets will be missed. With the DEBUG line enabled to servos should also sweep to and fro 3 times at program start-up.

To reduce the risk of the receiver picking up LoRa packets from other sources, the packet sent contains a 'TXidentity' number, valid values are 0 - 255. The receiver must be set-up with the matching RXIdentity   number in Settings.h or the received packets will be ignored.    

The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file. These settings are not necessarily optimised for long range.


#### 37\_Servo\_Sweep\_Tester &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware_Checks folder) 

This program sweeps two servos from one end of their travel to the other. Useful to check servos are connected correctly and working.


#### 38\_lora\_Relay &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Tracker folder) 

This program will receive a lora packet and relay (re-transmit) it. The receiving and transmitting can use different frequencies and lora settings. The receiving and transmitting settings are in the 'Settings.h' file. For an example of it's use see this report;

How to Search 500 Square Kilometres in 10 minutes.pdf in the libraries 'Test_Reports' folder.



#### 40\_LoRa\_Transmitter\_ImplicitPacket &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Implicit folder) 

This is an example of the use of implicit or fixed length LoRa packets. 
Implicit packets have no header so both transmitter and receiver need to be programmed with the packet length in use. The use of spreading factor 6 requires implicit packets and together with a bandwidth of 500khz, leads to the shortest possible and lowest air time packets. 

This example sends a buffer that is 23 characters long and that length must be defined in Settings.h as the constant 'PacketLength'. 
  
A packet containing ASCII text is sent according to the frequency and LoRa settings specified in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

The details of the packet sent and any errors are shown on the Serial Monitor, together with the transmit power used, the packet length and the CRC of the packet. The matching receive program, '41\_LoRa\_Receiver\_ImplicitPackets' can be used to check the packets are being sent correctly, the frequency and LoRa settings (in Settings.h) must be the same for the Transmit and Receive program. 
  
Sample Serial Monitor output;

10dBm Packet> {packet contents*}  BytesSent,23  CRC,DAAB  TransmitTime,8mS  PacketsSent,1


#### 41\_LoRa\_Receiver\_ImplicitPacket &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Implicit folder) 

This is an example of the use of implicit or fixed length LoRa packets. Implicit packets have no header so both transmitter and receiver need to be programmed with the packet length in use. The use of spreading factor 6 requires implicit packets and together with a bandwidth of 500khz, leads to the shortest possible and lowest air time packets. The program listens for incoming packets using the LoRa settings in the 'Settings.h'. 

This example receives a buffer that is 19 characters long and that length must be defined in Settings.h as the constant 'PacketLength'.
  
The pins to access the lora device need to be defined in the 'Settings.h' file also.

There is a printout of the valid packets received, the packet is assumed to be in ASCII printable text, if its not ASCII text characters from 0x20 to 0x7F, expect weird things to happen on the Serial Monitor. The LED will flash for each packet received and the buzzer will sound, if fitted. 

Sample serial monitor output;

1109s  {packet contents}  CRC,3882,RSSI,-69dBm,SNR,10dB,Length,19,Packets,1026,Errors,0,IRQreg,50 

If there is a packet error it might look like this, which is showing a CRC error,

1189s PacketError,RSSI,-111dBm,SNR,-12dB,Length,0,Packets,1126,Errors,1,IRQreg,70,IRQ\_HEADER\_VALID,IRQ\_CRC\_ERROR,IRQ\_RX\_DONE


#### 42\_WiFi\_Scanner\_Display\_ESP32 &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware Checks\ESP32 folder) 

When the ESP32 turns on the WiFi function, there is a short high current pulse that can cause the ESP32 brownout detect to operate.

This test program at startup flashes an LED, leaves it on and then starts the WiFi. If the Wifi initiates a brownout, you will see the LED flash again. The LED stays on when scanning, the program reports the networks found to the serial console and displays them on an attached SSD1306 OLED.

Thus if you see the LED continually doing short bursts of flashing the turn on off the WiFi is causing the ESP32 to reset. There will also be a message on the serial monitor that the brownout detector operated.


#### 43\_SD\_Card\_Test\_ESP32 &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware Checks\ESP32 folder)

This test program has been written to check that a connected SD card adapter, Micro or standard, is functional. To use the program first copy the file (in this programs directory) called testfile.txt to the root directory of the SD card.

When the program runs it will attempt to open 'testfile.txt' and spool the contents to the Arduino IDE serial monitor. The testfile is part of the source code for the Apollo 11 Lunar Lander navigation and guidance computer. There are LED flashes at power up or reset, then at start of every loop of the test. The LED is on whilst the testfile is being read. If the LED flashes very rapidly then there is a problem accessing the SD card.

The program also has the option of using a logic pin to control the power to the lora and SD card devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.

#### 44\_SD\_Card\_Test\_With\_FS\_ESP322 &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware Checks\ESP32 folder)

This test program has been written to check that a connected SD card adapter, Micro or standard, is functional with the FS functions. To use the program first copy the file (in this programs directory) called testfile.txt to the root directory of the SD card.

When the program runs it will attempt to open 'testfile.txt' and spool the contents to the Arduino IDE serial monitor. The testfile is part of the source code for the Apollo 11 Lunar Lander navigation and guidance computer. There are LED flashes at power up or reset, then at start of every loop of the test. The LED is on whilst the testfile is being read. If the LED flashes very rapidly then there is a problem accessing the SD card.

The program also has the option of using a logic pin to control the power to the lora and SD card devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.


#### 45\_Battery\_Voltage\_Read\_Test &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware_Checks folder) 


This test program has been written to check that hardware for reading the battery voltage has been assembled correctly such that it is funtional. The value defined as 'ADMultiplier' in settings.h is used to adjust the value read from the 91K\11K resistor divider and convert into mV. 

There is also an option of using a logic pin to turn the resistor divider used to read battery voltage on and off. This reduces current used in sleep mode. To use the feature set the define for pin BATVREADON in 'Settings.h' to the pin used. If not using the feature set the pin number to -1.

#### 47\_DeepSleep\_Timed\_Wakeup\_ESP32 &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware_Checks\ESP32 folder) 

This program flashes a LED connected to the pin defined by LED1, and puts the ESP32 to deep_sleep for a period determined by the TIME_TO_SLEEP variable (in seconds).

The program also has the option of using a logic pin to control the power to the lora and SD card devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.

Current in deep_sleep for a bare bones ESP32 with regulator and no other devices was 27uA.


#### 48\_DS18B20\_Test &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Hardware_Checks folder) 


The program reads a single DS18B20 temperature sensor and prints the result to the serial monitor.

The program also has the option of using a logic pin to control the power to the lora and SD card devices, which can save power in sleep mode. If the hardware is fitted to your board then these devices are assumed to be powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.



#### 50\_LightSleep\_Timed\_Wakeup\_ESP32 &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (ESP32 folder) 

The program flashes a LED connected to the pin defined by LED1, and puts the ESP32 to light_sleep for a period determined by TIME_TO_SLEEP (in seconds).

The program also has the option of using a logic pin to control the power to the lora device, SD card and DS18B20 devices, which can save power in sleep mode. If the hardware is fitted to your board these devices are   powered on by setting the VCCPOWER pin low. If your board does not have this feature set VCCPOWER to -1.

#### 51\_DeepSleep\_Switch\_Wakeup\_ESP32 &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (ESP32 folder) 

The program flashes a LED connected to the pin defined by LED1, and puts the ESP32 to deep_sleep. Pressing BOOT switch should wake up the ESP32 from sleep.

Only the specific RTC IO pins can be used as a source for external wakeup. 
These are pins: 0,2,4,12-15,25-27,32-39.

Current in deep sleep for a bare bones ESP32 with regulator and no other devices was 27uA.

#### 52\_FLRC\_Transmitter &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (SX128X\Examples\Basics folder) 

This is a test transmitter for the Fast Long Range Communication (FLRC) mode introduced in the SX128X devices. A packet containing ASCII text is sent according to the frequency and FLRC settings specified in the 'Settings.h' file. The pins to access the SX128X device need to be defined
in the 'Settings.h' file also.

The details of the packet sent and any errors are shown on the Serial Monitor, together with the transmit power used, the packet length and the CRC of the packet. The matching receive program, '53_FLRC_Receiver' can be used to check the packets are being sent correctly, the frequency and FLRC settings (in Settings.h) must be the same for the Transmit and Receive program. Sample Serial Monitor output;

10dBm Packet> {packet contents*}  BytesSent,23  CRC,DAAB  TransmitTime,54mS  PacketsSent,1


#### 53\_FLRC\_Receiver &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (SX128X_Examples\Basics folder) 

This is a test receiver for the Fast Long Range Communication (FLRC) mode introduced in the SX128X devices. The program listens for incoming packets using the FLRC settings in the 'Settings.h' file. The pins to access the SX128X device need to be defined in the 'Settings.h' file also.

There is a printout of the valid packets received, the packet is assumed to be in ASCII printable text, if its not ASCII text characters from 0x20 to 0x7F, expect weird things to happen on the Serial Monitor. The LED will flash for each packet received and the buzzer will sound, if fitted.

Sample serial monitor output;

3s  Hello World 1234567890*,CRC,DAAB,RSSI,-73dB,Length,23,Packets,1,Errors,0,IRQreg,6

If there is a packet error it might look like this, which is showing a CRC error,

6s PacketError,RSSI,-103dB,Length,119,Packets,3,Errors,1,IRQreg,46,IRQ\_RX\_DONE,IRQ\_SYNCWORD\_VALID,IRQ\_CRC\_ERROR


#### 58\_FM\_Tone &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Basics folder) 

Transmits a FM tone using the LoRa device that can be picked up on an FM UHF handheld receiver. The tones are not true FM but the UHF receiver does not know that. 


#### 59\_Play\_Star\_Wars\_Tune &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Silly folder) 

A silly program really, but does demonstrate that you can shift a carrier generated by the LoRa device in FSK mode fast enough to play audio tones that can be picked up on an FM UHF handheld receiver. The tones are not true FM but the receiver does not know that. 


#### 60\_LoRa\_Packet\_Logger\_Receiver\_SD &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Diagnostics and Test folder) 


The program listens for incoming packets using the LoRa settings in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

There is a printout and save to SD card of the valid packets received in HEX format. Thus the program can be used to receive and record non-ASCII packets. The LED will flash for each packet received and the buzzer will sound, if fitted. The measured frequency difference between the frequency used by the transmitter and the frequency used by the receiver is shown. If this frequency difference gets to 25% of the set LoRa bandwidth, packet reception will fail. The displayed error can be reduced by using the 'offset' setting in the 'Settings.h' file.

There will be a limit to how fast the logger can receive packets, mainly caused by the delay in writing to SD card, so at high packet rates, packets will be lost. 



#### 103\_LoRa\_Transmitter\_Detailed\_Setup &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Basics folder) 

This is a program that demonstrates the detailed setup of a LoRa test transmitter. A packet containing ASCII text is sent according to the frequency and LoRa settings specified in the Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

The details of the packet sent and any errors are shown on the Arduino IDE Serial Monitor, together with the transmit power used, the packet length and the CRC of the packet. The matching receive program, '104\_LoRa\_Receiver' can be used to check the packets are being sent correctly, the frequency and LoRa settings (in Settings.h) must be the same for the transmitter and receiver programs. Sample Serial Monitor output;

10dBm Packet> Hello World 1234567890*  BytesSent,23  CRC,DAAB  TransmitTime,64mS  PacketsSent,2

Serial monitor baud rate is set at 9600

#### 104\_LoRa\_Receiver\_Detailed\_Setup &emsp; &emsp; &emsp; &emsp; &emsp; &emsp; (Basics folder)

This is a program that demonstrates the detailed setup of a LoRa test receiver. The program listens for incoming packets using the LoRa settings in the 'Settings.h' file. The pins to access the lora device need to be defined in the 'Settings.h' file also.

There is a printout on the Arduino IDE Serial Monitor of the valid packets received, the packet is assumed to be in ASCII printable text, if it's not ASCII text characters from 0x20 to 0x7F, expect weird things to happen on the Serial Monitor. The LED will flash for each packet received and the   buzzer will sound, if fitted. Sample serial monitor output;

7s  Hello World 1234567890*,CRC,DAAB,RSSI,-52dBm,SNR,9dB,Length,23,Packets,5,Errors,0,IRQreg,50

If there is a packet error it might look like this, which is showing a CRC error,

968s PacketError,RSSI,-87dBm,SNR,-11dB,Length,23,Packets,613,Errors,2,IRQreg,70,IRQ\_HEADER\_VALID,IRQ\_CRC\_ERROR,IRQ\_RX\_DONE
  
Serial monitor baud rate is set at 9600.

