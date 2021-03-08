#ifndef _ADRC_H_
#define _ADRC_H_
#include <math.h>

struct ADRCREG   /*ADRC������*/
{
double v;//ϵͳ����
//**********   TD Parameters ************
double x1;//����΢����״̬��
double x2;//����΢����״̬��΢����
double r;//ʱ��߶�
double h;//ADRCϵͳ����ʱ��
double h0;//�˲�����
//**********   ESO Parameters ************
/******��ϵͳ���y������u�����ٹ���ϵͳ״̬���Ŷ�*****/
double z1;
double z2;
double z3;//���ݿ��ƶ����������������ȡ���Ŷ���Ϣ
double f1;//��֪������Ϣ
double e;//ϵͳ״̬���
double y;//ϵͳ�����
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
double beta1;	//��������
double beta2;	//΢������
double e0;//״̬��������
double e1;//״̬ƫ��
double e2;//״̬��΢����
double u0;//���������ϵͳ���
double u;//���Ŷ�����������
double u_old;
double b0;//�Ŷ�����
         
};

void ADRC(struct ADRCREG *ADRC);
void ADRC_restart(struct ADRCREG *ADRC);

void TD(struct ADRCREG *ADRC);
float fst(float x1,float x2,float r,float h);
float fal(float e,float a,float delta);
float sat(float x,float d);
float sign(float x);

#endif
