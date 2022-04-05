#include "mbed.h"
#include "DFRobot_RGBLCD.h"
#include "DevI2C.h"
#include <chrono>
#include <ratio>
InterruptIn button_1(PD_14);
InterruptIn button_2(PB_0);
InterruptIn button_3(PA_3);
InterruptIn button_4(PB_4);
PwmOut buzzer(PB_1);
DFRobot_RGBLCD rgbLCD(16,2, PB_9, PB_8);
bool pause_flag=false;
bool done_flag=false;
Timeout t;
std::chrono::microseconds remaining_time = t.remaining_time();

void buzz() {
  done_flag = true;
  buzzer = 0.5f;
}


void lcd() 
{
    // Print remaining time 
    rgbLCD.clear();
    remaining_time = t.remaining_time();
    rgbLCD.printf("%.2d", remaining_time.count()/1000000);
}

void pause() {
    remaining_time = t.remaining_time();
    if (!pause_flag)
        t.attach(&buzz, remaining_time);
}

void pause_button() {
    pause_flag = !pause_flag;
    pause();
}
void reset() {
    t.attach(&buzz, 60s);
    pause_flag = true;
    pause();
}

void extend() {
    t.attach(&buzz, t.remaining_time()+5s);
}

void reduce() {
    t.attach(&buzz, t.remaining_time()-5s);
}


int main() {
  rgbLCD.init();
  t.attach(&buzz, 60s);
  button_1.fall(&pause_button);
  button_2.fall(&reset);
  button_3.fall(&extend);
  button_4.fall(&reduce);
  while (!done_flag)
  {
    if (pause_flag)
      t.attach(&buzz, remaining_time);
    lcd();
    thread_sleep_for(50);
  }
  return 0;
   }