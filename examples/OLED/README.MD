

  

## TBEAM-S3-CORE

- `TBEAM-S3-CORE` uses `SH1106` by default. Please do not run the `SSD1306` example on `TBEAM-S3-CORE`
- For more examples, please refer to U8g2/examples

## TBEAM-V1.x

- `TBEAM-V1.x` The SSD1306 driver screen is used by default. If the SH1106 screen is used, please comment cancellation
    ```
    // #define DISPLAY_ MODEL               U8G2_ SH1106_ 128X64_ NONAME_ F_ HW_ I2C
    ```
    

## Other model uses SSD1306 display model

- [esp8266-oled-ssd1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)
- For more examples, please refer to esp8266-oled-ssd1306/examples