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
    const ShiftInRegister& shiftInRegister;
    const int* outPins;
public:
    BoardScanner(const BoardScanner&) = delete;
    BoardScanner(const BoardScanner&&) = delete;
    BoardScanner& operator=(const BoardScanner&) = delete;
    BoardScanner& operator=(const BoardScanner&&) = delete;

    explicit BoardScanner(const ShiftInRegister &shiftInRegister, const int *outPins);
    explicit BoardScanner(const ShiftInRegister & shiftInRegister);
    void init() const;
    uint64_t scan() const;

    void testPins(void test(int)) const;

    [[noreturn]] void scanLoop(LedThreadManager *ledThread) const;

    void cleanup();
};

class BoardChangeDetector{
    const BoardScanner& boardScanner;
    bool hasStarted = false;
    uint64_t prevBoard = 0;
public:
    BoardChangeDetector(const BoardChangeDetector&) = delete;
    BoardChangeDetector(const BoardChangeDetector&&) = delete;
    BoardChangeDetector& operator=(const BoardChangeDetector&) = delete;
    BoardChangeDetector& operator=(const BoardChangeDetector&&) = delete;

    explicit BoardChangeDetector(const BoardScanner &boardScanner);
    uint64_t awaitBoardChange();
};

class LedController{
public:
    const ShiftOutRegister& voltageRegister;
    const ShiftOutRegister& groundRegister;

    const int patchPin;
    const int patchIndex;

private:
    int microDelayBetweenRows = 1000;
    int numTimesMissedRefreshRate = 0;
    void onMissRefreshRate();
public:
    LedController(const LedController&) = delete;
    LedController(const LedController&&) = delete;
    LedController& operator=(const LedController&) = delete;
    LedController& operator=(const LedController&&) = delete;

    LedController(const ShiftOutRegister &voltageRegister, const ShiftOutRegister &groundRegister, int patchPin,
                  int patchIndex);

    void init();
    void reset();
    void setRefreshRate(int refreshRate);
    int countRefreshRateMisses();
    void cleanup() const;
    void setLeds(uint64_t board);
    [[noreturn]] void ledLoop();
};

#endif //CHESSBOARD_ELECTRONICS_H
