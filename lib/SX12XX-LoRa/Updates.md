## Updates - 30/12/20

A number of updates have been applied to the SX127X part of the library, in part to cure some known issues and also to add additional functionality. A list of the changes made is below;

1. Correct a problem with LORA\_IQ\_INVERTED mode not working when configured.
2. Apply some of the SX1276\_77\_8\_ErrataNote\_1\_1 fixes. '2.1 Sensitivity Optimization with a 500 kHz Bandwidth'. Note that '2.3 Receiver Spurious Reception of a LoRa Signal' has not yet been implemented, although the code is present, further testing of this 'fix' is required.  
3. Apply the revised RSSI and SNR value calculations to match the latest data sheet. The RSSI value was changed to be int16_t type since it can (now) go lower than -127dBm. 
4. Remove the private variables _PacketRSSI and _PacketSNR, they are not needed since they can be recovered direct from a SX127x register. 
5. Changes to the FSKRTTY transmit routines so that they automatically cope with transmission during a micros() roll-over. 
6. Make changes to several of the example programs that use a GPS to be able to cope automatically with a millis() roll-over
7.  Added support for SX127x devices that use the RFO\_LF or RFO\_HF ports for the transmit antenna. No support yet for an SX1272 using the RFO ports.
8.  Added 'AFSKRTTY2.h' library file to be used as the standard AFSKRTTY library for upload of HAB payloads into a PC. This version uses micros() to toggle a  pin for audio and has been tested on ATMega328, DUE and ESP32. The use of tone() would be preferred but some Arduino platforms don't support it.
9.  Changed the GPS library programs 'UBLOXSerialGPS.h', 'UBLOXI2CGPS.h' and 'QuectelSerialGPS.h' so that they have compatible functions and that the same library file can be used for both softwareserial and hardware serial.
10.  A function was added, doAFCPPM(); which adjusts the LoRa devices internal oscillator PPM offset. 
11. Adjusted the calibrateImage(); function to operate correctly with fast processors.
12. Add function to read current RSSI. 
13. Add an overloaded begin(int8\_t pinNSS, uint8\_t device); function for use with ESP32CAM.
14. Change Receive and Transmit functions to allow for millis() roll-over at 50days. 

### Stuart Robinson
