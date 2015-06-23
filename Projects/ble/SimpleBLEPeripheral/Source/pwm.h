#ifndef _PWM_H_
#define _PWM_H_

#include<iocc2540.h>
#include "hal_types.h"
#include "hal_defs.h"
#include"led_show_task.h"

#define T3_STATR_COUNT() (T3CTL |= BV(4))  //��������
#define T3_STOP_COUNT()  (T3CTL &= (~BV(4)))
#define T3_OPEN_OVFIM()  (T3CTL |= BV(3))  //���жϿ���
#define T3_CLOSE_OUFIM() (T3CTL &= (~BV(3)))

typedef enum{RED = 1, GREEN, BLUE, WHITE} COLOUR;

extern uint16 u16Fre_G; //��¼ʵʱ��Ƶ��ֵ������������С100����ȡֵMIN_FREQUECY����MAX_FREQUECY
extern uint8 u8Duty_G;  //��¼ʵʱ��ռ�ձȣ�����������100����ȡֵ��MIN_DUTY����MAX_DUTY
extern uint8 u8UpOrDownFlag_G;

void ini_pwm(void);
void set_luminance(uint8 u8Luminance);
void set_colour(void);

#endif
