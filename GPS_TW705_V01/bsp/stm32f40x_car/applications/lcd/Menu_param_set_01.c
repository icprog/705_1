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
#include "sed1520.h"

///功能为清空参数和数据

static int16_t 	pos;
static uint8_t	file_menu1 = 0; 		/*当前显示的界面状态*/


/**/
static void display( void )
{
	lcd_fill( 0 );
	switch( file_menu1 )
	{
		case 0:
			lcd_text12( 5,  4, "1.清空参数", 10, 3 - pos * 2 );
			lcd_text12( 5, 18, "2.清空数据", 10, pos * 2 + 1 );
			break;
		case 1:
			if(pos==0)
				{
				lcd_text12( 10, 10, "按确认键清空参数", 16, LCD_MODE_SET );
				}
			else
				{
				lcd_text12( 10, 10, "按确认键清空数据", 16, LCD_MODE_SET );
				}
			break;
		case 2:
			if(pos==0)
				{
				lcd_text12( 20, 10, "参数已经清空", 12, LCD_MODE_SET );
				}
			else
				{
				lcd_text12( 20, 10, "数据已经清空", 12, LCD_MODE_SET );
				reset(5);
				}
			break;
		default :break;
	}
	lcd_update_all( );
}

static void msg( void *p )
{
	pMenuItem->tick = rt_tick_get( );
}

/**/
static void show( void )
{
	pMenuItem->tick = rt_tick_get( );
	pos				= 0;
	file_menu1		= 0;
	display( );
}

/*按键处理，拍照或上传过程中如何判断?*/
static void keypress( unsigned int key )
{
	switch( key )
	{
		case KEY_MENU:
			if(file_menu1)
				{
				show();
				}
			else
				{
				pMenuItem = &Menu_1_Idle;
				pMenuItem->show( );
				}
			break;
		case KEY_OK:
			if(pos == 0)
			{
				if(file_menu1==1)
				{
					factory(1);
				}
			}
			else
			{
				if(file_menu1==1)
				{
					lcd_fill( 0 );
					lcd_text12( 20, 10, "数据清空中...", 13, LCD_MODE_SET );
					lcd_update_all( );
					factory(2);
					pMenuItem->tick = rt_tick_get();
				}
			}
			if(file_menu1<2)
				file_menu1++;
			else
				file_menu1=2;
			
			display( );
			break;
		case KEY_UP:
		case KEY_DOWN:
			if(file_menu1 == 0)
			{
				pos^=BIT(0);
				pos&=0x01;
				display( );
			}
			break;
	}
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
static void timetick( unsigned int tick )
{
}


MENUITEM Menu_param_set_01 =
{
	"清空参数和数据",
	14,				  0,
	&show,
	&keypress,
	&timetick_default,
	&msg,
	(void*)0
};

/************************************** The End Of File **************************************/

