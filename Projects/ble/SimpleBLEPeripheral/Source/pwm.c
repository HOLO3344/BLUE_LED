#include"pwm.h"
////////////////////////////////

#define OUT_FREQUENCY       123  //��λΪ����Hz �����ֵС��FRE_STEPʱ���ܻ���ֵݼ�ʱ����ͻ��Ϊ�ܴ��ֵ

//����PWMͨ��
#define PWM_CHANNAL_0        BV(2)
#define PWM_CHANNAL_1        BV(3)
#define PWM_CHANNAL_2        BV(4)
#define PWM_CHANNAL_3        BV(5)
#define PWM_CHANNAL_4        BV(6)
#define PWM_USE_CHANNAL      (PWM_CHANNAL_1 | PWM_CHANNAL_2 | PWM_CHANNAL_3 | PWM_CHANNAL_4) //PWM���ͨ������

#define MAIN_CLOCK          32000000 //������ʱ��Ƶ�ʵ�λHz
//�궨����ַ�Ƶϵ��
#define FRE_DIV_1           0x00
#define FRE_DIV_8           BV(2)
#define FRE_DIV_32          BV(3)
#define FRE_DIV_128         (BV(3) | BV(2))
#define USE_FRE_DIV         FRE_DIV_32   //ʵ�ʷ�Ƶ������

//�궨����ּ���ģʽ
#define MODULO_MODE         BV(1)

//�궨����ֱȽ�ģʽ
#define OUT_MODE 0x30

//���岶����߱Ƚ�ģʽ
#define COM_MODE      BV(2)

//������T1CC0��װ�ص�����
#if (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_1)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/1/OUT_FREQUENCY) //װ������ = (��Ƶ / ��Ƶ / ���Ƶ��)
#elif (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_8)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/8/OUT_FREQUENCY) //װ������ = (��Ƶ / ��Ƶ / ���Ƶ��)
#elif (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_32)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/32/OUT_FREQUENCY) //װ������ = (��Ƶ / ��Ƶ / ���Ƶ��)
#elif (defined USE_FRE_DIV)&&(USE_FRE_DIV == FRE_DIV_128)
#define DOWN_LOAD_VALUE (MAIN_CLOCK/128/OUT_FREQUENCY) //װ������ = (��Ƶ / ��Ƶ / ���Ƶ��)
#endif

#define T3_SET_FRE_DIV()   (T3CTL |= (BV(7)|BV(6)|BV(5))) //128��Ƶ
#define T3_USE_MODULE_COUNT() (T3CTL |= BV(1))//ʹ��ģ����
#define PWM_ON()  (T1CTL |= 0x02)
#define PWM_OFF() (T1CTL &= ~0x02)

#define LUMINANCE_STEP 4
#define INT_T3_TIMES_MS 50
typedef union
{
  uint16 u16Counters;
  uint8 u8Counters[2]; //С�˵�ַģʽu8Counters[1]Ϊ��8λ
}CC_TYPE;

CC_TYPE load_T1CC0_value;//װ�ص�T1CC0�Ĵ�����ֵ������PWM������
CC_TYPE load_T1CCx_value;//װ�ص�T1CC1�Ĵ�����ֵ������PWM����ռ�ձ�

static uint8 u8Luminance_S = 100; //���Ȱٷ�����ȡֵ0��100
uint8 u8UpOrDownFlag_G = 1;

static void InitTimer3(void);
//*********************************************
//
//�������ƣ�ini_pwm
//
//���������void
//
//���������void
//
//����    ��2014-9-13, by HOLO
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
  
  //����pwm�˿�Ϊ���
  P0DIR |= PWM_USE_CHANNAL;
  //����pwm�˿�Ϊ����˿ڣ���gpio
  P0SEL |= PWM_USE_CHANNAL;
  
  PERCFG |= 0X03; //��uart0��uart1�ƶ�������IO�˿�
  
  T1CNTL = 0; // Reset timer to 0;
  T1CNTH = 0; // Reset timer to 0;�� T1CNTL �Ĵ���������д����ʽ���λ 16 λ������
  
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
  
  T1CTL = USE_FRE_DIV + MODULO_MODE; // Div = USE_FRE_DIV,  MODE = module��
                                    //��һ������ 00ֵ��д�뵽 T1CTL.MODE ��������ʼ����  
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
  T3CNT = 0; //����������
  IEN1 |= BV(3);      //����ʱ��3�ж�
  IEN0 |= BV(7);      //�����ж�
  T3CC0 = 250;         //��ʱ��32Mhz����128��Ƶ������250֮��ǡ��1ms�ж�һ��
  T3CCTL0 |= 0X44;    //����ʱ��3ͨ��0�趨Ϊ�����жϣ���Ϊ�Ƚ�ģʽ
  T3_SET_FRE_DIV();      //�趨��Ƶ
  T3_USE_MODULE_COUNT(); //�趨Ϊģģʽ
  T3_STATR_COUNT();      //����������
}


//��ʱ�� T3 �жϴ����� 
#pragma vector = T3_VECTOR    
__interrupt void Timer3_ISR(void)
{
  static uint8 u8Index_S = 0;
  
  IEN0 &= ~BV(7);  //�����ж�
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
  IEN0 |= BV(7);  //�����ж�
}

