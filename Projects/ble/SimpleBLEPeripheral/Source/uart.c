#include<ioCC2540.h>
#include"uart.h"

void Uart_Init(void)
{    
  PERCFG = 0x01;        //λ��2 P1�� 
  P1SEL = 0x3c;        //P1_2,P1_3,P1_4,P1_5��������,�ڶ����� 
 // P2DIR &= ~0XC0;      //P0 ������ΪUART0 �����ȼ�
  
  U0CSR |= 0x80;       //UART ��ʽ 
  
  //U0GCR��U0BAUD���,��������Ϊ9600   
  U0BAUD |= 59;       //U0BAUD.BAUD_M = 216
  U0GCR |= 8;          //U0GCR.BAUD_E = 11;
  
  UTX0IF = 0;          //UART0 TX �жϱ�־��ʼ��λ1  ���շ�ʱ��
  U0CSR |= 0X40;       //������� 
  
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
