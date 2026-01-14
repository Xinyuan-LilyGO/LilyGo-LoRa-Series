#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorBMA423.hpp"

#ifndef SENSOR_SDA
#define SENSOR_SDA 10
#endif
#ifndef SENSOR_SCL
#define SENSOR_SCL 11
#endif
#ifndef SENSOR_IRQ
#define SENSOR_IRQ 14
#endif

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

SensorBMA423 accel;
volatile bool sensorIRQ = false;
void IRAM_ATTR setFlag() { sensorIRQ = true; }   // IRAM_ATTR ok on ESP; harmless elsewhere

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  pinMode(SENSOR_IRQ, INPUT);

  // Many watch boards use the "secondary" address 0x19
  if (!accel.begin(Wire, BMA423_I2C_ADDR_SECONDARY, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find BMA423 - check wiring!");
    for(;;) delay(1000);
  }
  Serial.println("Init BMA423 Sensor success!");

  // --- Accel running (ODR is fine with lib default) ---
  accel.configAccelerometer();        // lib’s default: 4G, ~200 Hz
  accel.enableAccelerometer();

  // --- Force board orientation (preset 5 = bottom layer, top-right corner) ---
  accel.setRemapAxes(SensorBMA423::REMAP_BOTTOM_LAYER_TOP_RIGHT_CORNER);
  Serial.println("Remap preset: REMAP_BOTTOM_LAYER_TOP_RIGHT_CORNER");

  // --- HARD RESET THE FEATURE ENGINE STATE / IRQ MAPS ---
  // Unmap all feature IRQs (public helpers)
  accel.disablePedometerIRQ();
  accel.disableActivityIRQ();
  accel.disableAnyNoMotionIRQ();
  accel.disableWakeupIRQ();
  accel.disableTiltIRQ();

  // Disable features you don’t want
  accel.enableFeature(SensorBMA423::FEATURE_STEP_CNTR, false);
  accel.enableFeature(SensorBMA423::FEATURE_ACTIVITY,  false);
  accel.enableFeature(SensorBMA423::FEATURE_ANY_MOTION,false);
  accel.enableFeature(SensorBMA423::FEATURE_NO_MOTION, false);
  accel.enableFeature(SensorBMA423::FEATURE_WAKEUP,    false);
  // Clear any latched status from the blob defaults
  accel.readIrqStatus();

  // --- Configure the INT pin and enable ONLY ANY-MOTION ---
  // edge_ctrl=0(level), level=1(active-high), od=0(push-pull),
  // output_en=1, input_en=0, int_line=0 (INT1)
  accel.configInterrupt(/*edge*/0, /*level*/1, /*od*/0, /*out_en*/1, /*in_en*/0, /*INT1*/0);

  accel.enableFeature(SensorBMA423::FEATURE_ANY_MOTION, true);
  accel.enableAnyNoMotionIRQ();  // map any-motion to INT1

  // Attach ISR (if your MCU requires digitalPinToInterrupt, use it)
#if defined(ESP_PLATFORM)
  attachInterrupt(SENSOR_IRQ, setFlag, RISING);
#else
  attachInterrupt(digitalPinToInterrupt(SENSOR_IRQ), setFlag, RISING);
#endif

  Serial.println("Any-motion IRQ armed. Move the device to trigger events...");
}

void loop() {
  if (sensorIRQ) {
    sensorIRQ = false;

    // Reading status clears the event (and releases the pin if latched by default)
    uint16_t status = accel.readIrqStatus();
    Serial.print("INT_STATUS: 0x");
    Serial.println(status,HEX);

    if (accel.isAnyNoMotion()) {
      Serial.println("ANY MOTION!");
    }
    // Optional: debug—if you still see other bits, uncomment to inspect
    // if (accel.isPedometer())   Serial.println("Step INT (unexpected)");
    // if (accel.isActivity())    Serial.println("Activity INT (unexpected)");
    // if (accel.isDoubleTap())   Serial.println("Wakeup/DoubleTap INT (unexpected)");
    // if (accel.isTilt())        Serial.println("Tilt INT (unexpected)");
  }

  delay(10);
}
