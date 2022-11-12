 #include "adc.h"

//��ʼ��ADC
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	
	//PA2 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;  //��PA.0 ��PA.2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		    //ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//����ADC�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			    //ɨ���ͨ��ģʽ
    //	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;		//����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//����ת��ģʽ ��ʱ����������Ҫ����
	//��ֹ�ⲿ����ת��ģʽ���������
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//AD�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;					//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//�������ͨ�����������ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);	
	
	ADC_DMACmd(ADC1, ENABLE); 								//ADC�����DMA����
	
	ADC_Cmd(ADC1, ENABLE);									  //ʹ��ADC 	  

	ADC_ResetCalibration(ADC1);								//��λADCУ׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));				//�ȴ�У׼�Ĵ�����λ���

	ADC_StartCalibration(ADC1);								//��ʼADCУ׼
	while(ADC_GetCalibrationStatus(ADC1));		//�ȴ�У׼���

	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//�������ADת��
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);    //ʹ���ⲿ��ʱ������
}




