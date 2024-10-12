#include"appUart.h"
#include "queue.h"
#include <msp430.h>
#include <stdbool.h>
#include <string.h>


#define MESSAGE_CAPACITY (8)
#define RX_IDX           (0)
#define TX_IDX           (1)
#define NUM_OF_QUEUES    (2)

uint8_t uartTXBuff [MESSAGE_CAPACITY];
uint8_t uartRXBuff [MESSAGE_CAPACITY];

static bool canWeRunRecieval;
static char* colors[] = {"red\0", "green\0", "blue\0", "yellow\0", "magenta\0", "cyan\0", "white\0"};
static char assignedColor[];

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
    UCA0CTL1 |= UCSSEL_2; // Use SMclk
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
                    strcpy(assignedColor, colors[i]);
                    break;
                }
            }
        }
        canWeRunRecieval = false;
    }
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

}

void setTxISR(bool condition)
{
    if (condition)
        IE2 |= UCA0TXIE;
    else
        IE2 &= ~UCA0TXIE;
}

