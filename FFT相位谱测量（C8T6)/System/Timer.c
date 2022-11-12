/*
 * 由于采样的方波频率在50KHZ，根据奈奎斯特定理，采样的信号需要在频率的两倍以上，因此定时中断频率需要设置为100KHZ以上
 * 由于通用定时器占用了PA0引脚，因此修改了ADC引脚
 * 疑问点： 1、定时器唤醒时间仅为10US，是否要修改AD的采样时间，55.5个时间间隔可行吗？
 *      ： 2、定时器的触发时间最短是多少？如此高频率进中断是否会影响主程？
 *      ： 3、AD和TIM2引脚之间是否存在干扰？
 *      ： 4、定时中断的抢占优先级设置多少合适呢？
 */

#include "timer.h"

//定时器1通道1pwm输出配置
void Timer1_Pwm_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);

	//设置该引脚为复用输出功能，输出TIM1 CH4的PWM脉冲波形 GPIOA11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;                     //TIM1_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                         //初始化GPIO

	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 20000-1;                 //arr
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //psc
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	//Tout(us) = ((psc+1) * (arr+1))/Tclk;

    //初始化TIM2 CHANNEL1 Pwm模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;            //选择定时器模式：TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //输出极性：TIM比较输出极性高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 500;		                 //发生反转时的计数器数值，用于控制占空比
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);                     //根据指定的参数初始化TIM2_CH1
	
	TIM_Cmd(TIM1, ENABLE);                                       //TIM2输出使能
	TIM_CtrlPWMOutputs(TIM1, ENABLE);							 //使能PWM输出
}

//定时器2通道1pwm输出配置
void Timer2_Pwm_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//输出TIM2的pwm波形 PA.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 						 //TIM2_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  				 //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							 //使能GPIO

    TIM_TimeBaseInitStructure.TIM_Period = arr;                      //设置在下一更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseInitStructure.TIM_Prescaler =psc;                    //设置用来作为TIMX时钟频率的预分频值
    TIM_TimeBaseInitStructure.TIM_ClockDivision = 0 ;                //设置时钟分割：TDTS = Tck_tim
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);              //根据TIM_TimeBaseStructure指定的参数用于初始化时钟TIM1

    //初始化TIM2 CHANNEL1 Pwm模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                //选择定时器模式：TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //输出极性： PWM1模式要LOW PWM2模式要HIGH
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;		                 //发生反转时的计数器数值，用于控制占空比
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);                         //根据指定的参数初始化TIM2_CH1

    TIM_CtrlPWMOutputs(TIM2, ENABLE);							     //使能PWM输出

	TIM_Cmd(TIM2, ENABLE);                                           //使能TIM1
}

/*
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
*/
