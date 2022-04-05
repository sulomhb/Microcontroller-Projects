#include "mbed.h"


volatile bool running = true;

struct Counter
{
    int seconds;
    int minutes;
    Mutex mutex;
};


void increaseMinutes(Counter *counter) 
{
    counter->minutes = 0;
    while(running)
    {
        thread_sleep_for(60000);
        counter->mutex.lock();
        counter->minutes++;
        counter->mutex.unlock();
    }
}

void increaseSeconds(Counter *counter) 
{
    counter->seconds = 0;
    while(running)
    {
        thread_sleep_for(1000);
        counter->mutex.lock();
        counter->seconds++;
        counter->mutex.unlock();
    }
}

void print_Counter(Counter *counter) {
    while(running)
    {
        /* resetter sekunder på 60 som en klokke, og stopper klokka på en time bare 
           så programmet faktisk stopper en gang og ikke kjører i evig tid*/
        if(counter->seconds == 60)
            counter->seconds = 0;
        if(counter->minutes == 60)
            running = false;

        counter->mutex.lock();
        printf("Seconds: %d : Minutes: %d\n", counter->seconds, counter->minutes);
        counter->mutex.unlock();
        thread_sleep_for(500);
    }
}


int main()
{
    Counter counter;
    
    Thread minuteCounter;
    Thread secondCounter;
    Thread printCounter;

    minuteCounter.start(callback(increaseMinutes, &counter));
    minuteCounter.set_priority(osPriorityNormal1);

    secondCounter.start(callback(increaseSeconds, &counter));
    secondCounter.set_priority(osPriorityNormal2);
    
    printCounter.start(callback(print_Counter, &counter));
    
    // siden alle funksjonene avslutter når running = false så trenger vi bare joine den ene tråden
    secondCounter.join();
    return 0;
}