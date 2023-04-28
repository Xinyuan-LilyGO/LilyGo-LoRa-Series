
#include "Arduino.h"
#include <SPI.h>
#include <SX128XLT.h>
#include "boards.h"

#define LORA_DEVICE DEVICE_SX1280                //we need to define the device we are using


//*******  Setup LoRa Parameters Here ! ***************
const uint32_t Frequency = 2445000000;           //frequency of transmissions in hz
const int32_t Offset = 0;                        //offset frequency in hz for calibration purposes
const uint8_t Bandwidth = LORA_BW_0800;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;        //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate
const uint16_t Calibration = 11350;              //Manual Ranging calibrarion value

const int8_t RangingTXPower = 3;                //Transmit power used   !!Cannot be greater than 3dbm!!
const uint32_t RangingAddress = 16;              //must match address in recever

const uint16_t  waittimemS = 10000;              //wait this long in mS for packet before assuming timeout
const uint16_t  TXtimeoutmS = 5000;              //ranging TX timeout in mS
const uint16_t  packet_delaymS = 0;              //forced extra delay in mS between ranging requests
const uint16_t  rangeingcount = 10;               //number of times ranging is cqarried out for each distance measurment
float distance_adjustment = 1.0000;              //adjustment factor to calculated distance


#define ENABLEOLED                               //enable this define to use display 
#define ENABLEDISPLAY                            //enable this define to use display

SX128XLT LT;

uint16_t rangeing_errors, rangeings_valid, rangeing_results;
uint16_t IrqStatus;
uint32_t endwaitmS, range_result_sum, range_result_average;
float distance, distance_sum, distance_average;
bool ranging_error;
int32_t range_result;
int16_t RangingRSSI;

void led_Flash(uint16_t flashes, uint16_t delaymS);

void setup()
{
    pinMode(BOARD_LED, OUTPUT);                                   //setup pin as output for indicator LED
    led_Flash(4, 125);                                       //two quick LED flashes to indicate program start

    initBoard();
    delay(100);

    Serial.println(F("Ranging Master Starting"));


    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    if (LT.begin(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN, RADIO_DIO1_PIN, LORA_DEVICE)) {
        Serial.println(F("Device found"));
        led_Flash(2, 125);
        delay(1000);
    } else {
        Serial.println(F("No device responding"));
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "No device responding");
        u8g2->sendBuffer();
        while (1) {
            led_Flash(50, 50);                                 //long fast speed flash indicates device error
        }
    }

    LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, RangingAddress, RANGING_MASTER);
    LT.setRangingCalibration(Calibration);               //override automatic lookup of calibration value from library table

    Serial.println();
    LT.printModemSettings();                               //reads and prints the configured LoRa settings, useful check
    Serial.println();
    LT.printOperatingSettings();                           //reads and prints the configured operating settings, useful check
    Serial.println();
    Serial.println();
    LT.printRegisters(0x900, 0x9FF);                       //print contents of device registers, normally 0x900 to 0x9FF
    Serial.println();
    Serial.println();

#ifdef ENABLEDISPLAY
    Serial.println("Display Enabled");
    u8g2->setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2->setFontDirection(0);
    char buf[256];
    u8g2->clearBuffer();
    u8g2->drawStr(0, 12, "Ranging RAW Ready");
    snprintf(buf, sizeof(buf), "Power:%.d dBm", RangingTXPower);
    u8g2->drawStr(0, 12 * 2, buf);
    snprintf(buf, sizeof(buf), "Cal: %d ", Calibration);
    u8g2->drawStr(0, 12 * 3, buf);
    snprintf(buf, sizeof(buf), "Adjust: %d ", distance_adjustment);
    u8g2->sendBuffer();
#endif

    Serial.print(F("Address "));
    Serial.println(RangingAddress);
    Serial.print(F("CalibrationValue "));
    Serial.println(LT.getSetCalibrationValue());
    Serial.println(F("Ranging master RAW ready"));
    delay(2000);
}

void loop()
{
    uint8_t index;
    distance_sum = 0;
    range_result_sum = 0;
    rangeing_results = 0;                           //count of valid results in each loop

    for (index = 1; index <= rangeingcount; index++) {
        Serial.println(F("Start Ranging"));

        LT.transmitRanging(RangingAddress, TXtimeoutmS, RangingTXPower, WAIT_TX);
        IrqStatus = LT.readIrqStatus();

        if (IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID) {
            rangeing_results++;
            rangeings_valid++;
            digitalWrite(BOARD_LED, HIGH);
            Serial.print(F("Valid"));
            range_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);
            Serial.print(F(",Register,"));
            Serial.print(range_result);

            if (range_result > 800000) {
                range_result = 0;
            }
            range_result_sum = range_result_sum + range_result;

            distance = LT.getRangingDistance(RANGING_RESULT_RAW, range_result, distance_adjustment);
            distance_sum = distance_sum + distance;

            Serial.print(F(",Distance,"));
            Serial.print(distance, 1);
            RangingRSSI = LT.getRangingRSSI();
            digitalWrite(BOARD_LED, LOW);
        } else {
            rangeing_errors++;
            distance = 0;
            range_result = 0;
            Serial.print(F("NotValid"));
            Serial.print(F(",Irq,"));
            Serial.print(IrqStatus, HEX);
        }
        delay(packet_delaymS);

        if (index == rangeingcount) {
            range_result_average = (range_result_sum / rangeing_results);
            if (rangeing_results == 0) {
                distance_average = 0;
            } else {
                distance_average = (distance_sum / rangeing_results);
            }

            Serial.print(F(",TotalValid,"));
            Serial.print(rangeings_valid);
            Serial.print(F(",TotalErrors,"));
            Serial.print(rangeing_errors);
            Serial.print(F(",AverageRAWResult,"));
            Serial.print(range_result_average);
            Serial.print(F(",AverageDistance,"));
            Serial.print(distance_average, 1);

#ifdef ENABLEDISPLAY
            u8g2->clearBuffer();
            char buf[256];
            u8g2->drawStr(0, 12, "Rang_Master");
            snprintf(buf, sizeof(buf), "Distance:%.2f m", distance_average);
            u8g2->drawStr(0, 12 * 2, buf);
            snprintf(buf, sizeof(buf), "RSSI: %d dBm", RangingRSSI);
            u8g2->drawStr(0, 12 * 3, buf);
            // snprintf(buf, sizeof(buf), "OK:  %d ", rangeings_valid);
            // u8g2->drawStr(0, 12 * 4, buf);
            // snprintf(buf, sizeof(buf), "Err:  %d ", rangeing_errors);
            // u8g2->drawStr(0, 12 * 5, buf);
            u8g2->sendBuffer();

#endif
            delay(2000);

        }
        Serial.println();
    }

}

void led_Flash(uint16_t flashes, uint16_t delaymS)
{
    uint16_t index;

    for (index = 1; index <= flashes; index++) {
        digitalWrite(BOARD_LED, HIGH);
        delay(delaymS);
        digitalWrite(BOARD_LED, LOW);
        delay(delaymS);
    }
}
