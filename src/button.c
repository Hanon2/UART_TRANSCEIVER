#include "debounce.h"
#include "button.h"
#include "appUart.h"
#include <msp430.h>
#define BUTTONPRESSED   (!(P1IN & BIT3))

button_contents_t buttonToSwitchColors = {
    .debouncingTime    = 400,
    .timerCounter      = 0,
    .longPressTime     = 32767,
    .doublePressGap    = 0,
    .releaseCounter    = 0,
    .pressCount        = 0,
    .longPressDetected = false,
    .isNowPressed      = false
};

void runButtonLogic(void) {
    switch (handleButton(&buttonToSwitchColors,
                         BUTTONPRESSED))
    {
        case BS_PRESSED:
        case BS_LONG_PRESSED:
            setCanWeRunTransmit(true);
        break;
        default:
        break;
    }
}
