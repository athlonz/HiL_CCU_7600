/*  
* target: rectifier PWM variable initiation   
* Copyright 2018,4,16 IEE, Inc. 
* description:  the goal is to use rectifier control funtion 
* function: 
* debug: 
* modification history:20180426 by wxx
*/
#include "vxWorks.h"
#include "GlobalVar_Rec.h"
 
/**********************************************************************/




double usdFw=0,usqFw=0;
double Vdc1MinuVdc2Rec=0;                   /* (0.0025,30*Ts_Cpu/2, lim1 better keep the Dt, 20100125) */
double NeuVoltCtl_deltTRec = 0,NeuVoltCtl_deltTRec1 = 0;
int sign_iaRec = 0,sign_ibRec = 0,sign_icRec = 0; 
unsigned int PtypeRec=1,NtypeRec=0,TempNPtype=0;
unsigned int Ptypek1=1,Ntypek1=0;

int    Sector_NumRec=0,Region_NumRec=0;
double epwm1cmpaRec=0,epwm2cmpaRec=0,epwm3cmpaRec=0,epwm4cmpaRec=0,epwm5cmpaRec=0,epwm6cmpaRec=0;
double ta0Rec=0,tb0Rec=0,tc0Rec=0,taRec=0,tbRec=0,tcRec=0;

double pt2t1Rec=0,pt3t2Rec=0,pt3t1Rec=0;
double pt1Rec=0,pt2Rec=0,pt3Rec=0;
double pepwm1cmpa=0,pepwm3cmpa=0,pepwm5cmpa=0;
int    MP_Debug=1;

int Ct1t2t3Rec=0,BbitRec=0,SbitRec=0,GbitRec=0;/* for MP_Rec */

double KvRec = 0.6;
int IntFlagRec = 0;
int Reg_UpDownRec =0;
short ZRORec[6]={2,1,2,1,2,1};
short CAURec[6]={0,0,0,0,0,0};
short PRDRec[6]={2,1,2,1,2,1};
short CADRec[6]={0,0,0,0,0,0};
int MPhalfRec=(int)(0.5 * MP_Rec);/* 0; */
int MPquart=0.25*MP_Rec;/* MPDouble=3800;//0; */
int MPDT=0;

int MP_testRec=0,DT_test=0;

double Gama_SVMRec = 0,VFGamaRec = 0.5;
double Gama_SVM_K1=0;
double Gama_SVM_DeltTRec=0;

int NPChangeRec=1;

double t1_zero,t2_zero,t3_zero;
double e1,e2,e3;
double t1_k1,t2_k1,t3_k1;
double t2NS,t2NB;
double  t1Rec=0,t2Rec=0,t3Rec=0;
/* Direct npchange */
const double * NPtimeTableRec[6][6][3]={
	              {
	               &t2Rec,&t1Rec,&t3Rec,
                 &t1Rec,&t3Rec,&t2Rec,
                 &t3Rec,&t1Rec,&t2Rec,
                 &t2Rec,&t3Rec,&t1Rec,
                 &t3Rec,&t2Rec,&t1Rec,
                 &t3Rec,&t2Rec,&t1Rec
                  },
                  
                 {
                 	&t1Rec,&t3Rec,&t2Rec,
                  &t3Rec,&t2Rec,&t1Rec,
                  &t1Rec,&t2Rec,&t3Rec,
                  &t3Rec,&t1Rec,&t2Rec,
                  &t2Rec,&t1Rec,&t3Rec,
                  &t2Rec,&t1Rec,&t3Rec
                  },
                  
                  {
                  &t3Rec,&t2Rec,&t1Rec, 
                  &t2Rec,&t1Rec,&t3Rec,
                  &t2Rec,&t3Rec,&t1Rec,
                  &t1Rec,&t2Rec,&t3Rec,
                  &t1Rec,&t3Rec,&t2Rec, 
                  &t1Rec,&t3Rec,&t2Rec 
                  },
                  
	              {
	               &t2Rec,&t1Rec,&t3Rec,
                 &t1Rec,&t3Rec,&t2Rec,
                 &t3Rec,&t1Rec,&t2Rec,
                 &t2Rec,&t3Rec,&t1Rec,
                 &t3Rec,&t2Rec,&t1Rec,
                 &t3Rec,&t2Rec,&t1Rec
                  },
                  
                 {
                 	&t1Rec,&t3Rec,&t2Rec,
                  &t3Rec,&t2Rec,&t1Rec,
                  &t1Rec,&t2Rec,&t3Rec,
                  &t3Rec,&t1Rec,&t2Rec,
                  &t2Rec,&t1Rec,&t3Rec,
                  &t2Rec,&t1Rec,&t3Rec
                  },
                  
                  {
                  &t3Rec,&t2Rec,&t1Rec, 
                  &t2Rec,&t1Rec,&t3Rec,
                  &t2Rec,&t3Rec,&t1Rec,
                  &t1Rec,&t2Rec,&t3Rec,
                  &t1Rec,&t3Rec,&t2Rec, 
                  &t1Rec,&t3Rec,&t2Rec 
                  },
                  }; /* 20100126               */


/* All N-type */
const double * DTable[6][6][3]={/*  t1Rec t2Rec t3Rec */
	              {/* Sector=0 */
                 &e3,&e1,&e2,/* Region=0 */
                 &e2,&e3,&e1,/* Region=1 */
                 &e1,&e3,&e2,/* Region=2 */
                 &e2,&e1,&e3,/* Region=3 */
                 &e1,&e2,&e3,/* Region=4 */
                 &e1,&e2,&e3 /* Region=5 */
                  },
                  
                 {/* Sector=1 */
              	  &e1,&e3,&e2,/* Region=0 */
                  &e3,&e2,&e1,/* Region=1 */
                  &e1,&e2,&e3,/* Region=2 */
                  &e2,&e3,&e1,/* Region=3 */
                  &e2,&e1,&e3,/* Region=4 */
                  &e2,&e1,&e3 /* Region=5 */
                  },
                  
                  {/* Sector=2 */
                  &e1,&e2,&e3, 
                  &e3,&e1,&e2,
                  &e2,&e1,&e3,
                  &e3,&e2,&e1,
                  &e2,&e3,&e1, 
                  &e2,&e3,&e1 
                  },
                  
                  {/* Sector=3 */
                   &e2,&e1,&e3,
                   &e1,&e3,&e2,
                   &e2,&e3,&e1,
                   &e3,&e1,&e2,
                   &e3,&e2,&e1,
                   &e3,&e2,&e1
                   },
                   
                 {/* Sector=4 */
              	  &e2,&e3,&e1,
                  &e1,&e2,&e3,
                  &e3,&e2,&e1,
                  &e1,&e3,&e2,
                  &e3,&e1,&e2,
                  &e3,&e1,&e2
                  }, 
                  
                 {/* Sector=5 */
                 &e3,&e2,&e1,
                 &e2,&e1,&e3,
                 &e3,&e1,&e2,
                 &e1,&e2,&e3,
                 &e1,&e3,&e2,
                 &e1,&e3,&e2
                 }
                 };            
/*  20091125  */

 
/* Region_Num：0--5 */
/* 1--低   2--高  3--反转  0--保持 */

/* direct NPchange */
const short ActTb_EPwm1Rec[6][6][4] = {
/* Reg 0     1         2        3        4       5 */
/*{2,3,1,3, 2,3,1,3, 2,3,1,3,	2,3,1,3, 2,3,1,3, 2,3,1,3,},//Sec  0  P  
{1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3,},//     1  N
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},//     2  P
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},//     3  N
{2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0,},//     4  P
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},//     5  N*/

{1,3,2,3, 1,3,2,3, 1,3,2,3,	1,3,2,3, 1,3,2,3, 1,3,2,3,},/* Sec  0  P   */
{2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3,},/*      1  N */
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},/*      2  P */
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},/*      3  N */
{1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0,},/*      4  P */
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},/*      5  */
};
const short ActTb_EPwm2Rec[6][6][4] = {
/*{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},
{1,3,2,3, 2,0,2,0, 1,3,2,3,	2,0,2,0, 1,3,2,3, 2,0,2,0,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3,},
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},	      
*/
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},
{2,3,1,3, 1,0,1,0, 2,3,1,3,	1,0,1,0, 2,3,1,3, 1,0,1,0,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3,},
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},
};    
const short ActTb_EPwm3Rec[6][6][4] = {
/*{2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0,},	
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3,},
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},*/
{1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0,},	
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3,},
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},
};
const short ActTb_EPwm4Rec[6][6][4] = {
/*{2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3,},	
{2,0,2,0, 2,0,2,0, 2,0,2,0,	2,0,2,0, 2,0,2,0, 2,0,2,0,},
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},
{1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},*/
{1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3,},	
{1,0,1,0, 1,0,1,0, 1,0,1,0,	1,0,1,0, 1,0,1,0, 1,0,1,0,},
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},
{2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
};
const short ActTb_EPwm5Rec[6][6][4] = {
/*{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},	
{1,0,1,0, 1,0,1,0, 1,0,1,0,	1,0,1,0, 1,0,1,0, 1,0,1,0,},
{2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3,},*/
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},	
{2,0,2,0, 2,0,2,0, 2,0,2,0,	2,0,2,0, 2,0,2,0, 2,0,2,0,},
{1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0,},
{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3,},
}; 

const short ActTb_EPwm6Rec[6][6][4] = {
/*{2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3, 2,3,1,3,},	
{1,3,2,3, 1,3,2,3, 1,3,2,3,	1,3,2,3, 1,3,2,3, 1,3,2,3,},
{2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3, 2,0,2,0, 2,3,1,3,},
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},
{2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0, 2,0,2,0,},
{1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0, 1,3,2,3, 2,0,2,0,},*/
{1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3, 1,3,2,3,},	
{2,3,1,3, 2,3,1,3, 2,3,1,3,	2,3,1,3, 2,3,1,3, 2,3,1,3,},
{1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3, 1,0,1,0, 1,3,2,3,},
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},
{1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,},
{2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0, 2,3,1,3, 1,0,1,0,},
}; 
 			             
/*  All of this tables are correct, direct NPchanged, examined by TLC at 20100429 */

/* Reg */
/* 0    1     2   3    4    5  */
const short Tbt1t2t3Rec[6][6] = {   
{613, 461, 163, 416, 136, 146,},/* Sector=0 */
{263, 631, 236, 361, 326, 316,},/* Sector=1 */
{235, 623, 325, 632, 352, 362,},/* Sector=2 */
{425, 253, 452, 523, 542, 532,},/* Sector=3 */
{451, 245, 541, 254, 514, 524,},/* Sector=4 */
{641, 415, 614, 145, 164, 154,},/* Sector=5	       */
};

const short TbPt1t2t3Rec[6][6] = {   
{316, 164, 361, 614, 631, 641,},
{362, 136, 632, 163, 623, 613,},
{532, 326, 523, 236, 253, 263,},
{524, 352, 254, 325, 245, 235,},
{154, 542, 145, 452, 415, 425,},
{146, 514, 416, 541, 461, 451,},	      
};
 


