/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 23/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Pro Mini,
//be sure to change the definitions to match your own setup. Some pins such as DIO1,
//DIO2 are not used by this particular sketch so they are set to -1 and not connected.

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define LED1 8                                  //on board LED, high for on
#define DIO0 3                                  //DIO0 pin on LoRa device, used for RX and TX done 

#define LORA_DEVICE DEVICE_SX1278               //we need to define the device we are using


//*******  Setup Direct Modem Parameters Here ! ***************

const uint32_t Frequency = 434000000;           //frequency of transmissions in hertz
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const int8_t TXpower = 2;                       //LoRa transmit power in dBm

