#ifndef _H_COMMON
#define _H_COMMON

#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
//#include <dfs_posix.h>

#include "stm32f4xx.h"

#ifndef BIT
#define BIT( i ) ( (unsigned long)( 1 << i ) )
#endif

extern u16 Hex_To_Ascii( u8* pDst, const u8* pSrc, u16 nSrcLength );
extern void printf_hex_data( const u8* pSrc, u16 nSrcLength );


#endif
