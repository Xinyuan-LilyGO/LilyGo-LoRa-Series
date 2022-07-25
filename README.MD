<h1 align = "center">🌟LilyGo LoRa Series🌟</h1>

## **English | [中文](./README_CN.MD)**


<h3 align = "left">Quick start:</h3>

1. Copy all folders in the `libdeps` directory to the `~/Arduino/libraries` directory, and put them in `"Documents/Arduino/libraries"` for Windons users
2. Open the corresponding example
3. `T-Beam` users can choose `TTGO T-Beam`, `lora32(T3)` users can choose `ESP32 Dev Module` , `T-Motion` users choose `Nucleo_l073RZ`
4. Select the port of the board in the port
5. For other examples except the `GPS` directory, please **uncomment** in the upper part of `utilities.h` according to your board model, otherwise the compilation will report an error
6. Upload

<h3 align = "left">Product 📷:</h3>

|           Product           |                                                         Product  Link                                                         |                                                                   Schematic                                                                    |
| :-------------------------: | :---------------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------: |
|    [T-Beam SX1278/76 ]()    |                                [Product link](https://pt.aliexpress.com/item/32967228739.html)                                | [TBeamV0.7](schematic/LilyGo_TBeam_V0.7.pdf)/     [TBeamV1.0](schematic/LilyGo_TBeam_V1.0.pdf)/  [TBeamV1.1 ](schematic/LilyGo_TBeam_V1.1.pdf) |
|      [T-Beam SX1262]()      |                               [Product link](https://pt.aliexpress.com/item/4001287221970.html)                               | [TBeamV0.7](schematic/LilyGo_TBeam_V0.7.pdf)/     [TBeamV1.0](schematic/LilyGo_TBeam_V1.0.pdf)/  [TBeamV1.1 ](schematic/LilyGo_TBeam_V1.1.pdf) |
|   [LoRa32 V1.0 SX1276]()    |                                [Product link](https://pt.aliexpress.com/item/32842155838.html)                                |                                                                       ❌                                                                        |
|   [LoRa32 V1.0 SX1278]()    |                                [Product link](https://pt.aliexpress.com/item/32842155838.html)                                |                                                                       ❌                                                                        |
|   [LoRa32 V1.3 SX1276]()    |                               [Product link](https://pt.aliexpress.com/item/4000628100802.html)                               |                                                      [LoRa32 V1.3](schematic/T3_V1.3.pdf)                                                      |
|       [LoRa32 V2.0]()       |                                [Product link](https://pt.aliexpress.com/item/32847443952.html)                                |                                                      [LoRa32 V2.0](schematic/T3_V2.0.pdf)                                                      |
| [LoRa32 V2.1_1.6 Version]() | [AliExpress](https://pt.aliexpress.com/item/32872078587.html)/[Amazon](https://www.amazon.com/dp/B09SHRWVNB?ref=myi_title_dp) |                                                    [LoRa32 V2.1_1.6](schematic/T3_V1.6.pdf)                                                    |
|      [T-Motion S76G]()      |                              [Product link](https://www.aliexpress.com/item/4000571051141.html)                               |                                                       [T-Motion](schematic/T-Motion.pdf)                                                       |


## Example description


```
examples
├── ArduinoLoRa                                 # Just only support SX1276/SX1278 radio module
│   ├── LoRaReceiver                            
│   └── LoRaSender
├── GPS                                         # T-Beam GPS demo examples
│   ├── Example1_BasicNMEARead
│   ├── Example2_NMEAParsing
│   ├── Example3_FactoryDefaultsviaSerial
│   ├── TinyGPS_Example
│   └── TinyGPS_KitchenSink
├── OLED                                        # OLED demo examples    
│   ├── SSD1306SimpleDemo
│   └── SSD1306UiDemo   
├── RadioLibExamples                            # RadioLib examples,Support SX1276/78/62
│   ├── SX1262
│   │   ├── SX1262_Receive_Interrupt
│   │   └── SX1262_Transmit_Interrupt
│   ├── SX1276
│   │   ├── SX1276_Receive_Interrupt
│   │   └── SX1276_Transmit_Interrupt
│   ├── SX1278
│   │   ├── SX1278_Receive
│   │   ├── SX1278_Receive_Interrupt
│   │   ├── SX1278_Transmit
│   │   └── SX1278_Transmit_Interrupt
│   └── TBeamAllFunction                        # T-Beam all function example
└── TTN
    └── TTN_OTTA                                # OTAA method Join TTN example

```



<h3 align = "left">Precautions :</h3>

1. If there is no data output from GPS, [see Restoring factory defaults ](firmware/GPS_%20reset)

<h3 align = "left">PinOut :</h3>

- See [utilities.h](examples/ArduinoLoRa/LoRaReceiver/utilities.h)

<h3 align = "left">Application :</h3>

- [T-Beam SoftRF](https://github.com/lyusupov/SoftRF)
- [T-Beam Paxcounter](https://github.com/cyberman54/ESP32-Paxcounter)
- [T-Beam Meshtastic](https://github.com/meshtastic/Meshtastic-device)

### TTN

To quickly start using TTN (The Things Network), please refer to [LilyGo_LoRa_Series_Quick_Start](./docs/en/LilyGo_LoRa_Series_Quick_Start_On_TTN.md)

<h3 align = "left">Datasheet :</h3>

- [AXP192](http://www.x-powers.com/en.php/Info/product_detail/article_id/29)
- [SX1262](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [SX1278](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278)
- [SX1276](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
- [GSP NEO-6/NEO-8](https://www.u-blox.com/en/product/neo-6-series)
- [GPS Air530]()



<h3 align = "left">Announcements :</h3>

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