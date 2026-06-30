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
#define RADIO_TYPE_STR              "T-Bema-1W-SX1262"
#define RADIO_CTRL                  (21)
#define RADIO_DIO1_PIN              (1)
#define BOARD_LED                   18
#define LED_ON                      HIGH
#define NTC_PIN                     (14)
#define FAN_CTRL                    (41)
#define ADC_PIN                     (4)

#define CONFIG_RADIO_FREQ           868.0
#define CONFIG_RADIO_OUTPUT_POWER   22

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

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

    // initialize SX1262 with default settings
    Serial.print(F("[SX1262] Initializing ... "));
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

    // Set output power
    radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER);

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);

#if defined(INITIATING_NODE)
    // * T-Beam 1W SX1262 LoRa LAN Control ,set Low turn off LAN , TX Mode
    Serial.println("Turn off LAN,Trun on PA, Enter Tx mode.");
    digitalWrite(RADIO_CTRL, LOW);


    // send the first packet on this node
    Serial.print(F("[SX1262] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;

#else
    // * T-Beam 1W SX1262 LoRa LAN Control ,set High turn off LAN , RX Mode
    Serial.println("Turn off LAN,Trun on PA, Enter Rx mode.");
    digitalWrite(RADIO_CTRL, HIGH);

    // start listening for LoRa packets on this node
    Serial.print(F("[SX1262] Starting to listen ... "));
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
                Serial.println(F("[SX1262] Received packet!"));

                // print data of the packet
                Serial.print(F("[SX1262] Data:\t\t"));
                Serial.println(str);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[SX1262] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[SX1262] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

            }

            // wait a second before transmitting again
            delay(1000);

            // send another one
            Serial.print(F("[SX1262] Sending another packet ... "));
            transmissionState = radio.startTransmit("Hello World!");
            transmitFlag = true;
        }

    }
}
