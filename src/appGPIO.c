#include "appGPIO.h"
#include <stdint.h>
#define NUM_OF_PINS 6

typedef struct
{
    volatile uint8_t *dir;        // Pointer to the direction register
    volatile uint8_t *sel;       // Pointer to the first function select register
    volatile uint8_t *sel2;       // Pointer to the second function select register
    volatile uint8_t *ie;         // Interrupt enable register
    volatile uint8_t *ies;        // Interrupt edge select register
    volatile uint8_t *ifg;        // Interrupt flag register
    uint8_t pin;                  // Pin number
    uint8_t direction;            // Direction (1 for output, 0 for input)
    uint8_t functionSelect;      // Function select bit for SEL0
    uint8_t functionSelect2;      // Function select bit for SEL1
    uint8_t isrEnable;            // Interrupt enable
    uint8_t fallingOrRisingEdge;  // Edge select
    uint8_t isrFlag;              // Interrupt flag
} gpio_config_t;

gpio_config_t usedGpio[NUM_OF_PINS] =
{
    {&P2DIR, &P2SEL, &P2SEL2, &P2IE, &P1IES, &P1IFG, BIT1, 1, 0, 0, 0, 0, 0},
    {&P2DIR, &P2SEL, &P2SEL2, &P2IE, &P1IES, &P1IFG, BIT3, 1, 0, 0, 0, 0, 0},
    {&P2DIR, &P2SEL, &P2SEL2, &P2IE, &P1IES, &P1IFG, BIT5, 1, 0, 0, 0, 0, 0},
    {&P1DIR, &P1SEL, &P1SEL2, &P1IE, &P1IES, &P1IFG, BIT3, 0, 0, 0, 1, 1, 0},
    {&P1DIR, &P1SEL, &P1SEL2, &P1IE, &P1IES, &P1IFG, BIT1, 0, 1, 1, 0, 0, 0},
    {&P1DIR, &P1SEL, &P1SEL2, &P1IE, &P1IES, &P1IFG, BIT2, 0, 1, 1, 0, 0, 0}
};

void configurePin(volatile uint8_t *registerToBeConfigured, uint8_t pin, uint8_t state)
{
    if (state)
    {
        *registerToBeConfigured |= pin; // Set the corresponding bit
    }
    else
    {
        *registerToBeConfigured &= ~pin; // Clear the corresponding bit
    }
}

void initGPIO(void)
{
    int i;
    for ( i = 0; i < NUM_OF_PINS; ++i)
    {
        configurePin(usedGpio[i].dir, usedGpio[i].pin, usedGpio[i].direction); // Configure direction
        configurePin(usedGpio[i].sel, usedGpio[i].pin, usedGpio[i].functionSelect); // Configure SEL0
        configurePin(usedGpio[i].sel2, usedGpio[i].pin, usedGpio[i].functionSelect2); // Configure SEL1
        configurePin(usedGpio[i].ie, usedGpio[i].pin, usedGpio[i].isrEnable); // Configure interrupt enable
        configurePin(usedGpio[i].ifg, usedGpio[i].pin, usedGpio[i].isrFlag); // Configure interrupt flag
        configurePin(usedGpio[i].ies, usedGpio[i].pin, usedGpio[i].fallingOrRisingEdge); // Configure edge select
    }
}
