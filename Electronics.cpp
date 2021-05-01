//
// Created by guykn on 3/2/2021.
// updated

#include "Electronics.h"
#include "LedThreadManager.h"
#include <wiringPi.h>
#include <iostream>

namespace {

    constexpr int NUM_COLS = 8;
    constexpr int NUM_ROWS = 8;
    constexpr int NUM_SQUARES = 64;

    const int boardScannerOutPins[8] = {25, 24, 23, 22, 21, 30, 14, 13};

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
}

uint64_t rotateBoard(uint64_t board) {
    uint64_t newBoard = 0;
    for (int square = 0; square < 64; ++square) {
        if (board & (1ll << square)) {
            int newSquareIndex = 8 * (7 - (square % 8)) + 7 - (square / 8);
            newBoard |= 1ll << newSquareIndex;
        }
    }
    return newBoard;
}

ShiftInRegister::ShiftInRegister(int clockPin, int serialInput, int enableSerial) : clockPin(clockPin),
                                                                                    serialInputPin(serialInput),
                                                                                    enableSerialPin(enableSerial) {}

void ShiftInRegister::init() {
    pinMode(clockPin, OUTPUT);
    digitalWrite(clockPin, 0);

    pinMode(enableSerialPin, OUTPUT);
    digitalWrite(enableSerialPin, 0);

    pinMode(serialInputPin, INPUT);

}


uint8_t ShiftInRegister::read() const {
    delayMicroseconds(150);
    digitalWrite(enableSerialPin, 0);
    digitalWrite(clockPin, 1);
    digitalWrite(clockPin, 0);
    digitalWrite(enableSerialPin, 1);
    uint8_t data = 0;
    for (int i = 0; i < 8; ++i) {
        data |= digitalRead(serialInputPin) ? 1 << i : 0;
        digitalWrite(clockPin, 1);
        delayMicroseconds(150);
        digitalWrite(clockPin, 0);
    }
    digitalWrite(enableSerialPin, 0);
    return data;
}

void ShiftInRegister::readLoop() const {
    uint8_t prevData = read();
    printData(prevData);
    unsigned prevTime = millis();
    for (;;) {
        uint8_t data = read();
        if (data != prevData) {
            prevData = data;
            printData(data);
        }
        unsigned time = millis();
        if (time - prevTime > 1000) {
            prevTime = time;
            std::cout << "*" << std::flush;
        }
    }
}

void ShiftInRegister::printData(uint8_t data) {
    std::cout << "\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << ((data & (1 << i)) ? "1" : ".");
    }
    std::cout << "\n";
}

ShiftInRegister ShiftInRegister::scanGroundRegister() {
    return ShiftInRegister(29, 28, 27);
}

void ShiftInRegister::cleanup() const {

    digitalWrite(clockPin, 0);
    pinMode(clockPin, INPUT);

    digitalWrite(enableSerialPin, 0);
    pinMode(enableSerialPin, INPUT);
}

void ShiftInRegister::testPins(void (test)(int)) {
    test(clockPin);
    test(enableSerialPin);
    pinMode(serialInputPin, OUTPUT); // for testing purposes only
    test(serialInputPin);
}


ShiftOutRegister::ShiftOutRegister(const int masterReset, const int dataClock, const int storageClock,
                                   const int dataOutput) : dataClock(dataClock), storageClock(storageClock),
                                                           dataOutput(dataOutput), masterReset(masterReset) {}


void ShiftOutRegister::init() const {
    pinMode(dataClock, OUTPUT);
    digitalWrite(dataClock, 0);

    pinMode(storageClock, OUTPUT);
    digitalWrite(storageClock, 0);

    pinMode(dataOutput, OUTPUT);
    digitalWrite(dataOutput, 0);
    if (masterReset != -1) {
        pinMode(masterReset, OUTPUT);
    }
    reset();
}


void ShiftOutRegister::reset() const {
    if (masterReset != -1) {
        digitalWrite(masterReset, 0);
        digitalWrite(masterReset, 1);
    } else {
        writeByte(0);
        updateStorage();
    }
}

inline void ShiftOutRegister::updateStorage() const {
    digitalWrite(storageClock, 1);
    digitalWrite(storageClock, 0);
}

inline void ShiftOutRegister::writeBit(bool bit) const {
    digitalWrite(dataOutput, bit);
    digitalWrite(dataClock, 1);
    digitalWrite(dataClock, 0);
}

inline void ShiftOutRegister::writeByte(uint8_t byte) const {
    for (int i = 0; i < 8; i++) {
        bool bit = static_cast<bool>(byte & (1 << i));
        writeBit(bit);
    }
}

ShiftOutRegister ShiftOutRegister::ledGroundRegister() {
    return ShiftOutRegister(-1, 8, 9, 7);
}

ShiftOutRegister ShiftOutRegister::ledVoltageRegister() {
    return ShiftOutRegister(-1, 15, 0, 1);
}

void ShiftOutRegister::cleanup() const {
    reset();

    if (masterReset != 0) {
        digitalWrite(masterReset, 0);
        pinMode(masterReset, INPUT);
    }

    digitalWrite(dataClock, 0);
    pinMode(dataClock, INPUT);

    digitalWrite(storageClock, 0);
    pinMode(storageClock, INPUT);

    digitalWrite(dataOutput, 0);
    pinMode(dataOutput, INPUT);
}

void ShiftOutRegister::testPins(void (*test)(int)) const {
    if (masterReset != -1) {
        test(masterReset);
    }
    test(dataOutput);
    test(storageClock);
    test(dataClock);
}

BoardScanner::BoardScanner(const ShiftInRegister &shiftInRegister, const int *outPins)
        : shiftInRegister(shiftInRegister), outPins(outPins) {}

BoardScanner::BoardScanner(const ShiftInRegister &shiftInRegister) : BoardScanner(shiftInRegister,
                                                                                  boardScannerOutPins) {}

void BoardScanner::init() const {
    for (int i = 0; i < 8; ++i) {
        pinMode(outPins[i], OUTPUT);
        digitalWrite(outPins[i], 0);
    }
}

uint64_t BoardScanner::scan() const {
    uint64_t board = 0;
    for (int i = 0; i < 8; ++i) {
        digitalWrite(outPins[i], 1);
        uint64_t data = shiftInRegister.read();
        board |= data << (8 * i);
        digitalWrite(outPins[i], 0);
    }
    // because of the different wiring in different places, we need to modify the board
    // so that ranks and files actually corespent to their phisical locations
    return rotateBoard(board);
}

[[noreturn]] void BoardScanner::scanLoop(LedThreadManager *ledThread) const {
    uint64_t prevBoard = scan();
    printBoard(prevBoard);
    if (ledThread != nullptr) {
        ledThread->setSlowBlinkingLeds(prevBoard, 0);
    }

    unsigned prevTime = millis();
    for (;;) {
        uint64_t board = scan();
        if (board != prevBoard) {
            prevTime = millis();
            prevBoard = board;
            std::cout << "\n";
            printBoard(board);
            std::cout << "\n";
            if (ledThread != nullptr) {
                ledThread->setSlowBlinkingLeds(board, 0);
            }
        }
        unsigned currentTime = millis();
        if (currentTime - prevTime > 1000) {
            prevTime = currentTime;
            std::cout << "*" << std::flush;
        }
    }
}

void BoardScanner::cleanup() {
    for (int i = 0; i < 8; ++i) {
        digitalWrite(outPins[i], 0);
        pinMode(outPins[i], INPUT);
    }
}

void BoardScanner::testPins(void (test)(int)) const {
    for (int i = 0; i < 8; ++i) {
        test(outPins[i]);
    }
}


LedController::LedController(const ShiftOutRegister &voltageRegister, const ShiftOutRegister &groundRegister,
                             int patchPin,
                             int patchIndex)
        : voltageRegister(voltageRegister), groundRegister(groundRegister), patchPin(patchPin),
          patchIndex(patchIndex) {}

void LedController::reset() {
    voltageRegister.reset();
    groundRegister.reset();
}

uint8_t rearrangeRow(uint8_t row) {
    uint8_t ret = row & 0b11111001;
    if (row & 2) {
        ret |= 4;
    }
    if (row & 4) {
        ret |= 2;
    }
    return ret;
}

void LedController::setLeds(uint64_t board) {
    board = ~rotateBoard(board);

    if (board == 0xFFFFFFFFFFFFFFFF) {
        // if the board is set to be completely empty, simply turn off all leds and wait 8 milliseconds
        voltageRegister.writeByte(0);
        groundRegister.writeByte(0xFF);
        groundRegister.updateStorage();
        voltageRegister.updateStorage();
        digitalWrite(patchPin, 1);
        delay(8);
        return;
    }

    voltageRegister.writeBit(1);
    for (int i = 0; i < 8; ++i) {
        uint8_t row = rearrangeRow(static_cast<uint8_t>(board >> (8 * i)));
        if (row != 0xFF) {
            unsigned startTime = micros();

            digitalWrite(patchPin, 1);
            groundRegister.writeByte(row);
            groundRegister.updateStorage();
            voltageRegister.updateStorage();
            digitalWrite(patchPin, row & (1 << patchIndex));

            unsigned endTime = micros();
            unsigned timeElapsed = endTime - startTime;
            if (timeElapsed >= microDelayBetweenRows) {
                onMissRefreshRate();
            } else {
                delayMicroseconds(microDelayBetweenRows - timeElapsed);
            }

            digitalWrite(patchPin, 1);
        }
        voltageRegister.writeBit(0);
    }
    voltageRegister.reset();
    digitalWrite(patchPin, 1);
}

[[noreturn]] void LedController::ledLoop() {
    for (;;) {
        setLeds(0x55AA55AA55AA55AA);
    }
}

void LedController::init() {
    pinMode(patchPin, OUTPUT);
    digitalWrite(patchPin, 1);
}

void LedController::cleanup() const {
    digitalWrite(patchPin, 0);
    pinMode(patchPin, INPUT);
}

void LedController::setRefreshRate(int refreshRate) {
    microDelayBetweenRows = 1000000 / NUM_ROWS / refreshRate;
    std::cout << "microDelayBetweenRows: " << microDelayBetweenRows << std::endl;
}

void LedController::onMissRefreshRate() {
    numTimesMissedRefreshRate++;
}

int LedController::countRefreshRateMisses() {
    int temp = numTimesMissedRefreshRate;
    numTimesMissedRefreshRate = 0;
    return temp;
}

void LedController::makeLedsEqualBrightness(uint64_t currentLeds, uint64_t otherLeds) const {
    int currentNumCols = numNonZeroColumns(currentLeds);
    int otherNumCols = numNonZeroColumns(otherLeds);
//    std::cout << "currentNumCols: " << currentNumCols << "      otherNumCols: " << otherNumCols;
    if (currentNumCols < otherNumCols) {
        int columnsDifference = otherNumCols - currentNumCols;
        delayMicroseconds((columnsDifference * microDelayBetweenRows * equalBrightnessFactor) / 1000);
//        std::cout << "     columnDifference: " << columnsDifference;
//        std::cout << "     sleeping for: " << columnsDifference * microDelayBetweenRows;
    }
//    std::cout << std::endl;
}

int LedController::numNonZeroColumns(uint64_t board) {
    int numActiveColumns = 0;
    constexpr uint64_t COL_1_MASK = 0x0101010101010101;
    uint64_t columnMask = COL_1_MASK;

    for (int i = 0; i < NUM_COLS; ++i) {
        if (board & columnMask) {
            numActiveColumns++;
        }
        columnMask <<= 1;
    }

    return numActiveColumns;

}

void LedController::setEqualBrightnessFactor(int equalBrightnessFactor) {
    this->equalBrightnessFactor = equalBrightnessFactor;
}

BoardChangeDetector::BoardChangeDetector(const BoardScanner &boardScanner) : boardScanner(boardScanner) {}

uint64_t BoardChangeDetector::awaitBoardChange() {
    if (!hasStarted) {
        hasStarted = true;
        return boardScanner.scan();
    }

    for (;;) {
        uint64_t board = boardScanner.scan();
        if (board != prevBoard) {
            prevBoard = board;
            return board;
        }
    }
}
