/**
 * @file      Factory.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2024-12-18
 * @note      This example is for T-beam V1.2, T-Beam-BPF,T-Beam-1W-xxxx, T-Beam SUPREME factory examples
 */

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SD.h>
#include <esp_adc_cal.h>
#include <SSD1306Wire.h>
#include <SH1106Wire.h>
#include "OLEDDisplayUi.h"
#include <RadioLib.h>
#include "utilities.h"
#include <AceButton.h>
#include <esp_sntp.h>
#include <WiFiMulti.h>
#include <TinyGPS++.h>
#include "Roboto_Mono_Medium_12.h"
#include "LoRaBoards.h"
#include <Preferences.h>

#ifndef WIFI_SSID
#define WIFI_SSID             "Your WiFi SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD         "Your WiFi PASSWORD"
#endif

#ifdef T_BEAM_S3_SUPREME
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SensorQMI8658.hpp>
#include <SensorQMC6310.hpp>
#include <SensorQMC6309.hpp>
#include <SensorPCF8563.hpp>
#include <MadgwickAHRS.h>       //MadgwickAHRS from https://github.com/arduino-libraries/MadgwickAHRS

MagnetometerBase  *magnetometer = nullptr;
SensorQMI8658  qmi;
SensorPCF8563  rtc;
Adafruit_BME280 bme;
#endif

Preferences preferences;
static const char *pref_key = "lora";

struct loraParams {
    float lora_freq;        ///< LoRa frequency
    float lora_bw;          ///< LoRa bandwidth
    uint8_t lora_cr;        ///< LoRa coding rate
    int8_t lora_tx_power;   ///< LoRa transmission power
    uint8_t lora_sf;        ///< LoRa spreading factor
    uint8_t lora_sw;        ///< LoRa sync word
    size_t lora_preamble_length; ///< LoRa preamble length
    bool lora_crc;          ///< LoRa CRC enable
    bool iq_inverted;       ///< LoRa IQ inversion
};

static struct loraParams lora_settings = {
    .lora_freq = CONFIG_RADIO_FREQ,
    .lora_bw = 125.0,
    .lora_cr = 6,
    .lora_tx_power = CONFIG_RADIO_SUB1G_OUTPUT_POWER,
    .lora_sf = 10,
    .lora_sw = 0x12,
    .lora_preamble_length = 15,
    .lora_crc = false,
    .iq_inverted = false
};

using namespace ace_button;

void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void radioRx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void wirelessInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void wifiTask(void *task);
void hwProbe(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void screenTest(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void listeningMode(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);

#ifdef HAS_GPS
void gpsInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);
#endif
#if defined(HAS_PMU) || defined(ADC_PIN)
void pmuInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
#endif
#ifdef T_BEAM_S3_SUPREME
void dateTimeInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);
void sensorInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);
void imuInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y);

static void setupSensor();
#else
#define setupSensor()
#endif
static void setupRfSwitch();
static bool setupLoRaParams(loraParams &params);
static void setupLoRa();
static void setupDisplay();
static void setupNetwork();
static void setupPreferences();
static void handleWomCommands();
static void printLoRaParams(loraParams params);
static bool reloadLoRa();
static void loopButton();
static void loopGPS();
static void confirmTxPower(int8_t &txPower, bool forcedHigh = false);

#if     defined(USING_SX1276)
SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);
#elif   defined(USING_SX1278)
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);
#elif   defined(USING_SX1262)
SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#elif   defined(USING_SX1280)
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#elif  defined(USING_SX1280PA)
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#elif   defined(USING_LR1121)
LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO_IRQ_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#elif   defined(USING_LR2021)
LR2021 radio = new Module(RADIO_CS_PIN, RADIO_IRQ_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif  /*Radio option*/

enum TransmissionDirection {
    TRANSMISSION,
    RECEIVE,
};

// save transmission state between loops
int             transmissionState = RADIOLIB_ERR_NONE;
bool            transmittedFlag = false;
uint32_t        transmissionCounter = 0;
TransmissionDirection  transmissionDirection = TRANSMISSION;
bool            isTimeAvailable = false;
DISPLAY_MODEL_SSD_LIB   *display = nullptr;
OLEDDisplayUi           *ui = nullptr;
bool            ledBlink = false;
bool            updateUseSecond = false;
uint32_t        gpsUseSecond = 0;
uint32_t        gpsStartMs = 0;
bool            isContinuousWave = false;
extern uint8_t display_address;

FrameCallback   frames[] = {
    hwProbe,
    radioTx,
    radioRx,
#ifdef HAS_GPS
    gpsInfo,
#endif
#if defined(HAS_PMU) || defined(ADC_PIN)
    pmuInfo,
#endif
#ifdef T_BEAM_S3_SUPREME
    dateTimeInfo,
    sensorInfo,
    imuInfo,
#endif
    wirelessInfo,
    screenTest,
    listeningMode,
};

const uint8_t   max_frames = sizeof(frames) / sizeof(frames[0]);
int             currentFrames = 0;
AceButton       button;
WiFiMulti       wifiMulti;

#ifdef HAS_GPS
TinyGPSPlus     gps;
#endif /*HAS_GPS*/

#ifdef BUTTON2_PIN
AceButton       button2;
#endif /*BUTTON2_PIN*/

String macStr;

void setFlag(void)
{
    // we got a packet, set the flag
    transmittedFlag = true;
}

void sleepDevice()
{
    Serial.println("Enter sleep...");
#ifndef HAS_PMU
    digitalWrite(RADIO_RST_PIN, HIGH);
    gpio_hold_en((gpio_num_t) RADIO_RST_PIN);
    gpio_deep_sleep_hold_en();
#endif
    radio.sleep();

    display->clear();

    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64, 28, "DEVICE ENTER SLEEP");
    display->display();
    delay(2000);
    display->displayOff();


#ifdef  RADIO_TCXO_ENABLE
    digitalWrite(RADIO_TCXO_ENABLE, LOW);
#endif

#ifdef BUTTON_PIN
    while (digitalRead(BUTTON_PIN) == LOW)
        delay(1);
#endif

    // PMU pre-sleep operation
    disablePeripherals();

    delay(200);

    SPI.end();

    Wire.end();

#ifdef I2C1_SDA
    Wire1.end();
#endif

#ifdef HAS_GPS
    SerialGPS.end();
#endif

    const uint8_t device_pins[] = {

        // GPS pins
#ifdef HAS_GPS
        GPS_RX_PIN,
        GPS_TX_PIN,
#endif
#ifdef GPS_PPS_PIN
        GPS_PPS_PIN,
#endif
#ifdef OLED_RST
        OLED_RST,
#endif
#ifdef GPS_EN_PIN
        GPS_EN_PIN,
#endif
        // Wire pins
        I2C_SDA,
        I2C_SCL,

#ifdef I2C1_SDA
        I2C1_SDA,
        I2C1_SCL,
#endif

        // Radio pins
#ifdef RADIO_DIO0_PIN
#if RADIO_DIO0_PIN != -1
        RADIO_DIO0_PIN,
#endif
#endif

#ifdef RADIO_DIO1_PIN
        RADIO_DIO1_PIN,
#endif
#ifdef RADIO_DIO9_PIN
        RADIO_DIO9_PIN,
#endif
#ifdef RADIO_BUSY_PIN
        RADIO_BUSY_PIN,
#endif
        RADIO_CS_PIN,
#ifdef HAS_PMU
        RADIO_RST_PIN,
#endif
        RADIO_SCLK_PIN,
        RADIO_MISO_PIN,
        RADIO_MOSI_PIN,

        // SD Card pins
#ifdef SDCARD_MOSI
        SDCARD_MOSI,
        SDCARD_MISO,
        SDCARD_SCLK,
        SDCARD_CS,
#endif

#ifdef HAS_PMU
        PMU_IRQ,
#endif

#ifdef BOARD_LED
        BOARD_LED,
#endif

#ifdef ADC_PIN
        ADC_PIN,
#endif
        BUTTON_PIN,

#ifdef IMU_INT
        IMU_INT,
#endif

#ifdef BUTTON2_PIN
        BUTTON2_PIN,
#endif

#ifdef RADIO_LDO_EN
        RADIO_LDO_EN,
#endif

#ifdef RADIO_CTRL
        RADIO_CTRL,
#endif

#ifdef FAN_CTRL
        FAN_CTRL,
#endif
    };

    for (auto pin : device_pins) {
        gpio_reset_pin((gpio_num_t )pin);
        pinMode(pin, OPEN_DRAIN);
    }

#ifdef GPS_SLEEP_HOLD_ON_LOW
#ifdef GPS_PPS_PIN
    pinMode(GPS_PPS_PIN, OUTPUT);
    digitalWrite(GPS_PPS_PIN, LOW);
    gpio_hold_en((gpio_num_t) GPS_PPS_PIN);
#endif /*GPS_PPS_PIN*/

#ifdef GPS_EN_PIN
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, LOW);
    gpio_hold_en((gpio_num_t) GPS_EN_PIN);
    gpio_deep_sleep_hold_en();
#endif
#endif /*GPS_SLEEP_HOLD_ON_LOW*/

#ifdef RADIO_LDO_EN
    pinMode(RADIO_LDO_EN, OUTPUT);
    digitalWrite(RADIO_LDO_EN, LOW);
    gpio_hold_en((gpio_num_t) RADIO_LDO_EN);
    gpio_deep_sleep_hold_en();
#endif /*RADIO_LDO_EN*/


#if CONFIG_IDF_TARGET_ESP32
    esp_sleep_ext1_wakeup_mode_t wakeup_mode = ESP_EXT1_WAKEUP_ALL_LOW;
#else
#if ESP_ARDUINO_VERSION_VAL(2,0,17) >=  ESP_ARDUINO_VERSION
    esp_sleep_ext1_wakeup_mode_t wakeup_mode = ESP_EXT1_WAKEUP_ANY_LOW;
#else
    esp_sleep_ext1_wakeup_mode_t wakeup_mode = ESP_EXT1_WAKEUP_ALL_LOW;
#endif
#endif

    /*
     * |     GPIO WAKE UP EXT 1      |
     * | Board             | Current  |
     * | ----------------  | -------- |
     * | T-BeamV 1.2 OLED  | ~ 450 uA |
     * | T-BeamV 1.2       | ~ 440 uA |
     * | T-Beam  BPF V1.2  | ~ 350 uA |
     * | T-Beam  1W SX1262 | ~ 442 uA |
     * | T-Beam  1W LR1121 | ~ 450 uA |
     * | T-Beam  1W LR2021 | ~ 450 uA |
     * | T-BeamS3 Supreme  | ~ 1.45mA |
     *
     */
    // GPIO WAKE UP EXT 1 NO  OLED  Display ~ 440 uA ,
    // GPIO WAKE UP EXT 1 +  OLED  Display  ~ 450 uA ,
    // See sleep_current.jpg
    // Serial.println("GPIO WAKE UP EXT 1");
    esp_sleep_enable_ext1_wakeup(_BV(BUTTON_PIN), wakeup_mode);


    // GPIO WAKE UP EXT 1 + TIMER WAKE UP NO  OLED  Display ~ 440 uA
    // GPIO WAKE UP EXT 1 + TIMER WAKE UP +  OLED  Display ~  450uA
    // Serial.println("GPIO WAKE UP EXT 1 + TIMER WAKE UP");
    // esp_sleep_enable_ext1_wakeup(_BV(BUTTON_PIN), wakeup_mode);
    // esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);


    // GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP  NO  OLED  Display + Disable gnss power backup power ~ 420 uA
    // GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP  NO  OLED  Display ~ 540 uA
    // GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP  +  OLED  Display  ~ 580 uA
    // Serial.println("GPIO WAKE UP EXT0 + EXT 1 + TIMER WAKE UP");
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);
    // esp_sleep_enable_ext1_wakeup(_BV(BUTTON_PIN), wakeup_mode);
    // esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);


    Serial.flush();

    Serial.end();

    delay(1000);

    esp_deep_sleep_start();

    Serial.println("Never print()");
}

void handleMenu()
{
    Serial.printf("currentFrames : %d\n", currentFrames);
    switch (currentFrames) {
    case 0:
        if (isContinuousWave) {
            radio.standby();
        }
        break;
    case 1:
#ifdef RADIO_CTRL
        Serial.println("Turn off LAN,Trun on PA, Enter Tx mode.");
        /*
        * T-Beam 1W SX1262 Version and T-Beam BPF LoRa LAN Control ,set Low turn off LAN , TX Mode
        * */
        digitalWrite(RADIO_CTRL, LOW);
#endif /*RADIO_CTRL*/

        if (isContinuousWave) {
            int16_t state = radio.transmitDirect();
            Serial.printf("transmitDirect:%d\n", state);
        } else {
            Serial.println("Start transmit");
            transmissionDirection = TRANSMISSION;
            transmissionState = radio.transmit((uint8_t *)&transmissionCounter, 4);
            if (transmissionState != RADIOLIB_ERR_NONE) {
                Serial.printf("[Radio] transmit packet failed! err: %d\n", transmissionState);
            }
        }
        break;

    case 2:
#ifdef RADIO_CTRL
        Serial.println("Turn on LAN, Enter Rx mode.");
        /*
        * T-Beam 1W SX1262 Version and T-Beam BPF LoRa LAN Control ,set HIGH turn on LAN ,RX Mode
        * */
        digitalWrite(RADIO_CTRL, HIGH);
#endif /*RADIO_CTRL*/
        Serial.println("Start receive");
        transmissionDirection = RECEIVE;
        transmissionState = radio.startReceive();
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.printf("[Radio] Received packet failed! err: %d\n", transmissionState);
        }
        break;
    default:
#ifdef RADIO_CTRL
        Serial.println("Turn on LAN, Enter Rx mode.");
        /*
        * T-Beam 1W SX1262 Version and T-Beam BPF LoRa LAN Control ,set HIGH turn on LAN ,RX Mode
        * */
        digitalWrite(RADIO_CTRL, HIGH);
#endif /*RADIO_CTRL*/
        break;
    }
    ui->transitionToFrame(currentFrames);
}

void prevButtonHandleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{
    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.printf("prevButtonHandleEvent currentFrames:%d frames_count:%d\n", currentFrames, max_frames);
#ifdef BUTTON2_PIN
        currentFrames =  ((currentFrames + 1) >= max_frames) ? currentFrames : currentFrames + 1;
#else /*BUTTON2_PIN*/
        currentFrames++;
        currentFrames %= max_frames;
#endif /*BUTTON2_PIN*/
        handleMenu();
        break;
    case AceButton::kEventDoubleClicked:
        Serial.println("Double clicked!");
        powerOff();
        break;
    case AceButton::kEventLongPressed: {
        // When using LR2021 or LR1121, press and hold the first button to switch between low and high frequencies.
#if defined(USING_LR1121) || defined(USING_LR2021)
        // Sub1G or 2.4G frequency toggle
        float tmp = lora_settings.lora_freq;
        if (tmp < 2400) {
            tmp = 2450;
        } else {
            tmp = CONFIG_RADIO_FREQ;
        }
        Serial.printf("Frequency toggle to %.2f\n", tmp);
        radio.standby();
        applyFreq(String(tmp));
        reloadLoRa();
#else
        sleepDevice();
#endif
    }
    break;
    default:
        break;
    }
}

#ifdef BUTTON2_PIN
void nextButtonHandleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{
    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.printf("nextButtonHandleEvent currentFrames:%d frames_count:%d\n", currentFrames, max_frames);
        currentFrames =  ((currentFrames - 1) < 0) ? currentFrames : currentFrames - 1;
        handleMenu();
        break;
    case AceButton::kEventDoubleClicked:
        Serial.println("Double clicked!");
        break;
    case AceButton::kEventLongPressed:
        Serial.println("Long pressed!");
#ifdef FAN_CTRL
        Serial.println("Long pressed! ,on/off FAN");
        digitalWrite(FAN_CTRL, 1 - digitalRead(FAN_CTRL));
#endif /*FAN_CTRL*/
        break;
    }
}
#endif /*BUTTON2_PIN*/

void setup()
{
    Serial.begin(115200);

    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_ALL) {

        gpio_deep_sleep_hold_dis();
#ifndef HAS_PMU
        gpio_hold_dis((gpio_num_t) RADIO_RST_PIN);
#endif /*HAS_PMU*/

#ifdef GPS_SLEEP_HOLD_ON_LOW
        gpio_hold_dis((gpio_num_t) GPS_EN_PIN);
#endif /*GPS_SLEEP_HOLD_ON_LOW*/

#ifdef RADIO_LDO_EN
        gpio_hold_dis((gpio_num_t) RADIO_LDO_EN);
#endif /*RADIO_LDO_EN*/

    }

    setupPreferences();

    setupBoards(false);

    setupSensor();

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    setupButton();

    setupDisplay();

    setLed(true);

    setupLoRa();

    setupNetwork();

    setupBLE();

    // Record GPS start time
    gpsStartMs = millis();
}

void power_key_pressed()
{
    // Turn on/off display
    static bool isOn = true;
    isOn ? display->displayOff()  : display->displayOn();
    isOn ^= 1;
    return;
}

void loop()
{
    handleWomCommands();

    loopGPS();

    loopButton();

    ui->update();
    delay(2);
}


void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{

    static constexpr uint32_t radio_tx_interval_ms = 1000;
    static uint32_t lastTxMills = 0;
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    if (isContinuousWave) {
        display->setFont(Roboto_Mono_Medium_12);
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 0 + y, "Continuous Wave");
        return;
    }

    if ((millis() - lastTxMills > radio_tx_interval_ms)
            && transmissionDirection == TRANSMISSION ) {
        if (transmittedFlag) {
            // reset flag
            transmittedFlag = false;
            if (transmissionState == RADIOLIB_ERR_NONE) {
                // packet was successfully sent
                Serial.println(F("[RadioTx] transmission finished!"));

                // NOTE: when using interrupt-driven transmit method,
                //       it is not possible to automatically measure
                //       transmission data rate using getDataRate()

            } else {
                Serial.print(F("[RadioTx] failed, code "));
                Serial.println(transmissionState);

            }

            // clean up after transmission is finished
            // this will ensure transmitter is disabled,
            // RF switch is powered down etc.
            radio.finishTransmit();

            // send another one
            Serial.print(F("[RadioTx]  Sending another packet ... "));

            // you can transmit C-string or Arduino string up to
            // 256 characters long
            // transmissionState = radio.startTransmit("Hello World!");
            radio.transmit((uint8_t *)&transmissionCounter, 4);
            transmissionCounter++;

            // you can also transmit byte array up to 256 bytes long
            /*
              byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                                0x89, 0xAB, 0xCD, 0xEF};
              int state = radio.startTransmit(byteArr, 8);
            */
            setLed(ledBlink);
            ledBlink ^= 1;
        }
        lastTxMills = millis();
    }
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "RADIO TX");

    if (transmissionState !=  RADIOLIB_ERR_NONE) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        static char buffer[64];
        snprintf(buffer, 64, "Radio Tx FAIL:%d", transmissionState);
        display->drawString(64 + x, 32 + y, buffer);
    } else {
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 16 + y, "Freq:" + String(lora_settings.lora_freq) + "MHz");
        display->drawString(0 + x, 32 + y, "TX :" + String(transmissionCounter));
    }


#ifdef NTC_PIN
    static char buffer[32];
    sprintf(buffer, "NTC:%.2f*C", getTempForNTC());
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0 + x, 48 + y, buffer);
#endif
}

void radioRx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static uint32_t recvCounter = 0;
    static float    radioRSSI   =   0;

    display->setFont(ArialMT_Plain_10);
    // The coordinates define the left starting point of the text
    display->setTextAlignment(TEXT_ALIGN_LEFT);

    // check if the flag is set
    if (transmittedFlag  && transmissionDirection == RECEIVE) {
        Serial.println("Radio RX done !");

#ifdef BOARD_LED
        digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));
#endif
        // reset flag
        transmittedFlag = false;
        // you can read received data as an Arduino String
        transmissionState = radio.readData((uint8_t *)&recvCounter, 4);
        // you can also read received data as byte array
        /*
          byte byteArr[8];
          int state = radio.readData(byteArr, 8);
        */
        if (transmissionState == RADIOLIB_ERR_NONE) {
            // packet was successfully received
            Serial.print("[RadioRx] Received packet!");
            radioRSSI = radio.getRSSI();
            Serial.printf(" COUNT:%u RSSI:%.2f SNR:%.2f\n", recvCounter, radioRSSI, radio.getSNR());

        } else if (transmissionState == RADIOLIB_ERR_CRC_MISMATCH) {
            // packet was received, but is malformed
            Serial.println(F("[RadioRx] CRC error!"));

        } else {
            // some other error occurred
            Serial.print(F("[RadioRx] Failed, code "));
            Serial.println(transmissionState);
        }
        // put module back to listen mode
        radio.startReceive();
    }
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "RADIO RX");

    if (transmissionState !=  RADIOLIB_ERR_NONE) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 32 + y, "Radio Rx Failed!");
    } else {
        // The coordinates define the left starting point of the text
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 16 + y, "Freq:" + String(lora_settings.lora_freq) + "MHz");
        display->drawString(0 + x, 32 + y, "RX :"  + String(recvCounter));
        display->drawString(0 + x, 48 + y, "RSSI:" + String(radioRSSI));
    }
}

void hwProbe(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_LEFT);

#if defined(T_BEAM_S3_SUPREME)
    display->drawString(x, 3  + y, "6DOF");
    display->drawString(x, 15 + y, "Power");
    display->drawString(x, 27 + y, "Radio");
    display->drawString(x, 39 + y, "GPS");
    display->drawString(x, 51 + y, "RTC");

    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(52  + x, 3  + y, (deviceOnline & QMI8658_ONLINE ) ? "+" : "-");
    display->drawString(52  + x, 15 + y, (deviceOnline & POWERMANAGE_ONLINE ) ? "+" : "-");
    display->drawString(52  + x, 27 + y, (deviceOnline & RADIO_ONLINE ) ? "+" : "-");
    display->drawString(52  + x, 39 + y, (deviceOnline & GPS_ONLINE )   ? "+" : "-");
    display->drawString(52  + x, 51 + y, (deviceOnline & PCF8563_ONLINE ) ? "+" : "-");

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(62 + x, 3  + y, "MAG");
    display->drawString(62 + x, 15 + y, "BME");
    display->drawString(62 + x, 27 + y, "PSRAM");
    display->drawString(62 + x, 39 + y, "SDCARD");
    display->drawString(62 + x, 51 + y, "OSC");

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(display->width()  + x,  3  + y, ((deviceOnline & QMC6310U_ONLINE) || (deviceOnline & QMC6310N_ONLINE) || (deviceOnline & QMC6309_ONLINE)) ? "+" : "-");
    display->drawString(display->width()  + x,  15 + y, (deviceOnline & BME280_ONLINE ) || (deviceOnline & BMP280_ONLINE ) ? "+" : "-");
    display->drawString(display->width()  + x,  27 + y, (deviceOnline & PSRAM_ONLINE )   ? "+" : "-");
    display->drawString(display->width()  + x,  39 + y, (deviceOnline & SDCARD_ONLINE )  ? "+" : "-");
    display->drawString(display->width()  + x,  51 + y, (deviceOnline & OSC32768_ONLINE ) ? "+" : "-");

#else
    display->drawString(x, 16 + y, "Radio");
    display->drawString(x, 32 + y, "GPS");
    display->drawString(x, 48 + y, "OLED");

    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(52 + x, 0 + y, BOARD_VARIANT_NAME " " RADIO_TYPE_STR);
    display->drawString(52 + x, 16 + y,  (deviceOnline & RADIO_ONLINE ) ? "+" : "-");

    display->drawString(52  + x, 32 + y, (deviceOnline & GPS_ONLINE ) ? "+" : "-");
    display->drawString(52  + x, 48 + y, (deviceOnline & DISPLAY_ONLINE ) ? "+" : "-");

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(62 + x, 16 + y, "PSRAM");
#ifdef HAS_SDCARD
    display->drawString(62 + x, 32 + y, "SDCARD");
#endif
#ifdef HAS_PMU
    display->drawString(62 + x, 48 + y, "Power");
#endif

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(display->width() + x,  16 + y, (deviceOnline & PSRAM_ONLINE ) ? "+" : "-");
#ifdef HAS_SDCARD
    display->drawString(display->width()  + x, 32 + y, (deviceOnline & SDCARD_ONLINE ) ? "+" : "-");
#endif /*HAS_SDCARD*/

#ifdef HAS_PMU
    display->drawString(display->width()  + x, 48 + y, (deviceOnline & POWERMANAGE_ONLINE ) ? "+" : "-");
#endif /*HAS_PMU*/

#endif
}

#define MOVE_INTERVAL 50
#define MOVE_STEP 1
#define LEFT_BOUND -64
#define RIGHT_BOUND 64

void screenTest(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static uint32_t lastMoveTime = 0;
    static int16_t moveOffset = 0;
    static bool isMovingRight = true;

    display->drawRect(0, 0, 128, 64);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    uint32_t currentTime = millis();
    if (currentTime - lastMoveTime >= MOVE_INTERVAL) {
        lastMoveTime = currentTime;
        if (isMovingRight) {
            moveOffset += MOVE_STEP;
            if (moveOffset >= RIGHT_BOUND) {
                isMovingRight = false;
            }
        } else {
            moveOffset -= MOVE_STEP;
            if (moveOffset <= LEFT_BOUND) {
                isMovingRight = true;
            }
        }
    }
    display->drawString(64 + x + moveOffset, 0 + y, "SCREEN");
    display->drawString(64 + x + moveOffset, 16 + y, "TEST");
    display->drawString(64 + x + moveOffset, 32 + y, "INFO");
    display->drawString(64 + x + moveOffset, 48 + y, "STATUS");
}

void wirelessInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "WIFI");

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0 + x, 16 + y, "SSID:");
    if (String(WIFI_SSID) == "Your WiFi SSID" ) {
        display->drawString(38 + x, 16 + y, "NO SET");
    } else {
        display->drawString(38 + x, 16 + y, WIFI_SSID);
    }

    display->drawString(0 + x, 32 + y, "RSSI:");
    if (WiFi.isConnected()) {
        display->drawString(38 + x, 32 + y, String(WiFi.RSSI()));
    } else {
        display->drawString(38 + x, 32 + y, "N.A");
    }
    display->drawString(0 + x, 48 + y, "MAC:");
    display->drawString(30 + x, 48 + y, macStr);
}

#ifdef HAS_GPS
void gpsInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    const uint8_t buffer_size = 128;
    static char buffer[buffer_size];
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);

    if (updateUseSecond) {
        display->drawString(48 + x, 0 + y, "GPS Use" + String(gpsUseSecond) + "S");
    } else {
        display->drawString(64 + x, 0 + y, "GPS");
    }
    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {

        if (!updateUseSecond) {
            updateUseSecond = true;
            gpsUseSecond = (millis() - gpsStartMs) / 1000;
        }

        display->setTextAlignment(TEXT_ALIGN_LEFT);
        snprintf(buffer, buffer_size, "lat:%.6f",  gps.location.lat());
        display->drawString(0 + x, 16 + y, buffer);
        snprintf(buffer, buffer_size, "lng:%.6f",   gps.location.lng());
        display->drawString(0 + x, 32 + y, buffer);
        snprintf(buffer, buffer_size, "%02d/%02d/%02d %02d:%02d:%02d", gps.date.year() - 2000, gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
        display->drawString(0 + x, 48 + y, buffer);
    } else {
        display->drawString(64 + x, 16 + y, "RX:" + String(gps.charsProcessed()));
        display->drawString(64 + x, 32 + y, "GPS No Lock");
    }
}
#endif

void pmuInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
#ifdef HAS_PMU
    float batteryVoltage = PMU->getBattVoltage();
    static char buffer[3][80];
    snprintf(buffer[0], sizeof(buffer[0]), "SYS VOL:%.2f V", PMU->getSystemVoltage() / 1000.0);
    snprintf(buffer[1], sizeof(buffer[1]), "BAT VOL:%.2f V", batteryVoltage == -1 ? 0 : batteryVoltage / 1000.0);
    if (PMU->isVbusIn()) {
        snprintf(buffer[2], sizeof(buffer[2]), "USB VOL:%.2f V", PMU->getVbusVoltage() / 1000.0);
    } else {
        snprintf(buffer[2], sizeof(buffer[2]), "USB LOST");
    }
    display->drawString(64 + x, 0 + y, "PMU");
    display->drawString(64 + x, 16 + y, buffer[0]);
    display->drawString(64 + x, 32 + y, buffer[1]);
    display->drawString(64 + x, 48 + y, buffer[2]);
#endif

#ifdef ADC_PIN
    static char buffer[64];
    static uint32_t interval = 0;
    if (millis() - interval > 1000) {
        // analogReadResolution(12); //Default is 12 bits, not need set
        const float vRef = 3.3;
        const int adcResolution = 4095;
        uint16_t  adcValue = analogRead(ADC_PIN);
        float dividedVoltage = (float)adcValue / adcResolution * vRef;
        float batteryVoltage = dividedVoltage * (BAT_ADC_PULLUP_RES + BAT_ADC_PULLDOWN_RES) / BAT_ADC_PULLDOWN_RES;
        batteryVoltage += BAT_VOL_COMPENSATION; // Voltage compensation
        uint32_t value = batteryVoltage * 1000;
        const uint32_t max_voltage = BAT_MAX_VOLTAGE * 1000;
        if (value > max_voltage) {
            value = max_voltage;
        }
        if (value < 3000) {
            value = 0;
        }
        snprintf(buffer, sizeof(buffer), "VOL:%.2f V", (float)value / 1000.0f);
        interval = millis();
    }
    display->drawString(64 + x, 0 + y, "Battery");
    display->drawString(64 + x, 32 + y, buffer);
#endif
}

static void loopGPS()
{
#ifdef HAS_GPS
    while (SerialGPS.available()) {
        int r = SerialGPS.read();
        if (frames[currentFrames] == gpsInfo) {
            Serial.write(r);
        }
        gps.encode(r);
    }
#endif
}

#ifdef T_BEAM_S3_SUPREME
void dateTimeInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    static char buffer[2][128] = {0};
    static uint32_t interval = 0;

    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);

    if (deviceOnline & PCF8563_ONLINE && millis() > interval) {
        struct tm timeinfo;
        rtc.getDateTime(&timeinfo);
        snprintf(buffer[0], 128, "%04d/%02d/%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
        snprintf(buffer[1], 128, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        interval = millis() + 1000;
    }
    display->drawString(64 + x, 0 + y, "RTC");
    display->drawString(64 + x, 16 + y, buffer[0]);
    display->drawString(64 + x, 32 + y, buffer[1]);
}

void sensorInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    static float temperature = 0;
    static float humidity = 0;
    static float pressure = 0;
    static uint32_t interval = 0;

    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "SENSOR");

    if (deviceOnline & BME280_ONLINE) {
        if (millis() > interval) {
            temperature = bme.readTemperature();
            humidity = bme.readHumidity();
            pressure = bme.readPressure();
            interval = millis() + 1000;
        }
    }
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(x + 0, y + 16, "TEMP:");
    display->drawString(x + 0, y + 32, "HUM:");
    display->drawString(x + 0, y + 48, "PRES:");

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(display->width() + x, y + 16, String(temperature) + "°C");
    display->drawString(display->width() + x, y + 32, String(humidity) + " %");
    display->drawString(display->width() + x, y + 48, String(pressure / 1000.0) + " kPa");
}

Madgwick  filter;


void imuInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    static float roll, pitch, heading, strength;
    static uint32_t interval = 0;

    MagnetometerData data;
    if (millis() - interval > 100) {
        if (magnetometer) {
            if (magnetometer->readData(data)) {
                strength = MagnetometerUtils::calculateMagneticStrength(data);
                strength = MagnetometerUtils::gaussToMicroTesla(strength);
            }
        }
        interval = millis();
    }

    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "IMU");

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(x + 0, y + 16, "STRENGTH:");
    display->drawString(x + 0, y + 32, "ROLL:");
    display->drawString(x + 0, y + 48, "HEADING:");

    // Read raw data from IMU
    if (digitalRead(IMU_INT) == HIGH) {
        IMUdata acc, gyr;
        qmi.getAccelerometer(acc.x, acc.y, acc.z);
        qmi.getGyroscope(gyr.x, gyr.y, gyr.z);
        // Update the filter, which computes orientation
        filter.updateIMU(gyr.x, gyr.y, gyr.z, acc.x, acc.y, acc.z);
        roll = filter.getRoll();
        pitch = filter.getPitch();
        heading = filter.getYaw();
    }

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(display->width() + x, y + 16, String(strength) + "uT");
    display->drawString(display->width() + x, y + 32, String(roll) + "°");
    display->drawString(display->width() + x, y + 48, String(heading) + "°");
}

static void setupSensor()
{
    extern uint8_t mag_address;

    // PMU and RTC share I2C bus
    if (!rtc.begin(PMU_WIRE_PORT, I2C_SDA, I2C_SCL)) {
        Serial.println("Failed to find PCF8563 - check your wiring!");
    }

    // The desired output data rate in Hz.  Allowed values are 1.0, 10.0, 50.0, 100.0 and 200.0HZ.
    float data_rate_hz = 200.0f;
    // op_mode: Allowed values are SUSPEND, NORMAL, SINGLE_MEASUREMENT, CONTINUOUS_MEASUREMENT
    OperationMode op_mode = OperationMode::CONTINUOUS_MEASUREMENT;
    // full_scale: Allowed values are  FS_8G, FS_16G ,FS_32G
    MagFullScaleRange full_scale = MagFullScaleRange::FS_8G;
    // over_sample_ratio: Allowed values are OSR_1, OSR_2, OSR_4, OSR_8
    MagOverSampleRatio over_sample_ratio = MagOverSampleRatio::OSR_1;
    // down_sample_ratio: QMC6309 does not support downsampling rate settings; this parameter is ignored.
    MagDownSampleRatio down_sample_ratio = MagDownSampleRatio::DSR_1;

    Serial.printf("Probing magnetometer at address 0x%02X...\n", mag_address);
    if (magnetometer == nullptr && mag_address == QMC6310U_SLAVE_ADDRESS) {
        magnetometer = new SensorQMC6310();
        if (!static_cast<SensorQMC6310*>(magnetometer)->begin(Wire, QMC6310U_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
            Serial.println("Failed to find QMC6310U - check your wiring!");
            delete magnetometer;
            magnetometer = nullptr;
        } else {
            Serial.println("QMC6310U found!");
            // The desired output data rate in Hz.  Allowed values are 10.0, 50.0, 100.0 and 200.0HZ.
            data_rate_hz = 10.0f;
            // op_mode: Allowed values are SUSPEND, NORMAL, SINGLE_MEASUREMENT, CONTINUOUS_MEASUREMENT
            op_mode = OperationMode::CONTINUOUS_MEASUREMENT;
            // full_scale: Allowed values are FS_2G, FS_8G, FS_12G ,FS_30G
            full_scale = MagFullScaleRange::FS_8G;
            // over_sample_ratio: Allowed values are OSR_1, OSR_2, OSR_4, OSR_8
            over_sample_ratio = MagOverSampleRatio::OSR_1;
            // down_sample_ratio: Allowed values are DSR_1, DSR_2, DSR_4, DSR_8
            down_sample_ratio = MagDownSampleRatio::DSR_1;
        }
    }

    if (magnetometer == nullptr && mag_address == QMC6310N_SLAVE_ADDRESS) {
        magnetometer = new SensorQMC6310();
        if (!static_cast<SensorQMC6310*>(magnetometer)->begin(Wire, QMC6310N_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
            Serial.println("Failed to find QMC6310 - check your wiring!");
            delete magnetometer;
            magnetometer = nullptr;
        } else {
            Serial.println("QMC6310N found!");
            // The desired output data rate in Hz.  Allowed values are 10.0, 50.0, 100.0 and 200.0HZ.
            data_rate_hz = 10.0f;
            // op_mode: Allowed values are SUSPEND, NORMAL, SINGLE_MEASUREMENT, CONTINUOUS_MEASUREMENT
            op_mode = OperationMode::CONTINUOUS_MEASUREMENT;
            // full_scale: Allowed values are FS_2G, FS_8G, FS_12G ,FS_30G
            full_scale = MagFullScaleRange::FS_8G;
            // over_sample_ratio: Allowed values are OSR_1, OSR_2, OSR_4, OSR_8
            over_sample_ratio = MagOverSampleRatio::OSR_1;
            // down_sample_ratio: Allowed values are DSR_1, DSR_2, DSR_4, DSR_8
            down_sample_ratio = MagDownSampleRatio::DSR_1;
        }
    }

    if (magnetometer == nullptr && mag_address == QMC6309_SLAVE_ADDRESS) {
        magnetometer = new SensorQMC6309();
        if (!static_cast<SensorQMC6309*>(magnetometer)->begin(Wire, QMC6309_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
            Serial.println("Failed to find QMC6309 - check your wiring!");
            delete magnetometer;
            magnetometer = nullptr;
        } else {
            Serial.println("QMC6309 found!");
            // The desired output data rate in Hz.  Allowed values are 1.0, 10.0, 50.0, 100.0 and 200.0HZ.
            data_rate_hz = 10.0f;
            // op_mode: Allowed values are SUSPEND, NORMAL, SINGLE_MEASUREMENT, CONTINUOUS_MEASUREMENT
            op_mode = OperationMode::CONTINUOUS_MEASUREMENT;
            // full_scale: Allowed values are  FS_8G, FS_16G ,FS_32G
            full_scale = MagFullScaleRange::FS_8G;
            // over_sample_ratio: Allowed values are OSR_1, OSR_2, OSR_4, OSR_8
            over_sample_ratio = MagOverSampleRatio::OSR_1;
            // down_sample_ratio: QMC6309 does not support downsampling rate settings; this parameter is ignored.
            down_sample_ratio = MagDownSampleRatio::DSR_1;
        }
    }

    if (magnetometer) {
        /* Config Magnetometer */
        if (magnetometer->configMagnetometer(
                    op_mode,
                    full_scale,
                    data_rate_hz,
                    over_sample_ratio,
                    down_sample_ratio)) {
            Serial.println("Magnetometer configured successfully.");
        } else {
            Serial.println("Magnetometer configuration failed.");
        }
    }

    extern uint8_t bme280_address;
    if (!bme.begin(bme280_address)) {
        Serial.println("Failed to find BME280 - check your wiring!");
    }

    pinMode(SPI_CS, OUTPUT);    //sdcard pin set high
    digitalWrite(SPI_CS, HIGH);


    pinMode(IMU_INT, INPUT_PULLUP); //IMU set interrupt pin
    // SDCard shares SPI bus with QMI8658
    // SPI has been initialized in initBoard.
    // Only need to pass SPIhandler to the QMI class.
    if (!qmi.begin(SDCardSPI, IMU_CS)) {
        Serial.println("Failed to find QMI8658 - check your wiring!");
    } else {
        /* Get chip id*/
        Serial.print("QMI8658 Device ID:");
        Serial.println(qmi.getChipID(), HEX);
        deviceOnline |= QMI8658_ONLINE;

        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_1000Hz,
            SensorQMI8658::LPF_MODE_0
        );
        qmi.configGyroscope(
            SensorQMI8658::GYR_RANGE_256DPS,
            SensorQMI8658::GYR_ODR_896_8Hz,
            SensorQMI8658::LPF_MODE_3
        );


        // In 6DOF mode (accelerometer and gyroscope are both enabled),
        // the output data rate is derived from the nature frequency of gyroscope
        qmi.enableGyroscope();
        qmi.enableAccelerometer();

        // Enable data ready to interrupt pin2
        qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_2);
        qmi.enableDataReadyINT();
    }

    // start  filter
    filter.begin(25);
}
#endif

void listeningMode(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "Signal");

    static uint32_t lastUpdate = 0;
    static float rssi;
    static float snr;
    if (millis() - lastUpdate > 500) {
        rssi = radio.getRSSI();
        snr = radio.getSNR();
        lastUpdate = millis();
        Serial.printf("RSSI: %.2f dBm, SNR: %.2f dB\n", rssi, snr);
    }
    display->drawString(64 + x, 16 + y, "RSSI:" +  String(rssi) + "dBm");
    display->drawString(64 + x, 32 + y, "SNR:" + String(snr) + "dB");
    display->drawString(64 + x, 48 + y, "Update:" + String(lastUpdate / 1000));
}


void wifiTask(void *task)
{
    while (1) {
        wifiMulti.run();
        if (isTimeAvailable) {
            Serial.println("---REMOVE WIFI TASK---");
            vTaskDelete(NULL);
        }
        delay(1000);
    }
}

static void setupPreferences()
{
    // Get preferences
    bool res = preferences.begin(pref_key, false);
    if (!res) {
        Serial.println("Set default params");
        preferences.putBytes("lora_params", (const void *)&lora_settings, sizeof(lora_settings));
    } else {
        preferences.getBytes("lora_params", (void *)&lora_settings, sizeof(lora_settings));
    }
    preferences.end();
}

static void setupButton()
{
    static ButtonConfig prevButtonConfigure;
    prevButtonConfigure.setEventHandler(prevButtonHandleEvent);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureClick);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureLongPress);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureDoubleClick);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    button.init(BUTTON_PIN);
    button.setButtonConfig(&prevButtonConfigure);

#ifdef BUTTON2_PIN
    static ButtonConfig nextButtonConfigure;
    nextButtonConfigure.setEventHandler(nextButtonHandleEvent);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureClick);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureLongPress);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureDoubleClick);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    button2.init(BUTTON2_PIN);
    button2.setButtonConfig(&nextButtonConfigure);
#endif /*BUTTON2_PIN*/
}

static void loopButton()
{
#ifdef HAS_PMU
    loopPMU(power_key_pressed);
#endif

    button.check();

#ifdef BUTTON2_PIN
    button2.check();
#endif
}

static void timeavailable(struct timeval *t)
{
    Serial.println("[WiFi]: Got time adjustment from NTP!");
    isTimeAvailable = true;

#ifdef T_BEAM_S3_SUPREME
    if (deviceOnline & PCF8563_ONLINE) {
        rtc.hwClockWrite();
    }
#endif /*T_BEAM_S3_SUPREME*/
}

static void setupNetwork()
{
    uint8_t mac[6];
    char buffer[18] = { 0 };
    esp_efuse_mac_get_default(mac);
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    macStr = buffer;

    // set notification call-back function
    sntp_set_time_sync_notification_cb( timeavailable );


    /**
     * This will set configured ntp servers and constant TimeZone/daylightOffset
     * should be OK if your time zone does not need to adjust daylightOffset twice a year,
     * in such a case time adjustment won't be handled automagically.
     */
    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
    configTzTime("CST-8", "cn.ntp.org.cn", "pool.ntp.org", "time.nist.gov");

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.print("WiFi connected");
        Serial.print(" IP address: ");
        Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
    }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

    WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.wifi_sta_disconnected.reason);
    }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    if (String(WIFI_SSID) == "Your WiFi SSID" || String(WIFI_PASSWORD) == "Your WiFi PASSWORD" ) {
        Serial.println("[Error] : WiFi ssid and password are not configured correctly");
        Serial.println("[Error] : WiFi ssid and password are not configured correctly");
        Serial.println("[Error] : WiFi ssid and password are not configured correctly");
    } else {
        wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

#ifdef WIFI_SSID2
        wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
#endif
        xTaskCreate(wifiTask, "wifi", 4 * 2048, NULL, 10, NULL);
    }

}

static void setupDisplay()
{
    // The device address is obtained through I2C scanning.
    display = new DISPLAY_MODEL_SSD_LIB(display_address, I2C_SDA, I2C_SCL);

    ui = new OLEDDisplayUi(display);

    // Initialising the UI will init the display too.
    ui->setTargetFPS(60);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    // ui->setIndicatorPosition(BOTTOM);
    ui->disableAllIndicators();

    // Defines where the first frame is located in the bar.
    ui->setIndicatorDirection(LEFT_RIGHT);

    // You can change the transition that is used
    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
    ui->setFrameAnimation(SLIDE_LEFT);

    // Add frames
    ui->setFrames(frames, sizeof(frames) / sizeof(frames[0]));

    ui->disableAutoTransition();

    // Initialising the UI will init the display too.
    ui->init();

    display->flipScreenVertically();
}

static void setupLoRa()
{

#if defined(USING_LR2021)
    radio.irqDioNum = RADIO_DIO_NUM;
#endif

    int  state = radio.begin();
    if ( state == RADIOLIB_ERR_NONE) {
        deviceOnline |= RADIO_ONLINE;
    }

    Serial.printf("[%s]:", RADIO_TYPE_STR);
    Serial.print(F("LoRa Initialized "));
    Serial.println(state == RADIOLIB_ERR_NONE ? F("Success!") : F("Failed!"));

    setupLoRaParams(lora_settings);

    reloadLoRa();

}

#if defined(T_BEAM_1W_LR1121)
// LR1121 Version PA RF switch table
static const uint32_t pa_version_rf_switch_dio_pins[] = {
    RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6, RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8, RADIOLIB_NC
};

static const Module::RfSwitchMode_t low_sub1g_switch_table[] = {
    // mode                  DIO5  DIO6 DIO7 DIO8
    { LR11x0::MODE_STBY,   { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_TX,     { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_RX,     { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_TX_HP,  { LOW,  LOW, LOW, HIGH} }, //Sub1G DIO8 SET HIGH
    { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_GNSS,   { LOW,  LOW, LOW, HIGH} },
    { LR11x0::MODE_WIFI,   { LOW,  LOW, LOW, HIGH} },
    END_OF_MODE_TABLE,
};

static const Module::RfSwitchMode_t high_2g4_switch_table[] = {
    // mode                  DIO5  DIO6 DIO7 DIO8
    { LR11x0::MODE_STBY,   { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_TX,     { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_RX,     { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_TX_HP,  { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_TX_HF,  { LOW,  LOW, HIGH, LOW} }, //2.4G TX DIO7 SET HIGH
    { LR11x0::MODE_GNSS,   { LOW,  LOW, LOW, LOW} },
    { LR11x0::MODE_WIFI,   { LOW,  HIGH, LOW, LOW} }, //2.4G RX DIO6 SET HIGH
    END_OF_MODE_TABLE,
};
#elif defined(T_BEAM_1W_LR2021)

// LR2021 Version PA RF switch table
static const uint32_t pa_version_rf_switch_dio_pins[] = {
    RADIOLIB_LR2021_DIO5, RADIOLIB_LR2021_DIO6, RADIOLIB_LR2021_DIO7, RADIOLIB_LR2021_DIO8, RADIOLIB_NC
};

static const Module::RfSwitchMode_t low_sub1g_switch_table[] = {
    // mode                  DIO5  DIO6 DIO7 DIO8
    { LR2021::MODE_STBY,   { LOW,  LOW, LOW, LOW} },
    { LR2021::MODE_TX,     { LOW,  LOW, LOW, HIGH} }, // Sub1G DIO8 SET HIGH
    { LR2021::MODE_RX,     { LOW,  LOW, LOW, LOW} },  // Sub1G ALL DIO SET LOW
    { LR2021::MODE_RX_HF,  { LOW,  LOW, LOW, LOW} },
    { LR2021::MODE_TX_HF,  { LOW,  LOW, LOW, LOW} },
    END_OF_MODE_TABLE,
};

static const Module::RfSwitchMode_t high_2g4_switch_table[] = {
    // mode                  DIO5  DIO6 DIO7 DIO8
    { LR2021::MODE_STBY,   { LOW,  LOW, LOW, LOW} },
    { LR2021::MODE_TX,     { LOW,  LOW, LOW, LOW} },
    { LR2021::MODE_RX,     { LOW,  LOW, LOW, LOW} },
    { LR2021::MODE_RX_HF,  { LOW,  HIGH, LOW, LOW} }, // 2.4G RX DIO6 SET HIGH
    { LR2021::MODE_TX_HF,  { LOW,  LOW, HIGH, LOW} }, // 2.4G TX DIO7 SET HIGH
    END_OF_MODE_TABLE,
};
#endif /*T_BEAM_1W_LR1121 | T_BEAM_1W_LR2021*/

static void setupRfSwitch()
{
#if defined(USING_SX1262)
    radio.setDio2AsRfSwitch(true);
#elif defined(USING_LR1121) && !defined(T_BEAM_1W_LR1121)
    // LR1121
    static const uint32_t rfswitch_dio_pins[] = {
        RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
        RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC
    };

    static const Module::RfSwitchMode_t rfswitch_table[] = {
        // mode                  DIO5  DIO6
        { LR11x0::MODE_STBY,   { LOW,  LOW  } },
        { LR11x0::MODE_RX,     { HIGH, LOW  } },
        { LR11x0::MODE_TX,     { LOW,  HIGH } },
        { LR11x0::MODE_TX_HP,  { LOW,  HIGH } },
        { LR11x0::MODE_TX_HF,  { LOW,  LOW  } },
        { LR11x0::MODE_GNSS,   { LOW,  LOW  } },
        { LR11x0::MODE_WIFI,   { LOW,  LOW  } },
        END_OF_MODE_TABLE,
    };
    radio.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);

    //  TCXO Voltage 2.85~3.15V
    radio.setTCXO(3.0);

#elif defined(USING_LR1121) || defined(USING_LR2021)
    // radio.setDioIrqParams(RADIOLIB_LR11X0_DIO10);
    if (lora_settings.lora_freq < 2400) {
        Serial.printf("[%s] Using low frequency switch table for PA version\n", RADIO_TYPE_STR);
        radio.setRfSwitchTable(pa_version_rf_switch_dio_pins, low_sub1g_switch_table);
    } else {
        Serial.printf("[%s] Using high frequency switch table for PA version\n", RADIO_TYPE_STR);
        radio.setRfSwitchTable(pa_version_rf_switch_dio_pins, high_2g4_switch_table);
    }
    // TCXO Voltage 2.85~3.15V
    radio.setTCXO(3.0);
#endif
#if defined(RADIO_RX_PIN) && defined(RADIO_TX_PIN)
    //The SX1280 version needs to set RX, TX antenna switching pins
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif
}

static void printLoRaParams(loraParams params)
{
    Serial.println("--------------LORA PARAMS------------------");
    Serial.printf("- Type:%s\n", RADIO_TYPE_STR);
    Serial.printf("- Freq:%.2f MHZ\n", params.lora_freq);
    Serial.printf("- TxPower:%d dBm\n", params.lora_tx_power);
    Serial.printf("- Spreading Factor:%u \n", params.lora_sf);
    Serial.printf("- Bandwidth:%.2f kHz\n", params.lora_bw);
    Serial.printf("- Coding Rate:%u \n", params.lora_cr);
    Serial.printf("- Sync Word:0x%02X \n", params.lora_sw);
    Serial.printf("- Preamble Length:%u \n", params.lora_preamble_length);
    Serial.printf("- LoRa CRC:%s \n", params.lora_crc ? "true" : "false");
    Serial.printf("- Invert IQ:%s \n", params.iq_inverted ? "true" : "false");
    Serial.printf("- Continuous Wave:%s \n", isContinuousWave ? "true" : "false");
    Serial.println("------------------------------------------");
}

static bool setupLoRaParams(loraParams &params)
{

    printLoRaParams(params);

    radio.standby();

    /*
    *   Sets carrier frequency.
    *   The final adjustable frequency depends on the version purchased.
    */
    if (radio.setFrequency(params.lora_freq) == RADIOLIB_ERR_INVALID_FREQUENCY) {
        Serial.println(F("Selected frequency is invalid for this module!"));
        return false;
    }

    /*
    *   Sets LoRa link bandwidth.
    *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
    *   SX1268/SX1262 : Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
    *   SX1280        : Allowed values are 203.125, 406.25, 812.5 and 1625.0 kHz.
    *   LR1121        : Allowed values are 62.5, 125.0, 250.0 and 500.0 kHz.
    *   LR2021        : Allowed values are 31.25, 41.67, 62.5, 83.34, 125.0, 101.56, 203.13, 250.0, 406.25, 500.0 kHz, 812.5 kHz and 1000.0 kHz.
    */
    if (radio.setBandwidth(params.lora_bw) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
        return false;
    }


    /*
    * Sets LoRa link spreading factor.
    * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
    * SX1262,SX1280,LR1121,LR2021 :  Allowed values range from 5 to 12.
    */
    if (radio.setSpreadingFactor(params.lora_sf) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        return false;
    }

    /*
    * Sets LoRa coding rate denominator.
    * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
    * SX1280        :  Allowed values range from 5 to 8.
    * LR1121        :  Allowed values range from 5 to 8.
    * LR2021        :  Allowed values range from 4 to 8.
    */
    if (radio.setCodingRate(params.lora_cr) == RADIOLIB_ERR_INVALID_CODING_RATE) {
        Serial.println(F("Selected coding rate is invalid for this module!"));
        return false;
    }

    /*
    * Sets LoRa sync word.
    * Sets LoRa sync word. Only available in LoRa mode.
    */
    if (radio.setSyncWord(params.lora_sw) != RADIOLIB_ERR_NONE) {
        Serial.println(F("Unable to set sync word!"));
        return false;
    }

    // Recalibrate maximum transmit power
    confirmTxPower(params.lora_tx_power);

    /*
    * Sets transmission output power.
    * The parameter ranges below are for the version without PA. The PA version uses the maximum power see CONFIG_RADIO_SUB1G_OUTPUT_POWER and CONFIG_RADIO_2G4_OUTPUT_POWER
    * SX1278/SX1276 :  Allowed values range from -3 to 15 dBm (RFO pin) or +2 to +17 dBm (PA_BOOST pin). High power +20 dBm operation is also supported, on the PA_BOOST pin. Defaults to PA_BOOST.
    * SX1262        :  Allowed values are in range from -9 to 22 dBm. This method is virtual to allow override from the SX1261 class.
    * SX1268        :  Allowed values are in range from -9 to 22 dBm.
    * SX1280        :  Allowed values are in range from -18 to 13 dBm. PA Version range : -18 ~ 3dBm
    * LR1121        :  Allowed values are in range from -17 to 22 dBm (high-power PA) or -18 to 13 dBm (High-frequency PA)
    * LR2021        :  Allowed values are in range from -9 to 22 dBm (sub-GHz PA) or -19 to 12 dBm (high-frequency PA).
    */
    if (radio.setOutputPower(params.lora_tx_power) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        return false;
    }


#if !defined(USING_SX1280) && !defined(USING_LR1121) && !defined(USING_SX1280PA) && !defined(USING_LR2021)
    /*
    * Sets current limit for over current protection at transmitter amplifier.
    * SX1278/SX1276 : Allowed values range from 45 to 120 mA in 5 mA steps and 120 to 240 mA in 10 mA steps.
    * SX1262/SX1268 : Allowed values range from 45 to 120 mA in 2.5 mA steps and 120 to 240 mA in 10 mA steps.
    * NOTE: set value to 0 to disable overcurrent protection
    */
    if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        Serial.println(F("Selected current limit is invalid for this module!"));
        return false;
    }
#endif

    /*
    * Sets preamble length for LoRa or FSK modem.
    * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
    * SX1262/SX1268/SX1280/LR1121/LR2021 : Allowed values range from 1 to 65535.
    */
    if (radio.setPreambleLength(params.lora_preamble_length) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
        Serial.println(F("Selected preamble length is invalid for this module!"));
        return false;
    }

    // Enables or disables CRC check of received packets.
    if (radio.setCRC(params.lora_crc) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
        Serial.println(F("Selected CRC is invalid for this module!"));
        return false;
    }

    return true;
}

static bool reloadLoRa()
{
    setupRfSwitch();

    // set the function that will be called
    radio.setPacketReceivedAction(setFlag);

    if (isContinuousWave) {
        int16_t state = radio.transmitDirect();
        if (state != RADIOLIB_ERR_NONE) {
            Serial.printf("transmitDirect:%d\n", state);
        }
        return true;
    }

    if (transmissionDirection == TRANSMISSION) {
        // Serial.println(F("Starting to transmit LoRa packets..."));
        int state = radio.startTransmit("Hello World!", 12);
        if (state == RADIOLIB_ERR_NONE) {
            // Serial.println(F("Transmitting LoRa packet..."));
        } else if (state == RADIOLIB_ERR_SPI_CMD_TIMEOUT) {
            Serial.println(F("SPI communication with LoRa module timed out!"));
            return false;
        } else {
            Serial.print(F("Failed to start transmission: "));
            Serial.println(state);
            return false;
        }
    } else if (transmissionDirection == RECEIVE) {
        // Serial.println(F("Starting to receive LoRa packets..."));
        int state = radio.startReceive();
        if (state == RADIOLIB_ERR_NONE) {
            // Serial.println(F("Waiting for LoRa packet..."));
        } else if (state == RADIOLIB_ERR_SPI_CMD_TIMEOUT) {
            Serial.println(F("SPI communication with LoRa module timed out!"));
            return false;
        } else {
            Serial.print(F("Failed to start reception: "));
            Serial.println(state);
            return false;
        }
    }

    return true;
}

static void confirmTxPower(int8_t &txPower, bool forcedHigh)
{
#if  defined(USING_LR1121) || defined(USING_LR2021)
    Serial.printf("Current Frequency: %.2f MHz set to %d dBm\n", lora_settings.lora_freq, txPower);
    if (lora_settings.lora_freq < 2400) {
        if (txPower > CONFIG_RADIO_SUB1G_OUTPUT_POWER || forcedHigh) {

            if (!forcedHigh) {
                Serial.print("Warning:sub1G max output power is limit to ");
                Serial.print(CONFIG_RADIO_SUB1G_OUTPUT_POWER);
                Serial.println(" dBm");
            } else {
                Serial.println("Forcing max output power to ");
                Serial.print(CONFIG_RADIO_SUB1G_OUTPUT_POWER);
                Serial.println(" dBm");
            }
            txPower = CONFIG_RADIO_SUB1G_OUTPUT_POWER;
        }
    } else {
        if (txPower > CONFIG_RADIO_2G4_OUTPUT_POWER || forcedHigh) {
            if (!forcedHigh) {
                Serial.print("Warning:2.4G max output power is limit to ");
                Serial.print(CONFIG_RADIO_2G4_OUTPUT_POWER);
                Serial.println(" dBm");
            } else {
                Serial.println("Forcing max output power to ");
                Serial.print(CONFIG_RADIO_2G4_OUTPUT_POWER);
                Serial.println(" dBm");
            }
            txPower = CONFIG_RADIO_2G4_OUTPUT_POWER;
        }
    }
#endif
}

static bool applySF(const String &v)
{
    int sf = v.toInt();
    if (radio.setSpreadingFactor(sf) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.printf("Invalid spreading factor: %d is not supported for this module.\n", sf);
        Serial.printf("  %s\n", SF_HELP);
        return false;
    }
    lora_settings.lora_sf = sf;
    Serial.printf("Spreading Factor set to %u\n", sf);
    return true;
}

static bool applyBW(const String &v)
{
    float bw = v.toFloat();
    if (radio.setBandwidth(bw) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.printf("Invalid bandwidth: %.1f kHz is not supported for this module.\n", bw);
        Serial.printf("  %s\n", BW_HELP);
        return false;
    }
    lora_settings.lora_bw = bw;
    Serial.printf("Bandwidth set to %.2f kHz\n", bw);
    return true;
}

static bool applySW(const String &v)
{
    int sw = (int)strtol(v.c_str(), nullptr, 0);
    if (sw == 0 && v != "0" && v != "0x0" && v != "0x00") {
        Serial.println("Invalid sync word: cannot parse given value.");
        return false;
    }
    if (radio.setSyncWord(sw) != RADIOLIB_ERR_NONE) {
        Serial.printf("Invalid sync word: 0x%02X is not supported for this module.\n", sw);
        Serial.printf("  %s\n", SW_HELP);
        return false;
    }
    lora_settings.lora_sw = sw;
    Serial.printf("SW set to 0x%02X (%u)\n", sw, sw);
    return true;
}

static bool applyCR(const String &v)
{
    int cr = v.toInt();
    if (radio.setCodingRate(cr) == RADIOLIB_ERR_INVALID_CODING_RATE) {
        Serial.printf("Invalid coding rate: %d is not supported for this module.\n", cr);
        Serial.printf("  %s\n", CR_HELP);
        return false;
    }
    lora_settings.lora_cr = cr;
    Serial.printf("Coding Rate set to %u\n", cr);
    return true;
}

static bool applyTP(const String &v)
{
    int8_t tp = v.toInt();

    // Recalibrate maximum transmit power
    confirmTxPower(tp);

    if (radio.setOutputPower(tp) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.printf("Invalid TX power: %d dBm is not supported for this module.\n", tp);
        Serial.printf("  %s\n", TP_HELP);
        return false;
    }
    lora_settings.lora_tx_power = tp;
    Serial.printf("Tx Power set to %d dBm\n", tp);
    return true;
}

static bool applyFreq(const String &v)
{
    float freq = v.toFloat();
    if (radio.setFrequency(freq) == RADIOLIB_ERR_INVALID_FREQUENCY) {
        Serial.printf("Invalid frequency: %.2f MHz is not supported for this module.\n", freq);
        Serial.printf("  %s\n", FREQ_HELP);
        return false;
    }

    // After changing the frequency, set it to the maximum power supported by that frequency.
    confirmTxPower(lora_settings.lora_tx_power, true);


    lora_settings.lora_freq = freq;
    Serial.printf("Frequency set to %.2f MHz\n", freq);
    return true;
}

static bool applyPL(const String &v)
{
    int pl = v.toInt();
    if (radio.setPreambleLength(pl) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
        Serial.printf("Invalid preamble length: %d is not supported for this module.\n", pl);
        Serial.printf("  %s\n", PL_HELP);
        return false;
    }
    lora_settings.lora_preamble_length = pl;
    Serial.printf("Preamble Length set to %d\n", pl);
    return true;
}

static bool applyCRC(const String &v)
{
    bool crc = (v == "1");
    if (radio.setCRC(crc) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
        Serial.println("Invalid CRC: CRC is not supported for this module.");
        return false;
    }
    lora_settings.lora_crc = crc;
    Serial.printf("LoRa CRC set to %s\n", lora_settings.lora_crc ? "true" : "false");
    return true;
}

static bool applyCWMode(const String &v)
{
    isContinuousWave = (v == "1");
#if !defined(USING_LR1121) && !defined(USING_LR2021)
    Serial.print("Begin Radio FSK ");
    int  state = radio.beginFSK();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("Radio FSK started successfully");
        return true;
    } else {
        Serial.print("Failed to start Radio FSK: ");
        Serial.println(state);
        return false;
    }
#endif
    return true;
}

static bool applyIQ(const String &v)
{
    bool iq = (v == "1");
    if (radio.invertIQ(iq) != RADIOLIB_ERR_NONE) {
        Serial.println("Failed to set IQ inversion.");
        return false;
    }
    lora_settings.iq_inverted = iq;
    return true;
}

static void printHelp()
{
    Serial.println(F("=========================================="));
    Serial.println(F("Available commands:"));
    Serial.println(F("  <key>:<value>  Set parameter (e.g. freq:868.0)"));
    Serial.println(F("--------------------------------------------------"));

    Serial.println(F("  freq:value     Set frequency in MHz"));
    Serial.println(F("  bw:value       Set bandwidth in kHz"));
    Serial.println(F("  sf:value       Set spreading factor"));
    Serial.println(F("  cr:value       Set coding rate"));
    Serial.println(F("  sw:value       Set sync word (hex: 0x12 or dec: 18)"));
    Serial.println(F("  tp:value       Set TX power in dBm"));
    Serial.println(F("  pl:value       Set preamble length"));
    Serial.println(F("  crc:1|0        Enable/disable CRC"));
    Serial.println(F("  cw:1|0         Enable/disable continuous wave"));
    Serial.println(F("  iq:1|0         Enable/disable IQ inversion"));
    Serial.println(F("--------------------------------------------------"));
    Serial.println(F("  show params    Print current LoRa parameters"));
    Serial.println(F("  wifi           Scan WiFi networks"));
    Serial.println(F("  help           Print this help message"));
    Serial.println(F("=========================================="));
}

struct ParamHandler {
    const char *key;
    const char *missingMsg;
    bool (*apply)(const String &);
    const char *helpText;
};

static const ParamHandler paramHandlers[] = {
    {"freq", "Error: missing value for freq",            applyFreq,   "freq:value     Set frequency in MHz"},
    {"bw",   "Error: missing value for bw",              applyBW,     "bw:value       Set bandwidth in kHz"},
    {"sf",   "Error: missing value for sf",              applySF,     "sf:value       Set spreading factor"},
    {"cr",   "Error: missing value for cr",              applyCR,     "cr:value       Set coding rate"},
    {"sw",   "Error: missing value for sw",              applySW,     "sw:value       Set sync word (hex: 0x12)"},
    {"tp",   "Error: missing value for tp",              applyTP,     "tp:value       Set TX power in dBm"},
    {"pl",   "Error: missing value for preamble length", applyPL,     "pl:value       Set preamble length"},
    {"crc",  "Error: missing value for crc",             applyCRC,    "crc:1|0        Enable/disable CRC"},
    {"iq",   "Error: missing value for iq",              applyIQ,     "iq:1|0         Enable/disable IQ inversion"},
    {"cw",   "Error: missing value for cw",              applyCWMode, "cw:1|0         Enable/disable continuous wave"},
};

static void handleWomCommands()
{
    if (!Serial.available()) {
        return;
    }

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) {
        return;
    }

    int colonPos = cmd.indexOf(':');
    String key = (colonPos == -1) ? cmd : cmd.substring(0, colonPos);
    String valStr = (colonPos == -1) ? "" : cmd.substring(colonPos + 1);
    valStr.trim();

    bool paramChanged = false;
    bool handled       = false;

    for (auto& h : paramHandlers) {
        if (key == h.key) {
            handled = true;
            if (valStr.length() == 0) {
                Serial.println(h.missingMsg);
                return;
            }
            radio.standby();
            if (h.apply(valStr)) {
                paramChanged = true;
            }
            break;
        }
    }

    if (!handled) {
        if (key == "show params") {
            printLoRaParams(lora_settings);
        } else if (key == "wifi") {
            scanWiFi();
        } else if (key == "help") {
            printHelp();
        } else {
            Serial.println("Unknown command");
        }
    }

    if (paramChanged) {
        reloadLoRa();
        Serial.println("LoRa parameters updated successfully");
        preferences.begin(pref_key, false);
        preferences.putBytes("lora_params", (const void*)&lora_settings, sizeof(lora_settings));
        preferences.end();
    }
}
