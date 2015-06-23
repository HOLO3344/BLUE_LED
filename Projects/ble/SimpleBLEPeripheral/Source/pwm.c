#include"pwm.h"
////////////////////////////////

#define OUT_FREQUENCY       123  //单位为：百Hz 如果该值小于FRE_STEP时可能会出现递减时数据突变为很大的值

//定义PWM通道
#define PWM_CHANNAL_0        BV(2)
#define PWM_CHANNAL_1        BV(3)
#define PWM_CHANNAL_2        BV(4)
#define PWM_CHANNAL_3        BV(5)
#define PWM_CHANNAL_4        BV(6)
#define PWM_USE_CHANNAL      (PWM_CHANNAL_1 | PWM_CHANNAL_2 | PWM_CHANNAL_3 | PWM_CHANNAL_4) //PWM输出通道定义

#define MAIN_CLOCK          32000000 //定义主时钟频率单位Hz
//宏定义各种分频系数
#define FRE_DIV_1           0x00
#define FRE_DIV_8           BV(2)
#define FRE_DIV_32          BV(3)
#define FRE_DIV_128         (BV(3) | BV(2))
#define USE_FRE_DIV         FRE_DIV_32   //实际分频数定义

//宏定义各种计数模式
#define MODULO_MODE         BV(1)

//宏定义各种比较模式
#define OUT_MODE 0x30

//定义捕获或者比较模式
#define COM_MODE      BV(2)

//定义向T1CC0中装载的数据
#if (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_1)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/1/OUT_FREQUENCY) //装载数据 = (主频 / 分频 / 输出频率)
#elif (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_8)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/8/OUT_FREQUENCY) //装载数据 = (主频 / 分频 / 输出频率)
#elif (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_32)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/32/OUT_FREQUENCY) //装载数据 = (主频 / 分频 / 输出频率)
#elif (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_128)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/128/OUT_FREQUENCY) //装载数据 = (主频 / 分频 / 输出频率)
#endif

#define T3_SET_FRE_DIV()   (T3CTL |= (BV(7)|BV(6)|BV(5))) //128分频
#define T3_USE_MODULE_COUNT() (T3CTL |= BV(1))//使用模计数
#define PWM_ON()  (T1CTL |= 0x02)
#define PWM_OFF() (T1CTL &= ~0x02)

#define LUMINANCE_STEP 4
#define INT_T3_TIMES_MS 50
typedef union
{
  uint16 u16Counters;
  uint8 u8Counters[2]; //小端地址模式u8Counters[1]为高8位
}CC_TYPE;

CC_TYPE load_T1CC0_value;//装载到T1CC0寄存器的值，决定PWM波周期
CC_TYPE load_T1CCx_value;//装载到T1CC1寄存器的值，决定PWM波的占空比

static uint8 u8Luminance_S = 100; //亮度百分数，取值0到100
uint8 u8UpOrDownFlag_G = 1;

static void InitTimer3(void);
//*********************************************
//
//函数名称：ini_pwm
//
//输入参数：void
//
//输出参数：void
//
//作者    ：2014-9-13, by HOLO
//*********************************************
void ini_pwm(void)
{
  load_T1CC0_value.u16Counters = (uint16)DOWN_LOAD_VALUE;
  load_T1CCx_value.u16Counters = (uint16)(load_T1CC0_value.u16Counters);
//  
//  load_T1CC0_value.u16Counters = 0xff;
//  load_T1CC0_value.u16Counters <<= 5;
//  load_T1CCx_value.u16Counters = load_T1CC0_value.u16Counters;
//  load_T1CC0_value.u16Counters += 100;
  
  //设置pwm端口为输出
  P0DIR |= PWM_USE_CHANNAL;
  //设置pwm端口为外设端口，非gpio
  P0SEL |= PWM_USE_CHANNAL;
  
  PERCFG |= 0X03; //将uart0和uart1移动到备用IO端口
  
  T1CNTL = 0; // Reset timer to 0;
  T1CNTH = 0; // Reset timer to 0;对 T1CNTL 寄存器的所有写入访问将复位 16 位计数器
  
  T1CCTL1 = OUT_MODE + COM_MODE; //Set when equal T1CC0, clear when equal T1CC1; Mode = Compare
  T1CCTL2 = OUT_MODE + COM_MODE; //Set when equal T1CC0, clear when equal T1CC1; Mode = Compare
  T1CCTL3 = OUT_MODE + COM_MODE; //Set when equal T1CC0, clear when equal T1CC1; Mode = Compare
  T1CCTL4 = OUT_MODE + COM_MODE; //Set when equal T1CC0, clear when equal T1CC1; Mode = Compare
  
  T1CC0H = load_T1CC0_value.u8Counters[1]; // 
  T1CC0L = load_T1CC0_value.u8Counters[0]; // 
  
  T1CC1H = load_T1CCx_value.u8Counters[1]; //
  T1CC1L = load_T1CCx_value.u8Counters[0];
  
  
  T1CC2H = load_T1CCx_value.u8Counters[1]; // 
  T1CC2L = load_T1CCx_value.u8Counters[0];
  
  T1CC3H = load_T1CCx_value.u8Counters[1]; 
  T1CC3L = load_T1CCx_value.u8Counters[0];
  
  T1CC4H = load_T1CCx_value.u8Counters[1]; //
  T1CC4L = load_T1CCx_value.u8Counters[0];
  
  T1CTL = USE_FRE_DIV + MODULO_MODE; // Div = USE_FRE_DIV,  MODE = module，
                                    //当一个不是 00值的写入到 T1CTL.MODE 计数器开始工作  
  InitTimer3();
}

void set_luminance(uint8 u8Luminance)
{
  u8Luminance_S = u8Luminance;
}

void set_colour(void)
{
  uint32 u32Temp;
  
  if(pColourNow == NULL)
  {
    return;
  }    
  
  CC_TYPE load_T1CC4_value;  
  u32Temp = pColourNow->u8Blue;
  u32Temp <<= 5;
  u32Temp = u32Temp * u8Luminance_S;
  u32Temp /= 100;
  load_T1CC4_value.u16Counters = (uint16)u32Temp;   //Blue
  
  CC_TYPE load_T1CC2_value;  
  u32Temp = pColourNow->u8Green;
  u32Temp <<= 5;
  u32Temp = u32Temp * u8Luminance_S;
  u32Temp /= 100;
  load_T1CC2_value.u16Counters = (uint16)u32Temp; //GREEN
  
  CC_TYPE load_T1CC3_value;
  u32Temp = pColourNow->u8Red;
  u32Temp <<= 5;
  u32Temp = u32Temp * u8Luminance_S;
  u32Temp /= 100;
  load_T1CC3_value.u16Counters = (uint16)u32Temp; //RED
  
  PWM_OFF();
 //case RED:
  T1CC3H = load_T1CC3_value.u8Counters[1]; // 
  T1CC3L = load_T1CC3_value.u8Counters[0];
  
//case GREEN:
  T1CC2H = load_T1CC2_value.u8Counters[1]; // 
  T1CC2L = load_T1CC2_value.u8Counters[0];
  
//case BLUE:
  T1CC4H = load_T1CC4_value.u8Counters[1]; // 
  T1CC4L = load_T1CC4_value.u8Counters[0];
  PWM_ON();
}

void InitTimer3(void)
{
  T3CNT = 0; //计数器清零
  IEN1 |= BV(3);      //开定时器3中断
  IEN0 |= BV(7);      //开总中断
  T3CC0 = 250;         //主时钟32Mhz经过128分频，计数250之后恰好1ms中断一次
  T3CCTL0 |= 0X44;    //将定时器3通道0设定为开启中断，且为比较模式
  T3_SET_FRE_DIV();      //设定分频
  T3_USE_MODULE_COUNT(); //设定为模模式
  T3_STATR_COUNT();      //启动计数器
}


//定时器 T3 中断处理函数 
#pragma vector = T3_VECTOR    
__interrupt void Timer3_ISR(void)
{
  static uint8 u8Index_S = 0;
  
  IEN0 &= ~BV(7);  //关总中断
  u8Index_S++;
  
  if(u8Index_S == INT_T3_TIMES_MS)
  {
    u8Index_S = 0;
    
    if(0 == u8UpOrDownFlag_G)
    {      
      if(u8Luminance_S < 100)
      {
        u8Luminance_S += LUMINANCE_STEP;
      }
    }
    else
    {
      if(u8Luminance_S > 2)
      {
        u8Luminance_S -= LUMINANCE_STEP;
      }
    }
    set_colour();
  }
  IEN0 |= BV(7);  //开总中断
}

