#ifndef __DMA_H
#define	__DMA_H	   

#include "sys.h"
#include "fft_calculate.h"

extern volatile u16 ADC_TempValue[2*NPT];

void MYDMA_Config(void);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);//Ê¹ÄÜDMA1_CHx
		   
#endif




