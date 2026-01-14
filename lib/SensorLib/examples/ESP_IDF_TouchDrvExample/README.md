# ESP-IDF SensorLib TouchDrv examples

```bash
The current example only writes the CSTxxx series touch application. 
```

## Configure the Project

Open the project configuration menu (`idf.py menuconfig`).


In the `SensorLib Example Configuration` menu:

* Select the communication method, will interface, callback interface, LL new version interface
    1. Implemented using built-in read and write methods (Read and write methods are provided internally by SensorLib, supporting high version esp-idf >= 5.0, and low version methods (< 5.0))
    2. Implemented using read and write callback methods (Implemented using externally provided methods, suitable for multiple platforms)
* In `Sensor SCL GPIO Num` select the clock pin to connect to the PMU,the default is 7
* In `Sensor SDA GPIO Num` select the data pin connected to the PMU,the default is 6
* Select the interrupt pin connected to the PMU in `Sensor Interrupt Pin`, the default is 8
* Select `Sensor reset Pin` , the default is 17
* `Master Frequency` The maximum communication frequency defaults to 100000HZ, and you can decide whether to change it according to the situation.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`. default use **esp32s3**

## Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The output information is to configure the output voltage and enable status of the PMU

```bash
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2810,len:0x178c
load:0x403c8700,len:0x4
load:0x403c8704,len:0xc10
load:0x403cb700,len:0x2dac
entry 0x403c8904
I (26) boot: ESP-IDF v5.3-beta1-105-g3f632df143-dirt 2nd stage bootloader
I (27) boot: compile time Jan 23 2025 16:49:34
I (27) boot: Multicore bootloader
I (27) boot: chip revision: v0.2
I (27) boot.esp32s3: Boot SPI Speed : 80MHz
I (28) boot.esp32s3: SPI Mode       : DIO
I (28) boot.esp32s3: SPI Flash Size : 2MB
I (28) boot: Enabling RNG early entropy source...
I (28) boot: Partition Table:
I (28) boot: ## Label            Usage          Type ST Offset   Length
I (29) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (29) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (30) boot:  2 factory          factory app      00 00 00010000 00100000
I (30) boot: End of partition table
I (30) esp_image: segment 0: paddr=00010020 vaddr=3c030020 size=0ca94h ( 51860) map
I (40) esp_image: segment 1: paddr=0001cabc vaddr=3fc92e00 size=02a38h ( 10808) load
I (43) esp_image: segment 2: paddr=0001f4fc vaddr=40374000 size=00b1ch (  2844) load
I (44) esp_image: segment 3: paddr=00020020 vaddr=42000020 size=21020h (135200) map
I (69) esp_image: segment 4: paddr=00041048 vaddr=40374b1c size=0e254h ( 57940) load
I (88) boot: Loaded app from partition at offset 0x10000
I (88) boot: Disabling RNG early entropy source...
I (88) cpu_start: Multicore app
I (98) cpu_start: Pro cpu start user code
I (98) cpu_start: cpu freq: 240000000 Hz
I (98) app_init: Application information:
I (98) app_init: Project name:     ESP_IDF_TouchDrvExample
I (98) app_init: App version:      v0.2.5-8-gde9a1d2-dirty
I (99) app_init: Compile time:     Jan 23 2025 16:51:47
I (99) app_init: ELF file SHA256:  179a53fa8...
I (99) app_init: ESP-IDF:          v5.3-beta1-105-g3f632df143-dirt
I (99) efuse_init: Min chip rev:     v0.0
I (99) efuse_init: Max chip rev:     v0.99
I (100) efuse_init: Chip rev:         v0.2
I (100) heap_init: Initializing. RAM available for dynamic allocation:
I (100) heap_init: At 3FC96158 len 000535B8 (333 KiB): RAM
I (100) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (101) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (101) heap_init: At 600FE100 len 00001EE8 (7 KiB): RTCRAM
I (102) spi_flash: detected chip: winbond
I (102) spi_flash: flash io: dio
W (102) spi_flash: Detected size(16384k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (103) sleep: Configure to isolate all GPIO pins in sleep state
I (103) sleep: Enable automatic switching of GPIO sleep configuration
I (104) main_task: Started on CPU0
I (114) main_task: Calling app_main()
I (114) I2C: Implemented using read and write callback methods (Use higher version >= 5.0 API)
I (114) gpio: GPIO[6]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (114) gpio: GPIO[7]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (114) main: I2C initialized successfully
I (114) TOUCH: ----SensorLib TouchDrv Examples----
Scand I2C Devices:
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- 5a -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- 6a -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --



I (224) TOUCH: Find touch address 0x5A
I (224) TOUCH: Implemented using built-in read and write methods (Use higher version >= 5.0 API)
I (224) SensorLib: Using ESP-IDF Driver interface.
I (224) SensorLib: Added Device Address : 0x5A  New Dev Address: 0x3fc9a3a0 Speed :400000 
I (224) TOUCH: Initializing the capacitive touch screen successfully
I (224) TOUCH: Using CST226SE model
I (224) gpio: GPIO[8]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (224) main: Run...
I (224) main_task: Returned from app_main()
X[0]:143  Y[0]:167 
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
X[0]:143  Y[0]:167
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
cd SensorLib/examples/ESP_IDF_TouchDrvExample

Configure SDA,SCL,INT,RST,Pin
...

idf.py menuconfig
idf.py build
idf.py -b 921600 flash
idf.py monitor

```
