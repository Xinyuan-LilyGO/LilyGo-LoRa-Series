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

@file      AXP192_AllFunction.py
@author    Lewis He (lewishe@outlook.com)
@date      2022-10-20

'''
from AXP192 import *
import time

SDA = None
SCL = None
IRQ = None
I2CBUS = None
if implementation.name == 'micropython':
    from machine import Pin, I2C
    SDA = 21
    SCL = 22
    IRQ = 35
    I2CBUS = I2C(0, scl=Pin(SCL), sda=Pin(SDA))
if implementation.name == 'circuitpython':
    import digitalio
    from board import *
    import busio
    SDA = IO15
    SCL = IO7
    IRQ = IO6
    I2CBUS = busio.I2C(SCL, SDA)

pmu_flag = False
irq = None


def __callback(args):
    global pmu_flag
    pmu_flag = True
    # print('callback')


PMU = AXP192(I2CBUS, addr=AXP192_SLAVE_ADDRESS)

print('getID:%s' % hex(PMU.getChipID()))


# Set the minimum system operating voltage inside the PMU,
# below this value will shut down the PMU
# Range: 2600~3300mV
PMU.setSysPowerDownVoltage(2600)

# Set the minimum common working voltage of the PMU VBUS input,
# below this value will turn off the PMU
PMU.setVbusVoltageLimit(PMU.XPOWERS_AXP192_VBUS_VOL_LIM_4V5)

# Turn off USB input current limit
PMU.setVbusCurrentLimit(PMU.XPOWERS_AXP192_VBUS_CUR_LIM_OFF)


#  DC1 700~3500mV, IMAX=1.2A
PMU.setDC1Voltage(3300)
# print('DC1  : %s   Voltage:%u mV ' % PMU.isEnableDC1()  ? '+': '-', PMU.getDC1Voltage())
print('DC1  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC1()], PMU.getDC1Voltage()))


#  DC2 700~2750 mV, IMAX=1.6A
PMU.setDC2Voltage(2750)
print(PMU.isEnableDC2())
print('DC2  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC2()], PMU.getDC2Voltage()))

#  DC3 700~3500mV,IMAX=0.7A
PMU.setDC3Voltage(3300)
print('DC3  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC3()], PMU.getDC3Voltage()))

# LDO2 1800~3300 mV, 100mV/step, IMAX=200mA
PMU.setLDO2Voltage(1800)

# LDO3 1800~3300 mV, 100mV/step, IMAX=200mA
PMU.setLDO3Voltage(1800)

# LDOio 1800~3300 mV, 100mV/step, IMAX=50mA
PMU.setLDOioVoltage(3300)

# Enable power output channel
PMU.enableDC1()
PMU.enableDC2()
PMU.enableDC3()
PMU.enableLDO2()
PMU.enableLDO3()
PMU.enableLDOio()

print('===================================')
print('DC1    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC1()], PMU.getDC1Voltage()))
print('DC2    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC2()], PMU.getDC2Voltage()))
print('DC3    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC3()], PMU.getDC3Voltage()))
print('===================================')

print('LDO2  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableLDO2()], PMU.getLDO2Voltage()))
print('LDO3  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableLDO3()], PMU.getLDO3Voltage()))
print('LDOio  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableLDOio()], PMU.getLDOioVoltage()))
print('===================================')


#  Set the time of pressing the button to turn off
powerOff = ['4', '6', '8', '10']
PMU.setPowerKeyPressOffTime(PMU.XPOWERS_POWEROFF_6S)
opt = PMU.getPowerKeyPressOffTime()
print('PowerKeyPressOffTime: %s Sceond' % powerOff[opt])


#  Set the button power-on press time
powerOn = ['128ms', '512ms', '1000ms', '2000ms']
PMU.setPowerKeyPressOnTime(PMU.XPOWERS_POWERON_2S)
opt = PMU.getPowerKeyPressOnTime()
print('PowerKeyPressOnTime: %s ' % powerOn[opt])


print('===================================')

#  It is necessary to disable the detection function of the TS pin on the board
#  without the battery temperature detection function, otherwise it will cause abnormal charging
PMU.disableTSPinMeasure()

PMU.enableTemperatureMeasure()
#  PMU.disableTemperatureMeasure()

#  Enable internal ADC detection
PMU.enableBattDetection()
PMU.enableVbusVoltageMeasure()
PMU.enableBattVoltageMeasure()
PMU.enableSystemVoltageMeasure()

'''
The default setting is CHGLED is automatically controlled by the PMU.
- XPOWERS_CHG_LED_OFF,
- XPOWERS_CHG_LED_BLINK_1HZ,
- XPOWERS_CHG_LED_BLINK_4HZ,
- XPOWERS_CHG_LED_ON,
- XPOWERS_CHG_LED_CTRL_CHG,
'''
PMU.setChargingLedMode(PMU.XPOWERS_CHG_LED_OFF)


#  Disable all interrupts
PMU.disableIRQ(PMU.XPOWERS_AXP192_ALL_IRQ)
#  Clear all interrupt flags
PMU.clearIrqStatus()
#  Enable the required interrupt function
PMU.enableIRQ(
    PMU.XPOWERS_AXP192_BAT_INSERT_IRQ | PMU.XPOWERS_AXP192_BAT_REMOVE_IRQ |   # BATTERY
    PMU.XPOWERS_AXP192_VBUS_INSERT_IRQ | PMU.XPOWERS_AXP192_VBUS_REMOVE_IRQ |   # VBUS
    PMU.XPOWERS_AXP192_PKEY_SHORT_IRQ | PMU.XPOWERS_AXP192_PKEY_LONG_IRQ |   # POWER KEY
    PMU.XPOWERS_AXP192_BAT_CHG_DONE_IRQ | PMU.XPOWERS_AXP192_BAT_CHG_START_IRQ |    # CHARGE
    #  PMU.XPOWERS_AXP192_PKEY_NEGATIVE_IRQ | PMU.XPOWERS_AXP192_PKEY_POSITIVE_IRQ   |   # POWER KEY
    PMU.XPOWERS_AXP192_TIMER_TIMEOUT_IRQ               # Timer
)


#  Set constant current charge current limit
PMU.setChargerConstantCurr(PMU.XPOWERS_AXP192_CHG_CUR_280MA)
#  Set stop charging termination current
PMU.setChargerTerminationCurr(PMU.XPOWERS_AXP192_CHG_ITERM_LESS_10_PERCENT)

#  Set charge cut-off voltage
PMU.setChargeTargetVoltage(PMU.XPOWERS_AXP192_CHG_VOL_4V2)


PMU.clearIrqStatus()

# Set the timing after one minute, the isWdtExpireIrq will be triggered in the loop interrupt function
PMU.setTimerout(1)

data = [1, 2, 3, 4]
print('Write buffer to pmu')
PMU.writeDataBuffer(data, 4)
print('Read buffer from pmu')
tmp = PMU.readDataBuffer(4)
print(tmp)


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
        mask = PMU.getIrqStatus()
        print('pmu_flag:', end='')
        print(bin(mask))

        if PMU.isAcinOverVoltageIrq():
            print("IRQ ---> isAcinOverVoltageIrq")
        if PMU.isAcinInserIrq():
            print("IRQ ---> isAcinInserIrq")
        if PMU.isAcinRemoveIrq():
            print("IRQ ---> isAcinRemoveIrq")
        if PMU.isVbusOverVoltageIrq():
            print("IRQ ---> isVbusOverVoltageIrq")
        if PMU.isVbusInsertIrq():
            print("IRQ ---> isVbusInsertIrq")
        if PMU.isVbusRemoveIrq():
            print("IRQ ---> isVbusRemoveIrq")
        if PMU.isVbusLowVholdIrq():
            print("IRQ ---> isVbusLowVholdIrq")
        if PMU.isBatInsertIrq():
            print("IRQ ---> isBatInsertIrq")
        if PMU.isBatRemoveIrq():
            print("IRQ ---> isBatRemoveIrq")
        if PMU.isBattEnterActivateIrq():
            print("IRQ ---> isBattEnterActivateIrq")
        if PMU.isBattExitActivateIrq():
            print("IRQ ---> isBattExitActivateIrq")
        if PMU.isBatChagerStartIrq():
            print("IRQ ---> isBatChagerStartIrq")
        if PMU.isBatChagerDoneIrq():
            print("IRQ ---> isBatChagerDoneIrq")
        if PMU.isBattTempHighIrq():
            print("IRQ ---> isBattTempHighIrq")
        if PMU.isBattTempLowIrq():
            print("IRQ ---> isBattTempLowIrq")
        if PMU.isChipOverTemperatureIrq():
            print("IRQ ---> isChipOverTemperatureIrq")
        if PMU.isChargingCurrentLessIrq():
            print("IRQ ---> isChargingCurrentLessIrq")
        if PMU.isDC1VoltageLessIrq():
            print("IRQ ---> isDC1VoltageLessIrq")
        if PMU.isDC2VoltageLessIrq():
            print("IRQ ---> isDC2VoltageLessIrq")
        if PMU.isDC3VoltageLessIrq():
            print("IRQ ---> isDC3VoltageLessIrq")
        if PMU.isPekeyShortPressIrq():
            print("IRQ ---> isPekeyShortPress")
        if PMU.isPekeyLongPressIrq():
            print("IRQ ---> isPekeyLongPress")
        if PMU.isNOEPowerOnIrq():
            print("IRQ ---> isNOEPowerOnIrq")
        if PMU.isNOEPowerDownIrq():
            print("IRQ ---> isNOEPowerDownIrq")
        if PMU.isVbusEffectiveIrq():
            print("IRQ ---> isVbusEffectiveIrq")
        if PMU.isVbusInvalidIrq():
            print("IRQ ---> isVbusInvalidIrq")
        if PMU.isVbusSessionIrq():
            print("IRQ ---> isVbusSessionIrq")
        if PMU.isVbusSessionEndIrq():
            print("IRQ ---> isVbusSessionEndIrq")
        if PMU.isLowVoltageLevel2Irq():
            print("IRQ ---> isLowVoltageLevel2Irq")
        if PMU.isWdtExpireIrq():
            print("IRQ ---> isWdtExpire")
            # Clear the timer state and continue to the next timer
            PMU.clearTimerFlag()

        if PMU.isGpio2EdgeTriggerIrq():
            print("IRQ ---> isGpio2EdgeTriggerIrq")
        if PMU.isGpio1EdgeTriggerIrq():
            print("IRQ ---> isGpio1EdgeTriggerIrq")
        if PMU.isGpio0EdgeTriggerIrq():
            print("IRQ ---> isGpio0EdgeTriggerIrq")
        # Clear PMU Interrupt Status Register
        PMU.clearIrqStatus()

    PMU.setChargingLedMode((PMU.XPOWERS_CHG_LED_OFF, PMU.XPOWERS_CHG_LED_ON)[
                           PMU.getChargingLedMode() == PMU.XPOWERS_CHG_LED_OFF])

    print("getBattVoltage:{0}mV".format(PMU.getBattVoltage()))
    print("getSystemVoltage:{0}mV".format(PMU.getSystemVoltage()))
    print("getBatteryPercent:{0}%".format(PMU.getBatteryPercent()))
    print("getTemperature:{0}%".format(PMU.getTemperature()))

    print("isCharging:{0}".format(PMU.isCharging()))
    print("isDischarge:{0}".format(PMU.isDischarge()))
    print("isVbusIn:{0}".format(PMU.isVbusIn()))

    time.sleep(0.8)
