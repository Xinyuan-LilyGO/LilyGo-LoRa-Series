/*
  This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
  Base on TinyGPSPlus //https://github.com/mikalhart/TinyGPSPlus
*/

#include <TinyGPS++.h>
#include "utilities.h"

TinyGPSPlus gps;

// For stats that happen every 5 seconds
unsigned long last = 0UL;

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println(F("DeviceExample.ino"));
    Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
    Serial.print(F("Testing TinyGPS++ library v. "));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println(F("by Mikal Hart"));
    Serial.println();
}

void loop()
{
    // This sketch displays information every time a new sentence is correctly encoded.
    while (Serial1.available() > 0)
        gps.encode(Serial1.read());


    if (gps.location.isUpdated()) {
        Serial.print(F("LOCATION   Fix Age="));
        Serial.print(gps.location.age());
        Serial.print(F("ms Raw Lat="));
        Serial.print(gps.location.rawLat().negative ? "-" : "+");
        Serial.print(gps.location.rawLat().deg);
        Serial.print("[+");
        Serial.print(gps.location.rawLat().billionths);
        Serial.print(F(" billionths],  Raw Long="));
        Serial.print(gps.location.rawLng().negative ? "-" : "+");
        Serial.print(gps.location.rawLng().deg);
        Serial.print("[+");
        Serial.print(gps.location.rawLng().billionths);
        Serial.print(F(" billionths],  Lat="));
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(" Long="));
        Serial.println(gps.location.lng(), 6);
    }

    else if (gps.date.isUpdated()) {
        Serial.print(F("DATE       Fix Age="));
        Serial.print(gps.date.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps.date.value());
        Serial.print(F(" Year="));
        Serial.print(gps.date.year());
        Serial.print(F(" Month="));
        Serial.print(gps.date.month());
        Serial.print(F(" Day="));
        Serial.println(gps.date.day());
    }

    else if (gps.time.isUpdated()) {
        Serial.print(F("TIME       Fix Age="));
        Serial.print(gps.time.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps.time.value());
        Serial.print(F(" Hour="));
        Serial.print(gps.time.hour());
        Serial.print(F(" Minute="));
        Serial.print(gps.time.minute());
        Serial.print(F(" Second="));
        Serial.print(gps.time.second());
        Serial.print(F(" Hundredths="));
        Serial.println(gps.time.centisecond());
    }

    else if (gps.speed.isUpdated()) {
        Serial.print(F("SPEED      Fix Age="));
        Serial.print(gps.speed.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps.speed.value());
        Serial.print(F(" Knots="));
        Serial.print(gps.speed.knots());
        Serial.print(F(" MPH="));
        Serial.print(gps.speed.mph());
        Serial.print(F(" m/s="));
        Serial.print(gps.speed.mps());
        Serial.print(F(" km/h="));
        Serial.println(gps.speed.kmph());
    }

    else if (gps.course.isUpdated()) {
        Serial.print(F("COURSE     Fix Age="));
        Serial.print(gps.course.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps.course.value());
        Serial.print(F(" Deg="));
        Serial.println(gps.course.deg());
    }

    else if (gps.altitude.isUpdated()) {
        Serial.print(F("ALTITUDE   Fix Age="));
        Serial.print(gps.altitude.age());
        Serial.print(F("ms Raw="));
        Serial.print(gps.altitude.value());
        Serial.print(F(" Meters="));
        Serial.print(gps.altitude.meters());
        Serial.print(F(" Miles="));
        Serial.print(gps.altitude.miles());
        Serial.print(F(" KM="));
        Serial.print(gps.altitude.kilometers());
        Serial.print(F(" Feet="));
        Serial.println(gps.altitude.feet());
    }

    else if (gps.satellites.isUpdated()) {
        Serial.print(F("SATELLITES Fix Age="));
        Serial.print(gps.satellites.age());
        Serial.print(F("ms Value="));
        Serial.println(gps.satellites.value());
    }

    else if (gps.hdop.isUpdated()) {
        Serial.print(F("HDOP       Fix Age="));
        Serial.print(gps.hdop.age());
        Serial.print(F("ms raw="));
        Serial.print(gps.hdop.value());
        Serial.print(F(" hdop="));
        Serial.println(gps.hdop.hdop());
    }

    else if (millis() - last > 5000) {
        Serial.println();
        if (gps.location.isValid()) {
            static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
            double distanceToLondon =
                TinyGPSPlus::distanceBetween(
                    gps.location.lat(),
                    gps.location.lng(),
                    LONDON_LAT,
                    LONDON_LON);
            double courseToLondon =
                TinyGPSPlus::courseTo(
                    gps.location.lat(),
                    gps.location.lng(),
                    LONDON_LAT,
                    LONDON_LON);

            Serial.print(F("LONDON     Distance="));
            Serial.print(distanceToLondon / 1000, 6);
            Serial.print(F(" km Course-to="));
            Serial.print(courseToLondon, 6);
            Serial.print(F(" degrees ["));
            Serial.print(TinyGPSPlus::cardinal(courseToLondon));
            Serial.println(F("]"));
        }

        Serial.print(F("DIAGS      Chars="));
        Serial.print(gps.charsProcessed());
        Serial.print(F(" Sentences-with-Fix="));
        Serial.print(gps.sentencesWithFix());
        Serial.print(F(" Failed-checksum="));
        Serial.print(gps.failedChecksum());
        Serial.print(F(" Passed-checksum="));
        Serial.println(gps.passedChecksum());

        if (gps.charsProcessed() < 10)
            Serial.println(F("WARNING: No GPS data.  Check wiring."));

        last = millis();
        Serial.println();
    }
}

