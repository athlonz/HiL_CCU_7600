/*******************************************************************/
/*
//   Module  : variable_main.h
//   Created : 8/10/2015 3:25:30
//
//
//   Language  : C++*/
/*******************************************************************/
#ifndef _ANYBUSPFB_
#define _ANYBUSPFB_

#ifdef __cplusplus
extern "C" {
#endif
////////////////ISG状态字上传/////////////////////////////////////////////
struct ISG_stsByte_BITS
{
	UINT16 remoteOk:1;//1R-bit10	0x0400	远方控制	  
	UINT16 LSWP_Ok:1;//1R-bit7	0x0080	LSWP装置正常
	UINT16 powerSwichOk:1;//1R-bit6	0x0040	供电开关正常  	
	UINT16 GndSwitchOn:1; //1R-bit4	0x0010	接地开关接通 
	
	UINT16 CarWorkPosition:1;//1R-bit2	0x0004	小车工作位置
	UINT16 PreChargeSwitchOn:1;//1R-bit1	0x0002	接触器接通
	UINT16 SwitchOn:1;//1R-bit1	0x0002	接触器接通 
	UINT16 ProfibusOk:1;;
};
 

union  ISG_stsByte
{
	UINT8 BYTE;
	struct ISG_stsByte_BITS bit;
};
struct ISG_PFBrx{
	union  ISG_stsByte stsWord;
	UINT8  rsd1; 
	UINT8  rsd2 ;
};
///////////////ISG控制字下发//////////////////////// 

struct ISG_ctrlWord_BITS
{
 	UINT16  rsd:14;//1R-bit15	0x8000	备用	 
	
	UINT16  PreChargeSwitchOn:1;//1R-bit3	0x0008	 备用
	UINT16  SwitchOn:1;//1R-bit1	0x0002	 备用
 
};
 

union  ISG_ctrlWord
{
	UINT16 word;
	struct ISG_ctrlWord_BITS bit;
};
 
struct ISG_PFBtx{
	union  ISG_ctrlWord ctrlWord;	
};

////////////////OSG状态字上传/////////////////////////////////////////////
struct OSG_stsByte_BITS
{
	UINT16 remoteOk:1;//1R-bit10	0x0400	远方控制	  
	UINT16 LSWP_Ok:1;//1R-bit7	0x0080	LSWP装置正常
	UINT16 powerSwichOk:1;//1R-bit6	0x0040	供电开关正常  	
	UINT16 GndSwitchOn:1; //1R-bit4	0x0010	接地开关接通 
	
	UINT16 CarWorkPosition:1;//1R-bit2	0x0004	小车工作位置
	UINT16 DTTBSwitchOn:1;//1R-bit1	0x0002	接触器接通,==0,DT ,==1,TB
	UINT16 SwitchOn:1;//1R-bit1	0x0002	接触器接通 
	UINT16 ProfibusOk:1;;
};
 

union  OSG_stsByte
{
	UINT8 BYTE;
	struct OSG_stsByte_BITS bit;
};
struct OSG_PFBrx{
	union  OSG_stsByte stsWord;
	UINT8  rsd1; 
	UINT8  rsd2 ;
};
///////////////OSG控制字下发//////////////////////// 

struct OSG_ctrlWord_BITS
{
 	UINT16  rsd:14;//1R-bit15	0x8000	备用	 
	
	UINT16  DTTBSwitchOn:1;//1R-bit3	0x0008	 备用
	UINT16  SwitchOn:1;//1R-bit1	0x0002	 备用
 
};
union  OSG_ctrlWord
{
	UINT16 word;
	struct OSG_ctrlWord_BITS bit;
};
 
struct OSG_PFBtx{
	union  OSG_ctrlWord ctrlWord;	
};
/////////////////////////////////////////////////////
#pragma pack(1)
struct PFBrxStruct{
	struct ISG_PFBrx  ISG; 
	struct OSG_PFBrx  OSG;	 
};
#pragma pack()
#pragma pack(1)
struct PFBtxStruct{
	struct ISG_PFBtx ISG;//1号站2号柜GE9030 PLC 控制器,offset:0 
	struct OSG_PFBtx OSG;//1号站3号柜GE9030 PLC 控制器,20 
};
#pragma pack()
////////////////////////////////////////////////////
extern struct PFBrxStruct CCU11_PFB_RX;
extern  struct PFBtxStruct CCU11_PFB_TX;
#define SWAP(x) ( (x<<8)|(x>>8) )
/****************************************************************************************************/
extern int pfb_AB_init( void );
 
extern void stopAnybusTask(void);
/****************************************************************************************************/
#ifdef __cplusplus
}
#endif

#endif
