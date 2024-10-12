#include "timer.h"
#include "appUart.h"
#include <msp430.h>
#include <stdbool.h>

void initClk(void)
{
    TACTL = TASSEL_2 + ID_3 + MC_1; // SMCLK, divide by 8, Up mode
    TACCR0 = 56250 - 1;             // 1MHz/8 = 125000Hz, 0.45s = 56250 counts
    TACCTL0 = CCIE;                 // Enable interrupt for TACCR0
}
void initTimerA(void)
{
    BCSCTL1 = RSEL0 | RSEL1 | RSEL2;
    DCOCTL = DCO2;
    BCSCTL2 = (BCSCTL2 & ~SELS) | DIVM0;
}

// ISR for timer A
// This is responsible for toggling the pins of the LEDs.
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void)
{
    static bool ledOn = true;
    if (ledOn)
        P2OUT = getAssignedColor();
    else
        P2OUT = 0;
}



