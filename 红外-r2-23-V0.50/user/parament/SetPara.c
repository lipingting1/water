#include "app_cfg.h"
extern uint8_t	chInterFaceState,Para_Pos,chParaNum;
extern uint8_t ParaBuffer[37],DisBuffer[12];
extern code uint8_t YposLim[4];	  
extern uint8_t Yper,Xper,Xpos,Ypos,chPage;
extern bit bSavPar;
extern Sys_Para SystemPara;
extern unsigned char code Para[] ;
extern uint16_t RdacValue;

void ParSetUpdPage()		   				//参数修改界面刷新 		0x42
{
	chInterFaceState=0x41;
	DisBuffer[0]=ParaBuffer[Para_Pos];
	DisBuffer[1]=0;
	LCD_Display_String(Xpos,Ypos,ParaBuffer,ADDPART1,NORMAL,NORMAL);
	LCD_Display_String((Xpos+Xper), (Ypos+Yper),DisBuffer,ADDPART1,REVERSE,REVERSE); 
}

void ParaPage(uint8_t chMode)							//参数修改页面  
{
	Para_Pos=0;
	Yper=0;
	Xper=0;
	if(0 == (SystemPara.MeaState&(1<<6)))Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
    switch(chMode){
	case 1:	LCD_Display_String(48,60,"液",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(64,112,"体积:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(136,112,"ml",ADDPART1,NORMAL,NORMAL);
			if(9999<SystemPara.Volume) Read_EEprom(System_ram,(uint8_t *)&SystemPara.BKG_ALL,54,14); //  999	 999
			chParaNum=sprintf(ParaBuffer,"%04d",SystemPara.Volume);
			LCD_Display_String(104,112,ParaBuffer,ADDPART1,NORMAL,NORMAL); 
			Xpos=104;Ypos=112;
			break;				   //液体
	case 2: LCD_Display_String(48,60,"固",ADDPART1,NORMAL,NORMAL);				 //固体
			LCD_Display_String(64,112,"质量:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(64,132,"体积:",ADDPART1,NORMAL,NORMAL); 
			LCD_Display_String(136,112,"g",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(136,132,"ml",ADDPART1,NORMAL,NORMAL);
			if(9999<SystemPara.Quality) Read_EEprom(System_ram,(uint8_t *)&SystemPara.BKG_ALL,54,14);
			if(9999<SystemPara.Volume)	Read_EEprom(System_ram,(uint8_t *)&SystemPara.Volume,68,2);
			chParaNum=sprintf(ParaBuffer,"%04d%c%04d",SystemPara.Quality,0x0d,SystemPara.Volume);
			LCD_Display_String(104,112,ParaBuffer,ADDPART1,NORMAL,NORMAL); 
			Xpos=104;Ypos=112; 
			break;					 
	case 3: LCD_Display_String(48,60,"测 量",ADDPART1,NORMAL,NORMAL);	 
			LCD_Display_String(48,96,"测量时间:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(48,116,"测量精度:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(48,136,"报警阈值:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(172,96,"分钟",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(164,116,"%",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(180,136,"Bq/L",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(180,156,"Bq/Kg",ADDPART1,NORMAL,NORMAL);
			if(9999<SystemPara.MTime)	Read_EEprom(System_ram,(uint8_t *)&SystemPara.MTime,70,2);
			if(99<SystemPara.MeasureAcc)Read_EEprom(System_ram,(uint8_t *)&SystemPara.MeasureAcc,72,1);
			if(9999<SystemPara.Alarm_V) Read_EEprom(System_ram,(uint8_t *)&SystemPara.Alarm_V,74,2);
			if(9999<SystemPara.Alarm_Q) Read_EEprom(System_ram,(uint8_t *)&SystemPara.Alarm_Q,76,2);
			chParaNum=sprintf(ParaBuffer,"%04d%c%02d%c%04d%c%04d",
				(uint16_t)(SystemPara.MTime),0x0d,
				(uint16_t)SystemPara.MeasureAcc,0x0d,
				(uint16_t)SystemPara.Alarm_V,0x0d,
				(uint16_t)SystemPara.Alarm_Q);
			LCD_Display_String(132,96,ParaBuffer,ADDPART1,NORMAL,NORMAL);
			Xpos=132;Ypos=96; 
			break;
	case 4: LCD_Display_String(48,60 ,"系 统",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(56,96 ,"Cs-137:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(56,116," I-131:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(56,136,"时  间: ",ADDPART1,NORMAL,NORMAL);	 //
			LCD_Display_String(56,176,"    HV: ",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(56,196,"   AMP: ",ADDPART1,NORMAL,NORMAL);
			if((1.0<SystemPara.CalibFactor_Cs)|(0>SystemPara.CalibFactor_Cs))Read_EEprom(System_ram,(uint8_t *)&SystemPara.CalibFactor_Cs,78,4);
			if((1.0<SystemPara.CalibFactor_I) |(0>SystemPara.CalibFactor_I))Read_EEprom(System_ram,(uint8_t *)&SystemPara.CalibFactor_I,82,4);
			if((900<SystemPara.hwHvPara)|(300>SystemPara.hwHvPara))Read_EEprom(System_ram,(uint8_t *)&SystemPara.hwHvPara,92,2);
			if(9999<SystemPara.hwAmpPara)Read_EEprom(System_ram,(uint8_t *)&SystemPara.hwAmpPara,94,2);
			chParaNum=sprintf(ParaBuffer,"%.2f%c%.2f%c%02x/%02x/%02x%c%02x:%02x%c%04d%c%04d",
								SystemPara.CalibFactor_Cs,0x0d,
								SystemPara.CalibFactor_I,0x0d,
								(uint16_t) SystemPara.SysData[0],
								(uint16_t)(SystemPara.SysData[1]&0x1f),
								(uint16_t)(SystemPara.SysData[2]&0x3f),0x0d,
								(uint16_t)(SystemPara.SysTime[0]&0x3f),
								(uint16_t)(SystemPara.SysTime[1]&0x7f),0x0d,	  //小时：钟
								(uint16_t)SystemPara.hwHvPara,0x0d,
								(uint16_t)SystemPara.hwAmpPara);	 
			LCD_Display_String(132,96,ParaBuffer,ADDPART1,NORMAL,NORMAL);
   			Xpos=132;Ypos=96; 
			break;				   //系统参数
	default:break;
	}
	Show_String(276,40 ,"+",ADDPART1,NORMAL);
	Show_String(276,80 ,"#",ADDPART1,NORMAL);
	Show_String(276,120,"-",ADDPART1,NORMAL);
	LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(264,160,"确认",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame (0,32,240,184,ADDPART1,NORMAL);
	ParSetUpdPage();
}

void ParPagUpd(uint8_t chKey,uint8_t chMode)			//参数修改操做处理      0x41		KEY		  chSetParMode
{
	uint8_t i,j,k1,k2,chSetNum,chSetNumShad;
	uint8_t *piPtrShad;
	uint16_t *piPtr,SetValueBuf; 
	extern uint16_t RdacValue;
	switch(chKey){
	case 1: ParaBuffer[Para_Pos]++;		//上+				//液体 //固体	//测量参数	//系统参数
			if('9' < ParaBuffer[Para_Pos])ParaBuffer[Para_Pos]='0';
			ParSetUpdPage();
			break;
	case 3: ParaBuffer[Para_Pos]--;									
			if('0' > ParaBuffer[Para_Pos])ParaBuffer[Para_Pos]='9';
			ParSetUpdPage();
			break;	//下-
	case 2: Para_Pos++;	  	//右移
			Xper+=8; 
			if(Para_Pos >= chParaNum){
				Para_Pos=0;
				Xper=0;
				Yper=0;
			}
			if(ParaBuffer[Para_Pos]==0x0d | ParaBuffer[Para_Pos]=='/' | ParaBuffer[Para_Pos]=='.' | ParaBuffer[Para_Pos]==':'){
				if(ParaBuffer[Para_Pos]==0x0d){
					Yper+=20;
					if(Yper >= YposLim[chMode-1]) Yper=0;
					Xper=0;
				}else Xper+=8;
				Para_Pos++;
			}
			ParSetUpdPage();
			break;
	case 4: chSetNum=0;
			switch(chMode){							  //确认
			case 1:										 //液体
			case 2: if(0==(SystemPara.MeaState&(1<<4))){						 //固体
						StartMes();
						if(2==chMode){(uint16_t *)piPtr=&SystemPara.Volume;}
						else{(uint16_t *)piPtr=&SystemPara.Volume;}
						for(i=0,j=0;i<chParaNum;i++,j++){
							if(ParaBuffer[i] == 0x0d){
								sscanf(DisBuffer,"%04d",(uint16_t *)piPtr);
								(uint16_t *)piPtr+=1;
								j = 255;
								chSetNum++;
							}else DisBuffer[j]=ParaBuffer[i];	//sprintf(&DisBuffer[j],"%c",DisBuffer[i]);
						}
						sscanf(DisBuffer,"%04d",(uint16_t *)piPtr);
						chSetNum++;
//						if(chMode != chSetNum){
////							Write_EEprom(System_ram,(uint8_t *)&Para[4],70,8);
//							Read_EEprom(System_ram,(uint8_t *)&SystemPara.Quality,66,8);
//						}
						Write_EEprom(System_ram,(uint8_t *)&SystemPara.Volume,16,4);
						MeasureProPage(Get_MeaMode(),1);	
					}else MeasureBusyPage();
					break;
			case 3:			  //测量参数	   //系统参数
			case 4: if(3 == chMode){
						(uint16_t *)piPtr=&SystemPara.MTime;
					}else{
						(float *)piPtr=&SystemPara.CalibFactor_Cs;
					}
					piPtrShad=(uint8_t *)piPtr;
					for(i=0,j=0;i<chParaNum;i++,j++){
						if((ParaBuffer[i] == 0x0d) | (ParaBuffer[i] == '/') | (ParaBuffer[i] == ':')){
							if((&SystemPara.CalibFactor_I == (float *)piPtr) | (&SystemPara.CalibFactor_Cs == (float *)piPtr)){	 //浮点型
								for(;j < 8;j++)DisBuffer[j]=0;
								sscanf(DisBuffer,"%f",(float *)piPtr); //
								((float *)piPtr)++;
							}else{
								if(		(&SystemPara.MTime    == (uint16_t *)piPtr)
									|| 	(&SystemPara.Alarm_V  == (uint16_t *)piPtr)
//									||	(&SystemPara.Alarm_Q  == (uint16_t *)piPtr) 
									||	(&SystemPara.hwHvPara == (uint16_t *)piPtr)
								  )
								{ //整形
									for(;j < 8;j++)DisBuffer[j]=0;
									sscanf(DisBuffer,"%04d",(uint16_t *)piPtr);
									((uint16_t *)piPtr)++;
								}else{//字符型
									for(;j < 8;j++)DisBuffer[j]=0;
									if(3 == chMode)sscanf(DisBuffer,"%d",&SetValueBuf);
									else{
										sscanf(DisBuffer,"%x",&SetValueBuf);
									}
									*(uint8_t *)piPtr=(uint8_t)SetValueBuf;
									if((&SystemPara.SysTime[1] == (uint8_t *)piPtr) || (&SystemPara.MeasureAcc == (uint8_t *)piPtr))((uint8_t *)piPtr)++;

										((uint8_t *)piPtr)++;		
								}
							}
							j=255;
							chSetNum++;
						}else{
							DisBuffer[j]=ParaBuffer[i];	///sprintf(&DisBuffer[j],"%c",DisBuffer[i]);
						}
					}
					chSetNum++;
					sscanf(DisBuffer,"%04d",(uint16_t *)piPtr);
					if(3==chMode){
						k1=20;k2=8;chSetNumShad=4;
					}else{	
						if(SystemPara.SysData[1]>0x11) SystemPara.SysData[1]=0x12;
						if(SystemPara.SysData[2]>0x30) SystemPara.SysData[2]=0x31;

						if(SystemPara.SysTime[0]>0x22) SystemPara.SysTime[0]=0x23;
						if(SystemPara.SysTime[1]>0x58) SystemPara.SysTime[1]=0x59;

						RTC_Init(&SystemPara.SysTime[1],6);

						RdacValue=(uint16_t)((1252.7-SystemPara.hwHvPara)/1.2095);	  //y = -12.095x + 1252.7  (1252.7-B65)/1.2095

						DACCommand(CMD_WRITE_TO_RDAC, &RdacValue);
						DacAmpCommand(CMD_WRITE_TO_RDAC, &SystemPara.hwAmpPara);
						k1=28;k2=18;chSetNumShad=9;	
					}
					Write_EEprom(System_ram,(uint8_t *)piPtrShad,k1,k2);

					LCD_Display_String(164,192,"修改成功!",ADDPART1,NORMAL,NORMAL);
					ParaPage(chMode);
					bSavPar=1;
					break;
			}break;		
	case 5: switch(chMode){								 	//返回
			case 1:
			case 2:if(0==SystemPara.MeaState&(1<<6));//chMesMode=Free;
					 else{
					 	if((SystemPara.MeaState&(1<<4)))MeaModePaste();
					 }
					Mea_Free();
					InterfaceInit();break;
			case 3:
			case 4:MainMenu (4);break;
			}break;
	default:break;
	}
}




void StartPara(void)
{
	chInterFaceState=0x10; //  参数设置操作页面
	if(SystemPara.MeaState&(1<<6)){
		if(4!=Get_MeaMode()){
		Mea_Busy();
		MeaModeCopy();
		}	 
	}
	Mes_Cail(M);
	Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
	if(0==SystemPara.MeaState&(1<<6)) Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
	Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);
	Chinese_String(248,80,"测量参数",ADDPART1,NORMAL);
	Chinese_String(248,120,"系统参数",ADDPART1,NORMAL);
	if((SystemPara.MeaState&(1<<6))&&(0==(SystemPara.MeaState&(1<<4))))Chinese_String(260,160,"测量中",ADDPART1,NORMAL);
	else Chinese_String(248,160,"仪器校准",ADDPART1,NORMAL);
	LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(48,60,"参 数 设 置 选 择",ADDPART1,NORMAL,NORMAL);
	Chinese_String(16,112,"按下右侧按键进入相",ADDPART1,NORMAL);
	Chinese_String(16,136,"应参数调整页面",ADDPART1,NORMAL); 
}

