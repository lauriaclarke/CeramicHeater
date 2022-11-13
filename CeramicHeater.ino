#include "CeramicHeater.h"

#define BAUD_RATE   115200

// pin definitions
#define ENABLE_PIN  0
#define ERROR_PIN   1
#define TEMP_PIN    A0
// debug prints
#define DEBUG       1
// target temperature (max set point is 84)
#define TARGET_TEMP 75
// hysterisis for temperature control
#define DELTA_TEMP  4


CeramicHeater heater(ENABLE_PIN, ERROR_PIN, TEMP_PIN);

void setup() {
  Serial.begin(BAUD_RATE);
  heater.setup(TARGET_TEMP, DELTA_TEMP, DEBUG);
}

void loop() {
  heater.run();
  delay(1000);
}
