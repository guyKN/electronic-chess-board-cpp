//
// Created by guykn on 3/5/2021.
//

#ifndef CHESSBOARD_LEDTHREAD_H
#define CHESSBOARD_LEDTHREAD_H


#include "Electronics.h"
#include<thread>

using std::thread;
class LedThread {
    LedController ledController;
    thread myThread;
    uint64_t ledData[2];
    const unsigned blinkDelay;

    [[noreturn]] void threadLoop();

public:
    LedThread(LedController ledController, unsigned blinkDelay);
    // aa1

    void setLeds(uint64_t blink1, uint64_t blink2);
};


#endif //CHESSBOARD_LEDTHREAD_H
