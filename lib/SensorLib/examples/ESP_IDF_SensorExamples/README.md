# ESP-IDF Sensor hub examples

## Configure the Project

Open the project configuration menu (`idf.py menuconfig`).

In the `SensorLib Example Configuration` menu:

* Select Sensor Type , Different sensors have different transmission methods, I2C, SPI
  - BHI260 Sensor   (I2C & SPI)
  - PCF8563         (I2C Only)
  - BMA423          (I2C Only)
  - FT636X          (I2C Only)
  - XL9555          (I2C Only)
  - The other models are the same
* If you choose a sensor with I2C communication, there will be three methods to choose from:
* Configuring the sensor pins

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`. default use **esp32**

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The output information is to configure the output voltage and enable status of the Sensor

### LL Driver

```bash
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:7176
load:0x40078000,len:15564
ho 0 tail 12 room 4
load:0x40080400,len:4
0x40080400: _init at ??:?

load:0x40080404,len:3904
entry 0x40080640
I (30) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (30) boot: compile time Jan 22 2025 17:30:49
I (33) boot: Multicore bootloader
I (37) boot: chip revision: v1.0
I (41) boot.esp32: SPI Speed      : 40MHz
I (45) boot.esp32: SPI Mode       : DIO
I (50) boot.esp32: SPI Flash Size : 2MB
I (54) boot: Enabling RNG early entropy source...
I (60) boot: Partition Table:
I (63) boot: ## Label            Usage          Type ST Offset   Length
I (71) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (78) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (85) boot:  2 factory          factory app      00 00 00010000 00100000
I (93) boot: End of partition table
I (97) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=0d54ch ( 54604) map
I (124) esp_image: segment 1: paddr=0001d574 vaddr=3ffb0000 size=022b8h (  8888) load
I (128) esp_image: segment 2: paddr=0001f834 vaddr=40080000 size=007e4h (  2020) load
I (131) esp_image: segment 3: paddr=00020020 vaddr=400d0020 size=1c450h (115792) map
I (178) esp_image: segment 4: paddr=0003c478 vaddr=400807e4 size=0bf98h ( 49048) load
I (204) boot: Loaded app from partition at offset 0x10000
I (204) boot: Disabling RNG early entropy source...
I (216) cpu_start: Multicore app
I (224) cpu_start: Pro cpu start user code
I (224) cpu_start: cpu freq: 240000000 Hz
I (224) app_init: Application information:
I (227) app_init: Project name:     ESP_IDF_TouchDrv_Example
I (234) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (240) app_init: Compile time:     Jan 22 2025 17:32:37
I (246) app_init: ELF file SHA256:  ef2e805bc...
I (251) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (258) efuse_init: Min chip rev:     v0.0
I (263) efuse_init: Max chip rev:     v3.99
I (268) efuse_init: Chip rev:         v1.0
I (273) heap_init: Initializing. RAM available for dynamic allocation:
I (280) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (286) heap_init: At 3FFB2BD0 len 0002D430 (181 KiB): DRAM
I (292) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (299) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (305) heap_init: At 4008C77C len 00013884 (78 KiB): IRAM
I (313) spi_flash: detected chip: winbond
I (316) spi_flash: flash io: dio
W (320) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (334) main_task: Started on CPU0
I (344) main_task: Calling app_main()
I (344) I2C: Implemented using read and write callback methods (Use higher version >= 5.0 API)
W (344) i2c.master: Please check pull-up resistances whether be connected properly. Otherwise unexpected behavior would happen. For more detailed information, please read docs
I (364) gpio: GPIO[21]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 0| Pulldown: 0| Intr:0 
I (374) gpio: GPIO[22]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 0| Pulldown: 0| Intr:0
I (384) main: I2C initialized successfully
Scan I2C Devices:
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- 19 -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- 35 -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- 51 -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --



I (434) RTC: ----DRIVER PCF8563 ----
I (434) RTC: Implemented using built-in read and write methods (Use higher version >= 5.0 API)
Using ESP-IDF Driver interface.Added Device Address : 0x51  New Dev Address: 0x3ffb4f9c Speed :400000 I (444) RTC: Initializing PCF8563 real-time clock successfully!
I (464) BMA: ----DRIVER BMA423----
I (464) BMA: Implemented using built-in read and write methods (Use higher version >= 5.0 API)
Using ESP-IDF Driver interface.Added Device Address : 0x19  New Dev Address: 0x3ffb4ffc Speed :400000 No need configure!I (474) BMA: Initialization of BMA423 accelerometer is successful!
I (494) main: Run...
I (494) main_task: Returned from app_main()
I (494) RTC: Friday, January 17 2025 04:30:30
I (504) RTC: Jan 17 2025 04:30:30
I (504) RTC: Friday, 17. January 2025 04:30AM
I (514) BMA423: Temperature:38.00*C
I (514) BMA423: Temperature:100.40*F
I (524) BMA423: Direction:5
I (1524) RTC: Friday, January 17 2025 04:30:31
I (1524) RTC: Jan 17 2025 04:30:31
I (1524) RTC: Friday, 17. January 2025 04:30AM
I (1524) BMA423: Temperature:38.00*C
I (1524) BMA423: Temperature:100.40*F
I (1534) BMA423: Direction:5
I (2534) RTC: Friday, January 17 2025 04:30:32
I (2534) RTC: Jan 17 2025 04:30:32
I (2534) RTC: Friday, 17. January 2025 04:30AM
I (2534) BMA423: Temperature:38.00*C
I (2534) BMA423: Temperature:100.40*F
I (2544) BMA423: Direction:5
I (3544) RTC: Friday, January 17 2025 04:30:33
I (3544) RTC: Jan 17 2025 04:30:33
I (3544) RTC: Friday, 17. January 2025 04:30AM
I (3544) BMA423: Temperature:38.00*C
I (3544) BMA423: Temperature:100.40*F
```

### Legacy Driver

```bash
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:7176
load:0x40078000,len:15564
ho 0 tail 12 room 4
load:0x40080400,len:4
0x40080400: _init at ??:?

load:0x40080404,len:3904
entry 0x40080640
I (30) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (31) boot: compile time Jan 22 2025 17:30:49
I (33) boot: Multicore bootloader
I (37) boot: chip revision: v1.0
I (41) boot.esp32: SPI Speed      : 40MHz
I (45) boot.esp32: SPI Mode       : DIO
I (50) boot.esp32: SPI Flash Size : 2MB
I (54) boot: Enabling RNG early entropy source...
I (60) boot: Partition Table:
I (63) boot: ## Label            Usage          Type ST Offset   Length
I (71) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (78) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (86) boot:  2 factory          factory app      00 00 00010000 00100000
I (93) boot: End of partition table
I (97) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=0d200h ( 53760) map
I (124) esp_image: segment 1: paddr=0001d228 vaddr=3ffb0000 size=022e8h (  8936) load
I (128) esp_image: segment 2: paddr=0001f518 vaddr=40080000 size=00b00h (  2816) load
I (131) esp_image: segment 3: paddr=00020020 vaddr=400d0020 size=1b71ch (112412) map
I (177) esp_image: segment 4: paddr=0003b744 vaddr=40080b00 size=0ccd8h ( 52440) load
I (204) boot: Loaded app from partition at offset 0x10000
I (205) boot: Disabling RNG early entropy source...
I (216) cpu_start: Multicore app
I (225) cpu_start: Pro cpu start user code
I (225) cpu_start: cpu freq: 240000000 Hz
I (225) app_init: Application information:
I (228) app_init: Project name:     ESP_IDF_SensorPCF8563
I (234) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (240) app_init: Compile time:     Jan 22 2025 17:47:23
I (246) app_init: ELF file SHA256:  0fcff42da...
I (251) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (258) efuse_init: Min chip rev:     v0.0
I (263) efuse_init: Max chip rev:     v3.99
I (268) efuse_init: Chip rev:         v1.0
I (273) heap_init: Initializing. RAM available for dynamic allocation:
I (280) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (286) heap_init: At 3FFB2BF0 len 0002D410 (181 KiB): DRAM
I (292) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (299) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (305) heap_init: At 4008D7D8 len 00012828 (74 KiB): IRAM
I (313) spi_flash: detected chip: winbond
I (316) spi_flash: flash io: dio
W (320) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
W (333) i2c: This driver is an old driver, please migrate your application code to adapt `driver/i2c_master.h`
I (344) main_task: Started on CPU0
I (354) main_task: Calling app_main()
I (354) I2C: Implemented using read and write callback methods (Use lower version < 5.0 API)
I (354) main: I2C initialized successfully
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- 19 -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- 35 -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- 51 -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
I (404) RTC: ----DRIVER PCF8563 ----
I (404) RTC: Implemented using built-in read and write methods (Use lower version < 5.0 API)
I (414) RTC: Initializing PCF8563 real-time clock successfully!
I (424) BMA: ----DRIVER BMA423----
I (424) BMA: Implemented using built-in read and write methods (Use lower version < 5.0 API)
I (444) BMA: Initialization of BMA423 accelerometer is successful!
I (444) main: Run...
I (444) main_task: Returned from app_main()
I (454) RTC: Friday, January 17 2025 04:30:30
I (454) RTC: Jan 17 2025 04:30:30
I (464) RTC: Friday, 17. January 2025 04:30AM
I (464) BMA423: Temperature:37.00*C
I (474) BMA423: Temperature:98.60*F
I (474) BMA423: Direction:5
I (1474) RTC: Friday, January 17 2025 04:30:31
I (1474) RTC: Jan 17 2025 04:30:31
I (1474) RTC: Friday, 17. January 2025 04:30AM
I (1474) BMA423: Temperature:37.00*C
I (1474) BMA423: Temperature:98.60*F
I (1484) BMA423: Direction:5
```

### Use Read/Write/Hal Callback

```bash
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:7176
load:0x40078000,len:15564
ho 0 tail 12 room 4
load:0x40080400,len:4
0x40080400: _init at ??:?

load:0x40080404,len:3904
entry 0x40080640
I (30) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (31) boot: compile time Jan 22 2025 17:30:49
I (33) boot: Multicore bootloader
I (37) boot: chip revision: v1.0
I (41) boot.esp32: SPI Speed      : 40MHz
I (45) boot.esp32: SPI Mode       : DIO
I (50) boot.esp32: SPI Flash Size : 2MB
I (54) boot: Enabling RNG early entropy source...
I (60) boot: Partition Table:
I (63) boot: ## Label            Usage          Type ST Offset   Length
I (71) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (78) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (86) boot:  2 factory          factory app      00 00 00010000 00100000
I (93) boot: End of partition table
I (97) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=0d2b0h ( 53936) map
I (124) esp_image: segment 1: paddr=0001d2d8 vaddr=3ffb0000 size=022e8h (  8936) load
I (128) esp_image: segment 2: paddr=0001f5c8 vaddr=40080000 size=00a50h (  2640) load
I (131) esp_image: segment 3: paddr=00020020 vaddr=400d0020 size=1b97ch (113020) map
I (177) esp_image: segment 4: paddr=0003b9a4 vaddr=40080a50 size=0cd88h ( 52616) load
I (205) boot: Loaded app from partition at offset 0x10000
I (205) boot: Disabling RNG early entropy source...
I (216) cpu_start: Multicore app
I (225) cpu_start: Pro cpu start user code
I (225) cpu_start: cpu freq: 240000000 Hz
I (225) app_init: Application information:
I (228) app_init: Project name:     ESP_IDF_SensorPCF8563
I (234) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (240) app_init: Compile time:     Jan 22 2025 18:19:56
I (246) app_init: ELF file SHA256:  952e2e97b...
I (252) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (259) efuse_init: Min chip rev:     v0.0
I (263) efuse_init: Max chip rev:     v3.99
I (268) efuse_init: Chip rev:         v1.0
I (273) heap_init: Initializing. RAM available for dynamic allocation:
I (281) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (286) heap_init: At 3FFB2BF0 len 0002D410 (181 KiB): DRAM
I (293) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (299) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (306) heap_init: At 4008D7D8 len 00012828 (74 KiB): IRAM
I (313) spi_flash: detected chip: winbond
I (316) spi_flash: flash io: dio
W (320) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
W (333) i2c: This driver is an old driver, please migrate your application code to adapt `driver/i2c_master.h`
I (345) main_task: Started on CPU0
I (355) main_task: Calling app_main()
I (355) I2C: Implemented using read and write callback methods (Use lower version < 5.0 API)
I (355) main: I2C initialized successfully
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- 19 -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- 35 -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- 51 -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
I (405) RTC: ----DRIVER PCF8563 ----
I (405) RTC: Implemented using read and write callback methods
I (415) RTC: Initializing PCF8563 real-time clock successfully!
I (425) BMA: ----DRIVER BMA423----
I (425) BMA: Implemented using read and write callback methods
I (435) BMA: Initialization of BMA423 accelerometer is successful!
I (445) main: Run...
I (445) main_task: Returned from app_main()
I (445) RTC: Friday, January 17 2025 04:30:30
I (455) RTC: Jan 17 2025 04:30:30
I (455) RTC: Friday, 17. January 2025 04:30AM
I (465) BMA423: Temperature:35.00*C
I (465) BMA423: Temperature:95.00*F
I (475) BMA423: Direction:5
I (1475) RTC: Friday, January 17 2025 04:30:31
I (1475) RTC: Jan 17 2025 04:30:31
I (1475) RTC: Friday, 17. January 2025 04:30AM
I (1475) BMA423: Temperature:35.00*C
I (1475) BMA423: Temperature:95.00*F
I (1485) BMA423: Direction:5
I (2485) RTC: Friday, January 17 2025 04:30:32
I (2485) RTC: Jan 17 2025 04:30:32
I (2485) RTC: Friday, 17. January 2025 04:30AM
```

### BHI260AP SPI Interface

```bash
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2810,len:0x178c
load:0x403c8700,len:0x4
load:0x403c8704,len:0xc10
load:0x403cb700,len:0x2dac
entry 0x403c8904
I (26) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (27) boot: compile time Jan 23 2025 11:42:56
I (27) boot: Multicore bootloader
I (27) boot: chip revision: v0.1
I (27) boot.esp32s3: Boot SPI Speed : 80MHz
I (27) boot.esp32s3: SPI Mode       : DIO
I (28) boot.esp32s3: SPI Flash Size : 2MB
I (28) boot: Enabling RNG early entropy source...
I (28) boot: Partition Table:
I (28) boot: ## Label            Usage          Type ST Offset   Length
I (29) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (29) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (29) boot:  2 factory          factory app      00 00 00010000 00100000
I (30) boot: End of partition table
I (30) esp_image: segment 0: paddr=00010020 vaddr=3c030020 size=29698h (169624) map
I (61) esp_image: segment 1: paddr=000396c0 vaddr=3fc94a00 size=02b0ch ( 11020) load
I (64) esp_image: segment 2: paddr=0003c1d4 vaddr=40374000 size=03e44h ( 15940) load
I (68) esp_image: segment 3: paddr=00040020 vaddr=42000020 size=28a70h (166512) map
I (98) esp_image: segment 4: paddr=00068a98 vaddr=40377e44 size=0cb2ch ( 52012) load
I (116) boot: Loaded app from partition at offset 0x10000
I (117) boot: Disabling RNG early entropy source...
I (117) cpu_start: Multicore app
I (127) cpu_start: Pro cpu start user code
I (127) cpu_start: cpu freq: 240000000 Hz
I (127) app_init: Application information:
I (127) app_init: Project name:     ESP_IDF_SensorExamples
I (127) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (128) app_init: Compile time:     Jan 23 2025 11:44:54
I (128) app_init: ELF file SHA256:  3512ddfc8...
I (128) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (128) efuse_init: Min chip rev:     v0.0
I (128) efuse_init: Max chip rev:     v0.99
I (128) efuse_init: Chip rev:         v0.1
I (129) heap_init: Initializing. RAM available for dynamic allocation:
I (129) heap_init: At 3FC97EC0 len 00051850 (326 KiB): RAM
I (129) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (129) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (130) heap_init: At 600FE100 len 00001EE8 (7 KiB): RTCRAM
I (131) spi_flash: detected chip: generic
I (131) spi_flash: flash io: dio
W (131) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (132) sleep: Configure to isolate all GPIO pins in sleep state
I (132) sleep: Enable automatic switching of GPIO sleep configuration
I (133) main_task: Started on CPU0
I (143) main_task: Calling app_main()
I (143) BHI: ----DRIVER BHI260AP----
I (143) gpio: GPIO[36]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (153) SensorLib: BHI260/BHA260 found. Product ID read 0x89
I (583) SensorLib: Boot successful. Kernel version 5991.
I (583) SensorLib: [META EVENT WAKE UP] Firmware initialized. Firmware version 5991
I (583) SensorLib: [META EVENT] Firmware initialized. Firmware version 5991
I (943) BHI: Initialization of BHI260AP  is successful!
Product ID     : 89
Kernel version : 5991
User version   : 5991
ROM version    : 5166
Power state    : sleeping
Host interface : SPI
Feature status : 0x4a
Boot Status : 0x38:     No flash installed.     Host interface ready.   Firmware verification done. Virtual sensor list.
Sensor ID |                          Sensor Name |  ID | Ver |  Min rate |  Max rate |
----------+--------------------------------------+-----+-----+-----------+-----------|
        1 |            Accelerometer passthrough | 205 |   1 |    1.5625 |  400.0000 |
        3 |           Accelerometer uncalibrated | 203 |   1 |    1.5625 |  400.0000 |
        4 |              Accelerometer corrected | 241 |   1 |    1.5625 |  400.0000 |
        5 |                 Accelerometer offset | 209 |   1 |    1.0000 |    1.0000 |
        6 |      Accelerometer corrected wake up | 192 |   1 |    1.5625 |  400.0000 |
        7 |   Accelerometer uncalibrated wake up | 204 |   1 |    1.5625 |  400.0000 |
       10 |                Gyroscope passthrough | 207 |   1 |    1.5625 |  400.0000 |
       12 |               Gyroscope uncalibrated | 244 |   1 |    1.5625 |  400.0000 |
       13 |                  Gyroscope corrected | 243 |   1 |    1.5625 |  400.0000 |
       14 |                     Gyroscope offset | 208 |   1 |    1.0000 |    1.0000 |
       15 |                    Gyroscope wake up | 194 |   1 |    1.5625 |  400.0000 |
       16 |       Gyroscope uncalibrated wake up | 195 |   1 |    1.5625 |  400.0000 |
       28 |                       Gravity vector | 247 |   1 |    1.5625 |  400.0000 |
       29 |               Gravity vector wake up | 198 |   1 |    1.5625 |  400.0000 |
       31 |                  Linear acceleration | 246 |   1 |    1.5625 |  400.0000 |
       32 |          Linear acceleration wake up | 197 |   1 |    1.5625 |  400.0000 |
       37 |                 Game rotation vector | 252 |   1 |    1.5625 |  400.0000 |
       38 |         Game rotation vector wake up | 200 |   1 |    1.5625 |  400.0000 |
       48 |                        Tilt detector | 236 |   1 |    1.0000 |    1.0000 |
       50 |                        Step detector | 248 |   1 |    1.0000 |    1.0000 |
       52 |                         Step counter | 249 |   1 |    1.0000 |    1.0000 |
       53 |                 Step counter wake up | 231 |   1 |    0.0005 |   25.0000 |
       55 |                   Significant motion | 250 |   1 |    1.0000 |    1.0000 |
       57 |                         Wake gesture | 232 |   1 |    1.0000 |    1.0000 |
       59 |                       Glance gesture | 234 |   1 |    1.0000 |    1.0000 |
       61 |                       Pickup gesture | 233 |   1 |    1.0000 |    1.0000 |
       63 |                 Activity recognition | 235 |   1 |    1.0000 |    1.0000 |
       67 |                   Wrist tilt gesture | 162 |   1 |    1.0000 |    1.0000 |
       69 |                   Device orientation | 163 |   1 |    1.0000 |    1.0000 |
       70 |           Device orientation wake up | 164 |   1 |    1.0000 |    1.0000 |
       75 |                    Stationary detect | 161 |   1 |    1.0000 |    1.0000 |
       77 |                        Motion detect | 160 |   1 |    1.0000 |    1.0000 |
       94 |                Step detector wake up | 230 |   1 |    1.0000 |    1.0000 |
I (1293) gpio: GPIO[37]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:2
I (1293) main: Run...
I (1293) main_task: Returned from app_main()
I (1683) SensorLib: [META EVENT] Power mode changed for sensor id 1
I (1683) SensorLib: [META EVENT] Sample rate changed for sensor id 1
I (1683) SensorLib: [META EVENT] Power mode changed for sensor id 10
I (1683) SensorLib: [META EVENT] Sample rate changed for sensor id 10
I (1683) BHI: Accelerometer passthrough: x: -0.212646, y: 0.300293, z: 0.953613;
I (1683) BHI: Gyroscope passthrough: x: -0.671387, y: -0.732422, z: -0.122070;
I (2313) BHI: Accelerometer passthrough: x: -0.213379, y: 0.302734, z: 0.960205;
I (2313) BHI: Gyroscope passthrough: x: -0.610352, y: -0.671387, z: -0.061035;
I (2953) BHI: Accelerometer passthrough: x: -0.213623, y: 0.302734, z: 0.960449;
I (2953) BHI: Gyroscope passthrough: x: -0.671387, y: -0.732422, z: -0.061035;
I (3583) BHI: Accelerometer passthrough: x: -0.213379, y: 0.302979, z: 0.960205;
I (3583) BHI: Gyroscope passthrough: x: -0.610352, y: -0.610352, z: -0.061035;
I (4223) BHI: Accelerometer passthrough: x: -0.213623, y: 0.302490, z: 0.960449;
I (4223) BHI: Gyroscope passthrough: x: -0.610352, y: -0.671387, z: -0.122070;
I (4853) BHI: Accelerometer passthrough: x: -0.213867, y: 0.303467, z: 0.960205;
I (4853) BHI: Gyroscope passthrough: x: -0.610352, y: -0.671387, z: -0.122070;
```

### BHI260 I2C Interface

```bash
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2810,len:0x178c
load:0x403c8700,len:0x4
load:0x403c8704,len:0xcb8
load:0x403cb700,len:0x2d9c
entry 0x403c8914
I (26) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (27) boot: compile time Jan 23 2025 14:20:21
I (28) boot: Multicore bootloader
I (32) boot: chip revision: v0.2
I (36) boot.esp32s3: Boot SPI Speed : 80MHz
I (41) boot.esp32s3: SPI Mode       : DIO
I (45) boot.esp32s3: SPI Flash Size : 2MB
I (50) boot: Enabling RNG early entropy source...
I (55) boot: Partition Table:
I (59) boot: ## Label            Usage          Type ST Offset   Length
I (66) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (74) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (81) boot:  2 factory          factory app      00 00 00010000 00100000
I (89) boot: End of partition table
I (93) esp_image: segment 0: paddr=00010020 vaddr=3c030020 size=2abc8h (175048) map
I (133) esp_image: segment 1: paddr=0003abf0 vaddr=3fc95300 size=02bcch ( 11212) load
I (135) esp_image: segment 2: paddr=0003d7c4 vaddr=40374000 size=02854h ( 10324) load
I (141) esp_image: segment 3: paddr=00040020 vaddr=42000020 size=2b8d8h (178392) map
I (179) esp_image: segment 4: paddr=0006b900 vaddr=40376854 size=0ea24h ( 59940) load
I (199) boot: Loaded app from partition at offset 0x10000
I (200) boot: Disabling RNG early entropy source...
I (211) cpu_start: Multicore app
I (220) cpu_start: Pro cpu start user code
I (221) cpu_start: cpu freq: 160000000 Hz
I (221) app_init: Application information:
I (223) app_init: Project name:     ESP_IDF_SensorExamples
I (230) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (236) app_init: Compile time:     Jan 23 2025 14:33:38
I (242) app_init: ELF file SHA256:  54e4e1748...
I (247) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (254) efuse_init: Min chip rev:     v0.0
I (259) efuse_init: Max chip rev:     v0.99
I (264) efuse_init: Chip rev:         v0.2
I (269) heap_init: Initializing. RAM available for dynamic allocation:
I (276) heap_init: At 3FC988C8 len 00050E48 (323 KiB): RAM
I (282) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (288) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (294) heap_init: At 600FE100 len 00001EE8 (7 KiB): RTCRAM
I (302) spi_flash: detected chip: winbond
I (305) spi_flash: flash io: dio
W (309) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (322) sleep: Configure to isolate all GPIO pins in sleep state
I (329) sleep: Enable automatic switching of GPIO sleep configuration
I (337) main_task: Started on CPU0
I (347) main_task: Calling app_main()
I (347) I2C: Implemented using read and write callback methods (Use higher version >= 5.0 API)
W (357) i2c.master: Please check pull-up resistances whether be connected properly. Otherwise unexpected behavior would happen. For more detailed information, please read docs
I (367) gpio: GPIO[2]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 0| Pulldown: 0| Intr:0
I (377) gpio: GPIO[3]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 0| Pulldown: 0| Intr:0 
I (387) main: I2C initialized successfully
Scan I2C Devices:
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- 28 -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- 51 -- -- -- -- -- -- -- -- 5a -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- 6b -- -- -- -- 
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --



I (437) BHI: ----DRIVER BHI260AP----
I (447) BHI: Implemented using built-in read and write methods (Use higher version >= 5.0 API)
I (457) SensorLib: Using ESP-IDF Driver interface.
I (457) SensorLib: Added Device Address : 0x28  New Dev Address: 0x3fc9cc34 Speed :400000 
I (477) SensorLib: BHI260/BHA260 found. Product ID read 0x89
I (3497) SensorLib: Boot successful. Kernel version 5991.
I (3497) SensorLib: [META EVENT WAKE UP] Firmware initialized. Firmware version 5991
I (3497) SensorLib: [META EVENT] Firmware initialized. Firmware version 5991
I (3597) BHI: Initialize BHI260AP using I2C interface
Product ID     : 89
Kernel version : 5991
User version   : 5991
ROM version    : 5166
Power state    : sleeping
Host interface : I2C
Feature status : 0x4a
Boot Status : 0x38:     No flash installed.     Host interface ready.   Firmware verification done. Virtual sensor list.
Sensor ID |                          Sensor Name |  ID | Ver |  Min rate |  Max rate |
----------+--------------------------------------+-----+-----+-----------+-----------|
        1 |            Accelerometer passthrough | 205 |   1 |    1.5625 |  400.0000 |
        3 |           Accelerometer uncalibrated | 203 |   1 |    1.5625 |  400.0000 |
        4 |              Accelerometer corrected | 241 |   1 |    1.5625 |  400.0000 |
        5 |                 Accelerometer offset | 209 |   1 |    1.0000 |    1.0000 |
        6 |      Accelerometer corrected wake up | 192 |   1 |    1.5625 |  400.0000 |
        7 |   Accelerometer uncalibrated wake up | 204 |   1 |    1.5625 |  400.0000 |
       10 |                Gyroscope passthrough | 207 |   1 |    1.5625 |  400.0000 |
       12 |               Gyroscope uncalibrated | 244 |   1 |    1.5625 |  400.0000 |
       13 |                  Gyroscope corrected | 243 |   1 |    1.5625 |  400.0000 |
       14 |                     Gyroscope offset | 208 |   1 |    1.0000 |    1.0000 |
       15 |                    Gyroscope wake up | 194 |   1 |    1.5625 |  400.0000 |
       16 |       Gyroscope uncalibrated wake up | 195 |   1 |    1.5625 |  400.0000 |
       28 |                       Gravity vector | 247 |   1 |    1.5625 |  400.0000 |
       29 |               Gravity vector wake up | 198 |   1 |    1.5625 |  400.0000 |
       31 |                  Linear acceleration | 246 |   1 |    1.5625 |  400.0000 |
       32 |          Linear acceleration wake up | 197 |   1 |    1.5625 |  400.0000 |
       37 |                 Game rotation vector | 252 |   1 |    1.5625 |  400.0000 |
       38 |         Game rotation vector wake up | 200 |   1 |    1.5625 |  400.0000 |
       48 |                        Tilt detector | 236 |   1 |    1.0000 |    1.0000 |
       50 |                        Step detector | 248 |   1 |    1.0000 |    1.0000 |
       52 |                         Step counter | 249 |   1 |    1.0000 |    1.0000 |
       53 |                 Step counter wake up | 231 |   1 |    0.0005 |   25.0000 |
       55 |                   Significant motion | 250 |   1 |    1.0000 |    1.0000 |
       57 |                         Wake gesture | 232 |   1 |    1.0000 |    1.0000 |
       59 |                       Glance gesture | 234 |   1 |    1.0000 |    1.0000 |
       61 |                       Pickup gesture | 233 |   1 |    1.0000 |    1.0000 |
       63 |                 Activity recognition | 235 |   1 |    1.0000 |    1.0000 |
       67 |                   Wrist tilt gesture | 162 |   1 |    1.0000 |    1.0000 |
       69 |                   Device orientation | 163 |   1 |    1.0000 |    1.0000 |
       70 |           Device orientation wake up | 164 |   1 |    1.0000 |    1.0000 |
       75 |                    Stationary detect | 161 |   1 |    1.0000 |    1.0000 |
       77 |                        Motion detect | 160 |   1 |    1.0000 |    1.0000 |
       94 |                Step detector wake up | 230 |   1 |    1.0000 |    1.0000 |
I (3937) gpio: GPIO[8]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:2
I (3947) main: Run...
I (3947) main_task: Returned from app_main()
I (4347) SensorLib: [META EVENT] Power mode changed for sensor id 1
I (4347) SensorLib: [META EVENT] Sample rate changed for sensor id 1
I (4347) SensorLib: [META EVENT] Power mode changed for sensor id 10
I (4357) SensorLib: [META EVENT] Sample rate changed for sensor id 10
I (4357) BHI: Accelerometer passthrough: x: 0.129395, y: 0.024658, z: 1.018799;
I (4367) BHI: Gyroscope passthrough: x: -0.305176, y: 0.183105, z: 0.183105;
I (4987) BHI: Accelerometer passthrough: x: 0.129639, y: 0.024414, z: 1.024902;
I (4987) BHI: Gyroscope passthrough: x: -0.244141, y: 0.183105, z: 0.183105;
I (5627) BHI: Accelerometer passthrough: x: 0.129639, y: 0.025391, z: 1.025635;
I (5627) BHI: Gyroscope passthrough: x: -0.305176, y: 0.183105, z: 0.183105;
I (6267) BHI: Accelerometer passthrough: x: 0.130127, y: 0.025146, z: 1.025391;
I (6267) BHI: Gyroscope passthrough: x: -0.244141, y: 0.244141, z: 0.183105;
I (6907) BHI: Accelerometer passthrough: x: 0.129883, y: 0.024902, z: 1.025635;
I (6907) BHI: Gyroscope passthrough: x: -0.305176, y: 0.244141, z: 0.244141;
I (7547) BHI: Accelerometer passthrough: x: 0.129883, y: 0.025635, z: 1.025635;
I (7547) BHI: Gyroscope passthrough: x: -0.305176, y: 0.183105, z: 0.244141;
I (8187) BHI: Accelerometer passthrough: x: 0.130127, y: 0.025146, z: 1.026123;
I (8187) BHI: Gyroscope passthrough: x: -0.305176, y: 0.244141, z: 0.183105;
I (8827) BHI: Accelerometer passthrough: x: 0.129883, y: 0.024902, z: 1.025879;
I (8827) BHI: Gyroscope passthrough: x: -0.305176, y: 0.183105, z: 0.183105;
```

### FT63XX

```bash
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:7176
load:0x40078000,len:15564
ho 0 tail 12 room 4
load:0x40080400,len:4
0x40080400: _init at ??:?

load:0x40080404,len:3904
entry 0x40080640
I (30) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (30) boot: compile time Jan 23 2025 15:30:25
I (33) boot: Multicore bootloader
I (37) boot: chip revision: v1.0
I (41) boot.esp32: SPI Speed      : 40MHz
I (45) boot.esp32: SPI Mode       : DIO
I (50) boot.esp32: SPI Flash Size : 2MB
I (54) boot: Enabling RNG early entropy source...
I (60) boot: Partition Table:
I (63) boot: ## Label            Usage          Type ST Offset   Length
I (71) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (78) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (85) boot:  2 factory          factory app      00 00 00010000 00100000
I (93) boot: End of partition table
I (97) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=0b860h ( 47200) map
I (122) esp_image: segment 1: paddr=0001b888 vaddr=3ffb0000 size=02258h (  8792) load
I (125) esp_image: segment 2: paddr=0001dae8 vaddr=40080000 size=02530h (  9520) load
I (131) esp_image: segment 3: paddr=00020020 vaddr=400d0020 size=18744h (100164) map
I (170) esp_image: segment 4: paddr=0003876c vaddr=40082530 size=0a210h ( 41488) load
I (193) boot: Loaded app from partition at offset 0x10000
I (193) boot: Disabling RNG early entropy source...
I (205) cpu_start: Multicore app
I (214) cpu_start: Pro cpu start user code
I (214) cpu_start: cpu freq: 160000000 Hz
I (214) app_init: Application information:
I (217) app_init: Project name:     ESP_IDF_SensorExamples
I (223) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (229) app_init: Compile time:     Jan 23 2025 15:44:44
I (235) app_init: ELF file SHA256:  fe7a7c460...
I (240) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (247) efuse_init: Min chip rev:     v0.0
I (252) efuse_init: Max chip rev:     v3.99
I (257) efuse_init: Chip rev:         v1.0
I (262) heap_init: Initializing. RAM available for dynamic allocation:
I (269) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (275) heap_init: At 3FFB2B78 len 0002D488 (181 KiB): DRAM
I (281) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (288) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (294) heap_init: At 4008C740 len 000138C0 (78 KiB): IRAM
I (302) spi_flash: detected chip: winbond
I (305) spi_flash: flash io: dio
W (309) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (323) main_task: Started on CPU0
I (333) main_task: Calling app_main()
I (333) I2C: Implemented using read and write callback methods (Use higher version >= 5.0 API)
I (333) gpio: GPIO[23]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (343) gpio: GPIO[32]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
I (353) main: I2C initialized successfully
Scan I2C Devices:
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- 38 -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --



I (403) FT63XX: ----DRIVER FT63XX----
I (403) FT63XX: Implemented using built-in read and write methods (Use higher version >= 5.0 API)
I (413) SensorLib: Using ESP-IDF Driver interface.
I (423) SensorLib: Added Device Address : 0x38  New Dev Address: 0x3ffb4f58 Speed :400000
I (433) SensorLib: Vend ID: 0x11
I (433) SensorLib: Chip ID: 0x64
I (443) SensorLib: Firm Version: 0x3
I (443) SensorLib: Point Rate Hz: 10
I (453) SensorLib: Thresh : 60
I (453) SensorLib: Chip library version : 0x501
I (453) SensorLib: Chip period of monitor status : 0x28
I (463) FT63XX: Initialization of FT63XX is successful!
I (473) main: Run...
I (473) main_task: Returned from app_main()
I (6733) FT63X6: Point[00] - X:113 Y:126
I (6743) FT63X6: Point[00] - X:113 Y:126
I (6753) FT63X6: Point[00] - X:113 Y:126
I (6763) FT63X6: Point[00] - X:113 Y:126
I (6773) FT63X6: Point[00] - X:113 Y:126
I (6783) FT63X6: Point[00] - X:113 Y:126
I (6793) FT63X6: Point[00] - X:113 Y:128
I (6803) FT63X6: Point[00] - X:110 Y:140
I (6813) FT63X6: Point[00] - X:109 Y:150
I (6823) FT63X6: Point[00] - X:107 Y:162
I (6833) FT63X6: Point[00] - X:105 Y:176
I (6843) FT63X6: Point[00] - X:100 Y:193
I (6853) FT63X6: Point[00] - X:92 Y:206
I (7273) FT63X6: Point[00] - X:88 Y:273
I (7283) FT63X6: Point[00] - X:88 Y:273
I (7293) FT63X6: Point[00] - X:88 Y:273
I (7303) FT63X6: Point[00] - X:90 Y:270
I (7313) FT63X6: Point[00] - X:93 Y:266
I (7323) FT63X6: Point[00] - X:99 Y:259
I (7333) FT63X6: Point[00] - X:109 Y:247
I (7343) FT63X6: Point[00] - X:118 Y:236
I (7353) FT63X6: Point[00] - X:128 Y:223
I (7363) FT63X6: Point[00] - X:138 Y:209
I (7373) FT63X6: Point[00] - X:148 Y:195
I (7383) FT63X6: Point[00] - X:158 Y:180
I (7393) FT63X6: Point[00] - X:165 Y:169
I (7403) FT63X6: Point[00] - X:170 Y:160
I (7413) FT63X6: Point[00] - X:174 Y:153
I (7423) FT63X6: Point[00] - X:177 Y:148
I (7433) FT63X6: Point[00] - X:178 Y:145
```

### XL9555

```bash
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2810,len:0x178c
load:0x403c8700,len:0x4
load:0x403c8704,len:0xcb8
load:0x403cb700,len:0x2d9c
entry 0x403c8914
I (26) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (27) boot: compile time Jan 23 2025 16:15:44
I (28) boot: Multicore bootloader
I (32) boot: chip revision: v0.1
I (36) boot.esp32s3: Boot SPI Speed : 80MHz
I (41) boot.esp32s3: SPI Mode       : DIO
I (45) boot.esp32s3: SPI Flash Size : 2MB
I (50) boot: Enabling RNG early entropy source...
I (55) boot: Partition Table:
I (59) boot: ## Label            Usage          Type ST Offset   Length
I (66) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (74) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (81) boot:  2 factory          factory app      00 00 00010000 00100000
I (89) boot: End of partition table
I (93) esp_image: segment 0: paddr=00010020 vaddr=3c030020 size=0c95ch ( 51548) map
I (110) esp_image: segment 1: paddr=0001c984 vaddr=3fc93300 size=02ad8h ( 10968) load
I (113) esp_image: segment 2: paddr=0001f464 vaddr=40374000 size=00bb4h (  2996) load
I (119) esp_image: segment 3: paddr=00020020 vaddr=42000020 size=20b8ch (134028) map
I (151) esp_image: segment 4: paddr=00040bb4 vaddr=40374bb4 size=0e6c4h ( 59076) load
I (170) boot: Loaded app from partition at offset 0x10000
I (170) boot: Disabling RNG early entropy source...
I (182) cpu_start: Multicore app
I (191) cpu_start: Pro cpu start user code
I (191) cpu_start: cpu freq: 160000000 Hz
I (192) app_init: Application information:
I (194) app_init: Project name:     ESP_IDF_SensorExamples
I (200) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (207) app_init: Compile time:     Jan 23 2025 16:28:47
I (213) app_init: ELF file SHA256:  c5b0aa4bb...
I (218) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (225) efuse_init: Min chip rev:     v0.0
I (230) efuse_init: Max chip rev:     v0.99
I (235) efuse_init: Chip rev:         v0.1
I (240) heap_init: Initializing. RAM available for dynamic allocation:
I (247) heap_init: At 3FC967E8 len 00052F28 (331 KiB): RAM
I (253) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (259) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (265) heap_init: At 600FE100 len 00001EE8 (7 KiB): RTCRAM
I (272) spi_flash: detected chip: winbond
I (276) spi_flash: flash io: dio
W (280) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (293) sleep: Configure to isolate all GPIO pins in sleep state
I (300) sleep: Enable automatic switching of GPIO sleep configuration
I (307) main_task: Started on CPU0
I (317) main_task: Calling app_main()
I (317) I2C: Implemented using read and write callback methods (Use higher version >= 5.0 API)
I (327) gpio: GPIO[8]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
I (337) gpio: GPIO[48]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (347) main: I2C initialized successfully
Scan I2C Devices:
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: 20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- 38 -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --



I (397) XL9555: ----DRIVER XL9555 ----
I (397) XL9555: Implemented using built-in read and write methods (Use higher version >= 5.0 API)
I (407) SensorLib: Using ESP-IDF Driver interface.
I (417) SensorLib: Added Device Address : 0xFF  New Dev Address: 0x3fc9aad8 Speed :400000
I (417) SensorLib: i2c_master_bus_rm_device
I (427) SensorLib: Using ESP-IDF Driver interface.
I (427) SensorLib: Added Device Address : 0x20  New Dev Address: 0x3fc9aad8 Speed :400000 
I (437) XL9555: Initializing XL9555 successfully!
I (447) main: Run...
I (447) main_task: Returned from app_main()
```

## Build process example

Assuming you don't have esp-idf yet

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
git clone https://github.com/lewisxhe/SensorLib.git
cd esp-idf
./install.sh
. ./export.sh
cd ..
cd SensorLib/examples/ESP_IDF_SensorExamples

Configure Sensor Type 
Configure I2C Pins or SPI Pins 
...
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -b 921600 flash
idf.py monitor

```
