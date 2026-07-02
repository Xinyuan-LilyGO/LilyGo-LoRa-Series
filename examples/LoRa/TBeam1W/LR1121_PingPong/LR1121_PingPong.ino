/*
  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

#include <RadioLib.h>

// uncomment the following only on one
// of the nodes to initiate the pings
// 注释INITIATING_NODE这行初始化接收,打开这行初始化发送,两个设备，必须一个初始化为接收另一个初始化为发送才能收到
// #define INITIATING_NODE

// Board pin definitions
#define I2C_SDA                     (8)
#define I2C_SCL                     (9)
#define GPS_RX_PIN                  (5)
#define GPS_TX_PIN                  (6)
#define GPS_PPS_PIN                 (7)
#define GPS_EN_PIN                  (16)
#define BUTTON_PIN                  (0)          /*BUTTON 1 = GPIO0*/
#define BUTTON2_PIN                 (17)         /*BUTTON 2 = GPIO17*/
#define SPI_MOSI                    (11)
#define SPI_SCK                     (13)
#define SPI_MISO                    (12)
#define SPI_CS                      (10)
#define SDCARD_CS                   SPI_CS
#define RADIO_SCLK_PIN              (SPI_SCK)
#define RADIO_MISO_PIN              (SPI_MISO)
#define RADIO_MOSI_PIN              (SPI_MOSI)
#define RADIO_CS_PIN                (15)
#define RADIO_RST_PIN               (3)
#define RADIO_BUSY_PIN              (38)
#define RADIO_LDO_EN                (40)
#define RADIO_TYPE_STR              "T-Beam-1W-LR1121"
#define RADIO_DIO10_PIN             (1)      // Connect to DIO10
#define RADIO_DIO11_PIN             (21)     // Connect to DIO11
#define RADIO_DIO_IRQ_PIN           (RADIO_DIO11_PIN)
#define BOARD_LED                   (18)
#define LED_ON                      HIGH
#define NTC_PIN                     (14)
#define FAN_CTRL                    (41)
#define ADC_PIN                     (4)
#define CONFIG_RADIO_2G4_OUTPUT_POWER    1  //注意:WARNING:The 2.4G transmit power must not exceed 1dBm, otherwise it will cause permanent damage to LoRa.
#define CONFIG_RADIO_SUB1G_OUTPUT_POWER  22 //注意:WARNING:The maximum output power of Sub1G can be set to 22dBm.
#pragma message "Using LR2021 PA Version,The transmit power must not exceed 0dBm, otherwise it will cause permanent damage to LoRa."

#define CONFIG_RADIO_FREQ           2450.0


LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO_IRQ_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

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

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

void setFlag(void)
{
    // we sent or received a packet, set the flag
    operationDone = true;
}

void setup()
{
    Serial.begin(115200);

    // Set LED pin as output
    pinMode(BOARD_LED, OUTPUT);

    // Set LDO_EN to HIGH to initialize the Radio
    pinMode(RADIO_LDO_EN, OUTPUT);
    digitalWrite(RADIO_LDO_EN, HIGH);

    // Initialize SPI
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    // initialize LR1121 with default settings
    Serial.print(F("[LR1121] Initializing ... "));
    int state = radio.begin(CONFIG_RADIO_FREQ);
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true) {
            delay(10);
        }
    }

    // radio.setDioIrqParams(RADIOLIB_LR11X0_DIO10);
    if (CONFIG_RADIO_FREQ < 2400) {

        // Set output power
        radio.setOutputPower(CONFIG_RADIO_SUB1G_OUTPUT_POWER);

        Serial.printf("[%s] Using low frequency switch table for PA version\n", RADIO_TYPE_STR);
        radio.setRfSwitchTable(pa_version_rf_switch_dio_pins, low_sub1g_switch_table);

    } else {

        // Set output power
        radio.setOutputPower(CONFIG_RADIO_2G4_OUTPUT_POWER);

        Serial.printf("[%s] Using high frequency switch table for PA version\n", RADIO_TYPE_STR);
        radio.setRfSwitchTable(pa_version_rf_switch_dio_pins, high_2g4_switch_table);
    }

    // TCXO Voltage 2.85~3.15V
    radio.setTCXO(3.0);

    // set the function that will be called
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);

#if defined(INITIATING_NODE)
    // send the first packet on this node
    Serial.print(F("[LR1121] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;
#else
    // start listening for LoRa packets on this node
    Serial.print(F("[LR1121] Starting to listen ... "));
    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true) {
            delay(10);
        }
    }
#endif
}

void loop()
{
    // check if the previous operation finished
    if (operationDone) {
        // reset flag
        operationDone = false;

        if (transmitFlag) {

            digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));

            // the previous operation was transmission, listen for response
            // print the result
            if (transmissionState == RADIOLIB_ERR_NONE) {
                // packet was successfully sent
                Serial.println(F("transmission finished!"));

            } else {
                Serial.print(F("failed, code "));
                Serial.println(transmissionState);

            }

            // listen for response
            radio.startReceive();
            transmitFlag = false;

        } else {
            // the previous operation was reception
            // print data and send another packet
            String str;
            int state = radio.readData(str);

            if (state == RADIOLIB_ERR_NONE) {

                digitalWrite(BOARD_LED, 1 - digitalRead(BOARD_LED));

                // packet was successfully received
                Serial.println(F("[LR1121] Received packet!"));

                // print data of the packet
                Serial.print(F("[LR1121] Data:\t\t"));
                Serial.println(str);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[LR1121] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[LR1121] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

            }

            // wait a second before transmitting again
            delay(1000);

            // send another one
            Serial.print(F("[LR1121] Sending another packet ... "));
            transmissionState = radio.startTransmit("Hello World!");
            transmitFlag = true;
        }

    }
}
