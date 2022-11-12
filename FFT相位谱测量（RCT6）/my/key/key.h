#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"

void key_init ( void ) ;
u8 key_scan ( u8 mode ) ;

#define key0 PCin(5)   	
#define key1 PAin(15)	 

#endif
