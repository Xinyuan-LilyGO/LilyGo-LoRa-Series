<div align="center" markdown="1">
  <img src=".github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align = "center">🌟LilyGo LoRa Series🌟</h1>

## News

- Arduino IDE users need to configure the board to be used in the <utilities.h> file in the sketch directory before using the example. The <utilities.h> file contains the pin definitions of the board. If you do not select it, an error will occur during compilation.
- Arduino IDE 用户需要在 Sketch 目录下的 <utilities.h> 文件的最上方配置要使用的开发板，然后才能使用该示例。<utilities.h> 文件包含开发板的引脚定义。如果您未选择该文件，编译时将出现错误。
- ⚠️ If using the PA version, please carefully read the PA power setting limits on the corresponding page. Incorrect output power settings may damage the PA.
- ⚠️ 如果使用PA版本，请仔细阅读对应页面的PA功率设置限制，错误的设置输出功率很有可能会损坏PA

### `1` Get Started

- 🔧 **[T-Beam Quick Start](./docs/en/t_beam/t_beam_hw.md)**
- 🔧 **[T-Beam Supreme Quick Start](./docs/en/t_beam_supreme/t_beam_supreme_hw.md)**
- 🔧 **[T-Beam Supreme 144~148MHZ Quick Start](./docs/en/t_beam_supreme/t_beam_supreme_144_hw.md)**
- 🔧 **[T-Beam BPF Quick Start](./docs/en/t_beam_bpf/t_beam_bpf_hw.md)**
- 🔧 **[T-Beam 1W SX1262 Quick Start](./docs/en/t_beam_1w_sx1262/t_beam_1w_sx1262.md)**
- 🔧 **[T-Beam 1W LR1121 Quick Start](./docs/en/t_beam_1w_lr1121/t_beam_1w_lr1121.md)**
- 🔧 **[T-Beam 1W LR2021 Quick Start](./docs/en/t_beam_1w_lr2021/t_beam_1w_lr2021.md)**
- 🔧 **[LoRa32 V1.3 Quick Start](./docs/en/t3_v13_sx1276/t3_v13_sx1276.md)**
- 🔧 **[LoRa32 V1.6.1 Quick Start](./docs/en/t3_v161_sx1276/t3_v161_sx1276_hw.md)**
- 🔧 **[LoRa32 TCXO Quick Start](./docs/en/t3_tcxo_sx1276/t3_tcxo_sx1276_hw.md)**
- 🔧 **[T3-C6 Quick Start](./docs/en/t3_c6_sx1262/t3_c6_sx1262_hw.md)**
- 🔧 **[T3-S3-SX1262 Quick Start](./docs/en/t3_s3_sx1262/t3_s3_sx1262_hw.md)**
- 🔧 **[T3-S3-LR1121 Quick Start](./docs/en/t3_s3_lr1121/t3_s3_lr1121_hw.md)**
- 🔧 **[T3-S3-LR1121 PA Version Quick Start](./docs/en/t3_s3_lr1121_pa/t3_s3_lr1121_pa_hw.md)**
- 🔧 **[T3-S3-SX1280 Quick Start](./docs/en/t3_s3_sx1280/t3_s3_sx1280_hw.md)**
- 🔧 **[T3-S3-SX1280PA Quick Start](./docs/en/t3_s3_sx1280pa/t3_s3_sx1280pa.md)**
- 🔧 **[T3-S3-SX1276 Quick Start](./docs/en/t3_s3_sx1276/t3_s3_sx1276_hw.md)**
- 🔧 **[T3-S3-SX1278 Quick Start](./docs/en/t3_s3_sx1278/t3_s3_sx1278_hw.md)**

## `2` Example description

```bash
./examples/
├── ArduinoLoRa                              # Only support SX1276/SX1278 radio module (仅支持 SX1276/SX1278 无线电模块)
│   ├── LoRaReceiver
│   └── LoRaSender
├── BLE                                      # Bluetooth Low Energy examples
│   └── BLE_server
├── Display                                  # Only supports TBeam TFT Shield
│   ├── Free_Font_Demo
│   ├── TBeam_TFT_Shield
│   ├── TFT_Char_times
│   └── UTFT_demo
├── GPS                                      # T-Beam GPS demo examples
│   ├── TinyGPS_Example
│   ├── TinyGPS_FullExample
│   ├── TinyGPS_KitchenSink
│   ├── UBlox_BasicNMEARead                  # Only support Ublox GNSS Module
│   ├── UBlox_NMEAParsing                    # Only support Ublox GNSS Module
│   ├── UBlox_OutputRate                     # Only support Ublox GNSS Module
│   └── UBlox_Recovery                       # Only support Ublox GNSS Module
├── LoRa                                     # LoRa Minimum Example
│   ├── Receive_Interrupt                    # Common receive example for all boards
│   ├── Transmit_Interrupt                   # Common transmit example for all boards
│   ├── T3_LoRa32                            # LoRa32 V1.6
│   │   ├── SX1276_PingPong
│   │   └── SX1278_PingPong
│   ├── T3C6                                 # T3-C6
│   │   └── SX1262_PingPong
│   ├── T3S3                                 # T3-S3 (multiple radio chip versions)
│   │   ├── SX1262_PingPong
│   │   ├── SX1276_PingPong
│   │   ├── SX1278_PingPong
│   │   ├── SX1280_PingPong
│   │   ├── SX1280_Ranging
│   │   ├── SX1280PA_PingPong
│   │   ├── LR1121_PingPong
│   │   └── LR1121PA_PingPong
│   ├── TBeam_AXP2101                        # T-Beam V1.2 (AXP2101 PMU)
│   │   ├── SX1262_PingPong
│   │   ├── SX1276_PingPong
│   │   └── SX1278_PingPong
│   ├── TBeam_S3_Supreme                     # T-Beam S3 Supreme
│   │   ├── SX1262_PingPong
│   │   └── LR1121_PingPong
│   ├── TBeam1W                              # T-Beam 1W (High Power)
│   │   ├── SX1262_PingPong
│   │   ├── LR1121_PingPong
│   │   └── LR2021_PingPong
│   └── TBeamBPF_SX1278                      # T-Beam BPF
│       └── SX1278_PingPong
├── LoRaWAN                                  # LoRaWAN examples
│   ├── LMIC_Library_OTTA
│   ├── LoRaWAN_ABP
│   └── RadioLib_OTAA
├── OLED
│   ├── SH1106FontUsage
│   ├── SH1106GraphicsTest
│   ├── SH1106IconMenu
│   ├── SH1106PrintUTF8
│   ├── SSD1306SimpleDemo
│   └── SSD1306UiDemo
├── PMU                                      # T-Beam & T-Beam S3 PMU demo examples
├── SD                                       # SD card examples
│   └── SD_Test
├── Sensor                                   # Sensor examples,only support t-beams3-supreme
│   ├── BME280_AdvancedsettingsExample
│   ├── BME280_TestExample
│   ├── BME280_UnifiedExample
│   ├── I2CScanner
│   ├── PCF8563_AlarmByUnits
│   ├── PCF8563_SimpleTime
│   ├── PCF8563_TimeLib
│   ├── PCF8563_TimeSynchronization
│   ├── QMC6310_CalibrateExample
│   ├── QMC63xx_GetDataExample
│   ├── QMI8658_BlockExample
│   ├── QMI8658_GetDataExample
│   ├── QMI8658_InterruptBlockExample
│   ├── QMI8658_InterruptExample
│   ├── QMI8658_LockingMechanismExample
│   ├── QMI8658_MadgwickAHRS
│   ├── QMI8658_PedometerExample
│   ├── QMI8658_ReadFromFifoExample
│   └── QMI8658_WakeOnMotion
├── T3S3Factory                              # T3 S3 factory test examples
├── USBMSC                                   # USB Mass Storage examples
└── Factory                                  # T-Beam & T-Beam S3 and BPF factory test examples
```

## `3` Use reference

|    Product    |                        Youtube  link                        |                                                   explanation                                                   | Status |
| :-----------: | :---------------------------------------------------------: | :-------------------------------------------------------------------------------------------------------------: | :----: |
|    T -BEAM    | [Youtube link](https://www.youtube.com/watch?v=Xf1Wf-aTSmc) | How do I drive the T-Beam display  How to use T-Beam for positioning,How to use T-Beams  for communication test |   ✅    |
| LilyGo device | [Youtube link](https://www.youtube.com/watch?v=f3BybP3L7ls) |                                 How to enter boot mode to download the firmware                                 |   ✅    |

## `4` TTN

To access TTN (The Things Network), please refer to [LilyGo_LoRa_Series_Quick_Start](./docs/zh/LilyGo_LoRa_Series_Quick_Start_On_TTN_zh.md)
