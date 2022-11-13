#ifndef CeramicHeater_h
#define CeramicHeater_h

#include <Arduino.h>
#include <avr/pgmspace.h>

#define NUMSAMPLES 3
#define SERIESRESISTOR 10000


#define PRECISION  1
// length of table
#define LUT_LENGTH 126
// defgrees between entries
#define UNITS      1
// temp in C of 0th entry
#define OFFSET     0

#define FLASH(X) pgm_read_word_near(X)

const uint16_t tempLUT[] PROGMEM = {274, 283, 292, 301, 310, 319, 328, 337, 347, 356, 366, 375, 385, 395, 404, 414, 424, 434, 443, 453, 463, 473, 483, 492, 502, 512, 521, 531, 540, 549, 559, 568, 577, 586, 595, 604, 613, 621, 630, 638, 646, 655, 663, 671, 678, 686, 694, 701, 708, 715, 723, 729, 736, 743, 749, 756, 762, 768, 774, 780, 786, 791, 797, 802, 807, 813, 818, 823, 827, 832, 837, 841, 845, 850, 854, 858, 862, 866, 869, 873, 877, 880, 884, 887, 890, 893, 896, 899, 902, 905, 908, 911, 913, 916, 918, 921, 923, 926, 928, 930, 932, 934, 936, 938, 940, 942, 944, 946, 948, 949, 951, 953, 954, 956, 957, 959, 960, 961, 963, 964, 965, 967, 968, 969, 970, 971};

class CeramicHeater {
  private:
    // pin setup
    int enablePin;
    int tempPin;
    int errorPin;
    // state variables
    int currentTemp;
    int targetTemp;
    int deltaTemp;
    // state variable
    //  0 = off
    //  1 = on
    // -1 = error
    int heaterState;
    // debug variable
    bool debug = false;

  public:
    // constructor
    CeramicHeater(int enable, int error, int temp);
    // setup the pins and set a temp
    void setup(int target, int delta, bool debug);
    // get the temperature
    int getTemp();
    // get the temperature from lookup table
    int lookupTemp(uint16_t analogValue);
    // helper funtion for getTemp
    uint16_t interpolate(uint16_t x_low16, uint16_t x_hi16, int32_t y_low, int32_t y_hi, uint16_t x16);
    // turn on and check for errors
    int turnOn();
    // get the error state
    int getError();
    // turn it off!
    int turnOff();
    // set the target heating temp
    void setTargetTemp(int target);
    // get the target temp
    int getTargetTemp();
    // get the current state
    void setDeltaTemp(int delta);
    // set the delta temp
    int getDeltaTemp();
    // get the delta temp
    int getState();
    // run the heater at the set temp
    void run();
};

#endif
