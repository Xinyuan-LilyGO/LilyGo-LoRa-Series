# sx1302_hal.sh

## Download

download script:

```shell
curl https://raw.githubusercontent.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/master/tools/sx1302_hal.sh
```

Alternatively, download the script and execute it automatically:

```shell
curl https://raw.githubusercontent.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/master/tools/sx1302_hal.sh | sudo bash
```

## Usage

> NOTE
>
> This script requires administrator privileges.

```shell
Script to automatically deploy packet_forwarder

Usage: sudo ./sx1302_hal.sh [-s server] [-p port] [-f frequency] [--eui eui] [--verbose] [-o output]
       sudo ./sx1302_hal.sh
       ./sx1302_hal.sh -v
       ./sx1302_hal.sh --help

Option:
  -s --server  Gateway Server address. The default value is eu1.cloud.thethings.network.
  -p --port  Gateway Server port. The default value is 1700.
  -f --frequency  The frequency band of lorawan supports eu868, us915, as923. The default value is eu868.
  -eui  Gateway EUI. The default is produced by the mac address of eth0.
  -o --output  Output gateway information.

Miscellaneous options:
  --verbose  Output extra information about the work being done.

  -v --version  Output version info.
  --help  Output this help.

Report bugs to lbquue@163.com.
```

## The Things Network

The Things Network has the following three servers, just choose one of them. The port is 1700.

Europe 1:

```
eu1.cloud.thethings.network
```


North America 1:

```
nam1.cloud.thethings.network
```

Australia 1:

```
au1.cloud.thethings.network
```

## Test

| Model                                 | OS                                             | Gateway         | Status |
| ------------------------------------- | ---------------------------------------------- | --------------- | ------ |
| [Raspberry Pi 3 Model B Rev 1.2][1-1] | [Raspberry Pi OS Lite(64-bit) 2022-09-26][1-2] | [T-SX1302][1-3] | ✔     |

[1-1]: https://www.raspberrypi.com/products/raspberry-pi-3-model-b/
[1-2]: https://downloads.raspberrypi.org/raspios_lite_arm64/images/raspios_lite_arm64-2022-09-26/
[1-3]: https://www.aliexpress.us/item/3256801172821554.html

> NOTE
>
> Theoretically, other models of Raspberry Pi are also supported, welcome to submit test results!
