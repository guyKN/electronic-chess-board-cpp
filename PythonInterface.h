//
// Created by guykn on 3/5/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef CHESSBOARD_PYTHONINTERFACE_H
#define CHESSBOARD_PYTHONINTERFACE_H

#include <cstdint>

void init();

uint64_t scanBoard();


uint64_t awaitBoardChange();


void setLeds(uint64_t const_leds = 0,
             uint64_t slow_blink_leds = 0, uint64_t slow_blink_leds_2 = 0,
             uint64_t fast_blink_leds = 0, uint64_t fast_blink_leds_2 = 0,
             bool reset_blink_timer = false);

void testPins();

void setLedRefreshRate(int refreshRate);

void setSlowBlinkDuration(unsigned int blinkDuration);

void setFastBlinkDuration(unsigned int blinkDuration);

void setUseEqualBrightness(bool useEqualBrightness);

void setEqualBrightnessFactor(int equalBrightnessFactor);

void resetBlinkTimer();

void setTempLeds(uint64_t leds, unsigned int duration);

void clearTempLeds();

void cleanup();

#endif //CHESSBOARD_PYTHONINTERFACE_H
#pragma clang diagnostic pop
