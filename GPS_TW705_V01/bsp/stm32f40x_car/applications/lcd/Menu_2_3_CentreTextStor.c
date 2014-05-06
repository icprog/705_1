/************************************************************
 * Copyright (C), 2008-2012,
 * FileName:		// �ļ���
 * Author:			// ����
 * Date:			// ����
 * Description:		// ģ������
 * Version:			// �汾��Ϣ
 * Function List:	// ��Ҫ�������书��
 *     1. -------
 * History:			// ��ʷ�޸ļ�¼
 *     <author>  <time>   <version >   <desc>
 *     David    96/10/12     1.0     build this moudle
 ***********************************************************/
#include "Menu_Include.h"
#include <string.h>
#include "sed1520.h"
#include "menu_include.h"

static TEXTMSG	textmsg;                /*���ݽ�������Ϣ������TEXTMSG_HEAD*/
#if 1
static uint8_t	item_pos		= 0;    /*��ǰҪ���ʵ�*/
static uint8_t	item_pos_read	= 0xff; /*��ǰ�Ѷ�ȡ��*/

/*�鿴ģʽ*/
#define VIEW_NONE	0
#define VIEW_ITEM	1
#define VIEW_DETAIL 2

static uint8_t view_mode = VIEW_NONE;

//static uint8_t body_pos;		/*������ʾ���ݵ�λ��*/

//static DISP_ROW disp_row[32];

static uint8_t	split_lines_count = 0;
static uint8_t	split_lines[32];

static uint8_t	line_pos = 0;         /*��ǰ�к�*/



/*********************************************************************************
  *��������:void analy_textmsg( void )
  *��������:���ı���Ϣtextmsg���ı��ֳ�split_lines_count�У�ÿһ�еĿ�ʼλ��Ϊsplit_lines[i],
  			ͬʱ�޳��˲��ɼ��ַ�
  *��	��:	none
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void analy_textmsg( void )
{
	uint8_t pos = 0;
	uint8_t len;
	char	buf[32];
	while( pos < textmsg.len )
	{
		len = my_strncpy( buf, (char*)( textmsg.body + pos ), 20 );
		if( len )
		{
			split_lines[split_lines_count++]	= pos;
			pos									+= len;
		}else /*�������Ĳ��ɼ��ַ�*/
		{
			pos++;
		}
	}
}

/*
   ��ʾitem 9���ֽڵ�ͷ
   uint32_t	id;        �������
   MYTIME		datetime;  �յ���ʱ��
   uint8_t	len;       ����

   ��11�ֽ���flag,
   ���ݴӵ�12�ֽڿ�ʼ������ 12x12 ÿ��10�� ASC 0612 ÿ�� 20��

 */
static void display_item( )
{
	char	buf1[32], buf2[32];
	uint8_t len;

	if( item_pos >= textmsg_count )
	{
		rt_kprintf("\n%d>item_pos >= textmsg_count");
		return;                     /*item_pos=0���Ի���*/
	}

	/*item_pos ��Χ[0..textmsg_count-1]*/
	if( item_pos_read != item_pos ) /*�б仯��Ҫ��*/
	{
		jt808_textmsg_get( item_pos, &textmsg );
		item_pos_read = item_pos;
	}

	lcd_fill( 0 );
	sprintf( buf1, "[%02d] %02d/%02d/%02d %02d:%02d",
	         item_pos + 1,
	         YEAR( textmsg.datetime ),
	         MONTH( textmsg.datetime ),
	         DAY( textmsg.datetime ),
	         HOUR( textmsg.datetime ),
	         MINUTE( textmsg.datetime ) );

	split_lines_count = 0;
	analy_textmsg( );
	//len = my_strncpy( buf2, (char*)( textmsg.body ), 20 ); /*��ȡ��һ��*/
	len = my_strncpy( buf2, (char*)( textmsg.body + split_lines[0] ), 20 );		/*��ȡ��һ��*/
	if( len )
	{
		lcd_text12( 0, 4, buf1, strlen( buf1 ), LCD_MODE_SET );
		lcd_text12( 0, 16, buf2, len, LCD_MODE_SET );
	}
	lcd_update_all( );
}

/*��ʾ��ϸ����*/
static void display_detail( void )
{
	char	buf1[32], buf2[32];
	int8_t	len1, len2;
	lcd_fill( 0 );

	if( line_pos < split_lines_count )
	{
		len1 = my_strncpy( buf1, (char*)( textmsg.body + split_lines[line_pos] ), 20 );
		lcd_text12( 0, 4, buf1, len1, LCD_MODE_SET );
	}
	if( ( line_pos + 1 ) < split_lines_count )
	{
		len2 = my_strncpy( buf2, (char*)( textmsg.body + split_lines[line_pos + 1] ), 20 );
		lcd_text12( 0, 16, buf2, len2, LCD_MODE_SET );
	}

	lcd_update_all( );
}

/**/
static void msg( void *p )
{
}

/*��ʾ�����·���Ϣ*/
static void show( void )
{
	if( textmsg_count == 0 )
	{
		lcd_fill( 0 );
		lcd_text12( ( 122 - 4 * 12 ) >> 1, 16, "[�޼�¼]", 8, LCD_MODE_SET );
		lcd_update_all( );
		view_mode = VIEW_NONE;
		return;
	}

	pMenuItem->tick = rt_tick_get( );
	item_pos		= 0;
	item_pos_read	= 0xff;
	view_mode		= VIEW_ITEM;
	display_item( );
}

/*������������ΪITEM��DETAIL���ַ�ʽ�鿴*/
static void keypress( unsigned int key )
{
	switch( key )
	{
		case KEY_MENU:
			pMenuItem = &Menu_2_InforCheck;
			pMenuItem->show( );
			break;
		case KEY_OK:                            /*�鿴ģʽ�л�*/
			if( view_mode == VIEW_ITEM )
			{
				view_mode	= VIEW_DETAIL;
				line_pos	= 0;                /*������ʾ*/
				display_detail( );
			}else if( view_mode == VIEW_DETAIL )
			{
				view_mode = VIEW_ITEM;
				display_item( );
			}
			break;
		case KEY_UP:
			if( view_mode == VIEW_ITEM )
			{
				if( item_pos )
				{
					item_pos--; //��Ϊ item_pos=textmsg_count-1 ���Ի���
				}
				display_item( );
			}else if( view_mode == VIEW_DETAIL )
			{
				if( line_pos > 1 )
				{
					line_pos -= 2;
				}
				display_detail( );
			}
			break;
		case KEY_DOWN:
			if( view_mode == VIEW_ITEM )
			{
				if( item_pos < textmsg_count - 1 )
				{
					item_pos++;
				}
				display_item( );
			}else if( view_mode == VIEW_DETAIL )
			{
				if( line_pos < split_lines_count - 2 ) /*һ�α仯����*/
				{
					line_pos += 2;
				}
				display_detail( );
			}
			break;
	}
}

MENUITEM Menu_2_3_CentreTextStor =
{
	"�ı���Ϣ�鿴",
	12,				  0,
	&show,
	&keypress,
	&timetick_default,
	&msg,
	(void*)0
};

#else
static TEXTMSG	textmsg;                /*���ݽ�������Ϣ������TEXTMSG_HEAD*/
static uint8_t	item_pos		= 0;    /*��ǰҪ���ʵ�*/
static uint8_t	item_pos_read	= 0xff; /*��ǰ�Ѷ�ȡ��*/

static uint8_t	line_pos = 0;           /*��ǰ�к�*/

/*�鿴ģʽ*/
#define VIEW_ITEM	0
#define VIEW_DETAIL 0xFF

static uint8_t view_mode = VIEW_ITEM;

#if 0
/*���֧��32�м�¼ÿһ�п�ʼ��λ��*/
static uint8_t	split_lines_pos[32][2];
static uint8_t	split_lines_count = 0;

/*���ݷָ�Ϊ�У���¼���ס���β��ַ*/
static uint8_t split_content( void )
{
	uint8_t count;
	uint8_t pos = 0;
	uint8_t * p;
	int		nbfields = 0;

	uint8_t start = 0;

	memset( split_lines_pos, 0, 64 );

	p		= textmsg.body;
	count	= 0;
	pos		= 0;
	while( pos < textmsg.len )
	{
		if( *p < 0x20 )
		{
			if( count ) /*������*/
			{
				split_lines_pos[nbfields][0]	= start;
				split_lines_pos[nbfields][1]	= pos - 1;
				nbfields++;
				count = 0;
			}
			pos++;
			p++;
		}else
		{
			if( count == 0 )        /*�������ÿ�ʼ*/
			{
				start = pos;
			}
			if( *p > 0x7F )         /*�п����Ǻ��ֵĿ�ʼ�����*/
			{
				if( count == 9 )    /*��������,����һ������*/
				{
					split_lines_pos[nbfields][0]	= start;
					split_lines_pos[nbfields][1]	= pos - 1;
					nbfields++;
					start = pos;    /*����һ��*/
				}
				pos		+= 2;       /*��Ҫ��������*/
				p		+= 2;
				count	+= 2;
				if( count == 10 )
				{
					split_lines_pos[nbfields][0]	= start;
					split_lines_pos[nbfields][1]	= pos - 1;
					nbfields++;
					count = 0;
				}
			}else
			{
				count++;
				if( count == 10 ) /*����*/
				{
					split_lines_pos[nbfields][0]	= start;
					split_lines_pos[nbfields][1]	= pos;
					nbfields++;
					count = 0;
				}
				pos++;
				p++;
			}
		}
	}

	if( count )
	{
		split_lines_pos[nbfields][0]	= start;
		split_lines_pos[nbfields][1]	= pos - 1;
		return nbfields + 1;
	}else
	{
		return nbfields;
	}
}

/**/
static uint8_t get_line( uint8_t pos, char *pout )
{
	char		* pdst = pout;
	signed char len;
	uint8_t		from, to;
	from	= split_lines_pos[pos][0];
	to		= split_lines_pos[pos][1];
	len		= to - from + 1;
	if( len > 0 )
	{
		memcpy( pdst, textmsg.body + from, len );
	}
	pdst[len] = '\0';
	return len;
}

#endif

#if 0

/*���֧��32�м�¼ÿһ�п�ʼ��λ��*/

static struct _disp_row
{
	uint8_t start;
	uint8_t count;
}				disp_row[32];

static uint8_t	split_lines_count = 0;

/*���ݷָ�Ϊ�У���¼���ס���β��ַ*/
static uint8_t split_content( void )
{
	uint8_t count;
	uint8_t pos = 0;
	uint8_t * p;
	uint8_t row = 0;

	uint8_t start = 0;

	p		= textmsg.body;
	count	= 0;
	pos		= 0;

	while( pos < textmsg.len )
	{
		if( *p < 0x20 ) /*�����ַ�������*/
		{
			if( count ) /*������*/
			{
				disp_row[row].start = start;
				disp_row[row].count = count;
				count				= 0;
				row++;
			}
			pos++;
			p++;
		}else
		{
			if( count == 0 )
			{
				start = pos;
			}
			if( *p > 0x7F ) /*����*/
			{
				if( count == 19 )
				{
					disp_row[row].start = start;
					disp_row[row].count = count;
					count				= 0;
					row++;
					start = pos;
				}
				pos		+= 2; /*��Ҫ��������*/
				p		+= 2;
				count	+= 2;
			}else
			{
				count++;
				pos++;
				p++;
			}
			if( count == 20 ) /*����*/
			{
				disp_row[row].start = start;
				disp_row[row].count = count;
				row++;
				count = 0;
			}
		}
		if( row > 31 )
		{
			break;
		}
	}

	if( count )
	{
		disp_row[row].start = start;
		disp_row[row].count = count;
		row++;
	}
	return row;
}

#endif

static DISP_ROW disp_row[32];

static uint8_t	split_lines_count = 0;

/**/
static uint8_t get_line( uint8_t pos, char *pout )
{
	char * pdst = pout;
	memcpy( pdst, textmsg.body + disp_row[pos].start, disp_row[pos].count );
	*( pdst + disp_row[pos].count ) = '\0';
	return disp_row[pos].count;
}

/*
   ��ʾitem 9���ֽڵ�ͷ
   uint32_t	id;        �������
   MYTIME		datetime;  �յ���ʱ��
   uint8_t	len;       ����

   ��11�ֽ���flag,
   ���ݴӵ�12�ֽڿ�ʼ������ 12x12 ÿ��10�� ASC 0612 ÿ�� 20��

 */
static void display_item( )
{
	char	buf1[32], buf2[32];
	uint8_t len, ret;

	if( textmsg_count == 0 )
	{
		lcd_fill( 0 );
		lcd_text12( ( 122 - 4 * 12 ) >> 1, 16, "[�޼�¼]", 8, LCD_MODE_SET );
		lcd_update_all( );
		return;
	}
	if( item_pos >= textmsg_count )
	{
		return;                     /*item_pos=0���Ի���*/
	}
	/*item_pos ��Χ[0..textmsg_count-1]*/
	if( item_pos_read != item_pos ) /*�б仯��Ҫ��*/
	{
		jt808_textmsg_get( item_pos, &textmsg );
		item_pos_read = item_pos;
		//split_lines_count	= split_content( );
		split_lines_count = split_content( textmsg.body, textmsg.len, disp_row, 20 );
#if 1
		rt_kprintf( "split_lines_count=%d\r\n", split_lines_count );
		for( len = 0; len < split_lines_count; len++ )
		{
			ret = get_line( len, buf1 );
			rt_kprintf( "\r\nid=%d len=%d>%s", len, ret, buf1 );
		}
#endif
	}

	lcd_fill( 0 );
	sprintf( buf1, "[%02d] %02d/%02d/%02d %02d:%02d",
	         item_pos + 1,
	         YEAR( textmsg.datetime ),
	         MONTH( textmsg.datetime ),
	         DAY( textmsg.datetime ),
	         HOUR( textmsg.datetime ),
	         MINUTE( textmsg.datetime ) );

	line_pos	= 0; /**/
	len			= get_line( 0, buf2 );
	if( len )
	{
		lcd_text12( 0, 4, buf1, strlen( buf1 ), LCD_MODE_SET );
		lcd_text12( 0, 16, buf2, len, LCD_MODE_SET );
	}
	lcd_update_all( );
}

/*��ʾ��ϸ����*/
static void display_detail( void )
{
	char	buf1[32], buf2[32];
	int8_t	len1, len2;
	lcd_fill( 0 );
	if( line_pos < split_lines_count )
	{
		len1 = get_line( line_pos, buf1 );
		lcd_text12( 0, 4, buf1, len1, LCD_MODE_SET );
	}
	if( ( line_pos + 1 ) < split_lines_count )
	{
		len2 = get_line( line_pos + 1, buf2 );
		lcd_text12( 0, 16, buf2, len2, LCD_MODE_SET );
	}
	lcd_update_all( );
}

/**/
static void msg( void *p )
{
}

/*��ʾ�����·���Ϣ*/
static void show( void )
{
	pMenuItem->tick = rt_tick_get( );
	item_pos		= 0;
	item_pos_read	= 0xff;
	view_mode		= VIEW_ITEM;
	display_item( );
}

/*������������ΪITEM��DETAIL���ַ�ʽ�鿴*/
static void keypress( unsigned int key )
{
	switch( key )
	{
		case KEY_MENU:
			pMenuItem = &Menu_2_InforCheck;
			pMenuItem->show( );
			break;
		case KEY_OK:            /*�鿴ģʽ�л�*/
			view_mode ^= 0xFF;
			if( view_mode == VIEW_ITEM )
			{
				display_item( );
			}else
			{
				line_pos = 0;   /*������ʾ*/
				display_detail( );
			}
			break;
		case KEY_UP:
			if( view_mode == VIEW_ITEM )
			{
				if( item_pos )
				{
					item_pos--; //��Ϊ item_pos=textmsg_count-1 ���Ի���
				}
				display_item( );
			}else
			{
				if( line_pos > 1 )
				{
					line_pos -= 2;
				}
				display_detail( );
			}
			break;
		case KEY_DOWN:
			if( view_mode == VIEW_ITEM )
			{
				if( item_pos < textmsg_count - 1 )
				{
					item_pos++;
				}
				display_item( );
			}else
			{
				if( line_pos < split_lines_count - 2 ) /*һ�α仯����*/
				{
					line_pos += 2;
				}
				display_detail( );
			}
			break;
	}
}

MENUITEM Menu_2_3_CentreTextStor =
{
	"�ı���Ϣ�鿴",
	12,				  0,
	&show,
	&keypress,
	&timetick_default,
	&msg,
	(void*)0
};
#endif
/************************************** The End Of File **************************************/