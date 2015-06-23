#ifndef _PWM_H_
#define _PWM_H_

#include<iocc2540.h>
#include "hal_types.h"
#include "hal_defs.h"
#include"led_show_task.h"

#define T3_STATR_COUNT() (T3CTL |= BV(4))  //启动计数
#define T3_STOP_COUNT()  (T3CTL &= (~BV(4)))
#define T3_OPEN_OVFIM()  (T3CTL |= BV(3))  //打开中断开关
#define T3_CLOSE_OUFIM() (T3CTL &= (~BV(3)))

typedef enum{RED = 1, GREEN, BLUE, WHITE} COLOUR;

extern uint16 u16Fre_G; //记录实时的频率值，并将数据缩小100倍，取值MIN_FREQUECY——MAX_FREQUECY
extern uint8 u8Duty_G;  //记录实时的占空比，并数据扩大100倍，取值在MIN_DUTY——MAX_DUTY
extern uint8 u8UpOrDownFlag_G;

void ini_pwm(void);
void set_luminance(uint8 u8Luminance);
void set_colour(void);

#endif
