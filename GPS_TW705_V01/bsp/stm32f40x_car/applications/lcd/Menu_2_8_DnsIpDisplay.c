/************************************************************
 * Copyright (C), 2008-2012,
 * FileName:		// 文件名
 * Author:			// 作者
 * Date:			// 日期
 * Description:		// 模块描述
 * Version:			// 版本信息
 * Function List:	// 主要函数及其功能
 *     1. -------
 * History:			// 历史修改记录
 *     <author>  <time>   <version >   <desc>
 *     David    96/10/12     1.0     build this moudle
 ***********************************************************/
#include "Menu_Include.h"
#include  <string.h>

#include "sed1520.h"

static uint8_t pos;

/*显示网络信息*/
static void display( )
{
	char	buf[32];
	uint8_t page;

	lcd_fill( 0 );
	if(pos>4)
		pos = 0;
	if(pos > 1)
	{
		page = (pos-2);
		//page = (pos-2) >> 1;
		/*
		if( ( pos-2 ) & 0x01 )
		{
			lcd_text12( 0, 4, gsm_socket[page].ip_addr, strlen( gsm_socket[page].ip_addr ), LCD_MODE_SET );
			sprintf( buf, "端口:%d", gsm_socket[page].port );
			lcd_text12( 0, 18, buf, strlen( buf ), LCD_MODE_SET );
		}else
		*/
		{
			if( gsm_socket[page].state == CONNECTED )
			{
				sprintf( buf, "连接%d  OK,端口:%05d", page,gsm_socket[page].port );
			}else
			{
				sprintf( buf, "连接%d未连接:%05d", page,gsm_socket[page].port );
			}
			lcd_text12( 0, 4, buf, strlen(buf), LCD_MODE_SET );
			lcd_text12( 0, 18, gsm_socket[page].ipstr, strlen( gsm_socket[page].ipstr ), LCD_MODE_SET );
		}
	}
	else
	{
		if(pos == 0)
		{
			sprintf( buf, "主服务器  端口%d", jt808_param.id_0x0018 );
			lcd_text12( 0, 4, buf, strlen( buf ), LCD_MODE_SET );
			lcd_text12( 0, 18, jt808_param.id_0x0013, strlen( jt808_param.id_0x0013 ), LCD_MODE_SET );
		}
		else if(pos == 1)
		{
			sprintf( buf, "备份服务器 端口%d", jt808_param.id_0xF031);
			lcd_text12( 0, 4, buf, strlen( buf ), LCD_MODE_SET );
			lcd_text12( 0, 18, jt808_param.id_0x0017, strlen( jt808_param.id_0x0017 ), LCD_MODE_SET );
		}
	}
	lcd_update_all( );
}

/*显示网络信息*/
static void display_old( )
{
	char	buf[24];
	uint8_t page;

	lcd_fill( 0 );

	page = pos >> 1;
	
	if( pos & 0x01 )
	{
		lcd_text12( 0, 4, gsm_socket[page].ip_addr, strlen( gsm_socket[page].ip_addr ), LCD_MODE_SET );
		sprintf( buf, "端口:%d", gsm_socket[page].port );
		lcd_text12( 0, 18, buf, strlen( buf ), LCD_MODE_SET );
	}else
	{
		if( gsm_socket[page].state == CONNECTED )
		{
			sprintf( buf, "连接%d   已连接", page );
		}else
		{
			sprintf( buf, "连接%d   未连接", page );
		}
		lcd_text12( 0, 4, buf, 14, LCD_MODE_SET );
		lcd_text12( 0, 18, gsm_socket[page].ipstr, strlen( gsm_socket[page].ipstr ), LCD_MODE_SET );
	}
	lcd_update_all( );
}

/**/
static void msg( void *p )
{
}

/**/
static void show( void )
{
	pMenuItem->tick = rt_tick_get( );
	pos				= 0;
	display( );
}

/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
static void keypress( unsigned int key )
{
	switch( key )
	{
		case KEY_MENU:
			pMenuItem = &Menu_2_InforCheck;
			pMenuItem->show( );
			break;
		case KEY_OK:
			break;
		case KEY_UP:
			if( pos == 0 )
			{
				pos =5;
			}
			pos--;
			display( );
			break;
		case KEY_DOWN:
			pos++;
			pos %= 5;
			display( );
			break;
	}
}

MENUITEM Menu_2_8_DnsIpDisplay =
{
	"网络参数",
	8,				  0,
	&show,
	&keypress,
	&timetick_default,
	&msg,
	(void*)0
};

/************************************** The End Of File **************************************/
