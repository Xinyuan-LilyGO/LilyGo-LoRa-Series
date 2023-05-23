/*
MIT License

Copyright (c) 2022 lewis he

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
! WARN:
Please do not run the example without knowing the external load voltage of the PMU,
it may burn your external load, please check the voltage setting before running the example,
if there is any loss, please bear it by yourself
*/
#ifndef XPOWERS_NO_ERROR
#error "Running this example is known to not damage the device! Please go and uncomment this!"
#endif
// Defined using AXP2102
#define XPOWERS_CHIP_AXP2101

#include <Wire.h>
#include <Arduino.h>
#include "XPowersLib.h"

#ifndef CONFIG_PMU_SDA
#define CONFIG_PMU_SDA 21
#endif

#ifndef CONFIG_PMU_SCL
#define CONFIG_PMU_SCL 22
#endif

#ifndef CONFIG_PMU_IRQ
#define CONFIG_PMU_IRQ 35
#endif

bool  pmu_flag = 0;
XPowersPMU PMU;


const uint8_t i2c_sda = CONFIG_PMU_SDA;
const uint8_t i2c_scl = CONFIG_PMU_SCL;
const uint8_t pmu_irq_pin = CONFIG_PMU_IRQ;

void setFlag(void)
{
    pmu_flag = true;
}

void setup()
{
    Serial.begin(115200);

    bool result = PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);

    if (result == false) {
        Serial.println("PMU is not online..."); while (1)delay(50);
    }

    Serial.printf("getID:0x%x\n", PMU.getChipID());

    // Force add pull-up
    pinMode(pmu_irq_pin, INPUT_PULLUP);
    attachInterrupt(pmu_irq_pin, setFlag, FALLING);


    // Disable all interrupts
    PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);

    // Print interrupt register
    PMU.printIntRegister(&Serial);

    // Clear all interrupt flags
    PMU.clearIrqStatus();

    // Enable the required interrupt function
    // PMU.enableIRQ(
    //     XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
    //     XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
    //     XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
    //     XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
    //     // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
    // );

    PMU.enableIRQ(XPOWERS_AXP2101_BAT_NOR_UNDER_TEMP_IRQ);
    // Print AXP2101 interrupt control register
    PMU.printIntRegister(&Serial);

    PMU.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ | XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ);
    // Print AXP2101 interrupt control register
    PMU.printIntRegister(&Serial);

    PMU.enableIRQ(XPOWERS_AXP2101_BAT_OVER_VOL_IRQ);
    // Print AXP2101 interrupt control register
    PMU.printIntRegister(&Serial);

    // delay(30000);
}

void loop()
{

    if (pmu_flag) {

        pmu_flag = false;

        // Get PMU Interrupt Status Register
        uint32_t status = PMU.getIrqStatus();
        Serial.print("STATUS => HEX:");
        Serial.print(status, HEX);
        Serial.print(" BIN:");
        Serial.println(status, BIN);

        if (PMU.isDropWarningLevel2Irq()) {
            Serial.println("isDropWarningLevel2");
        }
        if (PMU.isDropWarningLevel1Irq()) {
            Serial.println("isDropWarningLevel1");
        }
        if (PMU.isGaugeWdtTimeoutIrq()) {
            Serial.println("isWdtTimeout");
        }
        if (PMU.isBatChargerOverTemperatureIrq()) {
            Serial.println("isBatChargeOverTemperature");
        }
        if (PMU.isBatWorkOverTemperatureIrq()) {
            Serial.println("isBatWorkOverTemperature");
        }
        if (PMU.isBatWorkUnderTemperatureIrq()) {
            Serial.println("isBatWorkUnderTemperature");
        }
        if (PMU.isVbusInsertIrq()) {
            Serial.println("isVbusInsert");
        }
        if (PMU.isVbusRemoveIrq()) {
            Serial.println("isVbusRemove");
        }
        if (PMU.isBatInsertIrq()) {
            Serial.println("isBatInsert");
        }
        if (PMU.isBatRemoveIrq()) {
            Serial.println("isBatRemove");
        }
        if (PMU.isPekeyShortPressIrq()) {
            Serial.println("isPekeyShortPress");
        }
        if (PMU.isPekeyLongPressIrq()) {
            Serial.println("isPekeyLongPress");
        }
        if (PMU.isPekeyNegativeIrq()) {
            Serial.println("isPekeyNegative");
        }
        if (PMU.isPekeyPositiveIrq()) {
            Serial.println("isPekeyPositive");
        }
        if (PMU.isWdtExpireIrq()) {
            Serial.println("isWdtExpire");
        }
        if (PMU.isLdoOverCurrentIrq()) {
            Serial.println("isLdoOverCurrentIrq");
        }
        if (PMU.isBatfetOverCurrentIrq()) {
            Serial.println("isBatfetOverCurrentIrq");
        }
        if (PMU.isBatChagerDoneIrq()) {
            Serial.println("isBatChagerDone");
        }
        if (PMU.isBatChagerStartIrq()) {
            Serial.println("isBatChagerStart");
        }
        if (PMU.isBatDieOverTemperatureIrq()) {
            Serial.println("isBatDieOverTemperature");
        }
        if (PMU.isChagerOverTimeoutIrq()) {
            Serial.println("isChagerOverTimeout");
        }
        if (PMU.isBatOverVoltageIrq()) {
            Serial.println("isBatOverVoltage");
        }

        // Clear PMU Interrupt Status Register
        PMU.clearIrqStatus();

        // Print AXP2101 interrupt control register
        PMU.printIntRegister(&Serial);

    }
    delay(10);
}

