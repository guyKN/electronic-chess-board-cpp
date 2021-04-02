%module chessboard
%{
    #include "../PythonInterface.h"
%}

%pythoncode %{
    import chess
%}

extern void init();

extern unsigned long long scanBoard();

extern void setLeds(unsigned long long const_leds = 0,
                    unsigned long long slow_blink_leds = 0, unsigned long long slow_blink_leds_2 = 0,
                    unsigned long long fast_blink_leds = 0, unsigned long long fast_blink_leds_2 = 0,
                    bool reset_blink_timer = false);

%pythoncode %{

    def scanBoard():
        return chess.SquareSet(_chessboard.scanBoard())

    def setLeds(const_leds=0, slow_blink_leds=0, slow_blink_leds_2=0, fast_blink_leds=0, fast_blink_leds_2=0, reset_blink_timer=False):
        return _chessboard.setLeds(int(const_leds), int(slow_blink_leds), int(slow_blink_leds_2), int(fast_blink_leds), int(fast_blink_leds_2), reset_blink_timer)
%}

void testPins();

void voltageRegisterWriteByte(unsigned char byte);
void voltageRegisterWriteBit(bool bit);

void setSlowBlinkDuration(unsigned int blinkDuration);
void setFastBlinkDuration(unsigned int blinkDuration);
extern void resetBlinkTimer();

extern void setTempLeds(unsigned long long leds, unsigned int duration);
extern void clearTempLeds();

extern void cleanup();