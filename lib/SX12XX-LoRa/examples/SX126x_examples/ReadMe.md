# SX126X Library
<cr>


This part of the SX12XX library supports the SX1261, SX1262 and SX1268 UHF LoRa devices.. 

The objective of the SX12XX library is to allow the same program sketches to be used across the range of UHF lora modules such as SX126x and SX127x as well as the 2.4Ghz SX128x modules.

The SX126X part of the library was tested on both NiceRF and Dorji modules. 

The Library **has not** been tested on the Semtech SX1261MB2BAS or similar development boards, so consider them unsupported. These development modules are not low cost and are not in a form (and too big) to be of practical use to the author of this library. 


###Considerations for pin usage

There is a range of SX126X modules available and they have slightly different pins usage. 

The library only supports the SPI based LoRa modules and these all require that the SPI bus pins, SCK, MOSI and MISO are connected. All modules also need a NSS (chip select pin) and NRESET (reset) pin. All devices need the RFBUSY pin to be used also. 

Of the LoRa devices DIO pins the SX126X library in standard form only uses DIO1. The Dorji DRF1262 and DRF1268 modules have an additional SW pin which must be configured and used since it provides power to the antenna switch on these modules. 

####RX and TX enable pins

Some SX126x modules with RF power amplifiers may have RX and TX enable pins. Whilst the SX126X library code contains functions that should switch RX and TX enable pins that are defined appropriately, this code has never been fully tested since at the time of writing I had no such SX126x modules available. Donations of suitable modules for testing will be accepted. 

Thus a begin function that initialised all possible permutations of pins would look like this;

begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX\_EN, TX\_EN, SW, LORA\_DEVICE);

Clearly the above begin statement is somewhat cumbersome and could potentially be shortened for the NiceRF S1262 devices to;

begin(NSS, NRESET, RFBUSY, DIO1, LORA\_DEVICE);

And shortened for the Dorji devices to;

begin(NSS, NRESET, RFBUSY, DIO1, SW, LORA\_DEVICE);

Which is a lot more manageable.

The first edition of the SX126X part of the library did use use a begin function in the examples of;

begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, SW, LORA\_DEVICE) 

This format is still valid so if you have written your own programs using the earlier library these programs do not need changing. You could not have used the newer constructs of the begin command (to support the newer devices) in your programs since the newer constructs did not exist in the older version library. 

Accepted its all a bit confusing, but regrettably module manufacturers have different ideas about design. 


