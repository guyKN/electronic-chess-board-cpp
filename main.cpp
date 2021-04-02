#include <iostream>
#include <wiringPi.h>
#include "Electronics.h"
#include "LedThreadManager.h"
#include "PythonInterface.h"

// todo: squares to replace: b5, h5, b6
//todo: rename from "chessboard" to a better name
namespace {
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


[[noreturn]]int main() {

    init();
    uint64_t prevBoard = scanBoard();
    setLeds(0xFF, 0xFF);
    for (;;) {
        uint64_t board = scanBoard();
        if (board != prevBoard) {
            uint64_t difference = board ^ prevBoard;
            setTempLeds(difference, 250);
            prevBoard = board;
        }
    }
}
// board   : 37, 35, 33, 31, 29, 27, 23, 21
// wiringPi: 25, 24, 23, 22, 21, 30, 14, 13

