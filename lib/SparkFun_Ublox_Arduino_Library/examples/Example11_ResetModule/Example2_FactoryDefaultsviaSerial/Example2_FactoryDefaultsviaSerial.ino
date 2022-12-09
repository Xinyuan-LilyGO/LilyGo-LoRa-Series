/*
  Test baud rate changes on serial, factory reset, and hard reset.
  By: Thorsten von Eicken
  Date: January 29rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to reset the U-Blox module to factory defaults over serial.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Connect the U-Blox serial port to Serial1
  If you're using an Uno or don't have a 2nd serial port (Serial1), consider using software serial
  Open the serial monitor at 115200 baud to see the output
*/

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

int state = 0; // steps through auto-baud, reset, etc states

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun Ublox Example");
}

void loop()
{
    Serial.print("===== STATE ");
    Serial.println(state);
    switch (state) {
    case 0: // auto-baud connection, then switch to 38400 and save config
        do {
            Serial.println("GPS: trying 38400 baud");
            Serial1.begin(38400);
            if (myGPS.begin(Serial1)) break;

            delay(100);
            Serial.println("GPS: trying 9600 baud");
            Serial1.begin(9600);
            if (myGPS.begin(Serial1)) {
                Serial.println("GPS: connected at 9600 baud, switching to 38400");
                myGPS.setSerialRate(38400);
                delay(100);
            } else {
                delay(2000); //Wait a bit before trying again to limit the Serial output flood
            }
        } while(1);
        myGPS.setUART1Output(COM_TYPE_UBX); //Set the UART port to output UBX only
        myGPS.saveConfiguration(); //Save the current settings to flash and BBR
        Serial.println("GPS serial connected, saved config");
        state++;
        break;
    case 1: // hardReset, expect to see GPS back at 38400 baud
        Serial.println("Issuing hardReset (cold start)");
        myGPS.hardReset();
        delay(1000);
        Serial1.begin(38400);
        if (myGPS.begin(Serial1)) {
            Serial.println("Success.");
            state++;
        } else {
            Serial.println("*** GPS did not respond at 38400 baud, starting over.");
            state = 0;
        }
        break;
    case 2: // factoryReset, expect to see GPS back at 9600 baud
        Serial.println("Issuing factoryReset");
        myGPS.factoryReset();
        delay(2000); // takes more than one second... a loop to resync would be best
        Serial1.begin(9600);
        if (myGPS.begin(Serial1)) {
            Serial.println("Success.");
            state++;
        } else {
            Serial.println("*** GPS did not come back at 9600 baud, starting over.");
            state = 0;
        }
        break;
    case 3: // print version info
        Serial.print("GPS protocol version: ");
        Serial.print(myGPS.getProtocolVersionHigh());
        Serial.print('.');
        Serial.print(myGPS.getProtocolVersionLow());
        state = 0;
    }
    delay(1000);
}
