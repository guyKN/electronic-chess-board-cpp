//
// Created by guykn on 3/5/2021.
//

#include <wiringPi.h>
#include <string>
#include <iostream>
#include "PythonInterface.h"
#include "Electronics.h"
#include "LedThreadManager.h"

namespace {
    bool isInitialized = false;

    ShiftOutRegister ledGroundRegister = ShiftOutRegister::ledGroundRegister();
    ShiftOutRegister ledVoltageRegister = ShiftOutRegister::ledVoltageRegister();
    LedController ledController{ledVoltageRegister, ledGroundRegister};
    LedThreadManager ledThreadManager = LedThreadManager(ledController, 500);

    ShiftInRegister scanGroundRegister = ShiftInRegister::scanGroundRegister();
    BoardScanner boardScanner{scanGroundRegister};
}

void initGpio() {
    if(!isInitialized) {
        isInitialized = true;
        wiringPiSetup();

        ledGroundRegister.init();

        ledVoltageRegister.init();

        ledThreadManager.init();

        scanGroundRegister.init();
        boardScanner.init();

    } else{
        std::cout << "Waring: called init() multiple times" << std::endl;
    }

}

uint64_t readBoard() {
    if(!isInitialized){
        std::cout << "Must call init() before using readBoard()" << std::endl;
        abort();
    }

    return boardScanner.scan();
}

void writeLeds(uint64_t blink1, uint64_t blink2) {
    if(!isInitialized){
        std::cout << "Must call init() before using writeLeds()" << std::endl;
        abort();
    }
    ledThreadManager.setLeds(blink1, blink2);
}

void cleanupGpio() {
    ledGroundRegister.cleanup();
    ledVoltageRegister.cleanup();
    scanGroundRegister.cleanup();
    boardScanner.cleanup();
    ledThreadManager.stopThread();
}

