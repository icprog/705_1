#include  <string.h>
#include "Menu_Include.h"
#include "sed1520.h"

u8 comfirmation_flag=0;
u8 col_screen=0;
u8 CarBrandCol_Cou=1;

unsigned char car_col[13]={"������ɫ:��ɫ"}; 

void car_col_fun(u8 par)
{                                                      
                                           //������ɫ�����
if(par==1)
	memcpy(Menu_VecLogoColor,"��ɫ",4);     //   1
else if(par==2)
	memcpy(Menu_VecLogoColor,"��ɫ",4);    //   2
else if(par==3)
	memcpy(Menu_VecLogoColor,"��ɫ",4);     //   3
else if(par==4)
	memcpy(Menu_VecLogoColor,"��ɫ",4);    //   4
else if(par==5)
   {	memcpy(Menu_VecLogoColor,"����",4);  par=9; } //   9
   
Menu_color_num=par; 

memcpy(car_col+9,Menu_VecLogoColor,4);  
lcd_fill(0);
lcd_text12(20,10,(char *)car_col,13,LCD_MODE_SET);
lcd_update_all();
}
static void msg( void *p)
{

}
static void show(void)
{
CounterBack=0;
col_screen=1;
car_col_fun(2);// old 1
}


static void keypress(unsigned int key)
{
	switch(key)
		{
		case KEY_MENU:
			if(comfirmation_flag==4)
				{
				pMenuItem=&Menu_1_Idle;
				pMenuItem->show();
				}
			else
				{
				pMenuItem=&Menu_0_loggingin;
				pMenuItem->show();
				}
			col_screen=0;
			CarBrandCol_Cou=1;
			comfirmation_flag=0;
			break;
		case KEY_OK:
             if(col_screen==1)
				{
				if(MENU_set_carinfor_flag==1)
					{
					comfirmation_flag=4;
					// ������ɫ
					if(jt808_param.id_0xF00E==1) 
						jt808_param.id_0x0084=0;
					else
						jt808_param.id_0x0084=Menu_color_num; 								 
					param_save(1);
						
					lcd_fill(0);
					lcd_text12(12,3,"������ɫ�������",16,LCD_MODE_SET);
					lcd_text12(24,18,"���˵�������",12,LCD_MODE_SET);
					lcd_update_all();
					rt_kprintf("\r\n������ɫ������ɣ����˵������أ�%d",jt808_param.id_0x0084);
					}
				else
					{
					col_screen=2;
					CarSet_0_counter=0;//
					menu_color_flag=1;//������ɫ�������
					lcd_fill(0);
					lcd_text12(20,3,(char *)car_col,13,LCD_MODE_SET);
					lcd_text12(12,18,"��ȷ�ϼ��鿴��Ϣ",16,LCD_MODE_SET);
					lcd_update_all();
					}
				}
			else if(col_screen==2)
				{
				menu_color_flag=0;
				
				col_screen=3;
				comfirmation_flag=1;//����������Ϣ��־
				lcd_fill(0);
				if(jt808_param.id_0xF00E==1)
					lcd_text12(0,0,"������",6,LCD_MODE_SET);
				else
					lcd_text12(0,0,(char *)Menu_Car_license,8,LCD_MODE_SET);
				lcd_text12(54,0,(char *)Menu_VechileType,6,LCD_MODE_SET);
				      //====  ���ƺ�δ����=====
                if(jt808_param.id_0xF00E==1)
					 lcd_text12(96,0,(char *)"0",1,LCD_MODE_SET); 
			    else
				     lcd_text12(96,0,(char *)Menu_VecLogoColor,4,LCD_MODE_SET);
				
				lcd_text12(0,12,"SIM����",7,LCD_MODE_SET);
				lcd_text12(43,12,(char *)Menu_sim_Code,12,LCD_MODE_SET);
				lcd_text12(24,23,"ȷ��",4,LCD_MODE_INVERT);
				lcd_text12(72,23,"ȡ��",4,LCD_MODE_SET);
				lcd_update_all();
				}
			else if(comfirmation_flag==1)
				{
				col_screen=0;
				comfirmation_flag=4;
				//�������õ���Ϣ
				lcd_fill(0);
				lcd_text12(18,3,"������������Ϣ",14,LCD_MODE_SET);
				lcd_text12(0,18,"���˵��������������",20,LCD_MODE_SET);
				lcd_update_all();

                            //���ƺ�
                //rt_kprintf("\r\n(������Ϣ)Menu_Car_license=%s",Menu_Car_license);
				memset(jt808_param.id_0x0083,0,sizeof(jt808_param.id_0x0083));
				memcpy(jt808_param.id_0x0083,Menu_Car_license,strlen((const char*)Menu_Car_license));

				// ��������
				memset(jt808_param.id_0xF00A,0,sizeof(jt808_param.id_0xF00A));
				memcpy(jt808_param.id_0xF00A,Menu_VechileType,10);
                                
				 //����VIN
				memset(jt808_param.id_0xF005,0,sizeof(jt808_param.id_0xF005));
				memcpy(jt808_param.id_0xF005,Menu_Vin_Code,17);
                 
				// SIM������
				//memset(JT808Conf_struct.Vech_sim,0,sizeof(JT808Conf_struct.Vech_sim));
				//memcpy(JT808Conf_struct.Vech_sim,Menu_sim_Code,11);
				//-----------------------------------------------------------------------------
			    memset(jt808_param.id_0xF006,0,sizeof(jt808_param.id_0xF006));
			    memcpy(jt808_param.id_0xF006,Menu_sim_Code,12);									 
                               
				// ������ɫ
				if(jt808_param.id_0xF00E==1) 
                     jt808_param.id_0x0084=0;
				else
				     jt808_param.id_0x0084=Menu_color_num;
				//�����������
				jt808_param.id_0xF00F=1;     //  �������Ϊ0 
		      
				//  �洢
				//-------------------------------------------------------------------------------------
                //  select mode
                memset(jt808_param.id_0xF003,0,sizeof(jt808_param.id_0xF003));
				if(get_sock_state(0))
					jt808_state = JT808_REGISTER;

				if(jt808_param.id_0xF00D==1)   //����һΣ
				 	{
				 	   /* Vechicle_Info.Link_Frist_Mode=1;    //������
                        Socket_main_Set("60.28.50.210:9131"); 
					    //--------    �����Ȩ�� --------------------
					     idip("clear");	*/	
					     memset(jt808_param.id_0x0013,0,sizeof(jt808_param.id_0x0013));
					     strcpy(jt808_param.id_0x0013,"60.28.50.210");
					     memcpy(jt808_param.id_0x0017,jt808_param.id_0x0013,sizeof(jt808_param.id_0x0013));
						 jt808_param.id_0x0018 = 9131;
						 jt808_param.id_0xF031 = 9131;
				 	}
				else
				//if(jt808_param.id_0xF00D==2)  //  ��������ƽ̨
				 	{
                        /* Vechicle_Info.Link_Frist_Mode=0;   //dns
						 port_main("7008");
						 //--------    �����Ȩ�� -------------------
					     idip("clear");	*/	
					     jt808_param.id_0xF00D = 2;
					     memset(jt808_param.id_0x0013,0,sizeof(jt808_param.id_0x0013));
					     memset(jt808_param.id_0x0017,0,sizeof(jt808_param.id_0x0017));
					     strcpy(jt808_param.id_0x0013,"jt1.gghypt.net");
					     strcpy(jt808_param.id_0x0017,"jt2.gghypt.net");
						 jt808_param.id_0x0018 = 7008;
						 jt808_param.id_0xF031 = 7008;
				 	}
				//------------------------------------------------------------------------------------				
				param_save(1);
				}
			else if(comfirmation_flag==2)
				{
				col_screen=0;
				comfirmation_flag=3;
				lcd_fill(0);
				lcd_text12(6, 3,"��ȷ���Ƿ���������",18,LCD_MODE_SET);
				lcd_text12(12,18,"��ȷ�ϼ���������",16,LCD_MODE_SET);
				lcd_update_all();
				}
			else if(comfirmation_flag==3)
				{
				col_screen=0;
				comfirmation_flag=0;
				//��������
				pMenuItem=&Menu_0_loggingin;
				pMenuItem->show();
				
				comfirmation_flag=0;
				col_screen=0;
				CarBrandCol_Cou=1;
				}

			break;
		case KEY_UP:
			if(col_screen==1)
				{
				CarBrandCol_Cou--;
				if(CarBrandCol_Cou<1)
					CarBrandCol_Cou=5;
				car_col_fun(CarBrandCol_Cou);
				}
			else if(col_screen==3)
				{
				comfirmation_flag=1;

				lcd_fill(0);
				if(jt808_param.id_0xF00E==1)
					lcd_text12(0,0,"������",6,LCD_MODE_SET);
				else
					lcd_text12(0,0,(char *)Menu_Car_license,8,LCD_MODE_SET);
				lcd_text12(54,0,(char *)Menu_VechileType,6,LCD_MODE_SET);
				      //====  ���ƺ�δ����=====
                if(jt808_param.id_0xF00E==1)
					 lcd_text12(96,0,(char *)"0",1,LCD_MODE_SET); 
			    else
				     lcd_text12(96,0,(char *)Menu_VecLogoColor,4,LCD_MODE_SET);
				
				lcd_text12(0,12,"SIM����",7,LCD_MODE_SET);
				lcd_text12(43,12,(char *)Menu_sim_Code,12,LCD_MODE_SET);
				lcd_text12(24,23,"ȷ��",4,LCD_MODE_INVERT);
				lcd_text12(72,23,"ȡ��",4,LCD_MODE_SET);
				lcd_update_all();
				}

			break;
		case KEY_DOWN:
			if(col_screen==1)
				{
				CarBrandCol_Cou++;
				if(CarBrandCol_Cou>5)
					CarBrandCol_Cou=1;
				car_col_fun(CarBrandCol_Cou);
				}
			else if(col_screen==3)
				{
				comfirmation_flag=2;
				lcd_fill(0);
				if(jt808_param.id_0xF00E==1)
					lcd_text12(0,0,"������",6,LCD_MODE_SET);
				else
					lcd_text12(0,0,(char *)Menu_Car_license,8,LCD_MODE_SET);
				lcd_text12(54,0,(char *)Menu_VechileType,6,LCD_MODE_SET);
				      //====  ���ƺ�δ����=====
                if(jt808_param.id_0xF00E==1)
					 lcd_text12(96,0,(char *)"0",1,LCD_MODE_SET); 
			    else
				     lcd_text12(96,0,(char *)Menu_VecLogoColor,4,LCD_MODE_SET);
				
				lcd_text12(0,12,"SIM����",7,LCD_MODE_SET);
				lcd_text12(43,12,(char *)Menu_sim_Code,12,LCD_MODE_SET);
				lcd_text12(24,23,"ȷ��",4,LCD_MODE_SET);
				lcd_text12(72,23,"ȡ��",4,LCD_MODE_INVERT);
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
MENUITEM	Menu_0_5_Colour=
{
"������ɫ����",
	12,0,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};


