#include "Arduino.h"
#include "Timer.h"

Timer::Timer(){
    hours = 0;
    minutes = 0;
    seconds = 0;
}

void Timer::setTime(int hours, int minutes, int seconds){
            setHours(hours);
            setMinutes(minutes);
            setSeconds(seconds);
        }
void Timer::incrementTimer(){
            if(seconds < 59){
                seconds++;
            }
            else{
                seconds = 0;
                if(minutes < 59){
                    minutes++;
                }
                else{
                    minutes = 0;
                    if(hours < 23){
                        hours++;
                    }
                    else{
                        hours = 0;
                    }
                }
            }
        };