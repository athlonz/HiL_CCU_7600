#ifndef _CLOSELOOP_
#define _CLOSELOOP_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/
//磁浮模拟同步电机参数	   
#define Rs 0.2//0.0035
#define Ld 0.0042//0.0022//0.0011//0.0022//0.0002
#define Lq 0.0042//0.0022//0.0011//0.0022//0.0002
#define If 18
#define Msf 0.1346  

//自动串并联切换参数   
#define uptime 2.5             //iqcmd上升的时间
#define staytime 16           //iqcmd保持参考值的时间
#define downtime 0.5 //1.5   //iqcmd下降为0的时间
#define closetime 0.8//2    //手动模式下串联模式和并联模式都关断的时间
#define freqclosetime 1.8 //3.5  //按频率切串并联时的downtime+transienttime+closetime
#define transienttime 0.5    //iqcmd下降为0到两种模式均关断的过度时间
//#define iq_cmd_ref 25//15//15   无速度环时用

#define vref_Up_Rate  50.

float ua,ub,uc,Grid_u_alph,Grid_u_beta,Grid_u_Ampli,GridU,usd,usq,GridFOC_GamaTransf,GridFOC_GamaPhaseShift;


extern double speed_ref,speed_fb,speed_for;

#ifdef __cplusplus
}
#endif

#endif
