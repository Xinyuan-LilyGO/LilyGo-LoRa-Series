<h1 align = "center">🌟LilyGo LoRa Series🌟</h1>

## **English | [中文](./README_CN.MD)**

## USE Arduino IDE

1. Install the current upstream Arduino IDE at the 1.8 level or later. The current version is at the [Arduino website](http://www.arduino.cc/en/main/software). (NOTE: MacOS may require that you install separate drivers to interact with your board. You will get error messages that occur when the IDE attempts to upload code. You should consider running the Arduino IDE in a Virtual Machine on Linux. Installing other drivers on your Mac is out of scope for this document.)
2. Start Arduino and open Preferences window. In additional board manager add url: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` If you have multiple URLs (e.g., for other boards), just add previously mentioned URL, separated by a comma.
3. Copy all folders in the `lib` directory to your Arduino Libraries Directory,
- in MacOS, it is `~/Documents/Arduino/libraries`
- in Linux, it is `~/Arduino/libraries` 
- in Windows, it is `Documents/Arduino/libraries` 
4. Open the corresponding example
5. In the Arduino Application, navigate to the Board Manager menu ("Tools => Board => Board Manager"), search for `esp32`, and make sure that you install the latest version. After you do this, you can select the board you have from the list of boards in the "ESP32 Arduino" menu ("Tools=> Board => ESP32 Arduino"). It's very important to note that the list of boards is NOT in alphabetical order. Look for the board you are working with carefully in the list. 
6. `T-Beam` users can choose `TTGO T-Beam`, `lora32(T3)` users can choose `ESP32 Dev Module` , `T-Motion` users choose `Nucleo_l073RZ`,`T3-S3-V1.0`and `T-BeamS3 Core` users can selected `ESP32S3 Dev Module`
7. Select the port of the board in the port
8. For other examples except the `GPS` directory, please **uncomment** in the upper part of `utilities.h` according to your board model, otherwise the compilation will report an error
9. Upload

## Cautions 
1. The output power setting of SX1280 cannot be greater than 3dbm. Failure to do so may result in damage to the FEM chip.  Setting 3dbm already has 20dmb of output power(radio.setOutputPower(3);)

## Product 📷

|          Product          |          Product Link           |                       Schematic                        | Corresponding example macro definition |
| :-----------------------: | :-----------------------------: | :----------------------------------------------------: | :------------------------------------: |
|  [T-Beam SX1278/76][1-4]  |         [AliExpress][1]         | [TBeamV0.7][1-1] / [TBeamV1.0][1-2] / [TBeamV1.1][1-3] | LILYGO_TBeam_V0_7 / LILYGO_TBeam_V1_X  |
|   [T-Beam SX1262][1-4]    |         [AliExpress][2]         | [TBeamV0.7][1-1] / [TBeamV1.0][1-2] / [TBeamV1.1][1-3] | LILYGO_TBeam_V0_7 / LILYGO_TBeam_V1_X  |
| [LoRa32 V1.0 SX1276][3-1] |         [AliExpress][3]         |                           ❌                            |             LILYGO_T3_V1_0             |
| [LoRa32 V1.0 SX1278][3-1] |         [AliExpress][4]         |                           ❌                            |             LILYGO_T3_V1_0             |
| [LoRa32 V1.3 SX1276][5-2] |         [AliExpress][5]         |                   [LoRa32 V1.3][5-1]                   |             LILYGO_T3_V1_3             |
|    [LoRa32 V2.0][6-2]     |         [AliExpress][6]         |                   [LoRa32 V2.0][6-1]                   |             LILYGO_T3_V1_3             |
| [LoRa32 V2.1 & 1.6][7-4]  | [AliExpress][7-1]/[Amazon][7-2] |                 [LoRa32 V2.1_1.6][7-3]                 |             LILYGO_T3_V1_6             |
|   [T-Motion S76G][8-2]    |         [AliExpress][8]         |                    [T-Motion][8-1]                     |             LILYGO_T3_V1_6             |
|  [LoRa32 V1.8 2.4G][9-2]  |         [lilygo.cc][9]          |                [LoRa32 V1.8 2.4G][9-1]                 |             LILYGO_T3_V1_8             |
|   [LoRa T3 S3 V1.0][10]   |         [lilygo.cc][10]         |                [LoRa T3 S3 V1.0][10-1]                 |           LILYGO_T3_S3_V1_0            |
|   [LoRa T3 S3 V1.1][11]   |         [lilygo.cc][11]         |                [LoRa T3 S3 V1.1][11-1]                 |    LILYGO_T3_S3_V1_0  (Same as 1.0)    |


[1]: https://pt.aliexpress.com/item/32967228739.html "T-Beam SX1278/76"
[1-1]: schematic/LilyGo_TBeam_V0.7.pdf
[1-2]: schematic/LilyGo_TBeam_V1.0.pdf
[1-3]: schematic/LilyGo_TBeam_V1.1.pdf
[1-4]: assets/image/t-beam_v1.1_pinmap.jpg
[2]: https://pt.aliexpress.com/item/4001287221970.html "T-Beam SX1262"
[3]: https://pt.aliexpress.com/item/32842155838.html "LoRa32 V1.0 SX1276"
[3-1]: assets/image/lora32_v1.0_pinmap.jpg
[4]: https://pt.aliexpress.com/item/32842155838.html "LoRa32 V1.0 SX1278"
[5]: https://pt.aliexpress.com/item/4000628100802.html "LoRa32 V1.3 SX1276"
[5-1]: schematic/T3_V1.3.pdf
[5-2]: assets/image/lora32_v1.3_pinmap.jpg
[6]: https://pt.aliexpress.com/item/32847443952.html "LoRa32 V2.0"
[6-1]: schematic/T3_V2.0.pdf
[6-2]: assets/image/lora32_v2.0_pinmap.jpg
[7-1]: https://pt.aliexpress.com/item/32872078587.html "AliExpress"
[7-2]: https://www.amazon.com/dp/B09SHRWVNB?ref=myi_title_dp "Amazon"
[7-3]: schematic/T3_V1.6.pdf
[7-4]: assets/image/lora32_v1.6.1_pinmap.jpg
[8]: https://www.aliexpress.com/item/4000571051141.html "T-Motion S76G"
[8-1]: schematic/T-Motion.pdf
[8-2]: assets/image/t-motion_v1.0_pinmap.jpg
[9]: https://www.lilygo.cc/products/lora-v1-8-esp32 "LoRa32 V1.8 2.4G"
[9-1]: schematic/T3_V1.8.pdf
[9-2]: assets/image/lora32_v1.8_pinmap.jpg
[10]: https://pt.aliexpress.com/item/1005004627139838.html "LoRa T3 S3 V1.0"
[10-1]: schematic/T3_S3_V1.0.pdf
[11]: NULL
[11-1]: schematic/T3_S3_V1.1.pdf




## Example description

```
├─ArduinoLoRa       # Only support SX1276/SX1278 radio module
│  ├─LoRaReceiver
│  └─LoRaSender
├─GPS                           # T-Beam GPS demo examples
│  ├─TinyGPS_Example
│  ├─TinyGPS_FullExample
│  ├─TinyGPS_KitchenSink
│  ├─UBlox_BasicNMEARead        # Only support Ublox GNSS Module 
│  ├─UBlox_Recovery             # Only support Ublox GNSS Module 
│  ├─UBlox_OutputRate           # Only support Ublox GNSS Module 
│  └─UBlox_NMEAParsing          # Only support Ublox GNSS Module 
├─OLED                          # OLED demo examples
│  ├─SH1106FontUsage
│  ├─SH1106GraphicsTest
│  ├─SH1106IconMenu
│  ├─SH1106PrintUTF8
│  ├─SSD1306SimpleDemo
│  └─SSD1306UiDemo
├─RadioLibExamples              # RadioLib examples,Support SX1276/78/62/80
│  ├─SX1262
│  │  ├─SX1262_Receive_Interrupt
│  │  └─SX1262_Transmit_Interrupt
│  ├─SX1268
│  │  ├─SX1268_Receive_Interrupt
│  │  └─SX1268_Transmit_Interrupt
│  ├─SX1276
│  │  ├─SX1276_Receive_Interrupt
│  │  └─SX1276_Transmit_Interrupt
│  ├─SX1278
│  │  ├─SX1278_Receive
│  │  ├─SX1278_Receive_Interrupt
│  │  ├─SX1278_Transmit
│  │  └─SX1278_Transmit_Interrupt
│  ├─SX1280
│  │  ├─Ranging
│  │  │  ├─Ranging_Master
│  │  │  └─Ranging_Slave
│  │  ├─SX128x_Receive_Interrupt
│  │  └─SX128x_Transmit_Interrupt
│  └─TBeamAllFunction   # T-Beam all function example,Only tbeamv1.x is supported temporarily
├── Sensor            # Sensor examples , just support t-beam-s3-core
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
├─SleepTest
└─LoRaWAN                           # OTAA method Join TTN example
    └─OTTA
```

## Radio resolution

How to distinguish the model of Radio just need to look at the sticker on the board, 
433MHz is SX1278,
868MHz is SX1276,
If the label says SX1262 then it is SX1262

## Precautions

1. If there is no data output from GPS, [see Restoring factory defaults ](firmware/NMEA_Reset/README.MD)

## PinOut

- See [utilities.h](examples/ArduinoLoRa/LoRaReceiver/utilities.h)

## Application

- [T-Beam SoftRF](https://github.com/lyusupov/SoftRF)
- [T-Beam Paxcounter](https://github.com/cyberman54/ESP32-Paxcounter)
- [T-Beam Meshtastic](https://github.com/meshtastic/Meshtastic-device)

## TTN

To quickly start using TTN (The Things Network), please refer to [LilyGo_LoRa_Series_Quick_Start](./docs/en/LilyGo_LoRa_Series_Quick_Start_On_TTN.md)

<h3 align = "left">Datasheet :</h3>

- [AXP192](http://www.x-powers.com/en.php/Info/product_detail/article_id/29)
- [AXP2101](./lib/XPowersLib/datasheet/AXP2101_Datasheet_V1.0_en.pdf)
- [SX1262](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [SX1278](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278)
- [SX1276](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
- [SX1280](https://www.semtech.com/products/wireless-rf/lora-24ghz/sx1280)
- [sx1280](https://www.semtech.cn/products/wireless-rf/lora-connect/sx1280)
- [GSP NEO-6/NEO-8](https://www.u-blox.com/en/product/neo-6-series)
- [GSP MAX-M10](https://www.u-blox.com/zh/product/max-m10-series)


## Announcements

1. After setting the LoRa output power, set the lowest current. Set sx1262 as follows :

```
    // digitalWrite(LoRa_Busy, LOW);
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

## Tips:

1. When using the SX1280 PA, it is recommended that the software control 1280 chip output 2 to 5 dbm, so that the entire module will output 20 dbm of power;
When the output of 1280 chip is higher than 2 dbm, the output power of the entire module is only 20 dbm;
When the output of 1280 chip is higher than 5 dbm, the output power of the entire module is only 20 dbm, but if 5 dbm is given to the FEM, it will damage the FEM chip (the maximum allowable input for the FEM is 5 dbm)
If the output of the 1280 chip is lower than 2 dbm, the output power of the entire module is within 20 dbm.