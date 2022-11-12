#include "fft_calculate.h"
#include "math.h"
#include "dma.h"

#define PI 3.1415926535897932384626433832795

float angle = 0;

long InBufArray[NPT]={0};
long OutBufArray[NPT/2];
long MagBufArray[NPT/2];

void FFT_Compute(u16 *ADC_TempValue)
{
	int i;
	for(i=0;i<NPT;i++)
	{
		InBufArray[i] = ADC_TempValue[i] ;
		InBufArray[i] = ((signed short)InBufArray[i] - 2048 ) << 16 ;   //抬高 3.3 / 2 = 1.65V
	}
	cr4_fft_256_stm32(OutBufArray, InBufArray, NPT) ;
}

unsigned long GetPowerMag(void)
{
	signed short lX,lY;
	float X,Y,Mag;

	lX  = (OutBufArray[20] << 16) >> 16;
	lY  = (OutBufArray[20] >> 16);
	
	//除以32768再乘65536是为了符合浮点数计算规律
	X =(float) NPT * ((float)lX) / 32768;
	Y =(float) NPT * ((float)lY) / 32768;
	Mag = sqrt(X * X + Y * Y) / NPT;
	
	angle = atan(Y/X);	
	
	if(X>0 && Y>0 && angle<0) angle += PI;
	if(X<0 && Y<0 && angle>0) angle -= PI;
	
	if(X>0 && Y<0 && angle>0) angle -= PI;
	if(X<0 && Y>0 && angle<0) angle += PI;
	
	return (unsigned long)(Mag * 65536);
}
