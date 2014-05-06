#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>

#include "stm32f4xx.h"

u16 Hex_To_Ascii( u8* pDst, const u8* pSrc, u16 nSrcLength )
{
	const u8	tab[] = "0123456789ABCDEF"; // 0x0-0xf的字符查找表
	u16			i;

	for( i = 0; i < nSrcLength; i++ )
	{
		// 输出低4位
		*pDst++ = tab[*pSrc >> 4];

		// 输出高4位
		*pDst++ = tab[*pSrc & 0x0f];

		pSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return ( nSrcLength << 1 );
}


void printf_hex_data( const u8* pSrc, u16 nSrcLength )
{
 	const u8	tab[] = "0123456789ABCDEF"; // 0x0-0xf的字符查找表
 	char 		pDst[3];
	u16			i;

	
	pDst[2]  = 0;
	for( i = 0; i < nSrcLength; i++ )
	{
		// 输出低4位
		pDst[0] = tab[*pSrc >> 4];

		// 输出高4位
		pDst[1] = tab[*pSrc & 0x0f];

		pSrc++;

		rt_kprintf(pDst);
	}
}


u8 Get_Month_Day(u8 month,u8 leapyear)
{
  u8 day;
  switch(month)
 	{
	case 12 :
 		day=31;
		break;
	case 11 :
 		day=30;
		break;
	case 10 :
 		day=31;
		break;
	case 9 :
 		day=30;
		break;
	case 8 :
 		day=31;
		break;
	case 7 :
 		day=31;
		break;
	case 6 :
 		day=30;
		break;
	case 5 :
 		day=31;
		break;
	case 4 :
 		day=30;
		break;
	case 3 :
 		day=31;
		break;
	case 2 :
		{
 		day=28;
		day+=leapyear;	
		break;
		}	
	case 1 :
 		day=31;
		break;
	default :
		break;
 	}
  return day;
}



// **************************************************************************
// trim a string of it's leading and trailing characters
// 该函数功能为去掉字符串s中前后为c的字符
void strtrim( u8* s, u8 c )
{
		u8	i, j, * p1, * p2;

		if ( s == 0 )
		{
				return;
		}

		// delete the trailing characters
		if ( *s == 0 )
		{
				return;
		}
		j = strlen( (const char*)s );
		p1 = s + j;
		for ( i = 0; i < j; i++ )
		{
				p1--;
				if ( *p1 != c )
				{
						break;
				}
		}
		if ( i < j )
		{
				p1++;
		}
		*p1 = 0;	// null terminate the undesired trailing characters

		// delete the leading characters
		p1 = s;
		if ( *p1 == 0 )
		{
				return;
		}
		for ( i = 0; *p1++ == c; i++ )
				;
		if ( i > 0 )
		{
				p2 = s;
				p1--;
				for ( ; *p1 != 0; )
				{
						*p2++ = *p1++;
				}
				*p2 = 0;
		}
}

