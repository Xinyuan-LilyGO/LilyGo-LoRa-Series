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
#define I2C_SDA                     18
#define I2C_SCL                     17
#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              3
#define RADIO_MOSI_PIN              6
#define RADIO_CS_PIN                7
#define SDCARD_MOSI                 11
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13
#define BOARD_LED                   37
#define LED_ON                      HIGH
#define BUTTON_PIN                  0
#define ADC_PIN                     1
#define RADIO_RST_PIN               8
#define RADIO_DIO1_PIN              9       // SX1280 DIO1 = IO9
#define RADIO_BUSY_PIN              36      // SX1280 BUSY = IO36
#define RADIO_RX_PIN                21      // SX1280 RX = IO21
#define RADIO_TX_PIN                10      // SX1280 TX = IO10
#define BOARD_VARIANT_NAME          "T3-S3-SX1280PA"

#define CONFIG_RADIO_FREQ           2400.0

/*
* Important: SX1280 PA Version

* The 2.4G version does not have a power amplifier (PA). The permissible power setting is 13dBm.
* If it is a version with a built-in PA, do not set the maximum power beyond 3dBm.
* This is because a power amplifier is added to the RF front end; setting it to 3dBm will achieve an output power of 22dBm.
* Setting it beyond 3dBm may damage the PA.
*/
#define CONFIG_RADIO_OUTPUT_POWER   3

SX1280 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

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

    pinMode(BOARD_LED, OUTPUT);

    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    // initialize SX1280PA with default settings
    Serial.print(F("[SX1280PA] Initializing ... "));
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

    // Set RF switch pins
    radio.setRfSwitchPins(RADIO_RX_PIN, RADIO_TX_PIN);

#if defined(INITIATING_NODE)
    // send the first packet on this node
    Serial.print(F("[SX1280PA] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;
#else
    // start listening for LoRa packets on this node
    Serial.print(F("[SX1280PA] Starting to listen ... "));
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
                Serial.println(F("[SX1280PA] Received packet!"));

                // print data of the packet
                Serial.print(F("[SX1280PA] Data:\t\t"));
                Serial.println(str);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[SX1280PA] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[SX1280PA] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

            }

            // wait a second before transmitting again
            delay(1000);

            // send another one
            Serial.print(F("[SX1280PA] Sending another packet ... "));
            transmissionState = radio.startTransmit("Hello World!");
            transmitFlag = true;
        }

    }
}
