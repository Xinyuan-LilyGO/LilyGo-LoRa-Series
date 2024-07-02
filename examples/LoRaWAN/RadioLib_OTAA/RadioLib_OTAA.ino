/*
  RadioLib LoRaWAN ABP Example

  This example joins a LoRaWAN network and will send
  uplink packets. Before you start, you will have to
  register your device at https://www.thethingsnetwork.org/
  After your device is registered, you can run this example.
  The device will join the network and start uploading data.

  Running this examples REQUIRES you to check "Resets DevNonces"
  on your LoRaWAN dashboard. Refer to the network's
  documentation on how to do this.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/

  For LoRaWAN details, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/LoRaWAN

*/

#include <RadioLib.h>
#include "LoRaBoards.h"

// !!!!! APP EUI
#define RADIOLIB_LORAWAN_JOIN_EUI  0xAABB112233445566
// !!!!! DEV EUI
#define RADIOLIB_LORAWAN_DEV_EUI   0x70B3D57ED0066FDA
// !!!!! APP EUI
#define RADIOLIB_LORAWAN_APP_KEY   0xAA, 0xBB, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66
// !!! APP KEY
#define RADIOLIB_LORAWAN_NWK_KEY   0xF2, 0x84, 0x31, 0x0F, 0x82, 0x65, 0x99, 0x4F, 0x70, 0xCB, 0x88, 0x74, 0xFA, 0xD6, 0x71, 0xA2


#if  defined(USING_SX1276)
SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);
#elif defined(USING_SX1262)
SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#elif defined(USING_SX1278)
SX1278 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);
#elif   defined(USING_LR1121)
LR1121 radio = new Module(RADIO_CS_PIN, RADIO_DIO9_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#endif


// how often to send an uplink - consider legal & FUP constraints - see notes
const uint32_t uplinkIntervalSeconds = 5UL * 60UL;    // minutes x seconds


// for the curious, the #ifndef blocks allow for automated testing &/or you can
// put your EUI & keys in to your platformio.ini - see wiki for more tips

// regional choices: EU868, US915, AU915, AS923, IN865, KR920, CN780, CN500
const LoRaWANBand_t Region = EU868;
const uint8_t subBand = 0;  // For US915, change this to 2, otherwise leave on 0

// ============================================================================


// copy over the EUI's & keys in to the something that will not compile if incorrectly formatted
uint64_t joinEUI =   RADIOLIB_LORAWAN_JOIN_EUI;
uint64_t devEUI  =   RADIOLIB_LORAWAN_DEV_EUI;
uint8_t appKey[] = { RADIOLIB_LORAWAN_APP_KEY };
uint8_t nwkKey[] = { RADIOLIB_LORAWAN_NWK_KEY };

// create the LoRaWAN node
LoRaWANNode node(&radio, &Region, subBand);

// helper function to display any issues
void debug(bool isFail, const __FlashStringHelper *message, int state, bool Freeze)
{
    if (isFail) {
        Serial.print(message);
        Serial.print("(");
        Serial.print(state);
        Serial.println(")");
        while (Freeze);
    }
}

// helper function to display a byte array
void arrayDump(uint8_t *buffer, uint16_t len)
{
    for (uint16_t c = 0; c < len; c++) {
        char b = buffer[c];
        if (b < 0x10) {
            Serial.print('0');
        }
        Serial.print(b, HEX);
    }
    Serial.println();
}

static uint32_t txCounter = 0;


void setup()
{
    Serial.begin(115200);

    while (!Serial);

    setupBoards();

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    delay(5000);  // Give time to switch to the serial monitor

    Serial.println(F("\nSetup ... "));

    Serial.println(F("Initialise the radio"));

    int state = radio.begin();
    debug(state != RADIOLIB_ERR_NONE, F("Initialise radio failed"), state, true);
    printResult(state == RADIOLIB_ERR_NONE);
    delay(2000);

#ifdef USING_DIO2_AS_RF_SWITCH
#ifdef USING_SX1262
    // Some SX126x modules use DIO2 as RF switch. To enable
    // this feature, the following method can be used.
    // NOTE: As long as DIO2 is configured to control RF switch,
    //       it can't be used as interrupt pin!
    if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
        Serial.println(F("Failed to set DIO2 as RF switch!"));
        while (true);
    }
#endif //USING_SX1262
#endif //USING_DIO2_AS_RF_SWITCH

#ifdef RADIO_SWITCH_PIN
    // T-MOTION
    const uint32_t pins[] = {
        RADIO_SWITCH_PIN, RADIO_SWITCH_PIN, RADIOLIB_NC,
    };
    static const Module::RfSwitchMode_t table[] = {
        {Module::MODE_IDLE,  {0,  0} },
        {Module::MODE_RX,    {1, 0} },
        {Module::MODE_TX,    {0, 1} },
        END_OF_MODE_TABLE,
    };
    radio.setRfSwitchTable(pins, table);
#endif

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

    int retry = 0;
    while (1) {
        if (u8g2) {
            u8g2->clearBuffer();
            u8g2->setFont(u8g2_font_NokiaLargeBold_tf );
            uint16_t str_w =  u8g2->getStrWidth(BOARD_VARIANT_NAME);
            u8g2->drawStr((u8g2->getWidth() - str_w) / 2, 16, BOARD_VARIANT_NAME);
            u8g2->drawHLine(5, 21, u8g2->getWidth() - 5);

            u8g2->setCursor(0, 38);
            u8g2->print("Join LoRaWAN :");
            u8g2->println(String(retry++));
            u8g2->sendBuffer();
        }
        Serial.println(F("Join ('login') to the LoRaWAN Network"));
        node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
        state = node.activateOTAA();
        debug(state < RADIOLIB_ERR_NONE, F("Join failed"), state, false);
        if (state == RADIOLIB_ERR_NONE || state == RADIOLIB_LORAWAN_NEW_SESSION) {
            break;
        }
        delay(10000);
        Serial.print("Retry ");
    }

    Serial.println(F("Joined!\n"));
}


void loop()
{
    Serial.println(F("Sending uplink"));

    // This is the place to gather the sensor inputs
    // Instead of reading any real sensor, we just generate some random numbers as example
    uint8_t value1 = radio.random(100);
    uint16_t value2 = radio.random(2000);

    // Build payload byte array
    uint8_t uplinkPayload[3];
    uplinkPayload[0] = value1;
    uplinkPayload[1] = highByte(value2);   // See notes for high/lowByte functions
    uplinkPayload[2] = lowByte(value2);

    // Perform an uplink
    int state = node.sendReceive(uplinkPayload, sizeof(uplinkPayload));
    debug((state != RADIOLIB_LORAWAN_NO_DOWNLINK) && (state != RADIOLIB_ERR_NONE), F("Error in sendReceive"), state, false);

    if (u8g2) {
        u8g2->clearBuffer();
        u8g2->setFont(u8g2_font_NokiaLargeBold_tf );
        uint16_t str_w =  u8g2->getStrWidth(BOARD_VARIANT_NAME);
        u8g2->drawStr((u8g2->getWidth() - str_w) / 2, 16, BOARD_VARIANT_NAME);
        u8g2->drawHLine(5, 21, u8g2->getWidth() - 5);
        u8g2->setCursor(0, 38);
        u8g2->print(node.isActivated() ? "Joined." : "NoJoin");
        u8g2->print("\tTx:"); u8g2->println(++txCounter);
#ifdef ARDUINO_ARCH_ESP32
        u8g2->setCursor(0, 54);
        u8g2->println("MAC:"); u8g2->println(ESP.getEfuseMac(), HEX);
#endif
        u8g2->sendBuffer();
    }


    Serial.print(F("Uplink complete, next in "));
    Serial.print(uplinkIntervalSeconds);
    Serial.println(F(" seconds"));

#ifdef BOARD_LED
    digitalWrite(BOARD_LED, LED_ON);
    delay(200);
    digitalWrite(BOARD_LED, !LED_ON);
#endif

    // Wait until next uplink - observing legal & TTN FUP constraints
    delay(uplinkIntervalSeconds * 1000UL);  // delay needs milli-seconds
}
