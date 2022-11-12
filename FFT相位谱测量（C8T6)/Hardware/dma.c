#include "dma.h"

volatile u16 ADC_TempValue[2*NPT];

DMA_InitTypeDef DMA_InitStructure;
#define ADC1_DR_Address    ((u32)0x4001244C) //adc1����ʼ��ַ��40012400 ��ƫ����4c
u16 DMA1_MEM_LEN;
//����DMAÿ�����ݴ��͵ĳ���
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ�����

void MYDMA_Config(void)
{
	DMA_InitTypeDef  DMA_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE );

  	//DMA init;  Using DMA1 channel 1
  	DMA_DeInit(DMA1_Channel1);    							//��λDMA1�ĵ�1ͨ��
  	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;	//DMA��Ӧ���������ַ��ʹ�ö�����ṹ�����
  	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //ת�����16bits������
  	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)ADC_TempValue;
  	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;   		//DMA��ת��ģʽ��SRCģʽ������to�ڴ�
  	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;         		//M2Mģʽ��ֹ��memory to memory
  	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA���˵�����16bits
  
  	//����һ�����ݺ�Ŀ���ڴ��ַ���ƣ������ɼ�������ݵģ�����
  	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//����һ�����ݺ��豸��ַ�Ƿ����
  	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

  	//ת��ģʽ������ѭ������ģʽ�����M2M�����ˣ������ģʽʧЧ
  	//��һ����Normalģʽ����ѭ������һ��DMA
  	//DMA_InitStruct.DMA_Mode  = DMA_Mode_Circular;
  	DMA_InitStruct.DMA_Mode  = DMA_Mode_Normal;
	
  	DMA_InitStruct.DMA_Priority = DMA_Priority_High; 			//DMA���ȼ�����
  	DMA_InitStruct.DMA_BufferSize = 2*NPT;      				//DMA�����С
  	DMA_Init(DMA1_Channel1,&DMA_InitStruct);

  	DMA_ClearITPendingBit(DMA1_IT_TC1);
  	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);				//����DMA1CH1�ж�

   	DMA_Cmd(DMA1_Channel1, ENABLE);
}

//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	  

 








