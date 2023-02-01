#include "boards.h"
#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.
long startTime = 0; //Used to calc the actual update rate.
long updateCount = 0; //Used to calc the actual update rate.

void setup()
{

    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    if (myGPS.begin(Serial1) == false) { //Connect to the Ublox module using Serial port
        Serial.println(F("Ublox GPS not detected . Please check wiring. Freezing."));
        while (1);
    }

    myGPS.setNavigationFrequency(10); //Set output to 10 times a second

    byte rate = myGPS.getNavigationFrequency(); //Get the update rate of this module
    Serial.print("Current update rate:");
    Serial.println(rate);

    startTime = millis();
}

void loop()
{
    //Query module only every second.
    //The module only responds when a new position is available. This is defined
    //by the update freq.
    if (millis() - lastTime > 25) {
        lastTime = millis(); //Update the timer

        long latitude = myGPS.getLatitude();
        Serial.print(F("Lat: "));
        Serial.print(latitude);

        long longitude = myGPS.getLongitude();
        Serial.print(F(" Long: "));
        Serial.print(longitude);

        updateCount++;

        //Calculate the actual update rate based on the sketch start time and the
        //number of updates we've received.
        Serial.print(F(" Rate: "));
        Serial.print( updateCount / ((millis() - startTime) / 1000.0), 2);
        Serial.print(F("Hz"));

        Serial.println();
    }
}
