/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 21/03/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/
/*******************************************************************************************************
  Program Operation - This program reads the internal temperature sensor in the SX127X range of devices.
  The temeprature sensor needs calibrating, so run a test, check what the error is (could be +\- 10C or
  more) and calculate the value for the temperature_compensate constant. This constant will be different
  for each individual device.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V1.0"

//These are the pin definitions for one of the Tracker boards, be sure to change them to match
//your own setup.

#define NSS 10                              //SX127X device select
#define NRESET 9                            //SX127X reset pin
#define LED1 8                              //for on board LED, put high for on

#define LORA_DEVICE DEVICE_SX1278           //this is the device we are using 

#include <SPI.h>
#include <SX127XLT.h>                       //load the appropriate library 

SX127XLT LT;

const int8_t temperature_compensate = 0;    //value, degrees centigrade, to add to read temperature for calibration. Can be negative
                                            //this compensate value will be different for each LoRa device instance, so best to
                                            //label and record values for each device


void loop()
{
  int8_t temperature_register;
  LT.resetDevice();

  temperature_register = LT.getDeviceTemperature();

  Serial.print(F("Temperature Register Raw "));
  Serial.println(temperature_register);
  Serial.print(F("Temperature Compensated "));
  Serial.print(temperature_register + temperature_compensate);
  Serial.println(F("c"));
  Serial.println();
  delay(2000);
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;
  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);                                   //setup pin as output for indicator LED
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start
  
  Serial.begin(9600);
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("39_LoRa_SX127x_Temperature_Read Starting"));

  
  SPI.begin();

  //setup hardware pins used by device, then check if device is found
  if (LT.begin(NSS, NRESET, -1, -1, -1, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  Serial.println(F("Temperature Sensor Ready"));
  Serial.println();
}

