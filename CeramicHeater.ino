#include "CeramicHeater.h"

#define BAUD_RATE   115200

#define ESP32 

#ifdef ESP32
// pin definitions for ESP32
#define ENABLE_PIN  12
#define ERROR_PIN   27
#define TEMP_PIN    26
#else
// pin definitions for Arduino UNO
#define ENABLE_PIN  2
#define ERROR_PIN   3
#define TEMP_PIN    A0
#endif


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
