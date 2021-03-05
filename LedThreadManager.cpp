//
// Created by guykn on 3/5/2021.
//

#include "LedThreadManager.h"

#include <utility>
#include <wiringPi.h>
#include <iostream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
PI_THREAD(ledThread){
    LedThreadManager* threadManager = LedThreadManager::instance;
    unsigned prevTime = millis();
    int blinkState = 0;
    for (;;) {
        unsigned currentTime = millis();
        if (currentTime - prevTime > 500){
            prevTime = currentTime;
            blinkState++;
            blinkState%=2;
        }
        threadManager->ledController.setLeds(threadManager->ledData[blinkState]);
    }

}
#pragma clang diagnostic pop

LedThreadManager* LedThreadManager::instance = nullptr;

LedThreadManager::LedThreadManager(LedController ledController, unsigned blinkDelay) : ledController(std::move(ledController)),
                                                                                       blinkDelay(blinkDelay) {
    if(instance != nullptr){
        std::cout << "Error: trying to start multiple led threads. \n";
        abort();
    }
    ledData[0] = 0;
    ledData[1] = 0;
    instance = this;
    piThreadCreate(ledThread);
}

[[noreturn]] void LedThreadManager::threadLoop() {
}

void LedThreadManager::setLeds(uint64_t blink1, uint64_t blink2) {
    ledData[0] = blink1;
    ledData[1] = blink2;
}