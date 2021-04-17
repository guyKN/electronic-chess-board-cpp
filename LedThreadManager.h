//
// Created by guykn on 3/5/2021.
//

#ifndef CHESSBOARD_LEDTHREADMANAGER_H
#define CHESSBOARD_LEDTHREADMANAGER_H


#include <vector>
#include <thread>
#include "Electronics.h"

class LedThreadManager {

    static const int threadingKey = 0;

    LedController& ledController;
    uint64_t constantLeds = 0;
    uint64_t slowBlinkingLeds[2] = {0,0};
    uint64_t fastBlinkingLeds[2] = {0,0};
    unsigned slowBlinkDuration = 500;
    unsigned fastBlinkDuration = 125;

    bool shouldStop = false;
    bool shouldResetBlinkTimer = false;

    struct TemporaryLedData {
        uint64_t ledsToEnable;
        unsigned endTime;
    };
    std::vector<TemporaryLedData> temporaryLedVector{};

    uint64_t getTemporaryLeds();

    std::thread ledThread{};
    void threadLoop();


public:
    LedThreadManager(const LedThreadManager&) = delete;
    LedThreadManager(const LedThreadManager&&) = delete;
    LedThreadManager& operator=(const LedThreadManager&) = delete;
    LedThreadManager& operator=(const LedThreadManager&&) = delete;

    explicit LedThreadManager(LedController &ledController);

    void init();

    void setConstantLeds(uint64_t constantLeds);

    void setSlowBlinkingLeds(uint64_t blink1, uint64_t blink2);
    void setSlowBlinkDuration(unsigned blinkDuration);

    void setFastBlinkingLeds(uint64_t blink1, uint64_t blink2);
    void setFastBlinkDuration(unsigned blinkDuration);

    void resetBlinkTimer();

    void writeTempLeds(uint64_t leds, unsigned duration);

    void clearTempLeds();

    void stopThread();

    void printRefreshRateMisses();
};


#endif //CHESSBOARD_LEDTHREADMANAGER_H
