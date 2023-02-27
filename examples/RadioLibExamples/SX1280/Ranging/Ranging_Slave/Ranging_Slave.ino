#include <SPI.h>
#include <SX128XLT.h>
#include "boards.h"


#define LORA_DEVICE DEVICE_SX1280                //we need to define the device we are using
//*******  Setup LoRa Parameters Here ! ***************
//LoRa Modem Parameters
const uint32_t Frequency = 2445000000;           //frequency of transmissions in hz
const int32_t Offset = 0;                        //offset frequency in hz for calibration purposes
const uint8_t Bandwidth = LORA_BW_0800;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;        //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate
const uint16_t Calibration = 11350;              //Manual Ranging calibration value

const int8_t TXpower = 3;                       //Transmit power used   !!Cannot be greater than 3dbm!!
const uint32_t RangingAddress = 16;              //must match address in master

const uint16_t  rangingRXTimeoutmS = 0xFFFF;     //ranging RX timeout in mS


SX128XLT LT;

uint32_t endwaitmS;
uint16_t IrqStatus;
uint32_t response_sent;

void led_Flash(unsigned int flashes, unsigned int delaymS);

void setup()
{
    Serial.begin(115200);            //setup Serial console ouput

    Serial.println("Ranging Slave Starting");

    pinMode(BOARD_LED, OUTPUT);
    led_Flash(2, 125);

    initBoard();
    delay(100);

    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    if (LT.begin(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN, RADIO_DIO1_PIN, LORA_DEVICE)) {
        Serial.println(F("Device found"));
        led_Flash(2, 125);
        delay(1000);
    } else {
        Serial.println(F("No device responding"));
        while (1) {
            led_Flash(50, 50);                                 //long fast speed flash indicates device error
        }
    }

    //The function call list below shows the complete setup for the LoRa device for ranging using the information
    LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, RangingAddress, RANGING_SLAVE);

    LT.setRangingCalibration(11350);               //override automatic lookup of calibration value from library table

    Serial.print(F("Calibration,"));
    Serial.println(LT.getSetCalibrationValue());           //reads the calibratuion value currently set
    delay(2000);

    u8g2->clearBuffer();

    u8g2->drawStr(0, 12, "Rang_Slave");
    u8g2->sendBuffer();
}

char buf[256];
void loop()
{
    LT.receiveRanging(RangingAddress, 0, TXpower, NO_WAIT);

    endwaitmS = millis() + rangingRXTimeoutmS;

    while (!digitalRead(RADIO_DIO1_PIN) && (millis() <= endwaitmS));          //wait for Ranging valid or timeout

    if (millis() >= endwaitmS) {
        Serial.println("Error - Ranging Receive Timeout!!");
        led_Flash(2, 100);                                             //single flash to indicate timeout
    } else {
        IrqStatus = LT.readIrqStatus();
        digitalWrite(BOARD_LED, HIGH);

        if (IrqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE) {
            response_sent++;
            Serial.print(response_sent);
            Serial.print(" Response sent");
        } else {
            Serial.print("Slave error,");
            Serial.print(",Irq,");
            Serial.print(IrqStatus, HEX);
            LT.printIrqStatus();
        }
        digitalWrite(BOARD_LED, LOW);
        Serial.println();
    }

}

void led_Flash(unsigned int flashes, unsigned int delaymS)
{
    //flash LED to show board is alive
    unsigned int index;
    for (index = 1; index <= flashes; index++) {
        digitalWrite(BOARD_LED, HIGH);
        delay(delaymS);
        digitalWrite(BOARD_LED, LOW);
        delay(delaymS);
    }
}
