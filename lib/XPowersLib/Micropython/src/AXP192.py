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

@file      AXP192.py
@author    Lewis He (lewishe@outlook.com)
@date      2022-10-20

'''

from I2CInterface import *
import math

_AXP192_CHIP_ID = const(0x03)
_AXP192_STATUS = const(0x00)
_AXP192_MODE_CHGSTATUS = const(0x01)
_AXP192_OTG_STATUS = const(0x02)
_AXP192_IC_TYPE = const(0x03)
_AXP192_DATA_BUFFER1 = const(0x06)
_AXP192_DATA_BUFFER2 = const(0x07)
_AXP192_DATA_BUFFER3 = const(0x08)
_AXP192_DATA_BUFFER4 = const(0x09)
_AXP192_DATA_BUFFER5 = const(0x0A)
_AXP192_DATA_BUFFER6 = const(0x0B)
_AXP192_DATA_BUFFER_SIZE = const(6)
_AXP192_LDO23_DC123_EXT_CTL = const(0x12)
_AXP192_DC2OUT_VOL = const(0x23)
_AXP192_DC2_DVM = const(0x25)
_AXP192_DC3OUT_VOL = const(0x27)
_AXP192_LDO24OUT_VOL = const(0x28)
_AXP192_LDO3OUT_VOL = const(0x29)
_AXP192_IPS_SET = const(0x30)
_AXP192_VOFF_SET = const(0x31)
_AXP192_OFF_CTL = const(0x32)
_AXP192_CHARGE1 = const(0x33)
_AXP192_CHARGE2 = const(0x34)
_AXP192_BACKUP_CHG = const(0x35)
_AXP192_POK_SET = const(0x36)
_AXP192_DCDC_FREQSET = const(0x37)
_AXP192_VLTF_CHGSET = const(0x38)
_AXP192_VHTF_CHGSET = const(0x39)
_AXP192_APS_WARNING1 = const(0x3A)
_AXP192_APS_WARNING2 = const(0x3B)
_AXP192_TLTF_DISCHGSET = const(0x3C)
_AXP192_THTF_DISCHGSET = const(0x3D)
_AXP192_DCDC_MODESET = const(0x80)
_AXP192_ADC_EN1 = const(0x82)
_AXP192_ADC_EN2 = const(0x83)
_AXP192_ADC_SPEED = const(0x84)
_AXP192_ADC_INPUTRANGE = const(0x85)
_AXP192_ADC_IRQ_RETFSET = const(0x86)
_AXP192_ADC_IRQ_FETFSET = const(0x87)
_AXP192_TIMER_CTL = const(0x8A)
_AXP192_VBUS_DET_SRP = const(0x8B)
_AXP192_HOTOVER_CTL = const(0x8F)
_AXP192_PWM1_FREQ_SET = const(0x98)
_AXP192_PWM1_DUTY_SET1 = const(0x99)
_AXP192_PWM1_DUTY_SET2 = const(0x9A)
_AXP192_PWM2_FREQ_SET = const(0x9B)
_AXP192_PWM2_DUTY_SET1 = const(0x9C)
_AXP192_PWM2_DUTY_SET2 = const(0x9D)
# INTERRUPT REGISTER
_AXP192_INTEN1 = const(0x40)
_AXP192_INTEN2 = const(0x41)
_AXP192_INTEN3 = const(0x42)
_AXP192_INTEN4 = const(0x43)
_AXP192_INTEN5 = const(0x4A)
# INTERRUPT STATUS REGISTER
_AXP192_INTSTS1 = const(0x44)
_AXP192_INTSTS2 = const(0x45)
_AXP192_INTSTS3 = const(0x46)
_AXP192_INTSTS4 = const(0x47)
_AXP192_INTSTS5 = const(0x4D)
_AXP192_INTSTS_CNT = const(5)
_AXP192_DC1_VLOTAGE = const(0x26)
_AXP192_LDO23OUT_VOL = const(0x28)
_AXP192_GPIO0_CTL = const(0x90)
_AXP192_GPIO0_VOL = const(0x91)
_AXP192_GPIO1_CTL = const(0X92)
_AXP192_GPIO2_CTL = const(0x93)
_AXP192_GPIO012_SIGNAL = const(0x94)
_AXP192_GPIO34_CTL = const(0x95)
_AXP192_GPIO34_SIGNAL = const(0x96)
_AXP192_GPIO012_PULLDOWN = const(0x97)
_AXP192_GPIO5_CTL = const(0x9E)
_AXP192_GPIO0_VOL_ADC_H8 = const(0x64)
_AXP192_GPIO0_VOL_ADC_L4 = const(0x65)
_AXP192_GPIO1_VOL_ADC_H8 = const(0x66)
_AXP192_GPIO1_VOL_ADC_L4 = const(0x67)
_AXP192_GPIO2_VOL_ADC_H8 = const(0x68)
_AXP192_GPIO2_VOL_ADC_L4 = const(0x69)
_AXP192_GPIO3_VOL_ADC_H8 = const(0x6A)
_AXP192_GPIO3_VOL_ADC_L4 = const(0x6B)
_AXP192_GPIO_COUNT = const(5)
_AXP192_GPIO0_STEP = const(0.5)
_AXP192_GPIO1_STEP = const(0.5)
_AXP192_TS_IN_H8 = const(0x62)
_AXP192_TS_IN_L4 = const(0x63)
_AXP192_BAT_AVERCHGCUR_H8 = const(0x7A)
_AXP192_BAT_AVERCHGCUR_L5 = const(0x7B)
_AXP192_ACIN_VOL_H8 = const(0x56)
_AXP192_ACIN_VOL_L4 = const(0x57)
_AXP192_ACIN_CUR_H8 = const(0x58)
_AXP192_ACIN_CUR_L4 = const(0x59)
_AXP192_VBUS_VOL_H8 = const(0x5A)
_AXP192_VBUS_VOL_L4 = const(0x5B)
_AXP192_VBUS_CUR_H8 = const(0x5C)
_AXP192_VBUS_CUR_L4 = const(0x5D)
_AXP192_BAT_AVERDISCHGCUR_H8 = const(0x7C)
_AXP192_BAT_AVERDISCHGCUR_L5 = const(0x7D)
_AXP192_APS_AVERVOL_H8 = const(0x7E)
_AXP192_APS_AVERVOL_L4 = const(0x7F)
_AXP192_BAT_AVERVOL_H8 = const(0x78)
_AXP192_BAT_AVERVOL_L4 = const(0x79)
_AXP192_BAT_CHGCOULOMB3 = const(0xB0)
_AXP192_BAT_CHGCOULOMB2 = const(0xB1)
_AXP192_BAT_CHGCOULOMB1 = const(0xB2)
_AXP192_BAT_CHGCOULOMB0 = const(0xB3)
_AXP192_BAT_DISCHGCOULOMB3 = const(0xB4)
_AXP192_BAT_DISCHGCOULOMB2 = const(0xB5)
_AXP192_BAT_DISCHGCOULOMB1 = const(0xB6)
_AXP192_BAT_DISCHGCOULOMB0 = const(0xB7)
_AXP192_COULOMB_CTL = const(0xB8)
_AXP192_BATT_VOLTAGE_STEP = const(1.1)
_AXP192_BATT_DISCHARGE_CUR_STEP = const(0.5)
_AXP192_BATT_CHARGE_CUR_STEP = const(0.5)
_AXP192_ACIN_VOLTAGE_STEP = const(1.7)
_AXP192_ACIN_CUR_STEP = const(0.625)
_AXP192_VBUS_VOLTAGE_STEP = const(1.7)
_AXP192_VBUS_CUR_STEP = const(0.375)
_AXP192_APS_VOLTAGE_STEP = const(1.4)
_AXP192_TS_PIN_OUT_STEP = const(0.8)
_AXP192_LDO2_VOL_MIN = const(1800)
_AXP192_LDO2_VOL_MAX = const(3300)
_AXP192_LDO2_VOL_STEPS = const(100)
_AXP192_LDO2_VOL_BIT_MASK = const(4)
_AXP192_LDO3_VOL_MIN = const(1800)
_AXP192_LDO3_VOL_MAX = const(3300)
_AXP192_LDO3_VOL_STEPS = const(100)
_AXP192_DC1_VOL_STEPS = const(25)
_AXP192_DC1_VOL_MIN = const(700)
_AXP192_DC1_VOL_MAX = const(3500)
_AXP192_DC2_VOL_STEPS = const(25)
_AXP192_DC2_VOL_MIN = const(700)
_AXP192_DC2_VOL_MAX = const(3500)
_AXP192_DC3_VOL_STEPS = const(25)
_AXP192_DC3_VOL_MIN = const(700)
_AXP192_DC3_VOL_MAX = const(3500)
_AXP192_LDOIO_VOL_STEPS = const(100)
_AXP192_LDOIO_VOL_MIN = const(1800)
_AXP192_LDOIO_VOL_MAX = const(3300)
_AXP192_SYS_VOL_STEPS = const(100)
_AXP192_VOFF_VOL_MIN = const(2600)
_AXP192_VOFF_VOL_MAX = const(3300)
_AXP192_CHG_EXT_CURR_MIN = const(300)
_AXP192_CHG_EXT_CURR_MAX = const(1000)
_AXP192_CHG_EXT_CURR_STEP = const(100)
_AXP192_INTERNAL_TEMP_H8 = const(0x5E)
_AXP192_INTERNAL_TEMP_L4 = const(0x5)
_AXP192_INTERNAL_TEMP_STEP = const(0.1)
_AXP192_INERNAL_TEMP_OFFSET = const(144.7)

_MONITOR_TS_PIN = const(1 << 0)
_MONITOR_APS_VOLTAGE = const(1 << 1)
_MONITOR_USB_CURRENT = const(1 << 2)
_MONITOR_USB_VOLTAGE = const(1 << 3)
_MONITOR_AC_CURRENT = const(1 << 4)
_MONITOR_AC_VOLTAGE = const(1 << 5)
_MONITOR_BAT_CURRENT = const(1 << 6)
_MONITOR_BAT_VOLTAGE = const(1 << 7)
_MONITOR_ADC_IO3 = const(1 << 8)
_MONITOR_ADC_IO2 = const(1 << 9)
_MONITOR_ADC_IO1 = const(1 << 10)
_MONITOR_ADC_IO0 = const(1 << 11)
_MONITOR_TEMPERATURE = const(1 << 16)

AXP192_SLAVE_ADDRESS = const(0x34)


class PMU_Gpio:
    def __init__(self) -> None:
        self.mode = 0
        self.val = 0


class AXP192(I2CInterface):

    # ARGS ARGS ARGS ARGS ARGS ARGS ARGS ARGS ARGS
    """VBUS voltage limit values"""
    XPOWERS_AXP192_VBUS_VOL_LIM_4V = const(0)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V1 = const(1)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V2 = const(2)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V3 = const(3)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V4 = const(4)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V5 = const(5)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V6 = const(6)
    XPOWERS_AXP192_VBUS_VOL_LIM_4V7 = const(7)

    """Power button press shutdown time values"""
    XPOWERS_POWEROFF_4S = const(0)
    XPOWERS_POWEROFF_6S = const(1)
    XPOWERS_POWEROFF_8S = const(2)
    XPOWERS_POWEROFF_10S = const(3)

    """Power on time value when the power button is pressed"""
    XPOWERS_POWERON_128MS = const(0)
    XPOWERS_POWERON_512MS = const(1)
    XPOWERS_POWERON_1S = const(2)
    XPOWERS_POWERON_2S = const(3)

    XPOWERS_AXP192_CHG_ITERM_LESS_10_PERCENT = const(0)
    XPOWERS_AXP192_CHG_ITERM_LESS_15_PERCENT = const(1)

    """Precharge timeout limit values"""
    XPOWERS_AXP192_PRECHG_TIMEOUT_30MIN = const(0)
    XPOWERS_AXP192_PRECHG_TIMEOUT_40MIN = const(1)
    XPOWERS_AXP192_PRECHG_TIMEOUT_50MIN = const(2)
    XPOWERS_AXP192_PRECHG_TIMEOUT_60MIN = const(3)

    """PowerKey longpress timeout values"""
    XPOWERS_AXP192_LONGPRESS_1000MS = const(0)
    XPOWERS_AXP192_LONGPRESS_1500MS = const(1)
    XPOWERS_AXP192_LONGPRESS_2000MS = const(2)
    XPOWERS_AXP192_LONGPRESS_2500MS = const(3)

    """PMU LED indicator function values"""
    XPOWERS_CHG_LED_OFF = const(0)
    XPOWERS_CHG_LED_BLINK_1HZ = const(1)
    XPOWERS_CHG_LED_BLINK_4HZ = const(2)
    XPOWERS_CHG_LED_ON = const(3)
    XPOWERS_CHG_LED_CTRL_CHG = const(4)

    XPOWERS_AXP192_CHG_CONS_TIMEOUT_7H = const(0)
    XPOWERS_AXP192_CHG_CONS_TIMEOUT_8H = const(1)
    XPOWERS_AXP192_CHG_CONS_TIMEOUT_9H = const(2)
    XPOWERS_AXP192_CHG_CONS_TIMEOUT_10H = const(3)

    """PMU backup battery voltage values"""
    XPOWERS_AXP192_BACKUP_BAT_VOL_3V1 = const(0)
    XPOWERS_AXP192_BACKUP_BAT_VOL_3V = const(1)
    # !NEED FIXDATASHEET ERROR!
    XPOWERS_AXP192_BACKUP_BAT_VOL_3V0 = const(2)
    XPOWERS_AXP192_BACKUP_BAT_VOL_2V5 = const(3)

    """PMU backup battery current values"""
    XPOWERS_AXP192_BACKUP_BAT_CUR_50UA = const(0)
    XPOWERS_AXP192_BACKUP_BAT_CUR_100UA = const(1)
    XPOWERS_AXP192_BACKUP_BAT_CUR_200UA = const(2)
    XPOWERS_AXP192_BACKUP_BAT_CUR_400UA = const(3)

    """PMU gpio values"""
    PMU_GPIO0 = const(0)
    PMU_GPIO1 = const(1)
    PMU_GPIO2 = const(2)
    PMU_GPIO3 = const(3)
    PMU_GPIO4 = const(4)
    PMU_GPIO5 = const(5)
    PMU_TS_PIN = const(6)

    """Charging voltage limit values"""
    XPOWERS_AXP192_CHG_VOL_4V1 = const(0)
    XPOWERS_AXP192_CHG_VOL_4V15 = const(1)
    XPOWERS_AXP192_CHG_VOL_4V2 = const(2)
    XPOWERS_AXP192_CHG_VOL_4V36 = const(3)

    """Charging current limit values"""
    XPOWERS_AXP192_CHG_CUR_100MA = const(0)
    XPOWERS_AXP192_CHG_CUR_190MA = const(1)
    XPOWERS_AXP192_CHG_CUR_280MA = const(2)
    XPOWERS_AXP192_CHG_CUR_360MA = const(3)
    XPOWERS_AXP192_CHG_CUR_450MA = const(4)
    XPOWERS_AXP192_CHG_CUR_550MA = const(5)
    XPOWERS_AXP192_CHG_CUR_630MA = const(6)
    XPOWERS_AXP192_CHG_CUR_700MA = const(7)
    XPOWERS_AXP192_CHG_CUR_780MA = const(8)
    XPOWERS_AXP192_CHG_CUR_880MA = const(9)
    XPOWERS_AXP192_CHG_CUR_960MA = const(10)
    XPOWERS_AXP192_CHG_CUR_1000MA = const(11)
    XPOWERS_AXP192_CHG_CUR_1080MA = const(12)
    XPOWERS_AXP192_CHG_CUR_1160MA = const(13)
    XPOWERS_AXP192_CHG_CUR_1240MA = const(14)
    XPOWERS_AXP192_CHG_CUR_1320MA = const(15)

    """VBUS current limit values"""
    XPOWERS_AXP192_VBUS_CUR_LIM_500MA = const(0)
    XPOWERS_AXP192_VBUS_CUR_LIM_100MA = const(1)
    XPOWERS_AXP192_VBUS_CUR_LIM_OFF = const(2)

    """PMU interrupt control mask values"""
    #! IRQ1 REG 40H
    # VBUS is available, but lower than V HOLD, IRQ enable
    XPOWERS_AXP192_VBUS_VHOLD_LOW_IRQ = const(1 << 1)
    # VBUS removed, IRQ enable
    XPOWERS_AXP192_VBUS_REMOVE_IRQ = const(1 << 2)
    # VBUS connected, IRQ enable
    XPOWERS_AXP192_VBUS_INSERT_IRQ = const(1 << 3)
    # VBUS over-voltage, IRQ enable
    XPOWERS_AXP192_VBUS_OVER_VOL_IRQ = const(1 << 4)
    # ACIN removed, IRQ enable
    XPOWERS_AXP192_ACIN_REMOVED_IRQ = const(1 << 5)
    # ACIN connected, IRQ enable
    XPOWERS_AXP192_ACIN_CONNECT_IRQ = const(1 << 6)
    # ACIN over-voltage, IRQ enable
    XPOWERS_AXP192_ACIN_OVER_VOL_IRQ = const(1 << 7)
    #! IRQ2 REG 41H
    # Battery low-temperature, IRQ enable
    XPOWERS_AXP192_BATT_LOW_TEMP_IRQ = const(1 << 8)
    # Battery over-temperature, IRQ enable
    XPOWERS_AXP192_BATT_OVER_TEMP_IRQ = const(1 << 9)
    # Charge finished, IRQ enable
    XPOWERS_AXP192_BAT_CHG_DONE_IRQ = const(1 << 10)
    # Be charging, IRQ enable
    XPOWERS_AXP192_BAT_CHG_START_IRQ = const(1 << 11)
    # Exit battery activate mode, IRQ enable
    XPOWERS_AXP192_BATT_EXIT_ACTIVATE_IRQ = const(1 << 12)
    # Battery activate mode, IRQ enable
    XPOWERS_AXP192_BATT_ACTIVATE_IRQ = const(1 << 13)
    # Battery removed, IRQ enable
    XPOWERS_AXP192_BAT_REMOVE_IRQ = const(1 << 14)
    # Battery connected, IRQ enable
    XPOWERS_AXP192_BAT_INSERT_IRQ = const(1 << 15)
    #! IRQ3 REG 42H
    # PEK long press, IRQ enable
    XPOWERS_AXP192_PKEY_LONG_IRQ = const(1 << 16)
    # PEK short press, IRQ enable
    XPOWERS_AXP192_PKEY_SHORT_IRQ = const(1 << 17)
    # **Reserved and unchangeable BIT 2
    # DC-DC3output voltage is lower than the set value, IRQ enable
    XPOWERS_AXP192_DC3_LOW_VOL_IRQ = const(1 << 19)
    # DC-DC2 output voltage is lower than the set value, IRQ enable
    XPOWERS_AXP192_DC2_LOW_VOL_IRQ = const(1 << 20)
    # DC-DC1 output voltage is lower than the set value, IRQ enable
    XPOWERS_AXP192_DC1_LOW_VOL_IRQ = const(1 << 21)
    # Charge current is lower than the set current, IRQ enable
    XPOWERS_AXP192_CHARGE_LOW_CUR_IRQ = const(1 << 22)
    # XPOWERS internal over-temperature, IRQ enable
    XPOWERS_AXP192_CHIP_TEMP_HIGH_IRQ = const(1 << 23)
    #! IRQ4 REG 43H
    # APS low-voltage, IRQ enable
    XPOWERS_AXP192_APS_LOW_VOL_LEVEL_IRQ = const(1 << 24)
    # **Reserved and unchangeable BIT 1
    # VBUS Session End IRQ enable
    XPOWERS_AXP192_VBUS_SESSION_END_IRQ = const(1 << 26)
    # VBUS Session A/B IRQ enable
    XPOWERS_AXP192_VBUS_SESSION_AB_IRQ = const(1 << 27)
    # VBUS invalid, IRQ enable
    XPOWERS_AXP192_VBUS_INVALID_IRQ = const(1 << 28)
    # VBUS valid, IRQ enable
    XPOWERS_AXP192_VBUS_VAILD_IRQ = const(1 << 29)
    # N_OE shutdown, IRQ enable
    XPOWERS_AXP192_NOE_OFF_IRQ = const(1 << 30)
    # N_OE startup, IRQ enable
    XPOWERS_AXP192_NOE_ON_IRQ = const(1 << 31)
    #! IRQ5 REG 4AH
    # GPIO0 input edge trigger, IRQ enable
    XPOWERS_AXP192_GPIO0_EDGE_TRIGGER_IRQ = const(1 << 32)
    # GPIO1input edge trigger or ADC input, IRQ enable
    XPOWERS_AXP192_GPIO1_EDGE_TRIGGER_IRQ = const(1 << 33)
    # GPIO2input edge trigger, IRQ enable
    XPOWERS_AXP192_GPIO2_EDGE_TRIGGER_IRQ = const(1 << 34)
    # **Reserved and unchangeable BIT 3
    # **Reserved and unchangeable BIT 4
    # **Reserved and unchangeable BIT 5
    # **Reserved and unchangeable BIT 6
    # Timer timeout, IRQ enable
    XPOWERS_AXP192_TIMER_TIMEOUT_IRQ = const(1 << 39)
    XPOWERS_AXP192_ALL_IRQ = const(0xFFFFFFFFFF)

    LOW = const(0x0)
    HIGH = const(0x1)

    # GPIO FUNCTIONS
    INPUT = const(0x01)
    OUTPUT = const(0x03)
    PULLUP = const(0x04)
    INPUT_PULLUP = const(0x05)
    PULLDOWN = const(0x08)
    INPUT_PULLDOWN = const(0x09)
    ANALOG = const(0x10)

    # INT
    RISING = const(0x01)
    FALLING = const(0x02)

    def __init__(self, i2c_bus: I2C, addr: int = AXP192_SLAVE_ADDRESS) -> None:
        super().__init__(i2c_bus, addr)
        print('AXP192 __init__')

        if self.getChipID() != _AXP192_CHIP_ID:
            raise RuntimeError(
                "Failed to find %s - check your wiring!" % self.__class__.__name__
            )

        self.statusRegister = [0] * _AXP192_INTSTS_CNT

        self.intRegister = [0] * _AXP192_INTSTS_CNT
        self.gpio = [PMU_Gpio(), PMU_Gpio(), PMU_Gpio(),
                     PMU_Gpio(), PMU_Gpio(), PMU_Gpio()]

    def isAcinVbusStart(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_STATUS, 0))

    def isDischarge(self) -> bool:
        return not bool(super().getRegisterBit(_AXP192_STATUS, 2))

    def isVbusIn(void) -> bool:
        return bool(super().getRegisterBit(_AXP192_STATUS, 5))

    def isAcinEfficient(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_STATUS, 6))

    def isAcinIn(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_STATUS, 7))

    def isOverTemperature(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_MODE_CHGSTATUS, 7))

    def isCharging(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_MODE_CHGSTATUS, 6))

    def isBatteryConnect(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_MODE_CHGSTATUS, 5))

    def isBattInActiveMode(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_MODE_CHGSTATUS, 3))

    def isChargeCurrLessPreset(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_MODE_CHGSTATUS, 2))

    def enableVbusVoltageLimit(self) -> None:
        super().setRegisterBit(_AXP192_IPS_SET, 6)

    def disableVbusVoltageLimit(self) -> None:
        super().clrRegisterBit(_AXP192_IPS_SET, 6)

    def setVbusVoltageLimit(self, opt: int) -> None:
        val = super().readRegister(_AXP192_IPS_SET)[0]
        val &= 0xC7
        super().writeRegister(_AXP192_IPS_SET, val | (opt << 3))

    # @brief  Set VBUS Current Input Limit.
    # @param  opt: View the related chip type _AXP192_vbus_cur_limit_t enumeration
    #              parameters in "XPowersParams.hpp"

    def setVbusCurrentLimit(self, opt: int) -> None:
        if opt == self.XPOWERS_AXP192_VBUS_CUR_LIM_500MA:
            super().setRegisterBit(_AXP192_IPS_SET, 1)
            super().clrRegisterBit(_AXP192_IPS_SET, 0)
        elif opt == self.XPOWERS_AXP192_VBUS_CUR_LIM_100MA:
            super().setRegisterBit(_AXP192_IPS_SET, 1)
            super().setRegisterBit(_AXP192_IPS_SET, 0)
        elif opt == self.XPOWERS_AXP192_VBUS_CUR_LIM_OFF:
            super().clrRegisterBit(_AXP192_IPS_SET, 1)

    # @brief  Get VBUS Current Input Limit.
    # @retval View the related chip type _AXP192_vbus_cur_limit_t enumeration
    #              parameters in "XPowersParams.hpp"
    def getVbusCurrentLimit(self) -> int:
        if super().getRegisterBit(_AXP192_IPS_SET, 1) == 0:
            return self.XPOWERS_AXP192_VBUS_CUR_LIM_OFF
        if super().getRegisterBit(_AXP192_IPS_SET, 0):
            return self.XPOWERS_AXP192_VBUS_CUR_LIM_100MA
        return self.XPOWERS_AXP192_VBUS_CUR_LIM_500MA

    # Set the minimum system operating voltage inside the PMU,
    # below this value will shut down the PMU,Adjustment range 2600mV ~ 3300mV
    def setSysPowerDownVoltage(self, millivolt: int) -> bool:
        if millivolt % _AXP192_SYS_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_SYS_VOL_STEPS)
        if millivolt < _AXP192_VOFF_VOL_MIN:
            raise ValueError("Mistake ! SYS minimum output voltage is  %umV" %
                             _AXP192_VOFF_VOL_MIN)
        elif millivolt > _AXP192_VOFF_VOL_MAX:
            raise ValueError("Mistake ! SYS maximum output voltage is  %umV" %
                             _AXP192_VOFF_VOL_MAX)
        val = super().readRegister(_AXP192_VOFF_SET)[0]
        val &= 0xF8
        val |= (int)((millivolt - _AXP192_VOFF_VOL_MIN) /
                     _AXP192_SYS_VOL_STEPS)
        super().writeRegister(_AXP192_VOFF_SET, val)

    def getSysPowerDownVoltage(self) -> int:
        val = super().readRegister(_AXP192_VOFF_SET)[0]
        val &= 0x07
        return (val * _AXP192_SYS_VOL_STEPS) + _AXP192_VOFF_VOL_MIN

    # @brief  Set shutdown, calling shutdown will turn off all power channels,
    #         only VRTC belongs to normal power supply
    def shutdown(self) -> None:
        super().setRegisterBit(_AXP192_OFF_CTL, 7)

    # Charge setting
    def enableCharge(self) -> None:
        super().setRegisterBit(_AXP192_CHARGE1, 7)

    def disableCharge(self) -> None:
        super().clrRegisterBit(_AXP192_CHARGE1, 7)

    # @brief Set charge target voltage.
    # @param  opt: See _AXP192_chg_vol_t enum for details.
    def setChargeTargetVoltage(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError(
                "Charger target voltage must be a value within 0-3!")
        val = super().readRegister(_AXP192_CHARGE1)[0]
        val &= 0x9F
        super().writeRegister(_AXP192_CHARGE1, val | (opt << 5))

     # @brief Get charge target voltage settings.
     # @retval See _AXP192_chg_vol_t enum for details.
    def getChargeTargetVoltage(self) -> int:
        val = super().readRegister(_AXP192_CHARGE1)[0]
        return (val & 0x60) >> 5

    # @brief Set charge current settings.
    # @retval See _AXP192_chg_curr_t enum for details.

    def setChargerConstantCurr(self, opt: int) -> bool:
        if not 0 <= opt <= 15:
            raise ValueError(
                "Charger current must be a value within 0-15!")
        val = super().readRegister(_AXP192_CHARGE1)[0]
        val &= 0xF0
        super().writeRegister(_AXP192_CHARGE1, val | opt)

    # @brief Get charge current settings.
    # @retval See _AXP192_chg_curr_t enum for details.
    def getChargerConstantCurr(self) -> int:
        val = super().readRegister(_AXP192_CHARGE1)[0] & 0x0F
        return val

    def setChargerTerminationCurr(self, opt: int) -> None:
        if opt == self.XPOWERS_AXP192_CHG_ITERM_LESS_10_PERCENT:
            super().clrRegisterBit(_AXP192_CHARGE1, 0)
        elif opt == self.XPOWERS_AXP192_CHG_ITERM_LESS_15_PERCENT:
            super().setRegisterBit(_AXP192_CHARGE1, 0)

    def getChargerTerminationCurr(self) -> int:
        return super().getRegisterBit(_AXP192_CHARGE1, 4)

    def setPrechargeTimeout(self, opt: int):
        val = super().readRegister(_AXP192_CHARGE2)[0]
        val &= 0x3F
        super().writeRegister(_AXP192_CHARGE2, val | (opt << 6))

    #  External channel charge current setting,Range:300~1000mA
    def setChargerExternChannelCurr(self, milliampere: int) -> None:
        if milliampere % _AXP192_CHG_EXT_CURR_STEP:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_CHG_EXT_CURR_STEP)
        if milliampere < _AXP192_CHG_EXT_CURR_MIN:
            raise ValueError("Mistake ! The minimum external path charge current setting is:  %umA" %
                             _AXP192_CHG_EXT_CURR_MIN)
        elif milliampere > _AXP192_CHG_EXT_CURR_MAX:
            raise ValueError("Mistake ! The maximum external channel charge current setting is:  %umA" %
                             _AXP192_CHG_EXT_CURR_MAX)
        val = super().readRegister(_AXP192_CHARGE2)[0]
        val &= 0xC7
        val |= (int)((milliampere - _AXP192_CHG_EXT_CURR_MIN) /
                     _AXP192_CHG_EXT_CURR_STEP)
        super().writeRegister(_AXP192_CHARGE2, val)

    def enableChargerExternChannel(self) -> None:
        super().setRegisterBit(_AXP192_CHARGE2, 2)

    def disableChargerExternChannel(self) -> None:
        super().clrRegisterBit(_AXP192_CHARGE2, 2)

    #  Timeout setting in constant current mode
    def setChargerConstantTimeout(self, opt: int) -> None:
        val = super().readRegister(_AXP192_CHARGE2)[0]
        val &= 0xFC
        super().writeRegister(_AXP192_CHARGE2, val | opt)

    def enableBackupBattCharger(self) -> None:
        super().setRegisterBit(_AXP192_BACKUP_CHG, 7)

    def disableBackupBattCharger(self) -> None:
        super().clrRegisterBit(_AXP192_BACKUP_CHG, 7)

    def isEanbleBackupCharger(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_BACKUP_CHG, 7))

    def setBackupBattChargerVoltage(self, opt: int) -> int:
        val = super().readRegister(_AXP192_BACKUP_CHG)[0]
        val &= 0x9F
        super().writeRegister(_AXP192_BACKUP_CHG, val | (opt << 5))

    def setBackupBattChargerCurr(self, opt: int):
        val = super().readRegister(_AXP192_BACKUP_CHG)[0]
        val &= 0xFC
        super().writeRegister(_AXP192_BACKUP_CHG, val | opt)

    # Temperature
    def getTemperature(self) -> float:
        return super().readRegisterH8L4(_AXP192_INTERNAL_TEMP_H8, _AXP192_INTERNAL_TEMP_L4) * _AXP192_INTERNAL_TEMP_STEP - _AXP192_INERNAL_TEMP_OFFSET

    def enableTemperatureMeasure(self) -> None:
        super().setRegisterBit(_AXP192_ADC_EN2, 7)

    def disableTemperatureMeasure(self) -> None:
        super().clrRegisterBit(_AXP192_ADC_EN2, 7)

    # Power control LDOio functions
    def isEnableLDOio(self) -> bool:
        return bool(super().readRegister(_AXP192_GPIO0_CTL)[0] & 0x02)

    def enableLDOio(self) -> None:
        val = super().readRegister(_AXP192_GPIO0_CTL)[0] & 0xF8
        super().writeRegister(_AXP192_GPIO0_CTL, val | 0x02)

    def disableLDOio(self) -> None:
        val = super().readRegister(_AXP192_GPIO0_CTL)[0] & 0xF8
        super().writeRegister(_AXP192_GPIO0_CTL, val)

    def setLDOioVoltage(self, millivolt: int) -> None:
        if millivolt % _AXP192_LDOIO_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_LDOIO_VOL_STEPS)
        if millivolt < _AXP192_LDOIO_VOL_MIN:
            raise ValueError("Mistake ! LDOIO minimum output voltage is  %umV" %
                             _AXP192_LDOIO_VOL_MIN)
        elif millivolt > _AXP192_LDOIO_VOL_MAX:
            raise ValueError("Mistake ! LDOIO maximum output voltage is  %umV" %
                             _AXP192_LDOIO_VOL_MAX)
        val = super().readRegister(_AXP192_GPIO0_VOL)[0]
        val |= ((int)((millivolt - _AXP192_LDOIO_VOL_MIN) /
                _AXP192_LDOIO_VOL_STEPS) << 4)
        super().writeRegister(_AXP192_GPIO0_VOL, val)

    def getLDOioVoltage(self) -> int:
        val = super().readRegister(_AXP192_GPIO0_VOL)[0]
        val >>= 4
        val *= _AXP192_LDOIO_VOL_STEPS
        val += _AXP192_LDOIO_VOL_MIN
        return val

    # Power control LDO2 functions
    def isEnableLDO2(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 2))

    def enableLDO2(self) -> None:
        super().setRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 2)

    def disableLDO2(self) -> None:
        super().clrRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 2)

    def setLDO2Voltage(self, millivolt: int) -> None:
        if millivolt % _AXP192_LDO2_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_LDO2_VOL_STEPS)
        if millivolt < _AXP192_LDO2_VOL_MIN:
            raise ValueError("Mistake ! LDO2 minimum output voltage is  %umV" %
                             _AXP192_LDO2_VOL_MIN)
        elif millivolt > _AXP192_LDO2_VOL_MAX:
            raise ValueError("Mistake ! LDO2 maximum output voltage is  %umV" %
                             _AXP192_LDO2_VOL_MAX)
        val = super().readRegister(_AXP192_LDO23OUT_VOL)[0]
        val &= 0x0F
        super().writeRegister(_AXP192_LDO23OUT_VOL, val | ((int)((millivolt -
                                                                  _AXP192_LDO2_VOL_MIN) / _AXP192_LDO2_VOL_STEPS) << _AXP192_LDO2_VOL_BIT_MASK))

    def getLDO2Voltage(self) -> int:
        val = super().readRegister(_AXP192_LDO23OUT_VOL)[0] & 0xF0
        return (val >> _AXP192_LDO2_VOL_BIT_MASK) * _AXP192_LDO2_VOL_STEPS + _AXP192_LDO2_VOL_MIN

    # Power control LDO3 functions

    def isEnableLDO3(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 3))

    def enableLDO3(self) -> None:
        super().setRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 3)

    def disableLDO3(self) -> None:
        super().clrRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 3)

    def setLDO3Voltage(self, millivolt: int) -> None:
        if millivolt % _AXP192_LDO3_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_LDO3_VOL_STEPS)
        if millivolt < _AXP192_LDO3_VOL_MIN:
            raise ValueError("Mistake ! LDO3 minimum output voltage is  %umV" %
                             _AXP192_LDO3_VOL_MIN)
        elif millivolt > _AXP192_LDO3_VOL_MAX:
            raise ValueError("Mistake ! LDO3 maximum output voltage is  %umV" %
                             _AXP192_LDO3_VOL_MAX)
        val = super().readRegister(_AXP192_LDO23OUT_VOL)[0] & 0xF0
        super().writeRegister(_AXP192_LDO23OUT_VOL, val | (int)(
            (millivolt - _AXP192_LDO3_VOL_MIN) / _AXP192_LDO3_VOL_STEPS))

    def getLDO3Voltage(self) -> int:
        val = super().readRegister(_AXP192_LDO23OUT_VOL)[0]
        val &= 0x0F
        return (val * _AXP192_LDO3_VOL_STEPS) + _AXP192_LDO3_VOL_MIN

    # Power control DCDC1 functions
    def setDC1PwmMode(self) -> None:
        val = super().readRegister(_AXP192_DCDC_MODESET)[0] & 0xF7
        super().writeRegister(_AXP192_DCDC_MODESET, val | 0x08)

    def setDC1AutoMode(self) -> None:
        val = super().readRegister(_AXP192_DCDC_MODESET)[0] & 0xF7
        super().writeRegister(_AXP192_DCDC_MODESET, val)

    def isEnableDC1(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 0))

    def enableDC1(self) -> None:
        super().setRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 0)

    def disableDC1(self) -> None:
        super().clrRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 0)

    def setDC1Voltage(self, millivolt) -> None:
        if millivolt % _AXP192_DC1_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_DC1_VOL_STEPS)
        if millivolt < _AXP192_DC1_VOL_STEPS:
            raise ValueError("Mistake ! DCDC1 minimum output voltage is  %umV" %
                             _AXP192_DC1_VOL_STEPS)
        elif millivolt > _AXP192_DC1_VOL_MAX:
            raise ValueError("Mistake ! DCDC1 maximum output voltage is  %umV" %
                             _AXP192_DC1_VOL_MAX)
        val = super().readRegister(_AXP192_DC1_VLOTAGE)[0]
        val &= 0x80
        val |= (int)((millivolt - _AXP192_DC1_VOL_MIN) /
                     _AXP192_DC1_VOL_STEPS)
        super().writeRegister(_AXP192_DC1_VLOTAGE, val)

    def getDC1Voltage(self) -> int:
        val = super().readRegister(_AXP192_DC1_VLOTAGE)[0] & 0x7F
        return val * _AXP192_DC1_VOL_STEPS + _AXP192_DC1_VOL_MIN

    # Power control DCDC2 functions

    def setDC2PwmMode(self) -> None:
        val = super().readRegister(_AXP192_DCDC_MODESET)[0] & 0xFB
        super().writeRegister(_AXP192_DCDC_MODESET, val | 0x04)

    def setDC2AutoMode(self) -> None:
        val = super().readRegister(_AXP192_DCDC_MODESET)[0] & 0xFB
        super().writeRegister(_AXP192_DCDC_MODESET, val)

    def enableDC2VRC(self) -> None:
        val = super().readRegister(_AXP192_DC2_DVM)[0]
        super().writeRegister(_AXP192_DC2_DVM, val | 0x04)

    def disableDC2VRC(self) -> None:
        val = super().readRegister(_AXP192_DC2_DVM)[0]
        super().writeRegister(_AXP192_DC2_DVM, val & 0xFB)

    def setDC2VRC(self, opts: int) -> None:
        if opts > 1:
            raise ValueError(
                "DCDC VRC a value within 0-1!")
        val = super().readRegister(_AXP192_DC2_DVM)[0] & 0xFE
        super().writeRegister(_AXP192_DC2_DVM, val | opts)

    def isEanbleDC2VRC(self) -> bool:
        return (super().readRegister(_AXP192_DC2_DVM)[0] & 0x04) == 0x04

    def isEnableDC2(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 4))

    def enableDC2(self) -> None:
        super().setRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 4)

    def disableDC2(self) -> None:
        super().clrRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 4)

    def setDC2Voltage(self, millivolt: int) -> None:
        if millivolt % _AXP192_DC2_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_DC2_VOL_STEPS)
        if millivolt < _AXP192_DC2_VOL_MIN:
            raise ValueError("Mistake ! DCDC2 minimum output voltage is  %umV" %
                             _AXP192_DC2_VOL_MIN)
        elif millivolt > _AXP192_DC2_VOL_MAX:
            raise ValueError("Mistake ! DCDC2 maximum output voltage is  %umV" %
                             _AXP192_DC2_VOL_MAX)
        val = super().readRegister(_AXP192_DC2OUT_VOL)[0]
        val &= 0x80
        val |= (int)((millivolt - _AXP192_DC2_VOL_MIN) /
                     _AXP192_DC2_VOL_STEPS)
        super().writeRegister(_AXP192_DC2OUT_VOL, val)

    def getDC2Voltage(self) -> int:
        val = super().readRegister(_AXP192_DC2OUT_VOL)[0]
        return (val * _AXP192_DC2_VOL_STEPS) + _AXP192_DC2_VOL_MIN

    # Power control DCDC3 functions
    def setDC3PwmMode(self) -> None:
        val = super().readRegister(_AXP192_DCDC_MODESET)[0] & 0xFD
        super().writeRegister(_AXP192_DCDC_MODESET, val | 0x02)

    def setDC3AutoMode(self) -> None:
        val = super().readRegister(_AXP192_DCDC_MODESET)[0] & 0xFD
        super().writeRegister(_AXP192_DCDC_MODESET, val)

    def isEnableDC3(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 1))

    def enableDC3(self) -> None:
        super().setRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 1)

    def disableDC3(self) -> None:
        super().clrRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 1)

    def setDC3Voltage(self, millivolt: int) -> None:
        if millivolt % _AXP192_DC3_VOL_STEPS:
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP192_DC3_VOL_STEPS)
        if millivolt < _AXP192_DC3_VOL_MIN:
            raise ValueError("Mistake ! DCDC3 minimum output voltage is  %umV" %
                             _AXP192_DC3_VOL_MIN)
        elif millivolt > _AXP192_DC3_VOL_MAX:
            raise ValueError("Mistake ! DCDC3 maximum output voltage is  %umV" %
                             _AXP192_DC3_VOL_MAX)
        super().writeRegister(_AXP192_DC3OUT_VOL, (int)(
            (millivolt - _AXP192_DC3_VOL_MIN) / _AXP192_DC3_VOL_STEPS))

    def getDC3Voltage(self) -> int:
        val = super().readRegister(_AXP192_DC3OUT_VOL)[0]
        return (val * _AXP192_DC3_VOL_STEPS) + _AXP192_DC3_VOL_MIN

    # Power control EXTEN functions
    def enableExternalPin(self) -> None:
        super().setRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 6)

    def disableExternalPin(self) -> None:
        super().clrRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 6)

    def isEnableExternalPin(self) -> bool:
        return bool(super().getRegisterBit(_AXP192_LDO23_DC123_EXT_CTL, 6))

    # Interrupt status functions
    # @brief  Get the interrupt controller mask value.
    # @retval   Mask value corresponds to _AXP192_irq_t ,
    def getIrqStatus(self) -> int:
        self.statusRegister = super().readRegister(_AXP192_INTSTS1, 5)
        return (self.statusRegister[4]) << 32 |\
               (self.statusRegister[3]) << 24 |\
               (self.statusRegister[2]) << 16 |\
               (self.statusRegister[1]) << 8 |\
               (self.statusRegister[0])

     # @brief  Clear interrupt controller state.
    def clearIrqStatus(self) -> None:
        for i in range(_AXP192_INTSTS_CNT-1):
            super().writeRegister(_AXP192_INTSTS1 + i, 0xFF)
        super().writeRegister(_AXP192_INTSTS5, 0xFF)

     # @brief  Eanble PMU interrupt control mask .
     # @ param  opt: View the related chip type _AXP192_irq_t enumeration
     # parameters in "XPowersParams.hpp"
    def enableIRQ(self, opt: int) -> None:
        self._setInterruptImpl(opt, True)

     # @brief  Disable PMU interrupt control mask .
     # @ param  opt: View the related chip type _AXP192_irq_t enumeration
     # parameters in "XPowersParams.hpp"
    def disableIRQ(self, opt: int) -> None:
        self._setInterruptImpl(opt, False)

    # IRQ STATUS 0
    def isAcinOverVoltageIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_ACIN_OVER_VOL_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isAcinInserIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_ACIN_CONNECT_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isAcinRemoveIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_ACIN_REMOVED_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isVbusOverVoltageIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_OVER_VOL_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isVbusInsertIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_INSERT_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isVbusRemoveIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_REMOVE_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isVbusLowVholdIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_VHOLD_LOW_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    # IRQ STATUS 1

    def isBatInsertIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BAT_INSERT_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBatRemoveIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BAT_REMOVE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBattEnterActivateIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BATT_ACTIVATE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBattExitActivateIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BATT_EXIT_ACTIVATE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBatChagerStartIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BAT_CHG_START_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBatChagerDoneIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BAT_CHG_DONE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBattTempHighIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BATT_OVER_TEMP_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBattTempLowIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_BATT_LOW_TEMP_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    # IRQ STATUS 2
    def isChipOverTemperatureIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_CHIP_TEMP_HIGH_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isChargingCurrentLessIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_CHARGE_LOW_CUR_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isDC1VoltageLessIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_DC1_LOW_VOL_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isDC2VoltageLessIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_DC2_LOW_VOL_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isDC3VoltageLessIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_DC3_LOW_VOL_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isPekeyShortPressIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_PKEY_SHORT_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isPekeyLongPressIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_PKEY_LONG_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    # IRQ STATUS 3

    def isNOEPowerOnIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_NOE_ON_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    def isNOEPowerDownIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_NOE_OFF_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    def isVbusEffectiveIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_VAILD_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    def isVbusInvalidIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_INVALID_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    def isVbusSessionIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_SESSION_AB_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    def isVbusSessionEndIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_VBUS_SESSION_END_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    def isLowVoltageLevel2Irq(self) -> bool:
        mask = self.XPOWERS_AXP192_APS_LOW_VOL_LEVEL_IRQ >> 24
        if self.intRegister[3] & mask:
            return super()._IS_BIT_SET(self.statusRegister[3], mask)
        else:
            return False

    # IRQ STATUS 4
    def isWdtExpireIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_TIMER_TIMEOUT_IRQ >> 32
        if self.intRegister[4] & mask:
            return super()._IS_BIT_SET(self.statusRegister[4], mask)
        else:
            return False

    def isGpio2EdgeTriggerIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_GPIO2_EDGE_TRIGGER_IRQ >> 32
        if self.intRegister[4] & mask:
            return super()._IS_BIT_SET(self.statusRegister[4], mask)
        else:
            return False

    def isGpio1EdgeTriggerIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_GPIO1_EDGE_TRIGGER_IRQ >> 32
        if self.intRegister[4] & mask:
            return super()._IS_BIT_SET(self.statusRegister[4], mask)
        else:
            return False

    def isGpio0EdgeTriggerIrq(self) -> bool:
        mask = self.XPOWERS_AXP192_GPIO0_EDGE_TRIGGER_IRQ >> 32
        if self.intRegister[4] & mask:
            return super()._IS_BIT_SET(self.statusRegister[4], mask)
        else:
            return False

    #   ADC Functions
    def enableBattDetection(self) -> None:
        super().setRegisterBit(_AXP192_OFF_CTL, 6)

    def disableBattDetection(self) -> None:
        super().clrRegisterBit(_AXP192_OFF_CTL, 6)

    def enableVbusVoltageMeasure(self) -> None:
        self._setSignalCaptureImpl(
            _MONITOR_USB_CURRENT | _MONITOR_USB_VOLTAGE, True)

    def disableVbusVoltageMeasure(self) -> None:
        self._setSignalCaptureImpl(
            _MONITOR_USB_CURRENT | _MONITOR_USB_VOLTAGE, False)

    def enableBattVoltageMeasure(self) -> None:
        self._setSignalCaptureImpl(
            _MONITOR_BAT_CURRENT | _MONITOR_BAT_VOLTAGE, True)

    def disableBattVoltageMeasure(self) -> None:
        self._setSignalCaptureImpl(
            _MONITOR_BAT_CURRENT | _MONITOR_BAT_VOLTAGE, False)

    def enableSystemVoltageMeasure(self) -> None:
        self._setSignalCaptureImpl(_MONITOR_APS_VOLTAGE, True)

    def disableSystemVoltageMeasure(self) -> None:
        self._setSignalCaptureImpl(_MONITOR_APS_VOLTAGE, False)

    def enableTSPinMeasure(self) -> None:
        self._setSignalCaptureImpl(_MONITOR_TS_PIN, True)

    def disableTSPinMeasure(self) -> None:
        self._setSignalCaptureImpl(_MONITOR_TS_PIN, False)

    def enableAdcChannel(self, opts: int) -> None:
        self._setSignalCaptureImpl(opts, True)

    def disableAdcChannel(self, opts: int) -> None:
        self._setSignalCaptureImpl(opts, False)

    def getVbusVoltage(self) -> int:
        if not self.isVbusIn():
            return 0
        return super().readRegisterH8L4(_AXP192_VBUS_VOL_H8,
                                        _AXP192_VBUS_VOL_L4
                                        ) * _AXP192_VBUS_VOLTAGE_STEP

    def getVbusCurrent(self) -> float:
        if not self.isVbusIn():
            return 0
        return super().readRegisterH8L4(_AXP192_VBUS_CUR_H8,
                                        _AXP192_VBUS_CUR_L4
                                        ) * _AXP192_VBUS_CUR_STEP

    def getBattVoltage(self) -> float:
        if not self.isBatteryConnect():
            return 0
        return super().readRegisterH8L4(_AXP192_BAT_AVERVOL_H8,
                                        _AXP192_BAT_AVERVOL_L4
                                        ) * _AXP192_BATT_VOLTAGE_STEP

    def getBattDischargeCurrent(self) -> float:
        if not self.isBatteryConnect():
            return 0
        return super().readRegisterH8L5(_AXP192_BAT_AVERDISCHGCUR_H8,
                                        _AXP192_BAT_AVERDISCHGCUR_L5) * _AXP192_BATT_DISCHARGE_CUR_STEP

    def getAcinVoltage(self) -> float:
        if not self.isAcinIn():
            return 0
        return super().readRegisterH8L4(_AXP192_ACIN_VOL_H8, _AXP192_ACIN_VOL_L4) * _AXP192_ACIN_VOLTAGE_STEP

    def getAcinCurrent(self) -> float:
        if not self.isAcinIn():
            return 0
        return super().readRegisterH8L4(_AXP192_ACIN_CUR_H8, _AXP192_ACIN_CUR_L4) * _AXP192_ACIN_CUR_STEP

    def getSystemVoltage(self) -> float:
        return super().readRegisterH8L4(_AXP192_APS_AVERVOL_H8, _AXP192_APS_AVERVOL_L4) * _AXP192_APS_VOLTAGE_STEP

    # Timer Control
    def setTimerout(self, minute: int) -> None:
        super().writeRegister(_AXP192_TIMER_CTL, 0x80 | minute)

    def disableTimer(self) -> None:
        super().writeRegister(_AXP192_TIMER_CTL, 0x80)

    def clearTimerFlag(self) -> None:
        super().setRegisterBit(_AXP192_TIMER_CTL, 7)

    # Data Buffer
    def writeDataBuffer(self, data: list,  size: int) -> None:
        if size > _AXP192_DATA_BUFFER_SIZE:
            raise ValueError('Out of range!')
        for i in range(size):
            super().writeRegister(_AXP192_DATA_BUFFER1 + i, data[i])

    def readDataBuffer(self, size: int) -> list:
        if size > _AXP192_DATA_BUFFER_SIZE:
            raise ValueError('Out of range!')
        data = [0]*size
        for i in range(size):
            data[i] = super().readRegister(_AXP192_DATA_BUFFER1 + i)[0]
        return list(data)

    # Charge led functions
    # @brief Set charging led mode.
    # @retval See xpowers_chg_led_mode_t enum for details.
    def setChargingLedMode(self, mode: int) -> None:
        range = [self.XPOWERS_CHG_LED_OFF, self.XPOWERS_CHG_LED_BLINK_1HZ,
                 self.XPOWERS_CHG_LED_BLINK_4HZ, self.XPOWERS_CHG_LED_ON]
        if mode in range:
            val = super().readRegister(_AXP192_OFF_CTL)[0]
            val &= 0xC7
            val |= 0x08      # use manual ctrl
            val |= (mode << 4)
            super().writeRegister(_AXP192_OFF_CTL, val)
        else:
            super().clrRegisterBit(_AXP192_OFF_CTL, 3)

    def getChargingLedMode(self) -> int:
        if not bool(super().getRegisterBit(_AXP192_OFF_CTL, 3)):
            return self.XPOWERS_CHG_LED_CTRL_CHG
        val = super().readRegister(_AXP192_OFF_CTL)[0]
        val &= 0x30
        return val >> 4

    # Coulomb counter control
    def enableCoulomb(self) -> None:
        super().setRegisterBit(_AXP192_COULOMB_CTL, 7)

    def disableCoulomb(self) -> None:
        super().clrRegisterBit(_AXP192_COULOMB_CTL, 7)

    def stopCoulomb(self) -> None:
        super().setRegisterBit(_AXP192_COULOMB_CTL, 6)

    def clearCoulomb(self) -> None:
        super().setRegisterBit(_AXP192_COULOMB_CTL, 5)

    def getBattChargeCoulomb(self) -> int:
        data = [0]*4
        data[0] = super().readRegister(_AXP192_BAT_CHGCOULOMB3)[0]
        data[1] = super().readRegister(_AXP192_BAT_CHGCOULOMB2)[0]
        data[2] = super().readRegister(_AXP192_BAT_CHGCOULOMB1)[0]
        data[3] = super().readRegister(_AXP192_BAT_CHGCOULOMB0)[0]
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]

    def getBattDischargeCoulomb(self) -> int:
        data = [0]*4
        data[0] = super().readRegister(_AXP192_BAT_DISCHGCOULOMB3)[0]
        data[1] = super().readRegister(_AXP192_BAT_DISCHGCOULOMB2)[0]
        data[2] = super().readRegister(_AXP192_BAT_DISCHGCOULOMB1)[0]
        data[3] = super().readRegister(_AXP192_BAT_DISCHGCOULOMB0)[0]
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]

    def getAdcSamplingRate(self) -> int:
        val = super().readRegister(_AXP192_ADC_SPEED)[0]
        return 25 * math.pow(2, (val & 0xC0) >> 6)

    def getCoulombData(self) -> float:
        charge = self.getBattChargeCoulomb()
        discharge = self.getBattDischargeCoulomb()
        rate = self.getAdcSamplingRate()
        return 65536.0 * 0.5 * (charge - discharge) / 3600.0 / rate

    # GPIO control functions
    def getBatteryChargeCurrent(self) -> float:
        return super().readRegisterH8L5(
            _AXP192_BAT_AVERCHGCUR_H8,
            _AXP192_BAT_AVERCHGCUR_L5
        ) * _AXP192_BATT_CHARGE_CUR_STEP

    def getGpio0Voltage(self) -> int:
        return super().readRegisterH8L4(_AXP192_GPIO0_VOL_ADC_H8, _AXP192_GPIO0_VOL_ADC_L4) * _AXP192_GPIO0_STEP * 1000

    def getGpio1Voltage(self) -> int:
        return super().readRegisterH8L4(_AXP192_GPIO1_VOL_ADC_H8, _AXP192_GPIO1_VOL_ADC_L4) * _AXP192_GPIO1_STEP * 1000

    def pwmSetup(self, channel: int,  freq: int, duty: int) -> None:
        #  PWM输出频率 = 2.25MHz / (X+1) / Y1
        #  PWM输出占空比 = Y2 / Y1
        if channel == 0:
            super().writeRegister(_AXP192_PWM1_FREQ_SET,  freq)
            super().writeRegister(_AXP192_PWM1_DUTY_SET1, duty >> 8)
            super().writeRegister(_AXP192_PWM1_DUTY_SET2, duty & 0xFF)
        elif channel == 1:
            super().writeRegister(_AXP192_PWM2_FREQ_SET,  freq)
            super().writeRegister(_AXP192_PWM2_DUTY_SET1, duty >> 8)
            super().writeRegister(_AXP192_PWM2_DUTY_SET2, duty & 0xFF)

    def pwmEnable(self, channel) -> None:
        if channel == 0:
            val = super().readRegister(_AXP192_GPIO1_CTL)[0] & 0xF8
            super().writeRegister(_AXP192_GPIO1_CTL, val | 0x02)
        elif channel == 1:
            val = super().readRegister(_AXP192_GPIO2_CTL)[0] & 0xF8
            super().writeRegister(_AXP192_GPIO2_CTL, val | 0x02)

    def getBatteryPercent(self) -> int:
        if not self.isBatteryConnect():
            return -1
        table = [3000, 3650, 3700, 3740, 3760, 3795,
                 3840, 3910, 3980, 4070, 4150]
        voltage = self.getBattVoltage()
        if voltage < table[0]:
            return 0
        for i in range(11):
            if voltage < table[i]:
                return i * 10 - (10 * (int)(table[i] - voltage)) / (int)(table[i] - table[i - 1])
        return 100

    def getChipID(self) -> int:
        return super().readRegister(_AXP192_IC_TYPE)[0]

    # GPIO setting

    def pinMode(self, pin: int,  mode: int) -> None:
        if pin == self.PMU_GPIO0:
            '''
            * 000: NMOS open-drain output
            * 001: Universal input function
            * 010: Low noise LDO
            * 011: reserved
            * 100: ADC input
            * 101: Low output
            * 11X: Floating
            '''
            if mode == self.INPUT or mode == self.INPUT_PULLDOWN:
                if self.gpio[pin].mode != self.INPUT:
                    self.gpio[pin].mode = self.INPUT
                val = super().readRegister(_AXP192_GPIO0_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO0_CTL, val | 0x01)
                # Set pull-down mode
                val = super().readRegister(_AXP192_GPIO012_PULLDOWN)[0] & 0xFE
                if mode == self.INPUT_PULLDOWN:
                    super().writeRegister(_AXP192_GPIO012_PULLDOWN, val | 0x01)
                else:
                    super().writeRegister(_AXP192_GPIO012_PULLDOWN, val)

        elif pin == self.PMU_GPIO1:
            '''
            * 000: NMOS open-drain output
            * 001: Universal input function
            * 010: PWM1 output, high level is VINT, not Can add less than 100K pull-down resistance
            * 011: reserved
            * 100: ADC input
            * 101: Low output
            * 11X: Floating
            '''
            if mode == self.INPUT or mode == self.INPUT_PULLDOWN:
                if self.gpio[pin].mode != self.INPUT:
                    self.gpio[pin].mode = self.INPUT
                val = super().readRegister(_AXP192_GPIO1_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO1_CTL, val | 0x01)
                # Set pull-down mode
                val = super().readRegister(_AXP192_GPIO012_PULLDOWN)[0] & 0xFD
                if mode == self.INPUT_PULLDOWN:
                    super().writeRegister(_AXP192_GPIO012_PULLDOWN, val | 0x02)
                else:
                    super().writeRegister(_AXP192_GPIO012_PULLDOWN, val)

        elif pin == self.PMU_GPIO2:
            '''
            * 000: NMOS open-drain output
            * 001: Universal input function
            * 010: PWM2 output, high level is VINT, not Can add less than 100K pull-down resistance
            * 011: reserved
            * 100: ADC input
            * 101: Low output
            * 11X: Floating
            '''
            if mode == self.INPUT or mode == self.INPUT_PULLDOWN:
                if self.gpio[pin].mode != self.INPUT:
                    self.gpio[pin].mode = self.INPUT
                val = super().readRegister(_AXP192_GPIO2_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO2_CTL, val | 0x01)

                # Set pull-down mode
                val = super().readRegister(_AXP192_GPIO012_PULLDOWN)[0] & 0xFB
                if mode == self.INPUT_PULLDOWN:
                    super().writeRegister(_AXP192_GPIO012_PULLDOWN, val | 0x04)
                else:
                    super().writeRegister(_AXP192_GPIO012_PULLDOWN, val)
        elif pin == self.PMU_GPIO3:
            '''
            * 00: External charging control
            * 01: NMOS open-drain output port 3
            * 10: Universal input port 3
            * 11: ADC input
            '''
            if mode == self.INPUT:
                if self.gpio[pin].mode != self.INPUT:
                    self.gpio[pin].mode = self.INPUT
                val = super().readRegister(_AXP192_GPIO34_CTL)[0] & 0xFC
                super().writeRegister(_AXP192_GPIO34_CTL, val | 0x82)

        elif pin == self.PMU_GPIO4:
            '''
            * 00: External charging control
            * 01: NMOS open-drain output port 4
            * 10: Universal input port 4
            * 11: undefined
            '''
            if mode == self.INPUT:
                if self.gpio[pin].mode != self.INPUT:
                    self.gpio[pin].mode = self.INPUT
                val = super().readRegister(_AXP192_GPIO34_CTL)[0] & 0xF3
                super().writeRegister(_AXP192_GPIO34_CTL, val | 0x88)

        elif pin == self.PMU_GPIO5:
            if mode == self.INPUT:
                if self.gpio[pin].mode != self.INPUT:
                    self.gpio[pin].mode = self.INPUT

                val = super().readRegister(_AXP192_GPIO5_CTL)[0] & 0xBF
                super().writeRegister(_AXP192_GPIO5_CTL, val | 0x40)
        else:
            print('gpio is invalid')

    def digitalRead(self, pin: int) -> bool:
        if pin == self.PMU_GPIO0:
            return bool(super().getRegisterBit(_AXP192_GPIO012_SIGNAL, 4))
        elif pin == self.PMU_GPIO1:
            return bool(super().getRegisterBit(_AXP192_GPIO012_SIGNAL, 5))
        elif pin == self.PMU_GPIO2:
            return bool(super().getRegisterBit(_AXP192_GPIO012_SIGNAL, 6))
        elif pin == self.PMU_GPIO3:
            return bool(super().getRegisterBit(_AXP192_GPIO34_SIGNAL, 4))
        elif pin == self.PMU_GPIO4:
            return bool(super().getRegisterBit(_AXP192_GPIO34_SIGNAL, 5))
        elif pin == self.PMU_GPIO5:
            return bool(super().getRegisterBit(_AXP192_GPIO5_CTL, 4))
        else:
            print('gpio is invalid')
        return 0

    def digitalWrite(self, pin: int, val: int) -> None:
        if pin == self.PMU_GPIO0:
            if self.gpio[pin].mode != self.OUTPUT:
                self.gpio[pin].mode = self.OUTPUT
            reg = super().readRegister(_AXP192_GPIO0_CTL)[0] & 0xF8
            val = (reg, (reg | 0x05))[val]
            print(bin(val))
            super().writeRegister(_AXP192_GPIO0_CTL,  val)

        elif pin == self.PMU_GPIO1:
            if self.gpio[pin].mode != self.OUTPUT:
                self.gpio[pin].mode = self.OUTPUT
            reg = super().readRegister(_AXP192_GPIO1_CTL)[0] & 0xF8
            val = (reg, (reg | 0x05))[val]
            super().writeRegister(_AXP192_GPIO1_CTL,  val)

        elif pin == self.PMU_GPIO2:
            if self.gpio[pin].mode != self.OUTPUT:
                self.gpio[pin].mode = self.OUTPUT
            reg = super().readRegister(_AXP192_GPIO2_CTL)[0] & 0xF8
            val = (reg, (reg | 0x05))[val]
            super().writeRegister(_AXP192_GPIO2_CTL,  val)

        elif pin == self.PMU_GPIO3:
            if self.gpio[pin].mode != self.OUTPUT:
                self.gpio[pin].mode = self.OUTPUT
                reg = super().readRegister(_AXP192_GPIO34_CTL)[0] & 0xFC
                super().writeRegister(_AXP192_GPIO34_CTL,   reg | 0x01)

            reg = super().readRegister(_AXP192_GPIO34_SIGNAL)[0] & 0xF7
            val = (reg, (reg | 0x01))[val]
            super().writeRegister(_AXP192_GPIO34_SIGNAL,   val)

        elif pin == self.PMU_GPIO4:
            if self.gpio[pin].mode != self.OUTPUT:
                self.gpio[pin].mode = self.OUTPUT
                reg = super().readRegister(_AXP192_GPIO34_CTL)[0] & 0xF3
                super().writeRegister(_AXP192_GPIO34_CTL,  reg | 0x04)

            reg = super().readRegister(_AXP192_GPIO34_SIGNAL)[0] & 0xEF
            val = (reg, (reg | 0x01))[val]
            super().writeRegister(_AXP192_GPIO34_SIGNAL,   val)

        elif pin == self.PMU_GPIO5:
            if self.gpio[pin].mode != self.OUTPUT:
                self.gpio[pin].mode = self.OUTPUT
                reg = super().readRegister(_AXP192_GPIO5_CTL)[0] & 0xBF
                super().writeRegister(_AXP192_GPIO5_CTL,  reg)

            reg = super().readRegister(_AXP192_GPIO5_CTL)[0] & 0xDF
            val = (reg, (reg | 0x01))[val]
            super().writeRegister(_AXP192_GPIO5_CTL, val)
        else:
            print('gpio is invalid')

    #  ! Need FIX
    def analogRead(self, pin: int) -> int:
        if pin == self.PMU_GPIO0:
            if self.gpio[pin].mode != self.ANALOG:
                #  Enable GPIO ADC Function
                val = super().readRegister(_AXP192_GPIO0_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO0_CTL, val | 0x04)

                # Enable ADC2 / GPIO0
                #  val = super().readRegister(_AXP192_ADC_EN2)[0] | 0x08
                #  super().writeRegister(_AXP192_ADC_EN2, val )
                super().setRegisterBit(_AXP192_ADC_EN2, 3)

                #  Set adc input range 0~2.0475V
                super().clrRegisterBit(_AXP192_ADC_INPUTRANGE, 0)
                self.gpio[pin].mode = self.ANALOG

            return super().readRegisterH8L4(_AXP192_GPIO0_VOL_ADC_H8, _AXP192_GPIO0_VOL_ADC_L4)

        elif pin == self.PMU_GPIO1:
            if self.gpio[pin].mode != self.ANALOG:
                #  Enable GPIO ADC Function
                val = super().readRegister(_AXP192_GPIO1_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO1_CTL, val | 0x04)

                # Enable ADC2 / GPIO1
                #  val = super().readRegister(_AXP192_ADC_EN2)[0] | 0x04
                #  super().writeRegister(_AXP192_ADC_EN2, val )
                super().setRegisterBit(_AXP192_ADC_EN2, 2)

                #  Set adc input range 0~2.0475V
                super().clrRegisterBit(_AXP192_ADC_INPUTRANGE, 1)
                self.gpio[pin].mode = self.ANALOG

            return super().readRegisterH8L4(_AXP192_GPIO1_VOL_ADC_H8, _AXP192_GPIO1_VOL_ADC_L4)

        elif pin == self.PMU_GPIO2:
            if self.gpio[pin].mode != self.ANALOG:
                #  Enable GPIO ADC Function
                val = super().readRegister(_AXP192_GPIO1_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO1_CTL, val | 0x04)
                # Enable ADC2 / GPIO1
                #  val = super().readRegister(_AXP192_ADC_EN2)[0] | 0x02
                #  super().writeRegister(_AXP192_ADC_EN2, val )
                super().setRegisterBit(_AXP192_ADC_EN2, 1)

                #  Set adc input range 0~2.0475V
                super().clrRegisterBit(_AXP192_ADC_INPUTRANGE, 2)
                self.gpio[pin].mode = self.ANALOG

            return super().readRegisterH8L4(_AXP192_GPIO2_VOL_ADC_H8, _AXP192_GPIO2_VOL_ADC_L4)

        elif pin == self.PMU_GPIO3:
            if self.gpio[pin].mode != self.ANALOG:
                #  Enable GPIO ADC Function
                val = super().readRegister(_AXP192_GPIO1_CTL)[0] & 0xF8
                super().writeRegister(_AXP192_GPIO1_CTL, val | 0x04)

                # Enable ADC2 / GPIO1
                super().setRegisterBit(_AXP192_ADC_EN2, 0)

                #  Set adc input range 0~2.0475V
                super().clrRegisterBit(_AXP192_ADC_INPUTRANGE, 3)
                self.gpio[pin].mode = self.ANALOG

            return super().readRegisterH8L4(_AXP192_GPIO3_VOL_ADC_H8, _AXP192_GPIO3_VOL_ADC_L4)

        elif pin == self.PMU_TS_PIN:
            if self.gpio[pin].mode != self.ANALOG:
                #  Enable TS PIN ADC Function
                super().setRegisterBit(_AXP192_ADC_SPEED, 2)
                #  val = super().readRegister(_AXP192_ADC_SPEED)[0] & 0xFB
                #  super().writeRegister(_AXP192_ADC_SPEED, val | 0x04)
                self.gpio[pin].mode = self.ANALOG

            return super().readRegisterH8L4(_AXP192_TS_IN_H8, _AXP192_TS_IN_L4)
        else:
            print('gpio is invalid')

        return 0

    # Sleep function

    def enableSleep(self) -> None:
        super().setRegisterBit(_AXP192_VOFF_SET, 3)

    # Pekey function
    # @brief Set the PEKEY power-on long press time.
    # @param opt: See xpowers_press_on_time_t enum for details.
    def setPowerKeyPressOnTime(self, opt: int) -> None:
        val = super().readRegister(_AXP192_POK_SET)[0]
        super().writeRegister(_AXP192_POK_SET, (val & 0x3F) | (opt << 6))

    # @brief Get the PEKEY power-on long press time.
    # @retval See xpowers_press_on_time_t enum for details.

    def getPowerKeyPressOnTime(self) -> int:
        val = super().readRegister(_AXP192_POK_SET)[0]
        return (val & 0xC0) >> 6

    # @brief Set the PEKEY power-off long press time.
    # @ param opt: See xpowers_press_off_time_t enum for details.

    def setPowerKeyPressOffTime(self, opt: int) -> None:
        val = super().readRegister(_AXP192_POK_SET)[0]
        super().writeRegister(_AXP192_POK_SET, (val & 0xFC) | opt)

    # @brief Get the PEKEY power-off long press time.
    # @retval See xpowers_press_off_time_t enum for details.
    def getPowerKeyPressOffTime(self) -> int:
        val = super().readRegister(_AXP192_POK_SET)[0]
        return (val & 0x03)

    def setPowerKeyLongPressOnTime(self, opt: int) -> None:
        val = super().readRegister(_AXP192_POK_SET)[0]
        super().writeRegister(_AXP192_POK_SET, (val & 0xCF) | (opt << 4))

    def enablePowerKeyLongPressPowerOff(self) -> None:
        super().setRegisterBit(_AXP192_POK_SET, 3)

    def disablePowerKeyLongPressPowerOff(self) -> None:
        super().clrRegisterBit(_AXP192_POK_SET, 3)

    # Interrupt control functions
    def _setInterruptImpl(self, opts: int, enable: bool) -> None:
        # log_d("%s %s - 0x%llx\n", __func__, enable ? "ENABLE": "DISABLE", opts)
        if opts & 0xFF:
            value = opts & 0xFF
            data = super().readRegister(_AXP192_INTEN1)[0]
            self.intRegister[0] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP192_INTEN1, self.intRegister[0])

        if opts & 0xFF00:
            value = opts >> 8
            data = super().readRegister(_AXP192_INTEN2)[0]
            self.intRegister[1] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP192_INTEN2, self.intRegister[1])

        if opts & 0xFF0000:
            value = opts >> 16
            data = super().readRegister(_AXP192_INTEN3)[0]
            self.intRegister[2] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP192_INTEN3, self.intRegister[2])

        if opts & 0xFF000000:
            value = opts >> 24
            data = super().readRegister(_AXP192_INTEN4)[0]
            self.intRegister[3] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP192_INTEN4, self.intRegister[3])

        if opts & 0xFF00000000:
            value = opts >> 32
            data = super().readRegister(_AXP192_INTEN5)[0]
            self.intRegister[4] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP192_INTEN5, self.intRegister[4])

    # Signal Capture control functions
    def _setSignalCaptureImpl(self, opts: int,  enable: bool) -> None:
        if opts & 0xFF:
            value = super().readRegister(_AXP192_ADC_EN1)[0]
            value = ((value & (~opts)), (value | opts))[enable]
            super().writeRegister(_AXP192_ADC_EN1, value)

        if opts & 0xFF00:
            opts >>= 8
            value = super().readRegister(_AXP192_ADC_EN2)[0]
            value = ((value & (~opts)), (value | opts))[enable]
            super().writeRegister(_AXP192_ADC_EN2, value)
