/**
 * @file      Factory.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2024-12-18
 * @note      This example is for T-beam V1.2, T-Beam-BPF, T-Beam SUPREME factory examples
 * @note      This example is for T-beam V1.2, T-Beam-BPF, T-Beam SUPREME factory examples
 * @note      This example is for T-beam V1.2, T-Beam-BPF, T-Beam SUPREME factory examples
 * @note      This example is for T-beam V1.2, T-Beam-BPF, T-Beam SUPREME factory examples
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
#include <MadgwickAHRS.h>       //MadgwickAHRS from https://github.com/arduino-libraries/MadgwickAHRS
#include "Roboto_Mono_Medium_12.h"
#include "LoRaBoards.h"

// #define JAPAN_MIC_CERTIFICATION

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
#include <SensorPCF8563.hpp>

SensorQMC6310  qmc;
SensorQMI8658  qmi;
SensorPCF8563  rtc;
Adafruit_BME280 bme;
#endif

using namespace ace_button;

void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void radioRx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void wirelessInfo(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void wifiTask(void *task);
void hwProbe(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);

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
static void beginSensor();
#else
#define beginSensor()
#endif


#if     defined(USING_SX1276)

#ifdef T_BEAM_S3_BPF
// BPF Freq range : 144Mhz ~ 148MHz
#define CONFIG_RADIO_FREQ           144.0
#else  /*T_BEAM_S3_BPF*/
#define CONFIG_RADIO_FREQ           868.0
#endif /*T_BEAM_S3_BPF*/

#define CONFIG_RADIO_OUTPUT_POWER   17
#define CONFIG_RADIO_BW             125.0

SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1278)
#ifdef T_BEAM_S3_BPF
// BPF Freq range : 144Mhz ~ 148MHz
#define CONFIG_RADIO_FREQ           144.0
#else
#define CONFIG_RADIO_FREQ           433.0
#endif /*T_BEAM_S3_BPF*/

#define CONFIG_RADIO_OUTPUT_POWER   17
#define CONFIG_RADIO_BW             125.0
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

#elif   defined(USING_SX1262)
#define CONFIG_RADIO_FREQ           868.0
#define CONFIG_RADIO_OUTPUT_POWER   22
#define CONFIG_RADIO_BW             125.0

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_SX1280)
#define CONFIG_RADIO_FREQ           2400.0
#define CONFIG_RADIO_OUTPUT_POWER   13
#define CONFIG_RADIO_BW             203.125
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif  defined(USING_SX1280PA)
#define CONFIG_RADIO_FREQ           2400.0
#define CONFIG_RADIO_OUTPUT_POWER   3           // PA Version power range : -18 ~ 3dBm
#define CONFIG_RADIO_BW             203.125
SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#elif   defined(USING_LR1121)

// The maximum power of LR1121 2.4G band can only be set to 13 dBm
#define CONFIG_RADIO_FREQ           2450.0
#define CONFIG_RADIO_OUTPUT_POWER   13
#define CONFIG_RADIO_BW             125.0

// The maximum power of LR1121 Sub 1G band can only be set to 22 dBm
// #define CONFIG_RADIO_FREQ           868.0
// #define CONFIG_RADIO_OUTPUT_POWER   22
// #define CONFIG_RADIO_BW             125.0

LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO9_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif  /*Radio option*/


#ifdef JAPAN_MIC_CERTIFICATION
#undef  CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           920.0
#endif


enum TransmissionDirection {
    TRANSMISSION,
    RECEIVE,
};

// save transmission state between loops
int             transmissionState = RADIOLIB_ERR_NONE;
bool            transmittedFlag = false;
uint32_t        transmissionCounter = 0;
TransmissionDirection  transmissionDirection = TRANSMISSION;
bool            wifi_is_config = false;
bool            is_time_available = false;
DISPLAY_MODEL_SSD_LIB     display(0x3c, I2C_SDA, I2C_SCL);
OLEDDisplayUi   ui( &display );
bool            led_blink = false;
bool            update_use_second = false;
uint32_t        gps_use_second = 0;
uint32_t        gps_start_ms = 0;

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
    wirelessInfo
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

    display.clear();
    display.drawString(60, 28, "Sleep");
    display.display();
    delay(2000);
    display.displayOff();


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

#ifdef SerialGPS
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
     * | T-Beam  2W v1.0   | ~ 442 uA |
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
#ifdef RADIO_TX_CW
        radio.standby();
#endif
        break;
    case 1:
#ifdef RADIO_CTRL
        Serial.println("Turn off LAN,Trun on PA, Enter Tx mode.");
        /*
        * 2W and BPF LoRa LAN Control ,set Low turn off LAN , TX Mode
        * */
        digitalWrite(RADIO_CTRL, LOW);
#endif /*RADIO_CTRL*/

#ifdef RADIO_TX_CW
        radio.transmitDirect();
#else /*RADIO_TX_CW*/
        Serial.println("Start transmit");
        transmissionDirection = TRANSMISSION;
        transmissionState = radio.transmit((uint8_t *)&transmissionCounter, 4);
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.print(F("[Radio] transmit packet failed! err:"));
            Serial.println(transmissionState);
        }
#endif /*RADIO_TX_CW*/
        break;

    case 2:
#ifdef RADIO_CTRL
        Serial.println("Turn on LAN, Enter Rx mode.");
        /*
        * 2W and BPF LoRa LAN Control ,set HIGH turn on LAN ,RX Mode
        * */
        digitalWrite(RADIO_CTRL, HIGH);
#endif /*RADIO_CTRL*/
        Serial.println("Start receive");
        transmissionDirection = RECEIVE;
        transmissionState = radio.startReceive();
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.println(F("[Radio] Received packet failed!"));
        }
        break;

    default:
#ifdef RADIO_CTRL
        Serial.println("Turn on LAN, Enter Rx mode.");
        /*
        * 2W and BPF LoRa LAN Control ,set HIGH turn on LAN ,RX Mode
        * */
        digitalWrite(RADIO_CTRL, HIGH);
#endif /*RADIO_CTRL*/
        break;
    }
    ui.transitionToFrame(currentFrames);
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
    case AceButton::kEventLongPressed:
        sleepDevice();
        break;
    default:
        break;
    }
}

void nextButtonHandleEvent(AceButton   *button, uint8_t eventType, uint8_t buttonState)
{
    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.printf("nextButtonHandleEvent currentFrames:%d frames_count:%d\n", currentFrames, max_frames);
        currentFrames =  ((currentFrames - 1) < 0) ? currentFrames : currentFrames - 1;
        handleMenu();
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

void timeavailable(struct timeval *t)
{
    Serial.println("[WiFi]: Got time adjustment from NTP!");
    is_time_available = true;

#ifdef T_BEAM_S3_SUPREME
    if (deviceOnline & PCF8563_ONLINE) {
        rtc.hwClockWrite();
    }
#endif /*T_BEAM_S3_SUPREME*/
}

void setup()
{
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

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
    }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

    WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.wifi_sta_disconnected.reason);
    }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    setupBoards(true);

    setupBLE();

    beginSensor();

    if (String(WIFI_SSID) == "Your WiFi SSID" || String(WIFI_PASSWORD) == "Your WiFi PASSWORD" ) {
        Serial.println("[Error] : WiFi ssid and password are not configured correctly");
        Serial.println("[Error] : WiFi ssid and password are not configured correctly");
        Serial.println("[Error] : WiFi ssid and password are not configured correctly");
    } else {
        wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
        wifi_is_config = true;
    }

    // set notification call-back function
    sntp_set_time_sync_notification_cb( timeavailable );

    /**
     * This will set configured ntp servers and constant TimeZone/daylightOffset
     * should be OK if your time zone does not need to adjust daylightOffset twice a year,
     * in such a case time adjustment won't be handled automagically.
     */
    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
    configTzTime("CST-8", "cn.ntp.org.cn", "pool.ntp.org", "time.nist.gov");

    uint8_t mac[6];
    char buffer[18] = { 0 };
    esp_efuse_mac_get_default(mac);
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    macStr = buffer;
    delay(1000);

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    static ButtonConfig prevButtonConfigure;
    prevButtonConfigure.setEventHandler(prevButtonHandleEvent);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureClick);
    prevButtonConfigure.setFeature(ButtonConfig::kFeatureLongPress);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    button.init(BUTTON_PIN);
    button.setButtonConfig(&prevButtonConfigure);


#ifdef BUTTON2_PIN
    static ButtonConfig nextButtonConfigure;
    nextButtonConfigure.setEventHandler(nextButtonHandleEvent);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureClick);
    nextButtonConfigure.setFeature(ButtonConfig::kFeatureLongPress);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    button2.init(BUTTON2_PIN);
    button2.setButtonConfig(&nextButtonConfigure);
#endif /*BUTTON2_PIN*/


    // Initialising the UI will init the display too.
    ui.setTargetFPS(60);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    // ui.setIndicatorPosition(BOTTOM);
    ui.disableAllIndicators();

    // Defines where the first frame is located in the bar.
    ui.setIndicatorDirection(LEFT_RIGHT);

    // You can change the transition that is used
    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
    ui.setFrameAnimation(SLIDE_LEFT);

    // Add frames
    ui.setFrames(frames, sizeof(frames) / sizeof(frames[0]));

    ui.disableAutoTransition();

    // Initialising the UI will init the display too.
    ui.init();

    display.flipScreenVertically();

    Serial.printf("[%s]:", RADIO_TYPE_STR);
    Serial.print(F(" Initializing Radio ... "));

#if defined(RADIO_TX_CW) && !defined(USING_LR1121)
    Serial.println("Begin Radio FSK.");
    int  state = radio.beginFSK();
#else
    Serial.println("Begin Radio LoRa.");
    int  state = radio.begin();
#endif
    if ( state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
        deviceOnline |= RADIO_ONLINE;
    } else {
        Serial.println(F("failed!"));
    }

    Serial.printf("Freq:%.2f TxPower:%d Bandwidth:%.2f\n", CONFIG_RADIO_FREQ, CONFIG_RADIO_OUTPUT_POWER, CONFIG_RADIO_BW);

#if defined(RADIO_RX_PIN) && defined(RADIO_TX_PIN)
    //The SX1280 version needs to set RX, TX antenna switching pins
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);
#endif

    if (state == RADIOLIB_ERR_NONE) {
        /*
         *   Sets carrier frequency.
         *   SX1278/SX1276 : Allowed values range from 137.0 MHz to 525.0 MHz.
         *   SX1268/SX1262 : Allowed values are in range from 150.0 to 960.0 MHz.
         *   SX1280        : Allowed values are in range from 2400.0 to 2500.0 MHz.
         *   LR1121        : Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
         * * * */

        if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY) {
            Serial.println(F("Selected frequency is invalid for this module!"));
            while (true);
        }

#ifndef RADIO_TX_CW
        /*
        *   Sets LoRa link bandwidth.
        *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
        *   SX1268/SX1262 : Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
        *   SX1280        : Allowed values are 203.125, 406.25, 812.5 and 1625.0 kHz.
        *   LR1121        : Allowed values are 62.5, 125.0, 250.0 and 500.0 kHz.
        * * * */
        if (radio.setBandwidth(CONFIG_RADIO_BW) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            while (true);
        }


        /*
        * Sets LoRa link spreading factor.
        * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
        * SX1262        :  Allowed values range from 5 to 12.
        * SX1280        :  Allowed values range from 5 to 12.
        * LR1121        :  Allowed values range from 5 to 12.
        * * * */
        if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
            Serial.println(F("Selected spreading factor is invalid for this module!"));
            while (true);
        }

        /*
        * Sets LoRa coding rate denominator.
        * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
        * SX1280        :  Allowed values range from 5 to 8.
        * LR1121        :  Allowed values range from 5 to 8.
        * * * */
        if (radio.setCodingRate(6) == RADIOLIB_ERR_INVALID_CODING_RATE) {
            Serial.println(F("Selected coding rate is invalid for this module!"));
            while (true);
        }

        /*
        * Sets LoRa sync word.
        * SX1278/SX1276/SX1268/SX1262/SX1280 : Sets LoRa sync word. Only available in LoRa mode.
        * * */
        if (radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
            Serial.println(F("Unable to set sync word!"));
            while (true);
        }
#endif /*RADIO_TX_CW*/

        /*
        * Sets transmission output power.
        * SX1278/SX1276 :  Allowed values range from -3 to 15 dBm (RFO pin) or +2 to +17 dBm (PA_BOOST pin). High power +20 dBm operation is also supported, on the PA_BOOST pin. Defaults to PA_BOOST.
        * SX1262        :  Allowed values are in range from -9 to 22 dBm. This method is virtual to allow override from the SX1261 class.
        * SX1268        :  Allowed values are in range from -9 to 22 dBm.
        * SX1280        :  Allowed values are in range from -18 to 13 dBm. PA Version range : -18 ~ 3dBm
        * LR1121        :  Allowed values are in range from -17 to 22 dBm (high-power PA) or -18 to 13 dBm (High-frequency PA)
        * * * */
        if (radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
            Serial.println(F("Selected output power is invalid for this module!"));
            while (true);
        }

#if !defined(USING_SX1280) && !defined(USING_LR1121) && !defined(USING_SX1280PA)
        /*
        * Sets current limit for over current protection at transmitter amplifier.
        * SX1278/SX1276 : Allowed values range from 45 to 120 mA in 5 mA steps and 120 to 240 mA in 10 mA steps.
        * SX1262/SX1268 : Allowed values range from 45 to 120 mA in 2.5 mA steps and 120 to 240 mA in 10 mA steps.
        * NOTE: set value to 0 to disable overcurrent protection
        * * * */
        if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
            Serial.println(F("Selected current limit is invalid for this module!"));
            while (true);
        }
#endif

        /*
        * Sets preamble length for LoRa or FSK modem.
        * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
        * SX1262/SX1268 : Allowed values range from 1 to 65535.
        * SX1280        : Allowed values range from 1 to 65535.
        * LR1121        : Allowed values range from 1 to 65535.
        * * */
        if (radio.setPreambleLength(15) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
            Serial.println(F("Selected preamble length is invalid for this module!"));
            while (true);
        }

        // Enables or disables CRC check of received packets.
        if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
            Serial.println(F("Selected CRC is invalid for this module!"));
            while (true);
        }


#if  defined(USING_LR1121)
        // LR1121
        // set RF switch configuration for Wio WM1110
        // Wio WM1110 uses DIO5 and DIO6 for RF switching
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

        // LR1121 TCXO Voltage 2.85~3.15V
        radio.setTCXO(3.0);

#endif

#if  defined(USING_SX1262)
        radio.setDio2AsRfSwitch(true);
#endif

        // set the function that will be called
        // when new packet is received
        radio.setPacketReceivedAction(setFlag);

        // start listening for LoRa packets
        Serial.print(F("[Radio] Starting to listen ... "));
        state = radio.startReceive();
        if (state != RADIOLIB_ERR_NONE) {
            Serial.println(F("[Radio] Received packet failed!"));
        }
    }


    if (wifi_is_config) {
        xTaskCreate(wifiTask, "wifi", 4 * 2048, NULL, 10, NULL);
    }

    // Record GPS start time
    gps_start_ms = millis();
}

// PMU Power key callback
static uint8_t freq_index = 0;
const float factory_freq[] = {433.0, 470.0, 850.0, 868.0, 915.0, 923.0
#if   defined(USING_LR1121)
                              , 2400, 2450
#endif

                             };
float current_freq = CONFIG_RADIO_FREQ;

void power_key_pressed()
{
#if defined(JAPAN_MIC_CERTIFICATION) || defined(T_BEAM_S3_BPF)
    // Turn on/off display
    static bool isOn = true;
    isOn ? display.displayOff()  : display.displayOn();
    isOn ^= 1;
    return;
#else /*defined(JAPAN_MIC_CERTIFICATION) || defined(T_BEAM_S3_BPF)*/

    // Set freq function
    radio.standby();
#if  defined(USING_LR1121)
    // check if we need to recalibrate image
    bool skipCalibration = true;
    int16_t state =  radio.setFrequency(factory_freq[freq_index], skipCalibration);
#else /*defined(USING_LR1121)*/
    int16_t state =  radio.setFrequency(factory_freq[freq_index]);
#endif /*defined(USING_LR1121)*/

    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("Selected frequency %.2f is invalid for this module!\n", factory_freq[freq_index]);
        return;
    }
    current_freq = factory_freq[freq_index];
    Serial.printf("setFrequency:%.2f\n", current_freq);
    freq_index++;
    freq_index %= sizeof(factory_freq) / sizeof(factory_freq[0]);

#if   defined(USING_LR1121)
    bool forceHighPower = false;
    int8_t max_tx_power = 13;
    if (current_freq < 2400) {
        max_tx_power = 22;
        forceHighPower = true;
    }
    if (radio.setOutputPower(max_tx_power, forceHighPower) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.printf("Selected output power %d is invalid for this module!\n", max_tx_power);
    }
#endif /*defined(USING_LR1121)*/

    switch (transmissionDirection) {
    case TRANSMISSION:
#ifdef RADIO_TX_CW
        radio.transmitDirect();
#else /*RADIO_TX_CW*/
        transmissionState = radio.transmit((uint8_t *)&transmissionCounter, 4);
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.println(F("[Radio] transmit packet failed!"));
        }
#endif /*RADIO_TX_CW*/
        break;
    default:
        transmissionState = radio.startReceive();
        if (transmissionState != RADIOLIB_ERR_NONE) {
            Serial.println(F("[Radio] Received packet failed!"));
        }
        break;
    }
#endif /*defined(JAPAN_MIC_CERTIFICATION) || defined(T_BEAM_S3_BPF)*/
}

void loop()
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

#ifdef HAS_PMU
    loopPMU(power_key_pressed);
#endif

    button.check();

#ifdef BUTTON2_PIN
    button2.check();
#endif

    ui.update();
    delay(2);
}

void radioTx(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    static uint32_t interval = 0;
    display->setTextAlignment(TEXT_ALIGN_LEFT);

#ifndef RADIO_TX_CW
    if (millis() > interval && transmissionDirection == TRANSMISSION ) {
        if (transmittedFlag) {
            // reset flag
            transmittedFlag = false;
            if (transmissionState == RADIOLIB_ERR_NONE) {
                // packet was successfully sent
                Serial.println(F("transmission finished!"));

                // NOTE: when using interrupt-driven transmit method,
                //       it is not possible to automatically measure
                //       transmission data rate using getDataRate()

            } else {
                Serial.print(F("failed, code "));
                Serial.println(transmissionState);

            }

            // clean up after transmission is finished
            // this will ensure transmitter is disabled,
            // RF switch is powered down etc.
            radio.finishTransmit();

            // send another one
            Serial.print(F("[Radio] Sending another packet ... "));

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
#ifdef BOARD_LED
            digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));
#endif
#ifdef HAS_PMU
            if (PMU) {
                PMU->setChargingLedMode(led_blink);
                led_blink ^= 1;
            }
#endif
        }

        interval = millis() + 1000;

    }
#endif
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
        display->drawString(0 + x, 16 + y, "Freq:" + String(current_freq) + "MHz");
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
            Serial.println(F("[Radio] Received packet!"));
            radioRSSI = radio.getRSSI();

        } else if (transmissionState == RADIOLIB_ERR_CRC_MISMATCH) {
            // packet was received, but is malformed
            Serial.println(F("[Radio] CRC error!"));

        } else {
            // some other error occurred
            Serial.print(F("[Radio] Failed, code "));
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
        display->drawString(0 + x, 16 + y, "Freq:" + String(current_freq) + "MHz");
        display->drawString(0 + x, 32 + y, "RX :" + String(recvCounter));
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
    display->drawString(display->width()  + x,  3  + y, (deviceOnline & QMC6310_ONLINE ) ? "+" : "-");
    display->drawString(display->width()  + x,  15 + y, (deviceOnline & BME280_ONLINE ) || (deviceOnline & BMP280_ONLINE ) ? "+" : "-");
    display->drawString(display->width()  + x,  27 + y, (deviceOnline & PSRAM_ONLINE )   ? "+" : "-");
    display->drawString(display->width()  + x,  39 + y, (deviceOnline & SDCARD_ONLINE )  ? "+" : "-");
    display->drawString(display->width()  + x,  51 + y, (deviceOnline & OSC32768_ONLINE ) ? "+" : "-");

#else
    display->drawString(x, 16 + y, "Radio");
    display->drawString(x, 32 + y, "GPS");
    display->drawString(x, 48 + y, "OLED");

    display->setTextAlignment(TEXT_ALIGN_CENTER);
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

    if (update_use_second) {
        display->drawString(48 + x, 0 + y, "GPS Use" + String(gps_use_second) + "S");
    } else {
        display->drawString(64 + x, 0 + y, "GPS");
    }
    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {

        if (!update_use_second) {
            update_use_second = true;
            gps_use_second = (millis() - gps_start_ms) / 1000;
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
    if (millis() > interval) {
        // analogReadResolution(12); //Default is 12 bits, not need set
        const float vRef = 3.3;
        const int adcResolution = 4095;
        uint16_t  adcValue = analogRead(ADC_PIN);
        float dividedVoltage = (float)adcValue / adcResolution * vRef;
        float batteryVoltage = dividedVoltage * (BAT_ADC_PULLUP_RES + BAT_ADC_PULLDOWN_RES) / BAT_ADC_PULLDOWN_RES;
        batteryVoltage += BAT_VOL_COMPENSATION; // Voltage compensation
        snprintf(buffer, sizeof(buffer), "VOL:%.2f V", batteryVoltage > BAT_MAX_VOLTAGE ? BAT_MAX_VOLTAGE : batteryVoltage);
        interval = millis() + 1000;
    }
    display->drawString(64 + x, 0 + y, "Battery");
    display->drawString(64 + x, 32 + y, buffer);
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


static const int centreX = 32;
static const int centreY = 40;
static const int radius  = 10;
Madgwick  filter;

int last_dx = centreX, last_dy = centreY, dx, dy;

void arrow(OLEDDisplay *display, int offsetX, int offsetY, int x2, int y2, int x1, int y1, int alength, int awidth, OLEDDISPLAY_COLOR  color)
{
    display->setColor(color);
    float distance;
    int dx, dy, x2o, y2o, x3, y3, x4, y4, k;
    distance = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
    dx = x2 + (x1 - x2) * alength / distance;
    dy = y2 + (y1 - y2) * alength / distance;
    k = awidth / alength;
    x2o = x2 - dx;
    y2o = dy - y2;
    x3 = y2o * k + dx;
    y3 = x2o * k + dy;
    x4 = dx - y2o * k;
    y4 = dy - x2o * k;
    display->drawLine(x1 + offsetX, y1 + offsetY, offsetX + x2, offsetY + y2);
    display->drawLine(x1 + offsetX, y1 + offsetY, offsetX + dx, offsetY + dy);
    display->drawLine(x3 + offsetX, y3 + offsetY, offsetX + x4, offsetY + y4);
    display->drawLine(x3 + offsetX, y3 + offsetY, offsetX + x2, offsetY + y2);
    display->drawLine(x2 + offsetX, y2 + offsetY, offsetX + x4, offsetY + y4);
}

int drawCompass(OLEDDisplay *display, int16_t x, int16_t y, float magX, float magY)
{
    int angle;
    display->drawString(x + 29, y + 16, "N");
    display->drawString(x + 0,  y + 32, "W");
    display->drawString(x + 58, y + 32, "E");
    display->drawString(x + 29, y + 50, "S");

    float heading = atan2(magY, magX); // Result is in radians
    // Now add the 'Declination Angle' for you location. Declination is the variation in magnetic field at your location.
    // Find your declination here: http://www.magnetic-declination.com/
    // At my location it is :  -2° 20' W, or -2.33 Degrees, which needs to be in radians so = -2.33 / 180 * PI = -0.041 West is + E is -
    // Make declination = 0 if you can't find your Declination value, the error is negible for nearly all locations
    float declination = -0.041;
    heading = heading + declination;
    if (heading < 0)    heading += 2 * PI; // Correct for when signs are reversed.
    if (heading > 2 * PI) heading -= 2 * PI; // Correct for when heading exceeds 360-degree, especially when declination is included
    angle = int(heading * 180 / M_PI); // Convert radians to degrees for more a more usual result
    // For the screen -X = up and +X = down and -Y = left and +Y = right, so does not follow coordinate conventions
    dx = (0.7 * radius * cos((angle - 90) * 3.14 / 180)) + centreX + x; // calculate X position for the screen coordinates - can be confusing!
    dy = (0.7 * radius * sin((angle - 90) * 3.14 / 180)) + centreY + y; // calculate Y position for the screen coordinates - can be confusing!
    arrow(display, x, y,  last_dx,  last_dy,  centreX + x,  centreY + y, 2, 2, BLACK); // Erase last arrow
    arrow(display, x, y,  dx,  dy,  x + centreX,  centreY + y, 2, 2, WHITE); // Draw arrow in new position
    return angle;
}

void getMagData(float *x, float *y)
{
    float z = 0;
    if (qmc.isDataReady()) {
        qmc.readData();
        qmc.getMag(*x, *y, z);
    }
}

void imuInfo(OLEDDisplay *display, OLEDDisplayUiState *disp_state, int16_t x, int16_t y)
{
    static float  magX = 0, magY = 0;
    static int angle = 0;
    static float roll, pitch, heading;

    getMagData(&magX, &magY);
    angle = drawCompass(display, x, y, magX, magY);

    display->setFont(Roboto_Mono_Medium_12);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 0 + y, "IMU");

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setColor(BLACK);
    display->fillRect(x + 80, y + 16, 25, 50);
    display->setColor(WHITE);
    display->setFont(Roboto_Mono_Medium_12);
    display->drawString(x + 75, y + 16, String(angle) + "°");

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
        // Serial.printf("roll:%.2f pitch:%.2f heading:%.2f\n", roll, pitch, heading);
    }

    display->drawString(x + 75, y + 32, String(heading) + "°");
}

static void beginSensor()
{
    // PMU and RTC share I2C bus
    if (!rtc.begin(PMU_WIRE_PORT, PCF8563_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
        Serial.println("Failed to find PCF8563 - check your wiring!");
    }
    if (!qmc.begin(Wire, QMC6310_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
        Serial.println("Failed to find QMC6310 - check your wiring!");
    } else {
        qmc.configMagnetometer(
            SensorQMC6310::MODE_CONTINUOUS,
            SensorQMC6310::RANGE_8G,
            SensorQMC6310::DATARATE_200HZ,
            SensorQMC6310::OSR_1,
            SensorQMC6310::DSR_1);
    }
    if (!bme.begin()) {
        Serial.println("Failed to find BME280 - check your wiring!");
    }

    pinMode(SPI_CS, OUTPUT);    //sdcard pin set high
    digitalWrite(SPI_CS, HIGH);


    pinMode(IMU_INT, INPUT_PULLUP); //IMU set interrupt pin
    // SDCard shares SPI bus with QMI8658
    // SPI has been initialized in initBoard.
    // Only need to pass SPIhandler to the QMI class.
    if (!qmi.begin(IMU_CS, -1, -1, -1, SDCardSPI)) {
        Serial.println("Failed to find QMI8658 - check your wiring!");
    } else {
        /* Get chip id*/
        Serial.print("Device ID:");
        Serial.println(qmi.getChipID(), HEX);
        deviceOnline |= QMI8658_ONLINE;

        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_1000Hz,
            SensorQMI8658::LPF_MODE_0,
            true);
        qmi.configGyroscope(
            SensorQMI8658::GYR_RANGE_256DPS,
            SensorQMI8658::GYR_ODR_896_8Hz,
            SensorQMI8658::LPF_MODE_3,
            true);


        // In 6DOF mode (accelerometer and gyroscope are both enabled),
        // the output data rate is derived from the nature frequency of gyroscope
        qmi.enableGyroscope();
        qmi.enableAccelerometer();

        // Enable data ready to interrupt pin2
        qmi.enableINT(SensorQMI8658::IntPin2);
        qmi.enableDataReadyINT();
    }

    // start  filter
    filter.begin(25);
}
#endif

void wifiTask(void *task)
{
    while (1) {
        wifiMulti.run();
        if (is_time_available) {
            Serial.println("---REMOVE TASK---");
            vTaskDelete(NULL);
        }
        delay(1000);
    }
}
