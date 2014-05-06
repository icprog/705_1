/************************************************************
 * Copyright (C), 2008-2012,
 * FileName:		// ÎÄ¼şÃû
 * Author:			// ×÷Õß
 * Date:			// ÈÕÆÚ
 * Description:		// Ä£¿éÃèÊö
 * Version:			// °æ±¾ĞÅÏ¢
 * Function List:	// Ö÷Òªº¯Êı¼°Æä¹¦ÄÜ
 *     1. -------
 * History:			// ÀúÊ·ĞŞ¸Ä¼ÇÂ¼
 *     <author>  <time>   <version >   <desc>
 *     David    96/10/12     1.0     build this moudle
 ***********************************************************/

#include "jt808_param.h"
#include "sst25.h"
#include "jt808.h"
#include <finsh.h>
#include "string.h"
#include "jt808_gps.h"

#define TYPE_BYTE	0x01                /*¹Ì¶¨Îª1×Ö½Ú,Ğ¡¶Ë¶ÔÆë*/
#define TYPE_WORD	0x02                /*¹Ì¶¨Îª2×Ö½Ú,Ğ¡¶Ë¶ÔÆë*/
#define TYPE_DWORD	0x04                /*¹Ì¶¨Îª4×Ö½Ú,Ğ¡¶Ë¶ÔÆë*/
#define TYPE_STR	0x80                /*¹Ì¶¨Îª32×Ö½Ú,ÍøÂçË³Ğò*/
#define TYPE_CAN	0x48                /*¹Ì¶¨Îª8×Ö½Ú,µ±Ç°´æ´¢CAN_ID²ÎÊı*/

#define MAIN_PARA_CRC_ERROR				0x01			///Éè±¸Ö÷²ÎÊıCRC´íÎó
#define MAIN_PARA_VER_UP				0x02			///Éè±¸Ö÷²ÎÊı°æ±¾Éı¼¶
#define MAIN_PARA_FORMAT_ERR			0x04			///Éè±¸Ö÷²ÎÊı¸ñÊ½´íÎó


#define JT808_FLASH_FORMA_VER		0x464D5431			///"FMT0",²ÉÓÃ¿É¼û×Ö·ûÊÇÎªÁËÏÔÊ¾ºÍºóÆÚÔ¶³ÌĞŞ¸Ä·½±ã
#define JT808_FLASH_DATA_VER		0x30303030			///"0000",²ÉÓÃ¿É¼û×Ö·ûÊÇÎªÁËÏÔÊ¾ºÍºóÆÚÔ¶³ÌĞŞ¸Ä·½±ã

JT808_PARAM jt808_param;
//***********************************************************************************************************************
uint8_t	HARD_VER = 1;			///Ó²¼ş°æ±¾
uint8_t SOFT_VER = 102;			///Èí¼ş°æ±¾
/*  Èí¼ş°æ±¾¸üĞÂËµÃ÷:
	101:ÈÕÆÚ:2014-02-26.	
		ËµÃ÷:µÚÒ»°æ±¾µÄ³ÌĞò·¢²¼£¬¸Ã°æ±¾³ÌĞòÖ»ÓÃÓÚÌì½òĞ¡ÅúÁ¿5Ì¨Éè±¸Ê¹ÓÃ£¬Ã»ÓĞ¾­¹ı³¤ÆÚ¼Óµç²âÊÔ¡£
	102:ÈÕÆÚ:2014-03-10.	
		ËµÃ÷:¿ª»úÊ±Ôö¼ÓÁË¿ªÆôgpsºÍgsmµçÔ´£¬·ÀÖ¹µçÔ´²»ÎÈÔì³É¶ÁÈ¡flash´íÎó(101°æ±¾µç³Ø¹©µçÓĞ²âÊÔÊ±¶ªÊ§ÁË²ÎÊı)£¬½«MMA845ºÍÂö³å¼ì²â
			µÄÖÕ¶ËÓÅÏÈ¼¶½µµÍÎªËùÓĞÖĞ¶ÏÖĞ×îµÍµÄ¡£

*/
//***********************************************************************************************************************


#if 0
JT808_PARAM jt808_param =
{
	0x13091405,                         /*0x0000 °æ±¾*/
	50,                                 /*0x0001 ĞÄÌø·¢ËÍ¼ä¸ô*/
	10,                                 /*0x0002 TCPÓ¦´ğ³¬Ê±Ê±¼ä*/
	3,                                  /*0x0003 TCP³¬Ê±ÖØ´«´ÎÊı*/
	10,                                 /*0x0004 UDPÓ¦´ğ³¬Ê±Ê±¼ä*/
	5,                                  /*0x0005 UDP³¬Ê±ÖØ´«´ÎÊı*/
	60,                                 /*0x0006 SMSÏûÏ¢Ó¦´ğ³¬Ê±Ê±¼ä*/
	3,                                  /*0x0007 SMSÏûÏ¢ÖØ´«´ÎÊı*/
	"CMNET",                            /*0x0010 Ö÷·şÎñÆ÷APN*/
	"",                                 /*0x0011 ÓÃ»§Ãû*/
	"",                                 /*0x0012 ÃÜÂë*/
	//"jt1.gghypt.net",                   /*0x0013 Ö÷·şÎñÆ÷µØÖ·*/
	"60.28.50.210",
	"CMNET",                            /*0x0014 ±¸·İAPN*/
	"",                                 /*0x0015 ±¸·İÓÃ»§Ãû*/
	"",                                 /*0x0016 ±¸·İÃÜÂë*/
	"jt2.gghypt.net",                   /*0x0017 ±¸·İ·şÎñÆ÷µØÖ·£¬ip»òÓòÃû*/
	//7008,                               /*0x0018 TCP¶Ë¿Ú*/
	9131,
	5678,                               /*0x0019 UDP¶Ë¿Ú*/
	"www.google.com",                   /*0x001A ic¿¨Ö÷·şÎñÆ÷µØÖ·£¬ip»òÓòÃû*/
	9901,                               /*0x001B ic¿¨·şÎñÆ÷TCP¶Ë¿Ú*/
	8875,                               /*0x001C ic¿¨·şÎñÆ÷UDP¶Ë¿Ú*/
	"www.google.com",                   /*0x001D ic¿¨±¸·İ·şÎñÆ÷µØÖ·£¬ip»òÓòÃû*/
	0,                                  /*0x0020 Î»ÖÃ»ã±¨²ßÂÔ*/
	1,                                  /*0x0021 Î»ÖÃ»ã±¨·½°¸*/
	30,                                 /*0x0022 ¼İÊ»Ô±Î´µÇÂ¼»ã±¨Ê±¼ä¼ä¸ô*/
	120,                                /*0x0027 ĞİÃßÊ±»ã±¨Ê±¼ä¼ä¸ô*/
	5,                                  /*0x0028 ½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô*/
	30,                                 /*0x0029 È±Ê¡Ê±¼ä»ã±¨¼ä¸ô*/
	500,                                /*0x002c È±Ê¡¾àÀë»ã±¨¼ä¸ô*/
	1000,                               /*0x002d ¼İÊ»Ô±Î´µÇÂ¼»ã±¨¾àÀë¼ä¸ô*/
	1000,                               /*0x002e ĞİÃßÊ±¾àÀë»ã±¨¼ä¸ô*/
	100,                                /*0x002f ½ô¼±Ê±¾àÀë»ã±¨¼ä¸ô*/
	270,                                /*0x0030 ¹Õµã²¹´«½Ç¶È*/
	500,                                /*0x0031 µç×ÓÎ§À¸°ë¾¶£¨·Ç·¨Î»ÒÆãĞÖµ£©£¬µ¥Î»ÎªÃ×*/
	"1008611",                          /*0x0040 ¼à¿ØÆ½Ì¨µç»°ºÅÂë*/
	"",                                 /*0x0041 ¸´Î»µç»°ºÅÂë*/
	"",                                 /*0x0042 »Ö¸´³ö³§ÉèÖÃµç»°ºÅÂë*/
	"",                                 /*0x0043 ¼à¿ØÆ½Ì¨SMSºÅÂë*/
	"",                                 /*0x0044 ½ÓÊÕÖÕ¶ËSMSÎÄ±¾±¨¾¯ºÅÂë*/
	5,                                  /*0x0045 ÖÕ¶Ë½ÓÌıµç»°²ßÂÔ*/
	3,                                  /*0x0046 Ã¿´ÎÍ¨»°Ê±³¤*/
	3,                                  /*0x0047 µ±ÔÂÍ¨»°Ê±³¤*/
	"",                                 /*0x0048 ¼àÌıµç»°ºÅÂë*/
	"",                                 /*0x0049 ¼à¹ÜÆ½Ì¨ÌØÈ¨¶ÌĞÅºÅÂë*/
	5,                                  /*0x0050 ±¨¾¯ÆÁ±Î×Ö*/
	3,                                  /*0x0051 ±¨¾¯·¢ËÍÎÄ±¾SMS¿ª¹Ø*/
	5,                                  /*0x0052 ±¨¾¯ÅÄÕÕ¿ª¹Ø*/
	3,                                  /*0x0053 ±¨¾¯ÅÄÉã´æ´¢±êÖ¾*/
	5,                                  /*0x0054 ¹Ø¼ü±êÖ¾*/
	90,                                 /*0x0055 ×î¸ßËÙ¶Èkmh*/
	5,                                  /*0x0056 ³¬ËÙ³ÖĞøÊ±¼ä*/
	4 * 60 * 60,                        /*0x0057 Á¬Ğø¼İÊ»Ê±¼äÃÅÏŞ*/
	8 * 60 * 60,                        /*0x0058 µ±ÌìÀÛ¼Æ¼İÊ»Ê±¼äÃÅÏŞ*/
	20 * 60,                            /*0x0059 ×îĞ¡ĞİÏ¢Ê±¼ä*/
	12 * 60 * 60,                       /*0x005A ×î³¤Í£³µÊ±¼ä*/
	100,                                /*0x005B ³¬ËÙ±¨¾¯Ô¤¾¯²îÖµ£¬µ¥Î»Îª 1/10Km/h */
	90,                                 /*0x005C Æ£ÀÍ¼İÊ»Ô¤¾¯²îÖµ£¬µ¥Î»ÎªÃë£¨s£©£¬>0*/
	0x200a,                             /*0x005D Åö×²±¨¾¯²ÎÊıÉèÖÃ:*/
	30,                                 /*0x005E ²à·­±¨¾¯²ÎÊıÉèÖÃ£º ²à·­½Ç¶È£¬µ¥Î» 1 ¶È£¬Ä¬ÈÏÎª 30 ¶È*/
	0,                                  /*0x0064 ¶¨Ê±ÅÄÕÕ¿ØÖÆ*/
	0,                                  /*0x0065 ¶¨¾àÅÄÕÕ¿ØÖÆ*/
	3,                                  /*0x0070 Í¼ÏñÊÓÆµÖÊÁ¿(1-10)*/
	5,                                  /*0x0071 ÁÁ¶È*/
	3,                                  /*0x0072 ¶Ô±È¶È*/
	5,                                  /*0x0073 ±¥ºÍ¶È*/
	3,                                  /*0x0074 É«¶È*/
	5,                                  /*0x0080 ³µÁ¾Àï³Ì±í¶ÁÊı0.1km*/
	12,                                 /*0x0081 Ê¡ÓòID*/
	101,                                /*0x0082 ÊĞÓòID*/
	"½òAP6834",                         /*0x0083 »ú¶¯³µºÅÅÆ*/
	2,                                  /*0x0084 ³µÅÆÑÕÉ«	1À¶É« 2»ÆÉ« 3ºÚÉ« 4°×É« 9ÆäËû*/
	0x0f,                               /*0x0090 GNSS ¶¨Î»Ä£Ê½*/
	0x01,                               /*0x0091 GNSS ²¨ÌØÂÊ*/
	0x01,                               /*0x0092 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İÊä³öÆµÂÊ*/
	0x01,                               /*0x0093 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İ²É¼¯ÆµÂÊ*/
	0x01,                               /*0x0094 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İÉÏ´«·½Ê½*/
	0x01,                               /*0x0095 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İÉÏ´«ÉèÖÃ*/
	0,                                  /*0x0100 CAN ×ÜÏßÍ¨µÀ 1 ²É¼¯Ê±¼ä¼ä¸ô(ms)£¬0 ±íÊ¾²»²É¼¯*/
	0,                                  /*0x0101 CAN ×ÜÏßÍ¨µÀ 1 ÉÏ´«Ê±¼ä¼ä¸ô(s)£¬0 ±íÊ¾²»ÉÏ´«*/
	0,                                  /*0x0102 CAN ×ÜÏßÍ¨µÀ 2 ²É¼¯Ê±¼ä¼ä¸ô(ms)£¬0 ±íÊ¾²»²É¼¯*/
	0,                                  /*0x0103 CAN ×ÜÏßÍ¨µÀ 2 ÉÏ´«Ê±¼ä¼ä¸ô(s)£¬0 ±íÊ¾²»ÉÏ´«*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0110 CAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0111 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0112 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0113 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0114 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0115 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0116 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0117 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0118 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	{ 0, 0, 0, 0, 0, 0, 0, 0 },         /*0x0119 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/

	"70420",                            /*0xF000 ÖÆÔìÉÌID  70420*/
	"TW703",                            /*0xF001 ÖÕ¶ËĞÍºÅTW703-BD*/
	"0614100",                          /*0xF002 ÖÕ¶ËID*/
	"12345",                            /*0xF003 ¼øÈ¨Âë*/
	0x07,                               /*0xF004 ÖÕ¶ËÀàĞÍ*/
	"0000000000000000",                 /*0xF005 ³µÁ¾VIN*/
	"012002444390",                     /*0xF006 DeviceID*/
	"¼İÊ»Ö¤´úÂë",                       /*0xF007 ¼İÊ»Ö¤´úÂë*/
	"ÕÅÈı",                             /*0xF008 ¼İÊ»Ô±ĞÕÃû*/
	"120104197712015381",               /*0xF009 ¼İÊ»Ö¤ºÅÂë*/
	"´óĞÍ»õÔË",                         /*0xF00A ³µÁ¾ÀàĞÍ*/
	"Î´Öª",                             /*0xF00B ´ÓÒµ×Ê¸ñÖ¤*/
	"Î´Öª      ",                       /*0xF00C ·¢Ö¤»ú¹¹*/

	"1.00",                             /*0xF010 Èí¼ş°æ±¾ºÅ*/
	"1.00",                             /*0xF011 Ó²¼ş°æ±¾ºÅ*/
	"TJ.GT",                            /*0xF012 ÏúÊÛ¿Í»§´úÂë*/
	0x3020,                             /*0xF013 ±±¶·Ä£¿éĞÍºÅ0,Î´È·¶¨ ,0x3020 Ä¬ÈÏ 0x3017*/

	0,                                  /*0xF020 ×ÜÀï³Ì*/
	0,                                  /*0xF021 ³µÁ¾×´Ì¬*/

	0x35DECC80,                         /*0xF030 ¼ÇÂ¼ÒÇ³õ´Î°²×°Ê±¼ä,mytime¸ñÊ½*/
	0,                                  /*id_0xF031;      ³õÊ¼Àï³Ì*/
	6250,                               /*id_0xF032;      ³µÁ¾Âö³åÏµÊı*/

	6,                                  //line_space;               //ĞĞ¼ä¸ô
	0,                                  //margin_left;				//×ó±ß½ç
	0,                                  //margin_right;				//ÓÒ±ß½ç
	1,                                  //step_delay;               //²½½øÑÓÊ±,Ó°ÏìĞĞ¼ä¸ô
	1,                                  //gray_level;               //»Ò¶ÈµÈ¼¶,¼ÓÈÈÊ±¼ä
	5,                                  //heat_delay[0];			//¼ÓÈÈÑÓÊ±
	10,                                 //heat_delay[1];			//¼ÓÈÈÑÓÊ±
	15,                                 //heat_delay[2];			//¼ÓÈÈÑÓÊ±
	20,                                 //heat_delay[3];			//¼ÓÈÈÑÓÊ±
};
#endif
JT808_PARAM_BK 	jt808_param_bk;		///´æÈëbksramÖĞµÄËùÓĞ²ÎÊı£¬¸Ã²ÎÊıµÄ±£´æÊ±¼äÎª8Ğ¡Ê±×óÓÒ
STYLE_JT808_DATA	jt808_data;		///±£´æ³µÁ¾ĞèÒª¶¨ÆÚ±£´æµÄÊı¾İ
TYPE_CAR_DATA	car_data;			///³µÁ¾Ïà¹Øµ±Ç°Êı¾İ
uint32_t		param_factset_tick=0;	///Çå¿ÕÉè±¸²ÎÊıµÄÊ±¼ä£¬µ¥Î»Îªtick


#define FLAG_DISABLE_REPORT_INVALID 1   /*Éè±¸·Ç·¨*/

#define FLAG_DISABLE_REPORT_AREA 2      /*ÇøÓòÄÚ½ûÖ¹ÉÏ±¨*/

//static uint32_t flag_disable_report = 0;    /*½ûÖ¹ÉÏ±¨µÄ±êÖ¾Î»*/

#define ID_LOOKUP( id, type ) { id, type, (uint8_t*)&( jt808_param.id_ ## id ) }

struct _tbl_id_lookup
{
	uint16_t	id;
	uint8_t		type;
	uint8_t		* val;
} tbl_id_lookup[] = {
	ID_LOOKUP( 0x0000, TYPE_STR ),    	//uint32_t  id_0x0000;   /*0x0000 °æ±¾*/
	ID_LOOKUP( 0x0001, TYPE_DWORD ),    //uint32_t  id_0x0001;   /*0x0001 ĞÄÌø·¢ËÍ¼ä¸ô*/
	ID_LOOKUP( 0x0002, TYPE_DWORD ),    //uint32_t  id_0x0002;   /*0x0002 TCPÓ¦´ğ³¬Ê±Ê±¼ä*/
	ID_LOOKUP( 0x0003, TYPE_DWORD ),    //uint32_t  id_0x0003;   /*0x0003 TCP³¬Ê±ÖØ´«´ÎÊı*/
	ID_LOOKUP( 0x0004, TYPE_DWORD ),    //uint32_t  id_0x0004;   /*0x0004 UDPÓ¦´ğ³¬Ê±Ê±¼ä*/
	ID_LOOKUP( 0x0005, TYPE_DWORD ),    //uint32_t  id_0x0005;   /*0x0005 UDP³¬Ê±ÖØ´«´ÎÊı*/
	ID_LOOKUP( 0x0006, TYPE_DWORD ),    //uint32_t  id_0x0006;   /*0x0006 SMSÏûÏ¢Ó¦´ğ³¬Ê±Ê±¼ä*/
	ID_LOOKUP( 0x0007, TYPE_DWORD ),    //uint32_t  id_0x0007;   /*0x0007 SMSÏûÏ¢ÖØ´«´ÎÊı*/
	ID_LOOKUP( 0x0010, TYPE_STR ),      //char   id_0x0010[32];  /*0x0010 Ö÷·şÎñÆ÷APN*/
	ID_LOOKUP( 0x0011, TYPE_STR ),      //char   id_0x0011[32];  /*0x0011 ÓÃ»§Ãû*/
	ID_LOOKUP( 0x0012, TYPE_STR ),      //char   id_0x0012[32];  /*0x0012 ÃÜÂë*/
	ID_LOOKUP( 0x0013, TYPE_STR ),      //char   id_0x0013[32];  /*0x0013 Ö÷·şÎñÆ÷µØÖ·*/
	ID_LOOKUP( 0x0014, TYPE_STR ),      //char   id_0x0014[32];  /*0x0014 ±¸·İAPN*/
	ID_LOOKUP( 0x0015, TYPE_STR ),      //char   id_0x0015[32];  /*0x0015 ±¸·İÓÃ»§Ãû*/
	ID_LOOKUP( 0x0016, TYPE_STR ),      //char   id_0x0016[32];  /*0x0016 ±¸·İÃÜÂë*/
	ID_LOOKUP( 0x0017, TYPE_STR ),      //char   id_0x0017[32];  /*0x0017 ±¸·İ·şÎñÆ÷µØÖ·£¬ip»òÓòÃû*/
	ID_LOOKUP( 0x0018, TYPE_DWORD ),    //uint32_t  id_0x0018;   /*0x0018 TCP¶Ë¿Ú*/
	ID_LOOKUP( 0x0019, TYPE_DWORD ),    //uint32_t  id_0x0019;   /*0x0019 UDP¶Ë¿Ú*/
	ID_LOOKUP( 0x001A, TYPE_STR ),      //char   id_0x001A[32];  /*0x001A ic¿¨Ö÷·şÎñÆ÷µØÖ·£¬ip»òÓòÃû*/
	ID_LOOKUP( 0x001B, TYPE_DWORD ),    //uint32_t  id_0x001B;   /*0x001B ic¿¨·şÎñÆ÷TCP¶Ë¿Ú*/
	ID_LOOKUP( 0x001C, TYPE_DWORD ),    //uint32_t  id_0x001C;   /*0x001C ic¿¨·şÎñÆ÷UDP¶Ë¿Ú*/
	ID_LOOKUP( 0x001D, TYPE_STR ),      //char   id_0x001D[32];  /*0x001D ic¿¨±¸·İ·şÎñÆ÷µØÖ·£¬ip»òÓòÃû*/
	ID_LOOKUP( 0x0020, TYPE_DWORD ),    //uint32_t  id_0x0020;   /*0x0020 Î»ÖÃ»ã±¨²ßÂÔ*/
	ID_LOOKUP( 0x0021, TYPE_DWORD ),    //uint32_t  id_0x0021;   /*0x0021 Î»ÖÃ»ã±¨·½°¸*/
	ID_LOOKUP( 0x0022, TYPE_DWORD ),    //uint32_t  id_0x0022;   /*0x0022 ¼İÊ»Ô±Î´µÇÂ¼»ã±¨Ê±¼ä¼ä¸ô*/
	ID_LOOKUP( 0x0027, TYPE_DWORD ),    //uint32_t  id_0x0027;   /*0x0027 ĞİÃßÊ±»ã±¨Ê±¼ä¼ä¸ô*/
	ID_LOOKUP( 0x0028, TYPE_DWORD ),    //uint32_t  id_0x0028;   /*0x0028 ½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô*/
	ID_LOOKUP( 0x0029, TYPE_DWORD ),    //uint32_t  id_0x0029;   /*0x0029 È±Ê¡Ê±¼ä»ã±¨¼ä¸ô*/
	ID_LOOKUP( 0x002C, TYPE_DWORD ),    //uint32_t  id_0x002C;   /*0x002c È±Ê¡¾àÀë»ã±¨¼ä¸ô*/
	ID_LOOKUP( 0x002D, TYPE_DWORD ),    //uint32_t  id_0x002D;   /*0x002d ¼İÊ»Ô±Î´µÇÂ¼»ã±¨¾àÀë¼ä¸ô*/
	ID_LOOKUP( 0x002E, TYPE_DWORD ),    //uint32_t  id_0x002E;   /*0x002e ĞİÃßÊ±¾àÀë»ã±¨¼ä¸ô*/
	ID_LOOKUP( 0x002F, TYPE_DWORD ),    //uint32_t  id_0x002F;   /*0x002f ½ô¼±Ê±¾àÀë»ã±¨¼ä¸ô*/
	ID_LOOKUP( 0x0030, TYPE_DWORD ),    //uint32_t  id_0x0030;   /*0x0030 ¹Õµã²¹´«½Ç¶È*/
	ID_LOOKUP( 0x0031, TYPE_DWORD ),    //uint16_t  id_0x0031;   /*0x0031 µç×ÓÎ§À¸°ë¾¶£¨·Ç·¨Î»ÒÆãĞÖµ£©£¬µ¥Î»ÎªÃ×*/
	ID_LOOKUP( 0x0040, TYPE_STR ),      //char   id_0x0040[32];  /*0x0040 ¼à¿ØÆ½Ì¨µç»°ºÅÂë*/
	ID_LOOKUP( 0x0041, TYPE_STR ),      //char   id_0x0041[32];  /*0x0041 ¸´Î»µç»°ºÅÂë*/
	ID_LOOKUP( 0x0042, TYPE_STR ),      //char   id_0x0042[32];  /*0x0042 »Ö¸´³ö³§ÉèÖÃµç»°ºÅÂë*/
	ID_LOOKUP( 0x0043, TYPE_STR ),      //char   id_0x0043[32];  /*0x0043 ¼à¿ØÆ½Ì¨SMSºÅÂë*/
	ID_LOOKUP( 0x0044, TYPE_STR ),      //char   id_0x0044[32];  /*0x0044 ½ÓÊÕÖÕ¶ËSMSÎÄ±¾±¨¾¯ºÅÂë*/
	ID_LOOKUP( 0x0045, TYPE_DWORD ),    //uint32_t  id_0x0045;   /*0x0045 ÖÕ¶Ë½ÓÌıµç»°²ßÂÔ*/
	ID_LOOKUP( 0x0046, TYPE_DWORD ),    //uint32_t  id_0x0046;   /*0x0046 Ã¿´ÎÍ¨»°Ê±³¤*/
	ID_LOOKUP( 0x0047, TYPE_DWORD ),    //uint32_t  id_0x0047;   /*0x0047 µ±ÔÂÍ¨»°Ê±³¤*/
	ID_LOOKUP( 0x0048, TYPE_STR ),      //char   id_0x0048[32];  /*0x0048 ¼àÌıµç»°ºÅÂë*/
	ID_LOOKUP( 0x0049, TYPE_STR ),      //char   id_0x0049[32];  /*0x0049 ¼à¹ÜÆ½Ì¨ÌØÈ¨¶ÌĞÅºÅÂë*/
	ID_LOOKUP( 0x0050, TYPE_DWORD ),    //uint32_t  id_0x0050;   /*0x0050 ±¨¾¯ÆÁ±Î×Ö*/
	ID_LOOKUP( 0x0051, TYPE_DWORD ),    //uint32_t  id_0x0051;   /*0x0051 ±¨¾¯·¢ËÍÎÄ±¾SMS¿ª¹Ø*/
	ID_LOOKUP( 0x0052, TYPE_DWORD ),    //uint32_t  id_0x0052;   /*0x0052 ±¨¾¯ÅÄÕÕ¿ª¹Ø*/
	ID_LOOKUP( 0x0053, TYPE_DWORD ),    //uint32_t  id_0x0053;   /*0x0053 ±¨¾¯ÅÄÉã´æ´¢±êÖ¾*/
	ID_LOOKUP( 0x0054, TYPE_DWORD ),    //uint32_t  id_0x0054;   /*0x0054 ¹Ø¼ü±êÖ¾*/
	ID_LOOKUP( 0x0055, TYPE_DWORD ),    //uint32_t  id_0x0055;   /*0x0055 ×î¸ßËÙ¶Èkmh*/
	ID_LOOKUP( 0x0056, TYPE_DWORD ),    //uint32_t  id_0x0056;   /*0x0056 ³¬ËÙ³ÖĞøÊ±¼ä*/
	ID_LOOKUP( 0x0057, TYPE_DWORD ),    //uint32_t  id_0x0057;   /*0x0057 Á¬Ğø¼İÊ»Ê±¼äÃÅÏŞ*/
	ID_LOOKUP( 0x0058, TYPE_DWORD ),    //uint32_t  id_0x0058;   /*0x0058 µ±ÌìÀÛ¼Æ¼İÊ»Ê±¼äÃÅÏŞ*/
	ID_LOOKUP( 0x0059, TYPE_DWORD ),    //uint32_t  id_0x0059;   /*0x0059 ×îĞ¡ĞİÏ¢Ê±¼ä*/
	ID_LOOKUP( 0x005A, TYPE_DWORD ),    //uint32_t  id_0x005A;   /*0x005A ×î³¤Í£³µÊ±¼ä*/
	ID_LOOKUP( 0x005B, TYPE_WORD ),     //uint16_t  id_0x005B;   /*0x005B ³¬ËÙ±¨¾¯Ô¤¾¯²îÖµ£¬µ¥Î»Îª 1/10Km/h */
	ID_LOOKUP( 0x005C, TYPE_WORD ),     //uint16_t  id_0x005C;   /*0x005C Æ£ÀÍ¼İÊ»Ô¤¾¯²îÖµ£¬µ¥Î»ÎªÃë£¨s£©£¬>0*/
	ID_LOOKUP( 0x005D, TYPE_WORD ),     //uint16_t  id_0x005D;   /*0x005D Åö×²±¨¾¯²ÎÊıÉèÖÃ:b7..0£ºÅö×²Ê±¼ä(4ms) b15..8£ºÅö×²¼ÓËÙ¶È(0.1g) 0-79 Ö®¼ä£¬Ä¬ÈÏÎª10 */
	ID_LOOKUP( 0x005E, TYPE_WORD ),     //uint16_t  id_0x005E;   /*0x005E ²à·­±¨¾¯²ÎÊıÉèÖÃ£º ²à·­½Ç¶È£¬µ¥Î» 1 ¶È£¬Ä¬ÈÏÎª 30 ¶È*/
	ID_LOOKUP( 0x0064, TYPE_DWORD ),    //uint32_t  id_0x0064;   /*0x0064 ¶¨Ê±ÅÄÕÕ¿ØÖÆ*/
	ID_LOOKUP( 0x0065, TYPE_DWORD ),    //uint32_t  id_0x0065;   /*0x0065 ¶¨¾àÅÄÕÕ¿ØÖÆ*/
	ID_LOOKUP( 0x0070, TYPE_DWORD ),    //uint32_t  id_0x0070;   /*0x0070 Í¼ÏñÊÓÆµÖÊÁ¿(1-10)*/
	ID_LOOKUP( 0x0071, TYPE_DWORD ),    //uint32_t  id_0x0071;   /*0x0071 ÁÁ¶È*/
	ID_LOOKUP( 0x0072, TYPE_DWORD ),    //uint32_t  id_0x0072;   /*0x0072 ¶Ô±È¶È*/
	ID_LOOKUP( 0x0073, TYPE_DWORD ),    //uint32_t  id_0x0073;   /*0x0073 ±¥ºÍ¶È*/
	ID_LOOKUP( 0x0074, TYPE_DWORD ),    //uint32_t  id_0x0074;   /*0x0074 É«¶È*/
	ID_LOOKUP( 0x0080, TYPE_DWORD ),    //uint32_t  id_0x0080;   /*0x0080 ³µÁ¾Àï³Ì±í¶ÁÊı0.1km*/
	ID_LOOKUP( 0x0081, TYPE_WORD ),     //uint16_t  id_0x0081;   /*0x0081 Ê¡ÓòID*/
	ID_LOOKUP( 0x0082, TYPE_WORD ),     //uint16_t  id_0x0082;   /*0x0082 ÊĞÓòID*/
	ID_LOOKUP( 0x0083, TYPE_STR ),      //char   id_0x0083[32];  /*0x0083 »ú¶¯³µºÅÅÆ*/
	ID_LOOKUP( 0x0084, TYPE_BYTE ),     //uint8_t		 id_0x0084;      /*0x0084 ³µÅÆÑÕÉ«	1À¶É« 2»ÆÉ« 3ºÚÉ« 4°×É« 9ÆäËû*/
	ID_LOOKUP( 0x0090, TYPE_BYTE ),     //uint8_t		 id_0x0090;      /*0x0090 GNSS ¶¨Î»Ä£Ê½*/
	ID_LOOKUP( 0x0091, TYPE_BYTE ),     //uint8_t		 id_0x0091;      /*0x0091 GNSS ²¨ÌØÂÊ*/
	ID_LOOKUP( 0x0092, TYPE_BYTE ),     //uint8_t		 id_0x0092;      /*0x0092 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İÊä³öÆµÂÊ*/
	ID_LOOKUP( 0x0093, TYPE_DWORD ),    //uint32_t  id_0x0093;   /*0x0093 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İ²É¼¯ÆµÂÊ*/
	ID_LOOKUP( 0x0094, TYPE_BYTE ),     //uint8_t		 id_0x0094;      /*0x0094 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İÉÏ´«·½Ê½*/
	ID_LOOKUP( 0x0095, TYPE_DWORD ),    //uint32_t  id_0x0095;   /*0x0095 GNSS Ä£¿éÏêÏ¸¶¨Î»Êı¾İÉÏ´«ÉèÖÃ*/
	ID_LOOKUP( 0x0100, TYPE_DWORD ),    //uint32_t  id_0x0100;   /*0x0100 CAN ×ÜÏßÍ¨µÀ 1 ²É¼¯Ê±¼ä¼ä¸ô(ms)£¬0 ±íÊ¾²»²É¼¯*/
	ID_LOOKUP( 0x0101, TYPE_WORD ),     //uint16_t  id_0x0101;   /*0x0101 CAN ×ÜÏßÍ¨µÀ 1 ÉÏ´«Ê±¼ä¼ä¸ô(s)£¬0 ±íÊ¾²»ÉÏ´«*/
	ID_LOOKUP( 0x0102, TYPE_DWORD ),    //uint32_t  id_0x0102;   /*0x0102 CAN ×ÜÏßÍ¨µÀ 2 ²É¼¯Ê±¼ä¼ä¸ô(ms)£¬0 ±íÊ¾²»²É¼¯*/
	ID_LOOKUP( 0x0103, TYPE_WORD ),     //uint16_t  id_0x0103;   /*0x0103 CAN ×ÜÏßÍ¨µÀ 2 ÉÏ´«Ê±¼ä¼ä¸ô(s)£¬0 ±íÊ¾²»ÉÏ´«*/
	ID_LOOKUP( 0x0110, TYPE_BYTE | 8 ), //uint8_t		 id_0x0110[8];	 /*0x0110 CAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0111, TYPE_BYTE | 8 ), //uint8_t		 id_0x0111[8];	 /*0x0111 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0112, TYPE_BYTE | 8 ), //uint8_t		 id_0x0112[8];	 /*0x0112 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0113, TYPE_BYTE | 8 ), //uint8_t		 id_0x0113[8];	 /*0x0113 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0114, TYPE_BYTE | 8 ), //uint8_t		 id_0x0114[8];	 /*0x0114 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0115, TYPE_BYTE | 8 ), //uint8_t		 id_0x0115[8];	 /*0x0115 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0116, TYPE_BYTE | 8 ), //uint8_t		 id_0x0116[8];	 /*0x0116 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0117, TYPE_BYTE | 8 ), //uint8_t		 id_0x0117[8];	 /*0x0117 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0118, TYPE_BYTE | 8 ), //uint8_t		 id_0x0118[8];	 /*0x0118 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0x0119, TYPE_BYTE | 8 ), //uint8_t		 id_0x0119[8];	 /*0x0119 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/

	ID_LOOKUP( 0xF000, TYPE_STR ),      //uint8_t		 id_0x0119[8];	 /*0x0119 ÆäËûCAN ×ÜÏß ID µ¥¶À²É¼¯ÉèÖÃ*/
	ID_LOOKUP( 0xF001, TYPE_STR ),      /*0xF001 ÖÕ¶ËĞÍºÅ 20byte*/
	ID_LOOKUP( 0xF002, TYPE_STR ),      /*0xF002 ÖÕ¶ËID 7byte*/
	ID_LOOKUP( 0xF003, TYPE_STR ),      /*0xF003 ¼øÈ¨Âë*/
	ID_LOOKUP( 0xF004, TYPE_BYTE ),     /*0xF004 ÖÕ¶ËÀàĞÍ*/
	ID_LOOKUP( 0xF005, TYPE_STR ),      /*0xF005 ³µÁ¾±êÊ¶,VIN*/
	ID_LOOKUP( 0xF006, TYPE_STR ),      /*0xF006 ³µÁ¾±êÊ¶,MOBILE*/
	ID_LOOKUP( 0xF008, TYPE_STR ),      /*0xF008 ¼İÊ»Ô±ĞÕÃû*/
	ID_LOOKUP( 0xF009, TYPE_STR ),      /*0xF009 ¼İÊ»Ö¤ºÅÂë*/
	ID_LOOKUP( 0xF00A, TYPE_STR ),      /*0xF00A ³µÁ¾ÀàĞÍ*/
	ID_LOOKUP( 0xF00B, TYPE_STR ),      /*0xF00B ´ÓÒµ×Ê¸ñÖ¤*/
	ID_LOOKUP( 0xF00C, TYPE_STR ),      /*0xF00C ·¢Ö¤»ú¹¹*/
	ID_LOOKUP( 0xF00D, TYPE_BYTE ), 	//0xF00D Á¬½ÓÄ£Ê½ÉèÖÃ			0:»õÔËÄ£Ê½	 1:Á½¿ÍÒ»Î£
	ID_LOOKUP( 0xF00E, TYPE_BYTE ), 	//0xF00E ÓĞÎŞ³µÅÆºÅ				0:ÎŞ³µÅÆºÅ	 1:ÓĞ³µÅÆºÅ£¬ĞèÒªÉèÖÃ
	ID_LOOKUP( 0xF00F, TYPE_BYTE ), 	//0xF00F ³µÁ¾ĞÅÏ¢ÉèÖÃÊÇ·ñÍê³É	0:Î´ÉèÖÃ	 1:ÉèÖÃÍê³É

	ID_LOOKUP( 0xF010, TYPE_STR ),      /*0xF010 Èí¼ş°æ±¾ºÅ*/
	ID_LOOKUP( 0xF011, TYPE_STR ),      /*0xF011 Ó²¼ş°æ±¾ºÅ*/

	ID_LOOKUP( 0xF013, TYPE_DWORD ),    /*0xF013 ±±¶·Ä£¿éĞÍºÅ*/
		
	ID_LOOKUP( 0xF030, TYPE_DWORD ), 	/*0xF030 ¼ÇÂ¼ÒÇ³õ´Î°²×°Ê±¼ä,mytime¸ñÊ½*/
	ID_LOOKUP( 0xF031, TYPE_DWORD ), 	/*0xF031 ±¸·İTCP¶Ë¿Ú1*/
	ID_LOOKUP( 0xF032, TYPE_DWORD ), 	/*0xF032 ³µÁ¾Âö³åÏµÊı,ÓĞĞ§Î»ÊıÎªµÍ16Î»,±íÊ¾ÎªÃ¿¹«ÀïµÄÂö³åÊı£¬×î¸ßÎ»Îª1±íÊ¾ĞèÒªĞ£×¼£¬Îª0±íÊ¾²»ĞèÒªĞ£×¼*/

	ID_LOOKUP( 0xF040, TYPE_BYTE ),     //line_space;               //ĞĞ¼ä¸ô
	ID_LOOKUP( 0xF041, TYPE_BYTE ),     //margin_left;				//×ó±ß½ç
	ID_LOOKUP( 0xF042, TYPE_BYTE ),     //margin_right;				//ÓÒ±ß½ç
	ID_LOOKUP( 0xF043, TYPE_BYTE ),     //step_delay;               //²½½øÑÓÊ±,Ó°ÏìĞĞ¼ä¸ô
	ID_LOOKUP( 0xF044, TYPE_BYTE ),     //gray_level;               //»Ò¶ÈµÈ¼¶,¼ÓÈÈÊ±¼ä
	ID_LOOKUP( 0xF045, TYPE_BYTE ),     //heat_delay[0];			//¼ÓÈÈÑÓÊ±
	ID_LOOKUP( 0xF046, TYPE_BYTE ),     //heat_delay[1];			//¼ÓÈÈÑÓÊ±
	ID_LOOKUP( 0xF047, TYPE_BYTE ),     //heat_delay[2];			//¼ÓÈÈÑÓÊ±
	ID_LOOKUP( 0xF048, TYPE_BYTE ),     //heat_delay[3];			//¼ÓÈÈÑÓÊ±
	ID_LOOKUP( 0xF049, TYPE_STR ),     	/*0xF049 ±¸·İ·şÎñÆ÷µØÖ·2*/
	ID_LOOKUP( 0xF04A, TYPE_WORD ),     /*0xF04A ±¸·İTCP¶Ë¿Ú2*/
	ID_LOOKUP( 0xF04B, TYPE_STR ),     	/*0xF04B ±¸·İÎñÆ÷µØÖ·3*/
	ID_LOOKUP( 0xF04C, TYPE_WORD ),     /*0xF04C ±¸·İTCP¶Ë¿Ú3*/
};

extern void jt808_report_init( void );
extern void vdr_data_init(void);
extern u16 Cam_Flash_InitPara( u8 printf_info );
extern void printer_load_param( void );
static uint8_t param_save_ex( uint8_t sem_get ,uint8_t save_sequ);

/*********************************************************************************
  *º¯ÊıÃû³Æ:void data_save( void )
  *¹¦ÄÜÃèÊö:±£´æÉè±¸×´Ì¬Êı¾İµ½flashÖĞ
  *Êä	Èë:	none
  *Êä	³ö:	none
  *·µ »Ø Öµ:none
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
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
  *º¯ÊıÃû³Æ:void data_load( void )
  *¹¦ÄÜÃèÊö:¶ÁÈ¡ĞèÒª¶¨Ê±±£´æµÄÉè±¸×´Ì¬µÈÊı¾İ
  *Êä	Èë:	none
  *Êä	³ö:	none
  *·µ »Ø Öµ:none
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
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
  *º¯ÊıÃû³Æ:void param_fact_set(void)
  *¹¦ÄÜÃèÊö:½«Éè±¸Ö÷Òª²ÎÊıÉèÖÃÎª³ö³§²ÎÊı
  *Êä	Èë: none
  *Êä	³ö: none
  *·µ »Ø Öµ:none
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
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
	strcpy(jt808_param.id_0x0083,"½òAP6834");
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
	strcpy(jt808_param.id_0xF007,"¼İÊ»Ö¤´úÂë");
	strcpy(jt808_param.id_0xF008,"ÕÅÈı");
	strcpy(jt808_param.id_0xF009,"120104197712015381");
	strcpy(jt808_param.id_0xF00A,"´óĞÍ»õÔË");
	strcpy(jt808_param.id_0xF00B,"Î´Öª");
	strcpy(jt808_param.id_0xF00C,"Î´Öª      ");
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
  *º¯ÊıÃû³Æ:static void factory_ex(uint8_t n,uint8_t save_sequ)
  *¹¦ÄÜÃèÊö:»Ö¸´³ö³§ÉèÖÃ
  *Êä	Èë:	n			:0,Çå¿ÕËùÓĞÊı¾İ£»1£¬Çå¿ÕÊı¾İ£¬²¢ÇÒ»Ö¸´³ö³§ËùÓĞ²ÎÊı
  			save_sequ	:0±íÊ¾ÏÈ´æ´¢Ö÷ÇøÓò£¬ÔÙ´æ´¢Ö÷±¸·İÇøÓò£¬1±íÊ¾Ïà·´
  *Êä	³ö:none
  *·µ »Ø Öµ:rt_err_t
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
static void factory_ex(uint8_t n,uint8_t save_sequ)
{
 	uint32_t	i,addr = 58 * 4096;
	
	char tempbuf[32]="";
	param_factset_tick	= rt_tick_get();
	
	rt_kprintf("\n%d>Çå¿Õ=%02X",rt_tick_get(),n);
	if(n & BIT(0))
	{
		param_fact_set();
		param_save_ex( 1 , save_sequ);
		jt808_param_bk.updata_utctime = 0;
		param_save_bksram();
		printer_load_param();
		strcat(tempbuf,"»Ö¸´³ö³§²ÎÊı.");
	}
	
	if(n & BIT(1))
	{
		strcat(tempbuf,"Çå¿ÕÊı¾İ.");
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
  *º¯ÊıÃû³Æ:void factory( uint8_t n )
  *¹¦ÄÜÃèÊö:»Ö¸´³ö³§ÉèÖÃ
  *Êä	Èë:	n	:0,Çå¿ÕËùÓĞÊı¾İ£»1£¬Çå¿ÕÊı¾İ£¬²¢ÇÒ»Ö¸´³ö³§ËùÓĞ²ÎÊı
  *Êä	³ö:none
  *·µ »Ø Öµ:rt_err_t
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
void factory( uint8_t n )
{
 factory_ex(n,0);
}
FINSH_FUNCTION_EXPORT( factory, reset to factory );



/*±£´æ²ÎÊıµ½serialflash*/
uint8_t param_save_old( uint8_t sem_get )
{
	uint8_t i;
	static JT808_PARAM jt808_param_fun;			//±¾º¯ÊıÄÚ²¿Ê¹ÓÃ£¬ÓÃÓÚ±È½Ï²ÒÊÇÆ¥Åä
	
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
		rt_kprintf( "¹Ø¼ü²ÎÊı±£´æÊ§°Ü retry=%d\n", i );
		reset( 1 );
		return  0;
	}
	return 1;
}


/*********************************************************************************
  *º¯ÊıÃû³Æ:uint8_t param_save_addr( uint8_t sem_get ,uint32_t addr)
  *¹¦ÄÜÃèÊö:±£´æÉè±¸Ö÷Òª²ÎÊıµ½flashÖĞ,²¢ÇÒÖ¸¶¨ÁË±£´æÔÚflashÖĞµÄÎïÀíµØÖ·
  *Êä	Èë:	sem_get	:0±íÊ¾²»ĞèÒª»ñÈ¡flash²Ù×÷ĞÅºÅÁ¿£¬1±íÊ¾ĞèÒª»ñÈ¡ĞÅºÅÁ¿
  			addr	:Ö÷Òª²ÎÊıµÄ´æ´¢Î»ÖÃ
  *Êä	³ö:	none
  *·µ »Ø Öµ:uint8_t :0±íÊ¾±£´æÊ§°Ü£¬1±íÊ¾±£´æ³É¹¦
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
uint8_t param_save_addr( uint8_t sem_get ,uint32_t addr)
{
	uint8_t i;
	static JT808_PARAM jt808_param_fun;			//±¾º¯ÊıÄÚ²¿Ê¹ÓÃ£¬ÓÃÓÚ±È½Ï²ÒÊÇÆ¥Åä
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
  *º¯ÊıÃû³Æ:static uint8_t param_save_ex( uint8_t sem_get ,uint8_t save_sequ)
  *¹¦ÄÜÃèÊö:±£´æÉè±¸Ö÷Òª²ÎÊıµ½flashÖĞ,²¢ÇÒÖ¸¶¨ÁË±£´æµÄÏÈºóË³Ğò(Ö÷´æ´¢ÇøÓòºÍ±¸·İÖ÷´æ´¢ÇøÓò)
  *Êä	Èë:	sem_get		:0±íÊ¾²»ĞèÒª»ñÈ¡flash²Ù×÷ĞÅºÅÁ¿£¬1±íÊ¾ĞèÒª»ñÈ¡ĞÅºÅÁ¿
  			save_sequ	:0±íÊ¾ÏÈ´æ´¢Ö÷ÇøÓò£¬ÔÙ´æ´¢Ö÷±¸·İÇøÓò£¬1±íÊ¾Ïà·´
  *Êä	³ö:	none
  *·µ »Ø Öµ:uint8_t :0±íÊ¾±£´æÊ§°Ü£¬ÆäËüÊı×Ö±íÊ¾±£´æ³É¹¦µÄÇøÓòÊıÁ¿£¬¹²Á½¸ö´æ´¢ÇøÓò¡£
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
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
		rt_kprintf("\n Éè±¸¹Ø¼ü²ÎÊı±£´æÊ§°Ü!");
	}
	else
	{
		rt_kprintf("\n%d>MAIN_PARA_SAVE OK!",rt_tick_get());
	}
	return i;
}


/*********************************************************************************
  *º¯ÊıÃû³Æ:uint8_t param_save( uint8_t sem_get )
  *¹¦ÄÜÃèÊö:±£´æÉè±¸Ö÷Òª²ÎÊıµ½flashÖĞ
  *Êä	Èë:	sem_get	:0±íÊ¾²»ĞèÒª»ñÈ¡flash²Ù×÷ĞÅºÅÁ¿£¬1±íÊ¾ĞèÒª»ñÈ¡ĞÅºÅÁ¿
  *Êä	³ö:	none
  *·µ »Ø Öµ:uint8_t :0±íÊ¾±£´æÊ§°Ü£¬ÆäËüÊı×Ö±íÊ¾±£´æ³É¹¦µÄÇøÓòÊıÁ¿£¬¹²Á½¸ö´æ´¢ÇøÓò¡£
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
uint8_t param_save( uint8_t sem_get )
{
	return param_save_ex(sem_get,0);
}
FINSH_FUNCTION_EXPORT( param_save, save param );





/*
   ¼ÓÔØ²ÎÊı´Óserialflash
   Õâ¸öÊ±ºò¿ÉÒÔ²»ÓÃsem_dataflash
   ÒòÎªÃ»ÓĞÆäËûÊ¹ÓÃ

 */
void param_load_old( void )
{
	/*Ô¤¶ÁÒ»²¿·ÖÊı¾İ*/
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
  *º¯ÊıÃû³Æ:void param_load( void )
  *¹¦ÄÜÃèÊö:´ÓflashÖĞ¼ÓÔØÉè±¸Ö÷Òª²ÎÊı£¬Õâ¸öÊ±ºò¿ÉÒÔ²»ÓÃsem_dataflash£¬ÒòÎªÃ»ÓĞÆäËûÊ¹ÓÃ
  *Êä	Èë: Read_addr	:¼ÓÔØ²ÎÊıµÄflashµØÖ·
  *Êä	³ö: none
  *·µ »Ø Öµ:uint8_t		:0±íÊ¾Êı¾İÍêÈ«OK,ÆäËüº¬ÒåÈçÏÂå£º
  						BIT0		±íÊ¾CRC´íÎó
  						BIT1		±íÊ¾Êı¾İ°æ±¾²»Ò»ÖÂ£¬ĞèÒªÉı¼¶Êı¾İ
  						BIT2		±íÊ¾Êı¾İ¸ñÊ½ÍêÈ«²»Æ¥Åä£¬ĞèÒªÖØĞÂ³õÊ¼»¯¸ÃÇøÓò²ÎÊı
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
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
  *º¯ÊıÃû³Æ:void param_load( void )
  *¹¦ÄÜÃèÊö:´ÓflashÖĞ¼ÓÔØÉè±¸Ö÷Òª²ÎÊı£¬Õâ¸öÊ±ºò¿ÉÒÔ²»ÓÃsem_dataflash£¬ÒòÎªÃ»ÓĞÆäËûÊ¹ÓÃ
  *Êä	Èë: none
  *Êä	³ö: none
  *·µ »Ø Öµ:none
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-12-16
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
void param_load( void )
{
	/*Ô¤¶ÁÒ»²¿·ÖÊı¾İ*/
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
  *º¯ÊıÃû³Æ:void factory_bkp_sram( void )
  *¹¦ÄÜÃèÊö:¸ñÊ½»¯bkp´æ´¢ÇøÓò
  *Êä	Èë:none
  *Êä	³ö:none
  *·µ »Ø Öµ:none
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-11-18
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
uint8_t factory_bksram( void )
{
 memset((uint8_t *)&jt808_param_bk, 0, sizeof(jt808_param_bk));
 jt808_param_bk.format_para	= BKSRAM_FORMAT;
 jt808_param_bk.data_version	= BKSRAM_VERSION;
 param_save_bksram();
}


/*********************************************************************************
  *º¯ÊıÃû³Æ:uint8_t param_save_bksram( void )
  *¹¦ÄÜÃèÊö:backup sram Êı¾İĞ´Èë¶¨ÒåµÄ½á¹¹Ìå
  *Êä	Èë:	none
  *Êä	³ö:	none
  *·µ »Ø Öµ:uint8_t	:	1:±íÊ¾²Ù×÷Ê§°Ü£¬	0:±íÊ¾²Ù×÷³É¹¦
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-11-18
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
*********************************************************************************/
uint8_t param_save_bksram( void )
{
	return bkpsram_write( 0, (uint8_t *)&jt808_param_bk, sizeof(jt808_param_bk));
}


/*********************************************************************************
  *º¯ÊıÃû³Æ:uint8_t param_load_bksram( void )
  *¹¦ÄÜÃèÊö:backup sram Êı¾İ¶ÁÈ¡¶¨ÒåµÄ½á¹¹Ìå
  *Êä	Èë:	none
  *Êä	³ö:	none
  *·µ »Ø Öµ:uint8_t	:	1:±íÊ¾¶ÁÈ¡Êı¾İÊ±·¢ÏÖÊı¾İÒÑ¾­¶ªÊ§£¬½øĞĞÁËÖØĞÂ¸ñÊ½»¯£¬	0:±íÊ¾¶ÁÈ¡³É¹¦
  *×÷	Õß:°×ÑøÃñ
  *´´½¨ÈÕÆÚ:2013-11-18
  *---------------------------------------------------------------------------------
  *ĞŞ ¸Ä ÈË:
  *ĞŞ¸ÄÈÕÆÚ:
  *ĞŞ¸ÄÃèÊö:
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

/*ÉèÖÃ²ÎÊı*/
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

/*Ğ´Èë×Ö·û´®*/
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
/*¶ÁÈ¡²ÎÊı,·µ»Ø²ÎÊıÀàĞÍ²ÎÊı*/
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

/*¶ÁÈ¡²ÎÊı*/
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
			case TYPE_DWORD:    /*×Ö½Ú¶ÔÆë·½Ê½ little_endian*/
				val |= ( *p++ );
				val |= ( ( *p++ ) << 8 );
				val |= ( ( *p++ ) << 16 );
				val |= ( ( *p ) << 24 );
				rt_kprintf( "\nid=%04x value=%08x", id, val );
				break;
			case TYPE_CAN:      /*8¸ö×Ö½Ú*/
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

/*´òÓ¡²ÎÊıĞÅÏ¢*/
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

/*ÊÖ¶¯ÉèÖÃapn*/
void apn( uint8_t *s )
{
	param_put_str( 0x0010, s );
}

FINSH_FUNCTION_EXPORT( apn, set apn );

/*ÉèÖÃÖ÷ip port*/
void ipport1( uint8_t *ip, uint16_t port )
{
	param_put_str( 0x0013, ip );
	param_put_int( 0x0018, port );
	param_save( 1 );
}

FINSH_FUNCTION_EXPORT( ipport1, set ipport );

/*»ñÈ¡³µÁ¾µÄmobileÖÕ¶ËÊÖ»úºÅ 6×Ö½Ú,²»×ã12Î»²¹Êı×Ö0*/
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

static uint16_t id_get = 1; /*±£´æµ±Ç°·¢ËÍµÄid*/


/*
   ¶Á²ÎÊıÊı¾İ
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
   ¶Á²ÎÊıÊı¾İ
 */
uint16_t get_param_and_fill_buf( uint8_t* pdest,uint16_t dest_pos )
{
	uint16_t	i,j;
	uint8_t		*p;
	uint8_t* 	pbuf;
	static uint8_t	last_para_data[64];			//±íÊ¾µ±Ç°°üµÄ×îºóÒ»´Î¶ÁÈ¡µ½µÄ²ÎÊı
	static uint8_t	last_para_len	= 0;				//±íÊ¾µ±Ç°°üµÄ×îºóÒ»´Î¶ÁÈ¡µ½µÄ²ÎÊı³¤¶È
	static uint8_t	last_para_rd	= 0;				//±íÊ¾µ±Ç°°üµÄ×îºóÒ»´Î¶ÁÈ¡µ½µÄ²ÎÊıµÄ¶ÁÈ¡Î»ÖÃ

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

/*Ìî³äÊı¾İ*/
void jt808_0x8104_fill_data( JT808_TX_NODEDATA *pnodedata )
{
	uint8_t		buf[600];
	uint16_t	count;

	count = get_param_and_fill_buf( buf,0 );              /*×Ö½ÚÌîÊı¾İ*/
	rt_kprintf( "\ncount=%d id_get=%d\n", count, id_get );

	pnodedata->packet_no++;
	if( pnodedata->packet_no == pnodedata->packet_num ) /*´ïµ½×îºóÒ»°ü*/
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

/*Ó¦´ğ
   Ìì½òÖĞĞÄ»áÊÕµ½Ó¦´ğ
 */
static JT808_MSG_STATE jt808_0x8104_response( JT808_TX_NODEDATA * pnodedata, uint8_t *pmsg )
{
	if( pnodedata->packet_num == pnodedata->packet_no ) /*ÒÑ¾­·¢ËÍÁËËùÓĞ°ü*/
	{
		rt_kprintf( "0x8104_response_delete\n" );
		pnodedata->state = ACK_OK;
	}
	rt_kprintf( "0x8104_response_idle\n" );
	jt808_0x8104_fill_data( pnodedata );
	return IDLE;
}

/*³¬Ê±ºóµÄ´¦Àíº¯Êı*/
static JT808_MSG_STATE jt808_0x8104_timeout( JT808_TX_NODEDATA * pnodedata )
{
	if( pnodedata->packet_num == pnodedata->packet_no ) /*ÒÑ¾­·¢ËÍÁËËùÓĞ°ü*/
	{
		rt_kprintf( "0x8104_timeout_delete\n" );
		pnodedata->state = ACK_OK;
	}
	rt_kprintf( "0x8104_timeout_idle\n" );
	jt808_0x8104_fill_data( pnodedata );
	return IDLE;
}

/*ÉÏ±¨ËùÓĞÖÕ¶Ë²ÎÊı*/
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
	/*¼ÆËã×ÜÊıºÍ×Ü´óĞ¡£¬²»Í³¼Æ0x0000ºÍ0xFxxxµÄ*/

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
	pnodedata->packet_num	= ( param_size + 511 ) / 512;   /*Ä¬ÈÏ512·Ö°ü*/
	pnodedata->packet_no	= 1;
	rt_kprintf( "\npacket_num=%d \n", pnodedata->packet_num );

	id_get	= 1;
	count	= get_param_and_fill_buf( buf , 3 );            /*¿Õ³öÈı¸ö×Ö½Ú£¬ÌîĞ´Ó¦´ğÁ÷Ë®ºÅ²ÎÊı×ÜÊı*/
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

/*·ÖÏî²éÑ¯£¬Ö»Ó¦´ğµ¥°ü*/
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
	/*¼ÆËã×ÜÊıºÍ×Ü´óĞ¡£¬²»Í³¼Æ0x0000ºÍ0xFxxxµÄ*/
	pdata	= pmsg + 13;    /*Ö¸ÏòÊı¾İÇø*/
	count	= 0;
	pbuf	= buf + 3;      /*¿Õ³öÈı¸ö×Ö½Ú£¬ÌîĞ´Ó¦´ğÁ÷Ë®ºÅ²ÎÊı×ÜÊı*/
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

	if( *( pmsg + 2 ) >= 0x20 ) /*Èç¹ûÊÇ¶à°üµÄÉèÖÃ²ÎÊı*/
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

		if( res ) /*ÓĞ´íÎó*/
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
