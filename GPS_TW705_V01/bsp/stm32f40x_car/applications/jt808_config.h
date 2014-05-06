#ifndef _H_JT808_CONFIG
#define _H_JT808_CONFIG


/***************************************************************************************************************/
//���ô洢��ص�ַ����Ӧ���豸��������Ҫ�洢������
/***************************************************************************************************************/
/*�豸��Ҫ�����洢λ��*/
#define ADDR_DF_PARAM_MAIN 			0x000000				///�豸��Ҫ�����洢λ��
#define ADDR_DF_PARAM_MAIN_SECT		1						///����������

/*���������洢λ��*/
#define ADDR_DF_UPDATA_START		0x001000  				///�������ݴ洢��ʼλ��
#define ADDR_DF_UPDATA_END			0x03A000 				///�������ݴ洢����λ��
#define ADDR_DF_UPDATA_PARA			ADDR_DF_UPDATA_END 		///������ز����洢λ��,�ò�����С���ܳ���0x1000
#define ADDR_DF_UPDATA_SECT			58						///����������

/*�豸��Ҫ�����洢����λ��*/
#define ADDR_DF_PARAM_MAIN_BK 		0x040000				///�豸��Ҫ�����洢����λ��
#define ADDR_DF_PARAM_MAIN_BK_SECT	1						///����������

/*�豸�������������ݴ洢λ��*/
#define ADDR_DF_PARAM_BASE 			0x041000				///�����洢�豸����Ҫ�������û����ݵ���Ϣ
#define ADDR_DF_PARAM_BASE_SECT		32						///����������

/*�ı���Ϣ,�¼�����,��������,��Ϣ�㲥,�绰��,�洢����*/
#define ADDR_DF_MISC_BASE 			0x08D000				///�����洢808�����������Ϣ:�ı���Ϣ,�¼�����,��������,��Ϣ�㲥,�绰��,�洢����
#define ADDR_DF_MISC_BASE_SECT		7						///����������

/*��·�洢����*/
#define ADDR_DF_LINE_START			0x094000     			///·�����ݴ洢��ʼλ��
#define ADDR_DF_LINE_END			0x0A2000    			///·�����ݴ洢����λ��
#define ADDR_DF_LINE_SECT			14						///����������

/*����Χ���洢����*/
#define ADDR_DF_AREA_START			0x0A2000    			///����Χ�����ݴ洢��ʼλ��
#define ADDR_DF_AREA_END			0x0A4000    			///����Χ�����ݴ洢����λ��
#define ADDR_DF_AREA_SECT			2						///����������

/*��ý��洢λ��*/
#define ADDR_DF_CAM_START			0x0A4000    			///ͼƬ��¼�����ݴ洢��ʼλ��
#define ADDR_DF_CAM_END				0X1D0000    			///ͼƬ��¼�����ݴ洢����λ��
#define ADDR_DF_CAM_SECT			300						///����������
//#define ADDR_DF_VOC_START			0x1D0000    			///¼�����ݴ洢��ʼλ��
//#define ADDR_DF_VOC_END			0X298000    			///¼�����ݴ洢����λ��
//#define ADDR_DF_VOC_SECT		 	0x400       			///¼�����ݴ洢��С

/*��ʻ��¼��VDR���ݴ洢����*/
#define ADDR_DF_VDR_BASE 			0x1D0000				///��ʻ��¼��VDR���ݴ洢����ʼλ��
#define ADDR_DF_VDR_END				0x2AC000				///��ʻ��¼��VDR���ݴ洢�������λ��
#define ADDR_DF_VDR_SECT			220						///���������һЩ�ռ䣬ʵ����Ҫ182

/*gps�������ݴ洢����*/
#define ADDR_DF_POSTREPORT_START	0x2AC000				///gps�������ݴ洢����ʼλ��
#define ADDR_DF_POSTREPORT_END		0x310000				///gps�������ݴ洢�������λ��
#define ADDR_DF_POSTREPORT_SECT		100						///����������

/*gpsѹ�����ݴ洢����*/
//����1�� 517�ֽ�  ѹ���� 260
//���� 100������ ����25����ԭʼ����
#define ADDR_DF_GPS_PACK_START		0x310000				///gpsԭʼѹ�����ݴ洢����ʼλ��
#define ADDR_DF_GPS_PACK_SECT		100						///����������

#endif
