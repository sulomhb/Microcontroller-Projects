#include "mbed.h"
#define WAIT_TIME_MS 100


PwmOut led1(PB_0);
PwmOut led2(PB_4);
PwmOut led3(PB_1);
DigitalIn button(PD_14);
AnalogIn potentiometer(PC_5);


int main() {

  while (true) {
    int buttonValue = button.read();
    float potentiometer_value = potentiometer.read();

    if(buttonValue == 0) {
        led1 = 1;
        led2 = 1;
        led3 = 1;
        continue;
    }

    if (potentiometer_value <= 0.33f) {
      led1 = 3 * potentiometer_value;
      led2 = 0;
      led3 = 0;
    }
    if (potentiometer_value > 0.33f && potentiometer_value <= 0.66f) {
      led1 = 1;
      led2 = 3 * (potentiometer_value - 0.33f);
      led3 = 0;
    }
    if (potentiometer_value > 0.66f) {
      led1 = 1;
      led2 = 1;
      led3 = 3 * (potentiometer_value - 0.66f);
    }
    thread_sleep_for(WAIT_TIME_MS);
  }
}
