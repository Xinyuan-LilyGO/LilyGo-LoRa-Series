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

### PlatformIO Quick Start

1. Install the **CH9102 USB bridge** driver for the first time.
   * [Windows](https://www.wch-ic.com/downloads/CH343SER_ZIP.html)
   * [Mac OS](https://www.wch-ic.com/downloads/CH34XSER_MAC_ZIP.html)
2. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
3. Search for the `PlatformIO` plugin in the `Visual Studio Code` extension and install it.
4. After the installation is complete, you need to restart `Visual Studio Code`
5. After restarting `Visual Studio Code`, select `File` in the upper left corner of `Visual Studio Code` -> `Open Folder` -> select the `LilyGo-LoRa-Series` directory
6. Wait for the installation of third-party dependent libraries to complete
7. Click on the `platformio.ini` file, and in the `platformio` column
8. Select the board name you want to use in `default_envs` and uncomment it.
9. Uncomment one of the lines `src_dir = xxxx` to make sure only one line works , Please note the example comments, indicating what works and what does not.
10. Click the (✔) symbol in the lower left corner to compile
11. Connect the board to the computer USB-C , Micro-USB is used for module firmware upgrade
12. Click (→) to upload firmware
13. Click (plug symbol) to monitor serial output
14. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below

### Arduino IDE quick start

1. Install the **CH9102 USB bridge** driver for the first time.
   * [Windows](https://www.wch-ic.com/downloads/CH343SER_ZIP.html)
   * [Mac OS](https://www.wch-ic.com/downloads/CH34XSER_MAC_ZIP.html)
2. Install [Arduino IDE](https://www.arduino.cc/en/software)
3. Install [Arduino ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
4. Copy all folders in the `lib` directory to the `Sketchbook location` directory. How to find the location of your own libraries, [please see here](https://support.arduino.cc/hc/en-us/articles/4415103213714-Find-sketches-libraries-board-cores-and-other-files-on-your-computer)
    * Windows: `C:\Users\{username}\Documents\Arduino`
    * macOS: `/Users/{username}/Documents/Arduino`
    * Linux: `/home/{username}/Arduino`
5. Open the corresponding example
    * Open the downloaded `LilyGo-LoRa-Series`
    * Open `examples`
    * Select the sample file and open the file ending with `ino`
6. On Arduino ISelect the corresponding board in the DE tool project and click on the corresponding option in the list below to select

    | Name                                 | Value                                |
    | ------------------------------------ | ------------------------------------ |
    | Board                                | **ESP32 Dev Module**                 |
    | Port                                 | Your port                            |
    | CPU Frequency                        | 240MHZ(WiFi/BT)                      |
    | Core Debug Level                     | None                                 |
    | Erase All Flash Before Sketch Upload | Disable                              |
    | Events Run On                        | Core1                                |
    | Flash Frequency                      | 80MHZ                                |
    | Flash Mode                           | QIO                                  |
    | Flash Size                           | **4MB(32Mb)**                        |
    | JTAG Adapter                         | Disabled                             |
    | Arduino Runs On                      | Core1                                |
    | Partition Scheme                     | **Huge APP (3MB No OTA/1MB SPIFFS)** |
    | PSRAM                                | **Enable**                           |
    | Upload Speed                         | 921600                               |
    | Programmer                           | **Esptool**                          |

6. Please uncomment the `utilities.h` file of each sketch according to your board model ,e.g `T3_V3_0_SX1276_TCXO`, otherwise the compilation will report an error.
7. Upload sketch

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

> [!IMPORTANT]
> When the bandwidth is lower than 62.5kHz, LoRa modules using non-TCXOs will not be able to fully communicate, resulting in poor communication quality.
> Modules equipped with TCXOs do not experience this issue.
>

#### Resource

* [T3-TCXO schematic](../../../schematic/T3_V3.0.pdf)
* [SX1276 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
* [T3-TCXO DXF](../../../dimensions/T3_V3.0_TCXO.dxf)
