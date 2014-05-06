/*SLE相关操作函数*/

#include <rtthread.h>
#include <rthw.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "board.h"
#include <serial.h>
#include <string.h>
#include "SLE4442.h"
#include "hmi.h"
#include "menu_include.h"

unsigned int	R_Flag		= 0;
unsigned char	sle_pass[3] = { 0xFF, 0xFF, 0xFF }; //密码//{0x23,0x10,0x91};
unsigned int	delayint;

//#define _Nop() for(i=0;i<13;i++){__NOP;}//2us
//#define DELAY5us() for(i=0;i<35;i++){__NOP;}//5.2us


#define IDENTIFY1   0xA2     
#define IDENTIFY2   0x13  
#define IDENTIFY3   0x10
#define IDENTIFY4   0x91     

/*
仿模拟IIC格式
#define SCL_H		( GPIOB->BSRRL = GPIO_Pin_5 )
#define SCL_L		( GPIOB->BSRRH = GPIO_Pin_5 )
#define SCL_read	( GPIOB->IDR & GPIO_Pin_5 )
*/

#define _CardSetRST_HIGH    GPIOD->BSRRL=GPIO_Pin_7; //GPIO_SetBits(GPIOD,GPIO_Pin_7);
#define _CardSetRST_LOW     GPIOD->BSRRH=GPIO_Pin_7; //GPIO_ResetBits(GPIOD,GPIO_Pin_7);

#define _CardSetCLK_HIGH    GPIOB->BSRRL=GPIO_Pin_12; //GPIO_SetBits(GPIOB,GPIO_Pin_12);
#define _CardSetCLK_LOW     GPIOB->BSRRH=GPIO_Pin_12; //GPIO_ResetBits(GPIOB,GPIO_Pin_12);


#define _CardCMDVCC_HIGH     GPIOC->BSRRL=GPIO_Pin_6; //GPIO_SetBits(GPIOC,GPIO_Pin_6);
#define _CardCMDVCC_LOW      GPIOC->BSRRH=GPIO_Pin_6; //GPIO_ResetBits(GPIOC,GPIO_Pin_6);

#define _CardSetPower_HIGH  GPIOB->BSRRL=GPIO_Pin_0; //GPIO_SetBits(GPIOB,GPIO_Pin_0);
#define _CardSetPower_LOW   GPIOB->BSRRH=GPIO_Pin_0; //GPIO_ResetBits(GPIOB,GPIO_Pin_0);


#define MAM 0 /*定义主存储器代号*/
#define SCM 1 /*定义加密存储器代号*/
#define PRM 2 /*定义保护存储器代号*/


void _Nop( void )
{
	u8 i = 0;
	for( i = 0; i < 13; i++ )
	{
		; //2us
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
void DELAY5us( void )
{
	u8 i = 25;
	for( i = 0; i < 25; i++ )
	{
		;               //5.2us
	}
}

#if 0

static void delay_us( const uint32_t usec )
{
	__IO uint32_t	count	= 0;
	const uint32_t	utime	= ( 168 * usec / 7 );
	do
	{
		if( ++count > utime )
		{
			return;
		}
	}
	while( 1 );
}

#endif

/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
void _CardPutIO_HIGH( void )
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz; //GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );

	GPIO_SetBits( GPIOD, GPIO_Pin_6 );
#else
	GPIOD->BSRRL=GPIO_Pin_6;
#endif
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
void _CardPutIO_LOW( void )
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );

	GPIO_ResetBits( GPIOD, GPIO_Pin_6 );
#else
	GPIOD->BSRRH=GPIO_Pin_6;

#endif
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
unsigned char _CardReadIO( void )
{
	unsigned char i;
	//_CardPutIO_HIGH;
	i = GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_6 ); //((P6IN&BIT5)>>5);
	return i;
}
/*******************************************************************
   起动总线函数
   函数原型: void Start_COM();
   功能: 启动发送命令起始条件.
 ********************************************************************/
void Start_COM( void )
{
	_CardPutIO_HIGH( ); /*发送起始条件的数据信号*/
	_CardSetCLK_LOW;
	_Nop( );
	_Nop( );
	_Nop( );
	_CardSetCLK_HIGH;
	DELAY5us( );        /*起始条件建立时间大于4.7us,延时*/
	_CardPutIO_LOW( );  /*发送起始信号*/
	DELAY5us( );        /*起始条件锁定时间大于4 s*/
	_CardSetCLK_LOW;    /*钳住总线准备发送或接收数据 */
	_Nop( );
	_Nop( );
}

/*******************************************************************
   结束总线函数
   函数原型: void Stop_COM();
   功能: 命令发送结束信号
 ********************************************************************/
void Stop_COM( void )
{
	_CardSetCLK_LOW;
	_CardPutIO_LOW( );  /*发送结束条件的数据信号*/
	_Nop( );            /*发送结束条件的时钟信号*/
	_Nop( );
	_Nop( );
	_CardSetCLK_HIGH;   /*结束条件建立时间大于4 s*/
	DELAY5us( );
	_CardPutIO_HIGH( ); /*发送总线结束信号*/
	_Nop( );
	_Nop( );
}

/*******************************************************************
   字节数据传送函数
   函数原型: void SendByte(unsigned char c);
   功能: 将数据c 发送出去,可以是命令,也可以是数据
 ********************************************************************/
void SendByte( unsigned char c )
{
	unsigned char BitCnt;
	for( BitCnt = 0; BitCnt < 8; BitCnt++ ) /*要传送的数据长度为8 位*/
	{
		if( ( c >> BitCnt ) & 0x01 )
		{
			_CardPutIO_HIGH( );             /*判断发送位*/
		}else
		{
			_CardPutIO_LOW( );
		}
		_Nop( ); _Nop( );
		_CardSetCLK_HIGH;                   /*置时钟线为高通知被控器开始接收数据位*/
		DELAY5us( );                        /*保证时钟高电平周期大于4 s*/
		_CardSetCLK_LOW;
	}
}

/*******************************************************************
   字节数据接收函数
   函数原型: unsigned char RcvByte();
   功能: 用来接收从卡传来的数据
 ********************************************************************/
unsigned char RcvByte( void )
{
	unsigned char		retc = 0;
	unsigned char		BitCnt;
#if 0
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_Init( GPIOD, &GPIO_InitStructure );
#endif
	for( BitCnt = 0; BitCnt < 8; BitCnt++ )
	{
		_CardSetCLK_LOW;    /*置时钟线为低准备接收数据位*/
		DELAY5us( );
		DELAY5us( );        /*时钟低电平周期大于4.7 s*/
		_CardSetCLK_HIGH;   /*置时钟线为高使数据线上数据有效*/
		DELAY5us( );
		retc = retc >> 1;
		if( _CardReadIO( ) )
		{
			retc |= 0x80;   /*读数据位,接收的数据位放入retc 中 */
		}
		DELAY5us( );
	}
	_CardSetCLK_LOW;
	_Nop( ); _Nop( );
	return  retc;
}

/*******************************************************************
   复位和复位响应函数
   函数原型 void AnRst();
   功能 复位IC 卡并接收响应字节
*******************************************************************/
unsigned char AnRst( void )
{
	unsigned char buffer[4], ii = 0;

	_CardSetRST_LOW;
	_CardSetCLK_LOW;
	DELAY5us( );
	DELAY5us( );
	_CardSetRST_HIGH;
	DELAY5us( );
	_CardSetCLK_HIGH;
	DELAY5us( );
	DELAY5us( );
	DELAY5us( );
	_CardSetCLK_LOW;
	DELAY5us( );
	_CardSetRST_LOW;
	_Nop( );

	for( ii = 0; ii < 4; ii++ )
	{
		buffer[ii] = RcvByte( );
	}
	if( ( buffer[0] == IDENTIFY1 ) && ( buffer[1] == IDENTIFY2 ) && ( buffer[2] == IDENTIFY3 ) && ( buffer[3] == IDENTIFY4 ) )
	{
		return 0;   //复位值正确,返回复位成功
	}else
	{
		return 1;   //复位值错误,返回复位失败
	}
}

/*******************************************************************
   发送4442 处理脉冲函数
   函数原型 void WrmOption();
   功能 发送处理模式指令后要调用此程序发送脉冲
*******************************************************************/

void WrmOption( void )
{
	while( 1 )
	{
		_CardSetCLK_LOW;
		_Nop( ); _Nop( ); _Nop( ); _Nop( ); _Nop( ); _Nop( );

		_Nop( ); _Nop( );
		if( _CardReadIO( ) )
		{
			break; /*没有处理完则继续发送脉冲*/
		}
		_CardSetCLK_HIGH;
		_Nop( ); _Nop( ); _Nop( ); _Nop( ); _Nop( ); _Nop( );
	}
}



/*******************************************************************
   中止操作函数
   函数原型 void BreakN();
   功能 中止当前操作
*******************************************************************/
void BreakN( void )
{
	_CardSetCLK_LOW;
	DELAY5us( );
	_CardSetRST_HIGH; /*发出中止操作的时序*/
	DELAY5us( );
	_CardSetRST_LOW;
}

/*******************************************************************
   命令发送函数
   函数原型 void SendCOM(unsigned char com1,unsigned char com2,unsigned char com3);
   功能 负责起动命令发送3 字节命令字
   结束命令
*******************************************************************/
void SendCOM( unsigned char com1, unsigned char com2, unsigned char com3 )
{
	Start_COM( );
	SendByte( com1 ); /*连续发送3 字节指令*/
	SendByte( com2 );
	SendByte( com3 );
	Stop_COM( );
}

/*******************************************************************
   SLE4442 卡读数据函数
   函数原型: unsigned char IRcvdat_4442(unsigned char area,unsigned char addr,unsigned char num,unsigned char *buf);
   功能: 对SLE4442 卡进行读操作area 为存储器类型addr 为起始地址
   num 为读取数据字节数buf[]为数据缓冲区指针
   说明 操作成功返回1 参数area 错误返回0 使用前用判断卡插好没有
 ********************************************************************/
unsigned char IRcvdat_4442( unsigned char area, unsigned char addr, unsigned char num, unsigned char *buf )
{
	unsigned char i;

	switch( area )
	{
		case MAM:
			if( AnRst( ) == 1 )             /*复位SLE4442 卡接收复位响应*/
			{
				return 0;
			}
			SendCOM( 0x30, addr, 0x00 );    /*读主存储器*/
			for( i = 0; i < num; i++ )
			{
				*buf = RcvByte( );
				buf++;
			}
			BreakN( );
			break;
		case SCM:
			if( AnRst( ) == 1 )
			{
				return 0;
			}
			SendCOM( 0x31, 0x00, 0x00 );
			for( i = 0; i < num; i++ )
			{
				*buf = RcvByte( );
				buf++;
			}
			BreakN( );
			break;
		case PRM:
			AnRst( );
			SendCOM( 0x34, 0x00, 0x00 );
			for( i = 0; i < num; i++ )
			{
				*buf = RcvByte( );
				buf++;
			}
			BreakN( );
			break;
		default:
			return 0;
	}
	return  1;
}

/*******************************************************************
   SLE4442 卡写数据函数
   函数原型: unsigned char ISenddat_4442(unsigned char area,unsigned char addr,unsigned char num,unsigned char *buf);
   功能: 对SLE4442 卡进行写操作area 为存储器类型addr 为起始地址
   num 为读取数据字节数buf[]为数据缓冲区指针
   说明 操作成功返回1 参数area 错误返回0 使用前用判断卡插好没有
 ********************************************************************/
unsigned char ISenddat_4442( unsigned char area, unsigned char addr, unsigned char num, unsigned char *buf )
{
	unsigned char i;

	switch( area )
	{
		case MAM:
			if( AnRst( ) == 1 )
			{
				return 0x00;
			}
			for( i = 0; i < num; i++ )
			{
				SendCOM( 0x38, addr + i, *buf );    /*写主存储器*/
				buf++;
				WrmOption( );                       /*发送操作脉冲*/
			}
			break;
		case SCM:
			AnRst( );
			for( i = 0; i < num; i++ )
			{
				SendCOM( 0x39, addr + i, *buf );
				buf++;
				WrmOption( );
			}
			break;
		case PRM:
			AnRst( );
			for( i = 0; i < num; i++ )
			{
				SendCOM( 0x3c, addr + i, *buf );
				buf++;
				WrmOption( );
			}
			break;
		default:
			return ( 0 );
	}
	return ( 1 );
}

/*******************************************************************
   SLE4442 卡校验密码函数
   函数原型: unsigned char IChkpsw_4442(void);
   功能: 进行SLE4442 卡进行密码核对核对后方能进行写操作
   说明 操作成功返回0x00 卡无效或卡损坏返回0x01,密码错误返
   回0x02 卡只剩1 次机会返回0x03.
 ********************************************************************/
unsigned char IChkpsw_4442( void )
{
	unsigned char ec;
	if( IRcvdat_4442( SCM, 0x00, 1, &ec ) == 0 )
	{
		return 0x01;
	}
	switch( ec & 0x7 )
	{
		case 1:
		case 2:
		case 4: return 0x3;
		case 3:
		case 5: ec	= 0x1; break;
		case 6: ec	= 0x2; break;
		case 7: ec	= 0x3; break;
		default: return 0x1;
	}
	if( AnRst( ) == 1 )
	{
		return 0x01;
	}
	SendCOM( 0x39, 0x00, ec ); //回写EC 字节
	WrmOption( );
	SendCOM( 0x33, 0x01, sle_pass[0] );
	WrmOption( );
	SendCOM( 0x33, 0x02, sle_pass[1] );
	WrmOption( );
	SendCOM( 0x33, 0x03, sle_pass[2] );
	WrmOption( );
	SendCOM( 0x39, 0x00, 0xff ); //修改EC值
	WrmOption( );
	ec = 0;
	if( IRcvdat_4442( SCM, 0x00, 1, &ec ) == 0 )
	{
		return 0x01;
	}

	if( ( ec & 0x07 ) != 0x07 )
	{
		return ( 0x02 );
	}
	return ( 0x00 );
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
void Init_4442( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE );

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );
	
#if 0
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );
	GPIO_SetBits( GPIOD, GPIO_Pin_6 );
#else
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );
	GPIOD->BSRRL= GPIO_Pin_6;
#endif

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOC, &GPIO_InitStructure );
	GPIO_SetBits( GPIOC, GPIO_Pin_6 );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7; // IC  卡插入指示
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_Init( GPIOC, &GPIO_InitStructure );
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
unsigned char Rx_4442( unsigned char addr, unsigned char num, unsigned char *buf )
{
	if( addr < 32 )
	{
		return 0x03;
	}
	if( ( GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_7 ) ) == 0 )
	{
		return 0x01;
	}
	if( IRcvdat_4442( MAM, addr, num, buf ) == 0 )
	{
		return 0x02;
	}
	return 0x00;
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
unsigned char Tx_4442( unsigned char addr, unsigned char num, unsigned char *buf )
{
	if( addr < 32 )
	{
		return 0x03;
	}
	if( ( GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_7 ) ) == 0 )
	{
		return 0x01;
	}
	if( IChkpsw_4442( ) == 0 )
	{
		if( ISenddat_4442( MAM, addr, num, buf ) == 0 )
		{
			return 0x02;
		}
	}else
	{
		return 0x02;
	}
	return 0x00;
}


unsigned char	IC_Check_Count	= 0;
unsigned int	DelayCheckIc	= 0;

#define b_CardEdge 0x0001

typedef struct
{
 uint8_t	administrator_card;			/*非0表示为管理员卡*/
 uint32_t 	card_change_mytime; 		/*卡插入或拔出的时间,时间格式为MYTIME*/
 uint32_t 	IC_Card_valitidy; 			/*卡片有效期，时间格式为MYTIME*/
 uint8_t 	IC_Card_Checked; 			/*卡是否已检测*/
 uint8_t	card_state;					/*IC卡读取结果,
 										0:成功，读取成功;
 										1:失败，卡密码认证失败;
 										2:失败，卡被锁定;
 										3:失败，卡被拔出;
 										4:失败，数据校验错误;*/
}TYPE_IC_CARD_PARA;

/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/

TYPE_IC_CARD_PARA ic_card_para={0,0,0,0,0};



void CheckICCard( void )
{
//	unsigned char	write_flag	= 0;
	u8				result=0;
	unsigned char	buf[20];

	unsigned char	buf1[12];
	unsigned char	buf2[20];
	unsigned char	buf3[4];
	unsigned char	buf4[22];
	unsigned char	buf5[40];
	unsigned char	buf6[42];
	
	if( GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_7 ) )
	{
		if(ic_card_para.IC_Card_Checked) return;
		IC_Check_Count++;
		if( IC_Check_Count >= 10 )
		{
			IC_Check_Count = 0;
			R_Flag			|= b_CardEdge;
			_CardCMDVCC_LOW;
			for( DelayCheckIc = 0; DelayCheckIc < 500; DelayCheckIc++ )
			{
				DELAY5us( );
			}
			_CardSetPower_HIGH;
			_CardSetRST_LOW;
			for( DelayCheckIc = 0; DelayCheckIc < 15; DelayCheckIc++ )
			{
				_CardSetCLK_LOW;
				DELAY5us( ); DELAY5us( ); DELAY5us( );
				_CardSetCLK_HIGH;
				DELAY5us( ); DELAY5us( ); DELAY5us( );
				_CardSetCLK_LOW;
			}
			R_Flag		&= ~b_CardEdge;
			Rx_4442( 241, 13, buf );  
			ic_card_para.card_change_mytime = mytime_now;
			ic_card_para.IC_Card_valitidy	= 0xffffffff;
			if( strncmp( (char*)buf, "administrator", 13 ) == 0 )
			{
				rt_kprintf( "\n管理员卡" );
				ic_card_para.administrator_card = 1;
			}
			else
			{
				result = Rx_4442( 70, 10, buf1 );																			//读驾驶员姓名
				rt_kprintf( "\n驾驶员姓名:%s,result=%d", buf1, result );
			
				result += Rx_4442( 52, 18, buf2 );																			//读驾驶证号码
				rt_kprintf( "\n驾驶证代码:%s,result=%d", buf2, result );
			
				result += Rx_4442( 49, 3, buf3 );																			//读驾驶员代码
				rt_kprintf( "\n驾驶员代码:%s,result=%d", buf3, result );
			
				result += Rx_4442( 80, 20, buf4 );																			//身份证号码
				rt_kprintf( "\n身份证号码:%s,result=%d", buf4, result );
			
				result += Rx_4442( 100, 40,buf5 );																			//从业资格证
				rt_kprintf( "\n从业资格证:%s,result=%d", buf5, result );
			
				result += Rx_4442( 140, 41, buf6 );																			//发证机构
				rt_kprintf( "\n发证机构:%10s,result=%d", buf6, result );
				rt_kprintf( "\nIC result=%d", result );
				if(result==0)
				{	
					ic_card_para.IC_Card_Checked=1;
					ic_card_para.card_state	= 0;
					strncpy(jt808_param.id_0xF008,buf1,10);
					strncpy(jt808_param.id_0xF009,buf1,18);
					param_save(1);
					beep(5,5,2);
				}
				else
				{
					ic_card_para.card_state	= 4;
					ic_card_para.IC_Card_Checked=1;
					beep(2,2,3);
				}
			}
			IC_CARD_jt808_0x0702(1,0);
		}
	}else
	{
	
		IC_Check_Count = 0;
		_CardSetRST_HIGH;
		_CardSetPower_LOW;
		_CardCMDVCC_HIGH;
		ic_card_para.IC_Card_Checked=0;
		ic_card_para.card_state	= 3;
		if(ic_card_para.IC_Card_Checked==1)
		{
			ic_card_para.card_change_mytime = mytime_now;
			IC_CARD_jt808_0x0702(1,0);
			beep(10,10,1);
		}	
	}
}





/*********************************************************************************
  *函数名称:rt_err_t IC_CARD_jt808_0x0702(uint8_t linkno, uint8_t is_ack )
  *功能描述:发送IC卡用户信息到服务器，分为主动发送和被动应答，主动发送是is_ack为0，否则为1
  *输	入:	linkno	:发送链路编号
  			is_ack	:为0表示主动发送数据，需要应答，为1表示应答上位机请求，不需要应答
  *输	出:	none
  *返 回 值:rt_err_t
  *作	者:白养民
  *创建日期:2014-01-24
  *---------------------------------------------------------------------------------
  *修 改 人:
  *修改日期:
  *修改描述:
*********************************************************************************/
rt_err_t IC_CARD_jt808_0x0702(uint8_t linkno, uint8_t is_ack )
{
	u8						ptempbuf[128];
	u16						datalen = 0;
	JT808_TX_NODEDATA		* pnodedata;

	//1byte，1表示卡片已经插入，2表示没有插入
	if(ic_card_para.IC_Card_Checked)
		ptempbuf[datalen++]	= 1;
	else
		ptempbuf[datalen++]	= 2;

	//时间BCD,6byte
	mytime_to_bcd(ptempbuf+datalen, ic_card_para.card_change_mytime);
	datalen+=6;
	//IC卡读取结果，1byte
	ptempbuf[datalen++]	= ic_card_para.card_state;
	//驾驶员姓名长度，1byte
	ptempbuf[datalen++]	= strlen(jt808_param.id_0xF008);
	//驾驶员姓名
	memcpy(ptempbuf+datalen, jt808_param.id_0xF008, strlen(jt808_param.id_0xF008));
	datalen	+= strlen(jt808_param.id_0xF008);
	//从业资格证书编码
	memcpy(ptempbuf+datalen, jt808_param.id_0xF00B, 20);
	datalen	+= 20;
	//发证机构名称长度，1byte
	ptempbuf[datalen++]	= strlen(jt808_param.id_0xF00C);
	//发证机构名称
	memcpy(ptempbuf+datalen, jt808_param.id_0xF008, strlen(jt808_param.id_0xF00C));
	datalen	+= strlen(jt808_param.id_0xF00C);
	//证件有效期,4BYTE,
	ptempbuf[datalen++]	= 0x20;
	ptempbuf[datalen++]	= HEX2BCD(YEAR(ic_card_para.IC_Card_valitidy));
	ptempbuf[datalen++]	= HEX2BCD(MONTH(ic_card_para.IC_Card_valitidy));
	ptempbuf[datalen++]	= HEX2BCD(DAY(ic_card_para.IC_Card_valitidy));
	
	if(is_ack)
		{
		jt808_add_tx(linkno, SINGLE_FIRST, 0x0702, -1, RT_NULL, RT_NULL, datalen, ptempbuf, RT_NULL );
		}
	else
		{
		jt808_add_tx(linkno, SINGLE_CMD, 0x0702, -1, RT_NULL, RT_NULL, datalen, ptempbuf, RT_NULL );
		}
	return RT_EOK;
}


/************************************** The End Of File **************************************/
