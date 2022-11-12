#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 key_scan( u8 mode )
{
	
	static u8 key = 1 ;
	
	if( mode ) key = 1 ;
	
	if( key == 1 && ( key0 == 0 || key1 == 0 ) )
	{
		
		Delay_ms( 10 ) ;
		key = 0 ;
		if     ( key0 == 0 ) return 1 ;
		else if( key1 == 0 ) return 2 ;
		
	}else if ( ( key0 == 1 ) && ( key1 == 1 ) ) key = 1 ;
	
	return 0 ;
	
}
