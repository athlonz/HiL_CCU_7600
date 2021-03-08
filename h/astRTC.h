/*
 * �ļ�����astRTC.h
 * 
 * ˵�������ļ�Ϊʵʱʱ�ӷ��ʵ������ļ�ͷ�ļ�
 * 
 * �޸ļ�¼��
 * 	--	2018.04.24	��syjiang����
 */
#ifndef _AST_RTC_H_
#define _AST_RTC_H_


/*  */
#define AST_RTC_CENTURY	( 0x20 )

/*ʱ�����ݽṹ*/
typedef struct{
	
	UINT8 Seconds;		/*��*/
	UINT8 Minites;		/*��*/
	UINT8 Hours;		/*ʱ*/
	UINT8 Dates;		/*����*/
	UINT8 Mouths;		/*�·�*/
	UINT8 Years;		/*���*/
	UINT8 Centurys;		/*����*/
	UINT8 DaysOfWeek;	/*����*/
	
}AstRtcStructType;

/* �������� */
extern INT32 astRtcRead( AstRtcStructType *rtc_struct );
extern INT32 astRtcWrite( AstRtcStructType rtc_struct );
extern INT32 astRtcShow( void );

#endif
