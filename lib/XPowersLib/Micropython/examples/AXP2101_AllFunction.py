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

@file      AXP2101_AllFunction.py
@author    Lewis He (lewishe@outlook.com)
@date      2022-10-20

'''
from AXP2101 import *
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
    I2CBUS = I2C(scl=Pin(SCL), sda=Pin(SDA))
if implementation.name == 'circuitpython':
    import digitalio
    import board
    import busio
    SDA = board.IO42
    SCL = board.IO41
    IRQ = board.IO6
    I2CBUS = busio.I2C(SCL, SDA)
pmu_flag = False
irq = None


def __callback(args):
    global pmu_flag
    pmu_flag = True
    # print('callback')


PMU = AXP2101(I2CBUS, addr=AXP2101_SLAVE_ADDRESS)

id = PMU.getChipID()
if id != XPOWERS_AXP2101_CHIP_ID:
    print("PMU is not online...")
    while True:
        pass

print('getID:%s' % hex(PMU.getChipID()))

#  Set the minimum common working voltage of the PMU VBUS input,
#  below this value will turn off the PMU
PMU.setVbusVoltageLimit(PMU.XPOWERS_AXP2101_VBUS_VOL_LIM_4V36)

#  Set the maximum current of the PMU VBUS input,
#  higher than this value will turn off the PMU
PMU.setVbusCurrentLimit(PMU.XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA)

#  Get the VSYS shutdown voltage
vol = PMU.getSysPowerDownVoltage()
print('->  getSysPowerDownVoltage:%u' % vol)

#  Set VSY off voltage as 2600mV, Adjustment range 2600mV ~ 3300mV
PMU.setSysPowerDownVoltage(2600)

vol = PMU.getSysPowerDownVoltage()
print('->  getSysPowerDownVoltage:%u' % vol)

#  DC1 IMAX = 2A
#  1500~3400mV, 100mV/step, 20steps
PMU.setDC1Voltage(3300)
# print('DC1  : %s   Voltage:%u mV ' % PMU.isEnableDC1()  ? '+': '-', PMU.getDC1Voltage())
print('DC1  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC1()], PMU.getDC1Voltage()))


#  DC2 IMAX = 2A
#  500~1200mV  10mV/step, 71steps
#  1220~1540mV 20mV/step, 17steps
PMU.setDC2Voltage(1000)
print(PMU.isEnableDC2())
print('DC2  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC2()], PMU.getDC2Voltage()))

#  DC3 IMAX = 2A
#  500~1200mV, 10mV/step, 71steps
#  1220~1540mV, 20mV/step, 17steps
#  1600~3400mV, 100mV/step, 19steps
PMU.setDC3Voltage(3300)
print('DC3  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC3()], PMU.getDC3Voltage()))

#  DCDC4 IMAX = 1.5A
#  500~1200mV, 10mV/step, 71steps
#  1220~1840mV, 20mV/step, 32steps
PMU.setDC4Voltage(1000)
print('DC4  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC4()], PMU.getDC4Voltage()))

#  DC5 IMAX = 2A
#  1200mV
#  1400~3700mV, 100mV/step, 24steps
PMU.setDC5Voltage(3300)
print('DC5  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC5()], PMU.getDC5Voltage()))

# ALDO1 IMAX = 300mA
# 500~3500mV, 100mV/step, 31steps
PMU.setALDO1Voltage(3300)

# ALDO2 IMAX = 300mA
# 500~3500mV, 100mV/step, 31steps
PMU.setALDO2Voltage(3300)

# ALDO3 IMAX = 300mA
# 500~3500mV, 100mV/step, 31steps
PMU.setALDO3Voltage(3300)

# ALDO4 IMAX = 300mA
# 500~3500mV, 100mV/step, 31steps
PMU.setALDO4Voltage(3300)

# BLDO1 IMAX = 300mA
# 500~3500mV, 100mV/step, 31steps
PMU.setBLDO1Voltage(3300)

# BLDO2 IMAX = 300mA
# 500~3500mV, 100mV/step, 31steps
PMU.setBLDO2Voltage(3300)

# CPUSLDO IMAX = 30mA
# 500~1400mV, 50mV/step, 19steps
PMU.setCPUSLDOVoltage(1000)

# DLDO1 IMAX = 300mA
# 500~3400mV, 100mV/step, 29steps
PMU.setDLDO1Voltage(3300)

# DLDO2 IMAX = 300mA
# 500~1400mV, 50mV/step, 2steps
PMU.setDLDO2Voltage(3300)

#  PMU.enableDC1()
PMU.enableDC2()
PMU.enableDC3()
PMU.enableDC4()
PMU.enableDC5()
PMU.enableALDO1()
PMU.enableALDO2()
PMU.enableALDO3()
PMU.enableALDO4()
PMU.enableBLDO1()
PMU.enableBLDO2()
PMU.enableCPUSLDO()
PMU.enableDLDO1()
PMU.enableDLDO2()

print('===================================')
print('DC1    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC1()], PMU.getDC1Voltage()))
print('DC2    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC2()], PMU.getDC2Voltage()))
print('DC3    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC3()], PMU.getDC3Voltage()))
print('DC4    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC4()], PMU.getDC4Voltage()))
print('DC5    : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDC5()], PMU.getDC5Voltage()))
print('===================================')
print('ALDO1  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableALDO1()], PMU.getALDO1Voltage()))
print('ALDO2  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableALDO2()], PMU.getALDO2Voltage()))
print('ALDO3  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableALDO3()], PMU.getALDO3Voltage()))
print('ALDO4  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableALDO4()], PMU.getALDO4Voltage()))
print('===================================')
print('BLDO1  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableBLDO1()], PMU.getBLDO1Voltage()))
print('BLDO2  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableBLDO2()], PMU.getBLDO2Voltage()))
print('===================================')
print('CPUSLDO: {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableCPUSLDO()], PMU.getCPUSLDOVoltage()))
print('===================================')
print('DLDO1  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDLDO1()], PMU.getDLDO1Voltage()))
print('DLDO2  : {0}   Voltage:{1} mV '.format(
    ('-', '+')[PMU.isEnableDLDO2()], PMU.getDLDO2Voltage()))
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

#  DCDC 120 % (130 %) high voltage turn off PMIC function
en = PMU.getDCHighVoltagePowerDowmEn()
print('getDCHighVoltagePowerDowmEn:%s' % ('DISABLE', 'ENABLE')[en])

#  DCDC1 85 % low voltage turn off PMIC function
en = PMU.getDC1LowVoltagePowerDowmEn()
print('getDC1LowVoltagePowerDowmEn:%s' % ('DISABLE', 'ENABLE')[en])

#  DCDC2 85 % low voltage turn off PMIC function
en = PMU.getDC2LowVoltagePowerDowmEn()
print('getDC2LowVoltagePowerDowmEn:%s' % ('DISABLE', 'ENABLE')[en])

#  DCDC3 85 % low voltage turn off PMIC function
en = PMU.getDC3LowVoltagePowerDowmEn()
print('getDC3LowVoltagePowerDowmEn:%s' % ('DISABLE', 'ENABLE')[en])

#  DCDC4 85 % low voltage turn off PMIC function
en = PMU.getDC4LowVoltagePowerDowmEn()
print('getDC4LowVoltagePowerDowmEn:%s' % ('DISABLE', 'ENABLE')[en])

#  DCDC5 85 % low voltage turn off PMIC function
en = PMU.getDC5LowVoltagePowerDowmEn()
print('getDC5LowVoltagePowerDowmEn:%s' % ('DISABLE', 'ENABLE')[en])


#  PMU.setDCHighVoltagePowerDowm(true)
#  PMU.setDC1LowVoltagePowerDowm(true)
#  PMU.setDC2LowVoltagePowerDowm(true)
#  PMU.setDC3LowVoltagePowerDowm(true)
#  PMU.setDC4LowVoltagePowerDowm(true)
#  PMU.setDC5LowVoltagePowerDowm(true)

#  It is necessary to disable the detection function of the TS pin on the board
#  without the battery temperature detection function, otherwise it will cause abnormal charging
PMU.disableTSPinMeasure()

#  PMU.enableTemperatureMeasure()

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
PMU.disableIRQ(PMU.XPOWERS_AXP2101_ALL_IRQ)
#  Clear all interrupt flags
PMU.clearIrqStatus()
#  Enable the required interrupt function
PMU.enableIRQ(
    PMU.XPOWERS_AXP2101_BAT_INSERT_IRQ | PMU.XPOWERS_AXP2101_BAT_REMOVE_IRQ |  # BATTERY
    PMU.XPOWERS_AXP2101_VBUS_INSERT_IRQ | PMU.XPOWERS_AXP2101_VBUS_REMOVE_IRQ |  # VBUS
    PMU.XPOWERS_AXP2101_PKEY_SHORT_IRQ | PMU.XPOWERS_AXP2101_PKEY_LONG_IRQ |  # POWER KEY
    PMU.XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | PMU.XPOWERS_AXP2101_BAT_CHG_START_IRQ  # CHARGE
    #  PMU.XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | PMU.XPOWERS_AXP2101_PKEY_POSITIVE_IRQ | # POWER KEY
)

#  Set the precharge charging current
PMU.setPrechargeCurr(PMU.XPOWERS_AXP2101_PRECHARGE_50MA)
#  Set constant current charge current limit
PMU.setChargerConstantCurr(PMU.XPOWERS_AXP2101_CHG_CUR_200MA)
#  Set stop charging termination current
PMU.setChargerTerminationCurr(PMU.XPOWERS_AXP2101_CHG_ITERM_25MA)

#  Set charge cut-off voltage
PMU.setChargeTargetVoltage(PMU.XPOWERS_AXP2101_CHG_VOL_4V1)

#  Set the watchdog trigger event type
PMU.setWatchdogConfig(PMU.XPOWERS_AXP2101_WDT_IRQ_TO_PIN)
#  Set watchdog timeout
PMU.setWatchdogTimeout(PMU.XPOWERS_AXP2101_WDT_TIMEOUT_4S)
#  Enable watchdog to trigger interrupt event
#  PMU.enableWatchdog()

PMU.disableWatchdog()

PMU.clearIrqStatus()


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

        if PMU.isPekeyShortPressIrq():
            print("IRQ ---> isPekeyShortPress")
        if PMU.isPekeyLongPressIrq():
            print("IRQ ---> isPekeyLongPress")
        if PMU.isPekeyNegativeIrq():
            print("IRQ ---> isPekeyNegative")
        if PMU.isPekeyPositiveIrq():
            print("IRQ ---> isPekeyPositive")
        if PMU.isWdtExpireIrq():
            print("IRQ ---> isWdtExpire")

        PMU.clearIrqStatus()

    PMU.setChargingLedMode((PMU.XPOWERS_CHG_LED_OFF, PMU.XPOWERS_CHG_LED_ON)[
                           PMU.getChargingLedMode() == PMU.XPOWERS_CHG_LED_OFF])
    print("getBattVoltage:{0}mV".format(PMU.getBattVoltage()))
    print("getSystemVoltage:{0}mV".format(PMU.getSystemVoltage()))
    print("getBatteryPercent:{0}%".format(PMU.getBatteryPercent()))

    print("isCharging:{0}".format(PMU.isCharging()))
    print("isDischarge:{0}".format(PMU.isDischarge()))
    print("isStandby:{0}".format(PMU.isStandby()))

    time.sleep(0.8)
