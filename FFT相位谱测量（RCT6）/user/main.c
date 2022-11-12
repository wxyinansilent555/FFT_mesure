/*
 * 项目：声源定位（基于FFT相位谱测量）
 * 最后修改时间：2022-11-9
 * 声波频率：5khz方波
 * ADC采样频率：24Khz  PA.0右  PA.2左
 * 舵机控制：50HZPWM  占空比控制角度 PA.11
 * 按键：PA.15 PC.5
 */

#include "stm32f10x.h"
#include "math.h"

#include "fft_calculate.h"
#include "table_fft.h"     //ST汇编库
#include "delay.h"

#include "timer.h"
#include "usart.h"
#include "key.h"
#include "lcd.h"
#include "adc.h"
#include "dma.h"
#include "led.h"

#define c 0.3442 //声速
#define d 80     //两接收模块间距离80
#define PI 3.1415926535897932384626433832795

#define Time 50

u16 ADC_val0[2*NPT];
u16 ADC_val1[2*NPT];

float cita = 0;     //偏角
int distance = 0;   //距离
int x = 0, y = 0;   //坐标

int flag = 0;
int calibration = 1;
int servo_num ;

float v0 = 0, v1 = 0;  //5kHZ信号幅值
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

//中位值均值滤波
float Mid_Value_Filter(float *databuffer)
{
    float temp;//中间数据存放
    u16 i,j;
    u8  times = Time;//50个数据一组进行滤波

    for(i=0;i<times-1;i++)
    {
        for(j=0;j<times-1-i;j++)
        {
            if(databuffer[j]>databuffer[j+1])           	//升序排列
            {
                temp=databuffer[j+1];
                databuffer[j+1]=databuffer[j];
                databuffer[j]=temp;
            }
        }
    }

    temp=0;
    for(i=times/4;i<(times-times/4);i++)							//抽取中间进行均值运算
    {
        temp+=databuffer[i];
    }
    temp=2*temp/times;

    return temp;
}


int main( void )
{
	u8 lcd_id[12];
	
    delay_init();
	uart_init(9600);
	key_init();
	LCD_Init();
	led_init();
	TIM1_PWM_Init();          //PA.11 --> servo(num)      num --> 500 - 2500
	TIM2_PWM_Init(1124,0);    //采样频率  64kHZ
	Adc_Init();
	MYDMA_Config();
	NVIC_Configuration();
	
	servo(722);              //左1000 右500 中722

	POINT_COLOR = RED;
	sprintf((char*)lcd_id ,"LCD ID : %04X" ,lcddev.id);
	BACK_COLOR  = WHITE ;
	LCD_Clear( BACK_COLOR );

	/*
    LCD_DrawLine(50,300,200,300);
	LCD_DrawLine(125,225,125,299);
    LCD_DrawLine(x+125,300-y,125,300);
	*/
	
	LCD_ShowString(10,10,88,16,16,(unsigned char*)"Distance = ");
	LCD_ShowString(10,30,88,16,16,(unsigned char*)"Angle    = ");
	LCD_ShowString(10,50,32,16,16,(unsigned char*)"X = ");
	LCD_ShowString(10,70,32,16,16,(unsigned char*)"y = ");
    LCD_ShowString(10,90,32,16,16,(unsigned char*)"v0 = ");
    LCD_ShowString(10,110,32,16,16,(unsigned char*)"v1 = ");
	LCD_ShowString(10,130,32,16,16,(unsigned char*)"DV = ");
    LCD_ShowString(10,150,32,16,16,(unsigned char*)"DA = ");
	LCD_ShowString(10,170,32,16,16,(unsigned char*)"SE = ");
	
	LCD_ShowString(130,10,16,16,16,(unsigned char*)"cm");
	LCD_ShowString(130,30,16,16,16,(unsigned char*)" C");
	LCD_ShowString(74,50,16,16,16,(unsigned char*)"cm");
	LCD_ShowString(74,70,16,16,16,(unsigned char*)"cm");
	
	LCD_ShowxNum(98+8,10,distance,3,16,0);
	LCD_ShowxNum(98+8,30,cita,3,16,0);
	LCD_ShowxNum(42+8,50,x,3,16,0);
	LCD_ShowxNum(42+8,70,y,3,16,0);
    LCD_ShowxNum(42+8,90,v0,3,16,0);
    LCD_ShowxNum(42+8,110,v1,3,16,0);

	while(1)
	{
        LCD_ShowxNum(42+8,90,v0,3,16,0);
        LCD_ShowxNum(42+8,110,v1,3,16,0);
        /*
        LCD_DrawLine(x+125,300-y,125,300);
		*/

		if(v0>950 && v1>950) //没有显示数据请考虑经过傅里叶变换后的数据是否没大于该值
		{
			cita = acos(c*tao1/d)*180.0/PI - 90;
			flag = 1;
		}
		
		if(flag == 1 && calibration == 1)
		{
			//显示角度
			if(cita>=0)
			{
				LCD_ShowString(98,30,8,16,16,(unsigned char*)"+");
				LCD_ShowxNum(98+8,30,cita,3,16,0);
			}
			if(cita< 0)
			{
				LCD_ShowString(98,30,8,16,16,(unsigned char*)"-");
				LCD_ShowxNum(98+8,30,-cita,3,16,0);
			}
			
			y = 270 + (int)(v0 - v1) % 50;  //根据要求声源和拾音器摆放在2.5m
			x = y * tan((double)cita*PI/180);
			distance = sqrt(x * x + y * y);
			
			//显示x
			if(x>=0)
			{
				LCD_ShowString(42,50,8,16,16,(unsigned char*)"+");
				LCD_ShowxNum(42+8,50,x,3,16,0);
			}
			if(x< 0)
			{
				LCD_ShowString(42,50,8,16,16,(unsigned char*)"-");
				LCD_ShowxNum(42+8,50,-x,3,16,0);
			}
			
			//显示y
			if(y>=0)
			{
				LCD_ShowString(42,70,8,16,16,(unsigned char*)"+");
				LCD_ShowxNum(42+8,70,y,3,16,0);
			}
			if(y< 0)
			{
				LCD_ShowString(42,70,8,16,16,(unsigned char*)"-");
				LCD_ShowxNum(42+8,70,-y,3,16,0);
			}
			
			LCD_ShowxNum(98+8,10,distance,3,16,0);
			
			laser = 0;
			
			if(722 - cita*2000/180 < 500) servo(500);//右转限幅 arr为20000对应100%占空比，2000/180=11.11
			else if(722 - cita*2000/180 > 1000) servo(1000);//左转限幅
			else servo(722 - cita*2000/180);//1000为中间值

			
			flag = 0;
		}
		
		if(key_scan(0) == 1)
		{
			dt = tao1*(180.0/100.0)*5.5/6 ;
			calibration = 1;
		}
		
		if(tao1>=0)
		{
			LCD_ShowString(20,200,8,16,16,(unsigned char*)"+");
			LCD_ShowxNum(30,200,tao1*100,5,16,0);
		}
		if(tao1< 0)
		{
			LCD_ShowString(20,200,8,16,16,(unsigned char*)"-");
			LCD_ShowxNum(30,200,-tao1*100,5,16,0);
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
			
			D1 = angle0 - angle1 - dt - 25;
			LCD_ShowxNum(42+8,130,v0-v1,3,16,0);
			LCD_ShowxNum(42+8,150,D1,3,16,0);
			
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
