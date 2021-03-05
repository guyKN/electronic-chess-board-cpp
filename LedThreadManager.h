//
// Created by guykn on 3/5/2021.
//

#ifndef CHESSBOARD_LEDTHREADMANAGER_H
#define CHESSBOARD_LEDTHREADMANAGER_H


#include "Electronics.h"

class LedThreadManager {
    friend void* ledThread(void*);
    LedController ledController;
    uint64_t ledData[2];
    const unsigned blinkDelay;

    [[noreturn]] void threadLoop();

public:
    LedThreadManager(LedController ledController, unsigned blinkDelay);

    void setLeds(uint64_t blink1, uint64_t blink2);

    static LedThreadManager* instance;
};


#endif //CHESSBOARD_LEDTHREADMANAGER_H
