/*
2022-11-1   AD双通道 1.5采样周期 PA0 PA2
*/

#include "stm32f10x.h"                  // Device header
#include "math.h"

#include "fft_calculate.h"
//#include "table_fft.h"
#include "Delay.h"

#include "Timer.h"
#include "Key.h"
#include "OLED.h"
#include "AD.h"
#include "dma.h"

#define c 0.3442
#define d 60
#define PI 3.1415926535897932384626433832795

#define Time 50

u16 ADC_val0[2*NPT];
u16 ADC_val1[2*NPT];
uint16_t AD_Value[4];

float cita = 0;     //偏角
int distance = 0;   //距离
int x = 0, y = 0;   //坐标

int flag = 0;
int calibration = 0;

float v0 = 0, v1 = 0;          //5k hz信号幅值
float angle0 = 0, angle1 = 0; //相位角
float D1 = 0;
float T1[Time] = {0};
float tao1 = 0;      //时间差
float dt = 0;        //偏移

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//中位值滤波函数
float Mid_Value_Filter(float *databuffer)
{
    float temp;
    u16 i,j;
    u8  times = Time;

    for(i=0;i<times-1;i++)
    {
        for(j=0;j<times-1-i;j++)
        {
            if(databuffer[j]>databuffer[j+1])           	//升序
            {
                temp=databuffer[j+1];
                databuffer[j+1]=databuffer[j];
                databuffer[j]=temp;
            }
        }
    }

    temp=0;
    for(i=times/4;i<(times-times/4);i++)							//抽取中间进行均值计算
    {
        temp+=databuffer[i];
    }
    temp=2*temp/times;

    return temp;
}

int main(void)
{
	OLED_Init();
	Timer1_Pwm_Init();           //PA.1 --> servo(num)      num --> 500 - 2500
	Timer2_Pwm_Init(1124,0);     //采样频率24khz
    AD_Init();
	MYDMA_Config();
	NVIC_Configuration();

	/*
    OLED_ShowString(1,1,"Dis:");
    OLED_ShowString(2,1,"Ang:");
    OLED_ShowString(3,1,"X = ");
    OLED_ShowString(4,1,"Y = ");

    //uint_32的是8个byte
	OLED_ShowNum(1,5,(uint32_t)distance,8);
    OLED_ShowNum(2,5,(uint32_t)cita,8);
    OLED_ShowNum(3,5,(uint32_t)x,8);
    OLED_ShowNum(4,5,(uint32_t)y,8);
	*/
	
    servo(500);

    while (1)
	{
		/*
		OLED_ShowNum(1,5,ADC_val0[0],2);
		OLED_ShowNum(2,5,ADC_val1[0],2);
		OLED_ShowNum(3,5,ADC_val0[1],2);
		OLED_ShowNum(4,5,ADC_val1[1],2);
		*/
		
		/*
        if(v0>1500 && v1>1500)
        {
            cita = acos(c*tao1/d)*180.0/PI - 90;
            flag = 1;
        }

        if(flag == 1 && calibration == 1)
        {
            //显示角度
            if(cita>=0)
            {
                OLED_ShowString(2,5,"+");
                OLED_ShowNum(2,6,(uint32_t)cita,8);
            }
            if(cita< 0)
            {
                OLED_ShowString(2,5,"-");
                OLED_ShowNum(2,6,(uint32_t)-cita,8);
            }

            y = 250 + (int)(v0 - v1) % 50;
            x = y * tan((double)cita*PI/180);
            distance = sqrt(x * x + y * y);

            //显示X
            if(x>=0)
            {

                OLED_ShowString(3,5,"+");
                OLED_ShowNum(3,6,(uint32_t)x,8);
            }
            if(x< 0)
            {
                OLED_ShowString(3,5,"-");
                OLED_ShowNum(3,6,(uint32_t)-x,8);
            }

            //显示Y
            if(y>=0)
            {
                OLED_ShowString(4,5,"+");
                OLED_ShowNum(4,6,(uint32_t)y,8);
            }
            if(y< 0)
            {
                OLED_ShowString(1,5,"+");
                OLED_ShowNum(4,5,(uint32_t)-y,8);
            }

            OLED_ShowNum(1,5,(uint32_t)distance,8);

            if(822 - cita*2000/180 < 500) servo(500)
            else if(822 - cita*2000/180 > 1100) servo(1100)
            else servo(822 - cita*2000/180);
			
            flag = 0;
        }
		*/
		
        if(key_scan(0) == 1)
        {
            dt = tao1*(6.0/5.5)*(180.0/100.0);
            calibration = 1;
        }

        if(tao1>=0)
        {
           OLED_ShowString(1,1,"+");
           OLED_ShowNum(1,2,tao1*100,8);
        }
        if(tao1< 0)
        {
            OLED_ShowString(1,1,"-");
            OLED_ShowNum(1,2,-tao1*100,8);
        }
		
    }
}

void DMA1_Channel1_IRQHandler(void)
{
    int i = 0;
    static int n = 0;
    static int count = 0;
	
    if(DMA_GetITStatus(DMA1_IT_TC1)==1)
    {
        if(count == 0)
        {
            for(i = 0;i < 2*NPT;i++) ADC_val0[i] = ADC_TempValue[i];
            ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);
            count++;
        }
        else if(count == 1)
        {
            for(i = 0;i < 2*NPT;i++) ADC_val1[i] = ADC_TempValue[i];
            ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);

            FFT_Compute(ADC_val0);
            v0 = 2 * GetPowerMag() * 3300.0 / 4095;
            angle0 = angle*180.0/PI;

            FFT_Compute(ADC_val1);
            v1 = 2 * GetPowerMag() * 3300.0 / 4095;
            angle1 = angle*180.0/PI;

            D1 = angle0 - angle1 - 51 - dt;

            if(D1>-180 && D1<180)
            {
                T1[n] = D1*100.0/180;
                n++;
                if(n>=Time)
                {
                    tao1 = Mid_Value_Filter(T1)*5.5/6 ;
                    n = 0;
                }
            }
            count = 0;
        }

        DMA_Cmd(DMA1_Channel1, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel1,2*NPT);
        DMA_Cmd(DMA1_Channel1, ENABLE);

        DMA_ClearITPendingBit(DMA1_IT_TC1);
        DMA_ClearFlag(DMA1_FLAG_TC1);
    }
}

