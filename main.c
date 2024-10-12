#include "appGPIO.h"
#include "appUart.h"
int main(void)
{
    initGPIO();
    initUart();
    while (1)
    {
        runRxHandler();
    }
}

