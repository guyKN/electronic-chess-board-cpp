//
// Created by guykn on 3/5/2021.
//

#include "LedThread.h"

#include <utility>
#include <wiringPi.h>
#include <iostream>

LedThread::LedThread(LedController ledController, unsigned blinkDelay) : ledController(std::move(ledController)),
                                                                         blinkDelay(blinkDelay),
                                                                         myThread(&LedThread::threadLoop,this) {
    ledData[0] = 0;
    ledData[1] = 0;
}

[[noreturn]] void LedThread::threadLoop() {
    unsigned prevTime = millis();
    int blinkState = 0;
    for (;;) {
        unsigned currentTime = millis();
        if (currentTime - prevTime > 500){
            prevTime = currentTime;
            blinkState++;
            blinkState%=2;
        }
        ledController.setLeds(ledData[blinkState]);
    }
}

void LedThread::setLeds(uint64_t blink1, uint64_t blink2) {
    ledData[0] = blink1;
    ledData[1] = blink2;
}