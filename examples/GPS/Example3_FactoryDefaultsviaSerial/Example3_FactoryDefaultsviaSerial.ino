/*
  Test baud rate changes on serial, factory reset, and hard reset.
  Base on SparkFun_Ublox_Arduino_Library //https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
*/

#include "SparkFun_Ublox_Arduino_Library.h"
#include "utilities.h"

SFE_UBLOX_GPS myGPS;
int state = 0; // steps through auto-baud, reset, etc states

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);
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
            Serial1.begin(38400, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
            if (myGPS.begin(Serial1)) break;

            delay(100);
            Serial.println("GPS: trying 9600 baud");
            Serial1.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

            if (myGPS.begin(Serial1)) {
                Serial.println("GPS: connected at 9600 baud, switching to 38400");
                myGPS.setSerialRate(38400);
                delay(100);
            } else {
                delay(2000); //Wait a bit before trying again to limit the Serial output flood
            }
        } while (1);
        myGPS.setUART1Output(COM_TYPE_UBX); //Set the UART port to output UBX only
        myGPS.saveConfiguration(); //Save the current settings to flash and BBR
        Serial.println("GPS serial connected, saved config");
        state++;
        break;
    case 1: // hardReset, expect to see GPS back at 38400 baud
        Serial.println("Issuing hardReset (cold start)");
        myGPS.hardReset();
        delay(1000);
        Serial1.begin(38400, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

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
        Serial1.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
        if (myGPS.begin(Serial1)) {
            Serial.println("Success.");
            state++;
        } else {
            Serial.println("*** GPS did not come back at 9600 baud, starting over.");
            state = 0;
        }
        break;
    case 3: // print version info
        state = 0;
        Serial.println("Reset Done");
        while (1) {
            delay(10000);
        }
    }
    delay(1000);
}
