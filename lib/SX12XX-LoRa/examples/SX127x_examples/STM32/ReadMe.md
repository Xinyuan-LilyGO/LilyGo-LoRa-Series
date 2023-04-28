## STM32 - SX127X Library Example Programs

I decided to check if my SX127X library and examples would work with a STM32 board. When I require a micro controller that needs more pins or memory than a ATmega328P, my goto processor has been the ATMeg1284P. This has 128k of flash memory and 16K of RAM, so 4 times the flash and 8 times the RAM of an ATmega328P based Pro Mini. The 1284P still only runs at 8Mhz (3.3V) which can be considered slow in a modern context and more non-pinchange interrupts would be a definite advantage, so I decided to give a STM32 a go. With the STM32 there is the potential to use devices with much greater speed, flash, RAM and IO pins.

I chose an XNucleo F103RB since that will easily take one of my Arduino shields that uses breadboard friendly modules, so it would simple and quick to test out the SX127X lora library examples. The board is pictured below;


![Picture 1](/pictures/STM32_and_Shield.jpg)

The slim board at the top right of the picture is the supplied STLink programmer. 

I installed the STM32 core for Arduino;

[https://github.com/stm32duino/Arduino/_Core/_STM32](https://github.com/stm32duino/Arduino_Core_STM32)

Plugged in my Arduino shield, inserted the STlink programmer supplied with the board (see picture above) and connected the 'USB to USART' port to my PC so that I could monitor Serial Monitor stuff. 

I loaded the **1\_LED\_Blink\_STM32** program, then set-up the Arduino IDE as in the picture below;

Arduino_IDE_for _STM32

![Picture 1](/pictures/Arduino_IDE_for_STM32.jpg)

The blink program loaded and worked just fine. 

Next the **2\_Register\_Test\_STM32** was loaded and that worked too. 

Then the **3\_LoRa\_Transmitte\r_STM32** program was tried. I connected up my Realtek SDR and SDR#\Airspy combo and I could see lora packets being sent at 434Mhz. Hooked up another ATMega board as a receiver and it showed packets were being sent and received correctly.

I changed the XNucleo board over to the **4\_LoRa\_Receiver\_STM32** program and that could receive lora packets OK. 

In conclusion, the SX127X example lora transmit and receive programs worked on the STM32 with no changes needed.  


 **Stuart Robinson**

 **February 2020**
 