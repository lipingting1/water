#include"app_cfg.h"

extern uint8_t chInterFaceState,ParaBuffer[32],DisBuffer[12],chSetParMode;
extern bit bMeaResuSta,bConMea,chSec_Fla;
extern uint32_t MesCP_Cs,MesCP_I,MesCP_ALL,hwMeaCPMVal_Cs,hwMeaCPMVal_I;
extern Sys_Para SystemPara;
float fMeaVal;

uint8_t code Reason[3][20]={
							{"时间过短!"},
						    {"本底过大!"},
						    {"校准测量次数过少!"},		  
						   };

uint8_t code Measure[8][10]= {
						 {"XXXXX C"},
						 {"XXXXX CPM"},
						 {"XXX Bq/L"},
						 {"XXX Bq/L"},
						 {"XXX Bq/Kg"},
						 {"XXX Bq/Kg"},
						 {"XXX CPM"},
						 {"XXX CPM"},				
						};
uint8_t code Kind[4][6]={
						{"本 底"},
						{"液 体"},
						{"固 体"},
						{"校 准"},
						};

void StartMes(void)
{
	Init_T4;
	Init_T0;
	Init_T1;
	MesCP_Cs=0;
	MesCP_I=0;
	MesCP_ALL=0;
	SystemPara.MeasureTime[0]=0;
	SystemPara.MeasureTime[1]=0;
	SystemPara.MeasureTime[2]=0;

	Start_Measure();

}

void MeasureBusyPage(void)							 	//测量忙碌页面
{
	Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
	LCD_Display_String(80,60,"测 量 忙",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(60,128,"其他测量进行中!",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(264,120,"测量中",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame (0,32,240,184,ADDPART1,NORMAL);
	chInterFaceState=0x11;
}

void MeasureBusy(uint8_t chKey)							//测量忙碌处理  		0x11
{
	switch(chKey){
	case 1,2,4:break;
	case 3: if((SystemPara.MeaState&(1<<4)))MeaModePaste();		// 如果忙 调用测量种类备份 释放测量忙
			Mea_Free();
		    MainMenu (Get_MeaMode());break;
	case 5: MainMenu (Get_MeaMode());break;
	default:break;
	}
}

void MeaDetail(uint8_t chMode)
{
	chInterFaceState=0x43;
	Clear_Windows(0,80,320,120,ADDPART1,NORMAL);
	LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);
	
	LCD_Display_String(56,80 ,"CP:总:---",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(80,100 ,"Cs-137:---",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(80,120, "I-131:---",ADDPART1,NORMAL,NORMAL);

	LCD_Display_String(56,150,"CPM:总:---",ADDPART1,NORMAL,NORMAL);
	if(4 == chMode);
	else{
		LCD_Display_String(88,170,"Cs-137:---",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(88,190,"I-131:---",ADDPART1,NORMAL,NORMAL);
	}
}

void MeasureProPage(uint8_t chMode,bit bMeaState) 		//测量成功页面 和 测量中页面		   0x12
{
	bit bShoMeaStaF;														//chMode标志当前测量模式:1本地、2液体、3固体、4校准测中	bMeaState标识:0测量中、1测量成功
	if(1==bMeaState){
		TIM4_Open();				//开始运行
		chInterFaceState=0x14;	//页面处理函数
		bConMea=0;
		Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
		LCD_Display_String(108,60,"测 量 中",ADDPART1,NORMAL,NORMAL);	
		LCD_Display_String(264,120,"中止",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,80,"详情",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
		sprintf(DisBuffer,"BKG:%d%04d",(uint16_t)(SystemPara.BKG_ALL/10000),(uint16_t)(SystemPara.BKG_ALL%10000));
		LCD_Display_String(0,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
		bShoMeaStaF=1;
		switch(chMode){
		case 1: //LCD_Display_String(60,60,"本 底 ",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(32,220,"测量中",ADDPART1,NORMAL,NORMAL);
				break;
		case 2: //LCD_Display_String(60,60,"液 体 ",ADDPART1,NORMAL,NORMAL);
				sprintf(DisBuffer,"V:%dml",SystemPara.Volume);				
				LCD_Display_String(130,220,DisBuffer,ADDPART1,NORMAL,NORMAL);	
				break;
		case 3: //LCD_Display_String(60,60,"固 体 ",ADDPART1,NORMAL,NORMAL);   
				sprintf(ParaBuffer,"M:%04dg V:%dml",SystemPara.Quality,SystemPara.Volume);
				LCD_Display_String(96,220,ParaBuffer,ADDPART1,NORMAL,NORMAL);		  //下标  液体体积
				break;
		case 4://LCD_Display_String(60,60,"校 准 ",ADDPART1,NORMAL,NORMAL);
				break;
		default:bShoMeaStaF=0;break;
		}
		if(1 == bShoMeaStaF)LCD_Display_String(60,60,&Kind[chMode-1][0],ADDPART1,NORMAL,NORMAL);
		if(1==chMode){
			LCD_Display_String(84,116,&Measure[(chMode-1)<<1][0],ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(76,140,&Measure[((chMode-1)<<1)+1][0],ADDPART1,NORMAL,NORMAL);
		}else{	
			LCD_Display_String(116,104,&Measure[(chMode-1)<<1][0],ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(116,158,&Measure[((chMode-1)<<1)+1][0],ADDPART1,NORMAL,NORMAL);
	
			LCD_Display_String(36,86 ,"Cs-137:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(40,120,"精度:--%",ADDPART1,NORMAL,NORMAL);
			sprintf(ParaBuffer,"BKG:%d%04d",(uint16_t)(SystemPara.BKG_Cs/10000),(uint16_t)(SystemPara.BKG_Cs%10000));
			LCD_Display_String(120,120,ParaBuffer,ADDPART1,NORMAL,NORMAL);

			LCD_Display_String(44,144,"I-131:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(40,180,"精度:--%",ADDPART1,NORMAL,NORMAL);
			sprintf(ParaBuffer,"BKG:%d%04d",(uint16_t)(SystemPara.BKG_I/10000),(uint16_t)(SystemPara.BKG_I%10000));
			LCD_Display_String(120,180,ParaBuffer,ADDPART1,NORMAL,NORMAL);
		}
	}else{
		chInterFaceState=0x15;	//结果页面处理函数
		LCD_Display_String(156,60 ,"完 成",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,80,"详情",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,160,"保存",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,120,"重测",ADDPART1,NORMAL,NORMAL);
		if(1==chMode)Clear_Windows(0,220,80,16,ADDPART1,NORMAL);
//		if(4==chMode){
//			chCalinUum++;
//			fMeaVal+=hwMeaCPMVal_Cs;
//		}
	}
	LCD_Display_String(232,220,"T:",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);
}


void MeaProDeal(uint8_t chKey,uint8_t chMode)			//测量中处理			0x14
{
	uint8_t chReason;
	switch(chKey){
	case 2: MeaDetail(chMode);break;
	case 3:	Stop_Measure();				//中止
			if(SystemPara.MTime > (SystemPara.MeasureTime[0]*60+SystemPara.MeasureTime[1])){
				bMeaResuSta=0;
				chReason=0;
				chInterFaceState=0x15;
				Clear_Windows(0,80,320,120,ADDPART1,NORMAL);
				LCD_Display_String(80,144,&Reason[chReason][0],ADDPART1,NORMAL,NORMAL);	 //显示测量失败原因
				LCD_Display_String(96 ,96 ,"失 败 !",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(264,120,"重测",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(264,80,"详情",ADDPART1,NORMAL,NORMAL);
				if(1==chMode)LCD_Display_String(32,220,"测量失败!",ADDPART1,NORMAL,NORMAL);
				Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);break;	 //失败
			}MeasureProPage(Get_MeaMode(),(bit)(SystemPara.MeaState&(1<<6)));	//成功
			bMeaResuSta=1;
			break; //	判断时间是否到达临界时间 可能需要分情况讨论失败原因
	case 5: if(4==chMode | 0==(SystemPara.MeaState&(1<<6)))MainMenu(chMode);  /*需要考虑//////	 */
			else InterfaceInit();
			break;
	default:break;
	}
}

void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta)	//测量结束处理	0x15 bMeaResuSta
{
	switch(chKey){
	case 2: MeaDetail(chMode);break;
	case 3: StartMes();
			Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
		    MeasureProPage(Get_MeaMode(),1);break;	   //重测
	case 4: if(0==bMeaResuSta);
			else SaveValue(chMode);
			break;  //跳转到保存程序  保存		
	case 5: //chCalinUum=0;
			MainMenu(chMode);break;	   //返回
	default:break;
	}		
}
void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode)//测量详情查看界面  0x43
{
	switch(chKey){
	case 5: MeasureProPage(chMesMode,1);
			if((SystemPara.MeaState&(1<<6)));
			else {
				chInterFaceState=0x14;
				Data_Pro();
				MeaProDeal(3,Get_MeaMode());
			}
			break;
	default:break;
	}
}

void StartBacMes(void)
{
	if((0==(SystemPara.MeaState&(1<<6))) || ((SystemPara.MeaState&(1<<6)) && (1!=Get_MeaMode())) || (SystemPara.MeaState&(1<<4)))
	{
		chInterFaceState = 0x10;
		if(0 == (SystemPara.MeaState&(1<<4))){	 
			if((SystemPara.MeaState&(1<<6))&&(1!=Get_MeaMode())){
				Mea_Busy();
				MeaModeCopy();
			}
			Mes_Back(M);
		}
		Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
		if(0==(SystemPara.MeaState&(1<<6)))Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
		LCD_Display_String(72 ,60 ,"本 底 测 量",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(248,40 ,"开始测量",ADDPART1,NORMAL,NORMAL);
	    LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(8  ,112,"1 请将准备好的纯净水倒入容器",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(24 ,130,"内.",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(8  ,148,"2 按开始键启动本底测量.",ADDPART1,NORMAL,NORMAL);
		Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL); 
	}
	else MeasureProPage(Get_MeaMode(),1);
}

void StartLiqSolMes(uint8_t chKey)
{
	if(0==(SystemPara.MeaState&(1<<6))||((SystemPara.MeaState&(1<<6))&&(chKey!=Get_MeaMode())||(SystemPara.MeaState&(1<<4))))
	{
		if(0==(SystemPara.MeaState&(1<<4))){   //判断测量忙
			if((SystemPara.MeaState&(1<<6))&&(chKey!=Get_MeaMode())){
				Mea_Busy();
				MeaModeCopy();
			}
			if(2==chKey)Mes_Liq(M);
			else Mes_Solid(M);
			chSetParMode=chKey-1;
		}
		Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
		LCD_Display_String(70,60," 体 样 品 信 息",ADDPART1,NORMAL,NORMAL);	   
		ParaPage(chSetParMode);	
	} 
	else MeasureProPage(Get_MeaMode(),1);
}

void MeaValueUpd(void)
{
	uint16_t MeaSecTim;
//	uint32_t CP_Value;

	MeaSecTim = (SystemPara.MeasureTime[0])*3600+SystemPara.MeasureTime[1]*60+SystemPara.MeasureTime[2];	//测量秒数

//	CP_Value = MesCP_ALL/MeaSecTim;
//	SystemPara.hwMeaCPMVal_ALL =CP_Value*60;
//
//	CP_Value = MesCP_I*60;
//	hwMeaCPMVal_I =CP_Value/MeaSecTim;
//
//	CP_Value = MesCP_Cs*60;
//	hwMeaCPMVal_Cs =CP_Value/MeaSecTim;

	SystemPara.hwMeaCPMVal_ALL=MesCP_ALL*60/MeaSecTim;
	hwMeaCPMVal_I =MesCP_I*60/MeaSecTim;
	hwMeaCPMVal_Cs =MesCP_Cs*60/MeaSecTim;
}