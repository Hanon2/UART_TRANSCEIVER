#ifndef APPUART_H
#define APPUART_H
#include <stdbool.h>
#include <stdint.h>

void initUart(void);
void setCanWeRunTransmit(bool condition);
void runRxHandler(void);
void runTxHandler(void);
uint8_t getAssignedColor(void);

#endif
