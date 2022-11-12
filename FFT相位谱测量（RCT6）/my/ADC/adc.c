 #include "adc.h"

//初始化ADC
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能ADC1通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
	
	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	
	//PA2 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;  //左PA.0 右PA.2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		    //模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//两个ADC工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			    //扫描多通道模式
    //	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;		//连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//连续转换模式 定时器触发不需要连续
	//禁止外部触发转换模式，软件触发
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//AD数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;					//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//规则采样通道次序与采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);	
	
	ADC_DMACmd(ADC1, ENABLE); 								//ADC命令，和DMA关联
	
	ADC_Cmd(ADC1, ENABLE);									  //使能ADC 	  

	ADC_ResetCalibration(ADC1);								//复位ADC校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));				//等待校准寄存器复位完成

	ADC_StartCalibration(ADC1);								//开始ADC校准
	while(ADC_GetCalibrationStatus(ADC1));		//等待校准完成

	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//软件启动AD转换
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);    //使能外部定时器触发
}




