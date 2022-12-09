'''
@license MIT License

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

@file      AXP2101_Interrupt.py
@author    Lewis He (lewishe@outlook.com)
@date      2022-10-20

'''
from AXP2101 import *
import time

SDA = None
SCL = None
I2CBUS = None

if implementation.name == 'micropython':
    from machine import Pin, I2C
    SDA = 21
    SCL = 22
    IRQ = 35
    I2CBUS = I2C(scl=Pin(SCL), sda=Pin(SDA))
if implementation.name == 'circuitpython':
    import digitalio
    import board
    import busio
    SDA = board.IO42
    SCL = board.IO41
    IRQ = board.IO6
    I2CBUS = busio.I2C(SCL, SDA)

irq = None
pmu_flag = False


def __callback(args):
    global pmu_flag
    pmu_flag = True


PMU = AXP2101(I2CBUS, addr=AXP2101_SLAVE_ADDRESS)
print('getID:%s' % hex(PMU.getChipID()))

PMU.disableIRQ(PMU.XPOWERS_AXP2101_ALL_IRQ)
#  Clear all interrupt flags
PMU.clearIrqStatus()
PMU.printIntRegister()

PMU.enableIRQ(PMU.XPOWERS_AXP2101_PKEY_SHORT_IRQ |
              PMU.XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ)
# Print AXP2101 interrupt control register
PMU.printIntRegister()

# PMU.enableIRQ(
#     PMU.XPOWERS_AXP2101_BAT_INSERT_IRQ | PMU.XPOWERS_AXP2101_BAT_REMOVE_IRQ |  # BATTERY
#     PMU.XPOWERS_AXP2101_VBUS_INSERT_IRQ | PMU.XPOWERS_AXP2101_VBUS_REMOVE_IRQ |  # VBUS
#     PMU.XPOWERS_AXP2101_PKEY_SHORT_IRQ | PMU.XPOWERS_AXP2101_PKEY_LONG_IRQ |  # POWER KEY
#     PMU.XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | PMU.XPOWERS_AXP2101_BAT_CHG_START_IRQ  # CHARGE
# )

PMU.enableIRQ(PMU.XPOWERS_AXP2101_BAT_NOR_UNDER_TEMP_IRQ)
# Print AXP2101 interrupt control register
PMU.printIntRegister()

PMU.enableIRQ(PMU.XPOWERS_AXP2101_PKEY_SHORT_IRQ |
              PMU.XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ)
# Print AXP2101 interrupt control register
PMU.printIntRegister()

PMU.enableIRQ(PMU.XPOWERS_AXP2101_BAT_OVER_VOL_IRQ)
# Print AXP2101 interrupt control register
PMU.printIntRegister()

if implementation.name == 'micropython':
    irq = Pin(IRQ, Pin.IN, Pin.PULL_UP)
    irq.irq(trigger=Pin.IRQ_FALLING, handler=__callback)
if implementation.name == 'circuitpython':
    irq = digitalio.DigitalInOut(IRQ)
    irq.switch_to_input()

while True:
    if implementation.name == 'circuitpython':
        if irq.value == False:
            pmu_flag = True

    if pmu_flag:
        pmu_flag = False
        # Get PMU Interrupt Status Register
        status = PMU.getIrqStatus()
        print("STATUS => HEX:")
        print(hex(status))

        if PMU.isDropWarningLevel2Irq():
            print("isDropWarningLevel2")

        if PMU.isDropWarningLevel1Irq():
            print("isDropWarningLevel1")

        if PMU.isGaugeWdtTimeoutIrq():
            print("isWdtTimeout")

        if PMU.isBatChargerOverTemperatureIrq():
            print("isBatChargeOverTemperature")

        if PMU.isBatWorkOverTemperatureIrq():
            print("isBatWorkOverTemperature")

        if PMU.isBatWorkUnderTemperatureIrq():
            print("isBatWorkUnderTemperature")

        if PMU.isVbusInsertIrq():
            print("isVbusInsert")

        if PMU.isVbusRemoveIrq():
            print("isVbusRemove")

        if PMU.isBatInsertIrq():
            print("isBatInsert")

        if PMU.isBatRemoveIrq():
            print("isBatRemove")

        if PMU.isPekeyShortPressIrq():
            print("isPekeyShortPress")

        if PMU.isPekeyLongPressIrq():
            print("isPekeyLongPress")

        if PMU.isPekeyNegativeIrq():
            print("isPekeyNegative")

        if PMU.isPekeyPositiveIrq():
            print("isPekeyPositive")

        if PMU.isWdtExpireIrq():
            print("isWdtExpire")

        if PMU.isLdoOverCurrentIrq():
            print("isLdoOverCurrentIrq")

        if PMU.isBatfetOverCurrentIrq():
            print("isBatfetOverCurrentIrq")

        if PMU.isBatChagerDoneIrq():
            print("isBatChagerDone")

        if PMU.isBatChagerStartIrq():
            print("isBatChagerStart")

        if PMU.isBatDieOverTemperatureIrq():
            print("isBatDieOverTemperature")

        if PMU.isChagerOverTimeoutIrq():
            print("isChagerOverTimeout")

        if PMU.isBatOverVoltageIrq():
            print("isBatOverVoltage")

        # Clear PMU Interrupt Status Register
        PMU.clearIrqStatus()

        # Print AXP2101 interrupt control register
        PMU.printIntRegister()

    time.sleep(0.2)
