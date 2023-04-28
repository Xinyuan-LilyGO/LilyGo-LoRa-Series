
#include "Arduino.h"
#include "esp_adc_cal.h"
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
#include "images.h"
#include "boards.h"
#include "OLEDDisplayUi.h"
#include "utilities.h"

// include the library
#include <RadioLib.h>


SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);


#define ERR_NONE 0
// save transmission state between loops
int transmissionState = RADIOLIB_ERR_NONE;
int send_num = 0;
char send_str[2] = "";
char RSSI_str[20] = "";
uint8_t LoRa_state = 1;
uint8_t SD_state = 1;
uint8_t wifi_state = 1;
uint32_t  lora_Millis = 0;
uint32_t  bat_adc_Millis = 0;
float Voltage = 0.0;
String rssi_str = "RSSI --";
String SNR_str = "SNR --";
String rx_str;

// flag to indicate that a packet was sent
volatile bool transmittedFlag = false;

//oled
SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);
OLEDDisplayUi ui     ( &display );

//wifi
#include <WiFi.h>

// const char *ssid     = "xinyuan-2";
// const char *password = "Xydz202104";
const char *ssid     = "xinyuandianzi";
const char *password = "AA15994823428";


bool readkey();
void lora_tx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void lora_rx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void hardware_state(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void setFlag(void);
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);
uint32_t readADC_Cal(int ADC_Raw);

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { hardware_state,  lora_tx, lora_rx};
// how many frames are there?
int frameCount = 3;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;




// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;
void setFlag(void)
{
    // check if the interrupt is enabled
    if (!enableInterrupt) {
        return;
    }

    // we got a packet, set the flag
    receivedFlag = true;
}


void setup()
{
    Serial.begin(115200);
    Serial.println("initBoard");

    // SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    // if (!SD.begin(SD_CS)) {
    //     Serial.println("SDCard MOUNT FAIL");
    //     SD_state = 0;
    // } else {
    //     uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    //     String str = "SDCard Size: " + String(cardSize) + "MB";
    //     Serial.println(str);
    //     SD_state = 1;
    // }

    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    if (!SD.begin(SDCARD_CS, SDSPI)) {
        Serial.println("setupSDCard FAIL");
        SD_state = 0;
    } else {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.print("setupSDCard PASS . SIZE = ");
        Serial.print(cardSize);
        Serial.println(" MB");
        SD_state = 1;
    }
    initBoard();
    delay(1500);




    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.clear();

    // display.fillRect(0, 0, 200, 200);
    // display.display();
    // while (1) {
    //     delay(200);
    // }


//     display.drawString(0, 0, "Lora 2.4G");
//     display.drawString(0, 10, "WiFi connected....");
//     display.display();
// //wifi
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED) {
//         Serial.print(".");
//         delay(500);
//     }

//     Serial.println("");
//     Serial.println("WiFi connected");
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());

    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);


    // initialize SX1280 with default settings
    Serial.print(F("[SX1280] Initializing ... "));
    int  state = radio.begin();

    if ( state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        LoRa_state = 0;
    }

    if (radio.setOutputPower(3) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        LoRa_state = 0;
    }

    // set spreading factor to 10
    if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        LoRa_state = 0;
    }

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);

    // start listening for LoRa packets
    // Serial.print(F("[SX1280] Starting to listen ... "));
    // state = radio.startReceive();
    // if (state == RADIOLIB_ERR_NONE) {
    //     Serial.println(F("success!"));
    // } else {
    //     Serial.print(F("failed, code "));
    // }
    //LoRa.onReceive(cbk);
    // register the receive callback
    // LoRa.onReceive(onReceive);

    // put the radio into receive mode
    //  LoRa.receive();

//ui
    // The ESP is capable of rendering 60fps in 80Mhz mode
    // but that won't give you much time for anything else
    // run it in 160Mhz mode or just set it to 30 fps
    ui.setTargetFPS(60);

    // Customize the active and inactive symbol
    // ui.setActiveSymbol(activeSymbol);
    // ui.setInactiveSymbol(inactiveSymbol);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    ui.setIndicatorPosition(BOTTOM);

    // Defines where the first frame is located in the bar.
    ui.setIndicatorDirection(LEFT_RIGHT);

    // You can change the transition that is used
    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
    ui.setFrameAnimation(SLIDE_LEFT);

    // Add frames
    ui.setFrames(frames, frameCount);

    // Add overlays
    ui.setOverlays(overlays, overlaysCount);

    ui.disableAutoTransition();

    // Initialising the UI will init the display too.
    ui.init();

    display.flipScreenVertically();
}

void loop()
{

    int remainingTimeBudget = ui.update();
    if (remainingTimeBudget > 0) {
        // You can do some work here
        // Don't do stuff if you are below your
        // time budget.
        delay(remainingTimeBudget);
    }
    if (readkey()) {
        ui.nextFrame();
    }
}

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{

}

bool readkey()
{
    if (!digitalRead(BUTTON_PIN)) {
        delay(50);
        if (!digitalRead(BUTTON_PIN)) {
            return true;
        }

    }
    return false;
}


void lora_tx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    int tx_state;
    // Serial.print(F("lora_tx"));
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    if (millis() - lora_Millis > 1000) {
        int state = radio.transmit(send_str);
        send_num++;
        sprintf(send_str, "%d", send_num);

        lora_Millis = millis();
    }


    display->drawString(0 + x, 0 + y, "Lora_send");
    display->drawString(0 + x, 12 + y, "TX :");
    display->drawString(30 + x, 12 + y, send_str);

}


void lora_rx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{

    display->setFont(ArialMT_Plain_10);
    // The coordinates define the left starting point of the text
    display->setTextAlignment(TEXT_ALIGN_LEFT);


    if (receivedFlag) {
        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;
        // reset flag
        receivedFlag = false;
        int state = radio.readData(rx_str);
        if (state == RADIOLIB_ERR_NONE) {
            rssi_str = "RSSI: " + String(radio.getRSSI());
            // SNR_str = "SNR: " + String(radio.getSNR());
            // print RSSI (Received Signal Strength Indicator)
            // Serial.print(F("[SX1280] RSSI:\t\t"));
            // Serial.print(radio.getRSSI());
            // Serial.println(F(" dBm"));

            // // print SNR (Signal-to-Noise Ratio)
            // Serial.print(F("[SX1280] SNR:\t\t"));
            // Serial.print(radio.getSNR());
            // Serial.println(F(" dB"));

        }

        // put module back to listen mode
        radio.startReceive();

        // we're ready to receive more packets,
        // enable interrupt service routine
        enableInterrupt = true;
    }


    display->drawString(0 + x, 0 + y, "Lora_receive");
    display->drawString(0 + x, 10 + y, rssi_str);
    // display->drawString(0 + x, 20 + y, SNR_str);
    display->drawString(0 + x, 22 + y, "RX :");
    display->drawString(30 + x, 22 + y, rx_str);




}

char Bat_str[10] = "";
void hardware_state(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{


    if (millis() - bat_adc_Millis > 1000) {
        // Serial.println(readADC_Cal(BAT_ADC));
        //
        Voltage = ((readADC_Cal(analogRead(ADC_PIN))) * 2 / 1000.0);
        Serial.printf("%.2f", Voltage); // Print Voltage (in V)
        sprintf(Bat_str, "%.2fV", Voltage);
        bat_adc_Millis = millis();
    }


    display->setFont(ArialMT_Plain_10);
    // display->drawString(0 + x, 11 + y, "Left aligned (0,10)");
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    display->drawString(0 + x, 0 + y, "SX1280  ");
    // display->drawString(50 + x, 0 + y, wifi_state & 1 ? "+" : "NA");
    display->drawString(0 + x, 10 + y, "LORA  ");
    display->drawString(50 + x, 10 + y, LoRa_state & 1 ? "+" : "NA");


    display->drawString(0 + x, 20 + y, "SD   ");
    display->drawString(50 + x, 20 + y, SD_state & 1 ? "+" : "NA");

    display->drawString(0 + x, 30 + y, "BAT   ");
    display->drawString(50 + x, 30 + y, Bat_str);

}
uint32_t readADC_Cal(int ADC_Raw)
{
    esp_adc_cal_characteristics_t adc_chars;

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    // return (((esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars)) * 2) / 1000.0);
    return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
