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

#include "camera.h"


/*
   要能查看记录?

   图片记录
    获取图片记录数据 允许导出或发送
   拍照上传
   当前拍照
 */
static int16_t pos;

#define SCR_PHOTO_MENU				0
#define SCR_PHOTO_SELECT_ITEM		1
#define SCR_PHOTO_SELECT_DETAILED	2
#define SCR_PHOTO_TAKE				3

static uint8_t	scr_mode = SCR_PHOTO_MENU; /*当前显示的界面状态*/

static uint8_t	*pHead		= RT_NULL;
static uint16_t pic_count	= 0;

/**/
static void display( void )
{
	uint16_t			pic_page_start; /*每四个图片记录为一个page*/
	uint8_t				buf[32], buf_time[16];
	uint16_t			i;
	MYTIME				t;
	TypeDF_PackageHead	* pcurrhead;

	lcd_fill( 0 );
	switch( scr_mode )
	{
		case SCR_PHOTO_MENU:
			pos &= 0x01;
			lcd_text12( 5, 4, "1.图片记录", 10, 3 - pos * 2 );
			lcd_text12( 5, 18, "2.拍照上传", 10, pos * 2 + 1 );
			break;
		case SCR_PHOTO_SELECT_ITEM:
			if( pic_count ) /*有图片*/
			{
				if( pos >= pic_count )
				{
					pos = 0;
				}
				if( pos < 0 )
				{
					pos = pic_count - 1;
				}
				pic_page_start = pos & 0xFFFC; /*每4个1组*/
				for( i = pic_page_start; i < pic_page_start + 4; i++ )
				{
					if( i >= pic_count )
					{
						break;
					}

					pcurrhead	= (TypeDF_PackageHead*)( pHead + i * sizeof( TypeDF_PackageHead ) );
					t			= pcurrhead->Time;

					sprintf( buf, "%02d>%02d-%02d-%02d %02d:%02d:%02d",
					         i+1, YEAR( t ), MONTH( t ), DAY( t ), HOUR( t ), MINUTE( t ), SEC( t ));
					if( i == pos )
					{
						lcd_asc0608( 0, 8 * ( i & 0x03 ), buf, LCD_MODE_INVERT );
					} else
					{
						lcd_asc0608( 0, 8 * ( i & 0x03 ), buf, LCD_MODE_SET );
					}
				}
			}else /*没有图片*/
			{
				lcd_text12( 25, 12, "没有图片记录", 12, LCD_MODE_SET );
			}
			break;
		case SCR_PHOTO_SELECT_DETAILED:/*显示图片详细信息*/
			pcurrhead	= (TypeDF_PackageHead*)( pHead + pos * sizeof( TypeDF_PackageHead ) );
			t			= pcurrhead->Time;
			
			sprintf( buf, "%02d-%02d-%02d %02d:%02d:%02d",
					 YEAR( t ), MONTH( t ), DAY( t ), HOUR( t ), MINUTE( t ), SEC( t ));
			lcd_asc0608( 0, 0, buf, LCD_MODE_SET );

			sprintf(buf,"chn=%d trig=%d del=%d",pcurrhead->Channel_ID,pcurrhead->TiggerStyle,pcurrhead->State);
			lcd_asc0608( 0, 8, buf, LCD_MODE_SET );
			sprintf(buf,"size=%d",pcurrhead->Len-64);
			lcd_asc0608( 0, 16, buf, LCD_MODE_SET );
			lcd_asc0608(70,24,"usb",LCD_MODE_SET);
			lcd_asc0608(104,24,"rep",LCD_MODE_SET);
		
			break;
		case SCR_PHOTO_TAKE:
			lcd_text12( 20, 12, "拍照上传中...", 12, LCD_MODE_SET );
			break;
	}
	lcd_update_all( );
}

/*处理拍照及上传的过程*/
static void msg( void *p )
{
	pMenuItem->tick = rt_tick_get( );
}

/**/
static void show( void )
{
	pMenuItem->tick = rt_tick_get( );
	pos				= 0;
	scr_mode		= SCR_PHOTO_MENU;
	display( );
}

/*按键处理，拍照或上传过程中如何判断?*/
static void keypress( unsigned int key )
{
	switch( key )
	{
		case KEY_MENU:

			if( scr_mode == SCR_PHOTO_MENU )
			{
				pMenuItem = &Menu_3_InforInteract;
				pMenuItem->show( );
				break;
			}
			if( scr_mode == SCR_PHOTO_SELECT_ITEM ) /*回退到主菜单*/
			{
				scr_mode = SCR_PHOTO_MENU;
				rt_free( pHead );
				pHead = RT_NULL;
			}else if( scr_mode == SCR_PHOTO_SELECT_DETAILED) /*回退到拍照照片条目显示*/
			{
				scr_mode = SCR_PHOTO_SELECT_ITEM;
			}
			else if(scr_mode == SCR_PHOTO_TAKE)
			{
				scr_mode = SCR_PHOTO_MENU;
			}
			display();
			break;
		case KEY_OK:
			if( scr_mode == SCR_PHOTO_MENU )
			{
				if( pos == 0 ) /*图片记录*/
				{
					pic_count = 0;
					rt_free(pHead);
					pHead == RT_NULL;
					pHead		= Cam_Flash_SearchPicHead( 0x00000000, 0xFFFFFFFF, 0, 0xFF, &pic_count ,BIT(0));
					scr_mode	= SCR_PHOTO_SELECT_ITEM;
					
				}else /*图片拍照*/
				{
					scr_mode	= SCR_PHOTO_TAKE;
					if(Cam_get_state() == 0)
						Cam_takepic(0xFF,1,1,Cam_TRIGGER_OTHER);
				}
			}else if( scr_mode == SCR_PHOTO_SELECT_ITEM )
			{
				if(pic_count)
				{
					scr_mode = SCR_PHOTO_SELECT_DETAILED;
				}
			}
			else if(scr_mode == SCR_PHOTO_SELECT_DETAILED)
			{
				;
			}
			else if(scr_mode == SCR_PHOTO_TAKE)
			{
				scr_mode = SCR_PHOTO_MENU;
			}
			display( );
			break;
		case KEY_UP:
			pos--;
			display( );
			break;
		case KEY_DOWN:
			pos++;
			display( );
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
	
	MENUITEM *tmp;
	if( ( tick - pMenuItem->tick ) >= 100 * 30 )
	{
		
		if( pMenuItem->parent != (void*)0 )
		{
			tmp=pMenuItem->parent;
			pMenuItem->parent=(void*)0;
			pMenuItem=tmp;
		}else
		{
			pMenuItem = &Menu_1_Idle;
		}
		rt_free(pHead);
		pHead == RT_NULL;
		pMenuItem->show( );
	}
}


MENUITEM Menu_3_4_Multimedia =
{
	"多媒体信息",
	10,				  0,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};

/************************************** The End Of File **************************************/
