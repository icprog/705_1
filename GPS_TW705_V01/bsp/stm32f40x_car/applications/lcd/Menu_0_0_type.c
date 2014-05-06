#include  <string.h>
#include "Menu_Include.h"
//#include "Lcd.h"
#include "sed1520.h"

static u8 type_selc_screen=0;
static u8 type_selc_flag=0;
static void msg( void *p)
{

}
static void show(void)
{
lcd_fill(0);
lcd_text12(0, 3,"����һΣ",8,LCD_MODE_INVERT);
lcd_text12(0,18,"����",4,LCD_MODE_SET);
lcd_update_all();
type_selc_flag=1;
type_selc_screen=1;
}


static void keypress(unsigned int key)
{
	switch(key)
		{
		case KEY_MENU:
			break;
		case KEY_OK:
			if(type_selc_screen==1)
				{
				type_selc_screen=2;
				if(type_selc_flag==1)
					{
						lcd_fill(0);
						lcd_text12(9,3,"��������:����һΣ",17,LCD_MODE_SET);
						lcd_text12(6,18,"��ȷ�ϼ�������һ��",18,LCD_MODE_SET);
						lcd_update_all();
						jt808_param.id_0xF00D=1;
					}
				else
					{   
						lcd_fill(0);
						lcd_text12(15,3,"��������:���˳�",15,LCD_MODE_SET);
						lcd_text12(6,18,"��ȷ�ϼ�������һ��",18,LCD_MODE_SET);
						lcd_update_all();
						jt808_param.id_0xF00D=2;
					}
				}
			else if(type_selc_screen==2)
				{
				if(MENU_set_carinfor_flag==1)
					{
					 //--------------------------------------------------------------------------------
                     //  select mode
					 if(jt808_param.id_0xF00D==1)   //����һΣ
					 	{
					 	    /*Vechicle_Info.Link_Frist_Mode=1;
	                        Socket_main_Set("60.28.50.210:9131"); 
							 //--------    �����Ȩ�� -------------------
					        idip("clear");		
							*/

					 	}
					 else
					 if(jt808_param.id_0xF00D==2)  //  ��������ƽ̨
					 	{
					 	
	                        /* Vechicle_Info.Link_Frist_Mode=0; 
							 port_main("7008");
							  //--------    �����Ȩ�� ------------------- 
					         idip("clear");	*/	
					 	}
					param_save(1);
					}
				type_selc_screen=0;
				type_selc_flag=0;
				
				CarSet_0_counter=1;
				
				pMenuItem=&Menu_0_loggingin;
				pMenuItem->show();
				}
			break;
		case KEY_UP:
			if(type_selc_screen==1)
				{
				type_selc_flag=1;
				lcd_fill(0);
				lcd_text12(0, 3,"����һΣ",8,LCD_MODE_INVERT); 
				lcd_text12(0,18,"����",4,LCD_MODE_SET);
				lcd_update_all();
				}
			break;
		case KEY_DOWN:
			if(type_selc_screen==1)
				{
				type_selc_flag=2;
				lcd_fill(0);
				lcd_text12(0, 3,"����һΣ",8,LCD_MODE_SET);
				lcd_text12(0,18,"����",4,LCD_MODE_INVERT);
				lcd_update_all();
				}
			break;
		}
	KeyValue=0;
}


static void timetick(unsigned int systick)
{

}

ALIGN(RT_ALIGN_SIZE)
MENUITEM	Menu_0_0_type=
{
	"����",
	6,0,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};




