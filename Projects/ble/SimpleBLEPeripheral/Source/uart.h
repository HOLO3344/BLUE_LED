#ifndef _UART_H_
#define _UART_H_

#include "hal_types.h"

extern void Uart_Init(void);
extern void Uart_Print(const uint8 *p, const uint16 u16Len_C);

#endif

