#ifndef __DRV_UART_H_
#define __DRV_UART_H_
#include "main.h"

void SetToSendMode(const void *pData,unsigned long DataLength);
void ReceiveStart(void);
void SetToReceiveMode(const void *pData, unsigned long DataLength );
#endif
