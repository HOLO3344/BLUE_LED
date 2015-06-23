#include<ioCC2540.h>
#include"uart.h"

void Uart_Init(void)
{    
  PERCFG = 0x01;        //位置2 P1口 
  P1SEL = 0x3c;        //P1_2,P1_3,P1_4,P1_5用作串口,第二功能 
 // P2DIR &= ~0XC0;      //P0 优先作为UART0 ，优先级
  
  U0CSR |= 0x80;       //UART 方式 
  
  //U0GCR与U0BAUD配合,波特率设为9600   
  U0BAUD |= 59;       //U0BAUD.BAUD_M = 216
  U0GCR |= 8;          //U0GCR.BAUD_E = 11;
  
  UTX0IF = 0;          //UART0 TX 中断标志初始置位1  （收发时候）
  U0CSR |= 0X40;       //允许接收 
  
  URX0IE = 1;
}

void Uart_Print(const uint8 *p,  const uint16 u16Len_C)
{
  uint16 u16Index;
  
  for(u16Index = 0; u16Index < u16Len_C; u16Index++)
  {
    U0DBUF = *p++;
    while(!UTX0IF)
    {
      ;
    }
    UTX0IF = 0;
  }
}
