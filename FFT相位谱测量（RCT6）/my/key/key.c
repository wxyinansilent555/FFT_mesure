#include "key.h"

void key_init ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Init( GPIOA , & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_Init( GPIOC , & GPIO_InitStructure );
}

u8 key_scan ( u8 mode )
{
	static u8 key = 1 ;
	
	if( mode ) key = 1 ;
	
	if( key == 1 && ( key0 == 0 || key1 == 0 ) )
	{
		delay_ms( 10 ) ;
		key = 0 ;
		if     ( key0 == 0 ) return 1 ;
		else if( key1 == 0 ) return 2 ;
	}
	else if ( ( key0 == 1 ) && ( key1 == 1 ) ) key = 1 ;
	
	return 0 ;
}

