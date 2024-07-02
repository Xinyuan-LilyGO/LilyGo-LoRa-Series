<h1 align = "center">🌟LilyGo LoRa Series🌟</h1>

## **[English](./README.MD) | 中文**

## News 

- 本仓库目前仅维护在售版本的LilyGo，对于LilyGo停产版本的LoRa板请跳转[Archive package](https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/releases/tag/v0.0.1) 下载
- Arduino IDE 用户使用示例之前需要先在草图目录<utilities.h>文件中配置需要使用的板子，<utilities.h>文件包含板子的引脚定义，如果不进行选择，那么编译将会发生错误


## 1️⃣产品

| Product                            | SOC              | Flash         | PSRAM          |
| ---------------------------------- | ---------------- | ------------- | -------------- |
| [T-Beam S3 Supreme L76K][1]        | ESP32-S3FN8      | 8MB(Quad-SPI) | 8MB(Quad-SPI)  |
| [T-Beam S3 Supreme Ublox][2]       | ESP32-S3FN8      | 8MB(Quad-SPI) | 8MB(Quad-SPI)  |
| [T-Beam Meshtastic][3]             | ESP32-D0WDQ6-V3  | 4MB(Quad-SPI) | 8MB(Quad-SPI)  |
| [T-Beam SoftRF][4]                 | ESP32-D0WDQ6-V3  | 4MB(Quad-SPI) | 8MB(Quad-SPI)  |
| [T3-S3 SX1280 2.4G With out PA][5] | ESP32-S3FH4R2    | 4MB(Quad-SPI) | 2MB(Quad-SPI)  |
| [T3-S3 SX1280 2.4G With PA][6]     | ESP32-S3FH4R2    | 4MB(Quad-SPI) | 2MB(Quad-SPI)  |
| [T3-S3 SX1276][7]                  | ESP32-S3FH4R2    | 4MB(Quad-SPI) | 2MB(Quad-SPI)  |
| [T3-S3 SX1262][8]                  | ESP32-S3FH4R2    | 4MB(Quad-SPI) | 2MB(Quad-SPI)  |
| [LoRa32][8]                        | ESP32-PICO-D4    | 4MB(Quad-SPI) | ❌              |
| [T-Motion][9]                      | STM32L073RZ      | 192 Kbyte     | (20-Kbyte RAM) |
| [T3-C6][10]                        | ESP32-C6-MINI-1U | 4MB(Quad-SPI) | ❌              |

[1]: https://www.lilygo.cc/products/softrf-t-beamsupreme?variant=42880905052341 "T-Beam S3 Supreme L76K"
[2]: https://www.lilygo.cc/products/softrf-t-beamsupreme?variant=42880905281717 "T-Beam S3 Supreme Ublox"
[3]: https://www.lilygo.cc/products/t-beam-v1-1-esp32-lora-module "T-Beam Meshtastic"
[4]: https://www.lilygo.cc/products/t-beam-softrf "T-Beam SoftRF"
[5]: https://www.lilygo.cc/products/t3s3-v1-0?variant=42586879656117 "T3-S3 SX1280 2.4G With out PA"
[6]: https://www.lilygo.cc/products/t3s3-v1-0?variant=43043715678389 "T3-S3 SX1280 2.4G With PA"
[7]: https://www.lilygo.cc/products/t3s3-v1-0?variant=42586879688885 "T3-S3 SX1276"
[8]: https://www.lilygo.cc/products/t3s3-v1-0?variant=42586879721653 "T3-S3 SX1262"
[9]: https://www.lilygo.cc/products/t-motion-s76g-stm32-lora "T-Motion"
[10]: https://www.lilygo.cc/products "T3-C6"

## 2️⃣示例描述

```
./examples/
├── ArduinoLoRa                              # Only support SX1276/SX1278 radio module
│   ├── LoRaReceiver
│   └── LoRaSender
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
├── LoRaWAN                                  # LoRaWAN examples
│   ├── LMIC_Library_OTTA
│   └── RadioLib_OTAA
├── OLED
│   ├── SH1106FontUsage
│   ├── SH1106GraphicsTest
│   ├── SH1106IconMenu
│   ├── SH1106PrintUTF8
│   ├── SSD1306SimpleDemo
│   └── SSD1306UiDemo
├── PMU                                      # T-Beam & T-Beam S3 PMU demo examples
├── RadioLibExamples                         # RadioLib examples,Support SX1276/78/62/80...
│   ├── Receive_Interrupt
│   └── Transmit_Interrupt
├── Sensor                                   # Sensor examples,only support t-beams3-supreme
│   ├── BME280_AdvancedsettingsExample
│   ├── BME280_TestExample
│   ├── BME280_UnifiedExample
│   ├── PCF8563_AlarmByUnits
│   ├── PCF8563_SimpleTime
│   ├── PCF8563_TimeLib
│   ├── PCF8563_TimeSynchronization
│   ├── QMC6310_CalibrateExample
│   ├── QMC6310_CompassExample
│   ├── QMC6310_GetDataExample
│   ├── QMC6310_GetPolarExample
│   ├── QMI8658_BlockExample
│   ├── QMI8658_GetDataExample
│   ├── QMI8658_InterruptBlockExample
│   ├── QMI8658_InterruptExample
│   ├── QMI8658_LockingMechanismExample
│   ├── QMI8658_MadgwickAHRS
│   ├── QMI8658_PedometerExample
│   ├── QMI8658_ReadFromFifoExample
│   └── QMI8658_WakeOnMotion
└── T3S3Factory                              # T3 S3 factory test examples
```


## 3️⃣Arduino IDE 快速开始:

1. 安装 [Arduino IDE](https://www.arduino.cc/en/software)
2. 安装 [Arduino ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
3. 将 `lib` 目录中所有文件夹复制到`Sketchbook location`目录,如何找到自己的libraries 位置,[请看这里](https://support.arduino.cc/hc/en-us/articles/4415103213714-Find-sketches-libraries-board-cores-and-other-files-on-your-computer)
    - Windows: `C:\Users\{username}\Documents\Arduino`
    - macOS: `/Users/{username}/Documents/Arduino`
    - Linux: `/home/{username}/Arduino`
4. 打开对应的示例
   - 打开下载的`LilyGo-LoRa-Series`
   - 打开`examples`
   - 选择示例文件,打开`ino`结尾的文件
5. 在Arduino IDE 工具项目中选择相应的板子,点击下面列表中对应的选项进行选择
    <details>
    <summary>点击查看 T-Beam S3 Supreme Arduino IDE 选项</summary>

    | T-Beam S3 Supreme                    | Value                             |
    | ------------------------------------ | --------------------------------- |
    | Board                                | **ESP32S3 Dev Module**            |
    | Port                                 | Your port                         |
    | USB CDC On Boot                      | Enable                            |
    | CPU Frequency                        | 240MHZ(WiFi)                      |
    | Core Debug Level                     | None                              |
    | USB DFU On Boot                      | Disable                           |
    | Erase All Flash Before Sketch Upload | Disable                           |
    | Events Run On                        | Core1                             |
    | Flash Mode                           | QIO 80MHZ                         |
    | Flash Size                           | **8MB(64Mb)**                     |
    | Arduino Runs On                      | Core1                             |
    | USB Firmware MSC On Boot             | Disable                           |
    | Partition Scheme                     | **8M Flash(3M APP/1.5MB SPIFFS)** |
    | PSRAM                                | **QSPI PSRAM**                    |
    | Upload Mode                          | **UART0/Hardware CDC**            |
    | Upload Speed                         | 921600                            |
    | USB Mode                             | **CDC and JTAG**                  |
    | Programmer                           | **Esptool**                       |

    </details>


    <details>
    <summary>点击查看 T-Beam-V1.X Arduino IDE 选项</summary>

    | T-Beam-V1.X                          | Value                                                   |
    | ------------------------------------ | ------------------------------------------------------- |
    | Board                                | **ESP32 Dev Module**                                    |
    | Port                                 | Your port                                               |
    | CPU Frequency                        | 240MHZ(WiFi/BT)                                         |
    | Core Debug Level                     | None                                                    |
    | Erase All Flash Before Sketch Upload | Disable                                                 |
    | Events Run On                        | Core1                                                   |
    | Flash Frequency                      | 80MHZ                                                   |
    | Flash Mode                           | QIO                                                     |
    | Flash Size                           | **4MB(32Mb)**                                           |
    | JTAG Adapter                         | Disabled                                                |
    | Arduino Runs On                      | Core1                                                   |
    | Partition Scheme                     | **Default 4M Flash with spiffs(1.2M APP/1.5MB SPIFFS)** |
    | PSRAM                                | **Enable**                                              |
    | Upload Speed                         | 921600                                                  |
    | Programmer                           | **Esptool**                                             |

    </details>

    <details>
    <summary>点击查看 LoRa32 V1.x  Arduino IDE 选项</summary>

    | T-Beam-V1.X                          | Value                                                   |
    | ------------------------------------ | ------------------------------------------------------- |
    | Board                                | **ESP32 Dev Module**                                    |
    | Port                                 | Your port                                               |
    | CPU Frequency                        | 240MHZ(WiFi/BT)                                         |
    | Core Debug Level                     | None                                                    |
    | Erase All Flash Before Sketch Upload | Disable                                                 |
    | Events Run On                        | Core1                                                   |
    | Flash Frequency                      | 80MHZ                                                   |
    | Flash Mode                           | QIO                                                     |
    | Flash Size                           | **4MB(32Mb)**                                           |
    | JTAG Adapter                         | Disabled                                                |
    | Arduino Runs On                      | Core1                                                   |
    | Partition Scheme                     | **Default 4M Flash with spiffs(1.2M APP/1.5MB SPIFFS)** |
    | PSRAM                                | **Disable**                                             |
    | Upload Speed                         | 921600                                                  |
    | Programmer                           | **Esptool**                                             |

    </details>

    <details>
    <summary>点击查看 T3-S3 Arduino IDE 选项</summary>

    | T3-S3                                | Value                                                   |
    | ------------------------------------ | ------------------------------------------------------- |
    | Board                                | **ESP32S3 Dev Module**                                  |
    | Port                                 | Your port                                               |
    | USB CDC On Boot                      | Enable                                                  |
    | CPU Frequency                        | 240MHZ(WiFi)                                            |
    | Core Debug Level                     | None                                                    |
    | USB DFU On Boot                      | Disable                                                 |
    | Erase All Flash Before Sketch Upload | Disable                                                 |
    | Events Run On                        | Core1                                                   |
    | Flash Mode                           | QIO 80MHZ                                               |
    | Flash Size                           | **4MB(32Mb)**                                           |
    | Arduino Runs On                      | Core1                                                   |
    | USB Firmware MSC On Boot             | Disable                                                 |
    | Partition Scheme                     | **Default 4M Flash with spiffs(1.2M APP/1.5MB SPIFFS)** |
    | PSRAM                                | **QSPI PSRAM**                                          |
    | Upload Mode                          | **UART0/Hardware CDC**                                  |
    | Upload Speed                         | 921600                                                  |
    | USB Mode                             | **CDC and JTAG**                                        |
    | Programmer                           | **Esptool**                                             |

    </details>

    <details>
    <summary>点击查看 T-Motion  Arduino IDE 选项</summary>

    | T-Motion                    | Value                                    |
    | --------------------------- | ---------------------------------------- |
    | Board                       | **Nucleo-64**                            |
    | Port                        | Your port                                |
    | Debug symbols and core logs | None                                     |
    | Optimize                    | Smallest (-Os default)                   |
    | Board part number           | Nucleo L073RZ                            |
    | C Runtime Library           | Newlib Nano (default)                    |
    | Upload method               | STM32CubeProgrammer (DFU)                |
    | USB support (if available)  | CDC (generic 'Serial' supersede U(S)ART) |
    | U(S)ART support             | Enabled (generic 'Serial')               |
    | USB speed (if available)    | Low/Full Speed                           |
    
    - 需要上传草图,需要按住板子BOOT按键，然后插入USB，此时将进入DFU模式,才允许程序写入
    
    </details>


4. 请在每个草图的 `utilities.h` 文件的上方根据你的板子型号取消注释,否则编译将报错误
5. 上传草图

## 4️⃣ TTN

接入TTN(The Things Network)使用请参考 [LilyGo_LoRa_Series_Quick_Start](./docs/zh/LilyGo_LoRa_Series_Quick_Start_On_TTN_zh.md)


## 5️⃣ 应用

- [T-Beam SoftRF](https://github.com/lyusupov/SoftRF)
- [T-Beam Paxcounter](https://github.com/cyberman54/ESP32-Paxcounter)
- [T-Beam Meshtastic](https://github.com/meshtastic/Meshtastic-device)

# 6️⃣ 数据手册

##### T-Beam S3 Supreme datasheet

- [T-Beam Supreme schematic](schematic/LilyGo_T-BeamS3Supreme.pdf)
- [AXP2101 datasheet](./lib/XPowersLib/datasheet/AXP2101_Datasheet_V1.0_en.pdf)
- [SX1262 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [GSP MAX-M10 datasheet](https://www.u-blox.com/zh/product/max-m10-series)
- [PCF8563 datasheet](./lib/SensorsLib/datasheet/PCF8563%20Datasheet%20Rev.11.pdf)
- [QMI8658 datasheet](./lib/SensorsLib/datasheet/QMI8658A%20Datasheet%20Rev1.0.pdf)
- [QMC6310 datasheet](./lib/SensorsLib/datasheet/QMC6310%20Datasheet%20Rev.C.pdf)
- [BME280 datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)

##### T-Beam ESP32 datasheet

- [T-Beam_V1.2 schematic](schematic/LilyGo_TBeam_V1.2.pdf)
- [T-Beam_V1.1 schematic](schematic/LilyGo_TBeam_V1.1.pdf)
- [T-Beam_V1.0 schematic](schematic/LilyGo_TBeam_V1.0.pdf)
- [T-Beam_V0.7 schematic](schematic/LilyGo_TBeam_V0.7.pdf)
- [AXP192 datasheet](http://www.x-powers.com/en.php/Info/product_detail/article_id/29)
- [SX1262 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [GSP NEO-6/NEO-8 datasheet](https://www.u-blox.com/en/product/neo-6-series)

##### T3-S3 (ESP32S3) datasheet

- [T3_S3_V1.2 schematic](schematic/T3_S3_V1.2.pdf)
- [T3_S3_V1.1 schematic](schematic/T3_S3_V1.1.pdf)
- [T3_S3_V1.1 schematic](schematic/T3_S3_V1.0.pdf)
- [SX1262 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [SX1278 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278)
- [SX1276 datasheet](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
- [SX1280 datasheet](https://www.semtech.cn/products/wireless-rf/lora-connect/sx1280)

##### LoRa32(ESP32) datasheet

[T3_V1.6.1 schematic](schematic/T3_V1.6.1.pdf)

##### LoRa32(V1.0) datasheet

N.A

##### T-Motion datasheet

[T-Motion schematic](schematic/T-Motion.pdf)



# 7️⃣ 常见问题

1. T-Beam Supreme GPS备用电源来自于18650电池，如果移除18650电池将无法获得GPS热启动，如果需要使用GPS热启动，请连接18650电池
2. GPS没有数据输出，[参考GPS恢复出厂](firmware/NMEA_Reset/README.MD)
3. 如何找到引脚定义?
    <details>
    <summary>
    Click to view detailed steps
    </summary>

    - Each sketch contains <utilities.h>, and all board pins are written in this file
  
    </details>

4. 不能上传草图?
    <details>
    <summary>
    Click to view detailed steps
    </summary>

   1. 连接板子USB到电脑
   2. 按压BOOT按键，并且保持按压(如果没有BOOT按键,需要将GPIO0与GND连接)
   3. 按压 RST 按键
   4. 放开 RST 按键
   5. 最后松开BOOT按键(如果没有BOOT按键,需要将GPIO0与GND断开连接)
   6. 点击上传程序

    </details>
5. LoRa设置输出功率后需要设置最低电流。设置sx1262如下 :
    <details>
    <summary>
    Click to view detailed steps
    </summary>

    ```c
    // set output power to 10 dBm (accepted range is -17 - 22 dBm)
    if (radio.setOutputPower(22) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }
    // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    if (radio.setCurrentLimit(80) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        Serial.println(F("Selected current limit is invalid for this module!"));
        while (true);
    }
    ```

    </details>
6. SX1280 PA版本使用注意事项
    <details>
    <summary>
    Click to view detailed steps
    </summary>

   - 在使用SX1280 PA模块时，建议软件控制1280芯片输出2~5dbm，这样整个模组会输出20dbm的功率；
   当1280芯片输出高于2dbm时，整个模组输出功率也只有20dbm；
   当1280芯片输出高于5dbm时，整个模组输出功率也只有20dbm，但5dbm给到FEM，会损坏FEM芯片（FEM允许最大输入是5dbm）
   如果1280芯片输出低于2dbm，整个模组输出功率20dbm以内。
   - T3-S3 PA版本 SX1280 输出功率设置不能大于2~5dbm，否则可能导致FEM芯片损坏。设置3dbm就已经有20dmb的输出功率
  
    </details>
7. 如何分辨LoRa型号?
    <details>
    <summary>
    Click to view detailed steps
    </summary>

    - 分辨LoRa的型号只需要查看板子上的贴纸,433MHz则是SX1278，868MHz则是SX1276，如果标签写的是SX1262则是SX1262

    </details>

8. 为什么Serial没有任何输出?
   -  Arduino -> Tools -> USB CDC On Boot -> Enable


9. T-Beam-s3-supreme 天线信号质量
     <details>
     <summary>
     Click to view detailed
     </summary>

     ![t-beams3-supreme-ant-868M](./images/t-beams3-supreme-ant-868M.jpg)
     ![t-beams3-supreme-ant-915M](./images/t-beams3-supreme-ant-915M.jpg)

     </details>




