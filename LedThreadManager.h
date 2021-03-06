//
// Created by guykn on 3/5/2021.
//

#ifndef CHESSBOARD_LEDTHREADMANAGER_H
#define CHESSBOARD_LEDTHREADMANAGER_H


#include <vector>
#include "Electronics.h"

class LedThreadManager {

    static const int threadingKey = 0;

    friend void* ledThread(void*);
    LedController ledController;
    uint64_t ledData[2];
    const unsigned blinkDelay;
    bool shouldStop = false;

    struct TemporaryLedData{
        uint64_t ledsToEnable;
        unsigned endTime;
    };

    std::vector<TemporaryLedData> temporaryLedVector;

    uint64_t getTemporaryLeds();

    void threadLoop();



public:
    LedThreadManager(LedController ledController, unsigned blinkDelay);
    static LedThreadManager* instance;

    void init();
    void setLeds(uint64_t blink1, uint64_t blink2);
    void setTemporaryLeds(uint64_t leds, unsigned duration);
    void stopThread();
};


#endif //CHESSBOARD_LEDTHREADMANAGER_H
