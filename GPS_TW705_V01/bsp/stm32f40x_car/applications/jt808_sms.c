/************************************************************
 * Copyright (C), 2008-2012,
 * FileName:		//  jt808_sms.c
 * Author:			//  baiyangmin
 * Date:			//  2013-07-08
 * Description:		//  ���Ŵ������ͣ����գ��޸Ĳ����ȹ���
 * Version:			//  V0.01
 * Function List:	//  ��Ҫ�������书��
 *     1. -------
 * History:			//  ��ʷ�޸ļ�¼
 *     <author>  <time>   <version >   <desc>
 *     David    96/10/12     1.0     build this moudle
 ***********************************************************/

#include <rtthread.h>
#include <rthw.h>
#include "stm32f4xx.h"
#include <finsh.h>

#include  <stdlib.h> //����ת�����ַ���
#include  <stdio.h>
#include  <string.h>
#include  "jt808_sms.h"
#include "jt808.h"
#include "jt808_param.h"
#include "jt808_gps.h"
#include "m66.h"
#include "gps.h"

#define SMS_CMD_RESET		0x01			///��λ�豸
#define SMS_CMD_RELINK		0x02		///��������ģ��
#define SMS_CMD_FACTSET		0x04		///�ָ���������
#define SMS_CMD_SAVEPARAM	0x08		///���������flash��
#define SMS_CMD_FACTSETDATA	0x10		///�ָ���������


#define HALF_BYTE_SWAP( a ) ( ( ( ( a ) & 0x0f ) << 4 ) | ( ( a ) >> 4 ) )
#define GSM_7BIT	0x00
#define GSM_UCS2	0x08

static const char	sms_gb_data[]	= "������������ش�������������ɽ��ʹ����������³���������ԥ";
static uint16_t		sms_ucs2[31]	= { 0x5180, 0x4EAC, 0x6D25, 0x6CAA, 0x5B81, 0x6E1D, 0x743C, 0x85CF, 0x5DDD, 0x7CA4, 0x9752,
	                                    0x8D35,		   0x95FD, 0x5409, 0x9655, 0x8499, 0x664B, 0x7518, 0x6842, 0x9102, 0x8D63,
	                                    0x6D59,		   0x82CF, 0x65B0, 0x9C81, 0x7696, 0x6E58, 0x9ED1, 0x8FBD, 0x4E91, 0x8C6B, };

static char			sender[32];			///���յ��Ķ���Դ���룬�ú���ΪPDU��ʽ����
static char			smsc[32];			///���յ��Ķ������ĺ��룬�ú���ΪPDU��ʽ����
static char 		sms_command = 0;		///��ʾ���յ���������ز�������Ϊ��Ҫ���Ͷ��ųɹ����ִ�в�����������Ҫ�ȴ�һ��ʱ��

/**/
u16  gsmencode8bit( const char *pSrc, char *pDst, u16 nSrcLength )
{
	u16 m;
	// �򵥸���
	for( m = 0; m < nSrcLength; m++ )
	{
		*pDst++ = *pSrc++;
	}

	return nSrcLength;
}

/**/
static uint16_t ucs2_to_gb2312( uint16_t uni )
{
	uint8_t i;
	for( i = 0; i < 31; i++ )
	{
		if( uni == sms_ucs2[i] )
		{
			return ( sms_gb_data[i * 2] << 8 ) | sms_gb_data[i * 2 + 1];
		}
	}
	return 0xFF20;
}

/**/
static uint16_t gb2312_to_ucs2( uint16_t gb )
{
	uint8_t i;
	for( i = 0; i < 31; i++ )
	{
		if( gb == ( ( sms_gb_data[i * 2] << 8 ) | sms_gb_data[i * 2 + 1] ) )
		{
			return sms_ucs2[i];
		}
	}
	return 0;
}

/*ucs2�����ı���*/
u16 gsmdecodeucs2( char* pSrc, char* pDst, u16 nSrcLength )
{
	u16			nDstLength = 0; // UNICODE���ַ���Ŀ
	u16			i;
	uint16_t	uni, gb;
	char		* src	= pSrc;
	char		* dst	= pDst;

	for( i = 0; i < nSrcLength; i += 2 )
	{
		uni = ( src[i] << 8 ) | src[i + 1];
		if( uni > 0x80 ) /**/
		{
			gb			= ucs2_to_gb2312( uni );
			*dst++		= ( gb >> 8 );
			*dst++		= ( gb & 0xFF );
			nDstLength	+= 2;
		}else /*Ҳ��˫�ֽڵ� ���� '1' ==> 0031*/
		{
			*dst++ = uni;
			nDstLength++;
		}
	}
	return nDstLength;
}

/**/
u16 gsmencodeucs2( u8* pSrc, u8* pDst, u16 nSrcLength )
{
	uint16_t	len = nSrcLength;
	uint16_t	gb, ucs2;
	uint8_t		*src	= pSrc;
	uint8_t		*dst	= pDst;

	while( len )
	{
		gb = *src++;
		if( gb > 0x7F )
		{
			gb		|= *src++;
			ucs2	= gb2312_to_ucs2( gb ); /*�п���û���ҵ�����00*/
			*dst++	= ( ucs2 >> 8 );
			*dst++	= ( ucs2 & 0xFF );
			len		-= 2;
		}else
		{
			*dst++	= 0x00;
			*dst++	= ( gb & 0xFF );
			len--;
		}
	}
	// ����Ŀ����봮����
	return ( nSrcLength << 1 );
}

/**/
u16 gsmdecode7bit( char* pSrc, char* pDst, u16 nSrcLength )
{
	u16 nSrc;   // Դ�ַ����ļ���ֵ
	u16 nDst;   // Ŀ����봮�ļ���ֵ
	u16 nByte;  // ��ǰ���ڴ���������ֽڵ���ţ���Χ��0-6
	u8	nLeft;  // ��һ�ֽڲ��������

	// ����ֵ��ʼ��
	nSrc	= 0;
	nDst	= 0;

	// �����ֽ���źͲ������ݳ�ʼ��
	nByte	= 0;
	nLeft	= 0;

	// ��Դ����ÿ7���ֽڷ�Ϊһ�飬��ѹ����8���ֽ�
	// ѭ���ô�����̣�ֱ��Դ���ݱ�������
	// ������鲻��7�ֽڣ�Ҳ����ȷ����
	while( nSrc < nSrcLength )
	{
		*pDst	= ( ( *pSrc << nByte ) | nLeft ) & 0x7f;    // ��Դ�ֽ��ұ߲��������������ӣ�ȥ�����λ���õ�һ��Ŀ������ֽ�
		nLeft	= *pSrc >> ( 7 - nByte );                   // �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
		pDst++;                                             // �޸�Ŀ�괮��ָ��ͼ���ֵ
		nDst++;
		nByte++;                                            // �޸��ֽڼ���ֵ
		if( nByte == 7 )                                    // ����һ������һ���ֽ�
		{
			*pDst = nLeft;                                  // ����õ�һ��Ŀ������ֽ�
			pDst++;                                         // �޸�Ŀ�괮��ָ��ͼ���ֵ
			nDst++;
			nByte	= 0;                                    // �����ֽ���źͲ������ݳ�ʼ��
			nLeft	= 0;
		}
		pSrc++;                                             // �޸�Դ����ָ��ͼ���ֵ
		nSrc++;
	}
	*pDst = 0;
	return nDst;                                            // ����Ŀ�괮����
}


/*��Ϣ����Ϊ7bit pduģʽ,������*/


/***********************************************************
* Function:
* Description:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
void sms_encode_pdu_7bit( char* info ,char *sendnum )
{
#define BITMASK_7BITS 0x7F

	char			buf[200];
	uint16_t		len;
	char			*pSrc, *pDst;
	unsigned char	c;
	char			ASC[]	= "0123456789ABCDEF";
	uint8_t			count	= 0;

	unsigned char	nSrc	= 0, nDst = 0, nChar = 0;
	uint8_t			nLeft	= 0;
	char			DA_buf[32];

	memset(buf,0,sizeof(buf));
	memset(DA_buf,0,sizeof(DA_buf));
	if(((uint32_t)sendnum == 0)||(strlen(sendnum) == 0))
		{
		strcpy(DA_buf,sender);
		}
	else
		{
		strcpy(DA_buf,sendnum);
		}
	memset(buf,0,sizeof(buf));
	len = strlen( info );
	rt_kprintf( "\nSMS>ENCODE(%d):%s", len, info );

	//strcpy( buf, "0891683108200205F01100" );
	strcpy( buf, "001100" );
	count = strlen( DA_buf );
	strcat( buf, DA_buf );
	strcat( buf, "000000" );                                //	7bit����

	pSrc	= info;
	pDst	= buf + ( count + 12 + 2 );                     //sender����+12ASC+2Ԥ��TP����

	while( nSrc < ( len + 1 ) )
	{
		nChar = nSrc & 0x07;                                // ȡԴ�ַ����ļ���ֵ�����3λ
		if( nChar == 0 )                                    // ����Դ����ÿ���ֽ�
		{
			nLeft = *pSrc;                                  // ���ڵ�һ���ֽڣ�ֻ�Ǳ�����������������һ���ֽ�ʱʹ��
		}else
		{
			c		= ( *pSrc << ( 8 - nChar ) ) | nLeft;   // ���������ֽڣ������ұ߲��������������ӣ��õ�һ��Ŀ������ֽ�
			*pDst++ = ASC[c >> 4];
			*pDst++ = ASC[c & 0x0f];
			nLeft	= *pSrc >> nChar;                       // �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
			nDst++;
		}
		pSrc++;
		nSrc++;                                             // �޸�Դ����ָ��ͼ���ֵ
	}
	*pDst = 0;
	//buf[count + 28] = ASC[len >> 4];
	//buf[count + 29] = ASC[len & 0x0F];
	buf[count + 12] = ASC[len >> 4];
	buf[count + 13] = ASC[len & 0x0F];

	//rt_kprintf( "\nSMS>encode 7BIT:%s", buf );
	//sms_tx( buf, ( strlen( buf ) - 18 ) >> 1 );
	sms_tx( buf, ( strlen( buf ) - 2 ) >> 1 );				//��ΪCMGS���͵ĳ��Ȳ�����SCA�ĳ��ȣ��ڸ÷��Ͷ�����SCAΪ"00"�����Լ�2
}

/*��Ϣ����Ϊuscs2 pduģʽ,������*/
void sms_encode_pdu_ucs2( char* info ,char *sendnum)
{
	char		buf[200];
	uint16_t	len, gb, ucs2;
	char		*pSrc, *pDst;
	char		ASC[]	= "0123456789ABCDEF";
	uint8_t		count	= 0;
	uint8_t		TP_UL	= 0;
	char		DA_buf[32];

	memset(buf,0,sizeof(buf));
	memset(DA_buf,0,sizeof(DA_buf));
	if(((uint32_t)sendnum == 0)||(strlen(sendnum) == 0))
		{
		strcpy(DA_buf,sender);
		}
	else
		{
		strcpy(DA_buf,sendnum);
		}

	len = strlen( info );
	rt_kprintf( "\nSMS>ENCODE(%d):%s", len, info );

//	strcpy(buf,smsc,strlen(smsc));
	strcpy( buf, "001100" );	//�����ö������ķ�ʽ���ͣ�Ĭ�Ͼ����ø÷�������
	count = strlen( DA_buf );
	strcat( buf, DA_buf );
	strcat( buf, "00088F" );	//�������Һ��壬��һ��00��PID��Ĭ�Ͼ�Ϊ00��08ΪDCS��ʾUCS2���룻8FΪVP��ʾ��Ϣ��Ч�� (8F+1)*5 ���ӡ�
//	ucs2bit����
	pSrc	= info;
	pDst	= buf + ( count + 12 + 2 ); //sender����+12ASC+2Ԥ��TP����
	while( *pSrc )
	{
		gb = *pSrc++;
		if( gb > 0x7F )
		{
			gb		<<= 8;
			gb		|= *pSrc++;
			ucs2	= gb2312_to_ucs2( gb ); /*�п���û���ҵ�����00*/
			*pDst++ = ASC[( ucs2 & 0xF000 ) >> 12];
			*pDst++ = ASC[( ucs2 & 0x0F00 ) >> 8];
			*pDst++ = ASC[( ucs2 & 0x00F0 ) >> 4];
			*pDst++ = ASC[( ucs2 & 0x000F )];
		}else
		{
			*pDst++ = '0';
			*pDst++ = '0';
			*pDst++ = ASC[gb >> 4];
			*pDst++ = ASC[gb & 0x0F];
		}
		TP_UL += 2;
	}
	*pDst			= 0;
	buf[count + 12] = ASC[TP_UL >> 4];
	buf[count + 13] = ASC[TP_UL & 0x0F];

	//rt_kprintf( "\nSMS>encode UCS2:%s", buf );
	sms_tx( buf, ( strlen( buf ) - 2 ) >> 1 );						//��ΪCMGS���͵ĳ��Ȳ�����SCA�ĳ��ȣ��ڸ÷��Ͷ�����SCAΪ"00"�����Լ�2
}


/*********************************************************************************
  *��������:void set_phone_num(char * str)
  *��������:����Ҫ���͵�Ŀ���ƶ��豸����
  *��	��:	str	:Ŀ���ֻ�����
  *��	��:	none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-11-25
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void set_phone_num(char * str)
{
	uint8_t		i,j;
	uint8_t 	len=strlen(str);
	char *pbuf;
	
	memset(sender,0,sizeof(sender));
	//memcpy(sender,"0D9068",6);
	sprintf(sender,"%02X9168",len+2);
	//memcpy(sender,"0D9168",6);
	memcpy(sender+6,str,len);
	if(len%2)
		sender[6+len] = 'F';
	pbuf = sender + 6;
	for(i=0;i<len;i+=2)
		{
		j=pbuf[i];
		pbuf[i]=pbuf[i+1];
		pbuf[i+1]=j;
		}
	rt_kprintf("\n phont_num=%s",sender);
}
FINSH_FUNCTION_EXPORT( set_phone_num, sms set_phone_num );


/*********************************************************************************
  *��������:void sms_send(char * phone_num,char *str)
  *��������:���Ͷ��ţ��Զ�ʶ�����7BIT���뻹��UCS2����
  *��	��:	phone_num	:Ŀ���ֻ�����
  			str			:��������
  *��	��:	none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-11-25
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void sms_send(char *str,char * phone_num)
{
 uint8_t i;
 char	buf[162];
 int	len=0;
 
 if(((uint32_t)phone_num )&&(strlen(phone_num)))
	{
	set_phone_num(phone_num);
	}
 
 for(i=0;i<strlen(str);i++)
 	{
 	if(str[i] > 0x7F)
 		{
 		sms_encode_pdu_ucs2(str,0);
		return;
 		}
 	}
 while(strlen(str))
 	{
 	memset(buf,0,sizeof(buf));
	
 	if(strlen(str)>160)
		{
		len	= 160;
 		}
	else
		{
		len	= strlen(str);
		}

	memcpy(buf,str,len);
	sms_encode_pdu_7bit(str,0);
	str+=len;
 	}
}
FINSH_FUNCTION_EXPORT( sms_send, sms_send );


/*��������,��Ҫ�󷵻�
   ���ڲ�ѯ����ֱ�ӷ��� ����0������ҪӦ����ڲ���Ӧ��
   ���������1,���ɵ��÷�����,��Ҫͨ��Ӧ��


 */
uint8_t analy_param( char* cmd, char*value ,uint8_t send_sms)
{
	char buf[160];
	char ip[32];
	int port;
	uint32_t id;
	
	if( strlen( cmd ) == 0 )
	{
		return 0;
	}
	rt_kprintf( "\nSMS>cmd=%s,value=%s", cmd, value );
/*���Ҳ���������*/
	if( strncmp( cmd, "TIREDCLEAR", 10 ) == 0 )
	{
		jt808_param_bk.utc_over_time_start=0;
		return 1;
	}
	if( strncmp( cmd, "DISCLEAR", 8 ) == 0 )
	{
		jt808_param_bk.car_mileage=0;
		jt808_data.id_0xFA01 = 0;
		data_save();
		return 1;
	}
	if( strncmp( cmd, "RESET", 5 ) == 0 )               /*Ҫ��λ,������ɺ�λ�������?*/
	{
		sms_command |= SMS_CMD_RESET;
		return 1;
	}else if( strncmp( cmd, "CLEARREGIST", 11 ) == 0 )  /*���ע��*/
	{
		memset( jt808_param.id_0xF003, 0, 32 );         /*�����Ȩ��*/
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}


/*�������ģ�������û�в���*/
	if( strlen( value ) == 0 )
	{
		return 0;
	}
/*���Ҷ�Ӧ�Ĳ���*/
	if( strncmp( cmd, "DNSR", 4 ) == 0 )
	{
		if( cmd[4] == '1' )
		{
			strcpy( jt808_param.id_0x0013, value );
		}
		if( cmd[4] == '2' )
		{
			strcpy( jt808_param.id_0x0017, value );
		}
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	else if( strncmp( cmd, "PORT", 4 ) == 0 )
	{
		if( cmd[4] == '1' )
		{
			jt808_param.id_0x0018=atoi( value );
			sms_command |= SMS_CMD_SAVEPARAM;
		}
		if( cmd[4] == '2' )
		{
			jt808_param.id_0xF031=atoi( value );
			sms_command |= SMS_CMD_SAVEPARAM;
		}
		if( cmd[4] == '3' )
		{
			jt808_param_bk.update_port=atoi( value );
			param_save_bksram();
		}
		return 1;
	}
	else if( strncmp( cmd, "IP", 2 ) == 0 )
	{
		jt808_param_bk.updata_utctime = 0;
		if( cmd[2] == '1' )
		{
			strcpy( jt808_param.id_0x0013, value );
			sms_command |= SMS_CMD_SAVEPARAM;
		}
		if( cmd[2] == '2' )
		{
			strcpy( jt808_param.id_0x0017, value );
			sms_command |= SMS_CMD_SAVEPARAM;
		}
		if( cmd[2] == '3' )
		{
			strcpy( jt808_param_bk.update_ip, value );
			jt808_param_bk.updata_utctime = 7200;
			//gsmstate( GSM_POWEROFF );
		}
		sms_command |= SMS_CMD_RELINK;
		param_save_bksram();
		return 1;
	}
	else if( strncmp( cmd, "DUR", 3 ) == 0 )
	{
		jt808_param.id_0x0029=atoi(value);
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	else if( strncmp( cmd, "SIMID", 6 ) == 0 )
	{
		strcpy( jt808_param.id_0xF006, value );
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	else if( strncmp( cmd, "DEVICEID", 8 ) == 0 )
	{
		strcpy( jt808_param.id_0xF002, value ); /*�ն�ID ��д��ĸ+����*/
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	else if( strncmp( cmd, "MODE", 4 ) == 0 )        ///6. ���ö�λģʽ
	{
		if( strncmp( value, "BD", 2 ) == 0 )
		{
			gps_status.mode = MODE_BD;
		}
		if( strncmp( value, "GP", 2 ) == 0 )
		{
			gps_status.mode = MODE_GPS;
		}
		if( strncmp( value, "GN", 2 ) == 0 )
		{
			gps_status.mode= MODE_BDGPS;
		}
		gps_mode( gps_status.mode );
		return 1;
	}
	else if( strncmp( cmd, "VIN", 3 ) == 0 )
	{
		strcpy( jt808_param.id_0xF005, value ); /*VIN(TJSDA09876723424243214324)*/
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	/*	RELAY(0)�رռ̵��� �̵���ͨ RELAY(1) �Ͽ��̵��� �̵�����*/
	else if( strncmp( cmd, "RELAY", 5 ) == 0 )
	{
		return 1;
	}
	else if( strncmp( cmd, "TAKE", 4 ) == 0 )    /*���� 1.2.3.4·*/
	{	
		Cam_takepic(value[0]-'0',1,1,8);
		return 1;
	}
	else if( strncmp( cmd, "PLAY", 4 ) == 0 )    /*��������*/
	{
		tts_write( value, strlen( value ) );
		return 1;
	}
	else if(( strncmp( cmd, "QUERY", 5 ) == 0 )&&(send_sms))   /*0 ���������Ϣ 1 ����ϵͳ������Ϣ 2λ����Ϣ*/
	{
		/*ֱ�ӷ�����Ϣ*/
		if( *value == '0' )
		{
			sprintf( buf, "TCBTW703#%s#%s#%s#%d",
			         jt808_param.id_0x0083 + 2,
			         jt808_param.id_0xF002,
			         jt808_param.id_0xF005,
			         jt808_param.id_0x0029 );
			sms_send( buf, 0  );
		}

		if( *value == '1' )
		{
			sprintf( buf, "TCBTW703#%s#%s#%s#%d",
			         jt808_param.id_0x0083 + 2,
			         jt808_param.id_0xF002,
			         jt808_param.id_0x0013,
			         jt808_param.id_0x0018);
			sms_send( buf, 0  );
		}
		if( *value == '2' )
		{
			sprintf( buf, "TCBTW703#%s#%s#%s#%d",
			         jt808_param.id_0x0083 + 2,
			         jt808_param.id_0xF002,
			         jt808_param.id_0xF005,
			         jt808_param.id_0x0029 );
			sms_send( buf, 0  );
		}

		return 0;
	}
	else if( strncmp( cmd, "ISP", 3 ) == 0 )         /*	ISP(202.89.23.210��9000)*/
	{
		sscanf(value,"%s:%d",ip,&port);
		gsm_socket[2].index=2;
		strcpy(gsm_socket[2].ipstr,ip);
		gsm_socket[2].port=port;
		gsm_socket[2].state=CONNECT_IDLE;
		return 1;
	}
	else if( strncmp( cmd, "PLATENUM", 8 ) == 0 )    /*PLATENUM(��A8888)	*/
	{
		strcpy(jt808_param.id_0x0083,value);
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	else if( strncmp( cmd, "COLOR", 5 ) == 0 )       /* COLOR(0) 1�� ��ɫ  2�� ��ɫ    3�� ��ɫ    4�� ��ɫ    9��  ����*/
	{
		jt808_param.id_0x0084=atoi(value);
		sms_command |= SMS_CMD_SAVEPARAM;
		return 1;
	}
	else if( strncmp( cmd, "CONNECT", 7 ) == 0 )     /*CONNECT(0)  0:  DNSR   ����     1��  MainIP   ���� */
	{
		return 1;
	}
	else if( strncmp( cmd, "PASSWORD", 7 ) == 0 )    /*PASSWORD��0�� 1�� ͨ��   0�� ��ͨ��*/
	{
		return 1;
	}
	else if( strncmp( cmd, "FACT", 4 ) == 0 )   	/*1 ��ղ�����2�������*/
	{
		/*ֱ�ӷ�����Ϣ*/
		if( *value == '1' )
		{
			//factory(1);
			sms_command |= SMS_CMD_FACTSET;
		}

		else if( *value == '2' )
		{
			//factory(2);
			sms_command |= SMS_CMD_FACTSETDATA;
		}

		return 1;
	}
	else if(( strncmp( cmd, "0x", 2 ) == 0 )||( strncmp( cmd, "0X", 2 ) == 0 ))    /*����808Э��0x8103��Ӧ�Ĳ���*/
	{
		if(cmd[1]=='X')
		{
			cmd[1]='x';
		}
		if(sscanf( cmd, "%x", &id )==1)
		{
			if(jt808_param_set(id, value))
			{
				sms_command |= SMS_CMD_SAVEPARAM;
				return 1;
			}
		}
	}
	return 0;
}
FINSH_FUNCTION_EXPORT( analy_param, sms analy_param );

/*********************************************************************************
  *��������:void sms_param_proc(char *info_msg,uint8_t send_sms)
  *��������:�����յ��Ķ������ݣ���Ҫ�ظ����ŵĻ���Ҫsend_smsΪ��0ֵ
  *��	��:	info_msg:�յ��Ķ������ݣ����������Ѿ�����Ķ�������
  			send_sms:Ϊ0��ʾ���ظ����ţ���0��ʾ��Ҫ�ظ�����
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-13
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint8_t sms_param_proc(char *info_msg,uint8_t send_sms)
{
	uint16_t	i;
	uint16_t	count;
	uint8_t		cmd_step=0;
	char		*p;
	char		*psave;
	char		cmd[32];
	char		value[64];
	char		tmpbuf[180];        /*��������Ϣ*/
	
	/*decode_msg�����������Ϣ*/
		p = info_msg;
		if(( strncmp( p, "TW703", 5 ) != 0 )&&( strncmp( p, "TW705", 5 ) != 0 ))
		{
			return 0;
		}
		sprintf(tmpbuf,"SMS(%d,%s):%s",send_sms,sender,info_msg);
		sd_write_console(tmpbuf);
		p += 5;
		memset( cmd, 0, sizeof(cmd) );
		memset( value, 0, sizeof(value) );
		count	= 0;
		psave	= cmd;
		i		= 0;
	/*tmpbuf����Ӧ�����Ϣ TW703#*/
		while( *p != 0 )
		{
			switch( *p )
			{
				case '#':
					cmd_step = 1;
					memset( cmd, 0, sizeof(cmd) );
					memset( value, 0, sizeof(value) );
					psave	= cmd;		/*����cmd�����д�*/
					count	= 0;
					break;
				case '(':
					if(cmd_step == 1)
						cmd_step = 2;
					psave	= value;	/*��ȡֵ������*/
					count	= 0;
					break;
				case ')':				/*��ʾ����,������*/
					if(cmd_step == 2)
					{
						i += analy_param( cmd, value, send_sms );
						cmd_step = 0;
					}
					break;
				default:
					if( count < 64 )	/*����������*/
					{
						*psave++ = *p;
						count++;
					}
					break;
			}
			p++;
		}
		if((i)&&(send_sms))
		{
			memset(tmpbuf,0,sizeof(tmpbuf));
			sprintf( tmpbuf, "%s#%-7s#%s", jt808_param.id_0x0083, jt808_param.id_0xF002, info_msg + 6 );
	
			sms_send(tmpbuf,0);
		}
		if(sms_command)
		{
			if(sms_command & SMS_CMD_RESET)
			{
				//reset(4);
				control_device(DEVICE_RESET_DEVICE,0xFFFFFFFF);
				device_control.delay_counter = 60;
			}
			if(sms_command & SMS_CMD_RELINK)
			{
				gsmstate( GSM_POWEROFF );
			}
			if(sms_command & SMS_CMD_FACTSET)
			{
				factory(1);
				control_device(DEVICE_RESET_DEVICE,0xFFFFFFFF);
			}
			if(sms_command & SMS_CMD_SAVEPARAM)
			{
				param_save( 1 );
			}
			if(sms_command & SMS_CMD_FACTSETDATA)
			{
				factory(2);
				control_device(DEVICE_RESET_DEVICE,0xFFFFFFFF);
			}
			sms_command = 0;
		}
		return 1;

}


/*********************************************************************************
  *��������:void jt808_sms_rx( char *info, uint16_t size )
  *��������:�յ�����Ϣ����,���һظ���������
  *��	��:	info	:�յ��Ķ������ݣ���������PDU����Ķ�������
  			size	:info�ĳ���
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-13
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void jt808_sms_rx_old( char *info, uint16_t size )
{
	uint16_t	i;
	uint8_t		tbl[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
	char		*p, *pdst;
	char		c, pid;
	uint16_t	len;
	uint8_t		pdu_type, msg_len;
	uint8_t		dcs;

	char		decode_msg[180];    /*��������Ϣ*/
	uint16_t	decode_msg_len = 0;
	char		tmpbuf[180];        /*��������Ϣ*/

	char		cmd[64];
	char		value[64];
	uint8_t		count;
	char		*psave;
	uint8_t		cmd_step=0;

	memset( sender, 0, sizeof(sender) );
	memset( smsc, 0, sizeof(smsc) );
	p = info;                       /*���¶�λ����ʼ*/
	/*SCA*/
	len = tbl[p[0] - '0'] << 4;
	len |= tbl[p[1] - '0'];         /*�������ĺ��� Length (����)+Tosca�������������ͣ�+Address����ַ��*/
	
	if(len>12)
		return;
	len <<= 1;                      /*ת��˫��*/
	for( i = 0; i < len + 2; i++ )  /*��������*/
	{
		smsc[i] = *p++;
	}
	/*pdu����*/
	pdu_type	= tbl[*p++ - '0'] << 4;
	pdu_type	|= tbl[*p++ - '0'];

	/*OA��Դ��ַ 05 A1 0180F6*/
	/*M50ģ�����յ��ĵ�ַ�е�ַ����ΪA1,���͵�ʱ�򲻳ɹ�����Ҫת��Ϊ9186(91��ʾΪ���ʺ��룬86��ʾ�й�)���ܷ���ok*/
	len = tbl[p[0] - '0'] << 4;
	len |= tbl[p[1] - '0']; /*�ֻ�����Դ��ַ Length (����,�����ָ���)+Tosca����ַ���ͣ�+Address����ַ��*/
	
	if(len>21)
		return;
	if(strncmp(p+4,"68",2) != 0)
	{
		sprintf(sender,"%02X9168",len+2);
		if( len & 0x01 )
		{
			len++;              /*����������λ��������10086��5λ �������ֽڱ�ʾ*/
		}
		strncpy(sender+6,p+4,len);
	}
	else
	{
		if( len & 0x01 )
		{
			len++;              /*����������λ��������10086��5λ �������ֽڱ�ʾ*/
		}
		strncpy(sender,p,len+4);
	}
	p	+= 4+len;
	rt_kprintf("\nSMS>sender=%s",sender);

	/*PID��־*/
	pid = tbl[*p++ - '0'] << 4;
	pid |= tbl[*p++ - '0'];
	/*DCS����*/
	dcs = tbl[*p++ - '0'] << 4;
	dcs |= tbl[*p++ - '0'];
	/*SCTS��������ʱ��� yymmddhhmmsszz ����ʱ��*/
	p += 14;
	/*��Ϣ����*/
	//msg_len = tbl[*p++ - '0'] << 4;
	//msg_len |= tbl[*p++ - '0'];
	p += 2;
	
	if((uint32_t)info + size <= (uint32_t)p)
	{
		return;
	}
	msg_len = ((uint32_t)info + size - (uint32_t)p)/2;
	/*�Ƿ�Ϊ����Ϣpdu_type ��bit6 UDHI=1*/
	if( pdu_type & 0x40 )           /*��������Ϣ��ͷ*/
	{
		if( ( dcs & 0x0c ) != GSM_7BIT )		///��ʱ������7bit������������Ϊ7bit����Ը�����Ҳ������
			{
			len		= tbl[*p++ - '0'] << 4;
			len		|= tbl[*p++ - '0'];
			p		+= ( len << 1 );    /*���ڻ���OCTģʽ*/
			msg_len -= ( len + 1 );     /*����ռһ���ֽ�*/
			}
	}
	
	rt_kprintf( "\nSMS>PDU Type=%02x PID=%02x DCS=%02x TP-UDL=%d", pdu_type, pid, dcs, msg_len );

/*ֱ����info�ϲ�������?*/
	pdst = tmpbuf;
	/*��OCT����תΪHEX,�����ֽ�ƴ��һ��*/
	for( i = 0; i < msg_len; i++ )
	{
		c		= tbl[*p++ - '0'] << 4;
		c		|= tbl[*p++ - '0'];
		*pdst++ = c;
	}
	*pdst = 0;

	if( ( dcs & 0x0c ) == GSM_7BIT )
	{
		decode_msg_len = gsmdecode7bit( tmpbuf, decode_msg, msg_len );
		/*���û�����ͷ����ȥ��*/
		len = tmpbuf[0] + 1;		//�����û����ݣ������� �û�����ͷ����(1byte) + �û�����;
		if(len * 8 % 7)
			{
			len = len * 8 / 7 + 1;
			}
		else
			{
			len =  len * 8 / 7;
			}
		if(( pdu_type & 0x40 )&&(len < decode_msg_len))
			{
			decode_msg_len	-= len;
			memcpy(tmpbuf,decode_msg+len,decode_msg_len);
			memcpy(decode_msg,tmpbuf,decode_msg_len);
			}
	}else if( ( dcs & 0x0c ) == GSM_UCS2 )
	{
		decode_msg_len = gsmdecodeucs2( tmpbuf, decode_msg, msg_len );
	}else
	{
		memcpy( decode_msg, tmpbuf, msg_len );
		decode_msg_len = msg_len;   /*�򵥿�������*/
	}

	if( decode_msg_len == 0 )       /*���ڲ��ǺϷ��������ǲ���ֱ�ӷ���0*/
	{
		return;
	}
	decode_msg[decode_msg_len] = 0;

/*decode_msg�����������Ϣ*/
	p = decode_msg;
	rt_kprintf( "\nSMS(%dbytes)>%s", decode_msg_len, p );
	if(( strncmp( p, "TW703", 5 ) != 0 )&&( strncmp( p, "TW705", 5 ) != 0 ))
	{
		return;
	}
	p += 5;
	memset( cmd, 0, 64 );
	memset( value, 0, 64 );
	count	= 0;
	psave	= cmd;
	i		= 0;
/*tmpbuf����Ӧ�����Ϣ TW703#*/
	while( *p != 0 )
	{
		switch( *p )
		{
			case '#':
				cmd_step = 1;
				memset( cmd, 0, 64 );
				memset( value, 0, 64 );
				psave	= cmd;      /*����cmd�����д�*/
				count	= 0;
				break;
			case '(':
				if(cmd_step == 1)
					cmd_step = 2;
				psave	= value;    /*��ȡֵ������*/
				count	= 0;
				break;
			case ')':               /*��ʾ����,������*/
				if(cmd_step == 2)
				{
					i += analy_param( cmd, value, 1 ) ;
					cmd_step = 0;
				}
				break;
			default:
				if( count < 64 )    /*����������*/
				{
					*psave++ = *p;
					count++;
				}
				break;
		}
		p++;
	}

	if(i)
	{
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf( tmpbuf, "%s#%-7s#%s", jt808_param.id_0x0083, jt808_param.id_0xF002, decode_msg + 6 );

		sms_send(tmpbuf,0);
	}
	
	if(sms_command)
	{
		if(sms_command & SMS_CMD_RESET)
		{
			reset(4);
		}
		if(sms_command & SMS_CMD_RELINK)
		{
			gsmstate( GSM_POWEROFF );
		}
		if(sms_command & SMS_CMD_FACTSET)
		{
			rt_kprintf("SMS> �ָ���������;");
		}
		if(sms_command & SMS_CMD_SAVEPARAM)
		{
			param_save( 1 );
		}
		sms_command = 0;
	}
}

/*********************************************************************************
  *��������:void jt808_sms_rx( char *info, uint16_t size )
  *��������:�յ�����Ϣ����,���һظ���������
  *��	��:	info	:�յ��Ķ������ݣ���������PDU����Ķ�������
  			size	:info�ĳ���
  *��	��:none
  *�� �� ֵ:none
  *��	��:������
  *��������:2013-12-13
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
void jt808_sms_rx( char *info, uint16_t size )
{
	uint16_t	i;
	uint8_t		tbl[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
	char		*p, *pdst;
	char		c, pid;
	uint16_t	len;
	uint8_t		pdu_type, msg_len;
	uint8_t		dcs;

	char		decode_msg[180];    /*��������Ϣ*/
	uint16_t	decode_msg_len = 0;
	char		tmpbuf[180];        /*��������Ϣ*/

	//char		cmd[64];
	//char		value[64];
	//uint8_t		count;
	//char		*psave;
	//uint8_t		cmd_step=0;

	memset( sender, 0, sizeof(sender) );
	memset( smsc, 0, sizeof(smsc) );
	p = info;                       /*���¶�λ����ʼ*/
	/*SCA*/
	len = tbl[p[0] - '0'] << 4;
	len |= tbl[p[1] - '0'];         /*�������ĺ��� Length (����)+Tosca�������������ͣ�+Address����ַ��*/
	
	if(len>12)
		return;
	len <<= 1;                      /*ת��˫��*/
	for( i = 0; i < len + 2; i++ )  /*��������*/
	{
		smsc[i] = *p++;
	}
	/*pdu����*/
	pdu_type	= tbl[*p++ - '0'] << 4;
	pdu_type	|= tbl[*p++ - '0'];

	/*OA��Դ��ַ 05 A1 0180F6*/
	/*M50ģ�����յ��ĵ�ַ�е�ַ����ΪA1,���͵�ʱ�򲻳ɹ�����Ҫת��Ϊ9186(91��ʾΪ���ʺ��룬86��ʾ�й�)���ܷ���ok*/
	len = tbl[p[0] - '0'] << 4;
	len |= tbl[p[1] - '0']; /*�ֻ�����Դ��ַ Length (����,�����ָ���)+Tosca����ַ���ͣ�+Address����ַ��*/
	
	if(len>21)
		return;
	if(strncmp(p+4,"68",2) != 0)
	{
		sprintf(sender,"%02X9168",len+2);
		if( len & 0x01 )
		{
			len++;              /*����������λ��������10086��5λ �������ֽڱ�ʾ*/
		}
		strncpy(sender+6,p+4,len);
	}
	else
	{
		if( len & 0x01 )
		{
			len++;              /*����������λ��������10086��5λ �������ֽڱ�ʾ*/
		}
		strncpy(sender,p,len+4);
	}
	p	+= 4+len;
	rt_kprintf("\nSMS>sender=%s",sender);

	/*PID��־*/
	pid = tbl[*p++ - '0'] << 4;
	pid |= tbl[*p++ - '0'];
	/*DCS����*/
	dcs = tbl[*p++ - '0'] << 4;
	dcs |= tbl[*p++ - '0'];
	/*SCTS��������ʱ��� yymmddhhmmsszz ����ʱ��*/
	p += 14;
	/*��Ϣ����*/
	//msg_len = tbl[*p++ - '0'] << 4;
	//msg_len |= tbl[*p++ - '0'];
	p += 2;
	
	if((uint32_t)info + size <= (uint32_t)p)
	{
		return;
	}
	msg_len = ((uint32_t)info + size - (uint32_t)p)/2;
	/*�Ƿ�Ϊ����Ϣpdu_type ��bit6 UDHI=1*/
	if( pdu_type & 0x40 )           /*��������Ϣ��ͷ*/
	{
		if( ( dcs & 0x0c ) != GSM_7BIT )		///��ʱ������7bit������������Ϊ7bit����Ը�����Ҳ������
			{
			len		= tbl[*p++ - '0'] << 4;
			len		|= tbl[*p++ - '0'];
			p		+= ( len << 1 );    /*���ڻ���OCTģʽ*/
			msg_len -= ( len + 1 );     /*����ռһ���ֽ�*/
			}
	}
	
	rt_kprintf( "\nSMS>PDU Type=%02x PID=%02x DCS=%02x TP-UDL=%d", pdu_type, pid, dcs, msg_len );

/*ֱ����info�ϲ�������?*/
	pdst = tmpbuf;
	/*��OCT����תΪHEX,�����ֽ�ƴ��һ��*/
	for( i = 0; i < msg_len; i++ )
	{
		c		= tbl[*p++ - '0'] << 4;
		c		|= tbl[*p++ - '0'];
		*pdst++ = c;
	}
	*pdst = 0;

	if( ( dcs & 0x0c ) == GSM_7BIT )
	{
		decode_msg_len = gsmdecode7bit( tmpbuf, decode_msg, msg_len );
		/*���û�����ͷ����ȥ��*/
		len = tmpbuf[0] + 1;		//�����û����ݣ������� �û�����ͷ����(1byte) + �û�����;
		if(len * 8 % 7)
			{
			len = len * 8 / 7 + 1;
			}
		else
			{
			len =  len * 8 / 7;
			}
		if(( pdu_type & 0x40 )&&(len < decode_msg_len))
			{
			decode_msg_len	-= len;
			memcpy(tmpbuf,decode_msg+len,decode_msg_len);
			memcpy(decode_msg,tmpbuf,decode_msg_len);
			}
	}else if( ( dcs & 0x0c ) == GSM_UCS2 )
	{
		decode_msg_len = gsmdecodeucs2( tmpbuf, decode_msg, msg_len );
	}else
	{
		memcpy( decode_msg, tmpbuf, msg_len );
		decode_msg_len = msg_len;   /*�򵥿�������*/
	}

	if( decode_msg_len == 0 )       /*���ڲ��ǺϷ��������ǲ���ֱ�ӷ���0*/
	{
		return;
	}
	decode_msg[decode_msg_len] = 0;


//////
/*decode_msg�����������Ϣ*/
	rt_kprintf( "\nSMS(%dbytes)>%s", decode_msg_len, decode_msg );
	sms_param_proc(decode_msg,1);
#if 0
	p = decode_msg;
	if(( strncmp( p, "TW703", 5 ) != 0 )&&( strncmp( p, "TW705", 5 ) != 0 ))
	{
		return;
	}
	p += 5;
	memset( cmd, 0, 64 );
	memset( value, 0, 64 );
	count	= 0;
	psave	= cmd;
	i		= 0;
/*tmpbuf����Ӧ�����Ϣ TW703#*/
	while( *p != 0 )
	{
		switch( *p )
		{
			case '#':
				cmd_step = 1;
				memset( cmd, 0, 64 );
				memset( value, 0, 64 );
				psave	= cmd;      /*����cmd�����д�*/
				count	= 0;
				break;
			case '(':
				if(cmd_step == 1)
					cmd_step = 2;
				psave	= value;    /*��ȡֵ������*/
				count	= 0;
				break;
			case ')':               /*��ʾ����,������*/
				if(cmd_step == 2)
				{
					i += analy_param( cmd, value ,1);
					cmd_step = 0;
				}
				break;
			default:
				if( count < 64 )    /*����������*/
				{
					*psave++ = *p;
					count++;
				}
				break;
		}
		p++;
	}

	if(i)
	{
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf( tmpbuf, "%s#%-7s#%s", jt808_param.id_0x0083, jt808_param.id_0xF002, decode_msg + 6 );

		sms_send(tmpbuf,0);
	}
	
	if(sms_command)
	{
		if(sms_command & SMS_CMD_RESET)
		{
			reset(4);
		}
		if(sms_command & SMS_CMD_RELINK)
		{
			gsmstate( GSM_POWEROFF );
		}
		if(sms_command & SMS_CMD_FACTSET)
		{
			rt_kprintf("SMS> �ָ���������;");
		}
		if(sms_command & SMS_CMD_SAVEPARAM)
		{
			param_save( 1 );
		}
		sms_command = 0;
	}
#endif
}




#if 1
/*
   0891683108200205F0000D91683106029691F10008318011216584232C0054005700370030003300230050004C004100540045004E0055004D00286D25005400540036003500320029
   0891683108200205F0000D91683106029691F100003180112175222332D4EB0D361B0D9F4E67714845C15223A2732A8DA1D4F498EB7C46E7E17497BB4C4F8DA04F293586BAC160B814

   ������֤

   http://www.diafaan.com/sms-tutorials/gsm-modem-tutorial/online-sms-submit-pdu-decoder/
 */
void sms_test( uint8_t index )
{
	char *s[] =
	{ "0891683108200205F0000D91683128504568F30008318011216584232C0054005700370030003300230050004C004100540045004E0055004D00286D25005400540036003500320029",
	  "0891683108200205F0000D91683128504568F300003180112175222332D4EB0D361B0D9F4E67714845C15223A2732A8DA1D4F498EB7C46E7E17497BB4C4F8DA04F293586BAC160B814",
	  "0891683108200245F3240D90683128504568F30008312132513452232E6D259655004000280029004000230062006100630063006100620061003100310031003200320032003300330033"};
	jt808_sms_rx( s[index], strlen( s[index] ) );
}

FINSH_FUNCTION_EXPORT( sms_test, test sms );


void oiap_test(char *ip,uint16_t port)
{
	gsm_socket[2].index=2;
	strcpy(gsm_socket[2].ipstr,ip);
	gsm_socket[2].port=port;
	gsm_socket[2].state=CONNECT_IDLE;
}
FINSH_FUNCTION_EXPORT( oiap_test, conn onair iap );

#endif

/************************************** The End Of File **************************************/
