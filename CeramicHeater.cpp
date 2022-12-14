#include "CeramicHeater.h"



CeramicHeater::CeramicHeater(int enable, int error, int temp) {
  // assign Pin values
  enablePin = enable;
  tempPin = temp;
  errorPin = error;
}

void CeramicHeater::setup(int target, int delta, bool debug) {
  this->debug = debug;
  pinMode(enablePin, OUTPUT);
  pinMode(errorPin, INPUT);
  pinMode(13, OUTPUT);
  analogReference(DEFAULT);
  setTargetTemp(target);
  setDeltaTemp(delta);
}

// interpolation function
uint16_t CeramicHeater::interpolate(uint16_t x_low16, uint16_t x_hi16, int32_t y_low, int32_t y_hi, uint16_t x16) {
  //cast to 32 bits unsigned to avoid overflow
  int32_t x_low = (uint32_t) x_low16;
  int32_t x_hi = (uint32_t) x_hi16;
  int32_t x = (uint32_t) x16;
  int32_t interpolated = ((x_hi - x) * y_low + (x - x_low) * y_hi) / (x_hi - x_low);
  return (int16_t) ( interpolated );
}

// binary search for analog value in temperature lookup table
int CeramicHeater::lookupTemp(uint16_t analogValue) {
  // make sure analog value is in range 0 - 125 C
  if (analogValue < FLASH(tempLUT + 0)) {
    Serial.print("*** ERROR: temp below 0 C ***");
    return (OFFSET - 1) * PRECISION;
  }
  if (analogValue > FLASH(tempLUT + LUT_LENGTH - 1)) {
    Serial.print("*** ERROR: temp above 125 C ***");
    return (OFFSET + LUT_LENGTH * UNITS + 1) * PRECISION;
  }

  // pick ends of array to start binary search
  uint16_t high = LUT_LENGTH - 1;
  uint16_t low = 0;

  while (high - low > 1) {
    // get the value in the middle of the lookup table
    uint16_t mid = (high + low) >> 1;
    uint16_t midval = FLASH(tempLUT + mid);
    // if we hit the correct value then return
    if (midval == analogValue) {
      return (mid * UNITS + OFFSET) * PRECISION;
    }
    // set new hi / low for binary search
    if (midval > analogValue) {
      high = mid;
    } else if (midval < analogValue) {
      low = mid;
    }
  }

  // interpolate between values
  int32_t low_temp = (int16_t(low * UNITS + OFFSET)) * PRECISION;
  int32_t high_temp = (int16_t(high * UNITS + OFFSET)) * PRECISION;
  return interpolate(FLASH(tempLUT + low), FLASH(tempLUT + high), low_temp, high_temp, analogValue);
}

// takes ~10ms per NUMSAMPLES
int CeramicHeater::getTemp() {
  float average = 0;
  float sum = 0;
  int value = 0;

  for (int i = 0; i < NUMSAMPLES; i++) {
    value = analogRead(tempPin);
    sum += value;
    delay(10);
  }

  average = sum / NUMSAMPLES;

  // resisitance
  // int resistance = 1023.0 / average - 1.0;
  // resistance = SERIESRESISTOR / average;
  // voltage
  float voltage = average * (5.0 / 1023.0);

  int temp = lookupTemp((uint16_t) average);
  currentTemp = temp;

  if (debug) {
    // Serial.print("resistance: ");
    // Serial.println(resistance);
    Serial.print("voltage: ");
    Serial.print(voltage);
    Serial.print("V \ncurrent temp: ");
    Serial.print(temp);
    Serial.println("C \n\n");
  }

  return temp;
}

// turn on
int CeramicHeater::turnOn() {
  // turn on enable
  heaterState = 1;
  digitalWrite(enablePin, HIGH);
  digitalWrite(13, HIGH);

  return 1;
}

// get the error state
int CeramicHeater::getError() {
  if (digitalRead(errorPin) == LOW) {
    // set state variable to error
    heaterState = -1;
    return -1;
  } else {
    // reset state variable to 0 if we were previously in an error state
    if (heaterState == -1) {
      heaterState = 0;
    }
    return heaterState;
  }
}

// turn it off!
int CeramicHeater::turnOff() {
  heaterState = 0;
  digitalWrite(enablePin, LOW);
  digitalWrite(13, LOW);
  return 0;
}

// set the target temp
void CeramicHeater::setTargetTemp(int target) {
  targetTemp = target;
}

// get the target temp
int CeramicHeater::getTargetTemp() {
  return targetTemp;
}

// set the delta temp
void CeramicHeater::setDeltaTemp(int delta) {
  deltaTemp = delta;
}

// get the delta temp
int CeramicHeater::getDeltaTemp() {
  return deltaTemp;
}

// get the state
int CeramicHeater::getState() {
  return heaterState;
}

void CeramicHeater::run() {
  if (debug) {
    Serial.print("target temp: ");
    Serial.println(targetTemp);
    Serial.print("current state: ");
    Serial.println(getState());
  }

  if (getError() >= 0) {
    int currentTemp = getTemp();
    if (currentTemp >= targetTemp + deltaTemp / 2) {
      turnOff();
    } else if (currentTemp <= targetTemp - deltaTemp / 2 - deltaTemp % 2) {
      turnOn();
    }
  } else {
    Serial.println("** ERROR DETECTED **");
  }
}
