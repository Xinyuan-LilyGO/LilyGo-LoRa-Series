

## How to Flash ?


### Before burning firmware, please put the board into download mode and follow the steps below

 1. Connect the board via the USB cable
 2. Press and hold the **BOOT** button , While still pressing the **BOOT** button (If there is no **BOOT** button, you need to use wires to connect **GND** and **IO0** together.)
 3. Press **RST** button
 4. Release the **RST** button
 5. Release the **BOOT** button (If there is no **BOOT** button, disconnect IO0 from GND.)
 6. Upload sketch
 7. Press the **RST** button to exit download mode



### Use ESP Download Tool

- Download [Flash_download_tool](https://www.espressif.com.cn/sites/default/files/tools/flash_download_tool_3.9.6_0.zip)

![web_flasher](./images/esp_downloader.gif)

* **Note that after writing is completed, you need to press RST to reset**.

### Use Web Flasher

- [ESP Web Flasher Online](https://espressif.github.io/esptool-js/)

![web_flasher](./images/web_flasher.gif)

* **Note that after writing is completed, you need to press RST to reset**.

### Use command line


If system asks about install Developer Tools, do it.

```
python3 -m pip install --upgrade pip
python3 -m pip install esptool
```

In order to launch esptool.py, exec directly with this:

```
python3 -m esptool
```

For ESP32 use the following command to write

```
esptool --chip esp32  --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin

```


For ESP32-S3 use the following command to write

```
esptool --chip esp32s3  --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin

```
