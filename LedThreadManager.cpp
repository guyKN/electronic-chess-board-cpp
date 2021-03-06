//
// Created by guykn on 3/5/2021.
//

#include "LedThreadManager.h"

#include <utility>
#include <wiringPi.h>
#include <iostream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

PI_THREAD(ledThread) {
    LedThreadManager *threadManager = LedThreadManager::instance;
    unsigned int prevTime = millis();
    int blinkState = 0;
    for (;;) {
        if (threadManager->shouldStop) {
            break;
        }
        unsigned int currentTime = millis();
        if (currentTime - prevTime > 500) {
            prevTime = currentTime;
            blinkState++;
            blinkState %= 2;
        }
        uint64_t board = threadManager->ledData[blinkState];

        threadManager->ledController.setLeds(board);
    }
    LedThreadManager::instance = nullptr;
}

#pragma clang diagnostic pop

LedThreadManager *LedThreadManager::instance = nullptr;

LedThreadManager::LedThreadManager(LedController ledController, unsigned blinkDelay) : ledController(
        std::move(ledController)),
                                                                                       blinkDelay(blinkDelay) {
}

void LedThreadManager::init() {
    if (instance != nullptr) {
        std::cout << "Error: trying to start multiple led threads. \n";
        abort();
    }
    ledData[0] = 0;
    ledData[1] = 0;
    instance = this;
    temporaryLedVector.clear();
    piThreadCreate(ledThread);
}


void LedThreadManager::setLeds(uint64_t blink1, uint64_t blink2) {
    ledData[0] = blink1;
    ledData[1] = blink2;
}

void LedThreadManager::setTemporaryLeds(uint64_t leds, unsigned int duration) {
    unsigned endTime = millis() + duration;
    TemporaryLedData temporaryLedData{leds, endTime};
    piLock(threadingKey);
    temporaryLedVector.push_back(temporaryLedData);
    piUnlock(threadingKey);
}

void LedThreadManager::stopThread() {
    shouldStop = true;
    delay(20); //todo: find a better way to wait for thread to stop
}

uint64_t LedThreadManager::getTemporaryLeds() {
    unsigned time = millis();
    uint64_t temporaryLeds = 0;
    piLock(threadingKey);

    // turn on any leds that are temporary turned on, and delete temoraryLedData for LEDs that have expired
    auto iterator = temporaryLedVector.begin();
    while (iterator != temporaryLedVector.end()) {
        if (iterator->endTime <= time) {
            temporaryLeds |= iterator->ledsToEnable;
            iterator++;
        } else {
            temporaryLedVector.erase(iterator);
        }
    }
    piUnlock(threadingKey);

}

void LedThreadManager::threadLoop() {

}
