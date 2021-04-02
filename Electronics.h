//
// Created by guykn on 3/2/2021.
//

#ifndef CHESSBOARD_ELECTRONICS_H
#define CHESSBOARD_ELECTRONICS_H


#include <cstdint>

class LedThreadManager;

class ShiftInRegister {
public:
    const int clockPin;
    const int serialInputPin;
    const int enableSerialPin;
private:
    static void printData(uint8_t data);
public:
    ShiftInRegister(int clockPin, int serialInput, int enableSerial);
    void init();
    static ShiftInRegister scanGroundRegister();
    uint8_t read() const;
    void  testPins(void test(int));
    void cleanup() const;
    [[noreturn]] void readLoop() const;
};

class ShiftOutRegister {
public:
    const int dataClock;
    const int storageClock;
    const int dataOutput;
    const int masterReset;

    void updateStorage() const;

public:
    ShiftOutRegister(int masterReset, int dataClock, int storageClock, int dataOutput);
    static ShiftOutRegister ledGroundRegister();
    static ShiftOutRegister ledVoltageRegister();
    void init() const;
    void reset() const;
    void writeBit(bool bit) const;
    void writeByte(uint8_t byte) const;
    void testPins(void test(int)) const;
    void cleanup() const;

};
class LedController;
class BoardScanner{
public:
    const ShiftInRegister shiftInRegister;
    const int* outPins;
public:
    explicit BoardScanner(const ShiftInRegister &shiftInRegister, const int *outPins);
    explicit BoardScanner(const ShiftInRegister & shiftInRegister);
    void init() const;
    uint64_t scan() const;

    void testPins(void test(int)) const;

    [[noreturn]] void scanLoop(LedThreadManager *ledThread) const;

    void cleanup();
};

class LedController{
public:
    const ShiftOutRegister voltageRegister;
    const ShiftOutRegister groundRegister;

    const int patchPin;
    const int patchIndex;
public:
    LedController(const ShiftOutRegister &voltageRegister, const ShiftOutRegister &groundRegister, int patchPin,
                  int patchIndex);

    void init();

    void reset();
    void cleanup();
    [[noreturn]] void ledLoop() const;
    void setLeds(uint64_t board) const;
};

#endif //CHESSBOARD_ELECTRONICS_H
