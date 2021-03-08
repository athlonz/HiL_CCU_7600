#include "ADRC.h"
#include <math.h>
#include "variable_main.h"
void TD(struct ADRCREG *ADRC)
{
/**********   TD  ************/
	ADRC->x1 = ADRC->x1 + ADRC->h*ADRC->x2;                                                //x1(k+1)=x1(k)+h*x2(k);
    ADRC->x2=ADRC->x2+ADRC->h*fst((ADRC->x1-ADRC->v),ADRC->x2,ADRC->r,ADRC->h0);           //x2(k+1)=x2(k)+h*fst(x1(k)-v(k),x2(k),r,h0);
}

void ADRC(struct ADRCREG *ADRC)
{ 
	/******** TD *************/
//   ADRC->x1 = ADRC->x1 + ADRC->h*ADRC->x2;                                                //x1(k+1)=x1(k)+h*x2(k);
//   ADRC->x2=ADRC->x2+ADRC->h*fst((ADRC->x1-ADRC->v),ADRC->x2,ADRC->r,20*ADRC->h);           //x2(k+1)=x2(k)+h*fst(x1(k)-v(k),x2(k),r,h0);
   /******** Discrete LESO  *************/
    ADRC->A=(4*ADRC->beta1*ADRC->b+2*(ADRC->beta1*ADRC->beta_01*ADRC->b+ADRC->beta_02)*ADRC->h+ADRC->beta1*ADRC->b*ADRC->beta_02*ADRC->h*ADRC->h);
    ADRC->B=(2*ADRC->beta1*ADRC->b*ADRC->beta_02*ADRC->h*ADRC->h-8*ADRC->beta1*ADRC->b);
    ADRC->C=(4*ADRC->beta1*ADRC->b-2*(ADRC->beta1*ADRC->beta_01*ADRC->b+ADRC->beta_02)*ADRC->h+ADRC->beta1*ADRC->b*ADRC->beta_02*ADRC->h*ADRC->h);
    ADRC->D=4*ADRC->b+2*ADRC->b*ADRC->beta_01*ADRC->h;
    ADRC->E=-8*ADRC->b;
    ADRC->F=4*ADRC->b-2*ADRC->b*ADRC->beta_01*ADRC->h;
    
//    ADRC->e=ADRC->x1-ADRC->y;
    ADRC->e=ADRC->v-ADRC->y;
    ADRC->u=(ADRC->A*ADRC->e+ADRC->B*ADRC->ek1+ADRC->C*ADRC->ek2-ADRC->E*ADRC->uk1-ADRC->F*ADRC->uk2)/ADRC->D;
    
    ADRC->ek2=ADRC->ek1;
    ADRC->ek1=ADRC->e;
    ADRC->uk2=ADRC->uk1;
    ADRC->uk1=ADRC->u;
  
}
void ADRC_restart(struct ADRCREG *ADRC)
{
    ADRC->v=0;
    ADRC->y=0;
    ADRC->z1=0;
    ADRC->z2=0;
    ADRC->e=0;
    ADRC->h=0;
    ADRC->beta_01=0;
    ADRC->beta_02=0;
    ADRC->b=0;
    ADRC->e1=0;
    ADRC->u0=0;
    ADRC->u=0;
    ADRC->beta1=0;
    ADRC->f1=0;
}
float sign(float x)
{
  if(x>0)
      return (1);
  if(x<0)
      return (-1);
}
float sat(float x,float d)
{
	if (fabs(x)>d)
		return (d*sign(x));
	if (fabs(x)<=d)
		return (x);
}


float fst(float x1,float x2,float r,float h)
{
	float td_y=0;
	float a0=0;
	float a=0;
	float fhan=0;
	float d=0;
	float d0=0;
	
	d=r*h;
	d0=h*d;
	td_y=x1+h*x2;
	a0=sqrt(d*d+8*r*fabs(td_y));
	
	if(fabs(td_y)>d0)
		a=x2+0.5*(a0-d)*sign(td_y);
	else
		a=x2+td_y/h;
	if (fabs(a)>d)
		fhan=-r*sign(a);
	else
		fhan=-r*a/d;
	return (fhan);
    
}

float fal(float e,float a,float delta)
{
    float y=0;
    if (fabs(e)<=delta)
            y=e/(pow(delta,(1-a)));
    else
        y=pow(fabs(e),a)*sign(e);
    return (y);
}

