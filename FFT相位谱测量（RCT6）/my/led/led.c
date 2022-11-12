#include "stm32f10x.h"
#include "led.h"

void led_init( void )
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Init( GPIOA , & GPIO_InitStructure );
	GPIO_SetBits( GPIOA , GPIO_Pin_8 );
	
}


//¼Ä´æÆ÷
//void led_init( void )
//{
//	
//	RCC->APB2ENR |= 1 << 3 ;
//	RCC->APB2ENR |= 1 << 6 ;
//	
//	GPIOA->CRH &= 0xFFFFFFF0 ;
//	GPIOA->CRH |= 0x00000003 ;
//	GPIOA->ODR |= 1 << 8 ;
//	
//	GPIOD->CRL &= 0x0FFFF0FF ;
//	GPIOD->CRL |= 0x00000300 ;
//	GPIOD->ODR |= 1 << 2 ;
//	
//	
//}
	
	

