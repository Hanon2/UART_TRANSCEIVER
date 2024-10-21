#include "appGPIO.h"
#include "appUart.h"
#include "timer.h"
#include "button.h"
#include <msp430.h>
int main(void)
{
    initGPIO();
    initUart();
    initTimerA();
    initClk();
    WDTCTL = WDTPW | WDTHOLD;
    __enable_interrupt();
    while (1)
    {
        runRxHandler();
        runTxHandler();
        runButtonLogic();
    }
}

