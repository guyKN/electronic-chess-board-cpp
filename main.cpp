#include <iostream>
#include <wiringPi.h>
#include "Electronics.h"
#include "LedThreadManager.h"
#include "PythonInterface.h"

// todo: squares to replace: b5, h5, b6

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


int main() {

    initGpio();
    uint64_t prevBoard = readBoard();
    writeLeds(prevBoard, prevBoard);
    for (;;) {
        uint64_t board = readBoard();
        if (board != prevBoard) {
            prevBoard = board;
            printBoard(board);
            writeLeds(board, board);
        }
    }
}
// board   : 37, 35, 33, 31, 29, 27, 23, 21
// wiringPi: 25, 24, 23, 22, 21, 30, 14, 13

