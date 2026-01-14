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
static uint32_t lastStepCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // Many watch boards use the "secondary" address 0x19
  if (!accel.begin(Wire, BMA423_I2C_ADDR_SECONDARY, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find BMA423 - check wiring!");
    for(;;) delay(1000);
  }
  Serial.println("Init BMA423 Sensor success!");

  // Keep the accelerometer running for the step counter algorithm
  accel.configAccelerometer();        // library default: 4G, ~200 Hz
  accel.enableAccelerometer();

  // Force board orientation (preset 5 = bottom layer, top-right corner)
  accel.setRemapAxes(SensorBMA423::REMAP_BOTTOM_LAYER_TOP_RIGHT_CORNER);
  Serial.println("Remap preset: REMAP_BOTTOM_LAYER_TOP_RIGHT_CORNER");

  // Unmap all feature IRQs so the INT line stays idle
  accel.disablePedometerIRQ();
  accel.disableActivityIRQ();
  accel.disableAnyNoMotionIRQ();
  accel.disableWakeupIRQ();
  accel.disableTiltIRQ();

  // Disable any features that may be enabled by default
  accel.enableFeature(SensorBMA423::FEATURE_ACTIVITY,  false);
  accel.enableFeature(SensorBMA423::FEATURE_ANY_MOTION,false);
  accel.enableFeature(SensorBMA423::FEATURE_NO_MOTION, false);
  accel.enableFeature(SensorBMA423::FEATURE_WAKEUP,    false);
  accel.enableFeature(SensorBMA423::FEATURE_TILT,      false);

  // Enable only the step counter block
  accel.enableFeature(SensorBMA423::FEATURE_STEP_CNTR, true);
  accel.enablePedometer(true);
  accel.resetPedometer();
  lastStepCount = 0;

  // Clear any latched status bits from the firmware blob
  accel.readIrqStatus();

  Serial.println("Step counter enabled (polling mode, interrupts disabled).");
}

void loop() {
  uint32_t steps = accel.getPedometerCounter();
  if (steps != lastStepCount) {
    Serial.print("Steps: ");
    Serial.println(steps);
    lastStepCount = steps;
  }
  delay(200);
}
