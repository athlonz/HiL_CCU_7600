/*
 * 文件名：astRTC.h
 * 
 * 说明：本文件为实时时钟访问的驱动文件头文件
 * 
 * 修改记录：
 * 	--	2018.04.24	由syjiang创建
 */
#ifndef _AST_RTC_H_
#define _AST_RTC_H_


/*  */
#define AST_RTC_CENTURY	( 0x20 )

/*时钟数据结构*/
typedef struct{
	
	UINT8 Seconds;		/*秒*/
	UINT8 Minites;		/*分*/
	UINT8 Hours;		/*时*/
	UINT8 Dates;		/*日期*/
	UINT8 Mouths;		/*月份*/
	UINT8 Years;		/*年份*/
	UINT8 Centurys;		/*世纪*/
	UINT8 DaysOfWeek;	/*星期*/
	
}AstRtcStructType;

/* 函数声明 */
extern INT32 astRtcRead( AstRtcStructType *rtc_struct );
extern INT32 astRtcWrite( AstRtcStructType rtc_struct );
extern INT32 astRtcShow( void );

#endif
