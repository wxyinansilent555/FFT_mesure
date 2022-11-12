/*
 * ����޸�ʱ�䣺2022��/11��/8��
 * �޸���һЩADCͨ���ϵĴ��󣬸�����ADC����ΪA2,A3
 * ADCʹ���ⲿ��ʱ����Ҫʹ��PWM����������ô�Ƚϴ����ģ�
 */
#include "AD.h"

void AD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//GPIO���ýṹ��
    ADC_InitTypeDef ADC_InitStructure;//ADC���ýṹ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ADC1��ʱ�ӻ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//GPIOAʱ�ӻ���
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCCLK=72MHZ/6=12MHZ

    ADC_DeInit(ADC1);  //��λADC1����ADC1��ȫ���Ĵ�������Ϊȱʡֵ

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ģ������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_2;//GPIOA0 �� | GPIOA2 ��
	GPIO_Init(GPIOA, &GPIO_InitStructure);//���ؽṹ��

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;           //ADC����ģʽ����������ģʽ����ѡ��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		 //ADC�����Ҷ���ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//�ⲿ����Դѡ�񣬲�ʹ���ⲿ���������������,
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			 //����ģʽ���ǵ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;				 //ɨ��ģʽ���Ƿ�ɨ��ģʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1;						 //ͨ����Ŀ������ɨ��ģʽ������
	ADC_Init(ADC1, &ADC_InitStructure);							 //�ýṹ������ADC1

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);//16��ͨ��������ʱ��1.5

    //ADC_DMACmd(ADC1, ENABLE); 								//ADC���ʹADC1��DMA����

	ADC_Cmd(ADC1, ENABLE);//����ADC�ĵ�Դ
	
	//�����ֲᶨ�壬��ADCУ׼
	ADC_ResetCalibration(ADC1);//��λУ׼
	while (ADC_GetResetCalibrationStatus(ADC1))
		;//�ȴ���λУ׼���
	
	ADC_StartCalibration(ADC1);//ADC��ʼУ׼
	while (ADC_GetCalibrationStatus(ADC1))
		;//�ȴ�ADCУ׼���

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);    //ʹ���ⲿ��ʱ������
}

/*
//AD����DMAת�˲���
void AD_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA1_Channel1,4);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA1_Channel1, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 

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
