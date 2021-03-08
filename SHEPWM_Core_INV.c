#include <math.h>  
#include "GlobalVar_Inv.h"
#include "pwmControl.h"
#include "variable_main.h"
 
/***********************************************************/
/*Name		:MinPulseSetAse
*Description:最小脉宽设置，在上升计数时刻
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void MinPulseSetAse(struct PWMdataStruct *PWMdata)	/* in1--MinP */
{
	PWMdata->MinPulseFlag = 0;
	switch(g_i)
	{
		case 0:
			EPwm_g_i_PRDPre = PWMdata->EPwm1PRDPre;
			EPwm_g_i_CADPre = PWMdata->EPwm1CADPre;
			Epwm_g_i_timeconti = PWMdata->Epwm1_timeconti;	
			break;
		case 1:
			EPwm_g_i_PRDPre = PWMdata->EPwm2PRDPre;
			EPwm_g_i_CADPre = PWMdata->EPwm2CADPre;
			Epwm_g_i_timeconti = PWMdata->Epwm2_timeconti;	
			break;
		case 2:
			EPwm_g_i_PRDPre = PWMdata->EPwm3PRDPre;
			EPwm_g_i_CADPre = PWMdata->EPwm3CADPre;
			Epwm_g_i_timeconti = PWMdata->Epwm3_timeconti;		
			break;
		case 3:
			EPwm_g_i_PRDPre = PWMdata->EPwm4PRDPre;
			EPwm_g_i_CADPre = PWMdata->EPwm4CADPre;
			Epwm_g_i_timeconti = PWMdata->Epwm4_timeconti;			
			break;
		case 4:
			EPwm_g_i_PRDPre = PWMdata->EPwm5PRDPre;
			EPwm_g_i_CADPre = PWMdata->EPwm5CADPre;
			Epwm_g_i_timeconti = PWMdata->Epwm5_timeconti;			
			break;
		case 5:
			EPwm_g_i_PRDPre = PWMdata->EPwm6PRDPre;
			EPwm_g_i_CADPre = PWMdata->EPwm6CADPre;
			Epwm_g_i_timeconti = PWMdata->Epwm6_timeconti;		
			break;
	}		

	switch(g_i)
	{
		case 0:
			PWMdata->PRDPreTest = PWMdata->PRDPreTest + PWMdata->EPwm1PRDPre;
			PWMdata->CADPreTest = PWMdata->CADPreTest + PWMdata->EPwm1CADPre;
		break;
		case 1:
			PWMdata->PRDPreTest = PWMdata->PRDPreTest + PWMdata->EPwm2PRDPre*4;
			PWMdata->CADPreTest = PWMdata->CADPreTest + PWMdata->EPwm2CADPre*4;
		break;
		case 2:
			PWMdata->PRDPreTest = PWMdata->PRDPreTest + PWMdata->EPwm3PRDPre*16;
			PWMdata->CADPreTest = PWMdata->CADPreTest + PWMdata->EPwm3CADPre*16;
		break;
		case 3:
			PWMdata->PRDPreTest = PWMdata->PRDPreTest + PWMdata->EPwm4PRDPre*64;
			PWMdata->CADPreTest = PWMdata->CADPreTest + PWMdata->EPwm4CADPre*64;
		break;
		case 4:
			PWMdata->PRDPreTest = PWMdata->PRDPreTest + PWMdata->EPwm5PRDPre*256;
			PWMdata->CADPreTest = PWMdata->CADPreTest + PWMdata->EPwm5CADPre*256;
		break;
		case 5:
			PWMdata->PRDPreTest = PWMdata->PRDPreTest + PWMdata->EPwm6PRDPre*1024;
			PWMdata->CADPreTest = PWMdata->CADPreTest + PWMdata->EPwm6CADPre*1024;
		break;
	}
	//LL-LL
	if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==2)&&(CAU[g_i]==0) ))
	{
		Epwm_g_i_timeconti += T1PRHalfSHE;
		PWMdata->BranchTest[g_i] = 1;
	}
	//LL-HH
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==1)&&(CAU[g_i]==0) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 2;
			Epwm_g_i_timeconti = T1PRHalfSHE;  	  	
		}
		else //实际不会执行，MinP<T1PRHalfSHE 
		{
			PWMdata->BranchTest[g_i] = 3;
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				ZRO[g_i]=2;CAU[g_i]=3;
				CMPA[g_i] =MinP - Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
			PWMdata->BranchTest[g_i] = 4;
			ZRO[g_i]=2;CAU[g_i]=0;
			Epwm_g_i_timeconti += T1PRHalfSHE;
			}
		}
	}
	//LL-LH
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==2)&&(CAU[g_i]==3) ))	
	{
		if( (Epwm_g_i_timeconti + CMPA[g_i])>=MinP)
		{
			PWMdata->BranchTest[g_i] = 5;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
		}	
		else
		{
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )/*能够补上*/  /*移动CMPA使满足最小脉宽，若MP设置为150us，7200Hz采样频率可能导致这种情形*/
			{
				PWMdata->BranchTest[g_i] = 6;
				CMPA[g_i] =MinP - Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 7;
				ZRO[g_i]=2;CAU[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	    	
			}		
		}	
	}
	//LL-HL
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==1)&&(CAU[g_i]==3) ))	  
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if (CMPA[g_i]>=MinP)
			{
			PWMdata->BranchTest[g_i] = 8;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}
			else if (CMPA[g_i]<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 9;
				CMPA[g_i] =MinP;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 10;
				ZRO[g_i]=1;CAU[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
	  }
		else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 11;
		ZRO[g_i]=2;CAU[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;	
		}
	}
	//LH-LL
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==2)&&(CAU[g_i]==0) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 12;
			Epwm_g_i_timeconti = T1PRHalfSHE;
		}	       	
		else if(Epwm_g_i_timeconti<MinP)
		{
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 13;
				ZRO[g_i]=1;CAU[g_i]=3;
				CMPA[g_i] =MinP - Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 14;
				ZRO[g_i]=1;CAU[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	    	
			}		
	    }
	}
	//LH-LH
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==2)&&(CAU[g_i]==3) ))		
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if (CMPA[g_i]>=MinP)
			{
			PWMdata->BranchTest[g_i] = 15;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}
			else if (CMPA[g_i]<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 16;
				CMPA[g_i] =MinP;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 17;
				ZRO[g_i]=2;CAU[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
		}
	else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 18;
		ZRO[g_i]=1;CAU[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;
	    }	
	}
	//LH-HH
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==1)&&(CAU[g_i]==0) ))
	{
		PWMdata->BranchTest[g_i] = 19;
		Epwm_g_i_timeconti += T1PRHalfSHE;	
	}
	//LH-HL
	else if(((EPwm_g_i_PRDPre==2)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==1)&&(CAU[g_i]==3) ))		/*8th/16*/
	{
		if( (Epwm_g_i_timeconti + CMPA[g_i])>=MinP )
		{
			PWMdata->BranchTest[g_i] = 20;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
		}
		else
		{
		   if( (MinP - Epwm_g_i_timeconti)<T1PRHalfSHE )
		   {
				 PWMdata->BranchTest[g_i] = 21;
				CMPA[g_i] = MinP - Epwm_g_i_timeconti;  
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];    	
		   }   
		   else
		   {
				 PWMdata->BranchTest[g_i] = 22;
				ZRO[g_i]=1;CAU[g_i]=0;/*SET_HH*/
				Epwm_g_i_timeconti += T1PRHalfSHE; 
		   }	
		}
	}
//HH-LL
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==2)&&(CAU[g_i]==0) ))	
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 23;
			Epwm_g_i_timeconti = T1PRHalfSHE;  	  	
		}
		else //实际不会执行，MinP<T1PRHalfSHE 
		{
			PWMdata->BranchTest[g_i] = 24;
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				ZRO[g_i]=1;CAU[g_i]=3;
				CMPA[g_i] =MinP - Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
			PWMdata->BranchTest[g_i] = 25;
			ZRO[g_i]=1;CAU[g_i]=0;
			Epwm_g_i_timeconti += T1PRHalfSHE;
			}
		}
	}
	//HH-LH
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==2)&&(CAU[g_i]==3) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if (CMPA[g_i]>=MinP)
			{
			PWMdata->BranchTest[g_i] = 26;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}
			else if (CMPA[g_i]<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 27;
				CMPA[g_i] =MinP;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 28;
				ZRO[g_i]=2;CAU[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
	  }
		else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 29;
		ZRO[g_i]=1;CAU[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;	
		}
	}
	//HH-HH
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==1)&&(CAU[g_i]==0) ))
	{
	PWMdata->BranchTest[g_i] = 30;
	Epwm_g_i_timeconti += T1PRHalfSHE;	
	}
	//HH-HL
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==0))&&( (ZRO[g_i]==1)&&(CAU[g_i]==3) ))   
	{
		if( (Epwm_g_i_timeconti + CMPA[g_i])>=MinP )
		{
			PWMdata->BranchTest[g_i] = 31;
		 Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
		}
		else
		{
		  if( (MinP - Epwm_g_i_timeconti)<T1PRHalfSHE )
		  {
			PWMdata->BranchTest[g_i] = 32;
			  CMPA[g_i] = MinP - Epwm_g_i_timeconti;
			  Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
		  }  
		  else
		  {
			PWMdata->BranchTest[g_i] = 33;
			  ZRO[g_i]=1;CAU[g_i]=0;
			  Epwm_g_i_timeconti +=T1PRHalfSHE;	
		  }  
		} 	
	}
 //HL-LL
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==2)&&(CAU[g_i]==0) ))	
	{
		PWMdata->BranchTest[g_i] = 34;
		Epwm_g_i_timeconti += T1PRHalfSHE;
	}
 //HL-LH
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==2)&&(CAU[g_i]==3) ))		
	{
		if( (Epwm_g_i_timeconti+CMPA[g_i])>=MinP )
		{
			PWMdata->BranchTest[g_i] = 35;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
		}
		else
		{
			if( (MinP - Epwm_g_i_timeconti)<T1PRHalfSHE)		
			{
				PWMdata->BranchTest[g_i] = 36;
				CMPA[g_i] = MinP - Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}
			else
			{    
		        PWMdata->BranchTest[g_i] = 37;			
				ZRO[g_i]=2;CAU[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	
			}	
		}	
	}
	//HL-HH
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==1)&&(CAU[g_i]==0) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 38;
			Epwm_g_i_timeconti = T1PRHalfSHE;
		}	       	
		else if(Epwm_g_i_timeconti<MinP)
		{
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 39;
				ZRO[g_i]=2;CAU[g_i]=3;
				CMPA[g_i] =MinP - Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 40;
				ZRO[g_i]=2;CAU[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	    	
			}		
	    }
	}
	//HL-HL
	else if(((EPwm_g_i_PRDPre==1)&&(EPwm_g_i_CADPre==3))&&( (ZRO[g_i]==1)&&(CAU[g_i]==3) ))	  /*16th/16*/
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if (CMPA[g_i]>=MinP)
			{
			PWMdata->BranchTest[g_i] = 41;
			Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}
			else if (CMPA[g_i]<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 42;
				CMPA[g_i] =MinP;
				Epwm_g_i_timeconti = T1PRHalfSHE - CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 43;
				ZRO[g_i]=1;CAU[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
		}
	else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 44;
		ZRO[g_i]=2;CAU[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;
	    }	
	}

		if(Epwm_g_i_timeconti> (25000) )  // Limit the value into 16bits, MP = 2500
			Epwm_g_i_timeconti = 25000;
	switch(g_i)/*att2*/
		{	
			case 0:
				PWMdata->Epwm1_timeconti = Epwm_g_i_timeconti;	
				break;
			case 1:
				PWMdata->Epwm2_timeconti = Epwm_g_i_timeconti;	
				break;
			case 2:
				PWMdata->Epwm3_timeconti = Epwm_g_i_timeconti;	
				break;
			case 3:
				PWMdata->Epwm4_timeconti = Epwm_g_i_timeconti;			
				break;
			case 4:
				PWMdata->Epwm5_timeconti = Epwm_g_i_timeconti;			
				break;
			case 5:
				PWMdata->Epwm6_timeconti = Epwm_g_i_timeconti;			
				break;
		}
}/* end of MinPulseSetAse(void) */
/************************************************************************************/
/*Name		:MinPulseSetDes
*Description:最小脉宽设置，在减计数时刻
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void MinPulseSetDes(struct PWMdataStruct *PWMdata)
{
	PWMdata->MinPulseFlag = 1;
	switch(g_i)/*att2*/
	{
		case 0:
			EPwm_g_i_ZROPre = PWMdata->EPwm1ZROPre;
			EPwm_g_i_CAUPre = PWMdata->EPwm1CAUPre;
			Epwm_g_i_timeconti = PWMdata->Epwm1_timeconti;	
			break;
		case 1:
			EPwm_g_i_ZROPre = PWMdata->EPwm2ZROPre;
			EPwm_g_i_CAUPre = PWMdata->EPwm2CAUPre;
			Epwm_g_i_timeconti = PWMdata->Epwm2_timeconti;	
			break;
		case 2:
			EPwm_g_i_ZROPre = PWMdata->EPwm3ZROPre;
			EPwm_g_i_CAUPre = PWMdata->EPwm3CAUPre;
			Epwm_g_i_timeconti = PWMdata->Epwm3_timeconti;		
			break;
		case 3:
			EPwm_g_i_ZROPre = PWMdata->EPwm4ZROPre;
			EPwm_g_i_CAUPre = PWMdata->EPwm4CAUPre;
			Epwm_g_i_timeconti = PWMdata->Epwm4_timeconti;			
			break;
		case 4:
			EPwm_g_i_ZROPre = PWMdata->EPwm5ZROPre;
			EPwm_g_i_CAUPre = PWMdata->EPwm5CAUPre;
			Epwm_g_i_timeconti = PWMdata->Epwm5_timeconti;			
			break;
		case 5:
			EPwm_g_i_ZROPre = PWMdata->EPwm6ZROPre;
			EPwm_g_i_CAUPre = PWMdata->EPwm6CAUPre;
			Epwm_g_i_timeconti = PWMdata->Epwm6_timeconti;		
			break;
	}	
	switch(g_i)
	{
		case 0:
			PWMdata->ZROPreTest = PWMdata->ZROPreTest + PWMdata->EPwm1ZROPre;
			PWMdata->CAUPreTest = PWMdata->CAUPreTest + PWMdata->EPwm1CAUPre;
		break;
		case 1:
			PWMdata->ZROPreTest = PWMdata->ZROPreTest + PWMdata->EPwm2ZROPre*4;
			PWMdata->CAUPreTest = PWMdata->CAUPreTest + PWMdata->EPwm2CAUPre*4;
		break;
		case 2:
			PWMdata->ZROPreTest = PWMdata->ZROPreTest + PWMdata->EPwm3ZROPre*16;
			PWMdata->CAUPreTest = PWMdata->CAUPreTest + PWMdata->EPwm3CAUPre*16;
		break;
		case 3:
			PWMdata->ZROPreTest = PWMdata->ZROPreTest + PWMdata->EPwm4ZROPre*64;
			PWMdata->CAUPreTest = PWMdata->CAUPreTest + PWMdata->EPwm4CAUPre*64;
		break;
		case 4:
			PWMdata->ZROPreTest = PWMdata->ZROPreTest + PWMdata->EPwm5ZROPre*256;
			PWMdata->CAUPreTest = PWMdata->CAUPreTest + PWMdata->EPwm5CAUPre*256;
		break;
		case 5:
			PWMdata->ZROPreTest = PWMdata->ZROPreTest + PWMdata->EPwm6ZROPre*1024;
			PWMdata->CAUPreTest = PWMdata->CAUPreTest + PWMdata->EPwm6CAUPre*1024;
		break;
	}

	//LL-LL
	if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==2)&&(CAD[g_i]==0) ))
	{
		Epwm_g_i_timeconti += T1PRHalfSHE;
		PWMdata->BranchTest[g_i] = 45;
	}
	//LL-HH
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==1)&&(CAD[g_i]==0) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 46;
			Epwm_g_i_timeconti = T1PRHalfSHE;  	  	
		}
		else //实际不会执行，MinP<T1PRHalfSHE 
		{
			PWMdata->BranchTest[g_i] = 47;
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				PRD[g_i]=2;CAD[g_i]=3;
				CMPA[g_i] = T1PRHalfSHE - MinP + Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
			PWMdata->BranchTest[g_i] = 48;
			PRD[g_i]=2;CAD[g_i]=0;
			Epwm_g_i_timeconti += T1PRHalfSHE;
			}
		}
	}
	//LL-LH
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==2)&&(CAD[g_i]==3) ))	
	{
		if( (Epwm_g_i_timeconti + (T1PRHalfSHE - CMPA[g_i]))>=MinP)
		{
			PWMdata->BranchTest[g_i] = 49;
			Epwm_g_i_timeconti = CMPA[g_i];
		}	
		else
		{
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )/*能够补上*/  /*移动CMPA使满足最小脉宽，若MP设置为150us，7200Hz采样频率可能导致这种情形*/
			{
				PWMdata->BranchTest[g_i] = 50;
				CMPA[g_i] = T1PRHalfSHE - MinP + Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 51;
				PRD[g_i]=2;CAD[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	    	
			}		
		}	
	}
	//LL-HL
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==1)&&(CAD[g_i]==3) ))	  
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if ((T1PRHalfSHE - CMPA[g_i])>=MinP)
			{
			PWMdata->BranchTest[g_i] = 52;
			Epwm_g_i_timeconti = CMPA[g_i];
			}
			else if ((T1PRHalfSHE - CMPA[g_i])<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 53;
				CMPA[g_i] =(T1PRHalfSHE - MinP);
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 54;
				PRD[g_i]=1;CAD[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
	  }
		else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 55;
		PRD[g_i]=2;CAD[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;	
		}
	}
	//LH-LL
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==2)&&(CAD[g_i]==0) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 56;
			Epwm_g_i_timeconti = T1PRHalfSHE;
		}	       	
		else if(Epwm_g_i_timeconti<MinP)
		{
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 57;
				PRD[g_i]=1;CAD[g_i]=3;
				CMPA[g_i] = T1PRHalfSHE - MinP + Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 58;
				PRD[g_i]=1;CAD[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	    	
			}		
	    }
	}
	//LH-LH
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==2)&&(CAD[g_i]==3) ))		
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if ((T1PRHalfSHE - CMPA[g_i])>=MinP)
			{
			PWMdata->BranchTest[g_i] = 59;
			Epwm_g_i_timeconti = CMPA[g_i];
			}
			else if ((T1PRHalfSHE - CMPA[g_i])<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 60;
				CMPA[g_i] =(T1PRHalfSHE - MinP);
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 61;
				PRD[g_i]=2;CAD[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
		}
	else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 62;
		PRD[g_i]=1;CAD[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;
	    }	
	}
	//LH-HH
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==1)&&(CAD[g_i]==0) ))
	{
		PWMdata->BranchTest[g_i] = 63;
		Epwm_g_i_timeconti += T1PRHalfSHE;	
	}
	//LH-HL
	else if(((EPwm_g_i_ZROPre==2)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==1)&&(CAD[g_i]==3) ))		/*8th/16*/
	{
		if( (Epwm_g_i_timeconti + (T1PRHalfSHE - CMPA[g_i]))>=MinP )
		{
			PWMdata->BranchTest[g_i] = 64;
			Epwm_g_i_timeconti = CMPA[g_i];
		}
		else
		{
		   if( (MinP - Epwm_g_i_timeconti)<T1PRHalfSHE )
		   {
				 PWMdata->BranchTest[g_i] = 65;
				CMPA[g_i] = T1PRHalfSHE-MinP +Epwm_g_i_timeconti;  
				Epwm_g_i_timeconti = CMPA[g_i];    	
		   }   
		   else
		   {
				 PWMdata->BranchTest[g_i] = 66;
				PRD[g_i]=1;CAD[g_i]=0;/*SET_HH*/
				Epwm_g_i_timeconti += T1PRHalfSHE; 
		   }	
		}
	}
//HH-LL
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==2)&&(CAD[g_i]==0) ))	
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 67;
			Epwm_g_i_timeconti = T1PRHalfSHE;  	  	
		}
		else //实际不会执行，MinP<T1PRHalfSHE 
		{
			PWMdata->BranchTest[g_i] = 68;
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				PRD[g_i]=1;CAD[g_i]=3;
				CMPA[g_i] = T1PRHalfSHE - MinP + Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
			PWMdata->BranchTest[g_i] = 69;
			PRD[g_i]=1;CAD[g_i]=0;
			Epwm_g_i_timeconti += T1PRHalfSHE;
			}
		}
	}
	//HH-LH
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==2)&&(CAD[g_i]==3) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if ((T1PRHalfSHE - CMPA[g_i])>=MinP)
			{
			PWMdata->BranchTest[g_i] = 70;
			Epwm_g_i_timeconti = CMPA[g_i];
			}
			else if ((T1PRHalfSHE - CMPA[g_i])<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 71;
				CMPA[g_i] =(T1PRHalfSHE - MinP);
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 72;
				PRD[g_i]=2;CAD[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
	  }
		else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 73;
		PRD[g_i]=1;CAD[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;	
		}
	}
	//HH-HH
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==1)&&(CAD[g_i]==0) ))
	{
	PWMdata->BranchTest[g_i] = 74;
	Epwm_g_i_timeconti += T1PRHalfSHE;	
	}
	//HH-HL
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==0))&&( (PRD[g_i]==1)&&(CAD[g_i]==3) ))   
	{
		if( (Epwm_g_i_timeconti + (T1PRHalfSHE - CMPA[g_i]))>=MinP )
		{
			PWMdata->BranchTest[g_i] = 75;
		 Epwm_g_i_timeconti = CMPA[g_i];
		}
		else
		{
		  if( (MinP - Epwm_g_i_timeconti)<T1PRHalfSHE )
		  {
			PWMdata->BranchTest[g_i] = 76;
			  CMPA[g_i] = T1PRHalfSHE-MinP +Epwm_g_i_timeconti;
			  Epwm_g_i_timeconti = CMPA[g_i];
		  }  
		  else
		  {
			PWMdata->BranchTest[g_i] = 77;
			  PRD[g_i]=1;CAD[g_i]=0;
			  Epwm_g_i_timeconti +=T1PRHalfSHE;	
		  }  
		} 	
	}
 //HL-LL
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==2)&&(CAD[g_i]==0) ))	
	{
		PWMdata->BranchTest[g_i] = 78;
		Epwm_g_i_timeconti += T1PRHalfSHE;
	}
 //HL-LH
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==2)&&(CAD[g_i]==3) ))		
	{
		if( (Epwm_g_i_timeconti+(T1PRHalfSHE - CMPA[g_i]))>=MinP )
		{
			PWMdata->BranchTest[g_i] = 79;
			Epwm_g_i_timeconti = CMPA[g_i];
		}
		else
		{
			if( (MinP - Epwm_g_i_timeconti)<T1PRHalfSHE)		
			{
				PWMdata->BranchTest[g_i] = 80;
				CMPA[g_i] = T1PRHalfSHE-MinP +Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = CMPA[g_i];
			}
			else
			{    
		        PWMdata->BranchTest[g_i] = 81;			
				PRD[g_i]=2;CAD[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	
			}	
		}	
	}
	//HL-HH
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==1)&&(CAD[g_i]==0) ))
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			PWMdata->BranchTest[g_i] = 82;
			Epwm_g_i_timeconti = T1PRHalfSHE;
		}	       	
		else if(Epwm_g_i_timeconti<MinP)
		{
			if( (MinP -	Epwm_g_i_timeconti)<T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 83;
				PRD[g_i]=2;CAD[g_i]=3;
				CMPA[g_i] = T1PRHalfSHE - MinP + Epwm_g_i_timeconti;
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 84;
				PRD[g_i]=2;CAD[g_i]=0;
				Epwm_g_i_timeconti += T1PRHalfSHE;	    	
			}		
	    }
	}
	//HL-HL
	else if(((EPwm_g_i_ZROPre==1)&&(EPwm_g_i_CAUPre==3))&&( (PRD[g_i]==1)&&(CAD[g_i]==3) ))	  /*16th/16*/
	{
		if(Epwm_g_i_timeconti>=MinP)
		{
			if ((T1PRHalfSHE - CMPA[g_i])>=MinP)
			{
			PWMdata->BranchTest[g_i] = 85;
			Epwm_g_i_timeconti = CMPA[g_i];
			}
			else if ((T1PRHalfSHE - CMPA[g_i])<MinP)
		 {
              if( MinP <T1PRHalfSHE )
			{
				PWMdata->BranchTest[g_i] = 86;
				CMPA[g_i] =(T1PRHalfSHE - MinP);
				Epwm_g_i_timeconti = CMPA[g_i];
			}	    	    
			else
			{
				PWMdata->BranchTest[g_i] = 87;
				PRD[g_i]=1;CAD[g_i]=0;
				Epwm_g_i_timeconti = T1PRHalfSHE;	    	
			}		
		}
		}
	else if (Epwm_g_i_timeconti<MinP)
		{
		PWMdata->BranchTest[g_i] = 88;
		PRD[g_i]=2;CAD[g_i]=0;
		Epwm_g_i_timeconti += T1PRHalfSHE;
	    }	
	}


		if(Epwm_g_i_timeconti> (25000) )  // Limit the value into 16bits, MP = 2500
			Epwm_g_i_timeconti = 25000;
	switch(g_i)/*att2*/
	{
		case 0:
			PWMdata->Epwm1_timeconti = Epwm_g_i_timeconti;	
			break;
		case 1:
			PWMdata->Epwm2_timeconti = Epwm_g_i_timeconti;	
			break;
		case 2:
			PWMdata->Epwm3_timeconti = Epwm_g_i_timeconti;	
			break;
		case 3:
			PWMdata->Epwm4_timeconti = Epwm_g_i_timeconti;			
			break;
		case 4:
			PWMdata->Epwm5_timeconti = Epwm_g_i_timeconti;			
			break;
		case 5:
			PWMdata->Epwm6_timeconti = Epwm_g_i_timeconti;			
			break;
	}		
}	/* end of MinPulseSetDes(void) */
/***********************************************************/
/*Name		:ChangeTsSHE
*Description:在切换成SHE算法时，改变PWM定时器周期值到SHE周期
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void ChangeTsSHE(struct PWMdataStruct *PWMdata)
{

	PWMdata->Ts = TsSHE;  /* 3600Hz */
	PWMdata->T1PRHalf = T1PRHalfSHE;  /* 50M / 7200 = 6944 */
	PWMdata->T1PR = T1PRSHE;
	PWMdata->InvModulationFlag = 1;
	
/*	*(short *)(PWMdata->PWMTransLocalAddr+0x08) = 1;    
	*(short *)(PWMdata->PWMTransLocalAddr+0x06)=(short)(T1PRSHE);*//*20160531 del*/

	if(SHE_VME_Flag == 0){    /*只在SVM切换到SHE时执行一次 20150619 */
	PWMdata->Epwm1_timeconti = MinP+1; /* MinP+1*/
	PWMdata->Epwm2_timeconti = MinP+1;
	PWMdata->Epwm3_timeconti = MinP+1;
	PWMdata->Epwm4_timeconti = MinP+1;
	PWMdata->Epwm5_timeconti = MinP+1;
	PWMdata->Epwm6_timeconti = MinP+1;
	PWMdata->Epwm_g_i_timeconti = MinP+1;
	CntTestConti++;
	}
	SHE_VME_Flag = 1;
	/* } */
	SVM_VME_Flag = 0;
}
/***********************************************************/
/*Name		:SHEMinPulseCheck
*Description:SHE算法时小脉冲检测
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void SHEMinPulseCheck(struct PWMdataStruct *PWMdata)
{
	if((PWMdata->Epwm1_timeconti>=MinP)&&(PWMdata->Epwm2_timeconti>=MinP)&&(PWMdata->Epwm3_timeconti>=MinP)&&(PWMdata->Epwm4_timeconti>=MinP)&&(PWMdata->Epwm5_timeconti>=MinP)&&(PWMdata->Epwm6_timeconti>=MinP))
		PWMdata->MinPulseOK=1;
	else
		PWMdata->MinPulseOK=0;
}

void resetSHE(struct PWMdataStruct *PWMdata)
{
	short i;
	
/* 	PWMdata->vvvf1 = 20.0;
	PWMdata->Kv = 0.02;
	PWMdata->VFGama = 0;
	PWMdata->VFGama_Pre = 0;
	PWMdata->Ts = TsSVM; */   // 20160601 cdd
	
	for(i=0;i<6;i++)
	{ 
		PWMdata->CMPA[i]=0;
		PWMdata->VFGama6[i] = 0.01;
		PWMdata->VFGama_Pre6[i]= 0.01 ;
		PWMdata->gama_Num[i]= 0 ;
		PWMdata->gama_Num_pre[i]= 0 ;
	}
	
	PWMdata->Reg_UpDown = 0;
	PWMdata->IntFlag = 1;
	PWMdata->Gama_TotalNum_Pre = 0;
	PWMdata->HalfGama_TotalNum = 0;
	PWMdata->Gama_TotalNum = 0;
	
/*	PWMdata->Epwm1_timeconti = MinP+1;
	PWMdata->Epwm2_timeconti = MinP+1;
	PWMdata->Epwm3_timeconti = MinP+1;
	PWMdata->Epwm4_timeconti = MinP+1;
	PWMdata->Epwm5_timeconti = MinP+1;
	PWMdata->Epwm6_timeconti = MinP+1;
*/
	PWMdata->Epwm1_timeconti = 0;
	PWMdata->Epwm2_timeconti = 0;
	PWMdata->Epwm3_timeconti = 0;
	PWMdata->Epwm4_timeconti = 0;
	PWMdata->Epwm5_timeconti = 0;
	PWMdata->Epwm6_timeconti = 0;

	PWMdata->Epwm_g_i_timeconti = MinP+1;

	PWMdata->EPwm1PRDPre=AQ_CLEAR;
	PWMdata->EPwm1CADPre=AQ_NO_ACTION;
	PWMdata->EPwm1ZROPre=AQ_CLEAR;
	PWMdata->EPwm1CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm2PRDPre=AQ_CLEAR;
	PWMdata->EPwm2CADPre=AQ_NO_ACTION;
	PWMdata->EPwm2ZROPre=AQ_CLEAR;
	PWMdata->EPwm2CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm3PRDPre=AQ_CLEAR;
	PWMdata->EPwm3CADPre=AQ_NO_ACTION;
	PWMdata->EPwm3ZROPre=AQ_CLEAR;
	PWMdata->EPwm3CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm4PRDPre=AQ_CLEAR;
	PWMdata->EPwm4CADPre=AQ_NO_ACTION;
	PWMdata->EPwm4ZROPre=AQ_CLEAR;
	PWMdata->EPwm4CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm5PRDPre=AQ_CLEAR;
	PWMdata->EPwm5CADPre=AQ_NO_ACTION;
	PWMdata->EPwm5ZROPre=AQ_CLEAR;
	PWMdata->EPwm5CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm6PRDPre=AQ_CLEAR;
	PWMdata->EPwm6CADPre=AQ_NO_ACTION;
	PWMdata->EPwm6ZROPre=AQ_CLEAR;
	PWMdata->EPwm6CAUPre=AQ_NO_ACTION;
	 
	PWMdata->EPwm1EndState=1;
	PWMdata->EPwm2EndState=1;
	PWMdata->EPwm3EndState=1;
	PWMdata->EPwm4EndState=1;
	PWMdata->EPwm5EndState=1;
	PWMdata->EPwm6EndState=1;
	PWMdata->EPwmEndState=0;  

	PWMdata->EPwm1PRDPre=AQ_CLEAR;
	PWMdata->EPwm1CADPre=AQ_NO_ACTION;
	PWMdata->EPwm1ZROPre=AQ_CLEAR;
	PWMdata->EPwm1CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm2PRDPre=AQ_CLEAR;
	PWMdata->EPwm2CADPre=AQ_NO_ACTION;
	PWMdata->EPwm2ZROPre=AQ_CLEAR;
	PWMdata->EPwm2CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm3PRDPre=AQ_CLEAR;
	PWMdata->EPwm3CADPre=AQ_NO_ACTION;
	PWMdata->EPwm3ZROPre=AQ_CLEAR;
	PWMdata->EPwm3CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm4PRDPre=AQ_CLEAR;
	PWMdata->EPwm4CADPre=AQ_NO_ACTION;
	PWMdata->EPwm4ZROPre=AQ_CLEAR;
	PWMdata->EPwm4CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm5PRDPre=AQ_CLEAR;
	PWMdata->EPwm5CADPre=AQ_NO_ACTION;
	PWMdata->EPwm5ZROPre=AQ_CLEAR;
	PWMdata->EPwm5CAUPre=AQ_NO_ACTION;

	PWMdata->EPwm6PRDPre=AQ_CLEAR;
	PWMdata->EPwm6CADPre=AQ_NO_ACTION;
	PWMdata->EPwm6ZROPre=AQ_CLEAR;
	PWMdata->EPwm6CAUPre=AQ_NO_ACTION;
	
	PWMdata->ACQ_ZRO = 0x0999;
	PWMdata->ACQ_CAU = 0;
	PWMdata->ACQ_PRD = 0x0999;
	PWMdata->ACQ_CAD = 0;
	 
	PWMdata->EPwm1EndState=1;
	PWMdata->EPwm2EndState=1;
	PWMdata->EPwm3EndState=1;
	PWMdata->EPwm4EndState=1;
	PWMdata->EPwm5EndState=1;
	PWMdata->EPwm6EndState=1;
	PWMdata->EPwmEndState=0;
	
	PWMdata->FirstEnPWMFlag = 1;
	PWMdata->vvvf1IncreFlag = 1;
// 	  PWMdata->InvModulationFlag = 0;    // 20160601 cdd
	PWMdata->PWM4Cnt = 1;
 //   	PWMdata->T1PR = T1PRSVM;
//	PWMdata->T1PRHalf = T1PRHalfSVM; 
//	PWMdata->Ts = TsSVM;     // 20160601 cdd */
	PWMdata->Change_SL=0;
	PWMdata->SVMModeSwitchUp = 0;
	PWMdata->SVMModeSwitchDn = 0;
	PWMdata->SwitchSpeedDnOnce = 0;
	PWMdata->SwitchSpeedUpOnce = 0;
	PWMdata->ChangeSHENPJumpTest = 1;
	PWMdata->ModeFlag = 0;
	PWMdata->ChangeSHENPJumpFlag = 0;
	PWMdata->SVPWMType = 0;
	PWMdata->SHEReady = 0;
	PWMdata->MinPulseOK= 1 ;
	
}
/***********************************************************/
/*Name		:SHEPWM
*Description:SHE算法 
*Input		:*PWMdata PWM数据结构体指针		
*Output		:无
*Return		:无
*History	:2016,by cdd
*/
void SHEPWM(struct PWMdataStruct *PWMdata)
{
/*     ModeFlag = 4; */
		int i ;
	SHE2SVFLAG = 1;	
	PWMdata->FirstEnPWMFlag = 1;
	Kv = PWMdata->Kv;
	VFGama_pre=VFGama;
	VFGama = PWMdata->VFGama;
//	VFGama =  VFGama+2*pi*PWMdata->vvvf1/7200./2+2*pi*PWMdata->vvvf1/7200.;
	VFGama =  VFGama+8*pi*rxFre/7200.;
	while( VFGama<0)      VFGama= VFGama+2*PI;
	while( VFGama>2*PI)     VFGama= VFGama-2*PI;
	for(i=0;i<6;i++)
	{
		VFGama_Pre6[i]=PWMdata->VFGama_Pre6[i];
	}
/* 	SwitchCnt ++;  // 用于SHE到SVM切换的判据变量 */
	if((PWMdata->SHEShutFlag)==0)	/* 正常的SHEPWM */
	{
		Prdf1 = PWMdata->vvvf1;
		TsGama = 2 * pi * Prdf1 * 0.5 * PWMdata->Ts;	//VFGama-VFGama_pre;/* 半采样周期相位增量 */
		PWMdata->Kv=PWMdata->Kv;                  //zeng jia de chengxu
		ma_index = (int)ceil( PWMdata->Kv * 100);	/* 调制比，范围0-100 */
	// if(ma_index>100) ma_index = 100;                //ping bi de cheng xu 
             if(ma_index>100)                                //zeng jia de chengxu
                 {
                   if(PWMdata->HalfGama_TotalNum == 1) 
                    {
                      if(ma_index>110)
                        {ma_index=110;}
                      else
                        {ma_index=ma_index;}
                    }
                  else
                     ma_index=100;
                 }   


		PWMdata->Gama_TotalNum_Pre = PWMdata->Gama_TotalNum;	

		//频率上升段
	if (PWMdata->vvvf1IncreFlag==1)
{
	//SHE11
	//	if((PWMdata->vvvf1 < f2point) && (PWMdata->HalfGama_TotalNum!=9))
	//	{
	//		PWMdata->Gama_TotalNum = 22;
	//		PWMdata->HalfGama_TotalNum = 11;
	//		TbStartPointer = SHE_11;	
 //        }
 // //SHE_9
 //else if((PWMdata->vvvf1 < f3point) && (PWMdata->HalfGama_TotalNum!=7))
	//	{
	//		PWMdata->Gama_TotalNum = 18;
	//		PWMdata->HalfGama_TotalNum = 9;
	//		TbStartPointer = SHE_9;  	
	//	}
 ////SHE_7
 //else if( (PWMdata->vvvf1 < f4point) && (PWMdata->HalfGama_TotalNum!=5)) 
	//	{
	//		PWMdata->Gama_TotalNum = 14;
	//		PWMdata->HalfGama_TotalNum = 7;
	//		TbStartPointer = SHE_7;		
	//	}
 // //SHE_5
 //else if( (PWMdata->vvvf1 < f5point) && (PWMdata->HalfGama_TotalNum!=3)) 
	//{
	//	PWMdata->Gama_TotalNum = 10;
	//	PWMdata->HalfGama_TotalNum = 5;
	//	TbStartPointer = SHE_5;  
	//}
 // //SHE_3
 //else 
  if((PWMdata->vvvf1 < f6point)&&(PWMdata->HalfGama_TotalNum!=1) )//HalfGama_TotalNum!=1防止切到SHE1频率下降但标志位没有变又切回来SHE3，防止来回切				  	
	{
		PWMdata->Gama_TotalNum = 6;
		PWMdata->HalfGama_TotalNum = 3;
		TbStartPointer = SHE_3;
	}
 else /* SHE_1 */
	{
		PWMdata->Gama_TotalNum = 2;
		PWMdata->HalfGama_TotalNum = 1;
		TbStartPointer = SHE_1;
	}
 }
//频率下降段
else if (PWMdata->vvvf1IncreFlag==0)
{
	//SHE1
		if((PWMdata->vvvf1 >= f6pointB) && (PWMdata->HalfGama_TotalNum!=3))
		{
		PWMdata->Gama_TotalNum = 2;
		PWMdata->HalfGama_TotalNum = 1;
		TbStartPointer = SHE_1;
         }
  //SHE_3
 else
		{
		PWMdata->Gama_TotalNum = 6;
		PWMdata->HalfGama_TotalNum = 3;
		TbStartPointer = SHE_3;
		}
 ////SHE_5
 //else if( (PWMdata->vvvf1 >= f4pointB) && (PWMdata->HalfGama_TotalNum!=7)) 
	//	{
	//	PWMdata->Gama_TotalNum = 10;
	//	PWMdata->HalfGama_TotalNum = 5;
	//	TbStartPointer = SHE_5; 
	//	}
 // //SHE_7
 //else if( (PWMdata->vvvf1 >= f3pointB) && (PWMdata->HalfGama_TotalNum!=9)) 
	//{
	//	PWMdata->Gama_TotalNum = 14;
	//	PWMdata->HalfGama_TotalNum = 7;
	//	TbStartPointer = SHE_7;		
	//}
 // //SHE_9
 //else if((PWMdata->vvvf1 >= f2pointB)&&(PWMdata->HalfGama_TotalNum!=11) )//HalfGama_TotalNum!=11防止切到SHE9频率上升但标志位没有变又切回来SHE11，防止来回切				  	
	//{
	//	PWMdata->Gama_TotalNum = 18;
	//	PWMdata->HalfGama_TotalNum = 9;
	//	TbStartPointer = SHE_9;  	
	//}
 //else /* SHE_11 */
	//{
	//	PWMdata->Gama_TotalNum = 22;
	//	PWMdata->HalfGama_TotalNum = 11;
	//	TbStartPointer = SHE_11;	
	//}
 }

		if ( (PWMdata->IntFlag) == 0 )	  //设置ZRO,CAU动作
		{
			PWMdata->PRDPreTest = 0;
			PWMdata->CADPreTest = 0;
			for(g_i = 0; g_i<6; g_i++)
			{
				switch(g_i)
				{
					case 0:
						VFGama6[g_i] = VFGama - 4.712388980385;  /* 3pi/2 */
						break;	
					case 1:	
						VFGama6[g_i] = VFGama - 1.570796326795;  /* pi/2  */
						break;		
					case 2:
						VFGama6[g_i] = VFGama  - 0.523598775598;	/* pi/6 */
						break;
					case 3:
						VFGama6[g_i] = VFGama  - 3.665191429188;  /* 7pi/6 */
						break;	
					case 4:
						VFGama6[g_i] = VFGama  -  2.617993877991;  /* 5pi/6 */
						break;	
					case 5:
						VFGama6[g_i] = VFGama  -  5.759586531581;  /* 11pi/6 */
						break;	 	 	 		
				}
				
				if(VFGama6[g_i]<0)	
					VFGama6[g_i] = VFGama6[g_i] + 6.283185307179;

				PWMdata->gama_Num_pre[g_i] = PWMdata->gama_Num[g_i];
				
				PWMdata->gama_Num[g_i] = 0;			     	    

				/* 计算 gama_Num[g_i]*/
				if(VFGama6[g_i]>=pi)
					PWMdata->gama_Num[g_i] = PWMdata->Gama_TotalNum;
				else
				{
					if((VFGama6[g_i]<Halfpi))/* 0-1/4 */
					{		
						while((VFGama6[g_i] > *(TbStartPointer + ( (ma_index - 1) * (PWMdata->HalfGama_TotalNum) ) + PWMdata->gama_Num[g_i]) ) &&(PWMdata->gama_Num[g_i]<PWMdata->HalfGama_TotalNum)     )
						{ PWMdata->gama_Num[g_i]++; }
					}
					else
					{
						VFGama6_g_iC = pi - VFGama6[g_i];
						while(   (VFGama6_g_iC > *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + PWMdata->gama_Num[g_i]) ) &&(PWMdata->gama_Num[g_i]<PWMdata->HalfGama_TotalNum)     )
						{	PWMdata->gama_Num[g_i]++;	}
						PWMdata->gama_Num[g_i] = PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i]; 	     	      	
					}	
				}
				/* PWMdata->Gama_Num[g_i] = Gama_Num[g_i]; */
					
				/* 计算 gama_Num_pre[g_i] */
				if(PWMdata->Gama_TotalNum_Pre != PWMdata->Gama_TotalNum)
				{
					if(PWMdata->Gama_TotalNum_Pre == 0)/* shesvm */
						VFGama_Pre6[g_i] =  VFGama6[g_i] - (8.726646e-4)*PWMdata->vvvf1; /* VFGama6[g_i] - (1/7.2k)*2*pi*vvvf1 */
					if(VFGama_Pre6[g_i]<0)	
						VFGama_Pre6[g_i] = VFGama_Pre6[g_i] + 6.283185307179;
						PWMdata->gama_Num_pre[g_i] = 0; //{} or not
					if(VFGama_Pre6[g_i]>=pi)
						PWMdata->gama_Num_pre[g_i] = PWMdata->Gama_TotalNum;
					else
					{
						if((VFGama_Pre6[g_i]<Halfpi)) /* 0-1/4 */
						{		
							while(   (VFGama_Pre6[g_i] > *(TbStartPointer + ( (ma_index - 1) * (PWMdata->HalfGama_TotalNum) ) + PWMdata->gama_Num_pre[g_i]) ) &&(PWMdata->gama_Num_pre[g_i]<PWMdata->HalfGama_TotalNum)     )
							{	(PWMdata->gama_Num_pre[g_i])++;	}
						}
						else
						{
							VFGama_Pre6_g_iC = pi - VFGama_Pre6[g_i];
							while(   (VFGama_Pre6_g_iC > *(TbStartPointer + ((ma_index - 1) * ((PWMdata->HalfGama_TotalNum))) + PWMdata->gama_Num_pre[g_i]) ) &&(PWMdata->gama_Num_pre[g_i]<PWMdata->HalfGama_TotalNum)     )
							{	PWMdata->gama_Num_pre[g_i]++;	}
							PWMdata->gama_Num_pre[g_i] = PWMdata->Gama_TotalNum - PWMdata->gama_Num_pre[g_i]; 	     	      	
						}	
					}
				}	
				
				if( (g_i%2)==0 )
				{ 
					if(PWMdata->gama_Num_pre[g_i] == PWMdata->gama_Num[g_i])	/* 前后处于同一SHEPWM开关角度内 */
					{
						if( ((PWMdata->gama_Num[g_i])%2)==0 )
						{
							SHERoute[g_i] = 1;	
							ZRO[g_i] = AQ_CLEAR;
							CAU[g_i] = AQ_NO_ACTION;
						}
						else
						{
							SHERoute[g_i] = 2;	
							ZRO[g_i] = AQ_SET; 			
							CAU[g_i] = AQ_NO_ACTION;				           				    	
						}	
					}
					else	/* 前后处于不同SHEPWM开关角度内，本次采样中间有一个开关角度 */
					{
						if((fabs(VFGama6[g_i] - VFGama_Pre6[g_i])>pi)&&(VFGama != VFGamaStart)    )  /* 基波频率需达3600Hz，SHE不会发生 */         	
						{
							SHERoute[g_i] = 3;	
							ZRO[g_i] = AQ_CLEAR; 		
							CAU[g_i] = AQ_NO_ACTION;
						}
						else if(fabs(PWMdata->gama_Num[g_i] - PWMdata->gama_Num_pre[g_i]) == 2)  /* 双采样间隔138us，表格最小脉宽150us，SHE不会发生 */
						{
							if( ((PWMdata->gama_Num[g_i])%2)==0)
							{
								SHERoute[g_i] = 4;	
								ZRO[g_i] = AQ_CLEAR;	  
								CAU[g_i] = AQ_NO_ACTION; 	
							}	
							else
							{
								SHERoute[g_i] = 5;	
								ZRO[g_i] = AQ_SET; 			
								CAU[g_i] = AQ_NO_ACTION;					              		
							}
						}	
						else		/* 计算deltaGamaSHE，为定位CMPA做准备，SHE主要用到这段 */
						{
							if(VFGama6[g_i]>=VFGama_Pre6[g_i])   
							{    		           				    	  
								if((VFGama6[g_i]<Halfpi))
								{ SHERoute[g_i] = 6;	
								deltGamaSHE = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)) - VFGama_Pre6[g_i];}
								else
								{
									if(PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum)
									{ SHERoute[g_i] = 7;	
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1) - 1);}
									else												      		    	
									{ SHERoute[g_i] = 8;	
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1)); }
									if( PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum) 
										{ //SHERoute[g_i] = 9;	
										deltGamaSHE = GamaTb - VFGama_Pre6[g_i];}
									else
									{ //SHERoute[g_i] = 10;	
									deltGamaSHE = (pi - GamaTb) - VFGama_Pre6[g_i];		}
								}          	  			            	  
							}   	  
							else
							{ 		           				    	  
								if((VFGama6[g_i]>Halfpi))
								{ SHERoute[g_i] = 9;	
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - (PWMdata->gama_Num[g_i]+1) + 1)  -1));
									deltGamaSHE = (-1)*(pi - VFGama_Pre6[g_i]) + GamaTb;												           		 												          		
								}	          	  			            	  
								else
								{
									if(PWMdata->gama_Num[g_i] == (PWMdata->HalfGama_TotalNum))  { SHERoute[g_i] = 10;	
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1-1);}
									else {  SHERoute[g_i] = 11;	
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1);}
															
									if(PWMdata->gama_Num[g_i] == (PWMdata->HalfGama_TotalNum))
										deltGamaSHE = VFGama_Pre6[g_i] - (pi-GamaTb);
									else
										deltGamaSHE = VFGama_Pre6[g_i] - GamaTb;
								}													           				
							}

							CMPA[g_i] = (deltGamaSHE/TsGama) * PWMdata->T1PRHalf;	/* 更新开关比较值 */		
							
							if( ((PWMdata->gama_Num[g_i])%2)==0 )
							{
								ZRO[g_i] = AQ_SET;	 /* 注意此处和上面处于同一开关角度内的不同！ */
								CAU[g_i] = AQ_TOGGLE;              
							}	
							else
							{   		
								 ZRO[g_i] = AQ_CLEAR;	
								 CAU[g_i] = AQ_TOGGLE;  		
							}				           				    	  	
						}	
					}	
				}	/* end of (g_i%2)==0 */
				else
				{
					if(PWMdata->gama_Num_pre[g_i] == PWMdata->gama_Num[g_i])	
					{
						if( ((PWMdata->gama_Num[g_i])%2)==0 )
						{  SHERoute[g_i] = 12;	
							ZRO[g_i] = AQ_SET;
							CAU[g_i] = AQ_NO_ACTION;
						}
						else
						{  SHERoute[g_i] = 13;	
							ZRO[g_i] = AQ_CLEAR; 			
							CAU[g_i] = AQ_NO_ACTION;
						}	
					}
					else
					{
						if(  (fabs(VFGama6[g_i] - VFGama_Pre6[g_i])>pi)&&(VFGama != VFGamaStart)    )         	
						{  SHERoute[g_i] = 14;	
							ZRO[g_i] = AQ_SET; 		
							CAU[g_i] = AQ_NO_ACTION;
						}
						else if(fabs(PWMdata->gama_Num[g_i] - PWMdata->gama_Num_pre[g_i]) == 2)
						{
							if(  ((PWMdata->gama_Num[g_i])%2) ==0)
							{  SHERoute[g_i] = 15;	
								ZRO[g_i] = AQ_SET;	  
								CAU[g_i] = AQ_NO_ACTION; 	
							}
							else
							{  SHERoute[g_i] = 16;
								ZRO[g_i] = AQ_CLEAR; 			
								CAU[g_i] = AQ_NO_ACTION;					              		
							}	
						}	
						else
						{
							if(VFGama6[g_i]>=VFGama_Pre6[g_i])   
							{    		           				    	  
								if((VFGama6[g_i]<Halfpi))  {  SHERoute[g_i] = 17;
								deltGamaSHE = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)) - VFGama_Pre6[g_i]; }
								else
								{
									if(PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum)  {  SHERoute[g_i] = 28;
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1) - 1);  }
									else	{ SHERoute[g_i] = 19;											      		    	
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1)); }
								
									if( PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum) 
										deltGamaSHE = GamaTb - VFGama_Pre6[g_i];												      			 
									else
										deltGamaSHE = (pi - GamaTb) - VFGama_Pre6[g_i];		
								}          	  			            	  
							}   	  
							else
							{ 		           				    	  
								if((VFGama6[g_i]>Halfpi))
								{  SHERoute[g_i] = 20;
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - (PWMdata->gama_Num[g_i]+1) + 1)  -1));
									deltGamaSHE = (-1)*(pi - VFGama_Pre6[g_i]) + GamaTb;											           		 												          		
								}	          	  			            	  
								else
								{
									if(PWMdata->gama_Num[g_i] == (PWMdata->HalfGama_TotalNum))  {  SHERoute[g_i] = 21;
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1-1);  }
									else {  SHERoute[g_i] = 22;
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1);  }

									if(PWMdata->gama_Num[g_i] == (PWMdata->HalfGama_TotalNum))
										deltGamaSHE = VFGama_Pre6[g_i] - (pi-GamaTb);
									else	
										deltGamaSHE = VFGama_Pre6[g_i] - GamaTb;
								}													           				
							}	       				    	  
							
							CMPA[g_i] = (deltGamaSHE/TsGama) * PWMdata->T1PRHalf;	/* 更新比较值 */			           				    	  	          	  
							
							if( ((PWMdata->gama_Num[g_i])%2)==0 )
							{
								ZRO[g_i] = AQ_CLEAR;	 
								CAU[g_i] = AQ_TOGGLE;              
							}	
							else
							{   		
								ZRO[g_i] = AQ_SET;	
								CAU[g_i] = AQ_TOGGLE;  		
							}
						}	
					}	
				}	/* end of (g_i%2)!=0 */

				VFGama_Pre6[g_i] = VFGama6[g_i];/* 保留老角度 */
			
			if(CMPA[0]>PWMdata->T1PRHalf) CMPA[0] = 20;   // 20160727
			if(CMPA[1]>PWMdata->T1PRHalf) CMPA[1] = 20;
			if(CMPA[2]>PWMdata->T1PRHalf) CMPA[2] = 20;
			if(CMPA[3]>PWMdata->T1PRHalf) CMPA[3] = 20;
			if(CMPA[4]>PWMdata->T1PRHalf) CMPA[4] = 20;
			if(CMPA[5]>PWMdata->T1PRHalf) CMPA[5] = 20;
			
 			MinPulseSetAse(PWMdata);  /* 20150422 */ 
			                   /* 20150603，此函数有最小脉宽调整功能，也是SHE2SVM切换所必须 */
			}	/* for循环结束 where IntFlag == ET_CTR_PRD */

			PWMdata->EPwm1ZROPre = ZRO[0];	/* modified, 保持动作的SHEPWM和最小脉宽用到 */
			PWMdata->EPwm2ZROPre = ZRO[1];
			PWMdata->EPwm3ZROPre = ZRO[2];
			PWMdata->EPwm4ZROPre = ZRO[3];
			PWMdata->EPwm5ZROPre = ZRO[4];
			PWMdata->EPwm6ZROPre = ZRO[5];

			PWMdata->EPwm1CAUPre = CAU[0];
			PWMdata->EPwm2CAUPre = CAU[1];
			PWMdata->EPwm3CAUPre = CAU[2];
			PWMdata->EPwm4CAUPre = CAU[3];
			PWMdata->EPwm5CAUPre = CAU[4];
			PWMdata->EPwm6CAUPre = CAU[5];  
			
			PWMdata->ACQ_ZRO=ZRO[0]|(ZRO[1]<<2)|(ZRO[2]<<4)|(ZRO[3]<<6)|(ZRO[4]<<8)|(ZRO[5]<<10)&0xfff;
			PWMdata->ACQ_CAU=CAU[0]|(CAU[1]<<2)|(CAU[2]<<4)|(CAU[3]<<6)|(CAU[4]<<8)|(CAU[5]<<10)&0xfff;
		} // (PWMdata->IntFlag) == 1	
		else if ((PWMdata->IntFlag) == 1) /* IntFlag = 0,ZRO处中断*/  /* 20160706 */
		{
			PWMdata->ZROPreTest = 0;
			PWMdata->CAUPreTest = 0;
			for(g_i = 0; g_i<6; g_i++)
			{
				switch(g_i)
				{
					case 0:
						VFGama6[g_i] = VFGama - 4.712388980385;
						break;	
					case 1:	
						VFGama6[g_i] = VFGama - 1.570796326795;
						break;		
					case 2:
						VFGama6[g_i] = VFGama  - 0.523598775598;	
						break;	
					case 3:
						VFGama6[g_i] = VFGama  - 3.665191429188;
						break;	
					case 4:
						VFGama6[g_i] = VFGama  -  2.617993877991;
						break;	
					case 5:
						VFGama6[g_i] = VFGama  -  5.759586531581;
						break;	 	 	 		
				}			

				if(VFGama6[g_i]<0)	
					VFGama6[g_i] = VFGama6[g_i] + 6.283185307179;

				PWMdata->gama_Num_pre[g_i] = PWMdata->gama_Num[g_i];	
				PWMdata->gama_Num[g_i] = 0;
			  
				/* 计算 gama_Num[g_i] */
				if(VFGama6[g_i]>=pi)
					PWMdata->gama_Num[g_i] = PWMdata->Gama_TotalNum;
				else
				{
					if((VFGama6[g_i]<Halfpi))
					{		
						while(   (VFGama6[g_i] > *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + PWMdata->gama_Num[g_i]) ) &&(PWMdata->gama_Num[g_i]<PWMdata->HalfGama_TotalNum)     )
						{	PWMdata->gama_Num[g_i]++;	}
					}
					else
					{
						VFGama6_g_iC = pi - VFGama6[g_i];
						while(   (VFGama6_g_iC > *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + PWMdata->gama_Num[g_i]) ) &&(PWMdata->gama_Num[g_i]<PWMdata->HalfGama_TotalNum)     )
						{	PWMdata->gama_Num[g_i]++;	}
						PWMdata->gama_Num[g_i] = PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i]; 	     	      	
					}	
				}
				
				/* 计算 gama_Num_pre[g_i] */
				if(PWMdata->Gama_TotalNum_Pre != PWMdata->Gama_TotalNum)
				{
					/* svmshetemp */								    
					if(PWMdata->Gama_TotalNum_Pre == 0) 
						VFGama_Pre6[g_i] =  VFGama6[g_i] - (8.726646e-4)*PWMdata->vvvf1;/* VFGama6[g_i] - (1/7.2k)*2*pi*vvvf1 */
					if(VFGama_Pre6[g_i]<0)	
						VFGama_Pre6[g_i] = VFGama_Pre6[g_i] + 6.283185307179;
						PWMdata->gama_Num_pre[g_i] = 0;
					if(VFGama_Pre6[g_i]>=pi)
						PWMdata->gama_Num_pre[g_i] = PWMdata->Gama_TotalNum;
					else
					{
						if((VFGama_Pre6[g_i]<Halfpi))
						{		
							while(   (VFGama_Pre6[g_i] > *(TbStartPointer + ( (ma_index - 1) * (PWMdata->HalfGama_TotalNum) ) + PWMdata->gama_Num_pre[g_i]) ) &&(PWMdata->gama_Num_pre[g_i]<PWMdata->HalfGama_TotalNum)     )
							{	PWMdata->gama_Num_pre[g_i]++;	}
						}
						else
						{
							VFGama_Pre6_g_iC = pi - VFGama_Pre6[g_i];
							while(   (VFGama_Pre6_g_iC > *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + PWMdata->gama_Num_pre[g_i]) ) &&(PWMdata->gama_Num_pre[g_i]<PWMdata->HalfGama_TotalNum)     )
							{	PWMdata->gama_Num_pre[g_i]++;	}
							PWMdata->gama_Num_pre[g_i] = PWMdata->Gama_TotalNum - PWMdata->gama_Num_pre[g_i]; 	     	      	
						}	
					}
				}		

				
				if( (g_i%2)==0 )
				{		
					if((PWMdata->gama_Num_pre[g_i]) == (PWMdata->gama_Num[g_i]))
					{
						if( ((PWMdata->gama_Num[g_i])%2) == 0)
						{
							SHERoute[g_i] = 31;  // Err
							PRD[g_i] = AQ_CLEAR;	  
							CAD[g_i] = AQ_NO_ACTION;	
						}
						else
						{
							SHERoute[g_i] = 32;  // Err
							PRD[g_i] = AQ_SET; 			
							CAD[g_i] = AQ_NO_ACTION;	
						}	
					}
					else
					{
						if(  (fabs(VFGama6[g_i] - VFGama_Pre6[g_i])>pi)&&((VFGama) != VFGamaStart)    )
						{
							SHERoute[g_i] = 33;
							PRD[g_i] = AQ_CLEAR;	  
							CAD[g_i] = AQ_NO_ACTION;          	
						}				              
						else if(fabs((PWMdata->gama_Num[g_i] - PWMdata->gama_Num_pre[g_i])) == 2)
						{
							if( ((PWMdata->gama_Num[g_i])%2) ==0)
							{
								SHERoute[g_i] = 34;
								PRD[g_i] = AQ_CLEAR;	  
								CAD[g_i] = AQ_NO_ACTION; 	
							}
							else
							{
								SHERoute[g_i] = 35;
								PRD[g_i] = AQ_SET; 			
								CAD[g_i] = AQ_NO_ACTION;					              		
							}
						}	
						else
						{      	  
							if(VFGama6[g_i]>=VFGama_Pre6[g_i])   	        	  		            	  
							{
								if((VFGama6[g_i]<Halfpi)) { SHERoute[g_i] = 36;  // Err
								deltGamaSHE = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)) - VFGama_Pre6[g_i]; }
								else
								{
									if(PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum)  {SHERoute[g_i] = 37;
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1) - 1);}
									else	{SHERoute[g_i] = 38;			// Err								      		    	
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1));}

									if( PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum) 
										deltGamaSHE = GamaTb - VFGama_Pre6[g_i];												      			 
									else
										deltGamaSHE = (pi - GamaTb) - VFGama_Pre6[g_i]; /* 5-->6  6-->7  7-->8etc */
								}
							}				 
							else
							{
								if((VFGama6[g_i]>Halfpi))
								{  SHERoute[g_i] = 39;
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - (PWMdata->gama_Num[g_i]+1) + 1)  -1));/*ATENTION!*/
									deltGamaSHE = (-1)*(pi - VFGama_Pre6[g_i]) + GamaTb;												           		 												          		
								}	          	  			            	  
								else
								{
									if(PWMdata->gama_Num[g_i] == (PWMdata->HalfGama_TotalNum)) {  SHERoute[g_i] = 40;
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1-1);  }
									else {  SHERoute[g_i] = 41;
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1); }

									if(PWMdata->gama_Num[g_i] == (PWMdata->HalfGama_TotalNum))	/* 6-->5 */
										deltGamaSHE = VFGama_Pre6[g_i] - (pi-GamaTb);
									else	/* 5-->4  4-->3  3-->2etc */
										deltGamaSHE = VFGama_Pre6[g_i] - GamaTb;
								}													           				
							}
							
							CMPA[g_i] = (1 - deltGamaSHE/TsGama) * PWMdata->T1PRHalf;	/* 更新CMPA */
							
							if( ((PWMdata->gama_Num[g_i])%2) == 0)
							{
								PRD[g_i] = AQ_SET;	 
								CAD[g_i] = AQ_TOGGLE;     	    	
							}	
							else
							{
								PRD[g_i] = AQ_CLEAR;	
								CAD[g_i] = AQ_TOGGLE;     		
							}				 
						}
					}		
				}	/* end of (g_i%2)==0 */
				else
				{
					if((PWMdata->gama_Num_pre[g_i]) == (PWMdata->gama_Num[g_i]))
					{
						if( ((PWMdata->gama_Num[g_i])%2) == 0)
						{  SHERoute[g_i] = 42;
							PRD[g_i] = AQ_SET;	  
							CAD[g_i] = AQ_NO_ACTION;	
						}
						else
						{  SHERoute[g_i] = 43;
							PRD[g_i] = AQ_CLEAR;		
							CAD[g_i] = AQ_NO_ACTION;	
						}	
					}
					else
					{				            	
						if(  (fabs(VFGama6[g_i] - VFGama_Pre6[g_i])>pi)&&((VFGama) != VFGamaStart)    )
						{
							SHERoute[g_i] = 44;
							PRD[g_i] = AQ_SET;	  
							CAD[g_i] = AQ_NO_ACTION;          	
						}
						else if(fabs((PWMdata->gama_Num[g_i]) - (PWMdata->gama_Num_pre[g_i])) == 2)
						{
							if( ((PWMdata->gama_Num[g_i])%2)==0)
							{
								SHERoute[g_i] = 45;
								PRD[g_i] = AQ_SET;	  
								CAD[g_i] = AQ_NO_ACTION; 	
							}
							else
							{
								SHERoute[g_i] = 46;
								PRD[g_i] = AQ_CLEAR; 			
								CAD[g_i] = AQ_NO_ACTION;					              		
							}		              
						}		
						else
						{      	  
							if(VFGama6[g_i]>=VFGama_Pre6[g_i])   	        	  		            	  
							{
								if((VFGama6[g_i]<Halfpi)) {  SHERoute[g_i] = 47;  // Err
								deltGamaSHE = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)) - VFGama_Pre6[g_i];}
								else
								{
									if(PWMdata->gama_Num[g_i] == PWMdata->HalfGama_TotalNum) { SHERoute[g_i] = 48;
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1) - 1);}
									else	{ SHERoute[g_i] = 49;	  // Err
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - PWMdata->gama_Num[g_i] + 1)  -1)); }
									
									if( (PWMdata->gama_Num[g_i]) == (PWMdata->HalfGama_TotalNum)) /* 4-->5only */
										deltGamaSHE = GamaTb - VFGama_Pre6[g_i];												      			 
									else
										deltGamaSHE = (pi - GamaTb) - VFGama_Pre6[g_i];		
								}
					
							}				 
							else
							{ 		           				    	  
											 
								if((VFGama6[g_i]>Halfpi))
								{  SHERoute[g_i] = 50;
									GamaTb =  *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + ((PWMdata->Gama_TotalNum - (PWMdata->gama_Num[g_i]+1) + 1)  -1));
									deltGamaSHE = (-1)*(pi - VFGama_Pre6[g_i]) + GamaTb;												           		 												          		
								}	          	  			            	  
							   else	/* 0-90 */
							   {
									if((PWMdata->gama_Num[g_i]) == (PWMdata->HalfGama_TotalNum))  { SHERoute[g_i] = 51;
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1-1);}
									else {SHERoute[g_i] = 52;
									GamaTb = *(TbStartPointer + ((ma_index - 1) * (PWMdata->HalfGama_TotalNum)) + (PWMdata->gama_Num[g_i]-1)+1);}
									
									if((PWMdata->gama_Num[g_i]) == (PWMdata->HalfGama_TotalNum))/* 6-->5 */
										deltGamaSHE = VFGama_Pre6[g_i] - (pi-GamaTb);
									else	
										deltGamaSHE = VFGama_Pre6[g_i] - GamaTb;
								}													           				
							}

							CMPA[g_i] = (1 - deltGamaSHE/TsGama) * PWMdata->T1PRHalf;			            	  
							
							if( ((PWMdata->gama_Num[g_i])%2) == 0)
							{
								PRD[g_i] = AQ_CLEAR;	 
								CAD[g_i] = AQ_TOGGLE;     	    	
							}	
							else
							{
								PRD[g_i] = AQ_SET;	
								CAD[g_i] = AQ_TOGGLE;     		
							}
						}
					}		
				}	/* end of (g_i%2)!=0 */
				
				VFGama_Pre6[g_i] = VFGama6[g_i];/*保留老角度*/
				
				if(CMPA[0]>PWMdata->T1PRHalf) CMPA[0] = PWMdata->T1PRHalf-20;  // 20160727
				if(CMPA[1]>PWMdata->T1PRHalf) CMPA[1] = PWMdata->T1PRHalf-20;
				if(CMPA[2]>PWMdata->T1PRHalf) CMPA[2] = PWMdata->T1PRHalf-20;
				if(CMPA[3]>PWMdata->T1PRHalf) CMPA[3] = PWMdata->T1PRHalf-20;
				if(CMPA[4]>PWMdata->T1PRHalf) CMPA[4] = PWMdata->T1PRHalf-20;
				if(CMPA[5]>PWMdata->T1PRHalf) CMPA[5] = PWMdata->T1PRHalf-20;
				
 				MinPulseSetDes(PWMdata);	  /* 20150422 */  
				                      /* 20150603, 大致看了一下，此函数有最小脉宽处理功能，且是SHE2SVM切换所必须 */
			} 
			
			PWMdata->EPwm1PRDPre = PRD[0];	/* 保持动作的SHEPWM和最小脉宽用到 */
			PWMdata->EPwm2PRDPre = PRD[1];
			PWMdata->EPwm3PRDPre = PRD[2];
			PWMdata->EPwm4PRDPre = PRD[3];
			PWMdata->EPwm5PRDPre = PRD[4];
			PWMdata->EPwm6PRDPre = PRD[5];

			PWMdata->EPwm1CADPre = CAD[0];
			PWMdata->EPwm2CADPre = CAD[1];
			PWMdata->EPwm3CADPre = CAD[2];
			PWMdata->EPwm4CADPre = CAD[3];
			PWMdata->EPwm5CADPre = CAD[4];
			PWMdata->EPwm6CADPre = CAD[5];
			
			PWMdata->ACQ_PRD=PRD[0]|(PRD[1]<<2)|(PRD[2]<<4)|(PRD[3]<<6)|(PRD[4]<<8)|(PRD[5]<<10)&0xfff;
			PWMdata->ACQ_CAD=CAD[0]|(CAD[1]<<2)|(CAD[2]<<4)|(CAD[3]<<6)|(CAD[4]<<8)|(CAD[5]<<10)&0xfff;
		}  // (PWMdata->IntFlag) == 1

		PWMdata->VFGama_Pre = VFGama;/*保留老角度*/
		
		//SHEMinPulseCheck(PWMdata); //2018.4.47add
		
		/*正常的SHEPWM末尾对EPwmEndState清零*/
		PWMdata->EPwmEndState=0;
	}  /* Normal SHEPWM ends */
	else	/* SHEShutFlag!=0,保持动作的SHEPWM */
	{
		if ( (PWMdata->IntFlag) == 0 )	/* PRD处中断 */   /* 20160706 */
		{
			/* epwm1上次末尾的状态 */
            if((PWMdata->EPwmEndState)==0)	/* 没有赋值过EPwmEndState；正常的SHEPWM末尾对EPwmEndState清零 */
            {	
				if(	( ((PWMdata->EPwm1PRDPre)==AQ_CLEAR)&&((PWMdata->EPwm1CADPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm1PRDPre)==AQ_SET)&&((PWMdata->EPwm1CADPre)==AQ_TOGGLE) )	)	
					PWMdata->EPwm1EndState=AQ_CLEAR;	/* 最后的电平为低 */
				else 
					PWMdata->EPwm1EndState=AQ_SET;	/* 最后的电平为高 */	

				if(	( ((PWMdata->EPwm2PRDPre)==AQ_CLEAR)&&((PWMdata->EPwm2CADPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm2PRDPre)==AQ_SET)&&((PWMdata->EPwm2CADPre)==AQ_TOGGLE) )	)						
					PWMdata->EPwm2EndState=AQ_CLEAR;	/* 最后的电平为低 */
				else 
					PWMdata->EPwm2EndState=AQ_SET; /* 最后的电平为高 */	

				if(	( ((PWMdata->EPwm3PRDPre)==AQ_CLEAR)&&((PWMdata->EPwm3CADPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm3PRDPre)==AQ_SET)&&((PWMdata->EPwm3CADPre)==AQ_TOGGLE) ))							
					PWMdata->EPwm3EndState=AQ_CLEAR;	/* 最后的电平为低 */
				else 
					PWMdata->EPwm3EndState=AQ_SET;	/* 最后的电平为高 */	

				if(	( ((PWMdata->EPwm4PRDPre)==AQ_CLEAR)&&((PWMdata->EPwm4CADPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm4PRDPre)==AQ_SET)&&((PWMdata->EPwm4CADPre)==AQ_TOGGLE) ))							
					PWMdata->EPwm4EndState=AQ_CLEAR;	/* 最后的电平为低 */
				else 
					PWMdata->EPwm4EndState=AQ_SET; /* 最后的电平为高 */	

				if(	( ((PWMdata->EPwm5PRDPre)==AQ_CLEAR)&&((PWMdata->EPwm5CADPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm5PRDPre)==AQ_SET)&&((PWMdata->EPwm5CADPre)==AQ_TOGGLE) ))							
					PWMdata->EPwm5EndState=AQ_CLEAR; /* 最后的电平为低 */
				else 
					PWMdata->EPwm5EndState=AQ_SET;	/* 最后的电平为高 */	

				if(	( ((PWMdata->EPwm6PRDPre)==AQ_CLEAR)&&((PWMdata->EPwm6CADPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm6PRDPre)==AQ_SET)&&((PWMdata->EPwm6CADPre)==AQ_TOGGLE) ))	
					PWMdata->EPwm6EndState=AQ_CLEAR;	/* 最后的电平为低 */
				else 
					PWMdata->EPwm6EndState=AQ_SET; /* 最后的电平为高 */	
	
				PWMdata->EPwmEndState = 1;
            }
			else	/* EPwmEndState!=0 */
			{   
				/* EPwm1EndState～EPwm6EndState  的状态不更改 */
			}              
			ZRO[0] = PWMdata->EPwm1EndState;  
			ZRO[1] = PWMdata->EPwm2EndState; 
			ZRO[2] = PWMdata->EPwm3EndState;
			ZRO[3] = PWMdata->EPwm4EndState;  
			ZRO[4] = PWMdata->EPwm5EndState;  
			ZRO[5] = PWMdata->EPwm6EndState;
            CAU[0] = AQ_NO_ACTION;  
			CAU[1] = AQ_NO_ACTION;  
			CAU[2] = AQ_NO_ACTION;  
			CAU[3] = AQ_NO_ACTION;  
			CAU[4] = AQ_NO_ACTION;  
			CAU[5] = AQ_NO_ACTION;  
			
			PWMdata->ACQ_ZRO=ZRO[0]|(ZRO[1]<<2)|(ZRO[2]<<4)|(ZRO[3]<<6)|(ZRO[4]<<8)|(ZRO[5]<<10)&0xfff;
			PWMdata->ACQ_CAU=CAU[0]|(CAU[1]<<2)|(CAU[2]<<4)|(CAU[3]<<6)|(CAU[4]<<8)|(CAU[5]<<10)&0xfff;
			/* 因为NOACTION,所以无需CMPR值 */          
		}	/* end of IntFlag == ET_CTR_PRD */
		else if((PWMdata->IntFlag) == 1) /*ZRO处中断*/
		{
			/* epwm1上次末尾的状态 */
            if((PWMdata->EPwmEndState)==0)	/* 没有赋值过EPwmEndState */
            {
				if(	( ((PWMdata->EPwm1ZROPre)==AQ_CLEAR)&&((PWMdata->EPwm1CAUPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm1ZROPre)==AQ_SET)&&((PWMdata->EPwm1CAUPre)==AQ_TOGGLE) )	)
					PWMdata->EPwm1EndState=AQ_CLEAR;/* 最后的电平为低 */
				else 
					PWMdata->EPwm1EndState=AQ_SET;/* 最后的电平为高	*/
					
				if(	( ((PWMdata->EPwm2ZROPre)==AQ_CLEAR)&&((PWMdata->EPwm2CAUPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm2ZROPre)==AQ_SET)&&((PWMdata->EPwm2CAUPre)==AQ_TOGGLE) )	)
					PWMdata->EPwm2EndState=AQ_CLEAR;/* 最后的电平为低 */
				else 
					PWMdata->EPwm2EndState=AQ_SET;/* 最后的电平为高	*/

				if(	( ((PWMdata->EPwm3ZROPre)==AQ_CLEAR)&&((PWMdata->EPwm3CAUPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm3ZROPre)==AQ_SET)&&((PWMdata->EPwm3CAUPre)==AQ_TOGGLE) )	)
					PWMdata->EPwm3EndState=AQ_CLEAR;/* 最后的电平为低 */
				else 
					PWMdata->EPwm3EndState=AQ_SET;/* 最后的电平为高	*/
					
				if(	( ((PWMdata->EPwm4ZROPre)==AQ_CLEAR)&&((PWMdata->EPwm4CAUPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm4ZROPre)==AQ_SET)&&((PWMdata->EPwm4CAUPre)==AQ_TOGGLE) )	)
					PWMdata->EPwm4EndState=AQ_CLEAR;/* 最后的电平为低 */
				else 
					PWMdata->EPwm4EndState=AQ_SET;/* 最后的电平为高	*/
					
				if(	( ((PWMdata->EPwm5ZROPre)==AQ_CLEAR)&&((PWMdata->EPwm5CAUPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm5ZROPre)==AQ_SET)&&((PWMdata->EPwm5CAUPre)==AQ_TOGGLE) )	)
					PWMdata->EPwm5EndState=AQ_CLEAR;/* 最后的电平为低 */
				else 
					PWMdata->EPwm5EndState=AQ_SET;/* 最后的电平为高	*/
					
				if(	( ((PWMdata->EPwm6ZROPre)==AQ_CLEAR)&&((PWMdata->EPwm6CAUPre)==AQ_NO_ACTION) )||( ((PWMdata->EPwm6ZROPre)==AQ_SET)&&((PWMdata->EPwm6CAUPre)==AQ_TOGGLE) )	)
					PWMdata->EPwm6EndState=AQ_CLEAR;/* 最后的电平为低 */
				else 
					PWMdata->EPwm6EndState=AQ_SET;/* 最后的电平为高	*/
				
				PWMdata->EPwmEndState = 1;
            }
            else
            {   
				/*EPwm1EndState～EPwm6EndState  的状态不更改。*/
            }              

			PRD[0] = PWMdata->EPwm1EndState;
			PRD[1] = PWMdata->EPwm2EndState;
			PRD[2] = PWMdata->EPwm3EndState; 
			PRD[3] = PWMdata->EPwm4EndState;
			PRD[4] = PWMdata->EPwm5EndState;
			PRD[5] = PWMdata->EPwm6EndState;				
		
			CAD[0] = AQ_NO_ACTION; 
			CAD[1] = AQ_NO_ACTION; 
			CAD[2] = AQ_NO_ACTION; 
			CAD[3] = AQ_NO_ACTION; 
			CAD[4] = AQ_NO_ACTION; 
			CAD[5] = AQ_NO_ACTION; 
			
			PWMdata->ACQ_PRD=PRD[0]|(PRD[1]<<2)|(PRD[2]<<4)|(PRD[3]<<6)|(PRD[4]<<8)|(PRD[5]<<10)&0xfff;
			PWMdata->ACQ_CAD=CAD[0]|(CAD[1]<<2)|(CAD[2]<<4)|(CAD[3]<<6)|(CAD[4]<<8)|(CAD[5]<<10)&0xfff;
		}
			CMPASignDetection = 0;  // Sign of CMPA
			if(CMPA[0] < 0) CMPASignDetection = CMPASignDetection + 1;
			if(CMPA[1] < 0) CMPASignDetection = CMPASignDetection + 2;
			if(CMPA[2] < 0) CMPASignDetection = CMPASignDetection + 4;
			if(CMPA[3] < 0) CMPASignDetection = CMPASignDetection + 8;
			if(CMPA[4] < 0) CMPASignDetection = CMPASignDetection + 16;
			if(CMPA[5] < 0) CMPASignDetection = CMPASignDetection + 32;
			CMPASignDetection = CMPASignDetection & 0x003f;
	}	  
	
/*	MinPulseOperation(); */ /* 20150603 */


	PWMdata->SHEReady = 0;	/* svm2sheminp */  
		for(i=0;i<6;i++)
		{
		 if(/*CAU[i]!=AQ_NO_ACTION && */ CMPA[i]<20)
		 {
		   CMPA[i]=20;
		 }
		  if(/*CAD[i]!=AQ_NO_ACTION &&*/ CMPA[i]<20)
		 {
		   CMPA[i]=20;
		 }
			PWMdata->VFGama_Pre6[i]=VFGama_Pre6[i];
		}

		PWMdata->CMPA[0] = CMPA[0];
		PWMdata->CMPA[1] = CMPA[1];
		PWMdata->CMPA[2] = CMPA[2];
		PWMdata->CMPA[3] = CMPA[3];
		PWMdata->CMPA[4] = CMPA[4];
		PWMdata->CMPA[5] = CMPA[5];
}	/* end of SHEPWM()	*/  


