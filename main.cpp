#include <iostream>
#include <wiringPi.h>
#include "Electronics.h"
#include "LedThread.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"




const int outPins[8] = {25, 24, 23, 22, 21, 30, 14, 13};



int main() {
    wiringPiSetup();

    ShiftOutRegister groundRegister{-1,8,9,7};
    ShiftOutRegister voltageRegister{-1,15,0,1};
    LedController ledController{voltageRegister, groundRegister};
    ShiftInRegister shiftInRegister{29,28,27};
    BoardScanner boardScanner{shiftInRegister, outPins};

    //LedThread ledThread = LedThread(ledController, 500);

    std::cout << "newest version";
    boardScanner.scanLoop(nullptr);

// board   : 37, 35, 33, 31, 29, 27, 23, 21
// wiringPi: 25, 24, 23, 22, 21, 30, 14, 13
}

#pragma clang diagnostic pop