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
// temp considered "off"
#define OFF_TEMP    30


CeramicHeater heater(ENABLE_PIN, ERROR_PIN, TEMP_PIN);

void setup() {
  Serial.begin(BAUD_RATE);
  heater.setup(TARGET_TEMP, DELTA_TEMP, OFF_TEMP, DEBUG);
}


int deathCount[] = {1, 1, 1, 1};

void loop() {

  for(int i = 0; i < 4; i++){
    // reset local counter
    int cycleCount = 0;

    while(cycleCount < deathCount[i]){
      // cycle the heater
      heater.cycle();
      // increment the local counter
      cycleCount++;
      Serial.print("local cycle count: ");
      Serial.println(cycleCount);
      // keep track of the total cycles
      heater.incrementCycleCount();
      Serial.print("total heater cycles: ");
      Serial.println(heater.getCycleCount());
    }
    
    Serial.println("week is over!");
  }

  while(true){
    Serial.println("doing nothing");
    delay(100000);
  }

}
