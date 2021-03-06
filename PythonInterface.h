//
// Created by guykn on 3/5/2021.
//

#ifndef CHESSBOARD_PYTHONINTERFACE_H
#define CHESSBOARD_PYTHONINTERFACE_H

#include <cstdint>

void initGpio();

uint64_t readBoard();

void writeLeds(uint64_t blink1, uint64_t blink2);

void cleanupGpio();


#endif //CHESSBOARD_PYTHONINTERFACE_H