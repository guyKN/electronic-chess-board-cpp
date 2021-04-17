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
    LedController ledController{ledVoltageRegister, ledGroundRegister, 2, 1};
    LedThreadManager ledThreadManager{ledController};

    ShiftInRegister scanGroundRegister = ShiftInRegister::scanGroundRegister();
    BoardScanner boardScanner{scanGroundRegister};
    BoardChangeDetector boardChangeDetector{boardScanner};
}

void init() {
    if (!isInitialized) {
        isInitialized = true;
        wiringPiSetup();

        ledGroundRegister.init();
        ledVoltageRegister.init();
        ledController.init();

        ledThreadManager.init();
        scanGroundRegister.init();
        boardScanner.init();

    } else {
        std::cout << "Waring: called init() multiple times" << std::endl;
    }
}

void testIndividualPin(int pin){
    std::cout << "testing pin: " << pin << "\n";
    digitalWrite(pin, 1);
};

void testPins() {
    ledGroundRegister.testPins(&testIndividualPin);
    ledVoltageRegister.testPins(&testIndividualPin);
    scanGroundRegister.testPins(&testIndividualPin);
    boardScanner.testPins(&testIndividualPin);
}

uint64_t scanBoard() {
    if (!isInitialized) {
        std::cout << "Must call init() before using scanBoard()" << std::endl;
        abort();
    }

    return boardScanner.scan();
}

uint64_t awaitBoardChange() {
    if (!isInitialized) {
        std::cout << "Must call init() before using scanBoard()" << std::endl;
        abort();
    }

    return boardChangeDetector.awaitBoardChange();
}

void setLeds(uint64_t const_leds, uint64_t slow_blink_leds, uint64_t slow_blink_leds_2, uint64_t fast_blink_leds,
             uint64_t fast_blink_leds_2, bool reset_blink_timer) {
    if (!isInitialized) {
        std::cout << "Must call init() before using setSlowBlinkingLeds()" << std::endl;
        abort();
    }


    ledThreadManager.setConstantLeds(const_leds);
    ledThreadManager.setSlowBlinkingLeds(slow_blink_leds, slow_blink_leds_2);
    ledThreadManager.setFastBlinkingLeds(fast_blink_leds, fast_blink_leds_2);
    if (reset_blink_timer) {
        ledThreadManager.resetBlinkTimer();
    }
}

void setSlowBlinkDuration(unsigned int blinkDuration) {
    ledThreadManager.setSlowBlinkDuration(blinkDuration);
}

void setFastBlinkDuration(unsigned int blinkDuration) {
    ledThreadManager.setFastBlinkDuration(blinkDuration);
}

void setTempLeds(uint64_t leds, unsigned int duration) {
    ledThreadManager.writeTempLeds(leds, duration);
}

void clearTempLeds() {
    ledThreadManager.clearTempLeds();
}


void resetBlinkTimer(){
    ledThreadManager.resetBlinkTimer();
}

void cleanup() {
    ledThreadManager.stopThread();
    ledGroundRegister.cleanup();
    ledVoltageRegister.cleanup();
    scanGroundRegister.cleanup();
    boardScanner.cleanup();
}

void setLedRefreshRate(int refreshRate) {
    ledController.setRefreshRate(refreshRate);
}
