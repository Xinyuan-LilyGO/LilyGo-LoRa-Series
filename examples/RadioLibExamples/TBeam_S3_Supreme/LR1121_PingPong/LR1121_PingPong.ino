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
#define I2C_SDA                     (17)    // I2C0 attach oled and sensor i2c bus
#define I2C_SCL                     (18)    // I2C0 attach oled and sensor i2c bus
#define I2C1_SDA                    (42)    // I2C1 attach pmu and rtc i2c bus
#define I2C1_SCL                    (41)    // I2C1 attach pmu and rtc i2c bus
#define PMU_IRQ                     (40)
#define GPS_RX_PIN                  (9)
#define GPS_TX_PIN                  (8)
#define GPS_EN_PIN                  (7)
#define GPS_PPS_PIN                 (6)
#define BUTTON_PIN                  (0)
#define RADIO_SCLK_PIN              (12)
#define RADIO_MISO_PIN              (13)
#define RADIO_MOSI_PIN              (11)
#define RADIO_CS_PIN                (10)
#define RADIO_RST_PIN               (5)
#define RADIO_BUSY_PIN              (4)
#define RADIO_DIO9_PIN               (1)
#define SPI_MOSI                    (35)
#define SPI_SCK                     (36)
#define SPI_MISO                    (37)
#define SPI_CS                      (47)
#define IMU_CS                      (34)
#define IMU_INT                     (33)
#define SDCARD_MOSI                 SPI_MOSI
#define SDCARD_MISO                 SPI_MISO
#define SDCARD_SCLK                 SPI_SCK
#define SDCARD_CS                   SPI_CS
#define RTC_INT                     (14)
#define GPS_BAUD_RATE               (9600)
#define BOARD_VARIANT_NAME          "T-Beam-S3-Supreme-LR1121"

#define CONFIG_RADIO_FREQ           2450.0
#define CONFIG_RADIO_OUTPUT_POWER   13

XPowersPMU pmu; // Power management unit
LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO9_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

static const uint32_t rfswitch_dio_pins[] = {
    RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC
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
    bool success =  pmu.begin(Wire1, AXP2101_SLAVE_ADDRESS, I2C1_SDA, I2C1_SCL);
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

    // LoRa
    pmu.setALDO3Voltage(3300);
    pmu.enableALDO3();

    // BME280 & Display & MAG Sensor
    pmu.setALDO1Voltage(3300);
    pmu.enableALDO1();

    // Sensor
    pmu.setALDO2Voltage(3300);
    pmu.enableALDO2();

    // TF Card
    pmu.setBLDO1Voltage(3300);
    pmu.enableBLDO1();

    // External pin header
    pmu.setBLDO2Voltage(3300);
    pmu.enableBLDO2();

    // External M.2 Socket
    pmu.setDC3Voltage(3300);
    pmu.enableDC3();
    pmu.setDC4Voltage(3300);
    pmu.enableDC4();
    pmu.setDC5Voltage(3300);
    pmu.enableDC5();

    // Set charger constant current
    pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);
}


void setup()
{
    Serial.begin(115200);

    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    // Initialize power supplies
    setupPeripheralPowerSupplies();

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

    // Set output power
    radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER);

    // set the function that will be called
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);

    // set RF switch table
    radio.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);

    // LR1121 TCXO Voltage 2.85~3.15V
    radio.setTCXO(3.0);

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
