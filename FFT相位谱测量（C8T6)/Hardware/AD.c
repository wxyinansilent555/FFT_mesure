/*
 * 最后修改时间：2022年/11月/8日
 * 修改了一些ADC通道上的错误，更改了ADC引脚为A2,A3
 * ADC使用外部定时器需要使用PWM触发，是怎么比较触发的？
 */
#include "AD.h"

void AD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//GPIO配置结构体
    ADC_InitTypeDef ADC_InitStructure;//ADC配置结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ADC1的时钟唤醒
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//GPIOA时钟唤醒
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCCLK=72MHZ/6=12MHZ

    ADC_DeInit(ADC1);  //复位ADC1，将ADC1的全部寄存器重设为缺省值

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_2;//GPIOA0 左 | GPIOA2 右
	GPIO_Init(GPIOA, &GPIO_InitStructure);//传回结构体

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;           //ADC独立模式，还有其他模式可以选择
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		 //ADC数据右对齐模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//外部触发源选择，不使用外部触发，即软件触发,
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			 //连续模式还是单次模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;				 //扫描模式还是非扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;						 //通道数目，仅在扫描模式下有用
	ADC_Init(ADC1, &ADC_InitStructure);							 //用结构体配置ADC1

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);//16个通道，采样时间1.5

    //ADC_DMACmd(ADC1, ENABLE); 								//ADC命令，使ADC1与DMA关联

	ADC_Cmd(ADC1, ENABLE);//开启ADC的电源
	
	//根据手册定义，对ADC校准
	ADC_ResetCalibration(ADC1);//复位校准
	while (ADC_GetResetCalibrationStatus(ADC1))
		;//等待复位校准完成
	
	ADC_StartCalibration(ADC1);//ADC开始校准
	while (ADC_GetCalibrationStatus(ADC1))
		;//等待ADC校准完成

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);    //使能外部定时器触发
}

/*
//AD采样DMA转运测试
void AD_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA1_Channel1,4);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA1_Channel1, ENABLE);  //使能USART1 TX DMA1 所指示的通道 

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}
*/

/*
uint16_t AD_GetValue(void)
{
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}
*/
