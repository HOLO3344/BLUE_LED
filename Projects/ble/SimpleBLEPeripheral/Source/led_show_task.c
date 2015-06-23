#include"led_show_task.h"
#include "OSAL_Timers.h"
#include "pwm.h"
#include "simpleBLEPeripheral.h"

#define MIN_DELAY_MS 500  //定义最小延时，单位ms
#define TASK_MIN_DELAY_MS 1000

typedef struct
{
  const TASK_COLOUR *pHead;
  uint8 u8Size;
}INSIDE_TASK_TAB;

XDATA UUID9_REVIE UUID9Recive;

static uint8 u8ColourNum_S = 0;//标示实际状态个数
static uint8 u8ColourTabIndex = 0; //标志当前运行任务表下标

XDATA uint8 UserTaskColourTabSize = 0;  //记录用户自定义颜色模式个数
XDATA tempTASK_COLOUR UserTaskColourTab[MAX_USER_COLOUR_NUM];

const TASK_COLOUR *pColourNow = NULL;//指示当前点亮状态
const TASK_COLOUR *pCurTaskColourTab = NULL;
TASK_COLOUR *pUserTaskHead = NULL;

const TASK_COLOUR modelColourTab1[7] = \
{
  {3000,0xFF,0x03,0x02},
  {3000,0x01,0xFF,0x02},
  {3000,0x02,0x03,0xFF},
  {3000,0xFF,0xEC,0x01},
  {3000,0X00,0xFC,0xFB},
  {3000,0xA9,0x00,0xFC},
  {3000,0xFF,0xFF,0xFF},
};//七色渐变

const TASK_COLOUR modelColourTab2[2] = \
{
  {3000,0xFF,0x03,0x02},
  {3000, 0x00,0x00,0x00},
};//红色渐变

const TASK_COLOUR modelColourTab3[2] = \
{
  {1500,0x01,0xFF,0x02},
  {1500,0x00,0x00,0x00},
};//绿色渐变

const TASK_COLOUR modelColourTab4[2] =\
{
  {1500,0x02,0x03,0xFF},
  {1500,0x00,0x00,0x00},
};//蓝色渐变

const TASK_COLOUR modelColourTab5[2] =\
{
  {1500,0xFF,0xEC,0x01},
  {1500,0x00,0x00,0x00},
};//黄色渐变

const TASK_COLOUR modelColourTab6[2] =\
{
  {1500,0X00,0xFC,0xFB},
  {1500,0x00,0x00,0x00},
};//青色渐变

const TASK_COLOUR modelColourTab7[2] =\
{
  {3000,0xFF,0x03,0x02},
  {1500,0x00,0x00,0x00},
};//紫色渐变

const TASK_COLOUR modelColourTab8[2] =\
{
  {1500,0xFF,0xFF,0xFF},
  {1500,0x00,0x00,0x00},
};//白色渐变

const TASK_COLOUR modelColourTab9[2] =\
{
  {1500,0xFF,0x03,0x02},
  {1500,0x01,0xFF,0x02},
};//红绿渐变

const TASK_COLOUR modelColourTab10[2] =\
{
  {1500,0xFF,0x03,0x02},
  {1500,0x02,0x03,0xFF},
};//红蓝渐变

const TASK_COLOUR modelColourTab11[2] =\
{
  {1500,0x01,0xFF,0x02},
  {1500,0x02,0x03,0xFF},
};//绿蓝渐变

const TASK_COLOUR modelColourTab12[7] =\
{
  {5000,0xFF,0x03,0x02},
  {5000,0x01,0xFF,0x02},
  {5000,0x02,0x03,0xFF},
  {5000,0xFF,0xEC,0x01},
  {5000,0X00,0xFC,0xFB},
  {5000,0xA9,0x00,0xFC},
  {5000,0xFF,0xFF,0xFF}
};//七色频闪

const TASK_COLOUR modelColourTab13[2] =\
{
  {2500,0xFF,0x03,0x02},
  {2500,0x00,0x00,0x00},
};//红色频闪

const TASK_COLOUR modelColourTab14[2] =\
{
  {2500,0x01,0xFF,0x02},
  {2500,0x00,0x00,0x00},
};//绿色频闪

const TASK_COLOUR modelColourTab15[2] =\
{
  {2500,0x02,0x03,0xFF},
  {2500,0x00,0x00,0x00},
};//蓝色频闪

const TASK_COLOUR modelColourTab16[2] =\
{
  {2500,0xFF,0xEC,0x01},
  {2500,0x00,0x00,0x00},
};//黄色频闪

const TASK_COLOUR modelColourTab17[2] =\
{
  {2500,0X00,0xFC,0xFB},
  {2500,0x00,0x00,0x00},
};//青色频闪

const TASK_COLOUR modelColourTab18[2] =\
{
  {2500,0xFF,0x03,0x02},
  {2500,0x00,0x00,0x00},
};//紫色频闪

const TASK_COLOUR modelColourTab19[2] =\
{
  {2500,0xFF,0xFF,0xFF},
  {2500,0x00,0x00,0x00},
};//白色频闪

const TASK_COLOUR modelColourTab20[7] =\
{
  {5000,0xFF,0x00,0x00},
  {5000,0x00,0xFF,0x00},
  {5000,0x00,0x00,0xFF},
  {5000,0xFF,0xEC,0x00},
  {5000,0X00,0xFC,0xFB},
  {5000,0xA9,0x00,0xFC},
  {5000,0xFF,0xFF,0xFF},
};//七色跳变


const INSIDE_TASK_TAB InsideTaskTab[20] = \
{
  {modelColourTab1,7},
  {modelColourTab2,2},
  {modelColourTab3,2},
  {modelColourTab4,2},
  {modelColourTab5,2},
  {modelColourTab6,2},
  {modelColourTab7,2},
  {modelColourTab8,2},
  {modelColourTab9,2},
  {modelColourTab10,2},
  {modelColourTab11,2},
  {modelColourTab12,7},
  {modelColourTab13,2},
  {modelColourTab14,2},
  {modelColourTab15,2},
  {modelColourTab16,2},
  {modelColourTab17,2},
  {modelColourTab18,2},
  {modelColourTab19,2},
  {modelColourTab20,7},
};


void get_task_colour_def(uint8 u8Index)//将接收到的状态信息填入到任务数组中
{  
  pCurTaskColourTab = InsideTaskTab[u8Index].pHead;
  u8ColourNum_S = InsideTaskTab[u8Index].u8Size;
  u8ColourTabIndex = 0;
}

void switch_colour(void)
{  
  pColourNow = &pCurTaskColourTab[u8ColourTabIndex];
  set_luminance(1);
  u8UpOrDownFlag_G = 0;  //增加
  set_colour();
  osal_start_timerEx(GetSimpleBLEPeripheral_TaskID(), SBP_SET_START_T3_EVT, pColourNow->u32DelayTimes - TASK_MIN_DELAY_MS);
  osal_start_timerEx(GetSimpleBLEPeripheral_TaskID(), SBP_SET_PWM_EVT, pColourNow->u32DelayTimes);
  osal_start_timerEx(GetSimpleBLEPeripheral_TaskID(), SBP_SET_STOP_T3_EVT, pColourNow->u32DelayTimes + TASK_MIN_DELAY_MS);
  
  u8ColourTabIndex++;
  u8ColourTabIndex %= u8ColourNum_S;
}

void get_task_colour_user(void)
{
  uint8 u8TabSize = 0;
  uint8 u8Temp;
  uint8 u8Index;
  uint8 u8I;
  uint8 u8J;
  uint8 u8M;
  uint8 u8N;
  
  u8TabSize = UserTaskColourTabSize;
  
  for(u8I = 0; u8I < 5; u8I++)
  {
    if(0 == UUID9Recive.u8Circle[2 * u8I])
    {
      break;
    }
    u8Temp = UUID9Recive.u8Circle[2 * u8I] & 0x0f;
    u8Temp = u8Temp - (UUID9Recive.u8Circle[2 * u8I] >> 4);
    u8Temp++;
    u8Temp *= (UUID9Recive.u8Circle[2 * u8I + 1] - 1);
    u8TabSize += u8Temp;
  }
  u8ColourNum_S = u8TabSize;
  
  if(pUserTaskHead != NULL)
  {
    osal_mem_free(pUserTaskHead);
  }
  pUserTaskHead = (TASK_COLOUR *)osal_mem_alloc( sizeof( TASK_COLOUR ) * u8TabSize);
  
  u8Index = 0;
  u8N = 0;
  for(u8I = 0; u8I < UserTaskColourTabSize; u8I++)
  {   
    u8Temp = UUID9Recive.u8Circle[2*u8N] >> 4;
    if(u8I != u8Temp || UUID9Recive.u8Circle[2*u8N] == 0)
    {
      pUserTaskHead[u8Index].u32DelayTimes = UserTaskColourTab[u8I].u8DelayTimes * MIN_DELAY_MS;  //单位ms
      pUserTaskHead[u8Index].u8Red = UserTaskColourTab[u8I].u8Red;
      pUserTaskHead[u8Index].u8Green = UserTaskColourTab[u8I].u8Green;
      pUserTaskHead[u8Index++].u8Blue = UserTaskColourTab[u8I].u8Blue;
    }
    else if(UUID9Recive.u8Circle[2*u8N] != 0)
    {
      for(u8J = 0; u8J < UUID9Recive.u8Circle[2*u8N + 1]; u8J++)
      {
        for(u8M = u8Temp; u8M <= (UUID9Recive.u8Circle[2*u8N] & 0x0f); u8M++)
        {          
          pUserTaskHead[u8Index].u32DelayTimes = UserTaskColourTab[u8M].u8DelayTimes * MIN_DELAY_MS;  //单位ms
          pUserTaskHead[u8Index].u8Red = UserTaskColourTab[u8M].u8Red;
          pUserTaskHead[u8Index].u8Green = UserTaskColourTab[u8M].u8Green;
          pUserTaskHead[u8Index++].u8Blue = UserTaskColourTab[u8M].u8Blue;
        }
      }
      u8I = (UUID9Recive.u8Circle[2*u8N] & 0x0f);        
      u8N += 1;
    }  
  }
    
  pCurTaskColourTab = pUserTaskHead;
  u8ColourTabIndex = 0;  //置位下标
} 

void start_t3_task(void)
{
  set_luminance(100);
  u8UpOrDownFlag_G = 1;  //减小
  T3_STATR_COUNT();
  T3_OPEN_OVFIM();
}

void stop_t3_task(void)
{
  T3_STOP_COUNT();
  T3_CLOSE_OUFIM();
}