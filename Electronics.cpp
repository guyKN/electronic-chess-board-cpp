//
// Created by guykn on 3/2/2021.
//
//todo: figure out delays

#include "Electronics.h"
#include "LedThread.h"
#include <wiringPi.h>
#include <iostream>
#include <cassert>

std::ostream &printBoard(uint64_t bitboard, std::ostream &os = std::cout) {
    uint64_t squareMask = 1;
    os << "\n";
    for (int rank = 0; rank <= 7; ++rank) {
        for (int file = 0; file <= 7; ++file) {
            os << (squareMask & bitboard ? '1' : '.') << ' ';
            squareMask <<= 1;
        }
        os << std::endl;
    }
    return os;
}


ShiftInRegister::ShiftInRegister(int clockPin, int serialInput, int enableSerial) : clockPin(clockPin),
                                                                                    serialInputPin(serialInput),
                                                                                    enableSerialPin(enableSerial) {
    pinMode(clockPin, OUTPUT);
    digitalWrite(clockPin, 0);

    pinMode(enableSerial, OUTPUT);
    digitalWrite(enableSerial, 0);

    pinMode(serialInput, INPUT);
}

uint8_t ShiftInRegister::read() const {
    digitalWrite(enableSerialPin, 0);
    digitalWrite(clockPin, 1);
    digitalWrite(clockPin, 0);
    digitalWrite(enableSerialPin, 1);
    uint8_t data = 0;
    for (int i = 0; i < 8; ++i) {
        delay(1);
        data |= digitalRead(serialInputPin) ? 1 << i : 0;
        digitalWrite(clockPin, 1);
        delayMicroseconds(1000);
        digitalWrite(clockPin, 0);
    }
    digitalWrite(enableSerialPin, 0);
    return data;
}

void ShiftInRegister::readLoop() const {
    uint8_t prevData = read();
    printData(prevData);
    unsigned prevTime = millis();
    for(;;){
        uint8_t data = read();
        if (data != prevData){
            prevData = data;
            printData(data);
        }
        unsigned time = millis();
        if(time-prevTime>1000){
            prevTime = time;
            std::cout << "*" << std::flush;
        }
    }
}

void ShiftInRegister::printData(uint8_t data) {
    std::cout << "\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << ((data & (1<<i)) ? "1":".");
    }
    std::cout << "\n";
}


ShiftOutRegister::ShiftOutRegister(const int masterReset, const int dataClock, const int storageClock, const int dataOutput) : dataClock(dataClock), storageClock(storageClock),
                                                            dataOutput(dataOutput), masterReset(masterReset) {
    pinMode(dataClock, OUTPUT);
    digitalWrite(dataClock, 0);

    pinMode(storageClock, OUTPUT);
    digitalWrite(storageClock, 0);

    pinMode(dataOutput, OUTPUT);
    digitalWrite(dataOutput, 0);
    if(masterReset != -1) {
        pinMode(masterReset, OUTPUT);
        reset();
    }

}

void ShiftOutRegister::reset() const {
    assert(masterReset != -1);
    digitalWrite(masterReset, 0);
    digitalWrite(masterReset, 1);
}

inline void ShiftOutRegister::updateStorage() const {
    digitalWrite(storageClock, 1);
    digitalWrite(storageClock, 0);
}

inline void ShiftOutRegister::writeBit(bool bit) const {
    digitalWrite(dataOutput, bit);
    digitalWrite(dataClock, 1);
    digitalWrite(dataClock, 0);
    updateStorage();
}

void ShiftOutRegister::writeByte(uint8_t byte) const {
    for (int i = 0; i < 8; i++) {
        bool bit = static_cast<bool>(byte & (1 << i));
        writeBit(bit);
    }
    updateStorage();
}

BoardScanner::BoardScanner(const ShiftInRegister &shiftInRegister, const int *outPins)
        : shiftInRegister(shiftInRegister), outPins(outPins) {
    for (int i = 0; i < 8; ++i) {
        pinMode(outPins[i], OUTPUT);
        digitalWrite(outPins[i], 0);
    }
}

uint64_t BoardScanner::scan() {
    uint64_t board = 0;
    for (int i = 0; i < 8; ++i) {
        digitalWrite(outPins[i], 1);
        uint64_t data = shiftInRegister.read();
        board |= data << (8 * i);
        digitalWrite(outPins[i], 0);
    }
    // because of the different wiring in different places, we need to modify the board
    // so that ranks and files actually corespent to their phisical locations
    return board;
}

uint64_t BoardScanner::standardizeBoard(uint64_t board) {
    uint64_t newBoard = 0;
    for (int square = 0; square < 64; ++square) {
        if (board & (1 << square)) {
            int rank = square / 8;
            int file = square % 8;
            int standardizedSquare = rank +
                                     (file >= 4 ? (file - 4) * 8 : (7 - file) * 8);
            newBoard |= 1 << standardizedSquare;
        }
    }
    return newBoard;

}

[[noreturn]] void BoardScanner::scanLoop(LedThread *ledThread) {
    uint64_t prevBoard = scan();
    printBoard(prevBoard);
    if (ledThread != nullptr) {
        ledThread->setLeds(prevBoard, 0);
    }
    unsigned prevTime = millis();
    for(;;){
        uint64_t board = scan();
        if(board != prevBoard){
            prevTime = millis();
            prevBoard = board;
            std::cout << "\n";
            printBoard(board);
            std::cout << "\n";
            if (ledThread != nullptr) {
                ledThread->setLeds(board, 0);
            }
        }
        unsigned currentTime = millis();
        if(currentTime - prevTime > 1000){
            prevTime = currentTime;
            std::cout << "*" << std::flush;
        }
    }
}

LedController::LedController(const ShiftOutRegister &voltageRegister, const ShiftOutRegister &groundRegister)
        : voltageRegister(voltageRegister), groundRegister(groundRegister) {

}

inline void LedController::reset() {
    voltageRegister.reset();
    groundRegister.reset();
}


void LedController::setLeds(uint64_t board) const {
    board = ~board;
    //todo: go back to standardize
    voltageRegister.writeBit(1);

    for (int i = 0; i < 8; ++i) {
        auto row = static_cast<uint8_t>(board>>(8*i));
        if(row != 0xFF) {
            groundRegister.writeByte(row);
            delay(1); // todo: is it necessary to reduce the wait in order to also consider time spent writing to the shift Registers?
            //todo: go back to using refreshrate
        }
        voltageRegister.writeBit(0);
    }
}

uint64_t LedController::standardizeBoard(uint64_t board) {
    uint64_t newBoard = 0;
    for (int square = 0; square < 64; ++square) {
        if (board & (1 << square)) {
            int newSquareIndex = 8 * (square % 8) + square / 8;
            newBoard |= 1 << newSquareIndex;
        }
    }
    return newBoard;
}

[[noreturn]] void LedController::ledLoop() const {
    for(;;){
//        setLeds(0x0102040810204080);
        setLeds(0x55AA55AA55AA55AA);
    }
}
//change