#include "loramac.h"
#include "boards.h"

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);
    Serial.println("LoRa Receiver");
    setupLMIC();
}

void loop()
{
    loopLMIC();
}

