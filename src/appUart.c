#include "queue.h"
#include "appUart.h"
#include <msp430.h>
#include <stdbool.h>
#include <string.h>


#define MESSAGE_CAPACITY    (8)
#define RX_IDX              (0)
#define TX_IDX              (1)
#define NUM_OF_QUEUES       (2)
#define NUM_OF_COLORS        (sizeof(colors) / sizeof(colors[0]))

//Just place holders for the queue buffer don't mess with them.
static uint8_t uartTXBuff [MESSAGE_CAPACITY];
static uint8_t uartRXBuff [MESSAGE_CAPACITY];
enum
{
    red     = 0x02,
    green   = 0x08,
    blue    = 0x20,
    yellow  = 0x0A,
    magenta = 0x28,
    cyan    = 0x22,
    white   = 0x2A
};
uint8_t colorTable[] = {red, green, blue, yellow, magenta, cyan, white};
static char* colors[] = {"red\0", "green\0", "blue\0", "yellow\0", "magenta\0", "cyan\0", "white\0"};

static bool canWeRunRecieval, canWeRunTransmit = false;
static uint8_t assignedColor;
static uint8_t colorToBeTransmittedIdx = 0;


// Initialize queues
static fifo_queue_t queues[NUM_OF_QUEUES] =
{
    [RX_IDX] =
    {
        .buffer     = uartRXBuff,
        .capacity   = MESSAGE_CAPACITY,
        .head       = 0,
        .tail       = 0,
        .isFull     = false
    },
    [TX_IDX] =
    {
        .buffer     = uartTXBuff,
        .capacity   = MESSAGE_CAPACITY,
        .head       = 0,
        .tail       = 0,
        .isFull     = false
    }
};

void initUart(void)
{
    UCA0CTL1 |= UCSWRST; // Hold it in reset
    UCA0CTL0 = 0; //Async Uart mode, with one stop bit no parity
    UCA0CTL1 |= UCSSEL_2; // Use SMclk which is the same one we are using for time A.
    /*
     * N = clkFreq/Baud rate
     * UCBRx = INT(N)
     * UCBRSx = round( ( N – INT(N) ) × 8 )
     */
    UCA0BR0 = 104; // INT(1000000/9600) = 104
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1; //round( (104.167 – INT(104.167) ) × 8 )
    UCA0CTL1 &= ~UCSWRST; //Remove it from reset state
    IE2 |= UCA0RXIE; // Enable ISR
}
void runRxHandler(void)
{
    if (canWeRunRecieval)
    {
        char recievedData[MESSAGE_CAPACITY];
        {
            int i = 0;
            char characterToBeDequeued='x';
            while ((characterToBeDequeued!='\0') && i<MESSAGE_CAPACITY)
            {
                (void)dequeue(&queues[RX_IDX], (uint8_t*)characterToBeDequeued);
                recievedData[i] = characterToBeDequeued;
                ++i;
            }
        }
        {
            int i;
            for (i =0; i<MESSAGE_CAPACITY; ++i)
            {
                if (strcmp(recievedData, colors[i]) == 0)
                {
                    assignedColor = colorTable[i];
                    break;
                }
            }
        }
        canWeRunRecieval = false;
    }
}

void runTxHandler(void)
{
    if (canWeRunTransmit)
    {
        uint8_t idxForCharacterToBeSent = 0;
        do
        {
            (void)enqueue(&queues[TX_IDX], colors[colorToBeTransmittedIdx][idxForCharacterToBeSent]);
        }while (colors[colorToBeTransmittedIdx][idxForCharacterToBeSent++] != '\0');
        (void)enqueue(&queues[TX_IDX], colors[colorToBeTransmittedIdx][idxForCharacterToBeSent]);
    }
    canWeRunTransmit = false;
}
void setCanWeRunTransmit(bool condition)
{
    canWeRunTransmit = condition;
}
uint8_t getAssignedColor(void)
{
    return assignedColor;
}
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    char x = UCA0RXBUF;
    if (!canWeRunRecieval) //Make sure that we have parsed the message first so that we avoid overwriting
        (void)enqueue(&queues[RX_IDX], (uint8_t)x);
    if ('\0' == x)
        canWeRunRecieval = true;
}


#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
    char dataToBeDequed;
    (void)dequeue(&queues[TX_IDX], (uint8_t*)&dataToBeDequed);
     UCA0TXBUF  = dataToBeDequed;
     if (dataToBeDequed == '\0')
     {
         IE2 &= ~UCA0TXIE;
         colorToBeTransmittedIdx = (colorToBeTransmittedIdx + 1) & NUM_OF_COLORS;
     }
}
