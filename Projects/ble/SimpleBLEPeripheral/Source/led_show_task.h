#ifndef _LED_SHOW_TASK_H_
#define _LED_SHOW_TASK_H_

#include "hal_types.h"
#include "osal.h"

#define MAX_USER_COLOUR_NUM 16

typedef struct task_colour_s
{
  uint32 u32DelayTimes;  //��λms
  uint8 u8Red;
  uint8 u8Green;
  uint8 u8Blue; 
}TASK_COLOUR;

typedef struct
{
  uint8 u8DelayTimes;  //��λms
  uint8 u8Red;
  uint8 u8Green;
  uint8 u8Blue; 
}tempTASK_COLOUR;//�����ֻ����������Զ���RGBֵ�ͳ���ʱ��

typedef struct 
{
  uint8 u8Resive;
  uint8 u8Num;
  uint8 u8Model;
  uint8 u8Circle[10];
}UUID9_REVIE;

extern const TASK_COLOUR *pColourNow;//ָʾ��ǰ����״̬
extern uint8 UserTaskColourTabSize;  //��¼�û��Զ�����ɫģʽ����
extern tempTASK_COLOUR UserTaskColourTab[MAX_USER_COLOUR_NUM];
extern UUID9_REVIE UUID9Recive;

void extern get_task_colour_user(void);
void extern get_task_colour_def(uint8 u8Index);
void extern switch_colour(void);
void start_t3_task(void);
void stop_t3_task(void);

#endif