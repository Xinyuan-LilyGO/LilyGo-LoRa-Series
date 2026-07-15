
<div align="center" markdown="1">
  <img src="../.github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align="center">LilyGo LoRa 工厂固件</h1>

[English](README.md)

## 目录结构

```
firmware/
├── 蓝牙服务器
│   ├── esp32-ble-server-20241209_0x0.bin
│   └── esp32s3-ble-server-20241209_0x0.bin
│
├── T3 v1.3 (ESP32)
│   ├── lora-v1.3-868mhz-reciver-20260715_0x0.bin
│   ├── lora-v1.3-868mhz-sender-20260715_0x0.bin
│   ├── lora-v1.3-915mhz-reciver-20260715_0x0.bin
│   └── lora-v1.3-915mhz-sender-20260715_0x0.bin
│
├── T3 v1.6.1 (ESP32)
│   ├── lora-v1.6.1-433mhz-reciver-20260715_0x0.bin
│   ├── lora-v1.6.1-433mhz-sender-20260715_0x0.bin
│   ├── lora-v1.6.1-868mhz-paxcounter-20220919_0x0.bin
│   ├── lora-v1.6.1-868mhz-reciver-20260715_0x0.bin
│   ├── lora-v1.6.1-868mhz-sender-20260715_0x0.bin
│   ├── lora-v1.6.1-915mhz-reciver-20260715_0x0.bin
│   └── lora-v1.6.1-915mhz-sender-20260715_0x0.bin
│
├── T3 C6 (ESP32-C6)
│   ├── t3c6-v1.0-850mhz-reciver-20260715_0x0.bin
│   └── t3c6-v1.0-850mhz-sender-20260715_0x0.bin
│
├── T3 S3 (ESP32-S3)
│   ├── t3s3-v1.x-2400mhz-sx1280-factory-20241218_0x0.bin
│   ├── t3s3-v1.x-2400mhz-sx1280pa-factory-20241218_0x0.bin
│   ├── t3s3-v1.x-all-band-lr1121-factory_20250425_0x0.bin
│   ├── t3s3-v1.x-all-band-lr1121pa-20260715_0x0.bin
│   ├── t3s3-v1.x-all-band-sx1262-factory_20250425_0x0.bin
│   ├── t3s3-v1.x-all-band-sx1276-factory_20250425_0x0.bin
│   ├── t3s3-v1.x-all-band-sx1278-factory_20250425_0x0.bin
│   ├── t3s3-v1.x-all-band-sx1280-factory_20250425_0x0.bin
│   └── t3s3-v1.x-all-band-sx1280pa-factory_20250425_0x0.bin
│
├── T3 TCXO (ESP32)
│   ├── t3txco-v3.0-868mhz-reciver-20260715_0x0.bin
│   └── t3txco-v3.0-868mhz-sender-20260715_0x0.bin
│
├── T-Beam (ESP32)
│   ├── tbeam-v1.x-all-band-sx1262-factory-20260105_0x0.bin
│   ├── tbeam-v1.x-all-band-sx1262-factory-20260715_0x0.bin
│   ├── tbeam-v1.x-all-band-sx1276-factory-20260105_0x0.bin
│   ├── tbeam-v1.x-all-band-sx1276-factory-20260715_0x0.bin
│   ├── tbeam-v1.x-all-band-sx1278-factory-20260105_0x0.bin
│   └── tbeam-v1.x-all-band-sx1278-factory-20260715_0x0.bin
│
├── T-Beam-1W (ESP32-S3)
│   ├── tbeam1w-v1.x-all-band-lr1121-factory-20260715_0x0.bin
│   ├── tbeam1w-v1.x-all-band-lr2021-factory-20260715_0x0.bin
│   └── tbeam1w-v1.x-all-band-sx1262-factory-20260715_0x0.bin
│
├── T-Beam-BPF (ESP32-S3)
│   └── tbeambpf-v1.x-144mhz-factory-20260715_0x0.bin
│
└── T-Beam-S3-Supreme (ESP32-S3)
    ├── tbeam-s3-supreme-v2.7.23-meshtastic-b246bcd-factory-20260506_0x0.bin
    ├── tbeam-s3-supreme-v3.x-all-band-lr1121-factory-20260715_0x0.bin
    └── tbeam-s3-supreme-v3.x-all-band-sx1262-factory-20260715_0x0.bin
```

## 固件命名规范

固件文件名遵循以下命名规范：

```
[产品型号]-[版本号]-[频率/频段]-[芯片型号]-[功能类型]-[日期]_[烧录地址].bin
```

### 命名字段说明

| 字段 | 说明 | 示例 |
|------|------|------|
| **产品型号** | 设备型号标识 | `t3s3`, `tbeam`, `t3c6`, `t3txco` |
| **版本号** | 硬件版本号 | `v1.x`, `v1.0`, `v3.0` |
| **频率/频段** | 工作频率或频段 | `868mhz`, `915mhz`, `all-band`, `144mhz` |
| **芯片型号** | LoRa芯片型号 | `sx1262`, `sx1276`, `lr1121`, `lr2021` |
| **功能类型** | 固件功能 | `factory`, `sender`, `reciver`, `meshtastic`, `paxcounter` |
| **日期** | 编译日期 | `20260715` (YYYYMMDD格式) |
| **烧录地址** | 烧录到Flash的起始地址 | `0x0` |

### 常见功能类型

- **factory**: 工厂测试固件，用于验证硬件功能
- **sender**: LoRa发送端固件
- **reciver**: LoRa接收端固件
- **meshtastic**: Meshtastic固件
- **paxcounter**: 人流量计数器固件

### 常见频段

- **868mhz**: 欧洲频段
- **915mhz**: 美洲频段
- **433mhz**: 亚洲频段
- **all-band**: 多频段支持
- **2400mhz**: 2.4GHz频段

## 蓝牙固件

### 蓝牙功能测试固件

- **esp32-ble-server-20241209_0x0.bin**: 适用于ESP32，烧录地址0x0
- **esp32s3-ble-server-20241209_0x0.bin**: 适用于ESP32-S3，烧录地址0x0

### 测试方法

1. 下载并安装 [nRF Connect](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile) (Google Play)
2. 使用nRF Connect扫描蓝牙设备
3. 如果发现名为 **LilyGo-BLE** 的设备，说明蓝牙功能正常

## 如何烧录固件？

### 烧录前准备 - 进入下载模式

1. 使用USB线连接开发板
2. 按住 **BOOT** 按钮（如果没有BOOT按钮，需要使用导线连接 **GND** 和 **IO0**）
3. 按下 **RST** 按钮
4. 松开 **RST** 按钮
5. 松开 **BOOT** 按钮（如果没有BOOT按钮，断开IO0与GND的连接）
6. 上传固件
7. 按下 **RST** 按钮退出下载模式

### 方法 1: LILYGO Spark

1. 下载 [LILYGO Spark](https://lilygo.cc/pages/lilygo-spark)

![](./images/LILYGO-Spark.png)

### 方法二：使用ESP Download Tool

1. 下载 [Flash_download_tool](https://www.espressif.com.cn/sites/default/files/tools/flash_download_tool_3.9.6_0.zip)

> **注意**: 以下GIF演示的是ESP32S3的烧录过程。如果您使用的是ESP32，请选择ESP32而不是ESP32S3。

![ESP Download Tool](./images/esp_downloader.gif)

**烧录完成后需要按RST按钮进行复位**

### 方法三：使用Web Flasher

- [ESP Web Flasher在线工具](https://espressif.github.io/esptool-js/)

![Web Flasher](./images/web_flasher.gif)

**烧录完成后需要按RST按钮进行复位**

### 方法四：使用命令行

如果系统提示安装开发工具，请先完成安装：

```bash
python3 -m pip install --upgrade pip
python3 -m pip install esptool
```

启动esptool.py：

```bash
python3 -m esptool
```

#### ESP32 烧录命令

```bash
esptool --chip esp32 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin
```

#### ESP32-S3 烧录命令

```bash
esptool --chip esp32s3 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin
```

## 相关资源

- [乐鑫官方文档](https://docs.espressif.com/projects/esptool/)
