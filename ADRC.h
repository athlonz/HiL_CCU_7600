#ifndef _ADRC_H_
#define _ADRC_H_
#include <math.h>

struct ADRCREG   /*ADRC控制器*/
{
double v;//系统输入
//**********   TD Parameters ************
double x1;//跟踪微分期状态量
double x2;//跟踪微分期状态量微分项
double r;//时间尺度
double h;//ADRC系统积分时间
double h0;//滤波因子
//**********   ESO Parameters ************
/******已系统输出y和输入u来跟踪估计系统状态和扰动*****/
double z1;
double z2;
double z3;//根据控制对象输入与输出，提取的扰动信息
double f1;//已知部分信息
double e;//系统状态误差
double y;//系统输出量
double y1;
double y2;
double y3;
double beta_01;
double beta_02;
double beta_03;
double b;
double A,B,C,D,E,F;
double ek1,ek2,uk1,uk2;
//**********   NLSEF Parameters ************
double alpha1;
double alpha2;
double beta1;	//比例增益
double beta2;	//微分增益
double e0;//状态误差积分项
double e1;//状态偏差
double e2;//状态量微分项
double u0;//非线性组合系统输出
double u;//带扰动补偿后的输出
double u_old;
double b0;//扰动补偿
         
};

void ADRC(struct ADRCREG *ADRC);
void ADRC_restart(struct ADRCREG *ADRC);

void TD(struct ADRCREG *ADRC);
float fst(float x1,float x2,float r,float h);
float fal(float e,float a,float delta);
float sat(float x,float d);
float sign(float x);

#endif
