#ifndef _CLOSELOOP_
#define _CLOSELOOP_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************/
#include "vxWorks.h"
#include <math.h>
/*********************************************************************************************/
//�Ÿ�ģ��ͬ���������	   
#define Rs 0.2//0.0035
#define Ld 0.0042//0.0022//0.0011//0.0022//0.0002
#define Lq 0.0042//0.0022//0.0011//0.0022//0.0002
#define If 18
#define Msf 0.1346  

//�Զ��������л�����   
#define uptime 2.5             //iqcmd������ʱ��
#define staytime 16           //iqcmd���ֲο�ֵ��ʱ��
#define downtime 0.5 //1.5   //iqcmd�½�Ϊ0��ʱ��
#define closetime 0.8//2    //�ֶ�ģʽ�´���ģʽ�Ͳ���ģʽ���ضϵ�ʱ��
#define freqclosetime 1.8 //3.5  //��Ƶ���д�����ʱ��downtime+transienttime+closetime
#define transienttime 0.5    //iqcmd�½�Ϊ0������ģʽ���ضϵĹ���ʱ��
//#define iq_cmd_ref 25//15//15   ���ٶȻ�ʱ��

#define vref_Up_Rate  50.

float ua,ub,uc,Grid_u_alph,Grid_u_beta,Grid_u_Ampli,GridU,usd,usq,GridFOC_GamaTransf,GridFOC_GamaPhaseShift;


extern double speed_ref,speed_fb,speed_for;

#ifdef __cplusplus
}
#endif

#endif
