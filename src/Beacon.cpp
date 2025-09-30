// Standalone LoRa Beacon for T-Beam BPF
// Removes dependency on utilities.h & LoRaBoards.h
// Ensure your hardware variant actually uses an SX1278-compatible radio.

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#ifndef T_BEAM_S3_BPF
#define T_BEAM_S3_BPF
#endif

// ---------------- Pin Mapping (mirrored from examples/Factory/utilities.h under T_BEAM_S3_BPF) ----------------
#define I2C_SDA             (8)
#define I2C_SCL             (9)

#define RADIO_SCLK_PIN      (12)
#define RADIO_MISO_PIN      (13)
#define RADIO_MOSI_PIN      (11)
#define RADIO_CS_PIN        (1)
#define RADIO_RST_PIN       (18)
#define RADIO_DIO0_PIN      (14)
#define RADIO_DIO1_PIN      (21)

#define RADIO_LDO_EN        (16)
#define RADIO_CTRL          (39)   // LOW = TX path, HIGH = RX path per board design

// ---------------- Configuration Defaults ----------------
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ 145.10      // MHz
#endif

#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER 10  // dBm (adjust responsibly)
#endif

#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW 125.0         // kHz
#endif

// User adjustable beacon settings
static const char* CALLSIGN = "N0CALL";  // Replace with your legal call sign if transmitting on amateur spectrum
static const unsigned BEACON_INTERVAL_MS = 60000; // 60s
static uint32_t frameCounter = 0;
static unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println();
  Serial.println(F("[BOOT] T-Beam BPF LoRa Beacon"));

  // Initialize SPI bus for radio
  SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

  // Power enables
  pinMode(RADIO_LDO_EN, OUTPUT);
  digitalWrite(RADIO_LDO_EN, HIGH);  // Enable radio LDO (if tied)

  pinMode(RADIO_CTRL, OUTPUT);
  digitalWrite(RADIO_CTRL, LOW);     // Select TX path for beaconing

  // Initialize LoRa radio
  LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
  if (!LoRa.begin((long)(CONFIG_RADIO_FREQ * 1000000))) {
    Serial.println(F("[ERR] LoRa init failed"));
    while (true) { delay(1000); }
  }

  LoRa.setTxPower(CONFIG_RADIO_OUTPUT_POWER);
  LoRa.setSignalBandwidth((long)(CONFIG_RADIO_BW * 1000));
  LoRa.setSpreadingFactor(9);      // SF9 (moderate airtime)
  LoRa.setCodingRate4(5);          // 4/5
  LoRa.setPreambleLength(8);
  LoRa.enableCrc();

  Serial.printf("[INFO] Beacon active @ %.3f MHz BW=%.1f kHz Power=%d dBm\n",
                CONFIG_RADIO_FREQ, CONFIG_RADIO_BW, CONFIG_RADIO_OUTPUT_POWER);
}

void loop() {
  unsigned long now = millis();
  if (now - lastSend >= BEACON_INTERVAL_MS) {
    lastSend = now;
    frameCounter++;
    String msg = String(CALLSIGN) + ",FC=" + frameCounter + ",UptMs=" + now;
    Serial.println("[TX] " + msg);
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
  }
  delay(10);
}