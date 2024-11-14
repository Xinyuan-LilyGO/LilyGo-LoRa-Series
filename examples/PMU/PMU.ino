/**
 * @file      PMU.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-08-04
 * @note      The sketch is only suitable for boards carrying AXP192 or AXP2101 chips
 */


#include "LoRaBoards.h"


#if !defined(T_BEAM_S3_SUPREME) && !defined(T_BEAM) 
#error "The sketch is only suitable for boards carrying AXP192 or AXP2101 chips!"
#endif


uint32_t startMillis;

void setup()
{
    // Init PMU
    setupBoards();
}


void loop()
{
    if (millis() > startMillis) {
        Serial.print("isCharging:"); Serial.println(PMU->isCharging() ? "YES" : "NO");
        Serial.print("isDischarge:"); Serial.println(PMU->isDischarge() ? "YES" : "NO");
        Serial.print("isVbusIn:"); Serial.println(PMU->isVbusIn() ? "YES" : "NO");
        Serial.print("getBattVoltage:"); Serial.print(PMU->getBattVoltage()); Serial.println("mV");
        Serial.print("getVbusVoltage:"); Serial.print(PMU->getVbusVoltage()); Serial.println("mV");
        Serial.print("getSystemVoltage:"); Serial.print(PMU->getSystemVoltage()); Serial.println("mV");
        // The battery percentage may be inaccurate at first use, the PMU will automatically
        // learn the battery curve and will automatically calibrate the battery percentage
        // after a charge and discharge cycle
        if (PMU->isBatteryConnect()) {
            Serial.print("getBatteryPercent:"); Serial.print(PMU->getBatteryPercent()); Serial.println("%");
        }
        Serial.println();

        startMillis += millis() + 1000;
    }


    if (pmuInterrupt) {

        pmuInterrupt = false;

        // Get PMU Interrupt Status Register
        uint32_t status = PMU->getIrqStatus();
        Serial.print("STATUS => HEX:");
        Serial.print(status, HEX);
        Serial.print(" BIN:");
        Serial.println(status, BIN);

        if (PMU->isVbusInsertIrq()) {
            Serial.println("isVbusInsert");
        }
        if (PMU->isVbusRemoveIrq()) {
            Serial.println("isVbusRemove");
        }
        if (PMU->isBatInsertIrq()) {
            Serial.println("isBatInsert");
        }
        if (PMU->isBatRemoveIrq()) {
            Serial.println("isBatRemove");
        }
        if (PMU->isPekeyShortPressIrq()) {
            Serial.println("isPekeyShortPress");
        }
        if (PMU->isPekeyLongPressIrq()) {
            Serial.println("isPekeyLongPress");
        }
        if (PMU->isBatChargeDoneIrq()) {
            Serial.println("isBatChargeDone");
        }
        if (PMU->isBatChargeStartIrq()) {
            Serial.println("isBatChargeStart");
        }
        // Clear PMU Interrupt Status Register
        PMU->clearIrqStatus();

    }
    delay(10);

}
