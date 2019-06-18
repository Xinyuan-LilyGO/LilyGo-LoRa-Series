TTGO-T-Beam
=====================


![](images/1.png)


## PinOut
| Pins        | T_BEAM(AXP192) | T_BEAM_V07 |
| ----------- | -------------- | ---------- |
| LORA_SCK    | 5              | 5          |
| LORA_MISO   | 19             | 19         |
| LORA_MOSI   | 27             | 27         |
| LORA_SS     | 18             | 18         |
| LORA_DI0    | 26             | 26         |
| LORA_RST    | 23             | 23         |
| GPS_RX_PIN  | 34             | 12         |
| GPS_TX_PIN  | 12             | 15         |
| I2C_SDA     | 21             | 21         |
| I2C_SCL     | 22             | 22         |
| PMU_IRQ     | 35             | N/A        |
| USER BUTTON | 36             | N/A        |

## Power Control Channel
| Modules | T_BEAM_V10 | T_BEAM_V07 |
| ------- | ---------- | ---------- |
| GPS     | LDO3       | No supoort |
| LORA    | LDO2       | No supoort |
| OLED    | DCDC1      | No supoort |


### Button Map

| USB | BTN1            | BTN2        | BTN3  |
| --- | --------------- | ----------- | ----- |
| N/A | axp192 powerkey | user button | reset |


##  Button Function:
- Click button 2 will cycle the motherboard function, the default start to enable GPS positioning, the second click will be set to lora send, the third click will be set to lora receive, long press for two seconds and then release, will enter deep sleep

## About OLED:
- The boot will scan the I2C device. If the OLED is found, all log information will be displayed in the OLED. If not, it will be printed on the serial port.

## How to use
- The repository depends on other library files, please recursively clone the repository
    ```
    git clone --recursive https://github.com/Xinyuan-LilyGO/TTGO-T-Beam.git
    ```

- Then move the library files in the `libraries` directory to
    ```
    C:\Users\<UserName>\Documents\Arduino\libraries
    ```
- In the Arduino board select `T-Beam`