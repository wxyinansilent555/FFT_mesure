#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

#define key0 PBin(0)   	
#define key1 PBin(1)	 

void Key_Init(void);
u8 key_scan ( u8 mode );

#endif
