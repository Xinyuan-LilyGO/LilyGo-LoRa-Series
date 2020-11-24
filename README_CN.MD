<h1 align = "center">🌟LilyGo LoRa Series🌟</h1>

## **[English](./README.MD) | 中文**

<h3 align = "left">快速开始:</h3>

1. 将 `libdeps` 目录中所有文件夹复制到`~/Arduino/libraries`目录内,Windons用户放在 `"我的文档/Arduino/libraries"`
2. 打开对应的示例
3. 板子选择`TTGO T-Beam`
4. 在端口中选择板子的端口
5. 默认版本为1.0/1.1,如果购买的版本是v0.7，请在示例`utilities.h`中 注释掉 `T_BEAM_V10` ,并且取消`T_BEAM_V07`的注释,不清楚版本，请询问客服,或者查看板子上的丝印
6. 上传


<h3 align = "left">Product 📷:</h3>

|          Product          |                           Product  Link                           |
| :-----------------------: | :---------------------------------------------------------------: |
| [T-Beam SX1278/SX1276 ]() |  [Product link](https://pt.aliexpress.com/item/32967228739.html)  |
|     [T-Beam SX1262]()     | [Product link](https://pt.aliexpress.com/item/4001287221970.html) |



<h3 align = "left">Application :</h3>

- [T-Beam SoftRF](https://github.com/lyusupov/SoftRF)
- [T-Beam Paxcounter](https://github.com/cyberman54/ESP32-Paxcounter)
- [T-Beam Meshtastic](https://github.com/meshtastic/Meshtastic-device)

<h3 align = "left">Datasheet :</h3>

- [AXP192](http://www.x-powers.com/en.php/Info/product_detail/article_id/29)
- [SX1262 LoRa](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1262)
- [SX1278 LoRa](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278)
- [SX1276 LoRa](https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1276)
- [GSP NEO-6/NEO-8](https://www.u-blox.com/en/product/neo-6-series)
- [GPS Air530]()

<h3 align = "left">Schematic :</h3>

- [TBeam_V0.7 Schematic](schematic/LilyGo_TBeam_V0.7.pdf)
- [TBeam_V1.0 Schematic](schematic/LilyGo_TBeam_V1.0.pdf)
- [TBeam_V1.1 Schematic](schematic/LilyGo_TBeam_V1.1.pdf)
  

<h3 align = "left">PinOut :</h3>


| Pins        | T_BEAM(AXP192) | T_BEAM_V07 |
| ----------- | -------------- | ---------- |
| LORA_SCK    | 5              | 5          |
| LORA_MISO   | 19             | 19         |
| LORA_MOSI   | 27             | 27         |
| LORA_SS     | 18             | 18         |
| LORA_DIO0   | 26             | 26         |
| LORA_DIO1   | 33             | 33         |
| LORA_DIO2   | 32             | 32         |
| LORA_RST    | 23             | 23         |
| GPS_RX_PIN  | 34             | 12         |
| GPS_TX_PIN  | 12             | 15         |
| I2C_SDA     | 21             | 21         |
| I2C_SCL     | 22             | 22         |
| PMU_IRQ     | 35             | N/A        |
| USER BUTTON | 36             | N/A        |


<h3 align = "left">Power Control Channel :</h3>

| Modules | T_BEAM_V10/V1.1 | T_BEAM_V07 |
| ------- | --------------- | ---------- |
| GPS     | LDO3            | No supoort |
| LORA    | LDO2            | No supoort |
| OLED    | DCDC1           | No supoort |




