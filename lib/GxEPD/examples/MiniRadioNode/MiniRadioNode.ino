/**
 * @file      MiniRadioNode.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022
 * @date      2022-05-31
 * @note      Using TTGO MINI E-Paper 1.02" epaper , Send sensor value via nRF24 to LilyPi center ,
 *            Operating power consumption is about ~10mA, sleep current is about ~22uA
 *
 */

#define LILYGO_T5_V102                              //Using TTGO MINI E-Paper 1.02" epaper

#define USING_BME280_SENSOR                         //Using bme280 sensor , attach to SDA:18  SCL:33
#define TIME_TO_SLEPP_MINUTE        10              //Sleep time unit : minutes


#include <boards.h>
#include <GxEPD.h>                      //https://github.com/lewisxhe/GxEPD
#include <U8g2_for_Adafruit_GFX.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <Wire.h>
#include <Preferences.h>
#include <WiFi.h>
#include <RadioLib.h>                   //https://github.com/jgromes/RadioLib
#include <AceButton.h>                  //https://github.com/bxparks/AceButton
#include <SD.h>
#include <FS.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w 

#if defined(USING_BME280_SENSOR)
#include "SparkFunBME280.h"             //https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
BME280 mySensor;
#endif


using namespace ace_button;

void updateUI(uint8_t type, float value);
void setupDisplay();
float getSensorValue();


AceButton buttons[BUTTON_COUNT];
const uint8_t button_pin[BUTTON_COUNT] = BUTTONS;       //{36,39,0}
Preferences pref;
SPIClass SPI2(HSPI);        //Sdcard using hspi


// Radio shiled pins
#define RADIO_MOSI                (23)
#define RADIO_MISO                (38)
#define RADIO_SCLK                (26)
#define RADIO_CS                  (25)
#define RADIO_IRQ                 (37)
#define RADIO_CE                  (12)

#define BOARD_SDA                 (18)
#define BOARD_SCL                 (33)

enum SensorValType {
    SENSOR_TYPE_HUMIDITY,
    SENSOR_TYPE_TEMPERATURE,
} ;

nRF24 radio = new Module(RADIO_CS, RADIO_IRQ, RADIO_CE);
GxIO_Class io(EPD_SCLK, EPD_MISO, EPD_MOSI, EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;


static uint8_t addr[] = {0x00, 0x23, 0x45, 0x67, 0x89};         //RF configuration address

static uint8_t  option = 0;
static uint8_t  sleepEn = false;

static uint32_t startMillis     = 0;
const  uint32_t updatePeriod    = 30000;        // Update cycle when sleep is not configured
static uint32_t updateMillis    = 0;
static bool sensor_success      = false;

#define UPDATE_OPR_TIMER        (startMillis = millis() + 6000)     //Update operation timestamp

//Image generator : https://image2lcd.software.informer.com/2.9/

const unsigned char gImage_celsius[128] = { /* 0X00,0X01,0X20,0X00,0X20,0X00, */
    0X00, 0X00, 0X00, 0X00, 0X3F, 0XFF, 0XFF, 0XFC, 0X7F, 0XFF, 0XFF, 0XFE, 0X60, 0X00, 0X00, 0X06,
    0X60, 0X00, 0X00, 0X06, 0X60, 0X00, 0X00, 0X06, 0X60, 0X40, 0X00, 0X06, 0X61, 0XF0, 0X00, 0X06,
    0X61, 0XF0, 0X00, 0X06, 0X63, 0XB0, 0XFC, 0X06, 0X61, 0XF1, 0XFE, 0X06, 0X61, 0XF3, 0XFF, 0X06,
    0X60, 0X03, 0XC6, 0X06, 0X60, 0X07, 0X80, 0X06, 0X60, 0X07, 0X00, 0X06, 0X60, 0X07, 0X00, 0X06,
    0X60, 0X07, 0X00, 0X06, 0X60, 0X07, 0X00, 0X06, 0X60, 0X07, 0X82, 0X06, 0X60, 0X03, 0XC7, 0X06,
    0X60, 0X03, 0XFF, 0X86, 0X60, 0X01, 0XFF, 0X06, 0X60, 0X00, 0XFE, 0X06, 0X60, 0X00, 0X00, 0X06,
    0X60, 0X00, 0X00, 0X06, 0X60, 0X00, 0X00, 0X06, 0X60, 0X00, 0X00, 0X06, 0X60, 0X00, 0X00, 0X06,
    0X60, 0X00, 0X00, 0X06, 0X7F, 0XFF, 0XFF, 0XFE, 0X3F, 0XFF, 0XFF, 0XFC, 0X00, 0X00, 0X00, 0X00,
};


const unsigned char gImage_humidity[134] = {
    /*0X00, 0X01, 0X20, 0X00, 0X20, 0X00,*/
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X70, 0X00, 0X00, 0X00, 0X70, 0X00, 0X00, 0X00, 0X78, 0X00, 0X00, 0X00, 0XFC, 0X00, 0X00,
    0X01, 0XDC, 0X00, 0X00, 0X01, 0XCE, 0X00, 0X00, 0X03, 0X8E, 0X00, 0X00, 0X03, 0X86, 0X18, 0X00,
    0X03, 0X8E, 0X38, 0X00, 0X01, 0XFE, 0X3C, 0X00, 0X01, 0XFC, 0X3C, 0X00, 0X00, 0X78, 0X7E, 0X00,
    0X00, 0X00, 0XFE, 0X00, 0X00, 0X01, 0XE7, 0X00, 0X00, 0X01, 0XC7, 0X80, 0X00, 0X03, 0X83, 0X80,
    0X00, 0X03, 0XE1, 0XC0, 0X00, 0X03, 0X61, 0XC0, 0X00, 0X03, 0X61, 0XC0, 0X00, 0X03, 0X71, 0XC0,
    0X00, 0X03, 0XFB, 0X80, 0X00, 0X01, 0XFF, 0X80, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X7C, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

void setupPreferences()
{
    pref.begin("opt.db");
    option = pref.getUChar("opt", 0);
    sleepEn = pref.getUChar("sleep", 0);

    if (sleepEn) {
        Serial.println("sleep en is enable !");
    }
}


bool setupRadio()
{
    SPI.begin(RADIO_SCLK, RADIO_MISO, RADIO_MOSI);

    // initialize nRF24 with default settings
    Serial.print(F("[nRF24] Initializing ... "));
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        return false;
    }
    // set transmit address
    // NOTE: address width in bytes MUST be equal to the
    //       width set in begin() or setAddressWidth()
    //       methods (5 by default)
    // byte addr[] = {0x01, 0x23, 0x45, 0x67, 0x89};
    Serial.print(F("[nRF24] Setting transmit pipe ... "));
    state = radio.setTransmitPipe(addr);
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        return false;
    }

    return true;
}

void transmit(float value)
{
    Serial.printf("[nRF24] Transmitting packet ... %.2f\n", value);

    const uint8_t transmitBytes = 5;

    uint8_t pipeAddress = 0;

    uint8_t buffer[transmitBytes] = {addr[0], 0};

    //TODO:Classification transfer flag
    // switch (option) {
    // case SENSOR_TYPE_TEMPERATURE:
    //     break;
    // case SENSOR_TYPE_HUMIDITY:
    //     break;
    // default:
    //     break;
    // }

    memcpy(&buffer[1], &value, 4);

    int state = radio.startTransmit(buffer, transmitBytes, pipeAddress);
    if (state == RADIOLIB_ERR_NONE) {
        // the packet was successfully transmitted
        Serial.println(F("success!"));

    } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        // the supplied packet was longer than 32 bytes
        Serial.println(F("too long!"));

    } else if (state == RADIOLIB_ERR_ACK_NOT_RECEIVED) {
        // acknowledge from destination module
        // was not received within 15 retries
        Serial.println(F("ACK not received!"));

    } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
        // timed out while transmitting
        Serial.println(F("timeout!"));
    } else {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

// The event handler for the button.
void buttonHandleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
    uint8_t id = button->getId();
    Serial.print(F("handleEvent(): eventType: "));
    Serial.print(eventType);
    Serial.print(F("; buttonState: "));
    Serial.print(buttonState);
    Serial.print(" ID:");
    Serial.println(id);
    switch (id) {
    case 0:
        //Temperature
        if (AceButton::kEventReleased == eventType ) {
            option = SENSOR_TYPE_TEMPERATURE;
            pref.putUChar("opt", option);
            updateUI(option, getSensorValue());
        }
        break;
    case 1:
        // Humidity
        if (AceButton::kEventReleased == eventType ) {
            option = SENSOR_TYPE_HUMIDITY;
            pref.putUChar("opt", option);
            updateUI(option, getSensorValue());
        }
        break;
    case 2:
        // Sleep update
        if (AceButton::kEventLongPressed == eventType ) {
            if (sleepEn) {
                sleepEn = false;
                pref.putUChar("sleep", 0);
                Serial.println("disable sleep!");
            } else {
                sleepEn = true;
                pref.putUChar("sleep", 1);
                Serial.println("enable sleep!");
            }
        }
        break;
    default:
        break;
    }
    UPDATE_OPR_TIMER;
}

void loopButton()
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].check();
    }
}

void setupButton()
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        // initialize built-in LED as an output
        pinMode(button_pin[i], OUTPUT);
        // Button uses the built-in pull up register.
        pinMode(button_pin[i], INPUT_PULLUP);
        // initialize the corresponding AceButton
        buttons[i].init(button_pin[i], HIGH, i);
    }

    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(buttonHandleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
}

void setupDisplay()
{
#if defined(LILYGO_T5_V102)  || defined(LILYGO_EPD_DISPLAY_102)
    pinMode(EPD_POWER_ENABLE, OUTPUT);
    digitalWrite(EPD_POWER_ENABLE, HIGH);
#endif /*LILYGO_EPD_DISPLAY_102*/

    display.init(); // enable diagnostic output on Serial
    display.setRotation(1);
    u8g2Fonts.begin(display);

    u8g2Fonts.setFontMode(1);                           // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(0);                      // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);          // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);          // apply Adafruit GFX color
}

void updateUI(uint8_t type, float value)
{
    uint16_t x = display.width() / 2 - 60 ;
    uint16_t y = display.height() / 2;

    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    u8g2Fonts.setCursor(x, y + 15);

    int temp = value;
    u8g2Fonts.print(temp);

    u8g2Fonts.setFont(u8g2_font_ncenB18_tf);
    u8g2Fonts.setCursor(display.width() - 38, 55);
    u8g2Fonts.print(".");

    u8g2Fonts.setFont(u8g2_font_courB14_tf );
    temp = (temp - value) * 100;
    u8g2Fonts.print(abs(temp));

    display.drawBitmap(type ? gImage_celsius : gImage_humidity, display.width() - 38, 5, 32, 32, GxEPD_BLACK);
    display.drawFastHLine(0, display.height() - 20, display.width(), GxEPD_BLACK);

    u8g2Fonts.setFont(u8g2_font_helvR10_tf);
    u8g2Fonts.setCursor(3, display.height() - 5);
    u8g2Fonts.print("Designed By LilyGo");
    display.update();
}


void sleep()
{
    /**
    *  Operating power consumption is about ~10mA,
    *  sleep current is about ~22uA
    */

    radio.sleep();

    SPI.end();

    Wire.end();

#if defined(LILYGO_T5_V102)  || defined(LILYGO_EPD_DISPLAY_102)
    digitalWrite(EPD_POWER_ENABLE, LOW);
    pinMode(EPD_POWER_ENABLE, INPUT);
#endif /*LILYGO_EPD_DISPLAY_102*/

    // esp_sleep_enable_ext1_wakeup(BUTTON_3, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEPP_MINUTE * 60000000);
    esp_deep_sleep_start();
}

void setupSensor()
{
#if defined(USING_BME280_SENSOR)
    Wire.begin(BOARD_SDA, BOARD_SCL);
    Wire.setClock(400000); //Increase to fast I2C speed!
    if (!mySensor.beginI2C()) {
        sensor_success = false;
        Serial.println("setup Sensor failed!"); return;
    }
    mySensor.setMode(MODE_SLEEP); //Sleep for now
    sensor_success = true;
#endif
}


float getSensorValueImpl()
{
#if defined(USING_BME280_SENSOR)
    if (!sensor_success) {
        return false;
    }
    uint32_t startMillis = millis() + 1000;

    mySensor.setMode(MODE_FORCED); //Wake up sensor and take reading
    long startTime = millis();
    while (mySensor.isMeasuring() == false) {
        if (millis() > startMillis) {
            return 0.0;
        }
    } //Wait for sensor to start measurment

    startMillis = millis() + 1000;
    while (mySensor.isMeasuring() == true) {
        if (millis() > startMillis) {
            return 0.0;
        }
    } //Hang out while sensor completes the reading

    long endTime = millis();

    //Sensor is now back asleep but we get get the data
    Serial.print(" Measure time(ms): ");
    Serial.print(endTime - startTime);

    if (option) {
        return mySensor.readTempC();
    }

    return mySensor.readFloatHumidity();

#else
    return 0.0;
#endif
}


float getSensorValue()
{
    /*
    *   When there are no sensors, fake data is generated
    * */

    float temp = getSensorValueImpl() ;
    if ( temp < 1 ) {
        temp = (random(10, 35) + ((float)random(0, 80) / 100.0));
    }
    return temp;
}


bool setupSDCard()
{
    pinMode(SDCARD_MISO, INPUT_PULLUP);
    SPI2.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
    if (!SD.begin(SDCARD_CS, SPI2)) {
        Serial.println("SD begin failed!");
        return false;
    }
    return true;
}

void setup()
{
    /**
     *  Update the screen and transmit wireless, do not need such a high frequency,
     *  reducing the frequency can significantly reduce power consumption
     */
    setCpuFrequencyMhz(20);

    startMillis = millis();

    Serial.begin(115200);

    setupPreferences();

    setupButton();

    setupDisplay();

    setupSensor();


    bool rRes =  setupRadio();

    /*
    *   The sdcard is not used here, it is only used to demonstrate initialization
    */
    bool sdRes =  setupSDCard();

    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
        const char *radio_txt = "Radio";
        u8g2Fonts.setFont(u8g2_font_t0_14b_mr);
        u8g2Fonts.setCursor(5, 20);
        u8g2Fonts.print(radio_txt);
        u8g2Fonts.setCursor(display.width() - 20, 20);
        u8g2Fonts.print(rRes ? "+" : "-");

        const char *sdcard_txt = "SDCard";
        u8g2Fonts.setCursor(5, 50);
        u8g2Fonts.print(sdcard_txt);
        u8g2Fonts.setCursor(display.width() - 20, 50);
        u8g2Fonts.print(sdRes ? "+" : "-");
        display.update();
        delay(1000);
    }
}

void loop()
{
    loopButton();

    if (millis()  > updateMillis) {
        updateMillis = millis() +  updatePeriod;
        float temp = getSensorValue();
        updateUI(option, temp );
        transmit(temp);
        // Refresh sleep timer after screen update
        UPDATE_OPR_TIMER;
    }


    if (sleepEn) {
        // Wait for some time for button polling on startup
        if (millis()  > startMillis) {  //Cancelling the judgment will immediately go to sleep
            Serial.println("sleep !");
            sleep();
        }   //Cancelling the judgment will immediately go to sleep
    }
}




