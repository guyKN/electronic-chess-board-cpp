# Electronic Chess Board - Cpp code
The c plus plus code that directly interacts with hardware and controls the electronic chessboard. 

A simple demo of the chessboard may be found here: https://youtu.be/uFnUs-OOC5I

## How to build
Wiring Pi is the library used by this project, and it should come pre-installed on the Raspbian OS. But if it is not installed, you can download it using

``sudo apt-get install wiringpi``

Then compile using:

`bash python-release/compile.sh`

This will generate the `_chessboard.so` shared library and the `chessboard.py` python library that allow python code to control the hardware. 