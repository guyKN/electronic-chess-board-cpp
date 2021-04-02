%module chessboard
%{
    extern void init();
    extern unsigned long long scanBoard();
    extern void setLeds(unsigned long long blink1, unsigned long long blink2);
    extern void setBlinkDuration(unsigned int blinkDuration);
    extern void resetBlinkTimer();
    extern void clearTempLeds();
    extern void setTempLeds(unsigned long long leds, unsigned int duration);
    extern void cleanup();
%}
extern void init();
extern unsigned long long scanBoard();
extern void setLeds(unsigned long long blink1, unsigned long long blink2);
extern void setBlinkDuration(unsigned int blinkDuration);
extern void resetBlinkTimer();
extern void clearTempLeds();
extern void setTempLeds(unsigned long long leds, unsigned int duration);
extern void cleanup();
