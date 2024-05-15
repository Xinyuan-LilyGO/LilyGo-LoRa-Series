/**
 * @file      OTAA.ino
 * LMIC-Arduino library only support SX1276 Radio 
 * Test Devices : 
 *  T_MOTION
 *  T3_S3_V1_2_SX1276
 *  T_BEAM_SX1276
 */
#include "loramac.h"
#include "LoRaBoards.h"

void setup()
{
    setupBoards();
    // When the power is turned on, a delay is required.
    delay(1500);
    Serial.println("LMIC-Arduino library only support SX1276 Radio ...");
    Serial.println("LMIC-Arduino library only support SX1276 Radio ...");
    Serial.println("LMIC-Arduino library only support SX1276 Radio ...");
    Serial.println("LMIC-Arduino library only support SX1276 Radio ...");
    setupLMIC();
}

void loop()
{
    loopLMIC();
}

