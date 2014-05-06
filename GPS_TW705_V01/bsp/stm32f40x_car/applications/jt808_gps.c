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
#include <stdio.h>

#include <board.h>
#include <rtthread.h>
#include <finsh.h>

#include "stm32f4xx.h"
#include "jt808.h"
#include "jt808_gps.h"
#include "jt808_param.h"
#include "rtc.h"
#include "jt808_util.h"
#include "jt808_vehicle.h"
#include "jt808_area.h"
#include "menu_include.h"
#include "jt808_gps_pack.h"
#include "vdr.h"
#include "math.h"
#include "jt808_config.h"

//#define GPS_GGHYPT


typedef struct _GPSPoint
{
	int sign;
	int deg;
	int min;
	int sec;
} GPSPoint;

uint32_t gps_sec_count = 0;                         /*gps���������*/

/*Ҫ��union���ͱ��棬λ�򣬷�����?*/
uint32_t		jt808_alarm			= 0x0;
uint32_t		jt808_alarm_last	= 0x0;          /*��һ�ε��ϱ�״̬*/

uint32_t		jt808_status		= 0x0;
uint32_t		jt808_status_last	= 0x0;          /*��һ�ε�״̬��Ϣ*/

static uint32_t jt808_report_interval	= 60;       /*GPS�ϱ�ʱ������Ϊ0:ֹͣ�ϱ�*/
static uint32_t jt808_report_distance	= 1000;     /*GPS�ϱ�������,Ϊ0 ֹͣ�ϱ�*/

static float	distance		= 0;                /*�����ϱ���ǰ����ֵ*/
static uint32_t total_distance	= 0;                /*�ܵ��ۼ����*/

uint16_t		jt808_8202_track_interval	= 0;    /*jt808_8202 ��ʱλ�ø��ٿ���*/
uint32_t		jt808_8202_track_duration	= 0;
uint16_t		jt808_8202_track_counter;

uint32_t		jt808_8203_manual_ack_seq	= 0;    /*�˹�ȷ�ϱ����ı�ʶλ 0,3,20,21,22,27,28*/
uint16_t		jt808_8203_manual_ack_value = 0;

#if 0


/*
   ����Ķ���,ʹ��list�������������node����Ļ���
   RAM�Ƿ���
   ʹ��dataflash�洢����4k��Ϊcache,�������
   ÿ���λ����Ϣ��Ҫ�ж�
 */
struct
{
	uint32_t	id;                 /*����ID*/
	uint16_t	attr;               /*����*/
	uint32_t	latitude;           /*����γ��*/
	uint32_t	logitude;           /*���ľ���*/
	uint32_t	radius;             /*�뾶*/
	uint8_t		datetime_start[6];  /*��ʼʱ�̣�ʹ��utc�ǲ��Ǹ���?*/
	uint8_t		datetime_end[6];
	uint16_t	speed;
	uint8_t		duration;           /*����ʱ��*/
} circle;

struct
{
	uint32_t	id;                 /*����ID*/
	uint16_t	attr;               /*����*/
	uint32_t	latitude;           /*����γ��*/
	uint32_t	logitude;           /*���ľ���*/
	uint32_t	radius;             /*�뾶*/
	uint8_t		datetime_start[6];  /*��ʼʱ�̣�ʹ��utc�ǲ��Ǹ���?*/
	uint8_t		datetime_end[6];
	uint16_t	speed;
	uint8_t		duration;           /*����ʱ��*/
} rectangle;

#endif

uint32_t	gps_lati;
uint32_t	gps_longi;
uint16_t	gps_speed;

uint16_t	gps_cog; /*course over ground*/
uint16_t	gps_alti;
uint8_t		gps_datetime[6];

/*��¼��һ�ε�λ�ã��������������*/
static uint32_t gps_lati_last	= 0;
static uint32_t gps_longi_last	= 0;

/*����gps����λ����Ϣ*/
GPS_BASEINFO	gps_baseinfo;
/*gps��״̬*/
GPS_STATUS		gps_status = { 0x3020, MODE_BDGPS, 0, 0x0, 0 };


/*
   Epochָ����һ���ض���ʱ�䣺1970-01-01 00:00:00 UTC
   UNIXʱ�����Unixʱ�����Ӣ��ΪUnix time, POSIX time �� Unix timestamp��
   �Ǵ�Epoch��1970��1��1��00:00:00 UTC����ʼ�����������������������롣

 */

uint32_t	utc_now		= 0;
MYTIME		mytime_now	= 0;

uint8_t		ACC_status;     /*0:ACC��   1:ACC��  */
uint32_t	ACC_ticks;      /*ACC״̬�����仯ʱ��tickֵ����ʱGPS����δ��λ*/

uint32_t	gps_notfixed_count = 0;

struct
{
	uint8_t		mode;       /*�ϱ�ģʽ 0:��ʱ 1:���� 2:��ʱ����*/
	uint8_t		userlogin;  /*�Ƿ�ʹ�õ�¼*/
	uint32_t	time_unlog;
	uint32_t	time_sleep;
	uint32_t	time_emg;
	uint32_t	time_default;
	uint32_t	distance_unlog;
	uint32_t	distance_sleep;
	uint32_t	distance_emg;
	uint32_t	distance_default;

	uint32_t	last_tick;      /*��һ���ϱ���ʱ��*/
	uint32_t	last_distance;  /*��һ���ϱ�ʱ�����*/
} jt808_report;
extern uint16_t	gps_reset_time;			///gpsģ��೤ʱ�䲻��λ��Ҫ��λ����λΪ��;

#define DEBUG_GPS

#ifdef DEBUG_GPS
uint8_t		speed_add	= 0;
uint32_t	speed_count = 0;
#endif

/*hmi���15�����ٶ�*/
static void process_hmi_15min_speed( void )
{
	static uint8_t	hmi_15min_speed_count	= 0;                                    /*��??��?����?????��y*/
	static uint32_t hmi_15min_speed_sum		= 0;                                    /*?��?����??��o��*/
	MYTIME		temp_mytime_now	= mytime_now;
	
	if( ( temp_mytime_now & 0xFFFFFFC0 ) > jt808_param_bk.speed_15min[jt808_param_bk.speed_curr_index].time ) /*D?����?��,??������?��??��*/
	{
		//hmi_15min_speed[hmi_15min_speed_curr].speed=hmi_15min_speed_sum/hmi_15min_speed_count;
		jt808_param_bk.speed_curr_index = (jt808_param_bk.speed_curr_index+1)%15;
		jt808_param_bk.speed_15min[jt808_param_bk.speed_curr_index].time	= temp_mytime_now & 0xFFFFFFC0;
		//rt_kprintf("\n%d>speed_curr_index=%d",rt_tick_get(),jt808_param_bk.speed_curr_index);
		hmi_15min_speed_sum		= 0;
		hmi_15min_speed_count	= 0;
	}
	else if( ( temp_mytime_now & 0xFFFFFFC0 ) + 10 < jt808_param_bk.speed_15min[jt808_param_bk.speed_curr_index].time )
	{
		jt808_param_bk.speed_curr_index = 0;
		memset(jt808_param_bk.speed_15min,0,sizeof(jt808_param_bk.speed_15min));
	}
	jt808_param_bk.speed_curr_index	%= 15;
	//jt808_param_bk.speed_15min[jt808_param_bk.speed_curr_index].time	= temp_mytime_now & 0xFFFFFFC0;
	hmi_15min_speed_sum	+= gps_speed;
	hmi_15min_speed_count++;
	jt808_param_bk.speed_15min[jt808_param_bk.speed_curr_index].speed = hmi_15min_speed_sum / hmi_15min_speed_count; /*??����?��D?*/
}

/*
   LinuxԴ���е�mktime�㷨����
 */
static __inline unsigned long linux_mktime( unsigned int year, unsigned int mon,
                                            unsigned int day, unsigned int hour,
                                            unsigned int min, unsigned int sec )
{
	if( 0 >= (int)( mon -= 2 ) )    /**//* 1..12 -> 11,12,1..10 */
	{
		mon		+= 12;              /**//* Puts Feb last since it has leap day */
		year	-= 1;
	}

	return ( ( ( (unsigned long)( year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day ) +
	             year * 365 - 719499
	             ) * 24 + hour      /**//* now have hours */
	           ) * 60 + min         /**//* now have minutes */
	         ) * 60 + sec;          /**//* finally seconds */
}

/*�������*/
float calc_distance( void )
{
	static uint32_t utc_distance_last = 0;
#if 0
	if( gps_lati_last )                             /*�״ζ�λ*/
	{
		distance				= dis_Point2Point( gps_lati_last, gps_longi_last, gps_lati, gps_longi );
		total_distance			+= distance;
		jt808_data.id_0xFA01	= total_distance;   /*�����m*/
	}
	gps_lati_last	= gps_lati;
	gps_longi_last	= gps_longi;
	/**/
	return distance;
#endif
/*�ٶȻ���*/
	if( ( ( utc_now - utc_distance_last ) == 1 ) || ( utc_distance_last == 0 ) )
	{
		distance				+= gps_speed / 3.6; /*��λkmh->m/s*/
		total_distance			+= gps_speed / 3.6;
		//jt808_data.id_0xFA01	= total_distance;   /*�����m*/
		jt808_data.id_0xFA01 	= jt808_param_bk.car_mileage / 36;		//��λΪM.
	}
	return distance;
}

#if 0
/**/
static double gpsToRad( GPSPoint point )
{
	return point.sign * ( point.deg + ( point.min + point.sec / 60.0 ) / 60.0 ) * 3.141592654 / 180.0;
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
static double getDistance( GPSPoint latFrom, GPSPoint lngFrom, GPSPoint latTo, GPSPoint lngTo )
{
	double	latFromRad	= gpsToRad( latFrom );
	double	lngFromRad	= gpsToRad( lngFrom );
	double	latToRad	= gpsToRad( latTo );
	double	lngToRad	= gpsToRad( lngTo );
	double	lngDiff		= lngToRad - lngFromRad;
	double	part1		= pow( cos( latToRad ) * sin( lngDiff ), 2 );
	//double part2 = pow( cos(latFromRad)*sin(latToRad)*cos(lngDiff) , 2);
	double	part2 = pow( cos( latFromRad ) * sin( latToRad ) - sin( latFromRad ) * cos( latToRad ) * cos( lngDiff ), 2 );

	double	part3 = sin( latFromRad ) * sin( latToRad ) + cos( latFromRad ) * cos( latToRad ) * cos( lngDiff );
	//double centralAngle = atan2( sqrt(part1 + part2) / part3 );
	double	centralAngle = atan( sqrt( part1 + part2 ) / part3 );
	return 6371.01 * 1000.0 * centralAngle; //Return Distance in meter
}

#endif


/*ֻ����1->0��д��*/
#define ATTR_NONET_NOREPORT 0xFF    /*δ���� δ�ϱ� */
#define ATTR_ONNET_NOREPORT 0xFE    /*�ѵ��� �ϱ�ʧ�� */
#define ATTR_REPORTED		0xFC    /*���ϱ� 11111100*/

typedef __packed struct
{
	uint32_t	mn;
	uint32_t	id;
	uint8_t		attr;
	uint8_t		unused;
	uint16_t	len;
	//uint32_t	msg_id;		/*�ϱ�����Ϣͷ0x0200*/
}GPS_REPORT_HEAD;

typedef struct
{
	uint32_t	addr;
	uint32_t	id;
	uint8_t		attr;
	uint16_t	len;
}REPORT_INFO;

REPORT_INFO		report_curr = { 0, 0, 0, 0 };

static uint32_t report_addr[20];            /*�ϱ���¼�ĵ�ַ*/
static uint8_t	report_get_count	= 0;    /*��ȡ��δ�ϱ���¼��*/
static uint32_t report_get_addr		= 0;    /*��ȡ��δ�ϱ���¼��Сid�ĵ�ַ*/

static uint16_t report_count = 0;           /*��û���ϱ��ļ�¼��*/
static uint8_t	report_trans_normal = 0;	//0��ʾ��ǰ���͵��ǲ�������(��������0x0704)��1��ʾ��������(����0x0200)��

extern STRUCT_GSM_PARAM gsm_param;


extern void sd_write_console(char *str);

/*********************************************************************************
  *��������:void gps_fact_set( void )
  *��������:��ʽ��GPS��λ���ݴ洢����
  *��	��:none
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-01
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void gps_fact_set(void)
{
 	u32					TempAddress;
	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
	for( TempAddress = ADDR_DF_POSTREPORT_START; TempAddress < ADDR_DF_POSTREPORT_END; )
	{
		sst25_erase_4k( TempAddress );
		TempAddress += 4096;
	}
	rt_sem_release( &sem_dataflash );
}

/*
   �ϱ���Ϣ�յ���Ӧ,ͨ��Ӧ�� 0x8001
 */
static JT808_MSG_STATE jt808_report_response( JT808_TX_NODEDATA * nodedata, uint8_t *pmsg )
{
	uint8_t			* msg = pmsg + 12;
	uint16_t		ack_id;
	uint16_t		ack_seq;
	uint8_t			ack_res;
	uint32_t		addr;

	GPS_REPORT_HEAD head;
	uint32_t		i;

	ack_seq = ( msg[0] << 8 ) | msg[1];
	ack_id	= ( msg[2] << 8 ) | msg[3];
	ack_res = *( msg + 4 );
	rt_kprintf("\r\n JT808 TX 0x0200 RESPONSE_1");
	if( ack_res == 0 )
	{
		rt_kprintf("_2");
		rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );
		if( ack_id == 0x0200 )              /*�����ϱ�*/
		{
			rt_kprintf("_3");
			addr = *( (uint32_t*)( nodedata->user_para ) );
			sst25_read( addr, (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
			rt_kprintf("   write addr=%X",addr);
			rt_kprintf("   head=");
			printf_hex_data((const u8 *)&head,sizeof(head));
			if(( head.mn == 0x474E5353 )&&(head.attr >= ATTR_ONNET_NOREPORT))
			{
				head.attr &= 0x7F;          /*���λ��0*/
				sst25_write_through( addr, (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
				report_get_addr = addr;
				if( report_count )
				{
					report_count--;
				}
			}
			//nodedata->user_para = RT_NULL;  /*���ݽ���ֻ��ָ��flash��ַ��������Ҫ�ͷ�*/
		}
		else if( ack_id == 0x0704 )              /*�����ϱ�*/
		{
			for( i = 0; i < report_get_count; i++ )
			{
				sst25_read( report_addr[i], (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
				if(( head.mn == 0x474E5353 )&&(head.attr >= ATTR_ONNET_NOREPORT))
				{
					head.attr &= 0x7F;      /*���λ��0*/
					sst25_write_through( report_addr[i], (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
					report_get_addr = report_addr[i];
					if( report_count )
					{
						report_count--;
					}
				}
			}
		}
		rt_sem_release( &sem_dataflash );
	}

	return ACK_OK;
}

/*
   ��Ϣ���ͳ�ʱ,��������Ϊ����
 */
static JT808_MSG_STATE jt808_report_timeout( JT808_TX_NODEDATA * nodedata )
{
	//nodedata->user_para = RT_NULL; /*���ݽ���ֻ��ָ��flash��ַ��������Ҫ�ͷ�*/
	return ACK_TIMEOUT;
}


/*********************************************************************************
  *��������:void jt808_report_init( void )
  *��������:�������һ��д���λ��,  ����δ�ϱ���¼��Сid��λ��, 
  			��ʼ�����ݴ洢��ز�������ʼ��GPSѹ���������ݴ洢��ز���(gps_pack_init)
  			ע��:�ú���ֻ��������ʼ���洢���򣬲�������ֻ�ܳ�ʼ��һ�εĴ��룬���粻�ܳ�ʼ���ź���
  			���ܳ�ʼ���̵߳�
  *��	��:none
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2014-03-5
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void jt808_report_init( void )
{
	uint32_t		addr;
	GPS_REPORT_HEAD head;
	uint32_t		read_id = 0xFFFFFFFF;
	
	gps_pack_init( );		///ֻ����Ҫѹ��ԭʼGPS���ݲ���Ҫ�ú���
	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );

	report_count		= 0; /*���²���*/
	report_curr.addr	= 0;
	report_curr.id		= 0;
	report_curr.len		= 0;

	memset( report_addr, 0, sizeof( report_addr ) );

	for( addr = ADDR_DF_POSTREPORT_START; addr < ADDR_DF_POSTREPORT_END; addr += 64 )
	{
		sst25_read( addr, (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
		if( head.mn == 0x474E5353 )         /*��Ч��¼*/
		{
			if( head.id >= report_curr.id ) /*���������¼,��ǰд���λ��*/
			{
				report_curr.id		= head.id;
				report_curr.addr	= addr; /*��ǰ��¼�ĵ�ַ*/
				report_curr.len		= head.len;
			}
			if( head.attr > 0x7F )          /*bit7=1 ��û�з���*/
			{
				report_count++;             /*ͳ��δ�ϱ���¼��*/
				if( read_id > head.id )     /*Ҫ�ҵ���Сid�ĵ�ַ*/
				{
					report_get_addr = addr;
					read_id			= head.id;
				}
			}
		}
	}
	if(report_count == 0)
		{
		report_get_addr =  report_curr.addr;
		}
	rt_sem_release( &sem_dataflash );
	rt_kprintf( "\n%d>(%d)д��λ��:%x id=%d ����λ��:%x id=%d",
	            rt_tick_get( ),
	            report_count,
	            report_curr.addr,
	            report_curr.id,
	            report_get_addr,
	            read_id );
}

/*�������ݱ���*/
void jt808_report_put( uint8_t* pinfo, uint16_t len )
{
	GPS_REPORT_HEAD head;
	uint32_t *p_addr;
	char tempbuf[64];
	
	if((report_curr.id == 0)&&((jt808_status & BIT_STATUS_FIXED) == 0))
		{
		gps_reset_time = 600;
		return;
		}
	
	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );

	if( report_curr.addr )                  /*���ȵ��ϱ���ַ*/
	{
		report_curr.addr	+= ( 12 + report_curr.len + 63 );	//��63��Ϊ��ȡһ���Ƚϴ��ֵ
		report_curr.addr	&= 0xFFFFFFC0;  /*ָ����һ����ַ*/
	}else                                   /*��û�м�¼*/
	{
		report_curr.addr = ADDR_DF_POSTREPORT_START;
	}

	if( report_curr.addr >= ADDR_DF_POSTREPORT_END )
	{
		report_curr.addr = ADDR_DF_POSTREPORT_START;
	}
	if( ( report_curr.addr & 0xFFF ) == 0 )
	{
		sst25_erase_4k( report_curr.addr );
	}

	report_curr.id++;
	report_curr.len = len;

	head.mn		= 0x474E5353;
	head.id		= report_curr.id;
	head.len	= len;

	if(( gsm_socket[0].state == CONNECTED )&&(jt808_state == JT808_REPORT))  /*��ǰ���ߣ����沢ֱ���ϱ�*/
	{
		head.attr = ATTR_ONNET_NOREPORT;
		//jt808_add_tx( 1, SINGLE_CMD, 0x0200, -1, jt808_report_timeout, jt808_report_response, len, pinfo, (void*)report_curr.addr );
	}else /*����Ҫ��������Ϣ*/
	{
		report_trans_normal = 0;
		head.attr = ATTR_NONET_NOREPORT;
	}

	sst25_write_through( report_curr.addr, (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
	sst25_write_through( report_curr.addr + 12, pinfo, len );
	rt_sem_release( &sem_dataflash );
	sprintf(tempbuf,"д��GPS����,ID=%05d,״̬=%08X,����=%08X,�ٶ�=%05d",head.id,jt808_status,jt808_param_bk.car_alarm,BYTESWAP2(gps_baseinfo.speed_10x));
	sd_write_console(tempbuf);
	report_count++;                     /*����δ�ϱ���¼��*/
	if( report_count == 1 )
	{
		report_get_addr = report_curr.addr;
		if(( gsm_socket[0].state == CONNECTED )&&(jt808_state == JT808_REPORT))
		{
			report_trans_normal = 1;		///��ʾ���������Ѿ�������ɣ������������������ʽ����
		}
	}
	//�����ǰ��������ݰ������һ�����ߵ�ǰ��������λ���������ʱλ�ø�����Ϣ������Ҫ�ϱ�
	if(( report_count == 1 )||( jt808_8202_track_duration ))             /*Ҫ�ҵ���С��*/
	{
		///���ע��ɹ�����������
		if(( gsm_socket[0].state == CONNECTED )&&(jt808_state == JT808_REPORT))
		{
			p_addr = rt_malloc(sizeof(uint32_t));
			if(p_addr != RT_NULL)
			{
				*p_addr = report_curr.addr;
				jt808_add_tx( 1, SINGLE_CMD, 0x0200, -1, jt808_report_timeout, jt808_report_response, len, pinfo, (void*)p_addr );
			}
		}
		//rt_kprintf( "\n%d>(%d)д��λ��:%x id=%d", rt_tick_get( ), report_count, report_curr.addr, report_curr.id );
	}
}

/*
   �������ݸ���ԭ�ȵĲ���ȡ�¼�¼
   ÿ���յ�����Ӧ������
   ÿ�����²��ųɹ���Ҫ����ȷ��һ��
   �����״ζ������ϱ��еĶ���¼

   ʹ��0704�ϱ���Ҫƴ��
 */
uint8_t jt808_report_get( void )
{
	GPS_REPORT_HEAD head;
	uint8_t			buf[1024];              /*�����ϱ����ֽڲ�����256*/
	uint32_t		i;
	uint32_t		addr;
	uint16_t		get_size;               /*��ȡ��¼�Ĵ�С*/
	uint16_t		pos;
	uint8_t			read_count;             /*Ҫ��ȡ�ļ�¼��*/
	uint32_t 		*p_addr;
	
	if( report_count == 0 )                 /*û����δ�ϱ���¼*/
	{
		return 0;
	}

	if(( gsm_socket[0].state != CONNECTED )||(jt808_state != JT808_REPORT))  /*������*/
	{
		return 0;
	}

	rt_sem_take( &sem_dataflash, RT_TICK_PER_SECOND * FLASH_SEM_DELAY );

/*�������Ҽ�¼*/
	addr				= report_get_addr;
	report_get_count	= 0;
	get_size			= 3;
	pos					= 3;                            /*0x0704���ݰ�����ʼ��ƫ��*/
	read_count			= report_count;                 /*��û�б��ļ�¼����*/
	if( report_count > sizeof(report_addr)/sizeof(uint32_t) )                             /*���1���ϱ�10����¼ 303�ֽ�*/
	{
		read_count = sizeof(report_addr)/sizeof(uint32_t);
	}
	for( i = 0; i < ADDR_DF_POSTREPORT_SECT * 64; i++ )   /*ÿ��¼64�ֽ�,ÿsector(4096Byte)��64��¼*/
	{
		sst25_read( addr, (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
		if( head.mn == 0x474E5353 )                     /*��Ч��¼*/
		{
			if( head.attr >= ATTR_ONNET_NOREPORT )      /*��û�з���*/
			{
				rt_kprintf("\r\njt808_report_get addr=%X",addr);
				rt_kprintf("   head=");
				printf_hex_data((const u8 *)&head,sizeof(head));
				///ä����������������0x0704������ֻ�й�������ƽ̨֧�ָ�����
				//�ж���Ҫ���͵��ܳ����Ƿ񳬹��������͵���󳤶�
				if( ( pos + head.len + 2 ) >= JT808_PACKAGE_MAX )
				{
					break;
				}
				buf[pos++]	= head.len >> 8;
				buf[pos++]	= head.len;
				sst25_read( addr + 12, buf + pos, head.len );
				pos								+= ( head.len );
				report_addr[report_get_count]	= addr;
				report_get_count++;
				if( report_get_count == read_count ) /*�յ�ָ����������*/
				{
					break;
				}
				#ifndef GPS_GGHYPT
				break;
				#endif
				if( report_trans_normal )		///������������������0x0200
				{
					break;
				}
			}
			addr	+= ( 12 + head.len + 63 );	//��63��Ϊ��ȡһ���Ƚϴ��ֵ
			addr	&= 0xFFFFFFC0;  			//ָ����һ����ַ
		}
		else
		{
			addr += 64;
		}
		if( addr >= ADDR_DF_POSTREPORT_END )
		{
			addr = ADDR_DF_POSTREPORT_START;
		}
	}
	rt_sem_release( &sem_dataflash );

	if( report_get_count ) /*�õ��ļ�¼��*/
	{
		if( report_trans_normal )		///������������������0x0200
		{
			p_addr = rt_malloc(sizeof(uint32_t));
			if(p_addr != RT_NULL)
			{
				*p_addr = addr;
				jt808_add_tx( 1, SINGLE_CMD, 0x0200, -1, jt808_report_timeout, jt808_report_response, pos-5, buf+5, (void*)p_addr );
			}
			return;
		}
		#ifdef GPS_GGHYPT
		buf[0]	= report_get_count >> 8;
		buf[1]	= report_get_count & 0xff;
		buf[2]	= 1;
		jt808_add_tx( 1, SINGLE_CMD, 0x0704, -1, RT_NULL, jt808_report_response, pos, buf, RT_NULL );
		#else
		p_addr = rt_malloc(sizeof(uint32_t));
		if(p_addr != RT_NULL)
		{
			*p_addr = addr;
			jt808_add_tx( 1, SINGLE_CMD, 0x0200, -1, jt808_report_timeout, jt808_report_response, pos-5, buf+5, (void*)p_addr );
		}
		#endif
		return 1;
	}else
	{
		report_count = 0; /*û�еõ�Ҫ�ϱ�������*/
	}
	return 0;
	//rt_kprintf( "\n%d>(%d)����λ��:%x,��¼��:%d,����:%d", rt_tick_get( ), report_count, report_get_addr, report_get_count, pos );
}

/*
   ����gps��Ϣ,�ж���������ϣ��ϱ�
   ��ʱ���յ���ȡ��
 */
//#define GPS_TEST

#define FLAG_SEND_AREA			0x80
#define FLAG_SEND_STATUS		0x01
#define FLAG_SEND_ALARM			0x02
#define FLAG_SEND_FIX_TIME		0x04
#define FLAG_SEND_FIX_DISTANCE	0x08

/*�����ϱ�*/
static uint8_t process_gps_report( void )
{
	static uint32_t utc_report_last = 0;
	uint32_t		tmp_s,tmp_a;
	uint8_t			flag_send	= 0; /*Ĭ�ϲ��ϱ�*/
	uint8_t			*palarmdata = RT_NULL;
	uint16_t		alarm_length;
	uint32_t		alarm_bits;
	uint8_t 		ret = 0;

	uint8_t			buf[300];

	if(jt808_param.id_0xF00F==0)
		return 0;

/*����·�ߴ���*/
	alarm_bits = area_get_alarm( palarmdata, &alarm_length );
	if( alarm_bits ) /*�и澯*/
	{
		rt_kprintf( "\n�����и澯" );
		memcpy( buf + sizeof(gps_baseinfo), palarmdata, alarm_length );
		flag_send = FLAG_SEND_AREA;
	}
	jt808_param_bk.car_alarm			|= alarm_bits;
	gps_baseinfo.alarm	= BYTESWAP4( jt808_param_bk.car_alarm );
	gps_baseinfo.status = BYTESWAP4( jt808_status );
	memcpy( buf, (uint8_t*)&gps_baseinfo, sizeof(gps_baseinfo) );

	tmp_s = jt808_status ^ jt808_status_last;				/*״̬λ�����仯*/
	tmp_a = ( jt808_param_bk.car_alarm ^ jt808_alarm_last );				/*�澯λ�仯*/
	tmp_s &=~(BIT_STATUS_FIXED);			///���Զ�λ��Ϣ��������м����׼�����
	jt808_status_last	= jt808_status;
	jt808_alarm_last	= jt808_param_bk.car_alarm;

/*����׷��,ֱ���ϱ���������*/
	if( jt808_8202_track_duration ) /*Ҫ׷��*/
	{
		jt808_8202_track_counter++;
		if( jt808_8202_track_counter >= jt808_8202_track_interval )
		{
			jt808_8202_track_counter = 0;
			utc_report_last = utc_now;
			//jt808_tx( 0x0200, buf, 28 + alarm_length );
			//rt_kprintf( "\n%d>�ϱ�gps(%02x)", rt_tick_get( ), flag_send );
			jt808_report_put( buf, sizeof(gps_baseinfo) + alarm_length );
			ret = 1;
			if( jt808_8202_track_duration > jt808_8202_track_interval )
			{
				jt808_8202_track_duration -= jt808_8202_track_interval;
			}else
			{
				jt808_8202_track_duration = 0;
				jt808_8202_track_interval = 0;
			}
		}
		return ret;
	}

/*�����ϱ���ʽ,�����ϳ�������� */
	if( tmp_s )                                       /*״̬�����仯��Ҫ�ϱ�,*/
	{
		flag_send				|= FLAG_SEND_STATUS;
		jt808_report_distance	= 0;
		jt808_report_interval	= 0;
		utc_report_last			= utc_now;          /*���¼�ʱ*/
		distance				= 0;                /*���¼������*/
	}
/*�ж��ϱ���ʽ�������·��޸Ĳ���ʱ��Ҫ��ʱ�ı�*/
	if( ( jt808_param.id_0x0020 & 0x01 ) == 0x00 )   /*�ж�ʱ�ϱ�*/
	{
		if( jt808_status & BIT_STATUS_ACC )         /*��ǰ״̬ΪACC��*/
		{
			jt808_report_interval = jt808_param.id_0x0029;
		}else
		{
			jt808_report_interval = jt808_param.id_0x0027;
		}
	}
	if( jt808_param.id_0x0020 )             /*�ж����ϱ�*/
	{
		if( jt808_status & BIT_STATUS_ACC ) /*��ǰ״̬ΪACC��*/
		{
			jt808_report_distance = jt808_param.id_0x002C;
		}else
		{
			jt808_report_distance = jt808_param.id_0x002E;
		}
	}

	if( tmp_a )                                           /*�澯�����仯�������ϱ�,*/
	{
		flag_send		|= FLAG_SEND_ALARM;
		utc_report_last = utc_now;
		distance		= 0;
	}

	if( jt808_param_bk.car_alarm & BIT_ALARM_EMG )                   /*�����澯*/
	{
		if( ( jt808_param.id_0x0020 & 0x01 ) == 0x0 )   /*�ж�ʱ�ϱ�*/
		{
			jt808_report_interval = jt808_param.id_0x0028;
		}
		if( jt808_param.id_0x0020 )                     /*�ж����ϱ�*/
		{
			jt808_report_distance = jt808_param.id_0x002F;
		}
	}

/*���㶨ʱ�ϱ�*/
	if( ( jt808_param.id_0x0020 & 0x01 ) == 0x0 )       /*�ж�ʱ�ϱ�*/
	{
		if( utc_now - utc_report_last >= jt808_report_interval )
		{
			flag_send		|= FLAG_SEND_FIX_TIME;
			utc_report_last = utc_now;
		}
	}
/*���㶨���ϱ�*/
	if( jt808_param.id_0x0020 ) /*�ж����ϱ�*/
	{
		if( distance >= jt808_report_distance )
		{
			flag_send	|= FLAG_SEND_FIX_DISTANCE;
			distance	= 0;
		}
	}
#ifdef GPS_TEST
	if(gps_speed)
		{
		flag_send = 1;
		}
#endif

/*����Ҫ�ϱ�������,����ʱֱ���ϱ������Ƕ��ݴ����ϱ������ߡ�*/
	if( flag_send )
	{
		//rt_kprintf( "\n%d>�ϱ�gps(%02x)", rt_tick_get( ), flag_send );
		ret = 1;
		jt808_report_put( buf, sizeof(gps_baseinfo) + alarm_length );
	}
	return ret;
}

/*
   $GNRMC,074001.00,A,3905.291037,N,11733.138255,E,0.1,,171212,,,A*655220.9*3F0E
   $GNTXT,01,01,01,ANTENNA OK*2B7,N,11733.138255,E,0.1,,171212,,,A*655220.9*3F0E
   $GNGGA,074002.00,3905.291085,N,11733.138264,E,1,11,0.9,8.2,M,-1.6,M,,,1.4*68E
   $GNGLL,3905.291085,N,11733.138264,E,074002.00,A,0*02.9,8.2,M,-1.6,M,,,1.4*68E
   $GPGSA,A,3,18,05,08,02,26,29,15,,,,,,,,,,,,,,,,,,,,,,,,,,1.6,0.9,1.4,0.9*3F8E
   $BDGSA,A,3,04,03,01,07,,,,,,,,,,,,,,,,,,,,,,,,,,,,,1.6,0.9,1.4,0.9*220.9*3F8E
   $GPGSV,2,1,7,18,10,278,29,05,51,063,08,21,052,24,02,24,140,45*4C220.9*3F8E
   $GPGSV,2,2,7,26,72,055,24,29,35,244,37,15,66,224,37*76,24,140,45*4C220.9*3F8E
   $BDGSV,1,1,4,04,27,124,38,03,42,190,34,01,38,146,37,07,34,173,35*55220.9*3F8E

   ���ش������ֶ����������ȷ�Ļ�
 */

static uint8_t process_rmc( uint8_t * pinfo )
{
	//�������������,ִ������ת��
	uint8_t		year = 0, mon = 0, day = 0, hour = 0, min = 0, sec = 0, fDateModify = 0;
	uint32_t	degrees, minutes;
	uint8_t		commacount = 0, count = 0;

	uint32_t	lati		= 0, longi = 0;
	uint16_t	speed_10x	= 0;
	uint16_t	cog			= 0;    /*course over ground*/

	uint8_t		i;
	uint8_t		buf[22];
	uint8_t		*psrc = pinfo + 6;  /*ָ��ʼλ�� $GNRMC,074001.00,A,3905.291037,N,11733.138255,E,0.1,,171212,,,A*655220.9*3F0E*/

/*��Ϊ������һ�Σ����Դ�pinfo+6��ʼ*/
	while( *psrc++ )
	{
		if( *psrc != ',' )
		{
			buf[count++]	= *psrc;
			buf[count]		= '0';
			buf[count+1]	= 0;
			continue;
		}

		commacount++;
		switch( commacount )
		{
			case 1: /*ʱ��*/
				if( count < 6 )
				{
					return 1;
				}

				i = ( buf[0] - 0x30 ) * 10 + ( buf[1] - 0x30 ) + 8;
				if( i > 23 )
				{
					fDateModify = 1;
					i			-= 24;
				}
				/*ת��HEX��ʽ*/
				hour	= i;
				min		= ( buf[2] - 0x30 ) * 10 + ( buf[3] - 0x30 );
				sec		= ( buf[4] - 0x30 ) * 10 + ( buf[5] - 0x30 );
				break;
			case 2:                         /*A_V*/
				if( buf[0] != 'A' )         /*δ��λ*/
				{
					jt808_status	&= ~BIT_STATUS_FIXED;
					gps_lati_last	= 0;    /*���¼������*/
					gps_longi_last	= 0;
					gps_speed		= 0;
					return 2;
				}

#if 0
				if( buf[0] == 'A' )
				{
					jt808_status |= BIT_STATUS_GPS;
				} else if( buf[0] == 'V' )
				{
					jt808_status &= ~BIT_STATUS_GPS;
				} else
				{
					return 2;
				}
#endif
				break;
			case 3: /*γ�ȴ���ddmm.mmmmmm*/
				if( count < 9 )
				{
					return 3;
				}

				degrees = ( ( buf [0] - 0x30 ) * 10 + ( buf [1] - 0x30 ) ) * 1000000;
				minutes = ( buf [2] - 0x30 ) * 1000000 +
				          ( buf [3] - 0x30 ) * 100000 +
				          ( buf [5] - 0x30 ) * 10000 +
				          ( buf [6] - 0x30 ) * 1000 +
				          ( buf [7] - 0x30 ) * 100 +
				          ( buf [8] - 0x30 ) * 10 +
				          ( buf [9] - 0x30 );   /*�����һ��λ����Ҫ��֤����*/
				lati = degrees + minutes / 6;
				break;
			case 4:                             /*N_S����*/
				if( buf[0] == 'N' )
				{
					jt808_status &= ~BIT_STATUS_NS;
				} else if( buf[0] == 'S' )
				{
					jt808_status |= BIT_STATUS_NS;
				}else
				{
					return 4;
				}
				break;
			case 5: /*���ȴ���*/
				if( count < 10 )
				{
					return 5;
				}
				degrees = ( ( buf [0] - 0x30 ) * 100 + ( buf [1] - 0x30 ) * 10 + ( buf [2] - 0x30 ) ) * 1000000;
				minutes = ( buf [3] - 0x30 ) * 1000000 +
				          ( buf [4] - 0x30 ) * 100000 +
				          ( buf [6] - 0x30 ) * 10000 +
				          ( buf [7] - 0x30 ) * 1000 +
				          ( buf [8] - 0x30 ) * 100 +
				          ( buf [9] - 0x30 ) * 10 +
				          ( buf [10] - 0x30 );
				longi = degrees + minutes / 6;
				break;
			case 6: /*E_W����*/
				if( buf[0] == 'E' )
				{
					jt808_status &= ~BIT_STATUS_EW;
				} else if( buf[0] == 'W' )
				{
					jt808_status |= BIT_STATUS_EW;
				}else
				{
					return 6;
				}
				break;
			case 7: /*�ٶȴ��� */
				speed_10x = 0;
				for( i = 0; i < count; i++ )
				{
					if( buf[i] == '.' )
					{
						if(i+1 < count)
							speed_10x += ( buf[i + 1] - 0x30 );
						break;
					}else
					{
						speed_10x	+= ( buf[i] - 0x30 );
						speed_10x	= speed_10x * 10;
					}
				}
				/*��ǰ��0.1knot => 0.1Kmh  1����=1.852Km  1852=1024+512+256+32+16+8+4*/
#ifdef DEBUG_GPS
				if( speed_count )
				{
					speed_10x += ( speed_add * 10 );
					speed_count--;
				}

#endif
				speed_10x *= 1.852;
				//i=speed_10x;
				//speed_10x=(i<<10)|(i<<9)|(i<<8)|(i<<5)|(i<<4)|(i<<3)|(i<<2);
				//speed_10x/=1000;

				gps_speed = speed_10x / 10;

				break;

			case 8: /*������*/
				cog = 0;
				for( i = 0; i < count; i++ )
				{
					if( buf[i] == '.' )
					{
						break;
					}else
					{
						cog = cog * 10;
						cog += ( buf[i] - 0x30 );
					}
				}
				break;

			case 9: /*���ڴ���*/
				if( count < 6 )
				{
					return 9;
				}

				day		= ( ( buf [0] - 0x30 ) * 10 ) + ( buf [1] - 0x30 );
				mon		= ( ( buf [2] - 0x30 ) * 10 ) + ( buf [3] - 0x30 );
				year	= ( ( buf [4] - 0x30 ) * 10 ) + ( buf [5] - 0x30 );

				if( fDateModify )
				{
					day++;
					if( mon == 2 )
					{
						if( ( year % 4 ) == 0 ) /*û�п�������ʱ��Ҫ��400������NM��2100��*/
						{
							if( day == 30 )
							{
								day = 1; mon++;
							}
						} else
						if( day == 29 )
						{
							day = 1; mon++;
						}
					} else
					if( ( mon == 4 ) || ( mon == 6 ) || ( mon == 9 ) || ( mon == 11 ) )
					{
						if( day == 31 )
						{
							mon++; day = 1;
						}
					} else
					{
						if( day == 32 )
						{
							mon++; day = 1;
						}
						if( mon == 13 )
						{
							mon = 1; year++;
						}
					}
				}

				/*���������˸��� gps_baseinfo,û�и߳���Ϣ*/
				gps_lati		= lati;
				gps_longi		= longi;
				gps_speed		= speed_10x / 10;
				gps_cog			= cog;
				gps_datetime[0] = year;
				gps_datetime[1] = mon;
				gps_datetime[2] = day;
				gps_datetime[3] = hour;
				gps_datetime[4] = min;
				gps_datetime[5] = sec;

				gps_baseinfo.alarm		= BYTESWAP4( jt808_param_bk.car_alarm );
				gps_baseinfo.status		= BYTESWAP4( jt808_status );
				gps_baseinfo.latitude	= BYTESWAP4( lati );
				gps_baseinfo.longitude	= BYTESWAP4( longi );
				gps_baseinfo.speed_10x	= BYTESWAP2( speed_10x );
				gps_baseinfo.cog		= BYTESWAP2( cog );

				//utc_now = linux_mktime( 2000 + year, mon, day, hour, min, sec );

				gps_baseinfo.datetime[0]	= HEX2BCD( year );
				gps_baseinfo.datetime[1]	= HEX2BCD( mon );
				gps_baseinfo.datetime[2]	= HEX2BCD( day );
				gps_baseinfo.datetime[3]	= HEX2BCD( hour );
				gps_baseinfo.datetime[4]	= HEX2BCD( min );
				gps_baseinfo.datetime[5]	= HEX2BCD( sec );

				//���ۼ�·�̽��л���
				jt808_param_bk.car_mileage	+= speed_10x;
				
				//������Ϣ�岿��
				gps_baseinfo.mileage_id		= 0x01;
				gps_baseinfo.mileage_len	= 0x04;
				gps_baseinfo.csq_id			= 0x30;
				gps_baseinfo.csq_len		= 0x01;
				gps_baseinfo.NoSV_id		= 0x31;
				gps_baseinfo.NoSV_len		= 0x01;
				gps_baseinfo.mileage		= BYTESWAP4(jt808_param_bk.car_mileage / 3600);
				gps_baseinfo.csq			= gsm_param.csq;
				gps_baseinfo.NoSV			= gps_status.NoSV;

				/*�״ζ�λ,Уʱ*/
				if( ( jt808_status & BIT_STATUS_FIXED ) == 0 )
				{
					mytime_now								= MYDATETIME( year, mon, day, hour, min, sec );
					//*(__IO uint32_t*)( BKPSRAM_BASE + 4 )	= utc_now;
					jt808_param_bk.utctime = mytime_to_utc(mytime_now);
					param_save_bksram();
					date_set( year, mon, day );
					time_set( hour, min, sec );
					rt_kprintf( "\n%d>rtc sync %02d-%02d-%02d %02d:%02d:%02d", rt_tick_get( ), year, mon, day, hour, min, sec );
					sd_write_console(pinfo);
				}
				#ifdef GPS_TEST
				else if(gps_speed)
				{
					sd_write_console(pinfo);
				}
				#endif

				if( gps_datetime[5] == 0 )      /*������*/
				{
					mytime_now								= MYDATETIME( year, mon, day, hour, min, sec );
					//*(__IO uint32_t*)( BKPSRAM_BASE + 4 )	= utc_now;
					jt808_param_bk.utctime = mytime_to_utc(mytime_now);
					param_save_bksram();
					if( gps_datetime[4] == 0 )  /*��СʱУ׼*/
					{
						date_set( year, mon, day );
						time_set( hour, min, sec );
					}
				}
				jt808_status |= BIT_STATUS_FIXED;
				return 0;
		}
		count	= 0;
		memset(buf,0,sizeof(buf));
	}
	return 10;
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
uint8_t process_gga( uint8_t * pinfo )
{
	//�������������,ִ������ת��
	uint8_t		NoSV;
	uint8_t		i;
	uint8_t		buf[20];
	uint8_t		commacount	= 0, count = 0;
	uint8_t		*psrc		= pinfo + 7; //ָ��ʼλ��
	uint16_t	altitute;

	while( *psrc++ )
	{
		if( *psrc != ',' )
		{
			buf[count++]	= *psrc;
			buf[count]		= 0;
			continue;
		}
		commacount++;
		switch( commacount )
		{
			case 1: /*ʱ�䴦�� */
				if( count < 6 )
				{
					return 1;
				}
				break;

			case 2: /*γ�ȴ���ddmm.mmmmmm*/
				break;

			case 3: /*N_S����*/
				break;

			case 4: /*���ȴ���*/

				break;
			case 5: /*E_W����*/
				break;
			case 6: /*��λ����*/
				break;
			case 7: /*NoSV,������*/
				if( count < 1 )
				{
					break;
				}
				NoSV = 0;
				for( i = 0; i < count; i++ )
				{
					NoSV	= NoSV * 10;
					NoSV	+= ( buf[i] - 0x30 );
				}
				gps_status.NoSV = NoSV;
				break;
			case 8: /*HDOP*/
				return 0;

			case 9: /*MSL Altitute*/
				if( count < 1 )
				{
					break;
				}
				altitute = 0;
				for( i = 0; i < count; i++ )
				{
					if( buf[i] == '.' )
					{
						break;
					}
					altitute	= altitute * 10;
					altitute	+= ( buf[i] - '0' );
				}
				gps_baseinfo.altitude	= altitute;
				gps_alti				= altitute;
				return 0;
		}
		count	= 0;
		buf[0]	= 0;
	}
	return 9;
}

/***********************************************************
* Function:
* Description:gps�յ���Ϣ��Ĵ�����ͷ�����ֽ�Ϊ����
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
void gps_rx( uint8_t * pinfo, uint16_t length )
{
	uint8_t ret;
	uint16_t i;
	char	* psrc;
	psrc = (char*)pinfo;

	*( psrc + length ) = 0;
	/*�Ƿ����ԭʼ��Ϣ*/
	if( gps_status.Raw_Output )
	{
		//rt_kprintf( "\n%d gps<%s", rt_tick_get( ), psrc );
		rt_kprintf( "%s",psrc );
	}
	if(length < 6)
		return;
	/*����RAW����*/
	//jt808_gps_pack( (char*)pinfo, length );

	if( strncmp( psrc + 3, "GGA,", 4 ) == 0 )
	{
		if( strncmp( psrc + 1, "GN", 2 ) == 0 )
		{
			gps_status.mode = MODE_BDGPS;
		}else if( strncmp( psrc + 1, "GP", 2 ) == 0 )
		{
			gps_status.mode = MODE_GPS;
		}else if( strncmp( psrc + 1, "BD", 2 ) == 0 )
		{
			gps_status.mode = MODE_BD;
		}
		process_gga( (uint8_t*)psrc );
	}

	if( strncmp( psrc + 3, "RMC,", 4 ) == 0 )
	{
		gps_sec_count++;
		
		//ȥ������Ĳ��ɼ��ַ�
		for(i=length-1;i>0;i--)
			{
			if(pinfo[i] > 0x1F)
				break;
			else
				{
				pinfo[i] = 0;
				}
			}
		ret = process_rmc( (uint8_t*)psrc );

		if( ret == 0 )                   /*�Ѷ�λ*/
		{
			gps_notfixed_count = 0;
			if(jt808_param.id_0xF00F)
			{
				process_hmi_15min_speed( ); /*���15�����ٶ�*/
				vdr_rx_gps( );              /*�г���¼�����ݴ���*/
				area_process( );            /*������·�澯*/
				calc_distance( );
			}
		}else
		{
			if(gps_notfixed_count%10 == 0)
				sd_write_console(psrc);
			//adjust_mytime_now( );       /*����mytime_now*/
			gps_notfixed_count++;
		}
		process_gps_report( );      	/*����GPS�ϱ���Ϣ*/
	}


	/*���߿���·��� gps<
	   $GNTXT,01,01,01,ANTENNA OK*2B
	   $GNTXT,01,01,01,ANTENNA OPEN*3B
	 */
	#if 0
	if( strncmp( psrc + 3, "TXT", 3 ) == 0 )
	{
		/*
		if( jt808_param.id_0xF013 != 0x3017 ) ///�ͺŲ���,�����µģ�������
		{
			rt_kprintf( "\njt808_param.id_0xF013=%04x", jt808_param.id_0xF013 );
			jt808_param.id_0xF013 = 0x3017;
			param_save( 1 );
			reset( 3 );
		}
		*/
		if( strncmp( psrc + 24, "OK", 2 ) == 0 )
		{
			gps_status.Antenna_Flag = 0;
			jt808_param_bk.car_alarm				&= ~( BIT_ALARM_GPS_OPEN | BIT_ALARM_GPS_SHORT );
		}else if( strncmp( psrc + 24, "OPEN", 4 ) == 0 )
		{
			gps_status.Antenna_Flag = 1;
			jt808_param_bk.car_alarm				|= BIT_ALARM_GPS_OPEN; /*bit5 ���߿�·*/
		}else if( strncmp( psrc + 24, "SHORT", 4 ) == 0 )
		{
			gps_status.Antenna_Flag = 1;
			jt808_param_bk.car_alarm				|= BIT_ALARM_GPS_SHORT;
		}
	}
	#endif
}

/*��ʼ��jt808 gps��صĴ���*/
void jt808_gps_init( void )
{
	GPS_REPORT_HEAD head;
	rt_kprintf( "\n%d>jt808_gps_init",rt_tick_get() );
	jt808_report_init( );   /*�ϱ������ʼ��*/
	jt808_vehicle_init( );  /*������Ϣ��ʼ��*/
	area_init( );
	///����������һ����λλ������Ϊ��ǰλ��
	memset(&gps_baseinfo,0,sizeof(gps_baseinfo));
	if( report_curr.addr )
	{
		sst25_read( report_curr.addr, (uint8_t*)&head, sizeof( GPS_REPORT_HEAD ) );
		if( head.mn == 0x474E5353 )                     /*��Ч��¼*/
		{
			sst25_read( report_curr.addr + 12, (uint8_t*)&gps_baseinfo, sizeof(gps_baseinfo) );
			gps_baseinfo.status &= ~(BYTESWAP4( BIT_STATUS_FIXED ));			///��ǰ��λ��Ϣ����Ϊδ��λ��
			
			if( jt808_param_bk.car_mileage == 0 )
			{
				jt808_param_bk.car_mileage	= gps_baseinfo.mileage;
				jt808_param_bk.car_mileage	*= 3600LL;
			}
		}
	}
	gps_baseinfo.mileage_id		= 0x01;
	gps_baseinfo.mileage_len	= 0x04;
	gps_baseinfo.csq_id			= 0x30;
	gps_baseinfo.csq_len		= 0x01;
	gps_baseinfo.NoSV_id		= 0x31;
	gps_baseinfo.NoSV_len		= 0x01;

	jt808_param_bk.car_alarm	&=(BIT(0) | BIT(3) | BIT(20) | BIT(21) | BIT(22) | BIT(27) | BIT(28));
	jt808_alarm_last			= jt808_param_bk.car_alarm;          /*��һ�ε��ϱ�״̬*/
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
void gps_dump( uint8_t mode )
{
	gps_status.Raw_Output = ~gps_status.Raw_Output;
}
FINSH_FUNCTION_EXPORT( gps_dump, dump gps raw info );




/************************************** The End Of File **************************************/

#ifdef DEBUG_GPS
/**ģ�����gps�ٶ�*/
void gps_speed_add( uint32_t sp, uint32_t count )
{
	speed_add	= sp * 1000 / 1852;
	speed_count = count;
	rt_kprintf("\n GPG speed add=%d",sp);
}

FINSH_FUNCTION_EXPORT_ALIAS( gps_speed_add, gps_speed, debug gps speed );
#endif

/************************************** The End Of File **************************************/