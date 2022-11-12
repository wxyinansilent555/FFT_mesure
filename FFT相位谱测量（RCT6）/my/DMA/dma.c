#include "dma.h"

volatile u16 ADC_TempValue[2*NPT];

DMA_InitTypeDef DMA_InitStructure;
#define ADC1_DR_Address    ((u32)0x4001244C)
u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 	    

//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量 
void MYDMA_Config(void)
{
	DMA_InitTypeDef  DMA_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE );

  	//DMA init;  Using DMA1 channel 1
  	DMA_DeInit(DMA1_Channel1);    							//复位DMA1的第1通道
  	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;	//DMA对应的外设基地址
  	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //转换结果16bits
  	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)ADC_TempValue;
  	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;   		//DMA的转换模式是SRC模式，外设to内存
  	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;         		//M2M模式禁止，memory to memory
  	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA搬运的数据16bits
  
  	//接收一次数据后，目标内存地址后移，用来采集多个数据的
  	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	//接收一次数据后，设备地址是否后移
  	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

  	//转换模式：常用循环缓存模式。如果M2M开启了，则这个模式失效
  	//另一种是Normal模式：不循环，仅一次DMA
  	//DMA_InitStruct.DMA_Mode  = DMA_Mode_Circular;
  	DMA_InitStruct.DMA_Mode  = DMA_Mode_Normal;
	
  	DMA_InitStruct.DMA_Priority = DMA_Priority_High; 			    //DMA优先级，高
  	DMA_InitStruct.DMA_BufferSize = 2*NPT;      					//DMA缓存大小
  	DMA_Init(DMA1_Channel1,&DMA_InitStruct);

	DMA_ClearITPendingBit(DMA1_IT_TC1);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);				    //开启DMA1CH1中断

   	DMA_Cmd(DMA1_Channel1, ENABLE);
}

//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	  

 








