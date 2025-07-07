<div align="center" markdown="1">
  <img src="../../../.github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align = "center">🌟LilyGo T-Beam🌟</h1>

## Overview

* This page introduces the hardware parameters related to `LilyGo T-Beam`

### Notes on use

1. Please be sure to connect the antenna before transmitting, otherwise it is easy to damage the RF module

### Product

| Product                | SOC             | Flash         | PSRAM         |
| ---------------------- | --------------- | ------------- | ------------- |
| [T-Beam Meshtastic][1] | ESP32-D0WDQ6-V3 | 4MB(Quad-SPI) | 8MB(Quad-SPI) |
| [T-Beam SoftRF][2]     | ESP32-D0WDQ6-V3 | 4MB(Quad-SPI) | 8MB(Quad-SPI) |

[1]: https://www.lilygo.cc/products/t-beam-v1-1-esp32-lora-module "T-Beam Meshtastic"
[2]: https://www.lilygo.cc/products/t-beam-softrf "T-Beam SoftRF"

### 📍 Pins Map

| Name                          | GPIO NUM           | Free |
| ----------------------------- | ------------------ | ---- |
| SDA                           | 21                 | ❌    |
| SCL                           | 22                 | ❌    |
| OLED(**SSD1306**) SDA         | Share with I2C bus | ❌    |
| OLED(**SSD1306**) SCL         | Share with I2C bus | ❌    |
| GNSS(**Ublox M6/M8**) TX      | 12                 | ❌    |
| GNSS(**Ublox M6/M8**) RX      | 34                 | ❌    |
| LoRa(**SX1276/SX1278**) SCK   | 5                  | ❌    |
| LoRa(**SX1276/SX1278**) MISO  | 19                 | ❌    |
| LoRa(**SX1276/SX1278**) MOSI  | 27                 | ❌    |
| LoRa(**SX1276/SX1278**) RESET | 23                 | ❌    |
| LoRa(**SX1276/SX1278**) DIO1  | 33                 | ❌    |
| LoRa(**SX1276/SX1278**) DIO2  | 32                 | ❌    |
| LoRa(**SX1276/SX1278**) CS    | 18                 | ❌    |
| Button1                       | 38                 | ❌    |
| PMU (**AXP2101**) IRQ         | 35                 | ❌    |
| PMU (**AXP2101**) SDA         | Share with I2C bus | ❌    |
| PMU (**AXP2101**) SCL         | Share with I2C bus | ❌    |

### 🧑🏼‍🔧 I2C Devices Address

| Devices                     | 7-Bit Address | Share Bus |
| --------------------------- | ------------- | --------- |
| OLED Display (**SSD1306**)  | 0x3C          | ✅️         |
| Power Manager (**AXP2101**) | 0x34          | ❌         |

### ⚡ Electrical parameters

| Features             | Details                     |
| -------------------- | --------------------------- |
| 🔗USB-C Input Voltage | 3.9V-6V                     |
| ⚡Charge Current      | 0-1024mA (\(Programmable\)) |
| 🔋Battery Voltage     | 3.7V                        |

### ⚡ PowerManage Channel

| Channel    | Peripherals     |
| ---------- | --------------- |
| DC1        | **ESP32**       |
| DC2        | Unused          |
| DC3        | Unused          |
| DC4        | Unused          |
| DC5        | Unused          |
| LDO1(VRTC) | Unused          |
| ALDO1      | Unused          |
| ALDO2      | **Radio**       |
| ALDO3      | **GPS**         |
| ALDO4      | Unused          |
| BLDO1      | Unused          |
| BLDO2      | Unused          |
| DLDO1      | Unused          |
| CPUSLDO    | Unused          |
| VBACKUP    | **GNSS Backup** |

### Button Description

| Channel | Peripherals                       |
| ------- | --------------------------------- |
| PWR     | PMU button, customizable function |
| IO38    | Customizable function             |
| RST     | Reset button                      |

* The PWR button is connected to the PMU
  1. In shutdown mode, press the PWR button to turn on the power supply
  2. In power-on mode, press the PWR button for 6 seconds (default time) to turn off the power supply

### LED Description

* CHG LED
  1. If not controlled by the program, the default is always on when charging and off when fully charged
  2. This LED can be controlled by the program

* PPS LED,
  1. The red LED near the GPS module cannot be turned off
  2. This LED cannot be turned off and is connected to the GPS PPS Pin. This LED flashes to indicate that the PPS pulse has arrived.

### RF parameters

| Features            | Details                        |
| ------------------- | ------------------------------ |
| RF  Module          | SX1278/SX1276                  |
| Frequency range     | 868/915MHz                     |
| Transfer rate(LoRa) | 0.018 K ～ 62.5 Kbps           |
| Transfer rate(FSK)  | 0.6 K ～ 300 Kbps              |
| Modulation          | FSK, GFSK, MSK, GMSK, LoRa,OOK |

### Resource

* [Schematic](../../../schematic/LilyGo_TBeam_V1.2.pdf)
* [AXP2101 datasheet](http://www.x-powers.com/en.php/Info/product_detail/article_id/95)
<!-- * [AXP192 datasheet](http://www.x-powers.com/en.php/Info/product_detail/article_id/29) -->
* [SX1262 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
* [SX1278 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278)
* [SX1276 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
* [SX1262 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
* [GSP NEO-6/NEO-8 datasheet](https://www.u-blox.com/en/product/neo-6-series)
