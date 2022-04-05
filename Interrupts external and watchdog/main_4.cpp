#include "mbed.h"
#include "DFRobot_RGBLCD.h"
#include "DevI2C.h"

#define WAIT_TIME_MS 100
InterruptIn button_1(PD_14);
InterruptIn button_2(PB_0);
DFRobot_RGBLCD rgbLCD(16,2, PB_9, PB_8);
bool button_flag=true;
Timer t;
constexpr uint32_t WATCHDOG_TIMEOUT_MS = 10000;

void pause() {
  button_flag= !button_flag;
}

void refreshWatchdog() {
  Watchdog::get_instance().kick();							
}

int main() {
    rgbLCD.init();
  	button_1.fall(&refreshWatchdog);
	button_2.fall(&pause);
  	Watchdog &watchdog = Watchdog::get_instance();
	watchdog.start(WATCHDOG_TIMEOUT_MS);
  	t.start();


	while (1) {

    // Start watchdog
   if(button_flag == true) {
     t.start();
    
    rgbLCD.clear();
    double time = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();
    rgbLCD.printf("%.2f",(time/1000));
    thread_sleep_for(WAIT_TIME_MS);
   	continue;
   }
   
   // Timer stop
   if(button_flag == false) {
   watchdog.kick();
   t.stop();
   thread_sleep_for(WAIT_TIME_MS);
   continue;
   }
  }
}