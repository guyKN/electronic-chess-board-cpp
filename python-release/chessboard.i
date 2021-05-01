%module("threads"=1) chessboard
%{
    #include "../PythonInterface.h"
%}

%pythoncode %{
    import chess
%}
%nothread init;
extern void init();

extern unsigned long long scanBoard();
extern unsigned long long awaitBoardChange();

%nothread setLeds;
extern void setLeds(unsigned long long const_leds = 0,
                    unsigned long long slow_blink_leds = 0, unsigned long long slow_blink_leds_2 = 0,
                    unsigned long long fast_blink_leds = 0, unsigned long long fast_blink_leds_2 = 0,
                    bool reset_blink_timer = false);

%pythoncode %{

    def scanBoard():
        return chess.SquareSet(_chessboard.scanBoard())

    def awaitBoardChange():
        return chess.SquareSet(_chessboard.awaitBoardChange())

    def setLeds(const_leds=0, slow_blink_leds=0, slow_blink_leds_2=0, fast_blink_leds=0, fast_blink_leds_2=0, reset_blink_timer=False):
        return _chessboard.setLeds(int(const_leds), int(slow_blink_leds), int(slow_blink_leds_2), int(fast_blink_leds), int(fast_blink_leds_2), reset_blink_timer)
%}

%nothread testPins;
void testPins();

%nothread setSlowBlinkDuration;
void setSlowBlinkDuration(unsigned int blinkDuration);

%nothread setFastBlinkDuration;
void setFastBlinkDuration(unsigned int blinkDuration);

%nothread resetBlinkTimer;
extern void resetBlinkTimer();

%nothread setTempLeds;
extern void setTempLeds(unsigned long long leds, unsigned int duration);

%nothread clearTempLeds;
extern void clearTempLeds();

%nothread setLedRefreshRate;
void setLedRefreshRate(int refreshRate);

%nothread setUseEqualBrightness;
void setUseEqualBrightness(bool useEqualBrightness);

%nothread setEqualBrightnessFactor;
void setEqualBrightnessFactor(int equalBrightnessFactor);


%nothread cleanup;
extern void cleanup();