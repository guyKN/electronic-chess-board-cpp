//
// Created by guykn on 3/2/2021.
//

#ifndef CHESSBOARD_ELECTRONICS_H
#define CHESSBOARD_ELECTRONICS_H


#include <cstdint>

class LedThread;

class ShiftInRegister {
public:
    const int clockPin;
    const int serialInputPin;
    const int enableSerialPin;
private:
    static void printData(uint8_t data);
public:
    ShiftInRegister(int clockPin, int serialInput, int enableSerial);
    uint8_t read() const;
    [[noreturn]] void readLoop() const;
};

class ShiftOutRegister {
public:
    const int dataClock;
    const int storageClock;
    const int dataOutput;
    const int masterReset;
private:
    void updateStorage() const;

public:
    ShiftOutRegister(int masterReset, int dataClock, int storageClock, int dataOutput);
    void reset() const;
    void writeBit(bool bit) const;
    void writeByte(uint8_t byte) const;

};
class LedController;
class BoardScanner{
public:
    const ShiftInRegister shiftInRegister;
    const int* outPins;
private:
    static uint64_t standardizeBoard(uint64_t board);

public:
    BoardScanner(const ShiftInRegister &shiftInRegister, const int *outPins);
    uint64_t scan();

    [[noreturn]] void scanLoop(LedThread *ledThread);
};

class LedController{
public:
    const ShiftOutRegister voltageRegister;
    const ShiftOutRegister groundRegister;
private:
    static uint64_t standardizeBoard(uint64_t board);
public:
    LedController(const ShiftOutRegister &voltageRegister, const ShiftOutRegister &groundRegister);
    void reset();

    [[noreturn]] void ledLoop() const;
    void setLeds(uint64_t board) const;
};

#endif //CHESSBOARD_ELECTRONICS_H
