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
#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    4   // OLED Display reset pin
#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_RST_PIN               23
#define RADIO_DIO0_PIN              26
#define RADIO_DIO1_PIN              32
// TCXO pin must be set to HIGH before enabling Radio
#define RADIO_TCXO_ENABLE           12  //only sx1276 tcxo version
#define RADIO_BUSY_PIN              32
#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13
#define BOARD_LED                   25
#define LED_ON                      HIGH
#define BUTTON_PIN                  0
#define ADC_PIN                     35

#define BOARD_VARIANT_NAME          "T3 LoRa32" //https://lilygo.cc/products/lora3

#define CONFIG_RADIO_FREQ           868.0
#define CONFIG_RADIO_OUTPUT_POWER   17

SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

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

    // Set LED as output
    pinMode(BOARD_LED, OUTPUT);

    // Set TCXO enable pin,use radio must be enabled before
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);

    // Initialize SPI
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    // initialize SX1276 with default settings
    Serial.print(F("[SX1276] Initializing ... "));
    int state = radio.begin();
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
    radio.setPacketReceivedAction(setFlag);

#if defined(INITIATING_NODE)
    // send the first packet on this node
    Serial.print(F("[SX1276] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;
#else
    // start listening for LoRa packets on this node
    Serial.print(F("[SX1276] Starting to listen ... "));
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
                Serial.println(F("[SX1276] Received packet!"));

                // print data of the packet
                Serial.print(F("[SX1276] Data:\t\t"));
                Serial.println(str);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[SX1276] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[SX1276] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

            }

            // wait a second before transmitting again
            delay(1000);

            // send another one
            Serial.print(F("[SX1276] Sending another packet ... "));
            transmissionState = radio.startTransmit("Hello World!");
            transmitFlag = true;
        }

    }
}
