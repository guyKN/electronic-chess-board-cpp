#!/bin/bash
set -e
swig -python -c++ chessboard.i
g++ -O3 -c ../Electronics.cpp ../PythonInterface.cpp ../LedThreadManager.cpp chessboard_wrap.cxx -I /usr/include/python3.7/
g++ -shared Electronics.o LedThreadManager.o PythonInterface.o chessboard_wrap.o -I/usr/local/include -L/usr/local/lib -lwiringPi -o _chessboard.so
echo done