<h1 align = "center">🌟LilyGo LoRa Series🌟</h1>

## **[English](./README.MD) | 中文**

<h3 align = "left">快速开始:</h3>

1. 将 `libdeps` 目录中所有文件夹复制到`~/Arduino/libraries`目录内,Windons用户放在 `"我的文档/Arduino/libraries"`
2. 打开对应的示例
3. `T-Beam`用户可以选择`TTGO T-Beam`,`lora32(T3)`用户可以选择`ESP32 Dev Module`,`T-Motion` 用户选择 `Nucleo_l073RZ`
4. 在端口中选择板子的端口
5. 除了GPS 目录外的其他示例，请在`utilities.h` 中上方根据你的板子型号取消注释,否则编译将报错误
6. 上传


<h3 align = "left">Product 📷:</h3>


|           Product           |                           Product  Link                            |                                                                   Schematic                                                                    |
| :-------------------------: | :----------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------: |
|    [T-Beam SX1278/76 ]()    |  [Product link](https://pt.aliexpress.com/item/32967228739.html)   | [TBeamV0.7](schematic/LilyGo_TBeam_V0.7.pdf)/     [TBeamV1.0](schematic/LilyGo_TBeam_V1.0.pdf)/  [TBeamV1.1 ](schematic/LilyGo_TBeam_V1.1.pdf) |
|      [T-Beam SX1262]()      | [Product link](https://pt.aliexpress.com/item/4001287221970.html)  | [TBeamV0.7](schematic/LilyGo_TBeam_V0.7.pdf)/     [TBeamV1.0](schematic/LilyGo_TBeam_V1.0.pdf)/  [TBeamV1.1 ](schematic/LilyGo_TBeam_V1.1.pdf) |
|   [LoRa32 V1.0 SX1276]()    |  [Product link](https://pt.aliexpress.com/item/32842155838.html)   |                                                                       ❌                                                                        |
|   [LoRa32 V1.0 SX1278]()    |  [Product link](https://pt.aliexpress.com/item/32842155838.html)   |                                                                       ❌                                                                        |
|   [LoRa32 V1.3 SX1276]()    | [Product link](https://pt.aliexpress.com/item/4000628100802.html)  |                                                      [LoRa32 V1.3](schematic/T3_V1.3.pdf)                                                      |
|       [LoRa32 V2.0]()       |  [Product link](https://pt.aliexpress.com/item/32847443952.html)   |                                                      [LoRa32 V2.0](schematic/T3_V2.0.pdf)                                                      |
| [LoRa32 V2.1_1.6 Version]() |  [Product link](https://pt.aliexpress.com/item/32872078587.html)   |                                                    [LoRa32 V2.1_1.6](schematic/T3_V1.6.pdf)                                                    |
|      [T-Motion S76G]()      | [Product link](https://www.aliexpress.com/item/4000571051141.html) |                                                       [T-Motion](schematic/T-Motion.pdf)                                                       |


## 示例描述

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

<h3 align = "left">常见问题 :</h3>

1. GPS没有数据输出，[参考GPS恢复出厂](firmware/GPS_%20reset)


<h3 align = "left">引脚映射 :</h3>

- See [utilities.h](examples/ArduinoLoRa/LoRaReceiver/utilities.h)

<h3 align = "left">应用 :</h3>

- [T-Beam SoftRF](https://github.com/lyusupov/SoftRF)
- [T-Beam Paxcounter](https://github.com/cyberman54/ESP32-Paxcounter)
- [T-Beam Meshtastic](https://github.com/meshtastic/Meshtastic-device)

### TTN

接入TTN(The Things Network)使用请参考 [LilyGo_LoRa_Series_Quick_Start](./docs/zh/LilyGo_LoRa_Series_Quick_Start_On_TTN_zh.md)

<h3 align = "left">芯片手册 :</h3>

- [AXP192](http://www.x-powers.com/en.php/Info/product_detail/article_id/29)
- [SX1262](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [SX1278](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278)
- [SX1276](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
- [GSP NEO-6/NEO-8](https://www.u-blox.com/en/product/neo-6-series)
- [GPS Air530]()
