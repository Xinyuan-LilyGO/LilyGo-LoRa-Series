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

@file      AXP2101.py
@author    Lewis He (lewishe@outlook.com)
@date      2022-10-20

'''

from I2CInterface import *


_AXP2101_STATUS1 = const(0x00)
_AXP2101_STATUS2 = const(0x01)
_AXP2101_IC_TYPE = const(0x03)
_AXP2101_DATA_BUFFER1 = const(0x04)
_AXP2101_DATA_BUFFER2 = const(0x05)
_AXP2101_DATA_BUFFER3 = const(0x06)
_AXP2101_DATA_BUFFER4 = const(0x07)
_AXP2101_DATA_BUFFER_SIZE = const(4)
_AXP2101_COMMON_CONFIG = const(0x10)
_AXP2101_BATFET_CTRL = const(0x12)
_AXP2101_DIE_TEMP_CTRL = const(0x13)
_AXP2101_MIN_SYS_VOL_CTRL = const(0x14)
_AXP2101_INPUT_VOL_LIMIT_CTRL = const(0x15)
_AXP2101_INPUT_CUR_LIMIT_CTRL = const(0x16)
_AXP2101_RESET_FUEL_GAUGE = const(0x17)
_AXP2101_CHARGE_GAUGE_WDT_CTRL = const(0x18)
_AXP2101_WDT_CTRL = const(0x19)
_AXP2101_LOW_BAT_WARN_SET = const(0x1A)
_AXP2101_PWRON_STATUS = const(0x20)
_AXP2101_PWROFF_STATUS = const(0x21)
_AXP2101_PWROFF_EN = const(0x22)
_AXP2101_DC_OVP_UVP_CTRL = const(0x23)
_AXP2101_VOFF_SET = const(0x24)
_AXP2101_PWROK_SEQU_CTRL = const(0x25)
_AXP2101_SLEEP_WAKEUP_CTRL = const(0x26)
_AXP2101_IRQ_OFF_ON_LEVEL_CTRL = const(0x27)
_AXP2101_FAST_PWRON_SET0 = const(0x28)
_AXP2101_FAST_PWRON_SET1 = const(0x29)
_AXP2101_FAST_PWRON_SET2 = const(0x2A)
_AXP2101_FAST_PWRON_CTRL = const(0x2B)
_AXP2101_ADC_CHANNEL_CTRL = const(0x30)
_AXP2101_ADC_DATA_RELUST0 = const(0x34)
_AXP2101_ADC_DATA_RELUST1 = const(0x35)
_AXP2101_ADC_DATA_RELUST2 = const(0x36)
_AXP2101_ADC_DATA_RELUST3 = const(0x37)
_AXP2101_ADC_DATA_RELUST4 = const(0x38)
_AXP2101_ADC_DATA_RELUST5 = const(0x39)
_AXP2101_ADC_DATA_RELUST6 = const(0x3A)
_AXP2101_ADC_DATA_RELUST7 = const(0x3B)
_AXP2101_ADC_DATA_RELUST8 = const(0x3C)
_AXP2101_ADC_DATA_RELUST9 = const(0x3D)
# INTERRUPT REGISTER
_AXP2101_INTEN1 = const(0x40)
_AXP2101_INTEN2 = const(0x41)
_AXP2101_INTEN3 = const(0x42)
# INTERRUPT STATUS REGISTER
_AXP2101_INTSTS1 = const(0x48)
_AXP2101_INTSTS2 = const(0x49)
_AXP2101_INTSTS3 = const(0x4A)
_AXP2101_INTSTS_CNT = const(3)
_AXP2101_TS_PIN_CTRL = const(0x50)
_AXP2101_TS_HYSL2H_SET = const(0x52)
_AXP2101_TS_LYSL2H_SET = const(0x53)
_AXP2101_VLTF_CHG_SET = const(0x54)
_AXP2101_VHLTF_CHG_SET = const(0x55)
_AXP2101_VLTF_WORK_SET = const(0x56)
_AXP2101_VHLTF_WORK_SET = const(0x57)
_AXP2101_JIETA_EN_CTRL = const(0x58)
_AXP2101_JIETA_SET0 = const(0x59)
_AXP2101_JIETA_SET1 = const(0x5A)
_AXP2101_JIETA_SET2 = const(0x5B)
_AXP2101_IPRECHG_SET = const(0x61)
_AXP2101_ICC_CHG_SET = const(0x62)
_AXP2101_ITERM_CHG_SET_CTRL = const(0x63)
_AXP2101_CV_CHG_VOL_SET = const(0x64)
_AXP2101_THE_REGU_THRES_SET = const(0x65)
_AXP2101_CHG_TIMEOUT_SET_CTRL = const(0x67)
_AXP2101_BAT_DET_CTRL = const(0x68)
_AXP2101_CHGLED_SET_CTRL = const(0x69)
_AXP2101_BTN_VOL_MIN = const(2600)
_AXP2101_BTN_VOL_MAX = const(3300)
_AXP2101_BTN_VOL_STEPS = const(100)
_AXP2101_BTN_BAT_CHG_VOL_SET = const(0x6A)
_AXP2101_DC_ONOFF_DVM_CTRL = const(0x80)
_AXP2101_DC_FORCE_PWM_CTRL = const(0x81)
_AXP2101_DC_VOL0_CTRL = const(0x82)
_AXP2101_DC_VOL1_CTRL = const(0x83)
_AXP2101_DC_VOL2_CTRL = const(0x84)
_AXP2101_DC_VOL3_CTRL = const(0x85)
_AXP2101_DC_VOL4_CTRL = const(0x86)
_AXP2101_LDO_ONOFF_CTRL0 = const(0x90)
_AXP2101_LDO_ONOFF_CTRL1 = const(0x91)
_AXP2101_LDO_VOL0_CTRL = const(0x92)
_AXP2101_LDO_VOL1_CTRL = const(0x93)
_AXP2101_LDO_VOL2_CTRL = const(0x94)
_AXP2101_LDO_VOL3_CTRL = const(0x95)
_AXP2101_LDO_VOL4_CTRL = const(0x96)
_AXP2101_LDO_VOL5_CTRL = const(0x97)
_AXP2101_LDO_VOL6_CTRL = const(0x98)
_AXP2101_LDO_VOL7_CTRL = const(0x99)
_AXP2101_LDO_VOL8_CTRL = const(0x9A)
_AXP2101_BAT_PARAME = const(0xA1)
_AXP2101_FUEL_GAUGE_CTRL = const(0xA2)
_AXP2101_BAT_PERCENT_DATA = const(0xA4)
# DCDC 1~5
_AXP2101_DCDC1_VOL_MIN = const(1500)
_AXP2101_DCDC1_VOL_MAX = const(3400)
_AXP2101_DCDC1_VOL_STEPS = const(100)
_AXP2101_DCDC2_VOL1_MIN = const(500)
_AXP2101_DCDC2_VOL1_MAX = const(1200)
_AXP2101_DCDC2_VOL2_MIN = const(1220)
_AXP2101_DCDC2_VOL2_MAX = const(1540)
_AXP2101_DCDC2_VOL_STEPS1 = const(10)
_AXP2101_DCDC2_VOL_STEPS2 = const(20)
_AXP2101_DCDC2_VOL_STEPS1_BASE = const(0)
_AXP2101_DCDC2_VOL_STEPS2_BASE = const(71)
_AXP2101_DCDC3_VOL1_MIN = const(500)
_AXP2101_DCDC3_VOL1_MAX = const(1200)
_AXP2101_DCDC3_VOL2_MIN = const(1220)
_AXP2101_DCDC3_VOL2_MAX = const(1540)
_AXP2101_DCDC3_VOL3_MIN = const(1600)
_AXP2101_DCDC3_VOL3_MAX = const(3400)
_AXP2101_DCDC3_VOL_MIN = const(500)
_AXP2101_DCDC3_VOL_MAX = const(3400)
_AXP2101_DCDC3_VOL_STEPS1 = const(10)
_AXP2101_DCDC3_VOL_STEPS2 = const(20)
_AXP2101_DCDC3_VOL_STEPS3 = const(100)
_AXP2101_DCDC3_VOL_STEPS1_BASE = const(0)
_AXP2101_DCDC3_VOL_STEPS2_BASE = const(71)
_AXP2101_DCDC3_VOL_STEPS3_BASE = const(88)
_AXP2101_DCDC4_VOL1_MIN = const(500)
_AXP2101_DCDC4_VOL1_MAX = const(1200)
_AXP2101_DCDC4_VOL2_MIN = const(1220)
_AXP2101_DCDC4_VOL2_MAX = const(1840)
_AXP2101_DCDC4_VOL_STEPS1 = const(10)
_AXP2101_DCDC4_VOL_STEPS2 = const(20)
_AXP2101_DCDC4_VOL_STEPS1_BASE = const(0)
_AXP2101_DCDC4_VOL_STEPS2_BASE = const(71)
_AXP2101_DCDC5_VOL_1200MV = const(1200)
_AXP2101_DCDC5_VOL_VAL = const(0x19)
_AXP2101_DCDC5_VOL_MIN = const(1400)
_AXP2101_DCDC5_VOL_MAX = const(3700)
_AXP2101_DCDC5_VOL_STEPS = const(100)
_AXP2101_VSYS_VOL_THRESHOLD_MIN = const(2600)
_AXP2101_VSYS_VOL_THRESHOLD_MAX = const(3300)
_AXP2101_VSYS_VOL_THRESHOLD_STEPS = const(100)
# ALDO 1~4
_AXP2101_ALDO1_VOL_MIN = const(500)
_AXP2101_ALDO1_VOL_MAX = const(3500)
_AXP2101_ALDO1_VOL_STEPS = const(100)
_AXP2101_ALDO2_VOL_MIN = const(500)
_AXP2101_ALDO2_VOL_MAX = const(3500)
_AXP2101_ALDO2_VOL_STEPS = const(100)
_AXP2101_ALDO3_VOL_MIN = const(500)
_AXP2101_ALDO3_VOL_MAX = const(3500)
_AXP2101_ALDO3_VOL_STEPS = const(100)
_AXP2101_ALDO4_VOL_MIN = const(500)
_AXP2101_ALDO4_VOL_MAX = const(3500)
_AXP2101_ALDO4_VOL_STEPS = const(100)
# BLDO 1~2
_AXP2101_BLDO1_VOL_MIN = const(500)
_AXP2101_BLDO1_VOL_MAX = const(3500)
_AXP2101_BLDO1_VOL_STEPS = const(100)
_AXP2101_BLDO2_VOL_MIN = const(500)
_AXP2101_BLDO2_VOL_MAX = const(3500)
_AXP2101_BLDO2_VOL_STEPS = const(100)
# CPUSLDO
_AXP2101_CPUSLDO_VOL_MIN = const(500)
_AXP2101_CPUSLDO_VOL_MAX = const(1400)
_AXP2101_CPUSLDO_VOL_STEPS = const(50)
# DLDO 1~2
_AXP2101_DLDO1_VOL_MIN = const(500)
_AXP2101_DLDO1_VOL_MAX = const(3400)
_AXP2101_DLDO1_VOL_STEPS = const(100)
_AXP2101_DLDO2_VOL_MIN = const(500)
_AXP2101_DLDO2_VOL_MAX = const(3400)
_AXP2101_DLDO2_VOL_STEPS = const(100)

AXP2101_SLAVE_ADDRESS = const(0x34)
XPOWERS_AXP2101_CHIP_ID = const(0x4A)


class AXP2101(I2CInterface):

    # ARGS ARGS ARGS ARGS ARGS ARGS ARGS ARGS ARGS

    """Power ON OFF IRQ timmint control values"""
    XPOWERS_AXP2101_IRQ_TIME_1S = const(0)
    XPOWERS_AXP2101_IRQ_TIME_1S5 = const(1)
    XPOWERS_AXP2101_IRQ_TIME_2S = const(2)
    XPOWERS_AXP2101_PRESSOFF_2S5 = const(3)

    """Precharge current limit values"""
    XPOWERS_AXP2101_PRECHARGE_25MA = const(1)
    XPOWERS_AXP2101_PRECHARGE_50MA = const(2)
    XPOWERS_AXP2101_PRECHARGE_75MA = const(3)
    XPOWERS_AXP2101_PRECHARGE_100MA = const(4)
    XPOWERS_AXP2101_PRECHARGE_125MA = const(5)
    XPOWERS_AXP2101_PRECHARGE_150MA = const(6)
    XPOWERS_AXP2101_PRECHARGE_175MA = const(7)
    XPOWERS_AXP2101_PRECHARGE_200MA = const(8)

    """Charging termination of current limit"""
    XPOWERS_AXP2101_CHG_ITERM_0MA = const(0)
    XPOWERS_AXP2101_CHG_ITERM_25MA = const(1)
    XPOWERS_AXP2101_CHG_ITERM_50MA = const(2)
    XPOWERS_AXP2101_CHG_ITERM_75MA = const(3)
    XPOWERS_AXP2101_CHG_ITERM_100MA = const(4)
    XPOWERS_AXP2101_CHG_ITERM_125MA = const(5)
    XPOWERS_AXP2101_CHG_ITERM_150MA = const(6)
    XPOWERS_AXP2101_CHG_ITERM_175MA = const(7)
    XPOWERS_AXP2101_CHG_ITERM_200MA = const(8)

    """Thermal regulation threshold setting"""
    XPOWERS_AXP2101_THREMAL_60DEG = const(0)
    XPOWERS_AXP2101_THREMAL_80DEG = const(1)
    XPOWERS_AXP2101_THREMAL_100DEG = const(2)
    XPOWERS_AXP2101_THREMAL_120DEG = const(3)

    """Charging  status values"""
    XPOWERS_AXP2101_CHG_TRI_STATE = const(0)  # tri_charge
    XPOWERS_AXP2101_CHG_PRE_STATE = const(1)  # pre_charge
    XPOWERS_AXP2101_CHG_CC_STATE = const(2)  # constant charge
    XPOWERS_AXP2101_CHG_CV_STATE = const(3)  # constant voltage
    XPOWERS_AXP2101_CHG_DONE_STATE = const(4)  # charge done
    XPOWERS_AXP2101_CHG_STOP_STATE = const(5)  # not chargin

    """PMU wakeup method values"""
    XPOWERS_AXP2101_WAKEUP_IRQ_PIN_TO_LOW = const(1 << 4)
    XPOWERS_AXP2101_WAKEUP_PWROK_TO_LOW = const(1 << 3)
    XPOWERS_AXP2101_WAKEUP_DC_DLO_SELECT = const(1 << 2)

    """Fast Power On start sequence values"""
    XPOWERS_AXP2101_FAST_DCDC1 = const(0)
    XPOWERS_AXP2101_FAST_DCDC2 = const(1)
    XPOWERS_AXP2101_FAST_DCDC3 = const(2)
    XPOWERS_AXP2101_FAST_DCDC4 = const(3)
    XPOWERS_AXP2101_FAST_DCDC5 = const(4)
    XPOWERS_AXP2101_FAST_ALDO1 = const(5)
    XPOWERS_AXP2101_FAST_ALDO2 = const(6)
    XPOWERS_AXP2101_FAST_ALDO3 = const(7)
    XPOWERS_AXP2101_FAST_ALDO4 = const(8)
    XPOWERS_AXP2101_FAST_BLDO1 = const(9)
    XPOWERS_AXP2101_FAST_BLDO2 = const(10)
    XPOWERS_AXP2101_FAST_CPUSLDO = const(11)
    XPOWERS_AXP2101_FAST_DLDO1 = const(12)
    XPOWERS_AXP2101_FAST_DLDO2 = const(13)

    """Fast Power On start sequence values"""
    XPOWERS_AXP2101_SEQUENCE_LEVEL_0 = const(0)
    XPOWERS_AXP2101_SEQUENCE_LEVEL_1 = const(1)
    XPOWERS_AXP2101_SEQUENCE_LEVEL_2 = const(2)
    XPOWERS_AXP2101_SEQUENCE_DISABLE = const(3)

    """Watchdog config values"""
    # Just interrupt to pin
    XPOWERS_AXP2101_WDT_IRQ_TO_PIN = const(0)
    # IRQ to pin and reset pmu system
    XPOWERS_AXP2101_WDT_IRQ_AND_RSET = const(1)
    # IRQ to pin and reset pmu systempull down pwrok
    XPOWERS_AXP2101_WDT_IRQ_AND_RSET_PD_PWROK = const(2)
    # IRQ to pin and reset pmu systemturn off dcdc & ldo pull down pwrok
    XPOWERS_AXP2101_WDT_IRQ_AND_RSET_ALL_OFF = const(3)

    """Watchdog timeout values"""
    XPOWERS_AXP2101_WDT_TIMEOUT_1S = const(0)
    XPOWERS_AXP2101_WDT_TIMEOUT_2S = const(1)
    XPOWERS_AXP2101_WDT_TIMEOUT_4S = const(2)
    XPOWERS_AXP2101_WDT_TIMEOUT_8S = const(3)
    XPOWERS_AXP2101_WDT_TIMEOUT_16S = const(4)
    XPOWERS_AXP2101_WDT_TIMEOUT_32S = const(5)
    XPOWERS_AXP2101_WDT_TIMEOUT_64S = const(6)
    XPOWERS_AXP2101_WDT_TIMEOUT_128S = const(7)

    """VBUS voltage limit values"""
    XPOWERS_AXP2101_VBUS_VOL_LIM_3V88 = const(0)
    XPOWERS_AXP2101_VBUS_VOL_LIM_3V96 = const(1)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V04 = const(2)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V12 = const(3)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V20 = const(4)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V28 = const(5)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V36 = const(6)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V44 = const(7)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V52 = const(8)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V60 = const(9)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V68 = const(10)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V76 = const(11)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V84 = const(12)
    XPOWERS_AXP2101_VBUS_VOL_LIM_4V92 = const(13)
    XPOWERS_AXP2101_VBUS_VOL_LIM_5V = const(14)
    XPOWERS_AXP2101_VBUS_VOL_LIM_5V08 = const(15)

    """VSYS power supply voltage limit values"""
    XPOWERS_AXP2101_VSYS_VOL_4V1 = const(0)
    XPOWERS_AXP2101_VSYS_VOL_4V2 = const(1)
    XPOWERS_AXP2101_VSYS_VOL_4V3 = const(2)
    XPOWERS_AXP2101_VSYS_VOL_4V4 = const(3)
    XPOWERS_AXP2101_VSYS_VOL_4V5 = const(4)
    XPOWERS_AXP2101_VSYS_VOL_4V6 = const(5)
    XPOWERS_AXP2101_VSYS_VOL_4V7 = const(6)
    XPOWERS_AXP2101_VSYS_VOL_4V8 = const(7)

    """Power on source values"""
    # POWERON low for on level when POWERON Mode as POWERON Source
    XPOWER_POWERON_SRC_POWERON_LOW = const(0)
    # IRQ PIN Pull-down as POWERON Source
    XPOWER_POWERON_SRC_IRQ_LOW = const(1)
    # Vbus Insert and Good as POWERON Source
    XPOWER_POWERON_SRC_VBUS_INSERT = const(2)
    # Vbus Insert and Good as POWERON Source
    XPOWER_POWERON_SRC_BAT_CHARGE = const(3)
    # Battery Insert and Good as POWERON Source
    XPOWER_POWERON_SRC_BAT_INSERT = const(4)
    # POWERON always high when EN Mode as POWERON Source
    XPOWER_POWERON_SRC_ENMODE = const(5)
    XPOWER_POWERON_SRC_UNKONW = const(6)  # Unkonw

    """Power off source values"""
    # POWERON Pull down for off level when POWERON Mode as POWEROFF Source
    XPOWER_POWEROFF_SRC_PWEKEY_PULLDOWN = const(0)
    # Software configuration as POWEROFF Source
    XPOWER_POWEROFF_SRC_SOFT_OFF = const(1)
    # POWERON always low when EN Mode as POWEROFF Source
    XPOWER_POWEROFF_SRC_PWEKEY_LOW = const(2)
    # Vsys Under Voltage as POWEROFF Source
    XPOWER_POWEROFF_SRC_UNDER_VSYS = const(3)
    # VBUS Over Voltage as POWEROFF Source
    XPOWER_POWEROFF_SRC_OVER_VBUS = const(4)
    # DCDC Under Voltage as POWEROFF Source
    XPOWER_POWEROFF_SRC_UNDER_VOL = const(5)
    # DCDC Over Voltage as POWEROFF Source
    XPOWER_POWEROFF_SRC_OVER_VOL = const(6)
    # Die Over Temperature as POWEROFF Source
    XPOWER_POWEROFF_SRC_OVER_TEMP = const(7)
    XPOWER_POWEROFF_SRC_UNKONW = const(8)  # Unkonw

    """Power ok signal delay values"""
    XPOWER_PWROK_DELAY_8MS = const(0)
    XPOWER_PWROK_DELAY_16MS = const(1)
    XPOWER_PWROK_DELAY_32MS = const(2)
    XPOWER_PWROK_DELAY_64MS = const(3)

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

    """PMU LED indicator function values"""
    XPOWERS_CHG_LED_OFF = const(0)
    XPOWERS_CHG_LED_BLINK_1HZ = const(1)
    XPOWERS_CHG_LED_BLINK_4HZ = const(2)
    XPOWERS_CHG_LED_ON = const(3)
    XPOWERS_CHG_LED_CTRL_CHG = const(4)

    """Charging voltage limit values"""
    XPOWERS_AXP2101_CHG_VOL_4V = const(1)
    XPOWERS_AXP2101_CHG_VOL_4V1 = const(2)
    XPOWERS_AXP2101_CHG_VOL_4V2 = const(3)
    XPOWERS_AXP2101_CHG_VOL_4V35 = const(4)
    XPOWERS_AXP2101_CHG_VOL_4V4 = const(5)

    _CHG_VOL = (
        XPOWERS_AXP2101_CHG_VOL_4V,
        XPOWERS_AXP2101_CHG_VOL_4V1,
        XPOWERS_AXP2101_CHG_VOL_4V2,
        XPOWERS_AXP2101_CHG_VOL_4V35,
        XPOWERS_AXP2101_CHG_VOL_4V4
    )

    """Charging current limit values"""
    XPOWERS_AXP2101_CHG_CUR_100MA = const(4)
    XPOWERS_AXP2101_CHG_CUR_125MA = const(5)
    XPOWERS_AXP2101_CHG_CUR_150MA = const(6)
    XPOWERS_AXP2101_CHG_CUR_175MA = const(7)
    XPOWERS_AXP2101_CHG_CUR_200MA = const(8)
    XPOWERS_AXP2101_CHG_CUR_300MA = const(9)
    XPOWERS_AXP2101_CHG_CUR_400MA = const(10)
    XPOWERS_AXP2101_CHG_CUR_500MA = const(11)
    XPOWERS_AXP2101_CHG_CUR_600MA = const(12)
    XPOWERS_AXP2101_CHG_CUR_700MA = const(13)
    XPOWERS_AXP2101_CHG_CUR_800MA = const(14)
    XPOWERS_AXP2101_CHG_CUR_900MA = const(15)
    XPOWERS_AXP2101_CHG_CUR_1000MA = const(16)

    _CHG_CUR = (
        XPOWERS_AXP2101_CHG_CUR_100MA,
        XPOWERS_AXP2101_CHG_CUR_125MA,
        XPOWERS_AXP2101_CHG_CUR_150MA,
        XPOWERS_AXP2101_CHG_CUR_175MA,
        XPOWERS_AXP2101_CHG_CUR_200MA,
        XPOWERS_AXP2101_CHG_CUR_300MA,
        XPOWERS_AXP2101_CHG_CUR_400MA,
        XPOWERS_AXP2101_CHG_CUR_500MA,
        XPOWERS_AXP2101_CHG_CUR_600MA,
        XPOWERS_AXP2101_CHG_CUR_700MA,
        XPOWERS_AXP2101_CHG_CUR_800MA,
        XPOWERS_AXP2101_CHG_CUR_900MA,
        XPOWERS_AXP2101_CHG_CUR_1000MA,
    )

    """VBUS current limit values"""
    XPOWERS_AXP2101_VBUS_CUR_LIM_100MA = const(0)
    XPOWERS_AXP2101_VBUS_CUR_LIM_500MA = const(1)
    XPOWERS_AXP2101_VBUS_CUR_LIM_900MA = const(2)
    XPOWERS_AXP2101_VBUS_CUR_LIM_1000MA = const(3)
    XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA = const(4)
    XPOWERS_AXP2101_VBUS_CUR_LIM_2000MA = const(5)

    _VBUS_LIMIT = (
        XPOWERS_AXP2101_VBUS_CUR_LIM_100MA,
        XPOWERS_AXP2101_VBUS_CUR_LIM_500MA,
        XPOWERS_AXP2101_VBUS_CUR_LIM_900MA,
        XPOWERS_AXP2101_VBUS_CUR_LIM_1000MA,
        XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA,
        XPOWERS_AXP2101_VBUS_CUR_LIM_2000MA,
    )

    """PMU interrupt control mask values"""

    #! IRQ1 REG 40H
    XPOWERS_AXP2101_BAT_NOR_UNDER_TEMP_IRQ = const(
        1 << 0)   # Battery Under Temperature in Work
    XPOWERS_AXP2101_BAT_NOR_OVER_TEMP_IRQ = const(
        1 << 1)   # Battery Over Temperature in Work mode
    # Battery Under Temperature in Charge mode IRQ(bcut_irq)
    XPOWERS_AXP2101_BAT_CHG_UNDER_TEMP_IRQ = const(1 << 2)
    # Battery Over Temperature in Charge mode IRQ(bcot_irq) enable
    XPOWERS_AXP2101_BAT_CHG_OVER_TEMP_IRQ = const(1 << 3)
    # Gauge New SOC IRQ(lowsoc_irq) enable ???
    XPOWERS_AXP2101_GAUGE_NEW_SOC_IRQ = const(1 << 4)
    # Gauge Watchdog Timeout IRQ(gwdt_irq) enable
    XPOWERS_AXP2101_WDT_TIMEOUT_IRQ = const(1 << 5)
    # SOC drop to Warning Level1 IRQ(socwl1_irq) enable
    XPOWERS_AXP2101_WARNING_LEVEL1_IRQ = const(1 << 6)
    # SOC drop to Warning Level2 IRQ(socwl2_irq) enable
    XPOWERS_AXP2101_WARNING_LEVEL2_IRQ = const(1 << 7)

    #! IRQ2 REG 41H
    # POWERON Positive Edge IRQ(ponpe_irq_en) enable
    XPOWERS_AXP2101_PKEY_POSITIVE_IRQ = const(1 << 8)
    # POWERON Negative Edge IRQ(ponne_irq_en) enable
    XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ = const(1 << 9)
    # POWERON Long PRESS IRQ(ponlp_irq) enable
    XPOWERS_AXP2101_PKEY_LONG_IRQ = const(1 << 10)
    # POWERON Short PRESS IRQ(ponsp_irq_en) enable
    XPOWERS_AXP2101_PKEY_SHORT_IRQ = const(1 << 11)
    # Battery Remove IRQ(bremove_irq) enable
    XPOWERS_AXP2101_BAT_REMOVE_IRQ = const(1 << 12)
    # Battery Insert IRQ(binsert_irq) enabl
    XPOWERS_AXP2101_BAT_INSERT_IRQ = const(1 << 13)
    XPOWERS_AXP2101_VBUS_REMOVE_IRQ = const(
        1 << 14)  # VBUS Remove IRQ(vremove_irq) enabl
    # VBUS Insert IRQ(vinsert_irq) enable
    XPOWERS_AXP2101_VBUS_INSERT_IRQ = const(1 << 15)

    #! IRQ3 REG 42H
    # Battery Over Voltage Protection IRQ(bovp_irq) enable
    XPOWERS_AXP2101_BAT_OVER_VOL_IRQ = const(1 << 16)
    # Charger Safety Timer1/2 expire IRQ(chgte_irq) enable
    XPOWERS_AXP2101_CHAGER_TIMER_IRQ = const(1 << 17)
    # DIE Over Temperature level1 IRQ(dotl1_irq) enable
    XPOWERS_AXP2101_DIE_OVER_TEMP_IRQ = const(1 << 18)
    XPOWERS_AXP2101_BAT_CHG_START_IRQ = const(
        1 << 19)  # Charger start IRQ(chgst_irq) enable
    # Battery charge done IRQ(chgdn_irq) enable
    XPOWERS_AXP2101_BAT_CHG_DONE_IRQ = const(1 << 20)
    # BATFET Over Current Protection IRQ(bocp_irq) enable
    XPOWERS_AXP2101_BATFET_OVER_CURR_IRQ = const(1 << 21)
    # LDO Over Current IRQ(ldooc_irq) enable
    XPOWERS_AXP2101_LDO_OVER_CURR_IRQ = const(1 << 22)
    # Watchdog Expire IRQ(wdexp_irq) enable
    XPOWERS_AXP2101_WDT_EXPIRE_IRQ = const(1 << 23)
    XPOWERS_AXP2101_ALL_IRQ = const(0xFFFFFFFF)

    def __init__(self, i2c_bus: I2C, addr: int = AXP2101_SLAVE_ADDRESS) -> None:
        super().__init__(i2c_bus, addr)
        print('AXP2101 __init__')
        self.statusRegister = [0] * _AXP2101_INTSTS_CNT
        self.intRegister = [0] * _AXP2101_INTSTS_CNT

        if self.getChipID() != XPOWERS_AXP2101_CHIP_ID:
            raise RuntimeError(
                "Failed to find %s - check your wiring!" % self.__class__.__name__
            )

    #  PMU status functions
    def isVbusGood(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS1, 5))

    def getBatfetState(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS1, 4))

    # getBatPresentState
    def isBatteryConnect(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS1, 3))

    def isBatInActiveModeState(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS1, 3))

    def getThermalRegulationStatus(self) -> bool:
        return super().getRegisterBit(_AXP2101_STATUS1, 2)

    def getCurrnetLimitStatus(self) -> bool:
        return super().getRegisterBit(_AXP2101_STATUS1, 1)

    def isCharging(self) -> bool:
        return (super().readRegister(_AXP2101_STATUS2)[0] >> 5) == 0x01

    def isDischarge(self) -> bool:
        return (super().readRegister(_AXP2101_STATUS2)[0] >> 5) == 0x02

    def isStandby(self) -> bool:
        return (super().readRegister(_AXP2101_STATUS2)[0] >> 5) == 0x00

    def isPowerOn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS2, 4))

    def isPowerOff(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS2, 4))

    def isVbusIn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_STATUS2, 3) == 0)

    def getChargerStatus(self) -> None:
        return super().readRegister(_AXP2101_STATUS2)[0] & 0x07

    # Data Buffer
    def writeDataBuffer(self, data: list,  size: int) -> None:
        if not 0 <= size <= _AXP2101_DATA_BUFFER_SIZE:
            raise ValueError("size must be a value within 0-3!")
        for i in range(0, size):
            super().writeRegister(_AXP2101_DATA_BUFFER1 + i, data[i])

    def readDataBuffer(self, size: int) -> list:
        if not 0 <= size <= _AXP2101_DATA_BUFFER_SIZE:
            raise ValueError("size must be a value within 0-3!")
        return list(super().readRegister(_AXP2101_DATA_BUFFER1, size))

    # PMU common configuration
    # @brief   Internal off-discharge enable for DCDC & LDO & SWITCH
    def enableInternalDischarge(self) -> None:
        super().setRegisterBit(_AXP2101_COMMON_CONFIG, 5)

    def disableInternalDischarge(self) -> None:
        super().clrRegisterBit(_AXP2101_COMMON_CONFIG, 5)

    # @brief   PWROK PIN pull low to Restart
    def enablePwrOkPinPullLow(self) -> None:
        super().setRegisterBit(_AXP2101_COMMON_CONFIG, 3)

    def disablePwrOkPinPullLow(self) -> None:
        super().clrRegisterBit(_AXP2101_COMMON_CONFIG, 3)

    def enablePwronShutPMIC(self) -> None:
        super().setRegisterBit(_AXP2101_COMMON_CONFIG, 2)

    def disablePwronShutPMIC(self) -> None:
        super().clrRegisterBit(_AXP2101_COMMON_CONFIG, 2)

    # @brief  Restart the SoC System, POWOFF/POWON and reset the related registers
    def reset(self) -> None:
        super().setRegisterBit(_AXP2101_COMMON_CONFIG, 1)

    # @brief  Set shutdown, calling shutdown will turn off all power channels,
    # only VRTC belongs to normal power supply
    def shutdown(self) -> None:
        super().setRegisterBit(_AXP2101_COMMON_CONFIG, 0)

    # @brief  BATFET control / REG 12H
    # @note   DIE Over Temperature Protection Level1 Configuration
    # @param   opt: 0:115 , 1:125 , 2:135
    def setBatfetDieOverTempLevel1(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError("param must be a value within 0-3!")
        val = super().readRegister(_AXP2101_BATFET_CTRL)[0]
        val &= 0xF9
        super().writeRegister(_AXP2101_BATFET_CTRL, val | (opt << 1))

    def getBatfetDieOverTempLevel1(self) -> int:
        return (super().readRegister(_AXP2101_BATFET_CTRL)[0] & 0x06)

    def enableBatfetDieOverTempDetect(self) -> None:
        super().setRegisterBit(_AXP2101_BATFET_CTRL, 0)

    def disableBatfetDieOverTempDetect(self) -> None:
        super().clrRegisterBit(_AXP2101_BATFET_CTRL, 0)

    # @param   opt: 0:115 , 1:125 , 2:135
    def setDieOverTempLevel1(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError("level must be a value within 0-3!")
        val = super().readRegister(_AXP2101_DIE_TEMP_CTRL)[0]
        val &= 0xF9
        super().writeRegister(_AXP2101_DIE_TEMP_CTRL, val | (opt << 1))

    def getDieOverTempLevel1(self) -> int:
        return (super().readRegister(_AXP2101_DIE_TEMP_CTRL)[0] & 0x06)

    def enableDieOverTempDetect(self) -> None:
        super().setRegisterBit(_AXP2101_DIE_TEMP_CTRL, 0)

    def disableDieOverTempDetect(self) -> None:
        super().clrRegisterBit(_AXP2101_DIE_TEMP_CTRL, 0)

    # Linear Charger Vsys voltage dpm
    def setLinearChargerVsysDpm(self, opt: int) -> None:
        # todo:
        val = super().readRegister(_AXP2101_MIN_SYS_VOL_CTRL)[0]
        val &= 0x8F
        super().writeRegister(_AXP2101_MIN_SYS_VOL_CTRL, val | (opt << 4))

    def getLinearChargerVsysDpm(self) -> int:
        val = super().readRegister(_AXP2101_MIN_SYS_VOL_CTRL)[0]
        val &= 0x70
        return (val & 0x70) >> 4

    # Set the minimum common working voltage of the PMU VBUS input,
    # below this value will turn off the PMU
    def setVbusVoltageLimit(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_INPUT_VOL_LIMIT_CTRL)[0]
        val &= 0xF0
        super().writeRegister(_AXP2101_INPUT_VOL_LIMIT_CTRL, val | (opt & 0x0F))

    def getVbusVoltageLimit(self) -> int:
        return (super().readRegister(_AXP2101_INPUT_VOL_LIMIT_CTRL)[0] & 0x0F)

    # @brief  Set VBUS Current Input Limit.
    # @param   opt: View the related chip type _axp2101_vbus_cur_limit_t enumeration parameters in "Params.hpp"
    def setVbusCurrentLimit(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_INPUT_CUR_LIMIT_CTRL)[0]
        val &= 0xF8
        super().writeRegister(_AXP2101_INPUT_CUR_LIMIT_CTRL, val | (opt & 0x07))

     # @brief  Get VBUS Current Input Limit.
     # @retval View the related chip type _axp2101_vbus_cur_limit_t enumeration parameters in "Params.hpp"
    def getVbusCurrentLimit(self) -> int:
        return (super().readRegister(_AXP2101_INPUT_CUR_LIMIT_CTRL)[0] & 0x07)

    # @brief  Reset the fuel gauge
    def resetGauge(self) -> None:
        super().setRegisterBit(_AXP2101_RESET_FUEL_GAUGE, 3)

    # @brief   reset the gauge besides reset
    def resetGaugeBesides(self) -> None:
        super().setRegisterBit(_AXP2101_RESET_FUEL_GAUGE, 2)

    # @brief Gauge Module
    def enableGauge(self) -> None:
        super().setRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 3)

    def disableGauge(self) -> None:
        super().clrRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 3)

    # @brief  Button Battery charge
    def enableButtonBatteryCharge(self) -> None:
        super().setRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 2)

    def disableButtonBatteryCharge(self) -> None:
        super().clrRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 2)

    def isEanbleButtonBatteryCharge(self) -> bool:
        return super().getRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 2)

    # Button battery charge termination voltage setting
    def setButtonBatteryChargeVoltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_BTN_VOL_STEPS):
            raise ValueError("Mistake ! Button battery charging step voltage is %u mV" %
                             _AXP2101_BTN_VOL_STEPS)
        if (millivolt < _AXP2101_BTN_VOL_MIN):
            raise ValueError("Mistake ! The minimum charge termination voltage of the coin cell battery is %u mV" %
                             _AXP2101_BTN_VOL_MIN)
        elif (millivolt > _AXP2101_BTN_VOL_MAX):
            raise ValueError("Mistake ! The minimum charge termination voltage of the coin cell battery is %u mV" %
                             _AXP2101_BTN_VOL_MAX)
        val = super().readRegister(_AXP2101_BTN_BAT_CHG_VOL_SET)[0]
        val &= 0xF8
        val |= (int)(millivolt - _AXP2101_BTN_VOL_MIN) / \
            _AXP2101_BTN_VOL_STEPS
        super().writeRegister(_AXP2101_BTN_BAT_CHG_VOL_SET, val)

    def getButtonBatteryVoltage(self) -> int:
        val = super().readRegister(_AXP2101_BTN_BAT_CHG_VOL_SET)[0]
        return (val & 0x07) * _AXP2101_BTN_VOL_STEPS + _AXP2101_BTN_VOL_MIN

    # @brief Cell Battery charge
    def enableCellbatteryCharge(self) -> None:
        super().setRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 1)

    def disableCellbatteryCharge(self) -> None:
        super().clrRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 1)

    # @brief  Watchdog Module
    def enableWatchdog(self) -> None:
        super().setRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 0)
        self.enableIRQ(self.XPOWERS_AXP2101_WDT_EXPIRE_IRQ)

    def disableWatchdog(self) -> None:
        self.disableIRQ(self.XPOWERS_AXP2101_WDT_EXPIRE_IRQ)
        super().clrRegisterBit(_AXP2101_CHARGE_GAUGE_WDT_CTRL, 0)

    # @brief Watchdog Config
    # @param   opt: 0: IRQ Only 1: IRQ and System reset  2: IRQ, System Reset and Pull down PWROK 1s  3: IRQ, System Reset, DCDC/LDO PWROFF & PWRON
    def setWatchdogConfig(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError(
                "Watchdog Config optrion must be a value within 0-3!")
        val = super().readRegister(_AXP2101_WDT_CTRL)[0]
        val &= 0xCF
        super().writeRegister(_AXP2101_WDT_CTRL, val | (opt << 4))

    def getWatchConfig(self) -> int:
        return (super().readRegister(_AXP2101_WDT_CTRL)[0] & 0x30) >> 4

    def clrWatchdog(self) -> None:
        super().setRegisterBit(_AXP2101_WDT_CTRL, 3)

    def setWatchdogTimeout(self, opt: int) -> None:
        if not 0 < opt <= 7:
            raise ValueError(
                "Watchdog timeout must be a value within 0-7!")
        val = super().readRegister(_AXP2101_WDT_CTRL)[0]
        val &= 0xF8
        super().writeRegister(_AXP2101_WDT_CTRL, val | opt)

    def getWatchdogTimerout(self) -> int:
        return super().readRegister(_AXP2101_WDT_CTRL)[0] & 0x07

    # @brief Low battery warning threshold 5-20%, 1% per step
    def setLowBatWarnThreshold(self, opt: int) -> None:
        # todo:
        val = super().readRegister(_AXP2101_LOW_BAT_WARN_SET)[0]
        val &= 0x0F
        super().writeRegister(_AXP2101_LOW_BAT_WARN_SET, val | (opt << 4))

    def getLowBatWarnThreshold(self) -> int:
        return (super().readRegister(_AXP2101_LOW_BAT_WARN_SET)[0] & 0xF0) >> 4

    # @brief Low battery shutdown threshold 0-15%, 1% per step
    def setLowBatShutdownThreshold(self, opt: int) -> None:
        # todo:
        val = super().readRegister(_AXP2101_LOW_BAT_WARN_SET)[0]
        val &= 0xF0
        super().writeRegister(_AXP2101_LOW_BAT_WARN_SET, val | opt)

    def getLowBatShutdownThreshold(self) -> int:
        return (super().readRegister(_AXP2101_LOW_BAT_WARN_SET)[0] & 0x0F)

    #!  PWRON statu  20
    # POWERON always high when EN Mode as POWERON Source
    def isPoweronAlwaysHighSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWRON_STATUS, 5))

    # Battery Insert and Good as POWERON Source
    def isBattInsertOnSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWRON_STATUS, 4))

    # Battery Voltage > 3.3V when Charged as Source
    def isBattNormalOnSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWRON_STATUS, 3))

    # Vbus Insert and Good as POWERON Source
    def isVbusInsertOnSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWRON_STATUS, 2))

    # IRQ PIN Pull-down as POWERON Source
    def isIrqLowOnSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWRON_STATUS, 1))

    # POWERON low for on level when POWERON Mode as POWERON Source
    def isPwronLowOnSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWRON_STATUS, 0))

    def getPowerOnSource(self) -> int:
        return super().readRegister(_AXP2101_PWRON_STATUS)[0]

    #!  PWROFF status  21
    # Die Over Temperature as POWEROFF Source
    def isOverTemperatureOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 7))

    # DCDC Over Voltage as POWEROFF Source
    def isDcOverVoltageOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 6))

    # DCDC Under Voltage as POWEROFF Source
    def isDcUnderVoltageOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 5))

    # VBUS Over Voltage as POWEROFF Source
    def isVbusOverVoltageOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 4))

    # Vsys Under Voltage as POWEROFF Source
    def isVsysUnderVoltageOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 3))

    # POWERON always low when EN Mode as POWEROFF Source
    def isPwronAlwaysLowOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 2))

    # Software configuration as POWEROFF Source
    def isSwConfigOffSource(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 1))

    # POWERON Pull down for off level when POWERON Mode as POWEROFF Source
    def isPwrSourcePullDown(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_PWROFF_STATUS, 0))

    def getPowerOffSource(self) -> int:
        return super().readRegister(_AXP2101_PWROFF_STATUS)[0]

    #!REG 22H
    def enableOverTemperatureLevel2PowerOff(self) -> None:
        super().setRegisterBit(_AXP2101_PWROFF_EN, 2)

    def disableOverTemperaturePowerOff(self) -> None:
        super().clrRegisterBit(_AXP2101_PWROFF_EN, 2)

    def enableLongPressShutdown(self) -> None:
        super().setRegisterBit(_AXP2101_PWROFF_EN, 1)

    def disableLongPressShutdown(self) -> None:
        super().clrRegisterBit(_AXP2101_PWROFF_EN, 1)

    def setLongPressRestart(self) -> None:
        super().setRegisterBit(_AXP2101_PWROFF_EN, 0)

    def setLongPressPowerOFF(self) -> None:
        super().clrRegisterBit(_AXP2101_PWROFF_EN, 0)

    #!REG 23H
    # DCDC 120%(130%) high voltage turn off PMIC function
    def enableDCHighVoltageTurnOff(self) -> None:
        super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 5)

    def disableDCHighVoltageTurnOff(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 5)

    # DCDC5 85% low voltage turn Off PMIC function
    def enableDC5LowVoltageTurnOff(self) -> None:
        super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 4)

    def disableDC5LowVoltageTurnOff(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 4)

    # DCDC4 85% low voltage turn Off PMIC function
    def enableDC4LowVoltageTurnOff(self) -> None:
        super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 3)

    def disableDC4LowVoltageTurnOff(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 3)

    # DCDC3 85% low voltage turn Off PMIC function
    def enableDC3LowVoltageTurnOff(self) -> None:
        super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 2)

    def disableDC3LowVoltageTurnOff(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 2)

    # DCDC2 85% low voltage turn Off PMIC function
    def enableDC2LowVoltageTurnOff(self) -> None:
        super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 1)

    def disableDC2LowVoltageTurnOff(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 1)

    # DCDC1 85% low voltage turn Off PMIC function
    def enableDC1LowVoltageTurnOff(self) -> None:
        super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 0)

    def disableDC1LowVoltageTurnOff(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 0)

    # Set the minimum system operating voltage inside the PMU,
    # below this value will shut down the PMU,Adjustment range 2600mV~3300mV
    def setSysPowerDownVoltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_VSYS_VOL_THRESHOLD_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_VSYS_VOL_THRESHOLD_STEPS)
        if (millivolt < _AXP2101_VSYS_VOL_THRESHOLD_MIN):
            raise ValueError("Mistake ! The minimum settable voltage of VSYS is %u mV" %
                             _AXP2101_VSYS_VOL_THRESHOLD_MIN)
        elif (millivolt > _AXP2101_VSYS_VOL_THRESHOLD_MAX):
            raise ValueError("Mistake ! The maximum settable voltage of VSYS is %u mV" %
                             _AXP2101_VSYS_VOL_THRESHOLD_MAX)

        val = super().readRegister(_AXP2101_VOFF_SET)[0]
        val &= 0xF8
        super().writeRegister(_AXP2101_VOFF_SET, val | (int)((millivolt -
                                                              _AXP2101_VSYS_VOL_THRESHOLD_MIN) / _AXP2101_VSYS_VOL_THRESHOLD_STEPS))

    def getSysPowerDownVoltage(self) -> int:
        val = super().readRegister(_AXP2101_VOFF_SET)[0]
        return (val & 0x07) * _AXP2101_VSYS_VOL_THRESHOLD_STEPS + _AXP2101_VSYS_VOL_THRESHOLD_MIN

    # PWROK setting and PWROFF sequence control 25.
    # Check the PWROK Pin enable after all dcdc/ldo output valid 128ms
    def enablePwrOk(self) -> None:
        super().setRegisterBit(_AXP2101_PWROK_SEQU_CTRL, 4)

    def disablePwrOk(self) -> None:
        super().clrRegisterBit(_AXP2101_PWROK_SEQU_CTRL, 4)

    # POWEROFF Delay 4ms after PWROK enable
    def eanblePowerOffDelay(self) -> None:
        super().setRegisterBit(_AXP2101_PWROK_SEQU_CTRL, 3)

    # POWEROFF Delay 4ms after PWROK disable
    def disablePowerOffDelay(self) -> None:
        super().clrRegisterBit(_AXP2101_PWROK_SEQU_CTRL, 3)

    # POWEROFF Sequence Control the reverse of the Startup
    def eanblePowerSequence(self) -> None:
        super().setRegisterBit(_AXP2101_PWROK_SEQU_CTRL, 2)

    # POWEROFF Sequence Control at the same time
    def disablePowerSequence(self) -> None:
        super().clrRegisterBit(_AXP2101_PWROK_SEQU_CTRL, 2)

    # Delay of PWROK after all power output good
    def setPwrOkDelay(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_PWROK_SEQU_CTRL)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_PWROK_SEQU_CTRL, val | opt)

    def getPwrOkDelay(self) -> int:
        return (super().readRegister(_AXP2101_PWROK_SEQU_CTRL)[0] & 0x03)

    #  Sleep and 26
    def wakeupControl(self, opt: int, enable: bool) -> None:
        val = super().readRegister(_AXP2101_SLEEP_WAKEUP_CTRL)[0]
        if enable:
            val |= opt
        else:
            val &= (~opt)
        super().writeRegister(_AXP2101_SLEEP_WAKEUP_CTRL, val | opt)

    def enableWakeup(self) -> None:
        super().setRegisterBit(_AXP2101_SLEEP_WAKEUP_CTRL, 1)

    def disableWakeup(self) -> None:
        super().clrRegisterBit(_AXP2101_SLEEP_WAKEUP_CTRL, 1)

    def enableSleep(self) -> None:
        super().setRegisterBit(_AXP2101_SLEEP_WAKEUP_CTRL, 0)

    def disableSleep(self) -> None:
        super().clrRegisterBit(_AXP2101_SLEEP_WAKEUP_CTRL, 0)

    #  RQLEVEL/OFFLEVEL/ONLEVEL setting 27
    # @brief  IRQLEVEL configur
    # @param   opt: 0:1s  1:1.5s  2:2s 3:2.5s
    def setIrqLevel(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError('IRQ level must be a value within 0-3!')
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 4))

    # @brief  OFFLEVEL configuration
    # @param   opt:  0:4s 1:6s 2:8s 3:10s
    def setOffLevel(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError('OFF level must be a value within 0-3!')
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        super().writeRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 2))

    # @brief  ONLEVEL configuration
    # @param   opt: 0:128ms 1:512ms 2:1s  3:2s
    def setOnLevel(self, opt: int) -> None:
        if not 0 <= opt <= 3:
            raise ValueError('ON level must be a value within 0-3!')
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        super().writeRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | opt)

    # Fast pwron setting 0  28
    # Fast Power On Start Sequence
    def setDc4FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | ((opt & 0x3) << 6))

    def setDc3FastStartSequence(self,  opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | ((opt & 0x3) << 4))

    def setDc2FastStartSequence(self,  opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | ((opt & 0x3) << 2))

    def setDc1FastStartSequence(self,  opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | (opt & 0x3))

    #  Fast pwron setting 1  29
    def setAldo3FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | ((opt & 0x3) << 6))

    def setAldo2FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | ((opt & 0x3) << 4))

    def setAldo1FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | ((opt & 0x3) << 2))

    def setDc5FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | (opt & 0x3))

    #  Fast pwron setting 2  2A
    def setCpuldoFastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | ((opt & 0x3) << 6))

    def setBldo2FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | ((opt & 0x3) << 4))

    def setBldo1FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | ((opt & 0x3) << 2))

    def setAldo4FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | (opt & 0x3))

    #  Fast pwron setting 3  2B
    def setDldo2FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_CTRL)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_CTRL, val | ((opt & 0x3) << 2))

    def setDldo1FastStartSequence(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_FAST_PWRON_CTRL)[0]
        super().writeRegister(_AXP2101_FAST_PWRON_CTRL, val | (opt & 0x3))

    # @brief   Setting Fast Power On Start Sequence
    def setFastPowerOnLevel(self, opt, seq_level) -> None:
        if opt == self.XPOWERSAXP2101_FAST_DCDC1:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | seq_level)
        elif opt == self.XPOWERSAXP2101_FAST_DCDC2:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | (seq_level << 2))
        elif opt == self.XPOWERSAXP2101_FAST_DCDC3:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | (seq_level << 4))
        elif opt == self.XPOWERSAXP2101_FAST_DCDC4:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val | (seq_level << 6))
        elif opt == self.XPOWERSAXP2101_FAST_DCDC5:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | seq_level)
        elif opt == self.XPOWERSAXP2101_FAST_ALDO1:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | (seq_level << 2))
        elif opt == self.XPOWERSAXP2101_FAST_ALDO2:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | (seq_level << 4))
        elif opt == self.XPOWERSAXP2101_FAST_ALDO3:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val | (seq_level << 6))
        elif opt == self.XPOWERSAXP2101_FAST_ALDO4:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | seq_level)
        elif opt == self.XPOWERSAXP2101_FAST_BLDO1:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | (seq_level << 2))
        elif opt == self.XPOWERSAXP2101_FAST_BLDO2:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | (seq_level << 4))
        elif opt == self.XPOWERSAXP2101_FAST_CPUSLDO:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val | (seq_level << 6))
        elif opt == self.XPOWERSAXP2101_FAST_DLDO1:
            val = super().readRegister(_AXP2101_FAST_PWRON_CTRL)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_CTRL, val | seq_level)
        elif opt == self.XPOWERSAXP2101_FAST_DLDO2:
            val = super().readRegister(_AXP2101_FAST_PWRON_CTRL)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_CTRL, val | (seq_level << 2))

    def disableFastPowerOn(self, opt: int) -> None:
        if opt == self.XPOWERSAXP2101_FAST_DCDC1:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val & 0xFC)
        elif opt == self.XPOWERSAXP2101_FAST_DCDC2:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val & 0xF3)
        elif opt == self.XPOWERSAXP2101_FAST_DCDC3:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val & 0xCF)
        elif opt == self.XPOWERSAXP2101_FAST_DCDC4:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET0)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET0, val & 0x3F)
        elif opt == self.XPOWERSAXP2101_FAST_DCDC5:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val & 0xFC)
        elif opt == self.XPOWERSAXP2101_FAST_ALDO1:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val & 0xF3)
        elif opt == self.XPOWERSAXP2101_FAST_ALDO2:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val & 0xCF)
        elif opt == self.XPOWERSAXP2101_FAST_ALDO3:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET1)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET1, val & 0x3F)
        elif opt == self.XPOWERSAXP2101_FAST_ALDO4:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val & 0xFC)
        elif opt == self.XPOWERSAXP2101_FAST_BLDO1:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val & 0xF3)
        elif opt == self.XPOWERSAXP2101_FAST_BLDO2:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val & 0xCF)
        elif opt == self.XPOWERSAXP2101_FAST_CPUSLDO:
            val = super().readRegister(_AXP2101_FAST_PWRON_SET2)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_SET2, val & 0x3F)
        elif opt == self.XPOWERSAXP2101_FAST_DLDO1:
            val = super().readRegister(_AXP2101_FAST_PWRON_CTRL)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_CTRL, val & 0xFC)
        elif opt == self.XPOWERSAXP2101_FAST_DLDO2:
            val = super().readRegister(_AXP2101_FAST_PWRON_CTRL)[0]
            super().writeRegister(_AXP2101_FAST_PWRON_CTRL, val & 0xF3)

    def enableFastPowerOnCtrl(self) -> None:
        super().setRegisterBit(_AXP2101_FAST_PWRON_CTRL, 7)

    def disableFastPowerOnCtrl(self) -> None:
        super().clrRegisterBit(_AXP2101_FAST_PWRON_CTRL, 7)

    def enableFastWakeup(self) -> None:
        super().setRegisterBit(_AXP2101_FAST_PWRON_CTRL, 6)

    def disableFastWakeup(self) -> None:
        super().clrRegisterBit(_AXP2101_FAST_PWRON_CTRL, 6)

    # DCDC 120%(130%) high voltage turn off PMIC function
    def setDCHighVoltagePowerDowm(self, en: bool) -> None:
        if en:
            super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 5)
        else:
            super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 5)

    def getDCHighVoltagePowerDowmEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 5))

    # DCDCS force PWM control
    def setDcUVPDebounceTime(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_DC_FORCE_PWM_CTRL)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_DC_FORCE_PWM_CTRL, val | opt)

    def settDC1WorkModeToPwm(self, enable: bool) -> None:
        if enable:
            super().setRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 2)
        else:
            super().clrRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 2)

    def settDC2WorkModeToPwm(self, enable: bool) -> None:
        if enable:
            super().setRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 3)
        else:
            super().clrRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 3)

    def settDC3WorkModeToPwm(self, enable: bool) -> None:
        if enable:
            super().setRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 4)
        else:
            super().clrRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 4)

    def settDC4WorkModeToPwm(self, enable: bool) -> None:
        if enable:
            super().setRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 5)
        else:
            super().clrRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 5)

    # 1 = 100khz 0=50khz
    def setDCFreqSpreadRange(self, opt: int) -> None:
        if not 0 <= opt <= 1:
            raise ValueError(
                'set DCDC freq spread range must be a value within 0-1!')
        if opt:
            super().setRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 6)
        else:
            super().clrRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 6)

    def setDCFreqSpreadRangeEn(self, en: bool) -> None:
        if en:
            super().setRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 7)
        else:
            super().clrRegisterBit(_AXP2101_DC_FORCE_PWM_CTRL, 7)

    # Power control DCDC1 functions
    def isEnableDC1(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 0))

    def enableDC1(self) -> None:
        super().setRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 0)

    def disableDC1(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 0)

    def setDC1Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_DCDC1_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_DCDC1_VOL_STEPS)
        if (millivolt < _AXP2101_DCDC1_VOL_MIN):
            raise ValueError("Mistake ! DC1 minimum voltage is %u mV" %
                             _AXP2101_DCDC1_VOL_MIN)
        elif (millivolt > _AXP2101_DCDC1_VOL_MAX):
            raise ValueError("Mistake ! DC1 maximum voltage is %u mV" %
                             _AXP2101_DCDC1_VOL_MAX)
        super().writeRegister(_AXP2101_DC_VOL0_CTRL, (int)((millivolt -
                                                            _AXP2101_DCDC1_VOL_MIN) / _AXP2101_DCDC1_VOL_STEPS))

    def getDC1Voltage(self) -> int:
        return (super().readRegister(_AXP2101_DC_VOL0_CTRL)[0] & 0x1F) * _AXP2101_DCDC1_VOL_STEPS + _AXP2101_DCDC1_VOL_MIN

    # DCDC1 85% low voltage turn off PMIC function
    def setDC1LowVoltagePowerDowm(self, en):
        if en:
            super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 0)
        else:
            super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 0)

    def getDC1LowVoltagePowerDowmEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 0))

    # Power control DCDC2 functions
    def isEnableDC2(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 1))

    def enableDC2(self) -> None:
        super().setRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 1)

    def disableDC2(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 1)

    def setDC2Voltage(self, millivolt: int) -> None:
        val = super().readRegister(_AXP2101_DC_VOL1_CTRL)[0]
        val &= 0x80
        if (millivolt >= _AXP2101_DCDC2_VOL1_MIN and millivolt <= _AXP2101_DCDC2_VOL1_MAX):
            if (millivolt % _AXP2101_DCDC2_VOL_STEPS1):
                raise ValueError("Mistake !  The steps is must %umV" %
                                 _AXP2101_DCDC2_VOL_STEPS1)
            super().writeRegister(_AXP2101_DC_VOL1_CTRL, val | (int)((millivolt -
                                                                      _AXP2101_DCDC2_VOL1_MIN) / _AXP2101_DCDC2_VOL_STEPS1))
        elif (millivolt >= _AXP2101_DCDC2_VOL2_MIN and millivolt <= _AXP2101_DCDC2_VOL2_MAX):
            if (millivolt % _AXP2101_DCDC2_VOL_STEPS2):
                raise ValueError("Mistake !  The steps is must %umV" %
                                 _AXP2101_DCDC2_VOL_STEPS2)
            val |= (int)((((millivolt - _AXP2101_DCDC2_VOL2_MIN) /
                          _AXP2101_DCDC2_VOL_STEPS2) + _AXP2101_DCDC2_VOL_STEPS2_BASE))
            super().writeRegister(_AXP2101_DC_VOL1_CTRL, val)

    def getDC2Voltage(self) -> int:
        val = super().readRegister(_AXP2101_DC_VOL1_CTRL)[0]
        val &= 0x7F
        if (val < _AXP2101_DCDC2_VOL_STEPS2_BASE):
            return (val * _AXP2101_DCDC2_VOL_STEPS1) + _AXP2101_DCDC2_VOL1_MIN
        return (val * _AXP2101_DCDC2_VOL_STEPS2) - 200

    def getDC2WorkMode(self) -> int:
        return super().getRegisterBit(_AXP2101_DCDC2_VOL_STEPS2, 7)

    def setDC2LowVoltagePowerDowm(self, en: bool) -> None:
        if en:
            super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 1)
        else:
            super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 1)

    def getDC2LowVoltagePowerDowmEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 1))

    # Power control DCDC3 functions
    def isEnableDC3(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 2))

    def enableDC3(self) -> None:
        super().setRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 2)

    def disableDC3(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 2)

    # 0.5~1.2V,10mV/step,71steps
    # 1.22~1.54V,20mV/step,17steps
    # 1.6~3.4V,100mV/step,19steps
    def setDC3Voltage(self, millivolt: int) -> None:
        val = super().readRegister(_AXP2101_DC_VOL2_CTRL)[0]
        val &= 0x80
        if (millivolt >= _AXP2101_DCDC3_VOL1_MIN and millivolt <= _AXP2101_DCDC3_VOL1_MAX):
            if (millivolt % _AXP2101_DCDC3_VOL_STEPS1):
                raise ValueError("Mistake ! The steps is must %umV" %
                                 _AXP2101_DCDC3_VOL_STEPS1)
            super().writeRegister(_AXP2101_DC_VOL2_CTRL, val | (int)((millivolt -
                                                                      _AXP2101_DCDC3_VOL_MIN) / _AXP2101_DCDC3_VOL_STEPS1))
        elif (millivolt >= _AXP2101_DCDC3_VOL2_MIN and millivolt <= _AXP2101_DCDC3_VOL2_MAX):
            if (millivolt % _AXP2101_DCDC3_VOL_STEPS2):
                raise ValueError("Mistake ! The steps is must %umV" %
                                 _AXP2101_DCDC3_VOL_STEPS2)
            val |= (int)(((millivolt - _AXP2101_DCDC3_VOL2_MIN) /
                          _AXP2101_DCDC3_VOL_STEPS2) + _AXP2101_DCDC3_VOL_STEPS2_BASE)
            super().writeRegister(_AXP2101_DC_VOL2_CTRL, val)
        elif (millivolt >= _AXP2101_DCDC3_VOL3_MIN and millivolt <= _AXP2101_DCDC3_VOL3_MAX):
            if (millivolt % _AXP2101_DCDC3_VOL_STEPS3):
                raise ValueError("Mistake ! The steps is must %umV" %
                                 _AXP2101_DCDC3_VOL_STEPS3)
            val |= (int)(((millivolt - _AXP2101_DCDC3_VOL3_MIN) /
                          _AXP2101_DCDC3_VOL_STEPS3) + _AXP2101_DCDC3_VOL_STEPS3_BASE)
            super().writeRegister(_AXP2101_DC_VOL2_CTRL, val)

    def getDC3Voltage(self) -> int:
        val = super().readRegister(_AXP2101_DC_VOL2_CTRL)[0] & 0x7F
        if (val < _AXP2101_DCDC3_VOL_STEPS2_BASE):
            return (val * _AXP2101_DCDC3_VOL_STEPS1) + _AXP2101_DCDC3_VOL_MIN
        elif (val >= _AXP2101_DCDC3_VOL_STEPS2_BASE and val < _AXP2101_DCDC3_VOL_STEPS3_BASE):
            return (val * _AXP2101_DCDC3_VOL_STEPS2) - 200
        return (val * _AXP2101_DCDC3_VOL_STEPS3) - 7200

    def getDC3WorkMode(self) -> None:
        return super().getRegisterBit(_AXP2101_DC_VOL2_CTRL, 7)

    # DCDC3 85% low voltage turn off PMIC function
    def setDC3LowVoltagePowerDowm(self, en: bool) -> None:
        if en:
            super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 2)
        else:
            super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 2)

    def getDC3LowVoltagePowerDowmEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 2))

    # Power control DCDC4 functions
    # 0.5~1.2V,10mV/step,71steps
    # 1.22~1.84V,20mV/step,32steps
    def isEnableDC4(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 3))

    def enableDC4(self) -> None:
        super().setRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 3)

    def disableDC4(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 3)

    def setDC4Voltage(self, millivolt: int) -> None:
        val = super().readRegister(_AXP2101_DC_VOL3_CTRL)[0]
        val &= 0x80
        if (millivolt >= _AXP2101_DCDC4_VOL1_MIN and millivolt <= _AXP2101_DCDC4_VOL1_MAX):
            if (millivolt % _AXP2101_DCDC4_VOL_STEPS1):
                raise ValueError("Mistake ! The steps is must %umV" %
                                 _AXP2101_DCDC4_VOL_STEPS1)
            super().writeRegister(_AXP2101_DC_VOL3_CTRL, val | (int)((millivolt -
                                                                      _AXP2101_DCDC4_VOL1_MIN) / _AXP2101_DCDC4_VOL_STEPS1))
        elif (millivolt >= _AXP2101_DCDC4_VOL2_MIN and millivolt <= _AXP2101_DCDC4_VOL2_MAX):
            if (millivolt % _AXP2101_DCDC4_VOL_STEPS2):
                raise ValueError("Mistake ! The steps is must %umV" %
                                 _AXP2101_DCDC4_VOL_STEPS2)
            val |= (int)(((millivolt - _AXP2101_DCDC4_VOL2_MIN) /
                          _AXP2101_DCDC4_VOL_STEPS2) + _AXP2101_DCDC4_VOL_STEPS2_BASE)
            super().writeRegister(_AXP2101_DC_VOL3_CTRL, val)

    def getDC4Voltage(self) -> int:
        val = super().readRegister(_AXP2101_DC_VOL3_CTRL)[0]
        val &= 0x7F
        if (val < _AXP2101_DCDC4_VOL_STEPS2_BASE):
            return (val * _AXP2101_DCDC4_VOL_STEPS1) + _AXP2101_DCDC4_VOL1_MIN
        return (val * _AXP2101_DCDC4_VOL_STEPS2) - 200

    # DCDC4 85% low voltage turn off PMIC function
    def setDC4LowVoltagePowerDowm(self, en: bool) -> None:
        if en:
            super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 3)
        else:
            super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 3)

    def getDC4LowVoltagePowerDowmEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 3))

    # Power control DCDC5 functions,Output to gpio pin
    def isEnableDC5(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 4))

    def enableDC5(self) -> None:
        super().setRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 4)

    def disableDC5(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_ONOFF_DVM_CTRL, 4)

    def setDC5Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_DCDC5_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_DCDC5_VOL_STEPS)
        if (millivolt != _AXP2101_DCDC5_VOL_1200MV and millivolt < _AXP2101_DCDC5_VOL_MIN):
            raise ValueError("Mistake ! DC5 minimum voltage is %umV ,%umV" %
                             (_AXP2101_DCDC5_VOL_1200MV, _AXP2101_DCDC5_VOL_MIN))
        elif (millivolt > _AXP2101_DCDC5_VOL_MAX):
            raise ValueError("Mistake ! DC5 maximum voltage is %umV" %
                             _AXP2101_DCDC5_VOL_MAX)

        val = super().readRegister(_AXP2101_DC_VOL4_CTRL)[0]
        val &= 0xE0
        if (millivolt == _AXP2101_DCDC5_VOL_1200MV):
            super().writeRegister(_AXP2101_DC_VOL4_CTRL,
                                  val | _AXP2101_DCDC5_VOL_VAL)
        val |= (int)((millivolt - _AXP2101_DCDC5_VOL_MIN) /
                     _AXP2101_DCDC5_VOL_STEPS)
        super().writeRegister(_AXP2101_DC_VOL4_CTRL, val)

    def getDC5Voltage(self) -> int:
        val = super().readRegister(_AXP2101_DC_VOL4_CTRL)[0]
        val &= 0x1F
        if (val == _AXP2101_DCDC5_VOL_VAL):
            return _AXP2101_DCDC5_VOL_1200MV
        return (val * _AXP2101_DCDC5_VOL_STEPS) + _AXP2101_DCDC5_VOL_MIN

    def isDC5FreqCompensationEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_VOL4_CTRL, 5))

    def enableDC5FreqCompensation(self) -> None:
        super().setRegisterBit(_AXP2101_DC_VOL4_CTRL, 5)

    def disableFreqCompensation(self) -> None:
        super().clrRegisterBit(_AXP2101_DC_VOL4_CTRL, 5)

    # DCDC4 85% low voltage turn off PMIC function
    def setDC5LowVoltagePowerDowm(self, en: bool) -> None:
        if en:
            super().setRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 4)
        else:
            super().clrRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 4)

    def getDC5LowVoltagePowerDowmEn(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_DC_OVP_UVP_CTRL, 4))

    # Power control ALDO1 functions
    def isEnableALDO1(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 0))

    def enableALDO1(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 0)

    def disableALDO1(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 0)

    def setALDO1Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_ALDO1_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_ALDO1_VOL_STEPS)
        if (millivolt < _AXP2101_ALDO1_VOL_MIN):
            raise ValueError("Mistake ! ALDO1 minimum output voltage is  %umV" %
                             _AXP2101_ALDO1_VOL_MIN)
        elif (millivolt > _AXP2101_ALDO1_VOL_MAX):
            raise ValueError("Mistake ! ALDO1 maximum output voltage is  %umV" %
                             _AXP2101_ALDO1_VOL_MAX)

        val = super().readRegister(_AXP2101_LDO_VOL0_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_ALDO1_VOL_MIN) /
                     _AXP2101_ALDO1_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL0_CTRL, val)

    def getALDO1Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL0_CTRL)[0] & 0x1F
        return val * _AXP2101_ALDO1_VOL_STEPS + _AXP2101_ALDO1_VOL_MIN

    # Power control ALDO2 functions
    def isEnableALDO2(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 1))

    def enableALDO2(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 1)

    def disableALDO2(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 1)

    def setALDO2Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_ALDO2_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_ALDO2_VOL_STEPS)
        if (millivolt < _AXP2101_ALDO2_VOL_MIN):
            raise ValueError("Mistake ! ALDO2 minimum output voltage is  %umV" %
                             _AXP2101_ALDO2_VOL_MIN)
        elif (millivolt > _AXP2101_ALDO2_VOL_MAX):
            raise ValueError("Mistake ! ALDO2 maximum output voltage is  %umV" %
                             _AXP2101_ALDO2_VOL_MAX)

        val = super().readRegister(_AXP2101_LDO_VOL1_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_ALDO2_VOL_MIN) /
                     _AXP2101_ALDO2_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL1_CTRL, val)

    def getALDO2Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL1_CTRL)[0] & 0x1F
        return val * _AXP2101_ALDO2_VOL_STEPS + _AXP2101_ALDO2_VOL_MIN

    # Power control ALDO3 functions
    def isEnableALDO3(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 2))

    def enableALDO3(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 2)

    def disableALDO3(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 2)

    def setALDO3Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_ALDO3_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_ALDO3_VOL_STEPS)
        if (millivolt < _AXP2101_ALDO3_VOL_MIN):
            raise ValueError("Mistake ! ALDO3 minimum output voltage is  %umV" %
                             _AXP2101_ALDO3_VOL_MIN)
        elif (millivolt > _AXP2101_ALDO3_VOL_MAX):
            raise ValueError("Mistake ! ALDO3 maximum output voltage is  %umV" %
                             _AXP2101_ALDO3_VOL_MAX)

        val = super().readRegister(_AXP2101_LDO_VOL2_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_ALDO3_VOL_MIN) /
                     _AXP2101_ALDO3_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL2_CTRL, val)

    def getALDO3Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL2_CTRL)[0] & 0x1F
        return val * _AXP2101_ALDO3_VOL_STEPS + _AXP2101_ALDO3_VOL_MIN

    # Power control ALDO4 functions
    def isEnableALDO4(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 3))

    def enableALDO4(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 3)

    def disableALDO4(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 3)

    def setALDO4Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_ALDO4_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_ALDO4_VOL_STEPS)
        if (millivolt < _AXP2101_ALDO4_VOL_MIN):
            raise ValueError("Mistake ! ALDO4 minimum output voltage is  %umV" %
                             _AXP2101_ALDO4_VOL_MIN)
        elif (millivolt > _AXP2101_ALDO4_VOL_MAX):
            raise ValueError("Mistake ! ALDO4 maximum output voltage is  %umV" %
                             _AXP2101_ALDO4_VOL_MAX)
        val = super().readRegister(_AXP2101_LDO_VOL3_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_ALDO4_VOL_MIN) /
                     _AXP2101_ALDO4_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL3_CTRL, val)

    def getALDO4Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL3_CTRL)[0] & 0x1F
        return val * _AXP2101_ALDO4_VOL_STEPS + _AXP2101_ALDO4_VOL_MIN

    # Power control BLDO1 functions
    def isEnableBLDO1(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 4))

    def enableBLDO1(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 4)

    def disableBLDO1(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 4)

    def setBLDO1Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_BLDO1_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_BLDO1_VOL_STEPS)
        if (millivolt < _AXP2101_BLDO1_VOL_MIN):
            raise ValueError("Mistake ! BLDO1 minimum output voltage is  %umV" %
                             _AXP2101_BLDO1_VOL_MIN)
        elif (millivolt > _AXP2101_BLDO1_VOL_MAX):
            raise ValueError("Mistake ! BLDO1 maximum output voltage is  %umV" %
                             _AXP2101_BLDO1_VOL_MAX)
        val = super().readRegister(_AXP2101_LDO_VOL4_CTRL)[0]
        val &= 0xE0
        val |= (int)((millivolt - _AXP2101_BLDO1_VOL_MIN) /
                     _AXP2101_BLDO1_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL4_CTRL, val)

    def getBLDO1Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL4_CTRL)[0]
        val &= 0x1F
        return val * _AXP2101_BLDO1_VOL_STEPS + _AXP2101_BLDO1_VOL_MIN

    # Power control BLDO2 functions
    def isEnableBLDO2(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 5))

    def enableBLDO2(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 5)

    def disableBLDO2(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 5)

    def setBLDO2Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_BLDO2_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_BLDO2_VOL_STEPS)
        if (millivolt < _AXP2101_BLDO2_VOL_MIN):
            raise ValueError("Mistake ! BLDO2 minimum output voltage is  %umV" %
                             _AXP2101_BLDO2_VOL_MIN)
        elif (millivolt > _AXP2101_BLDO2_VOL_MAX):
            raise ValueError("Mistake ! BLDO2 maximum output voltage is  %umV" %
                             _AXP2101_BLDO2_VOL_MAX)
        val = super().readRegister(_AXP2101_LDO_VOL5_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_BLDO2_VOL_MIN) /
                     _AXP2101_BLDO2_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL5_CTRL, val)

    def getBLDO2Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL5_CTRL)[0]
        val &= 0x1F
        return val * _AXP2101_BLDO2_VOL_STEPS + _AXP2101_BLDO2_VOL_MIN

    # Power control CPUSLDO functions
    def isEnableCPUSLDO(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 6))

    def enableCPUSLDO(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 6)

    def disableCPUSLDO(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 6)

    def setCPUSLDOVoltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_CPUSLDO_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_CPUSLDO_VOL_STEPS)
        if (millivolt < _AXP2101_CPUSLDO_VOL_MIN):
            raise ValueError("Mistake ! CPULDO minimum output voltage is  %umV" %
                             _AXP2101_CPUSLDO_VOL_MIN)
        elif (millivolt > _AXP2101_CPUSLDO_VOL_MAX):
            raise ValueError("Mistake ! CPULDO maximum output voltage is  %umV" %
                             _AXP2101_CPUSLDO_VOL_MAX)
        val = super().readRegister(_AXP2101_LDO_VOL6_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_CPUSLDO_VOL_MIN) /
                     _AXP2101_CPUSLDO_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL6_CTRL, val)

    def getCPUSLDOVoltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL6_CTRL)[0]
        val &= 0x1F
        return val * _AXP2101_CPUSLDO_VOL_STEPS + _AXP2101_CPUSLDO_VOL_MIN

    # Power control DLDO1 functions
    def isEnableDLDO1(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 7))

    def enableDLDO1(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 7)

    def disableDLDO1(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL0, 7)

    def setDLDO1Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_DLDO1_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_DLDO1_VOL_STEPS)
        if (millivolt < _AXP2101_DLDO1_VOL_MIN):
            raise ValueError("Mistake ! DLDO1 minimum output voltage is  %umV" %
                             _AXP2101_DLDO1_VOL_MIN)
        elif (millivolt > _AXP2101_DLDO1_VOL_MAX):
            raise ValueError("Mistake ! DLDO1 maximum output voltage is  %umV" %
                             _AXP2101_DLDO1_VOL_MAX)
        val = super().readRegister(_AXP2101_LDO_VOL7_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_DLDO1_VOL_MIN) /
                     _AXP2101_DLDO1_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL7_CTRL, val)

    def getDLDO1Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL7_CTRL)[0]
        val &= 0x1F
        return val * _AXP2101_DLDO1_VOL_STEPS + _AXP2101_DLDO1_VOL_MIN

    # Power control DLDO2 functions
    def isEnableDLDO2(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_LDO_ONOFF_CTRL1, 0))

    def enableDLDO2(self) -> None:
        super().setRegisterBit(_AXP2101_LDO_ONOFF_CTRL1, 0)

    def disableDLDO2(self) -> None:
        super().clrRegisterBit(_AXP2101_LDO_ONOFF_CTRL1, 0)

    def setDLDO2Voltage(self, millivolt: int) -> None:
        if (millivolt % _AXP2101_DLDO2_VOL_STEPS):
            raise ValueError("Mistake ! The steps is must %u mV" %
                             _AXP2101_DLDO2_VOL_STEPS)
        if (millivolt < _AXP2101_DLDO2_VOL_MIN):
            raise ValueError("Mistake ! DLDO2 minimum output voltage is  %umV" %
                             _AXP2101_DLDO2_VOL_MIN)
        elif (millivolt > _AXP2101_DLDO2_VOL_MAX):
            raise ValueError("Mistake ! DLDO2 maximum output voltage is  %umV" %
                             _AXP2101_DLDO2_VOL_MAX)
        val = super().readRegister(_AXP2101_LDO_VOL8_CTRL)[0] & 0xE0
        val |= (int)((millivolt - _AXP2101_DLDO2_VOL_MIN) /
                     _AXP2101_DLDO2_VOL_STEPS)
        super().writeRegister(_AXP2101_LDO_VOL8_CTRL, val)

    def getDLDO2Voltage(self) -> int:
        val = super().readRegister(_AXP2101_LDO_VOL8_CTRL)[0]
        val &= 0x1F
        return val * _AXP2101_DLDO2_VOL_STEPS + _AXP2101_DLDO2_VOL_MIN

    #  Power ON OFF IRQ TIMMING Control method
    def setIrqLevelTime(self, opt: int):
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        val &= 0xCF
        super().writeRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 4))

    def getIrqLevelTime(self) -> int:
        return ((super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0] & 0x30) >> 4)

    # @brief Set the PEKEY power-on long press time.
    # @param  opt: See _press_on_time_t enum for details.
    def setPowerKeyPressOnTime(self, opt: int):
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | opt)

    # @brief Get the PEKEY power-on long press time.
    # @retval See _press_on_time_t enum for details.
    def getPowerKeyPressOnTime(self) -> int:
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        return (val & 0x03)

    # @brief Set the PEKEY power-off long press time.
    # @param  opt: See _press_off_time_t enum for details.
    # @retval
    def setPowerKeyPressOffTime(self, opt: int):
        val = super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0]
        val &= 0xF3
        super().writeRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL, val | (opt << 2))

    # @brief Get the PEKEY power-off long press time.
    # @retval See _press_off_time_t enum for details.
    def getPowerKeyPressOffTime(self) -> int:
        return ((super().readRegister(_AXP2101_IRQ_OFF_ON_LEVEL_CTRL)[0] & 0x0C) >> 2)

    #  ADC Control method
    def enableGeneralAdcChannel(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 5)

    def disableGeneralAdcChannel(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 5)

    def enableTemperatureMeasure(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 4)

    def disableTemperatureMeasure(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 4)

    def getTemperature(self) -> int:
        #!FIXME
        return super().readRegisterH6L8(_AXP2101_ADC_DATA_RELUST8, _AXP2101_ADC_DATA_RELUST9)

    def enableSystemVoltageMeasure(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 3)

    def disableSystemVoltageMeasure(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 3)

    def getSystemVoltage(self) -> int:
        return super().readRegisterH6L8(_AXP2101_ADC_DATA_RELUST6, _AXP2101_ADC_DATA_RELUST7)

    def enableVbusVoltageMeasure(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 2)

    def disableVbusVoltageMeasure(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 2)

    def getVbusVoltage(self) -> int:
        return super().readRegisterH6L8(_AXP2101_ADC_DATA_RELUST4, _AXP2101_ADC_DATA_RELUST5)

    def enableTSPinMeasure(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 1)

    def disableTSPinMeasure(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 1)

    def enableTSPinLowFreqSample(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 7)

    def disableTSPinLowFreqSample(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_DATA_RELUST2, 7)

    def getTsTemperature(self) -> int:
        return super().readRegisterH6L8(_AXP2101_ADC_DATA_RELUST2, _AXP2101_ADC_DATA_RELUST3)

    def enableBattVoltageMeasure(self) -> None:
        super().setRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 0)

    def disableBattVoltageMeasure(self) -> None:
        super().clrRegisterBit(_AXP2101_ADC_CHANNEL_CTRL, 0)

    def enableBattDetection(self) -> None:
        super().setRegisterBit(_AXP2101_BAT_DET_CTRL, 0)

    def disableBattDetection(self) -> None:
        super().clrRegisterBit(_AXP2101_BAT_DET_CTRL, 0)

    def getBattVoltage(self) -> int:
        if not self.isBatteryConnect():
            return 0
        return super().readRegisterH5L8(_AXP2101_ADC_DATA_RELUST0, _AXP2101_ADC_DATA_RELUST1)

    def getBatteryPercent(self) -> int:
        if not self.isBatteryConnect():
            return -1
        return super().readRegister(_AXP2101_BAT_PERCENT_DATA)[0]

    # CHG LED setting and control
    # @brief Set charging led mode.
    def setChargingLedMode(self, mode: int) -> None:
        range = [self.XPOWERS_CHG_LED_OFF, self.XPOWERS_CHG_LED_BLINK_1HZ,
                 self.XPOWERS_CHG_LED_BLINK_4HZ, self.XPOWERS_CHG_LED_ON]
        if mode in range:
            val = super().readRegister(_AXP2101_CHGLED_SET_CTRL)[0]
            val &= 0xC8
            val |= 0x05  # use manual ctrl
            val |= (mode << 4)
            super().writeRegister(_AXP2101_CHGLED_SET_CTRL, val)
        else:
            val = super().readRegister(_AXP2101_CHGLED_SET_CTRL)[0]
            val &= 0xF9
            super().writeRegister(_AXP2101_CHGLED_SET_CTRL, val | 0x01)  # use type A mode

    def getChargingLedMode(self) -> int:
        val = super().readRegister(_AXP2101_CHGLED_SET_CTRL)[0]
        val >>= 1
        if (val & 0x02) == 0x02:
            val >>= 4
            return val & 0x03
        return self.XPOWERS_CHG_LED_CTRL_CHG

    # @brief 预充电充电电流限制
    # @note  Precharge current limit 25N mA
    # @param   opt: 25  opt
    # # @retval None
    def setPrechargeCurr(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_IPRECHG_SET)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_IPRECHG_SET, val | opt)

    def getPrechargeCurr(self) -> None:
        return (super().readRegister(_AXP2101_IPRECHG_SET)[0] & 0x03)

     # @brief Set charge current.
     # @param   opt: See _axp2101_chg_curr_t enum for details.
     # @retval
    def setChargerConstantCurr(self, opt: int) -> None:
        if not 4 <= opt <= 16:
            raise ValueError(
                "Charger Constant Current must be a value within 4-16!")
        val = super().readRegister(_AXP2101_ICC_CHG_SET)[0]
        val &= 0xE0
        super().writeRegister(_AXP2101_ICC_CHG_SET, val | opt)

    # @brief Get charge current settings.
    # @retval See _axp2101_chg_curr_t enum for details.

    def getChargerConstantCurr(self) -> int:
        return (super().readRegister(_AXP2101_ICC_CHG_SET)[0] & 0x1F)

    # @brief  充电终止电流限制
    # @note   Charging termination of current limit

    def setChargerTerminationCurr(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_ITERM_CHG_SET_CTRL)[0]
        val &= 0xF0
        super().writeRegister(_AXP2101_ICC_CHG_SET, val | opt)

    def getChargerTerminationCurr(self) -> int:
        return (super().readRegister(_AXP2101_ITERM_CHG_SET_CTRL)[0] & 0x0F)

    def enableChargerTerminationLimit(self) -> None:
        val = super().readRegister(_AXP2101_ITERM_CHG_SET_CTRL)[0]
        super().writeRegister(_AXP2101_ITERM_CHG_SET_CTRL, val | 0x10)

    def disableChargerTerminationLimit(self) -> None:
        val = super().readRegister(_AXP2101_ITERM_CHG_SET_CTRL)[0]
        super().writeRegister(_AXP2101_ITERM_CHG_SET_CTRL, val & 0xEF)

    def isChargerTerminationLimit(self) -> bool:
        return bool(super().getRegisterBit(_AXP2101_ITERM_CHG_SET_CTRL, 4))

    # @brief Set charge target voltage.
    # @param   opt: See _axp2101_chg_vol_t enum for details.

    def setChargeTargetVoltage(self, opt: int) -> None:
        if not 1 <= opt <= 5:
            raise ValueError(
                "Charger target voltage must be a value within 0-3!")
        val = super().readRegister(_AXP2101_CV_CHG_VOL_SET)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_CV_CHG_VOL_SET, val | opt)

    # @brief Get charge target voltage settings.
    # @retval See _axp2101_chg_vol_t enum for details.

    def getChargeTargetVoltage(self) -> int:
        return (super().readRegister(_AXP2101_CV_CHG_VOL_SET)[0] & 0x03)

    # @brief  设定热阈值
    # @note   Thermal regulation threshold setting
    def setThermaThreshold(self, opt: int) -> None:
        val = super().readRegister(_AXP2101_THE_REGU_THRES_SET)[0]
        val &= 0xFC
        super().writeRegister(_AXP2101_THE_REGU_THRES_SET, val | opt)

    def getThermaThreshold(self) -> int:
        return (super().readRegister(_AXP2101_THE_REGU_THRES_SET)[0] & 0x03)

    def getBatteryParameter(self) -> int:
        return super().readRegister(_AXP2101_BAT_PARAME)[0]

    def fuelGaugeControl(self, writeROM: bool, enable: bool) -> int:
        if writeROM:
            super().clrRegisterBit(_AXP2101_FUEL_GAUGE_CTRL, 4)
        else:
            super().setRegisterBit(_AXP2101_FUEL_GAUGE_CTRL, 4)

        if enable:
            super().setRegisterBit(_AXP2101_FUEL_GAUGE_CTRL, 0)
        else:
            super().clrRegisterBit(_AXP2101_FUEL_GAUGE_CTRL, 0)

    #  Interrupt status/control functions
    # @brief  Get the interrupt controller mask value.
    # @retval   Mask value corresponds to _axp2101_irq_t ,

    def getIrqStatus(self) -> int:
        self.statusRegister = super().readRegister(_AXP2101_INTSTS1, 3)
        return (self.statusRegister[0] << 16) | (self.statusRegister[1] << 8) | (self.statusRegister[2])

    # @brief  Clear interrupt controller state.
    def clearIrqStatus(self) -> None:
        for i in range(0, _AXP2101_INTSTS_CNT):
            super().writeRegister(_AXP2101_INTSTS1 + i, 0xFF)
            self.statusRegister[i] = 0

    # @brief  Eanble PMU interrupt control mask .
    # @param   opt: View the related chip type _axp2101_irq_t enumeration parameters in "Params.hpp"
    def enableIRQ(self, opt: int, debug=False) -> None:
        self._setInterruptImpl(opt, True, debug)

    # @brief  Disable PMU interrupt control mask .
    # @param   opt: View the related chip type _axp2101_irq_t enumeration parameters in "Params.hpp"
    def disableIRQ(self, opt: int, debug=False) -> None:
        self._setInterruptImpl(opt, False, debug)

    # IRQ STATUS 0
    def isDropWarningLevel2Irq(self) -> bool:
        mask = self.XPOWERS_AXP2101_WARNING_LEVEL2_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isDropWarningLevel1Irq(self) -> bool:
        mask = self.XPOWERS_AXP2101_WARNING_LEVEL1_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isGaugeWdtTimeoutIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_WDT_TIMEOUT_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isBatChargerOverTemperatureIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_CHG_OVER_TEMP_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isBatChargerUnderTemperatureIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_CHG_UNDER_TEMP_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isBatWorkOverTemperatureIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_NOR_OVER_TEMP_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    def isBatWorkUnderTemperatureIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_NOR_UNDER_TEMP_IRQ
        if self.intRegister[0] & mask:
            return super()._IS_BIT_SET(self.statusRegister[0], mask)
        else:
            return False

    # IRQ STATUS 1
    def isVbusInsertIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_VBUS_INSERT_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isVbusRemoveIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_VBUS_REMOVE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBatInsertIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_INSERT_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isBatRemoveIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_REMOVE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isPekeyShortPressIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_PKEY_SHORT_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isPekeyLongPressIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_PKEY_LONG_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isPekeyNegativeIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    def isPekeyPositiveIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_PKEY_POSITIVE_IRQ >> 8
        if self.intRegister[1] & mask:
            return super()._IS_BIT_SET(self.statusRegister[1], mask)
        else:
            return False

    # IRQ STATUS 2
    def isWdtExpireIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_WDT_EXPIRE_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isLdoOverCurrentIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_LDO_OVER_CURR_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isBatfetOverCurrentIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BATFET_OVER_CURR_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isBatChagerDoneIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_CHG_DONE_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isBatChagerStartIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_CHG_START_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isBatDieOverTemperatureIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_DIE_OVER_TEMP_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isChagerOverTimeoutIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_CHAGER_TIMER_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def isBatOverVoltageIrq(self) -> bool:
        mask = self.XPOWERS_AXP2101_BAT_OVER_VOL_IRQ >> 16
        if self.intRegister[2] & mask:
            return super()._IS_BIT_SET(self.statusRegister[2], mask)
        else:
            return False

    def getChipID(self) -> int:
        return super().readRegister(_AXP2101_IC_TYPE)[0]

    def __to_bin(self, value, num) -> str:
        bin_chars = ""
        tmp = value
        for i in range(num):
            bin_char = bin(tmp % 2)[-1]
            tmp = tmp // 2
            bin_chars = bin_char + bin_chars
        return bin_chars.upper()

    def _setInterruptImpl(self, opts: int, enable: bool, debug: bool) -> None:
        if debug:
            print(("DISABLE", "ENABLE ")[enable], end='')
            print(': HEX:{:#08X}'.format(opts), end='')
            print(' BIN:', end='')
            print(self.__to_bin(opts, 64))
        if (opts & 0x0000FF):
            value = opts & 0xFF
            if debug:
                print('write in ints0 0b{0}'.format(self.__to_bin(value, 8)))
            data = super().readRegister(_AXP2101_INTEN1)[0]
            self.intRegister[0] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP2101_INTEN1, self.intRegister[0])
        if (opts & 0x00FF00):
            value = opts >> 8
            if debug:
                print('write in ints1 0b{0}'.format(self.__to_bin(value, 8)))
            data = super().readRegister(_AXP2101_INTEN2)[0]
            self.intRegister[1] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP2101_INTEN2, self.intRegister[1])
        if (opts & 0xFF0000):
            value = opts >> 16
            if debug:
                print('write in ints2 0b{0}'.format(self.__to_bin(value, 8)))
            data = super().readRegister(_AXP2101_INTEN3)[0]
            self.intRegister[2] = ((data & (~value)), (data | value))[enable]
            super().writeRegister(_AXP2101_INTEN3, self.intRegister[2])

    def printIntRegister(self) -> None:
        for i in range(0, _AXP2101_INTSTS_CNT):
            val = super().readRegister(_AXP2101_INTEN1+i)[0]
            print('[{0}]HEX={1} BIN={2}'.format(
                i, hex(val), self.__to_bin(val, 8)))
