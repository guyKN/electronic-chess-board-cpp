//
// Created by guykn on 3/5/2021.
//

#include "LedThreadManager.h"

#include <wiringPi.h>
#include <iostream>

//todo: ensure that timing is correct and avoid the problem with overflow

LedThreadManager::LedThreadManager(LedController &ledController) :
        ledController(ledController) {
    setSlowBlinkingLeds(0,0);
    setFastBlinkingLeds(0,0);
}

void LedThreadManager::init() {
    slowBlinkingLeds[0] = 0;
    slowBlinkingLeds[1] = 0;
    temporaryLedVector.clear();
    ledThread = std::thread(&LedThreadManager::threadLoop, this);
}


void LedThreadManager::setSlowBlinkingLeds(uint64_t blink1, uint64_t blink2) {
    slowBlinkingLeds[0] = blink1;
    slowBlinkingLeds[1] = blink2;
}

void LedThreadManager::stopThread() {
    if (!shouldStop) {
        shouldStop = true;
        ledThread.join();
    }
}

void LedThreadManager::writeTempLeds(uint64_t leds, unsigned int duration) {
    unsigned endTime = millis() + duration;
    TemporaryLedData temporaryLedData{leds, endTime};
    piLock(threadingKey);
    temporaryLedVector.push_back(temporaryLedData);
    piUnlock(threadingKey);
}

void LedThreadManager::clearTempLeds() {
    piLock(threadingKey);
    temporaryLedVector.clear();
    piUnlock(threadingKey);
}

uint64_t LedThreadManager::getTemporaryLeds() {
    // if actually using this method, deal with overflow of millis() function after ~50 days
    unsigned time = millis();
    uint64_t temporaryLeds = 0;
    piLock(threadingKey);

    // turn on any leds that are temporary turned on, and delete temoraryLedData for LEDs that have expired
    auto iterator = temporaryLedVector.begin();
    while (iterator != temporaryLedVector.end()) {
        if (iterator->endTime >= time) {
            temporaryLeds |= iterator->ledsToEnable;
            iterator++;
        } else {
            temporaryLedVector.erase(iterator);
        }
    }
    piUnlock(threadingKey);
    return temporaryLeds;
}


//todo: ensure equal brightness with blinking leds
void LedThreadManager::threadLoop() {
    unsigned int slowBlinkPrevTime = millis();
    unsigned int fastBlinkPrevTime = millis();
    int slowBlinkState = 0;
    int fastBlinkState = 0;

    // when no leds are set to be turned on, we only need to call setLeds() once, and then the leds will stay off forever.
    bool didEmptyLeds = false; // if this variable is true we have already set the leds to empty so we don't need to do that again.

    for (;;) {
        unsigned int currentTime = millis();
        if (shouldStop) {
            break;
        }
        if (shouldResetBlinkTimer) {
            shouldResetBlinkTimer = false;
            slowBlinkState = 0;
            slowBlinkPrevTime = currentTime;

            fastBlinkState = 0;
            fastBlinkPrevTime = currentTime;
        }
        if (static_cast<unsigned>(currentTime - slowBlinkPrevTime) > slowBlinkDuration) {
            slowBlinkPrevTime = currentTime;
            slowBlinkState++;
            slowBlinkState %= 2;

            if (slowBlinkState == 0) {
                printRefreshRateMisses();
            }
        }
        if (static_cast<unsigned>(currentTime - fastBlinkPrevTime) > fastBlinkDuration) {
            fastBlinkPrevTime = currentTime;
            fastBlinkState++;
            fastBlinkState %= 2;
        }

        uint64_t temporaryLeds = getTemporaryLeds();
        uint64_t currentLeds =
                constantLeds | slowBlinkingLeds[slowBlinkState] | fastBlinkingLeds[fastBlinkState] | temporaryLeds;
        uint64_t otherLeds =
                constantLeds | slowBlinkingLeds[!slowBlinkState] | fastBlinkingLeds[!fastBlinkState] | temporaryLeds;


        if(currentLeds == 0){
            if (didEmptyLeds){
                delay(8);
                continue;
            } else{
                didEmptyLeds = true;
            }
        } else{
            didEmptyLeds = false;
        }

        ledController.setLeds(currentLeds);
        if (useEqualBrightness) {
            ledController.makeLedsEqualBrightness(currentLeds, otherLeds);
        }
    }
}

void LedThreadManager::printRefreshRateMisses() {
    int numTimesMissedRefreshRate = ledController.countRefreshRateMisses();
    if (numTimesMissedRefreshRate !=0){
        std::cout << "Warning: Refresh Rate too high. Missed " << numTimesMissedRefreshRate << "times!" << std::endl;
    }
}

void LedThreadManager::resetBlinkTimer() {
    shouldResetBlinkTimer = true;
}

void LedThreadManager::setSlowBlinkDuration(unsigned int blinkDuration) {
    this->slowBlinkDuration = blinkDuration;
}

void LedThreadManager::setConstantLeds(uint64_t constantLeds) {
    this->constantLeds = constantLeds;
}

void LedThreadManager::setFastBlinkingLeds(uint64_t blink1, uint64_t blink2) {
    this->fastBlinkingLeds[0] = blink1;
    this->fastBlinkingLeds[1] = blink2;
}

void LedThreadManager::setFastBlinkDuration(unsigned blinkDuration) {
    this->slowBlinkDuration = blinkDuration;
}

void LedThreadManager::setUseEqualBrightness(bool useEqualBrightness) {
    this->useEqualBrightness = useEqualBrightness;
}
