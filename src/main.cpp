#include "board_def.h"
#include "SPI.h"
#include <Wire.h>
#include "axp20x.h"

#define SerialGPS Serial1

AXP20X_Class axp;

bool findPower = false;

void setup()
{
    Serial.begin(115200);

    Wire.begin(I2C_SDA, I2C_SCL);

    SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    findPower = !axp.begin(Wire, AXP192_SLAVE_ADDRESS);
    if (findPower) {
        // ! DC1 is the power supply of ESP32, do not control it
        // axp.setDCDC1Voltage(3300);  //esp32 core VDD    3v3
        // axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);

        //Setting LDO2 and LOD3 3300mV
        axp.setLDO2Voltage(3300);   //LORA VDD     3v3
        axp.setLDO3Voltage(3300);   //GPS VDD      3v3

        //Enable LDO2 , It controls the lora power
        axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
        //Enable LDO3, It controls the GPS power
        axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);

        //Idle the power supply, turn it off
        axp.setPowerOutPut(AXP192_DCDC2, AXP202_OFF);
        axp.setPowerOutPut(AXP192_EXTEN, AXP202_OFF);
    } else {
        Serial.println("AXP192 not found");
    }
}

void loop()
{
    while (SerialGPS.available()) {
        Serial.write(SerialGPS.read());
    }
}
