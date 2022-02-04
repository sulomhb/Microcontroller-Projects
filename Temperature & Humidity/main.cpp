#include "mbed.h"
#include "DFRobot_RGBLCD.h"
#include "./HTS221Sensor.h"
#include "DevI2C.h"

#define WAIT_TIME_MS 1000
InterruptIn button(PD_14);
DevI2C i2c(PB_11, PB_10);
HTS221Sensor hts221(&i2c);
DFRobot_RGBLCD rgbLCD(16,2, PB_9, PB_8);
bool button_flag=true;

void flip() {
  button_flag= !button_flag;
}

int main() {
    /* Initialiserer sensor*/
    if (hts221.init(NULL) != 0) printf("Failed to initialize");
    if (hts221.enable() != 0) printf("Failed to power up");
    /* Initialiserer LCD-skjerm */
    rgbLCD.init();

    /*Hver gang knapp trykkes, så endrer vi state.*/
	button.rise(&flip);

while (1) {
   float hum;
   float temp;
   
   /* Henter ut temperature og humidity */
   hts221.get_humidity(&hum);
   hts221.get_temperature(&temp);
   printf("Temperature: %.1f | Humidity: %.1f\n", temp, hum);

   /* Sjekker hvilken state knappen er i, og hvis den er true: vis temp*/
   if(button_flag == true) {
        if(temp < 20) rgbLCD.setRGB(0x00, 0x00, 255);
        if (temp >= 20 && temp <= 24) rgbLCD.setRGB(255, 69, 0);
        if (temp > 24) rgbLCD.setRGB(255, 0, 0);
        rgbLCD.clear();
        rgbLCD.printf("Temperature:%.1f", temp);
        thread_sleep_for(WAIT_TIME_MS);
        continue;
        }
   
   /* Sjekker hvilken state knappen er i, og hvis den er true: vis humidity*/
   if(button_flag == false) {
        rgbLCD.clear();
        rgbLCD.setRGB(int(255-(2.55f*hum)), int(255-(2.55f*hum)), 255);
        rgbLCD.printf("Humidity:%.1f", hum);
        thread_sleep_for(WAIT_TIME_MS);
        continue;
        }
    }
}