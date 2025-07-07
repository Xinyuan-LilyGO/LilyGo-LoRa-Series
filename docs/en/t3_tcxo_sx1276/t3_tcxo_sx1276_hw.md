<div align="center" markdown="1">
  <img src="../../../.github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align = "center">🌟LilyGo T3 TCXO SX1276🌟</h1>

## Overview

* This page introduces the hardware parameters related to `LilyGo T3 TCXO SX1276`

### Notes on use

1. This version does not have BMS, please use a lithium-ion battery with battery protection function
2. Please be sure to connect the antenna before transmitting, otherwise it is easy to damage the RF module

### Product

| Product       | SOC           | Flash         | PSRAM |
| ------------- | ------------- | ------------- | ----- |
| [T-3 TCXO][1] | ESP32-PICO-D4 | 4MB(Quad-SPI) | ❌     |

[1]: https://lilygo.cc/products/t3-tcxo "T-3 TCXO"

### 📍 Pins Map

| Name                     | GPIO NUM | Free |
| ------------------------ | -------- | ---- |
| OLED(**SSD1306**) SDA    | 21       | ❌    |
| OLED(**SSD1306**) SCL    | 22       | ❌    |
| SD CS                    | 13       | ❌    |
| SD MOSI                  | 15       | ❌    |
| SD MISO                  | 2        | ❌    |
| SD SCK                   | 14       | ❌    |
| LoRa(**SX1276**) SCK     | 5        | ❌    |
| LoRa(**SX1276**) MISO    | 19       | ❌    |
| LoRa(**SX1276**) MOSI    | 27       | ❌    |
| LoRa(**SX1276**) RESET   | 23       | ❌    |
| LoRa(**SX1276**) DIO0    | 26       | ❌    |
| LoRa(**SX1276**) DIO1    | 32       | ❌    |
| LoRa(**SX1276**) CS      | 7        | ❌    |
| LoRa(**SX1276**) TCXO EN | 12       | ❌    |
| Battery ADC              | 35       | ❌    |
| On Board LED             | 25       | ❌    |

* **The GPIOs above GPIO33 of the ESP32 chip only allow input functions and cannot be set as output functions.**
* **You need to remove the SD card when uploading the sketch, otherwise it will not be downloaded**
* **TCXO EN must be set to high before initializing the Radio**

### 🧑🏼‍🔧 I2C Devices Address

| Devices              | 7-Bit Address | Share Bus |
| -------------------- | ------------- | --------- |
| OLED Display SSD1306 | 0x3C          | ✅️         |

### ⚡ Electrical parameters

| Features             | Details |
| -------------------- | ------- |
| 🔗USB-C Input Voltage | 5V      |
| ⚡Charge Current      | 500mA   |
| 🔋Battery Voltage     | 3.7V    |


### Button Description

| Channel | Peripherals  |
| ------- | ------------ |
| RST     | Reset button |

### LED Description

* CHG LED

| LED State | Details               |
| --------- | --------------------- |
| On        | Battery charging      |
| Off       | Battery Full          |
| Blink     | Battery not connected |

* User LED

1. The LED is connected to ESP32 GPIO25, and the LED is turned on or off by writing a high or low level

### RF parameters

| Features            | Details                         |
| ------------------- | ------------------------------- |
| RF  Module          | SX1276                          |
| Frequency range     | 840～945MHz                     |
| Transfer rate(LoRa) | 0.018K～37.5Kbps                |
| Transfer rate(FSK)  | 1.2K～300Kbps                   |
| Modulation          | FSK, GFSK, MSK, GMSK, LoRa，OOK |

#### Resource

* [T3-TCXO schematic](../../../schematic/T3_V3.0.pdf)
* [SX1276 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)

