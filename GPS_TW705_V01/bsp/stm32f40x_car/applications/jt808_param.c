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

#include "jt808_param.h"
#include "sst25.h"
#include "jt808.h"
#include <finsh.h>
#include "string.h"
#include "jt808_gps.h"

#define TYPE_BYTE	0x01                /*�̶�Ϊ1�ֽ�,С�˶���*/
#define TYPE_WORD	0x02                /*�̶�Ϊ2�ֽ�,С�˶���*/
#define TYPE_DWORD	0x04                /*�̶�Ϊ4�ֽ�,С�˶���*/
#define TYPE_STR	0x80                /*�̶�Ϊ32�ֽ�,����˳��*/
#define TYPE_CAN	0x48                /*�̶�Ϊ8�ֽ�,��ǰ�洢CAN_ID����*/

#define MAIN_PARA_CRC_ERROR				0x01			///�豸������CRC����
#define MAIN_PARA_VER_UP				0x02			///�豸�������汾����
#define MAIN_PARA_FORMAT_ERR			0x04			///�豸��������ʽ����


#define JT808_FLASH_FORMA_VER		0x464D5431			///"FMT0",���ÿɼ��ַ���Ϊ����ʾ�ͺ���Զ���޸ķ���
#define JT808_FLASH_DATA_VER		0x30303030			///"0000",���ÿɼ��ַ���Ϊ����ʾ�ͺ���Զ���޸ķ���

JT808_PARAM jt808_param;
//***********************************************************************************************************************
uint8_t	HARD_VER = 1;			///Ӳ���汾
uint8_t SOFT_VER = 102;			///����汾
/*  ����汾����˵��:
	101:����:2014-02-26.	
		˵��:��һ�汾�ĳ��򷢲����ð汾����ֻ�������С����5̨�豸ʹ�ã�û�о������ڼӵ���ԡ�
	102:����:2014-03-10.	
		˵��:����ʱ�����˿���gps��gsm��Դ����ֹ��Դ������ɶ�ȡflash����(101�汾��ع����в���ʱ��ʧ�˲���)����MMA845��������
			���ն����ȼ�����Ϊ�����ж�����͵ġ�

*/
//***********************************************************************************************************************


#if 0
JT808_PARAM jt808_param =
{
	0x13091405,                         /*0x0000 �汾*/
	50,                                 /*0x0001 �������ͼ��*/
	10,                                 /*0x0002 TCPӦ��ʱʱ��*/
	3,                                  /*0x0003 TCP��ʱ�ش�����*/
	10,                                 /*0x0004 UDPӦ��ʱʱ��*/
	5,                                  /*0x0005 UDP��ʱ�ش�����*/
	60,                                 /*0x0006 SMS��ϢӦ��ʱʱ��*/
	3,                                  /*0x0007 SMS��Ϣ�ش�����*/
	"CMNET",                            /*0x0010 ��������APN*/
	"",                                 /*0x0011 �û���*/
	"",                                 /*0x0012 ����*/
	//"jt1.gghypt.net",                   /*0x0013 ����������ַ*/
	"60.28.50.210",
	"CMNET",                            /*0x0014 ����APN*/
	"",                                 /*0x0015 �����û���*/
	"",                                 /*0x0016 ��������*/
	"jt2.gghypt.net",                   /*0x0017 ���ݷ�������ַ��ip������*/
	//7008,                               /*0x0018 TCP�˿�*/
	9131,
	5678,                               /*0x0019 UDP�˿�*/
	"www.google.com",                   /*0x001A ic������������ַ��ip������*/
	9901,                               /*0x001B ic��������TCP�˿�*/
	8875,                               /*0x001C ic��������UDP�˿�*/
	"www.google.com",                   /*0x001D ic�����ݷ�������ַ��ip������*/
	0,                                  /*0x0020 λ�û㱨����*/
	1,                                  /*0x0021 λ�û㱨����*/
	30,                                 /*0x0022 ��ʻԱδ��¼�㱨ʱ����*/
	120,                                /*0x0027 ����ʱ�㱨ʱ����*/
	5,                                  /*0x0028 ��������ʱ�㱨ʱ����*/
	30,                                 /*0x0029 ȱʡʱ��㱨���*/
	500,                                /*0x002c ȱʡ����㱨���*/
	1000,                               /*0x002d ��ʻԱδ��¼�㱨������*/
	1000,                               /*0x002e ����ʱ����㱨���*/
	100,                                /*0x002f ����ʱ����㱨���*/
	270,                                /*0x0030 �յ㲹���Ƕ�*/
	500,                                /*0x0031 ����Χ���뾶���Ƿ�λ����ֵ������λΪ��*/
	"1008611",                          /*0x0040 ���ƽ̨�绰����*/
	"",                                 /*0x0041 ��λ�绰����*/
	"",                                 /*0x0042 �ָ��������õ绰����*/
	"",                                 /*0x0043 ���ƽ̨SMS����*/
	"",                                 /*0x0044 �����ն�SMS�ı���������*/
	5,                                  /*0x0045 �ն˽����绰����*/
	3,                                  /*0x0046 ÿ��ͨ��ʱ��*/
	3,                                  /*0x0047 ����ͨ��ʱ��*/
	"",                                 /*0x0048 �����绰����*/
	"",                                 /*0x0049 ���ƽ̨��Ȩ���ź���*/
	5,                                  /*0x0050 ����������*/
	3,                                  /*0x0051 ���������ı�SMS����*/
	5,                                  /*0x0052 �������տ���*/
	3,                                  /*0x0053 ��������洢��־*/
	5,                                  /*0x0054 �ؼ���־*/
	90,                                 /*0x0055 ����ٶ�kmh*/
	5,                                  /*0x0056 ���ٳ���ʱ��*/
	4 * 60 * 60,                        /*0x0057 ������ʻʱ������*/
	8 * 60 * 60,                        /*0x0058 �����ۼƼ�ʻʱ������*/
	20 * 60,                            /*0x0059 ��С��Ϣʱ��*/
	12 * 60 * 60,                       /*0x005A �ͣ��ʱ��*/
	100,                                /*0x005B ���ٱ���Ԥ����ֵ����λΪ 1/10Km/h */
	90,                                 /*0x005C ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0*/
	0x200a,                             /*0x005D ��ײ������������:*/
	30,                                 /*0x005E �෭�����������ã� �෭�Ƕȣ���λ 1 �ȣ�Ĭ��Ϊ 30 ��*/
	0,                                  /*0x0064 ��ʱ���տ���*/
	0,                                  /*0x0065 �������տ���*/
	3,                                  /*0x0070 ͼ����Ƶ����(1-10)*/
	5,                                  /*0x0071 ����*/
	3,                                  /*0x0072 �Աȶ�*/
	5,                                  /*0x0073 ���Ͷ�*/
	3,                                  /*0x0074 ɫ��*/
	5,                                  /*0x0080 ������̱����0.1km*/
	12,                                 /*0x0081 ʡ��ID*/
	101,                                /*0x0082 ����ID*/
	"��AP6834",                         /*0x0083 ����������*/
	2,                                  /*0x0084 ������ɫ	1��ɫ 2��ɫ 3��ɫ 4��ɫ 9����*/
	0x0f,                               /*0x0090 GNSS ��λģʽ*/
	0x01,                               /*0x0091 GNSS ������*/
	0x01,                               /*0x0092 GNSS ģ����ϸ��λ�������Ƶ��*/
	0x01,                               /*0x0093 GNSS ģ����ϸ��λ���ݲɼ�Ƶ��*/
	0x01,                               /*0x0094 GNSS ģ����ϸ��λ�����ϴ���ʽ*/
	0x01,                               /*0x0095 GNSS ģ����ϸ��λ�����ϴ�����*/
	0,                                  /*0x0100 CAN ����ͨ�� 1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�*/
	0,                                  /*0x0101 CAN ����ͨ�� 1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�*/
	0,                                  /*0x0102 CAN ����ͨ�� 2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�*/
	0,                                  /*0x0103 CAN ����ͨ�� 2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0110 CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0111 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0112 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0113 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0114 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0115 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0116 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0117 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0118 ����CAN ���� ID �����ɼ�����*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0119 ����CAN ���� ID �����ɼ�����*/

	"70420",                            /*0xF000 ������ID  70420*/
	"TW703",                            /*0xF001 �ն��ͺ�TW703-BD*/
	"0614100",                          /*0xF002 �ն�ID*/
	"12345",                            /*0xF003 ��Ȩ��*/
	0x07,                               /*0xF004 �ն�����*/
	"0000000000000000",                 /*0xF005 ����VIN*/
	"012002444390",                     /*0xF006 DeviceID*/
	"��ʻ֤����",                       /*0xF007 ��ʻ֤����*/
	"����",                             /*0xF008 ��ʻԱ����*/
	"120104197712015381",               /*0xF009 ��ʻ֤����*/
	"���ͻ���",                         /*0xF00A ��������*/
	"δ֪",                             /*0xF00B ��ҵ�ʸ�֤*/
	"δ֪      ",                       /*0xF00C ��֤����*/

	"1.00",                             /*0xF010 ����汾��*/
	"1.00",                             /*0xF011 Ӳ���汾��*/
	"TJ.GT",                            /*0xF012 ���ۿͻ�����*/
	0x3020,                             /*0xF013 ����ģ���ͺ�0,δȷ�� ,0x3020 Ĭ�� 0x3017*/

	0,                                  /*0xF020 �����*/
	0,                                  /*0xF021 ����״̬*/

	0x35DECC80,                         /*0xF030 ��¼�ǳ��ΰ�װʱ��,mytime��ʽ*/
	0,                                  /*id_0xF031;      ��ʼ���*/
	6250,                               /*id_0xF032;      ��������ϵ��*/

	6,                                  //line_space;               //�м��
	0,                                  //margin_left;				//��߽�
	0,                                  //margin_right;				//�ұ߽�
	1,                                  //step_delay;               //������ʱ,Ӱ���м��
	1,                                  //gray_level;               //�Ҷȵȼ�,����ʱ��
	5,                                  //heat_delay[0];			//������ʱ
	10,                                 //heat_delay[1];			//������ʱ
	15,                                 //heat_delay[2];			//������ʱ
	20,                                 //heat_delay[3];			//������ʱ
};
#endif
JT808_PARAM_BK 	jt808_param_bk;		///����bksram�е����в������ò����ı���ʱ��Ϊ8Сʱ����
STYLE_JT808_DATA	jt808_data;		///���泵����Ҫ���ڱ��������
TYPE_CAR_DATA	car_data;			///������ص�ǰ����
uint32_t		param_factset_tick=0;	///����豸������ʱ�䣬��λΪtick


#define FLAG_DISABLE_REPORT_INVALID 1   /*�豸�Ƿ�*/

#define FLAG_DISABLE_REPORT_AREA 2      /*�����ڽ�ֹ�ϱ�*/

//static uint32_t flag_disable_report = 0;    /*��ֹ�ϱ��ı�־λ*/

#define ID_LOOKUP( id, type ) { id, type, (uint8_t*)&( jt808_param.id_ ## id ) }

struct _tbl_id_lookup
{
	uint16_t	id;
	uint8_t		type;
	uint8_t		* val;
} tbl_id_lookup[] = {
	ID_LOOKUP( 0x0000, TYPE_STR ),    	//uint32_t  id_0x0000;   /*0x0000 �汾*/
	ID_LOOKUP( 0x0001, TYPE_DWORD ),    //uint32_t  id_0x0001;   /*0x0001 �������ͼ��*/
	ID_LOOKUP( 0x0002, TYPE_DWORD ),    //uint32_t  id_0x0002;   /*0x0002 TCPӦ��ʱʱ��*/
	ID_LOOKUP( 0x0003, TYPE_DWORD ),    //uint32_t  id_0x0003;   /*0x0003 TCP��ʱ�ش�����*/
	ID_LOOKUP( 0x0004, TYPE_DWORD ),    //uint32_t  id_0x0004;   /*0x0004 UDPӦ��ʱʱ��*/
	ID_LOOKUP( 0x0005, TYPE_DWORD ),    //uint32_t  id_0x0005;   /*0x0005 UDP��ʱ�ش�����*/
	ID_LOOKUP( 0x0006, TYPE_DWORD ),    //uint32_t  id_0x0006;   /*0x0006 SMS��ϢӦ��ʱʱ��*/
	ID_LOOKUP( 0x0007, TYPE_DWORD ),    //uint32_t  id_0x0007;   /*0x0007 SMS��Ϣ�ش�����*/
	ID_LOOKUP( 0x0010, TYPE_STR ),      //char   id_0x0010[32];  /*0x0010 ��������APN*/
	ID_LOOKUP( 0x0011, TYPE_STR ),      //char   id_0x0011[32];  /*0x0011 �û���*/
	ID_LOOKUP( 0x0012, TYPE_STR ),      //char   id_0x0012[32];  /*0x0012 ����*/
	ID_LOOKUP( 0x0013, TYPE_STR ),      //char   id_0x0013[32];  /*0x0013 ����������ַ*/
	ID_LOOKUP( 0x0014, TYPE_STR ),      //char   id_0x0014[32];  /*0x0014 ����APN*/
	ID_LOOKUP( 0x0015, TYPE_STR ),      //char   id_0x0015[32];  /*0x0015 �����û���*/
	ID_LOOKUP( 0x0016, TYPE_STR ),      //char   id_0x0016[32];  /*0x0016 ��������*/
	ID_LOOKUP( 0x0017, TYPE_STR ),      //char   id_0x0017[32];  /*0x0017 ���ݷ�������ַ��ip������*/
	ID_LOOKUP( 0x0018, TYPE_DWORD ),    //uint32_t  id_0x0018;   /*0x0018 TCP�˿�*/
	ID_LOOKUP( 0x0019, TYPE_DWORD ),    //uint32_t  id_0x0019;   /*0x0019 UDP�˿�*/
	ID_LOOKUP( 0x001A, TYPE_STR ),      //char   id_0x001A[32];  /*0x001A ic������������ַ��ip������*/
	ID_LOOKUP( 0x001B, TYPE_DWORD ),    //uint32_t  id_0x001B;   /*0x001B ic��������TCP�˿�*/
	ID_LOOKUP( 0x001C, TYPE_DWORD ),    //uint32_t  id_0x001C;   /*0x001C ic��������UDP�˿�*/
	ID_LOOKUP( 0x001D, TYPE_STR ),      //char   id_0x001D[32];  /*0x001D ic�����ݷ�������ַ��ip������*/
	ID_LOOKUP( 0x0020, TYPE_DWORD ),    //uint32_t  id_0x0020;   /*0x0020 λ�û㱨����*/
	ID_LOOKUP( 0x0021, TYPE_DWORD ),    //uint32_t  id_0x0021;   /*0x0021 λ�û㱨����*/
	ID_LOOKUP( 0x0022, TYPE_DWORD ),    //uint32_t  id_0x0022;   /*0x0022 ��ʻԱδ��¼�㱨ʱ����*/
	ID_LOOKUP( 0x0027, TYPE_DWORD ),    //uint32_t  id_0x0027;   /*0x0027 ����ʱ�㱨ʱ����*/
	ID_LOOKUP( 0x0028, TYPE_DWORD ),    //uint32_t  id_0x0028;   /*0x0028 ��������ʱ�㱨ʱ����*/
	ID_LOOKUP( 0x0029, TYPE_DWORD ),    //uint32_t  id_0x0029;   /*0x0029 ȱʡʱ��㱨���*/
	ID_LOOKUP( 0x002C, TYPE_DWORD ),    //uint32_t  id_0x002C;   /*0x002c ȱʡ����㱨���*/
	ID_LOOKUP( 0x002D, TYPE_DWORD ),    //uint32_t  id_0x002D;   /*0x002d ��ʻԱδ��¼�㱨������*/
	ID_LOOKUP( 0x002E, TYPE_DWORD ),    //uint32_t  id_0x002E;   /*0x002e ����ʱ����㱨���*/
	ID_LOOKUP( 0x002F, TYPE_DWORD ),    //uint32_t  id_0x002F;   /*0x002f ����ʱ����㱨���*/
	ID_LOOKUP( 0x0030, TYPE_DWORD ),    //uint32_t  id_0x0030;   /*0x0030 �յ㲹���Ƕ�*/
	ID_LOOKUP( 0x0031, TYPE_DWORD ),    //uint16_t  id_0x0031;   /*0x0031 ����Χ���뾶���Ƿ�λ����ֵ������λΪ��*/
	ID_LOOKUP( 0x0040, TYPE_STR ),      //char   id_0x0040[32];  /*0x0040 ���ƽ̨�绰����*/
	ID_LOOKUP( 0x0041, TYPE_STR ),      //char   id_0x0041[32];  /*0x0041 ��λ�绰����*/
	ID_LOOKUP( 0x0042, TYPE_STR ),      //char   id_0x0042[32];  /*0x0042 �ָ��������õ绰����*/
	ID_LOOKUP( 0x0043, TYPE_STR ),      //char   id_0x0043[32];  /*0x0043 ���ƽ̨SMS����*/
	ID_LOOKUP( 0x0044, TYPE_STR ),      //char   id_0x0044[32];  /*0x0044 �����ն�SMS�ı���������*/
	ID_LOOKUP( 0x0045, TYPE_DWORD ),    //uint32_t  id_0x0045;   /*0x0045 �ն˽����绰����*/
	ID_LOOKUP( 0x0046, TYPE_DWORD ),    //uint32_t  id_0x0046;   /*0x0046 ÿ��ͨ��ʱ��*/
	ID_LOOKUP( 0x0047, TYPE_DWORD ),    //uint32_t  id_0x0047;   /*0x0047 ����ͨ��ʱ��*/
	ID_LOOKUP( 0x0048, TYPE_STR ),      //char   id_0x0048[32];  /*0x0048 �����绰����*/
	ID_LOOKUP( 0x0049, TYPE_STR ),      //char   id_0x0049[32];  /*0x0049 ���ƽ̨��Ȩ���ź���*/
	ID_LOOKUP( 0x0050, TYPE_DWORD ),    //uint32_t  id_0x0050;   /*0x0050 ����������*/
	ID_LOOKUP( 0x0051, TYPE_DWORD ),    //uint32_t  id_0x0051;   /*0x0051 ���������ı�SMS����*/
	ID_LOOKUP( 0x0052, TYPE_DWORD ),    //uint32_t  id_0x0052;   /*0x0052 �������տ���*/
	ID_LOOKUP( 0x0053, TYPE_DWORD ),    //uint32_t  id_0x0053;   /*0x0053 ��������洢��־*/
	ID_LOOKUP( 0x0054, TYPE_DWORD ),    //uint32_t  id_0x0054;   /*0x0054 �ؼ���־*/
	ID_LOOKUP( 0x0055, TYPE_DWORD ),    //uint32_t  id_0x0055;   /*0x0055 ����ٶ�kmh*/
	ID_LOOKUP( 0x0056, TYPE_DWORD ),    //uint32_t  id_0x0056;   /*0x0056 ���ٳ���ʱ��*/
	ID_LOOKUP( 0x0057, TYPE_DWORD ),    //uint32_t  id_0x0057;   /*0x0057 ������ʻʱ������*/
	ID_LOOKUP( 0x0058, TYPE_DWORD ),    //uint32_t  id_0x0058;   /*0x0058 �����ۼƼ�ʻʱ������*/
	ID_LOOKUP( 0x0059, TYPE_DWORD ),    //uint32_t  id_0x0059;   /*0x0059 ��С��Ϣʱ��*/
	ID_LOOKUP( 0x005A, TYPE_DWORD ),    //uint32_t  id_0x005A;   /*0x005A �ͣ��ʱ��*/
	ID_LOOKUP( 0x005B, TYPE_WORD ),     //uint16_t  id_0x005B;   /*0x005B ���ٱ���Ԥ����ֵ����λΪ 1/10Km/h */
	ID_LOOKUP( 0x005C, TYPE_WORD ),     //uint16_t  id_0x005C;   /*0x005C ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0*/
	ID_LOOKUP( 0x005D, TYPE_WORD ),     //uint16_t  id_0x005D;   /*0x005D ��ײ������������:b7..0����ײʱ��(4ms) b15..8����ײ���ٶ�(0.1g) 0-79 ֮�䣬Ĭ��Ϊ10 */
	ID_LOOKUP( 0x005E, TYPE_WORD ),     //uint16_t  id_0x005E;   /*0x005E �෭�����������ã� �෭�Ƕȣ���λ 1 �ȣ�Ĭ��Ϊ 30 ��*/
	ID_LOOKUP( 0x0064, TYPE_DWORD ),    //uint32_t  id_0x0064;   /*0x0064 ��ʱ���տ���*/
	ID_LOOKUP( 0x0065, TYPE_DWORD ),    //uint32_t  id_0x0065;   /*0x0065 �������տ���*/
	ID_LOOKUP( 0x0070, TYPE_DWORD ),    //uint32_t  id_0x0070;   /*0x0070 ͼ����Ƶ����(1-10)*/
	ID_LOOKUP( 0x0071, TYPE_DWORD ),    //uint32_t  id_0x0071;   /*0x0071 ����*/
	ID_LOOKUP( 0x0072, TYPE_DWORD ),    //uint32_t  id_0x0072;   /*0x0072 �Աȶ�*/
	ID_LOOKUP( 0x0073, TYPE_DWORD ),    //uint32_t  id_0x0073;   /*0x0073 ���Ͷ�*/
	ID_LOOKUP( 0x0074, TYPE_DWORD ),    //uint32_t  id_0x0074;   /*0x0074 ɫ��*/
	ID_LOOKUP( 0x0080, TYPE_DWORD ),    //uint32_t  id_0x0080;   /*0x0080 ������̱����0.1km*/
	ID_LOOKUP( 0x0081, TYPE_WORD ),     //uint16_t  id_0x0081;   /*0x0081 ʡ��ID*/
	ID_LOOKUP( 0x0082, TYPE_WORD ),     //uint16_t  id_0x0082;   /*0x0082 ����ID*/
	ID_LOOKUP( 0x0083, TYPE_STR ),      //char   id_0x0083[32];  /*0x0083 ����������*/
	ID_LOOKUP( 0x0084, TYPE_BYTE ),     //uint8_t		 id_0x0084;      /*0x0084 ������ɫ	1��ɫ 2��ɫ 3��ɫ 4��ɫ 9����*/
	ID_LOOKUP( 0x0090, TYPE_BYTE ),     //uint8_t		 id_0x0090;      /*0x0090 GNSS ��λģʽ*/
	ID_LOOKUP( 0x0091, TYPE_BYTE ),     //uint8_t		 id_0x0091;      /*0x0091 GNSS ������*/
	ID_LOOKUP( 0x0092, TYPE_BYTE ),     //uint8_t		 id_0x0092;      /*0x0092 GNSS ģ����ϸ��λ�������Ƶ��*/
	ID_LOOKUP( 0x0093, TYPE_DWORD ),    //uint32_t  id_0x0093;   /*0x0093 GNSS ģ����ϸ��λ���ݲɼ�Ƶ��*/
	ID_LOOKUP( 0x0094, TYPE_BYTE ),     //uint8_t		 id_0x0094;      /*0x0094 GNSS ģ����ϸ��λ�����ϴ���ʽ*/
	ID_LOOKUP( 0x0095, TYPE_DWORD ),    //uint32_t  id_0x0095;   /*0x0095 GNSS ģ����ϸ��λ�����ϴ�����*/
	ID_LOOKUP( 0x0100, TYPE_DWORD ),    //uint32_t  id_0x0100;   /*0x0100 CAN ����ͨ�� 1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�*/
	ID_LOOKUP( 0x0101, TYPE_WORD ),     //uint16_t  id_0x0101;   /*0x0101 CAN ����ͨ�� 1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�*/
	ID_LOOKUP( 0x0102, TYPE_DWORD ),    //uint32_t  id_0x0102;   /*0x0102 CAN ����ͨ�� 2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�*/
	ID_LOOKUP( 0x0103, TYPE_WORD ),     //uint16_t  id_0x0103;   /*0x0103 CAN ����ͨ�� 2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�*/
	ID_LOOKUP( 0x0110, TYPE_BYTE | 8 ), //uint8_t		 id_0x0110[8];	 /*0x0110 CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0111, TYPE_BYTE | 8 ), //uint8_t		 id_0x0111[8];	 /*0x0111 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0112, TYPE_BYTE | 8 ), //uint8_t		 id_0x0112[8];	 /*0x0112 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0113, TYPE_BYTE | 8 ), //uint8_t		 id_0x0113[8];	 /*0x0113 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0114, TYPE_BYTE | 8 ), //uint8_t		 id_0x0114[8];	 /*0x0114 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0115, TYPE_BYTE | 8 ), //uint8_t		 id_0x0115[8];	 /*0x0115 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0116, TYPE_BYTE | 8 ), //uint8_t		 id_0x0116[8];	 /*0x0116 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0117, TYPE_BYTE | 8 ), //uint8_t		 id_0x0117[8];	 /*0x0117 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0118, TYPE_BYTE | 8 ), //uint8_t		 id_0x0118[8];	 /*0x0118 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0x0119, TYPE_BYTE | 8 ), //uint8_t		 id_0x0119[8];	 /*0x0119 ����CAN ���� ID �����ɼ�����*/

	ID_LOOKUP( 0xF000, TYPE_STR ),      //uint8_t		 id_0x0119[8];	 /*0x0119 ����CAN ���� ID �����ɼ�����*/
	ID_LOOKUP( 0xF001, TYPE_STR ),      /*0xF001 �ն��ͺ� 20byte*/
	ID_LOOKUP( 0xF002, TYPE_STR ),      /*0xF002 �ն�ID 7byte*/
	ID_LOOKUP( 0xF003, TYPE_STR ),      /*0xF003 ��Ȩ��*/
	ID_LOOKUP( 0xF004, TYPE_BYTE ),     /*0xF004 �ն�����*/
	ID_LOOKUP( 0xF005, TYPE_STR ),      /*0xF005 ������ʶ,VIN*/
	ID_LOOKUP( 0xF006, TYPE_STR ),      /*0xF006 ������ʶ,MOBILE*/
	ID_LOOKUP( 0xF008, TYPE_STR ),      /*0xF008 ��ʻԱ����*/
	ID_LOOKUP( 0xF009, TYPE_STR ),      /*0xF009 ��ʻ֤����*/
	ID_LOOKUP( 0xF00A, TYPE_STR ),      /*0xF00A ��������*/
	ID_LOOKUP( 0xF00B, TYPE_STR ),      /*0xF00B ��ҵ�ʸ�֤*/
	ID_LOOKUP( 0xF00C, TYPE_STR ),      /*0xF00C ��֤����*/
	ID_LOOKUP( 0xF00D, TYPE_BYTE ), 	//0xF00D ����ģʽ����			0:����ģʽ	 1:����һΣ
	ID_LOOKUP( 0xF00E, TYPE_BYTE ), 	//0xF00E ���޳��ƺ�				0:�޳��ƺ�	 1:�г��ƺţ���Ҫ����
	ID_LOOKUP( 0xF00F, TYPE_BYTE ), 	//0xF00F ������Ϣ�����Ƿ����	0:δ����	 1:�������

	ID_LOOKUP( 0xF010, TYPE_STR ),      /*0xF010 ����汾��*/
	ID_LOOKUP( 0xF011, TYPE_STR ),      /*0xF011 Ӳ���汾��*/

	ID_LOOKUP( 0xF013, TYPE_DWORD ),    /*0xF013 ����ģ���ͺ�*/
		
	ID_LOOKUP( 0xF030, TYPE_DWORD ), 	/*0xF030 ��¼�ǳ��ΰ�װʱ��,mytime��ʽ*/
	ID_LOOKUP( 0xF031, TYPE_DWORD ), 	/*0xF031 ����TCP�˿�1*/
	ID_LOOKUP( 0xF032, TYPE_DWORD ), 	/*0xF032 ��������ϵ��,��Чλ��Ϊ��16λ,��ʾΪÿ����������������λΪ1��ʾ��ҪУ׼��Ϊ0��ʾ����ҪУ׼*/

	ID_LOOKUP( 0xF040, TYPE_BYTE ),     //line_space;               //�м��
	ID_LOOKUP( 0xF041, TYPE_BYTE ),     //margin_left;				//��߽�
	ID_LOOKUP( 0xF042, TYPE_BYTE ),     //margin_right;				//�ұ߽�
	ID_LOOKUP( 0xF043, TYPE_BYTE ),     //step_delay;               //������ʱ,Ӱ���м��
	ID_LOOKUP( 0xF044, TYPE_BYTE ),     //gray_level;               //�Ҷȵȼ�,����ʱ��
	ID_LOOKUP( 0xF045, TYPE_BYTE ),     //heat_delay[0];			//������ʱ
	ID_LOOKUP( 0xF046, TYPE_BYTE ),     //heat_delay[1];			//������ʱ
	ID_LOOKUP( 0xF047, TYPE_BYTE ),     //heat_delay[2];			//������ʱ
	ID_LOOKUP( 0xF048, TYPE_BYTE ),     //heat_delay[3];			//������ʱ
	ID_LOOKUP( 0xF049, TYPE_STR ),     	/*0xF049 ���ݷ�������ַ2*/
	ID_LOOKUP( 0xF04A, TYPE_WORD ),     /*0xF04A ����TCP�˿�2*/
	ID_LOOKUP( 0xF04B, TYPE_STR ),     	/*0xF04B ����������ַ3*/
	ID_LOOKUP( 0xF04C, TYPE_WORD ),     /*0xF04C ����TCP�˿�3*/
};

extern void jt808_report_init( void );
extern void vdr_data_init(void);
extern u16 Cam_Flash_InitPara( u8 printf_info );
extern void printer_load_param( void );
static uint8_t param_save_ex( uint8_t sem_get ,uint8_t save_sequ);

/*********************************************************************************
  *��������:void data_save( void )
  *��������:�����豸״̬���ݵ�flash��
  *��	��:	none
  *��	��:	none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void data_save( void )
{
	if( jt808_status & BIT_STATUS_FIXED )
		{
		jt808_data.id_0xFA00 = utc_now;
		}
	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
	sst25_write_back( ADDR_DF_PARAM_BASE, (uint8_t*)&jt808_data, sizeof(jt808_data) );
	rt_sem_release( &sem_dataflash );
}
FINSH_FUNCTION_EXPORT( data_save, save data);

/*********************************************************************************
  *��������:void data_load( void )
  *��������:��ȡ��Ҫ��ʱ������豸״̬������
  *��	��:	none
  *��	��:	none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void data_load( void )
{
	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
	sst25_read( ADDR_DF_PARAM_BASE, (uint8_t*)&jt808_data, sizeof(jt808_data) );
	if(jt808_data.id_0xFA00 == 0xFFFFFFFF)
		{
		memset(&jt808_data,0,sizeof(jt808_data));
		sst25_write_back( ADDR_DF_PARAM_BASE, (uint8_t*)&jt808_data, sizeof(jt808_data) );
		}
	rt_sem_release( &sem_dataflash );
}


/*********************************************************************************
  *��������:void param_fact_set(void)
  *��������:���豸��Ҫ��������Ϊ��������
  *��	��: none
  *��	��: none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void param_fact_set(void)
{
	uint8_t 	buf[]="MAIN_PARA_FACT_SET!";
	uint32_t	flash_ver;
	uint32_t	data_ver;
	memset((uint8_t *)&jt808_param,0,sizeof(jt808_param));
	flash_ver 	= BYTESWAP4(JT808_FLASH_FORMA_VER);
	data_ver 	= BYTESWAP4(JT808_FLASH_DATA_VER);
	memcpy(jt808_param.id_0x0000,(uint8_t *)&flash_ver,4);
	memcpy(jt808_param.id_0x0000+4,(uint8_t *)&data_ver,4);
	jt808_param.id_0x0001 = 50;
	jt808_param.id_0x0002 = 10;
	jt808_param.id_0x0003 = 3;
	jt808_param.id_0x0004 = 10;
	jt808_param.id_0x0005 = 5;
	jt808_param.id_0x0006 = 60;
	jt808_param.id_0x0007 = 3;
	strcpy(jt808_param.id_0x0010,"CMNET");
	strcpy(jt808_param.id_0x0013,"60.28.50.210");
	strcpy(jt808_param.id_0x0014,"CMNET");
	strcpy(jt808_param.id_0x0017,"jt1.gghypt.net");
	jt808_param.id_0x0018 = 9131;
	jt808_param.id_0x0019 = 7008;
	strcpy(jt808_param.id_0x001A,"www.google.com");
	jt808_param.id_0x001B = 9901;
	jt808_param.id_0x001C = 8875;
	strcpy(jt808_param.id_0x001D,"www.google.com");
	jt808_param.id_0x0021 = 1;
	jt808_param.id_0x0022 = 30;
	jt808_param.id_0x0027 = 30;
	jt808_param.id_0x0028 = 5;
	jt808_param.id_0x0029 = 30;
	jt808_param.id_0x002C = 500;
	jt808_param.id_0x002D = 1000;
	jt808_param.id_0x002E = 1000;
	jt808_param.id_0x002F = 100;
	jt808_param.id_0x0030 = 270;
	jt808_param.id_0x0031 = 500;
	strcpy(jt808_param.id_0x0040,"1008611");
	jt808_param.id_0x0045 = 5;
	jt808_param.id_0x0046 = 3;
	jt808_param.id_0x0047 = 3;
	jt808_param.id_0x0050 = 5;
	jt808_param.id_0x0051 = 3;
	jt808_param.id_0x0052 = 5;
	jt808_param.id_0x0053 = 3;
	jt808_param.id_0x0054 = 5;
	jt808_param.id_0x0055 = 120;
	jt808_param.id_0x0056 = 5;
	jt808_param.id_0x0057 = 4 * 60 * 60;
	jt808_param.id_0x0058 = 8 * 60 * 60;
	jt808_param.id_0x0059 = 20 * 60;
	jt808_param.id_0x005A = 12 * 60 * 60;
	jt808_param.id_0x005B = 100;
	jt808_param.id_0x005C = 90;
	jt808_param.id_0x005D = 0x2030;
	jt808_param.id_0x005E = 80;
	jt808_param.id_0x0070 = 3;
	jt808_param.id_0x0071 = 5;
	jt808_param.id_0x0072 = 3;
	jt808_param.id_0x0073 = 5;
	jt808_param.id_0x0074 = 3;
	jt808_param.id_0x0080 = 5;
	jt808_param.id_0x0081 = 12;
	jt808_param.id_0x0082 = 101;
	strcpy(jt808_param.id_0x0083,"��AP6834");
	jt808_param.id_0x0084 = 2;
	jt808_param.id_0x0090 = 0x0F;
	jt808_param.id_0x0091 = 0x01;
	jt808_param.id_0x0092 = 0x01;
	jt808_param.id_0x0093 = 0x01;
	jt808_param.id_0x0094 = 0x01;
	jt808_param.id_0x0095 = 0x01;
	strcpy(jt808_param.id_0xF000,"70420");
	strcpy(jt808_param.id_0xF001,"TW705");
	strcpy(jt808_param.id_0xF002,"0614100");
	strcpy(jt808_param.id_0xF003,"12345");
	jt808_param.id_0xF004 = 0x07;
	strcpy(jt808_param.id_0xF005,"0000000000000000");
	strcpy(jt808_param.id_0xF006,"012002444390");
	strcpy(jt808_param.id_0xF007,"��ʻ֤����");
	strcpy(jt808_param.id_0xF008,"����");
	strcpy(jt808_param.id_0xF009,"120104197712015381");
	strcpy(jt808_param.id_0xF00A,"���ͻ���");
	strcpy(jt808_param.id_0xF00B,"δ֪");
	strcpy(jt808_param.id_0xF00C,"δ֪      ");
	strcpy(jt808_param.id_0xF010,"1.00");
	strcpy(jt808_param.id_0xF011,"1.00");
	strcpy(jt808_param.id_0xF012,"TJ.GT");
	jt808_param.id_0xF013 = 0X3020;
	jt808_param.id_0xF031 = 7008;
	jt808_param.id_0xF030 = 0x35DECC80;
	jt808_param.id_0xF032 = 6250;
	jt808_param.id_0xF032 |= BIT(31);
	jt808_param.id_0xF040 = 6;
	jt808_param.id_0xF041 = 0;
	jt808_param.id_0xF042 = 0;
	jt808_param.id_0xF043 = 1;
	jt808_param.id_0xF044 = 1;
	jt808_param.id_0xF045 = 5;
	jt808_param.id_0xF046 = 10;
	jt808_param.id_0xF047 = 15;
	jt808_param.id_0xF048 = 20;
	rt_kprintf("\n%d>%s",rt_tick_get(),buf);
	sd_write_console(buf);
}

/*********************************************************************************
  *��������:static void factory_ex(uint8_t n,uint8_t save_sequ)
  *��������:�ָ���������
  *��	��:	n			:0,����������ݣ�1��������ݣ����һָ��������в���
  			save_sequ	:0��ʾ�ȴ洢�������ٴ洢����������1��ʾ�෴
  *��	��:none
  *�� �� ֵ:rt_err_t
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
static void factory_ex(uint8_t n,uint8_t save_sequ)
{
 	uint32_t	i,addr = 58 * 4096;
	
	char tempbuf[32]="";
	param_factset_tick	= rt_tick_get();
	
	rt_kprintf("\n%d>���=%02X",rt_tick_get(),n);
	if(n & BIT(0))
	{
		param_fact_set();
		param_save_ex( 1 , save_sequ);
		jt808_param_bk.updata_utctime = 0;
		param_save_bksram();
		printer_load_param();
		strcat(tempbuf,"�ָ���������.");
	}
	
	if(n & BIT(1))
	{
		strcat(tempbuf,"�������.");
		rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY_2);
		if(rt_tick_get()>RT_TICK_PER_SECOND * 6)
		{
			rt_enter_critical( );
		}
		for( i = ADDR_DF_PARAM_BASE/4096; i < 1024; i++ )
		{
			addr	= i*4096;
			sst25_erase_4k( addr );
		}
		if(rt_tick_get()>RT_TICK_PER_SECOND * 6)
		{
			rt_exit_critical();
		}
		rt_sem_release( &sem_dataflash );
		jt808_report_init( );
		vdr_data_init( );
		jt808_misc_init( );
		Cam_Flash_InitPara( 0 );
		area_init();
		data_load();
		/*
		Cam_format();
		gps_format();
		*/
		factory_bksram();
	}
	//rt_kprintf("\n%d>%s",rt_tick_get(),tempbuf);
	sd_write_console(tempbuf);
}

/*********************************************************************************
  *��������:void factory( uint8_t n )
  *��������:�ָ���������
  *��	��:	n	:0,����������ݣ�1��������ݣ����һָ��������в���
  *��	��:none
  *�� �� ֵ:rt_err_t
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void factory( uint8_t n )
{
 factory_ex(n,0);
}
FINSH_FUNCTION_EXPORT( factory, reset to factory );



/*���������serialflash*/
uint8_t param_save_old( uint8_t sem_get )
{
	uint8_t i;
	static JT808_PARAM jt808_param_fun;			//�������ڲ�ʹ�ã����ڱȽϲ���ƥ��
	
	for(i=0;i<5;i++)
		{
		if(sem_get)
		{
			rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
		}
		sst25_write_back( ADDR_DF_PARAM_MAIN, (uint8_t*)&jt808_param, sizeof( jt808_param ) );
		sst25_read( ADDR_DF_PARAM_MAIN, (uint8_t*)&jt808_param_fun, sizeof( jt808_param_fun ) );
		if(sem_get)
		{
			rt_sem_release( &sem_dataflash );
		}
		if(memcmp((uint8_t*)&jt808_param,(uint8_t*)&jt808_param_fun, sizeof( jt808_param_fun )) == 0)
		{
			break;
		}
	}
	rt_kprintf( "parma_save size=%d\n", sizeof( jt808_param ) );
	if(i == 5)
	{
		rt_kprintf( "�ؼ���������ʧ�� retry=%d\n", i );
		reset( 1 );
		return  0;
	}
	return 1;
}


/*********************************************************************************
  *��������:uint8_t param_save_addr( uint8_t sem_get ,uint32_t addr)
  *��������:�����豸��Ҫ������flash��,����ָ���˱�����flash�е������ַ
  *��	��:	sem_get	:0��ʾ����Ҫ��ȡflash�����ź�����1��ʾ��Ҫ��ȡ�ź���
  			addr	:��Ҫ�����Ĵ洢λ��
  *��	��:	none
  *�� �� ֵ:uint8_t :0��ʾ����ʧ�ܣ�1��ʾ����ɹ�
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t param_save_addr( uint8_t sem_get ,uint32_t addr)
{
	uint8_t i;
	static JT808_PARAM jt808_param_fun;			//�������ڲ�ʹ�ã����ڱȽϲ���ƥ��
	uint16_t crc_wr,crc_rd;
	char	buf[64];
	crc_wr	= CalcCRC16((uint8_t*)&jt808_param,0,sizeof( jt808_param ), 0xFFFF );
	for(i=0;i<5;i++)
	{
		rt_kprintf("\n%d>MAIN_PARA_SAVE,  ADDR=0x%06X, RETRY=%d",rt_tick_get(),addr,i);
		if(sem_get)
		{
			rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
		}
		sst25_erase_4k( addr );
		sst25_write_through( addr, (uint8_t*)&jt808_param, sizeof( jt808_param ) );
		sst25_write_through( addr+4094, (uint8_t*)&crc_wr, 2 );
		sst25_read( addr, (uint8_t*)&jt808_param_fun, sizeof( jt808_param_fun ) );
		sst25_read( addr+4094, (uint8_t*)&crc_rd, 2 );
		if(sem_get)
		{
			rt_sem_release( &sem_dataflash );
		}
		if(memcmp((uint8_t*)&jt808_param,(uint8_t*)&jt808_param_fun, sizeof( jt808_param_fun )) == 0)
		{
			if(crc_wr == crc_rd)
				break;
		}
	}
	if(i == 5)
	{
		sprintf(buf,"MAIN_PARA_SAVE ERROR! ADDR=0x%06X",addr);
		rt_kprintf( "\n%d>%s", rt_tick_get(), buf );
		sd_write_console(buf);
		//reset( 1 );
		return  0;
	}
	return 1;
}





/*********************************************************************************
  *��������:static uint8_t param_save_ex( uint8_t sem_get ,uint8_t save_sequ)
  *��������:�����豸��Ҫ������flash��,����ָ���˱�����Ⱥ�˳��(���洢����ͱ������洢����)
  *��	��:	sem_get		:0��ʾ����Ҫ��ȡflash�����ź�����1��ʾ��Ҫ��ȡ�ź���
  			save_sequ	:0��ʾ�ȴ洢�������ٴ洢����������1��ʾ�෴
  *��	��:	none
  *�� �� ֵ:uint8_t :0��ʾ����ʧ�ܣ��������ֱ�ʾ����ɹ��������������������洢����
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
static uint8_t param_save_ex( uint8_t sem_get ,uint8_t save_sequ)
{
	u8 i=0;
	if(save_sequ)
		{
		i+= param_save_addr(sem_get,ADDR_DF_PARAM_MAIN_BK);
		i+= param_save_addr(sem_get,ADDR_DF_PARAM_MAIN);
		}
	else
		{
		i+= param_save_addr(sem_get,ADDR_DF_PARAM_MAIN);
		i+= param_save_addr(sem_get,ADDR_DF_PARAM_MAIN_BK);
		}
	if(i != 2)
	{
		rt_kprintf("\n �豸�ؼ���������ʧ��!");
	}
	else
	{
		rt_kprintf("\n%d>MAIN_PARA_SAVE OK!",rt_tick_get());
	}
	return i;
}


/*********************************************************************************
  *��������:uint8_t param_save( uint8_t sem_get )
  *��������:�����豸��Ҫ������flash��
  *��	��:	sem_get	:0��ʾ����Ҫ��ȡflash�����ź�����1��ʾ��Ҫ��ȡ�ź���
  *��	��:	none
  *�� �� ֵ:uint8_t :0��ʾ����ʧ�ܣ��������ֱ�ʾ����ɹ��������������������洢����
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t param_save( uint8_t sem_get )
{
	return param_save_ex(sem_get,0);
}
FINSH_FUNCTION_EXPORT( param_save, save param );





/*
   ���ز�����serialflash
   ���ʱ����Բ���sem_dataflash
   ��Ϊû������ʹ��

 */
void param_load_old( void )
{
	/*Ԥ��һ��������*/
	uint8_t		i;
	uint8_t		ver8[16];
	uint32_t	flash_ver;
	uint32_t	data_ver;
	uint32_t	addr;
	static	uint8_t	first_in = 0;
	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
	for(i=0;i<5;i++)
	{
		sst25_read( ADDR_DF_PARAM_MAIN, (uint8_t*)&jt808_param, sizeof( jt808_param) );
		memcpy((uint8_t *)&flash_ver,	jt808_param.id_0x0000,	4);
		memcpy((uint8_t *)&data_ver,	jt808_param.id_0x0000+4,	4);
		flash_ver 	= BYTESWAP4(flash_ver);
		data_ver 	= BYTESWAP4(data_ver);
		if(flash_ver == JT808_FLASH_FORMA_VER)
		{
			break;
		}
	}
	if(i==5)
	{
		param_fact_set();
		param_save( 0 );
		if(first_in == 0)
		{
			first_in	= 1;
			for( i = ADDR_DF_PARAM_BASE/4096; i < 1024; i++ )
			{
				addr	= i*4096;
				sst25_erase_4k( addr );
			}
		}
		rt_kprintf("\n Format flash !");
	}
	else
	{
		rt_kprintf("\n FLASH_INIT_OK!");
	}
	rt_sem_release( &sem_dataflash );
	rt_kprintf( "\nparma ver=%s size=%d\n", jt808_param.id_0x0000, sizeof( jt808_param ) );
}


/*********************************************************************************
  *��������:void param_load( void )
  *��������:��flash�м����豸��Ҫ���������ʱ����Բ���sem_dataflash����Ϊû������ʹ��
  *��	��: Read_addr	:���ز�����flash��ַ
  *��	��: none
  *�� �� ֵ:uint8_t		:0��ʾ������ȫOK,������������壺
  						BIT0		��ʾCRC����
  						BIT1		��ʾ���ݰ汾��һ�£���Ҫ��������
  						BIT2		��ʾ���ݸ�ʽ��ȫ��ƥ�䣬��Ҫ���³�ʼ�����������
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t param_load_addr( uint32_t Read_addr )
{
	uint8_t		i;
	uint8_t		ver8[16];
	uint32_t	flash_ver;
	uint32_t	data_ver;
	uint8_t		ret;
	uint16_t 	crc,crc_rd;

	ret =  MAIN_PARA_CRC_ERROR | MAIN_PARA_VER_UP | MAIN_PARA_FORMAT_ERR;
	for(i=0;i<5;i++)
	{
		sst25_init( );
		rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
		sst25_read( Read_addr, (uint8_t*)&jt808_param, sizeof( jt808_param) );
		sst25_read( Read_addr+4094, (uint8_t*)&crc_rd, 2 );
		rt_sem_release( &sem_dataflash );
		crc = CalcCRC16((uint8_t*)&jt808_param,0,sizeof( jt808_param ), 0xFFFF );
		memcpy((uint8_t *)&flash_ver,	jt808_param.id_0x0000,	4);
		memcpy((uint8_t *)&data_ver,	jt808_param.id_0x0000+4,	4);
		flash_ver 	= BYTESWAP4(flash_ver);
		data_ver 	= BYTESWAP4(data_ver);
		if(crc_rd == crc)
		{
			ret &= ~MAIN_PARA_CRC_ERROR;
		}
		if(data_ver == JT808_FLASH_DATA_VER)
		{
			ret &= ~MAIN_PARA_VER_UP;
		}
		if(flash_ver == JT808_FLASH_FORMA_VER)
		{
			ret &= ~MAIN_PARA_FORMAT_ERR;
		}
		if(ret == 0)
		{
			rt_kprintf("\n%d>MAIN_PARA_OK!    ADDR=0x%06X",rt_tick_get(),Read_addr);
			return ret;
		}
	}
	rt_kprintf("\n%d>MAIN_PARA_ERROR! ADDR=0x%06X, ERR=%d",rt_tick_get(), Read_addr, ret );
	return ret;
}


/*********************************************************************************
  *��������:void param_load( void )
  *��������:��flash�м����豸��Ҫ���������ʱ����Բ���sem_dataflash����Ϊû������ʹ��
  *��	��: none
  *��	��: none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-16
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void param_load( void )
{
	/*Ԥ��һ��������*/
	uint8_t		i;
	uint8_t		ver8[16];
	uint32_t	flash_ver;
	uint32_t	data_ver;
	uint32_t	addr;
	uint8_t		ret_a,ret_b;
	uint8_t		ret;
	uint8_t		save_sequ = 0;

	ret_a	= param_load_addr(ADDR_DF_PARAM_MAIN);
	ret_b	= param_load_addr(ADDR_DF_PARAM_MAIN_BK);
	if((ret_a==ret_b)&&(ret_a==0))
	{
		rt_kprintf("\n%d>MAIN_PARA_INIT_OK!",rt_tick_get());
		return;
	}
	if(ret_a <= ret_b)
	{
		save_sequ	= 1;
		ret = param_load_addr(ADDR_DF_PARAM_MAIN);
	}
	else
	{
		save_sequ	= 0;
		ret = ret_b;
	}
	memcpy((uint8_t *)&data_ver, jt808_param.id_0x0000+4, 4);
	if((ret & MAIN_PARA_FORMAT_ERR)||(data_ver < 0x30303030) ||(data_ver > JT808_FLASH_DATA_VER))
	{
		factory_ex(3,save_sequ);
	}
	else
	{
		switch(data_ver)
			{
			case JT808_FLASH_DATA_VER:
				{
				break;
				}
			}
		param_save_ex( 1, save_sequ);
	}
	rt_kprintf("\n%d>PARAM VER=%s SIZE=%d\n", rt_tick_get(),jt808_param.id_0x0000, sizeof( jt808_param ) );
}
FINSH_FUNCTION_EXPORT( param_load, load param );


/*********************************************************************************
  *��������:void factory_bkp_sram( void )
  *��������:��ʽ��bkp�洢����
  *��	��:none
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-11-18
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t factory_bksram( void )
{
 memset((uint8_t *)&jt808_param_bk, 0, sizeof(jt808_param_bk));
 jt808_param_bk.format_para	= BKSRAM_FORMAT;
 jt808_param_bk.data_version	= BKSRAM_VERSION;
 param_save_bksram();
}


/*********************************************************************************
  *��������:uint8_t param_save_bksram( void )
  *��������:backup sram ����д�붨��Ľṹ��
  *��	��:	none
  *��	��:	none
  *�� �� ֵ:uint8_t	:	1:��ʾ����ʧ�ܣ�	0:��ʾ�����ɹ�
  *��	��:������
  *��������:2013-11-18
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t param_save_bksram( void )
{
	return bkpsram_write( 0, (uint8_t *)&jt808_param_bk, sizeof(jt808_param_bk));
}


/*********************************************************************************
  *��������:uint8_t param_load_bksram( void )
  *��������:backup sram ���ݶ�ȡ����Ľṹ��
  *��	��:	none
  *��	��:	none
  *�� �� ֵ:uint8_t	:	1:��ʾ��ȡ����ʱ���������Ѿ���ʧ�����������¸�ʽ����	0:��ʾ��ȡ�ɹ�
  *��	��:������
  *��������:2013-11-18
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t param_load_bksram( void )
{
	bkpsram_read( 0, (uint8_t *)&jt808_param_bk, sizeof(jt808_param_bk));
	if(jt808_param_bk.format_para != BKSRAM_FORMAT)
		{
		memset((uint8_t *)&jt808_param_bk, 0, sizeof(jt808_param_bk));
		jt808_param_bk.format_para	= BKSRAM_FORMAT;
		jt808_param_bk.data_version	= BKSRAM_VERSION;
		jt808_param_bk.car_mileage  = jt808_data.id_0xFA01;
		jt808_param_bk.car_mileage *= 36;
		param_save_bksram();
		return 1;
		}
	else
		{
		switch(jt808_param_bk.data_version)
			{
			case 0x00000001:
				{
					break;
				}
			default:
			}
		}
	//rt_kprintf("\n%d>speed_curr_index=%d",rt_tick_get(),jt808_param_bk.speed_curr_index);
	return 0;
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
uint8_t param_put_old( uint16_t id, uint8_t len, uint8_t* value )
{
	int		i;
	uint8_t *psrc, *pdst;

	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
			if( ( tbl_id_lookup[i].type == TYPE_DWORD ) && ( len == 4 ) )
			{
				psrc = value;
				//rt_kprintf("psrc=%02x %02x %02x %02x \r\n",*(psrc+3),*(psrc+2),*(psrc+1),*(psrc+0));
				pdst	= tbl_id_lookup[i].val;
				*pdst++ = *( psrc + 3 );
				*pdst++ = *( psrc + 2 );
				*pdst++ = *( psrc + 1 );
				*pdst	= *( psrc + 0 );
				return 0;
			}
			if( ( tbl_id_lookup[i].type == TYPE_WORD ) && ( len == 2 ) )
			{
				psrc	= value;
				pdst	= tbl_id_lookup[i].val;
				*pdst++ = *( psrc + 1 );
				*pdst	= *psrc;
				return 0;
			}
			if( ( tbl_id_lookup[i].type == TYPE_BYTE ) && ( len == 1 ) )
			{
				psrc	= value;
				pdst	= tbl_id_lookup[i].val;
				*pdst	= *psrc;
				return 0;
			}
			if( tbl_id_lookup[i].type == TYPE_CAN )
			{
				psrc	= value;
				pdst	= tbl_id_lookup[i].val;
				*pdst++ = *( psrc + 3 );
				*pdst++ = *( psrc + 2 );
				*pdst++ = *( psrc + 1 );
				*pdst++ = *psrc;
				*pdst++ = *( psrc + 7 );
				*pdst++ = *( psrc + 6 );
				*pdst++ = *( psrc + 5 );
				*pdst	= *( psrc + 4 );
				return 0;
			}
			if( tbl_id_lookup[i].type == TYPE_STR )
			{
				psrc = tbl_id_lookup[i].val;
				strncpy( (char*)psrc, (char*)value, 32 );
				*( psrc + 31 ) = 0;
				return 0;
			}
		}
	}
	return 1;
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
uint8_t param_put( uint16_t id, uint8_t len, uint8_t* value )
{
	int			i;
	uint8_t 	*psrc, *pdst;
	uint32_t	u32data;

	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
			if((tbl_id_lookup[i].type <= TYPE_DWORD)&&(tbl_id_lookup[i].type)&&(len <= 4 ))
			{
				u32data = buf_to_data(value, len);
				if(tbl_id_lookup[i].type == TYPE_BYTE)
				{
					*tbl_id_lookup[i].val = u32data;
				}
				else if(tbl_id_lookup[i].type == TYPE_WORD)
				{
					*(uint16_t *)tbl_id_lookup[i].val = u32data;
				}
				else
				{
					*(uint32_t *)tbl_id_lookup[i].val = u32data;
				}
				return 0;
			}
			if( tbl_id_lookup[i].type == TYPE_CAN )
			{
				psrc	= value;
				pdst	= tbl_id_lookup[i].val;
				*pdst++ = *( psrc + 3 );
				*pdst++ = *( psrc + 2 );
				*pdst++ = *( psrc + 1 );
				*pdst++ = *psrc;
				*pdst++ = *( psrc + 7 );
				*pdst++ = *( psrc + 6 );
				*pdst++ = *( psrc + 5 );
				*pdst	= *( psrc + 4 );
				return 0;
			}
			if( tbl_id_lookup[i].type == TYPE_STR )
			{
				if(strlen(value) < 32)
				{
					psrc = tbl_id_lookup[i].val;
					memset(tbl_id_lookup[i].val,0,32);
					strncpy( (char*)psrc, (char*)value, strlen(value) );
					return 0;
				}
			}
		}
	}
	return 1;
}

/*���ò���*/
void param_put_int( uint16_t id, uint32_t value )
{
	uint32_t	i, j;
	uint8_t		*p;

	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
			p		= tbl_id_lookup[i].val;
			j		= value;
			*p++	= ( j & 0xff );
			*p++	= j >> 8;
			*p++	= j >> 16;
			*p		= j >> 24;
			break;
		}
	}
}

FINSH_FUNCTION_EXPORT( param_put_int, modify param );

/*д���ַ���*/
static void param_put_str( uint16_t id, uint8_t* value )
{
	int		i;
	uint8_t *p;

	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
			p = tbl_id_lookup[i].val;
			strncpy( (char*)p, (char*)value, 32 );
			break;
		}
	}
}

FINSH_FUNCTION_EXPORT( param_put_str, modify param );



uint8_t jt808_param_set(uint16_t id,char * value)
{
	int		i;
	uint32_t u32data;
	uint8_t *p;

	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
		switch(tbl_id_lookup[i].type)
			{
			case TYPE_BYTE:
			case TYPE_WORD:
			case TYPE_DWORD:
				if( sscanf(value,"%d",&u32data) == 1)
					{
					if(tbl_id_lookup[i].type == TYPE_BYTE)
						{
						*tbl_id_lookup[i].val = u32data;
						}
					else if(tbl_id_lookup[i].type == TYPE_WORD)
						{
						*(uint16_t *)tbl_id_lookup[i].val = u32data;
						}
					else
						{
						*(uint32_t *)tbl_id_lookup[i].val = u32data;
						}
					return 1;
					}
				break;
			case TYPE_STR:
				if(strlen(value) < 32)
					{
					p = tbl_id_lookup[i].val;
					memset(tbl_id_lookup[i].val,0,32);
					strncpy( (char*)p, (char*)value, strlen(value) );
					return 1;
					}
				break;
			default :
				break;
			}
		}
	}
	return  0;
}


static void param_set(uint16_t id,char * value)
{
	if(jt808_param_set(id,value))
	{
		param_save( 1 );
	}
}
FINSH_FUNCTION_EXPORT( param_set, modify param );


static void param_out(uint16_t id)
{
	int		i;
	uint32_t u32data;
	uint8_t *p;
	char buffer[33];
	param_load();
	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if(( id == tbl_id_lookup[i].id ) ||( 0 == id ))
		{
		rt_kprintf("\r\n jt808_param.id_0x%04X = ", tbl_id_lookup[i].id );
		switch(tbl_id_lookup[i].type)
			{
			case TYPE_BYTE:
			case TYPE_WORD:
			case TYPE_DWORD:
				if(tbl_id_lookup[i].type == TYPE_BYTE)
					{
					u32data=*tbl_id_lookup[i].val;
					}
				else if(tbl_id_lookup[i].type == TYPE_WORD)
					{
					u32data=*(uint16_t *)tbl_id_lookup[i].val;
					}
				else
					{
					u32data=*(uint32_t *)tbl_id_lookup[i].val;
					}
				rt_kprintf("%d;",u32data);
				break;
			case TYPE_STR:
				memset(buffer,0,sizeof(buffer));
				memcpy(buffer,tbl_id_lookup[i].val,32);
				rt_kprintf("\"%s\";",buffer);
				break;
			default :
				break;
			}
		}
	}
	rt_kprintf("\r\n \r\n" );
}
FINSH_FUNCTION_EXPORT( param_out, modify param );


#if 0
/*��ȡ����,���ز������Ͳ���*/
uint8_t param_get( uint16_t id, uint8_t* value )
{
	int			i;
	uint8_t		*p;
	uint32_t	val;
	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
			if( tbl_id_lookup[i].type == TYPE_DWORD )
			{
				p = tbl_id_lookup[i].val;
				memcpy( value, p, 4 );
				return 4;
			}

			if( tbl_id_lookup[i].type == TYPE_WORD )
			{
				p = tbl_id_lookup[i].val;
				memcpy( value, p, 2 );
				return 2;
			}

			if( tbl_id_lookup[i].type == TYPE_BYTE )
			{
				p		= tbl_id_lookup[i].val;
				*value	= *p;
				return 1;
			}
			if( tbl_id_lookup[i].type == TYPE_STR )
			{
				p = tbl_id_lookup[i].val;
				memcpy( value, p, strlen( p ) );
				return strlen( p );
			}
			if( tbl_id_lookup[i].type == TYPE_CAN )
			{
				p = tbl_id_lookup[i].val;
				memcpy( value, p, 8 );
				return 8;
			}
		}
	}
	return 0;
}

#endif

/*��ȡ����*/
uint32_t param_get_int( uint16_t id )
{
	int			i;
	uint8_t		*p;
	uint32_t	val = 0;
	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( id == tbl_id_lookup[i].id )
		{
			p	= tbl_id_lookup[i].val;
			val |= ( *p++ );
			val |= ( ( *p++ ) << 8 );
			val |= ( ( *p++ ) << 16 );
			val |= ( ( *p ) << 24 );
			break;
		}
	}
	return val;
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
void param_print( void )
{
	int			i, id;
	uint8_t		*p;
	uint32_t	val = 0;

	for( i = 0; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		id	= tbl_id_lookup[i].id;
		p	= tbl_id_lookup[i].val;
		val = 0;
		switch( tbl_id_lookup[i].type )
		{
			case TYPE_DWORD:    /*�ֽڶ��뷽ʽ little_endian*/
				val |= ( *p++ );
				val |= ( ( *p++ ) << 8 );
				val |= ( ( *p++ ) << 16 );
				val |= ( ( *p ) << 24 );
				rt_kprintf( "\nid=%04x value=%08x", id, val );
				break;
			case TYPE_CAN:      /*8���ֽ�*/
				val |= ( *p++ );
				val |= ( ( *p++ ) << 8 );
				val |= ( ( *p++ ) << 16 );
				val |= ( ( *p++ ) << 24 );
				rt_kprintf( "\nid=%04x value=%08x", id, val );
				val = 0;
				val |= ( *p++ );
				val |= ( ( *p++ ) << 8 );
				val |= ( ( *p++ ) << 16 );
				val |= ( ( *p ) << 24 );
				rt_kprintf( " %08x", val );
				break;
			case TYPE_STR:
				rt_kprintf( "\nid=%04x value=%s", id, p );
				break;
		}
	}
}

FINSH_FUNCTION_EXPORT( param_print, print param );

/*��ӡ������Ϣ*/
void param_dump( void )
{
	uint8_t tbl[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	int		i, count = 0;
	uint8_t c;
	uint8_t *p = (uint8_t*)&jt808_param;
	uint8_t printbuf[70];
	int32_t len = sizeof( jt808_param );

	while( len > 0 )
	{
		count = ( len < 16 ) ? len : 16;
		memset( printbuf, 0x20, 70 );
		for( i = 0; i < count; i++ )
		{
			c					= *p;
			printbuf[i * 3]		= tbl[c >> 4];
			printbuf[i * 3 + 1] = tbl[c & 0x0f];
			if( c < 0x20 )
			{
				c = '.';
			}
			if( c > 0x7f )
			{
				c = '.';
			}
			printbuf[50 + i] = c;
			p++;
		}
		printbuf[69] = 0;
		rt_kprintf( "%s\n", printbuf );
		len -= count;
	}
}

FINSH_FUNCTION_EXPORT( param_dump, dump param );

/*�ֶ�����apn*/
void apn( uint8_t *s )
{
	param_put_str( 0x0010, s );
}

FINSH_FUNCTION_EXPORT( apn, set apn );

/*������ip port*/
void ipport1( uint8_t *ip, uint16_t port )
{
	param_put_str( 0x0013, ip );
	param_put_int( 0x0018, port );
	param_save( 1 );
}

FINSH_FUNCTION_EXPORT( ipport1, set ipport );

/*��ȡ������mobile�ն��ֻ��� 6�ֽ�,����12λ������0*/
void deviceid( uint8_t *s )
{
	uint8_t len, i;
	char	buf[13];
	len = strlen( s );
	memset( buf, 0, 13 );
	if( len >= 12 )
	{
		strncpy( buf, s, 12 );
	}else
	{
		strcpy( buf + 12 - len, s );
	}
	buf[12] = 0;
	param_put_str( 0xF006, s );
	param_save( 1 );
}

FINSH_FUNCTION_EXPORT( deviceid, set deviceid );

static uint16_t id_get = 1; /*���浱ǰ���͵�id*/


/*
   ����������
 */
uint16_t get_param_and_fill_buf_old( uint8_t* pbuf )
{
	uint16_t	i;
	uint8_t		*p;
	uint16_t	count = 0;

	for( i = id_get; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( tbl_id_lookup[i].id >= 0xF000 )
		{
			continue;
		}
		*pbuf++ = ( tbl_id_lookup[i].id ) >> 24;
		*pbuf++ = ( tbl_id_lookup[i].id ) >> 16;
		*pbuf++ = ( tbl_id_lookup[i].id ) >> 8;
		*pbuf++ = ( tbl_id_lookup[i].id ) & 0xFF;
		count	+= 4;

		if( tbl_id_lookup[i].type == TYPE_DWORD )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = 4;
			*pbuf++ = p[3];
			*pbuf++ = p[2];
			*pbuf++ = p[1];
			*pbuf++ = p[0];
			count	+= 5;
		}

		if( tbl_id_lookup[i].type == TYPE_WORD )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = 2;
			*pbuf++ = p[1];
			*pbuf++ = p[0];
			count	+= 3;
		}

		if( tbl_id_lookup[i].type == TYPE_BYTE )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = 1;
			*pbuf++ = *p++;
			count	+= 2;
		}
		if( tbl_id_lookup[i].type == TYPE_STR )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = strlen( (char*)p );
			memcpy( pbuf, p, strlen( (char*)p ) );
			count	+= ( strlen( (char*)p ) + 1 );
			pbuf	+= strlen( (char*)p );
		}
		if( tbl_id_lookup[i].type == TYPE_CAN )
		{
			*pbuf++ = 8;
			p		= tbl_id_lookup[i].val;
			memcpy( pbuf, p, 8 );
			count	+= 9;
			pbuf	+= 8;
		}
		if( count > 512 )
		{
			break;
		}
	}
	id_get = i;
	return count;
}


/*
   ����������
 */
uint16_t get_param_and_fill_buf( uint8_t* pdest,uint16_t dest_pos )
{
	uint16_t	i,j;
	uint8_t		*p;
	uint8_t* 	pbuf;
	static uint8_t	last_para_data[64];			//��ʾ��ǰ�������һ�ζ�ȡ���Ĳ���
	static uint8_t	last_para_len	= 0;				//��ʾ��ǰ�������һ�ζ�ȡ���Ĳ�������
	static uint8_t	last_para_rd	= 0;				//��ʾ��ǰ�������һ�ζ�ȡ���Ĳ����Ķ�ȡλ��

	if(id_get<3)
	{
		memset(last_para_data,0,sizeof(last_para_data));
		last_para_len	= 0;
		last_para_rd	= 0;
	}
	for(;last_para_rd<last_para_len;)
	{
		pdest[dest_pos++] = last_para_data[last_para_rd++];
	}
	
	for( i = id_get; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ); i++ )
	{
		if( tbl_id_lookup[i].id >= 0xF000 )
		{
			continue;
		}
		memset(last_para_data,0,sizeof(last_para_data));
		pbuf	= last_para_data;
		*pbuf++ = ( tbl_id_lookup[i].id ) >> 24;
		*pbuf++ = ( tbl_id_lookup[i].id ) >> 16;
		*pbuf++ = ( tbl_id_lookup[i].id ) >> 8;
		*pbuf++ = ( tbl_id_lookup[i].id ) & 0xFF;
		last_para_len	= 4;
		last_para_rd	= 0;

		if( tbl_id_lookup[i].type == TYPE_DWORD )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = 4;
			*pbuf++ = p[3];
			*pbuf++ = p[2];
			*pbuf++ = p[1];
			*pbuf++ = p[0];
			last_para_len	+= 5;
		}

		if( tbl_id_lookup[i].type == TYPE_WORD )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = 2;
			*pbuf++ = p[1];
			*pbuf++ = p[0];
			last_para_len	+= 3;
		}

		if( tbl_id_lookup[i].type == TYPE_BYTE )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = 1;
			*pbuf++ = *p++;
			last_para_len	+= 2;
		}
		if( tbl_id_lookup[i].type == TYPE_STR )
		{
			p		= tbl_id_lookup[i].val;
			*pbuf++ = strlen( (char*)p );
			memcpy( pbuf, p, strlen( (char*)p ) );
			last_para_len	+= ( strlen( (char*)p ) + 1 );
			pbuf	+= strlen( (char*)p );
		}
		if( tbl_id_lookup[i].type == TYPE_CAN )
		{
			*pbuf++ = 8;
			p		= tbl_id_lookup[i].val;
			memcpy( pbuf, p, 8 );
			last_para_len	+= 9;
			pbuf	+= 8;
		}
		for(j=0;j<last_para_len;j++)
			{
			pdest[dest_pos++] = last_para_data[last_para_rd++];
			if(dest_pos>=512)
				{
				id_get = i+1;
				return dest_pos;
				}
			}
	}
	id_get = i;
	return dest_pos;
}

/*�������*/
void jt808_0x8104_fill_data( JT808_TX_NODEDATA *pnodedata )
{
	uint8_t		buf[600];
	uint16_t	count;

	count = get_param_and_fill_buf( buf,0 );              /*�ֽ�������*/
	rt_kprintf( "\ncount=%d id_get=%d\n", count, id_get );

	pnodedata->packet_no++;
	if( pnodedata->packet_no == pnodedata->packet_num ) /*�ﵽ���һ��*/
	{
		pnodedata->timeout = RT_TICK_PER_SECOND * 10;
	}
	memcpy( pnodedata->tag_data + 16, buf, count );
	pnodedata->retry		= 0;
	pnodedata->msg_len		= count + 16;
	pnodedata->tag_data[2]	= 0x20 + ( count >> 8 );
	pnodedata->tag_data[3]	= count & 0xFF;
	pnodedata->tag_data[12] = pnodedata->packet_num >> 8;
	pnodedata->tag_data[13] = pnodedata->packet_num & 0xFF;
	pnodedata->tag_data[14] = pnodedata->packet_no >> 8;
	pnodedata->tag_data[15] = pnodedata->packet_no & 0xFF;
	pnodedata->state		= IDLE;
}

/*Ӧ��
   ������Ļ��յ�Ӧ��
 */
static JT808_MSG_STATE jt808_0x8104_response( JT808_TX_NODEDATA * pnodedata, uint8_t *pmsg )
{
	if( pnodedata->packet_num == pnodedata->packet_no ) /*�Ѿ����������а�*/
	{
		rt_kprintf( "0x8104_response_delete\n" );
		pnodedata->state = ACK_OK;
	}
	rt_kprintf( "0x8104_response_idle\n" );
	jt808_0x8104_fill_data( pnodedata );
	return IDLE;
}

/*��ʱ��Ĵ�����*/
static JT808_MSG_STATE jt808_0x8104_timeout( JT808_TX_NODEDATA * pnodedata )
{
	if( pnodedata->packet_num == pnodedata->packet_no ) /*�Ѿ����������а�*/
	{
		rt_kprintf( "0x8104_timeout_delete\n" );
		pnodedata->state = ACK_OK;
	}
	rt_kprintf( "0x8104_timeout_idle\n" );
	jt808_0x8104_fill_data( pnodedata );
	return IDLE;
}

/*�ϱ������ն˲���*/
void jt808_param_0x8104( uint8_t *pmsg )
{
	JT808_TX_NODEDATA * pnodedata;
//	uint8_t				* pdata;
//	uint16_t			id;
	uint8_t		buf[600];
//	uint8_t				*p;
	uint16_t	param_size	= 0;
	uint16_t	param_count = 0;
	uint16_t	i, count;

	pnodedata = node_begin( 1, MULTI_CMD, 0x0104, -1, 600 );
	if( pnodedata == RT_NULL )
	{
		return;
	}

	memset( buf, 0, sizeof( buf ) );
	/*�����������ܴ�С����ͳ��0x0000��0xFxxx��*/

	for( i = 1; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ) - 1; i++ )
	{
		if( tbl_id_lookup[i].id >= 0xF000 )
		{
			continue;
		}
		param_count++;
		switch( tbl_id_lookup[i].type )
		{
			case TYPE_DWORD:
				param_size += 9;
				break;
			case TYPE_WORD:
				param_size += 7;
				break;
			case  TYPE_BYTE:
				param_size += 6;
				break;
			case TYPE_STR:
				param_size += ( strlen( (char*)( tbl_id_lookup[i].val ) ) + 5 );
				break;
			case TYPE_CAN:
				param_size += 13;
				break;
		}
	}
	rt_kprintf( "\ntotal param_count=%d size=%d\n", param_count, param_size );
	pnodedata->packet_num	= ( param_size + 511 ) / 512;   /*Ĭ��512�ְ�*/
	pnodedata->packet_no	= 1;
	rt_kprintf( "\npacket_num=%d \n", pnodedata->packet_num );

	id_get	= 1;
	count	= get_param_and_fill_buf( buf , 3 );            /*�ճ������ֽڣ���дӦ����ˮ�Ų�������*/
	rt_kprintf( "\ncount=%d id_get=%d\n", count, id_get );

	buf[0]	= pmsg[10];
	buf[1]	= pmsg[11];
	buf[2]	= param_count;
	node_data( pnodedata, buf, count );
	pnodedata->tag_data[12] = pnodedata->packet_num >> 8;
	pnodedata->tag_data[13] = pnodedata->packet_num & 0xFF;
	pnodedata->tag_data[14] = pnodedata->packet_no >> 8;
	pnodedata->tag_data[15] = pnodedata->packet_no & 0xFF;
	node_end(SINGLE_ACK, pnodedata, jt808_0x8104_timeout, jt808_0x8104_response, RT_NULL );
}

FINSH_FUNCTION_EXPORT_ALIAS( jt808_param_0x8104, param, desc );

/*�����ѯ��ֻӦ�𵥰�*/
void jt808_param_0x8106( uint8_t *pmsg )
{
	JT808_TX_NODEDATA	* pnodedata;
	uint8_t				* pdata;
	uint32_t			id;
	uint8_t				buf[600];
	uint8_t				* pbuf;
	uint8_t				*p;
	uint16_t			param_size	= 0;
	uint16_t			param_count = 0;
	uint16_t			i, count;

	memset( buf, 0, sizeof( buf ) );
	/*�����������ܴ�С����ͳ��0x0000��0xFxxx��*/
	pdata	= pmsg + 13;    /*ָ��������*/
	count	= 0;
	pbuf	= buf + 3;      /*�ճ������ֽڣ���дӦ����ˮ�Ų�������*/
	for( param_count = 0; param_count < pmsg[12]; param_count++ )
	{
		id	= *pdata++ << 24;
		id	|= *pdata++ << 16;
		id	|= *pdata++ << 8;
		id	|= *pdata++ & 0xFF;

		for( i = 1; i < sizeof( tbl_id_lookup ) / sizeof( struct _tbl_id_lookup ) - 1; i++ )
		{
			if( tbl_id_lookup[i].id == id )
			{
				*pbuf++ = id >> 24;
				*pbuf++ = id >> 16;
				*pbuf++ = id >> 8;
				*pbuf++ = id & 0xFF;
				count	+= 4;

				if( tbl_id_lookup[i].type == TYPE_DWORD )
				{
					p		= tbl_id_lookup[i].val;
					*pbuf++ = 4;
					*pbuf++ = p[3];
					*pbuf++ = p[2];
					*pbuf++ = p[1];
					*pbuf++ = p[0];
					count	+= 5;
				}

				if( tbl_id_lookup[i].type == TYPE_WORD )
				{
					p		= tbl_id_lookup[i].val;
					*pbuf++ = 2;
					*pbuf++ = p[1];
					*pbuf++ = p[0];
					count	+= 3;
				}

				if( tbl_id_lookup[i].type == TYPE_BYTE )
				{
					p		= tbl_id_lookup[i].val;
					*pbuf++ = 1;
					*pbuf++ = *p++;
					count	+= 2;
				}
				if( tbl_id_lookup[i].type == TYPE_STR )
				{
					p		= tbl_id_lookup[i].val;
					*pbuf++ = strlen( (char*)p );
					memcpy( pbuf, p, strlen( (char*)p ) );
					count	+= ( strlen( (char*)p ) + 1 );
					pbuf	+= strlen( (char*)p );
				}
				if( tbl_id_lookup[i].type == TYPE_CAN )
				{
					*pbuf++ = 8;
					p		= tbl_id_lookup[i].val;
					memcpy( pbuf, p, 8 );
					count	+= 9;
					pbuf	+= 8;
				}
				if( count > 512 )
				{
					break;
				}
			}
		}
	}
	rt_kprintf( "\ntotal count=%d size=%d\n", param_count, param_size );

	pnodedata = node_begin( 1, SINGLE_ACK, 0x0104, -1, 600 );
	if( pnodedata == RT_NULL )
	{
		return;
	}

	buf[0]				= pmsg[10];
	buf[1]				= pmsg[11];
	buf[2]				= param_count;
	pnodedata->timeout	= RT_TICK_PER_SECOND * 5;
	node_data( pnodedata, buf, count + 3 );
	node_end( SINGLE_ACK,pnodedata, jt808_0x8104_timeout, jt808_0x8104_response, RT_NULL );
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
void jt808_param_0x8103( uint8_t *pmsg )
{
	uint8_t		* p;
	uint8_t		res = 0;
	uint32_t	param_id;
	uint8_t		param_len;
	uint8_t		param_count;
	uint16_t	offset;
	uint16_t	seq, id;

	id	= ( pmsg[0] << 8 ) | pmsg[1];
	seq = ( pmsg[10] << 8 ) | pmsg[11];

	if( *( pmsg + 2 ) >= 0x20 ) /*����Ƕ�������ò���*/
	{
		rt_kprintf( "\n>%s multi packet no support!", __func__ );
		jt808_tx_0x0001( seq, id, 1 );
	}else
	{
		param_count = pmsg[12];
		offset		= 13;
		for( param_count = 0; param_count < pmsg[12]; param_count++ )
		{
			p			= pmsg + offset;
			param_id	= ( p[0] << 24 ) | ( p[1] << 16 ) | ( p[2] << 8 ) | ( p[3] );
			param_len	= p[4];
			res			|= param_put( param_id, param_len, &p[5] );
			offset		+= ( param_len + 5 );
			//rt_kprintf( "\n0x8103>id=%x res=%d \n", param_id, res );
		}

		if( res ) /*�д���*/
		{
			jt808_tx_0x0001( seq, id, 1 );
		}else
		{
			jt808_tx_0x0001( seq, id, 0 );
			param_save( 1 );
		}
	}
}

/************************************** The End Of File **************************************/
