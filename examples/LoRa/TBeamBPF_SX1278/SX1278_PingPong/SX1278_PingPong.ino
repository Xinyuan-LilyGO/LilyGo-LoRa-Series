/*
  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

#include <RadioLib.h>
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>

// uncomment the following only on one
// of the nodes to initiate the pings
// 注释INITIATING_NODE这行初始化接收,打开这行初始化发送,两个设备，必须一个初始化为接收另一个初始化为发送才能收到
// #define INITIATING_NODE

// Board pin definitions
#define I2C_SDA                     (8)
#define I2C_SCL                     (9)
#define PMU_IRQ                     (4)
#define GPS_RX_PIN                  (5)
#define GPS_TX_PIN                  (6)
#define GPS_PPS_PIN                 (7)
#define BUTTON_PIN                  (0)
#define BUTTON2_PIN                 (3)
#define RADIO_SCLK_PIN              (12)
#define RADIO_MISO_PIN              (13)
#define RADIO_MOSI_PIN              (11)
#define RADIO_CS_PIN                (1)
#define RADIO_RST_PIN               (18)
#define RADIO_DIO0_PIN              (14)
#define RADIO_DIO1_PIN              (21)
#define RADIO_LDO_EN                (16)
#define RADIO_CTRL                  (39)
#define SPI_MOSI                    (11)
#define SPI_SCK                     (12)
#define SPI_MISO                    (13)
#define SPI_CS                      (10)
#define SDCARD_MOSI                 SPI_MOSI
#define SDCARD_MISO                 SPI_MISO
#define SDCARD_SCLK                 SPI_SCK
#define SDCARD_CS                   SPI_CS
#define BOARD_VARIANT_NAME          "T-Beam BPF SX1278"

#define CONFIG_RADIO_FREQ           144.0
#define CONFIG_RADIO_OUTPUT_POWER   17

XPowersPMU pmu; // Power management unit
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);

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


void setupPeripheralPowerSupplies()
{
    // Initialize AXP2101
    bool success =  pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL);
    if (!success) {
        Serial.println(F("[AXP2101] Initialization failed!"));
        while (true) {
            delay(10);
        }
    }
    // WARNING: DC1 is the core power supply for the ESP32; do not configure it.

    // GNSS
    pmu.setALDO4Voltage(3300);
    pmu.enableALDO4();

    // TF Card
    pmu.setALDO2Voltage(3300);
    pmu.enableALDO2();

    // Extern Power source
    pmu.setDC3Voltage(3300);
    pmu.enableDC3();
    pmu.setDC5Voltage(3300);
    pmu.enableDC5();
    pmu.setALDO1Voltage(3300);
    pmu.enableALDO1();

    // Set charger constant current
    pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);
}

void setup()
{
    Serial.begin(115200);

    // Set LDO_EN to HIGH to initialize the Radio
    pinMode(RADIO_LDO_EN, OUTPUT);
    digitalWrite(RADIO_LDO_EN, HIGH);

    // Initialize SPI
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);


    // Initialize power supplies
    setupPeripheralPowerSupplies();

    // initialize SX1278 with default settings
    Serial.print(F("[SX1278] Initializing ... "));
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
    radio.setPacketReceivedAction(setFlag);

#if defined(INITIATING_NODE)
    // * T-Beam BPF SX1278 LoRa LAN Control ,set Low turn off LAN , TX Mode
    Serial.println("Turn off LAN,Trun on PA, Enter Tx mode.");
    digitalWrite(RADIO_CTRL, LOW);


    // send the first packet on this node
    Serial.print(F("[SX1278] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;

#else
    // * T-Beam BPF SX1278 LoRa LAN Control ,set High turn off LAN , RX Mode
    Serial.println("Turn off LAN,Trun on PA, Enter Rx mode.");
    digitalWrite(RADIO_CTRL, HIGH);

    // start listening for LoRa packets on this node
    Serial.print(F("[SX1278] Starting to listen ... "));
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

                // packet was successfully received
                Serial.println(F("[SX1278] Received packet!"));

                // print data of the packet
                Serial.print(F("[SX1278] Data:\t\t"));
                Serial.println(str);

                // print RSSI (Received Signal Strength Indicator)
                Serial.print(F("[SX1278] RSSI:\t\t"));
                Serial.print(radio.getRSSI());
                Serial.println(F(" dBm"));

                // print SNR (Signal-to-Noise Ratio)
                Serial.print(F("[SX1278] SNR:\t\t"));
                Serial.print(radio.getSNR());
                Serial.println(F(" dB"));

            }

            // wait a second before transmitting again
            delay(1000);

            // send another one
            Serial.print(F("[SX1278] Sending another packet ... "));
            transmissionState = radio.startTransmit("Hello World!");
            transmitFlag = true;
        }

    }
}
