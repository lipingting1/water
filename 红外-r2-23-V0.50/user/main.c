/***********************************
 *主函数
 *2015/7/29
*/

/****************include************/
#include "app_cfg.h"
#include "lcd.h"
#include "EEPROM.h"
#include "main.h"
#include "SetPara.h"
#include "MeaPro.h"
#include "HisRecord.h"
#include "I2C.h"
#include "DSPs.h"
#include "UART.h"

Uint32_t MesCP_Cs={0},MesCP_I={0};
                              
Sys_Para SystemPara = {	   				//		 0,0, 
	0x11111111,0,5,0,1000,1000, 120,10,1200, 0.98,0.98,{0x16,0x01,0x22},{0x13,0x15,0},0x1ff,0x1ff, {0}, 0,0,0, 0,0,0,0,  {0},1, 
};	//时，分 ，秒	
HistoryData His={0};				  //	50
													 //年 ，月 日
bit bKeyFlag,bAvoidRepeat,bConMea=0,bHighMea=0;	//bKeyFlag按键防抖标志位  bAvoidRepeat按键防重复标识位 bConMea上位机控制测量标识位 
xdata uint8_t Para_Pos=0,DisBuffer[12],ParaBuffer[37],chParaNum;//chParaNum;
uint16_t iCount;
uint8_t Xper,Xpos,Yper=0,Ypos=0;
code uint8_t YposLim[4]={10,30,70,110};

uint8_t CSR_Old,chPage;	//页面
//时间
uint16_t chSec_Tim=0;
uint8_t chKeyTime=0,chBuzzerTim=0;
bit chSec_Fla,DatUpdFla=0;
bit bDisDel;  //如果历史记录存储个数为0的标志位  如果为0则该位置1  否则置0
uint16_t CPM_Value;
uint8_t AccKeyValue,KEY=0,chInterFaceState=0x00,chCalinUum,chSetParMode;	//按键、入口地址、当前测量模式（1本底2液体3固体4校准）、当前测量模式副本、参数设置模式（1液体2固体3测量参数4系统参数）																				  
bit  bMeaResuSta,bSavPar=0,bSetSucFla=0,bMesCalib; //	 bMeaResuSta 0测量失败 1测量成功 	  bSavPar 0参数未更改	1参数修改过  bSetSucFla 1清除“修改成功”0无动作  bMesCalib 0测量核素为Cs-137   1 I-137  

//通讯
bit bShowMea=0;	//上位机显示测量循环发送测量值标志位
uint8_t chMunite=0;
static uint8_t HV_Num=0;

bit gggflag=0;

uint8_t hwCurRAMxxx,morroAddr,xxxflag=0,sd[10]={0},jux=0;
uint16_t sdf=0;
void MeaModeCopy(void)
{
	if(SystemPara.MeaState&0x01)SystemPara.MeaState|=0x04;
	else SystemPara.MeaState&=0xfb;
	if(SystemPara.MeaState&0x02)SystemPara.MeaState|=0x08;
	else SystemPara.MeaState&=0xf7;
}
void MeaModePaste(void)
{
	if(SystemPara.MeaState&0x04)	SystemPara.MeaState|=0x01;
	else SystemPara.MeaState&=0xFE;
	if(SystemPara.MeaState&0x08)	SystemPara.MeaState|=0x02;
	else SystemPara.MeaState&=0xfD;
}

uint8_t	Get_KeyNum(void)
{
	uint8_t	chKeyNum;
	AccKeyValue=1;
	if(0==KEY1){
		chKeyNum=1;
		AccKeyValue*=chKeyNum;
	}
	if(0==KEY2){
		chKeyNum=2;
		AccKeyValue*=chKeyNum;
	}
	if(0==KEY3){
		chKeyNum=3;
		AccKeyValue*=chKeyNum;
	}
	if(0==KEY4){
		chKeyNum=4;
		AccKeyValue*=chKeyNum;
	}
	if(0==KEY5){
		chKeyNum=5;
		AccKeyValue*=chKeyNum;
	}
	return	chKeyNum;
}

void InterfaceInit(void)       							//主界面页面
{
	Mea_Free();
	chInterFaceState=0x00;
	chSec_Fla=1;
	if(0 == (SystemPara.MeaState&(1<<6)))Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
	Clear_Windows(0,0,320,216,ADDPART1,NORMAL);
	Show_Power(224,8,ADDPART1,BAT1,NORMAL);
	LCD_Display_String(16,8,"SMMU-R-一放射性活度检测仪",ADDPART1,NORMAL,NORMAL);
	if((SystemPara.MeaState&(1<<7)))LCD_Display_String(216,8,"C",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(72,60,"欢 迎 使 用",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(65,112,"便携式食品&水",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(57,128,"放射性活度检测仪",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,40,"本底测量",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,80,"液体测量",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,120,"固体测量",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,160,"参数设置",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,200,"历史记录",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame (0,32,240,184,ADDPART1,NORMAL);
}

void MainMenu(uint8_t chKey)  							//主界面处理      0x00
{
	switch(chKey){
	case 1:	StartBacMes();
			break;  //本底测量 
	case 2:		   
	case 3: StartLiqSolMes(chKey);
			break;	//固体测量
				  	//液体测量
	case 4:	StartPara();
			break;	//参数设置
	case 5:	His_Record();
			break; 	//历史记录
	default:break;							  
	}				
}

void PreMea(uint8_t chKey,uint8_t chMode)				//测量预处理	  0x10
{
	switch(chMode){
	case 1:switch(chKey){	//本底
			case 1: if((SystemPara.MeaState&(1<<4))) MeasureBusyPage();
				    else{
					    StartMes();
					    MeasureProPage(Get_MeaMode(),1); //开始测量
				    }
					break;
			case 2,3,4:break;
			case 5: if((SystemPara.MeaState&(1<<4))){	
						MeaModePaste();
						
					    Mea_Free();
					}
				    InterfaceInit();break;//返回
			default:break;
			}break;
	case 4:switch(chKey){						   						//校准
			case 1:break;
			case 2:
			case 3: Clear_Windows(0,32,320,184,ADDPART1,NORMAL);	  	//进入参数设置页面	分的入口方式为 3,4
					LCD_Display_String(96,60,"参 数 调 整",ADDPART1,NORMAL,NORMAL);
					chSetParMode=chKey+1;
					ParaPage(chSetParMode);break;
			case 4: if((SystemPara.MeaState&(1<<4)))	MeasureBusyPage();
					else{
					if(0 == (SystemPara.MeaState&(1<<6)))StartMes();
					MeasureProPage(Get_MeaMode(),1);
					}break;
			case 5: if(0 == (SystemPara.MeaState&(1<<6)));//chMesMode=Free;				  //返回
					else{
						if((SystemPara.MeaState&(1<<4))){
							MeaModePaste();
							Mea_Free();
						}
					}
					InterfaceInit();break;
		   }break;
	default:break;
	}
}

void IntFacePro(void)								   	//函数集
{
	switch(chInterFaceState)
	{
		case 0x00 :MainMenu  (KEY);													break;  //主函数	 		
		case 0x50 :HistoryPro(KEY);													break;  //历史页面翻动函数
		case 0x51 :HistoryDel(KEY,chPage,CSR_Old);									break;  //删除历史记录

		case 0x10 :PreMea(KEY,Get_MeaMode());										break;	 //测量预处理
		case 0x11 :MeasureBusy(KEY);												break;//测量页面和测量结果页面
		case 0x14 :MeaProDeal(KEY,Get_MeaMode());									break;
		case 0x15 :MeaResDeal(KEY,Get_MeaMode(),bMeaResuSta);						break;	

		case 0x41 :ParPagUpd(KEY,chSetParMode);										break;	  //参数修改   0x41		KEY		  chSetParMode
		case 0x43 :MeaDetailDeal(KEY,Get_MeaMode());								break;

		default:break;
	}KEY=0;
}

void Data_Pro(void)									   	//时间处理函数
{
	bit bOverFla=0,bTestOveFla=0,bDebFla=0,bMea_Cou;
	uint32_t CPS_Value,RelativeValue;	  //SystemPara.chMeaValBuf
	uint8_t Size_CPM,Size_CP;//,CPS_Value_H;
	extern uint16_t RdacValue;
//	chMunite++;
	if(15 == chMunite){
		chMunite=0;
		HV_Num++;
		RdacValue=HV_Num*10;
		sprintf(DisBuffer,"%4d",(uint16_t)RdacValue);	//  /%d/%d/%d/
		LCD_Display_String(80,220,DisBuffer,ADDPART1,NORMAL,NORMAL);

//		DACCommand(CMD_WRITE_TO_RDAC, &RdacValue);

		if(0x03ff1 == RdacValue) HV_Num=0xff;
	}
	bMea_Cou=(bit)(SystemPara.MeaState&(1<<7));	// bFtat;
	if((SystemPara.MeaState&(1<<6))){
		SystemPara.MeasureTime[2]++;
		if(60==SystemPara.MeasureTime[2]){
			SystemPara.MeasureTime[1]++;
			SystemPara.MeasureTime[2]=0;
			if(60==SystemPara.MeasureTime[1]){
				SystemPara.MeasureTime[0]++;
				SystemPara.MeasureTime[1]=0;
			}
		}
		sprintf(DisBuffer,"%02d:%02d:%02d",(uint16_t)SystemPara.MeasureTime[0],(uint16_t)SystemPara.MeasureTime[1],(uint16_t)SystemPara.MeasureTime[2]);
		LCD_Display_String(248,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
		/*-----------DEBUG--------------*///连续测量使能
		if(1 == bMea_Cou){
			if((SystemPara.MeaState&(1<<4))){
				if(2 == Get_MeaModeSha() | 3 == Get_MeaModeSha()) bDebFla=1;
			}else{
				if(2 == Get_MeaMode() | 3 == Get_MeaMode()) bDebFla=1;
			}
			if(((SystemPara.MeasureTime[0]*60 + SystemPara.MeasureTime[1]) >= SystemPara.MTime & 1 == bDebFla)){
				if((SystemPara.MeaState&(1<<4)))MeaModePaste();
				Mea_Free();
				bTestOveFla=1;
				if(0x14 == chInterFaceState);
				else MeasureProPage(Get_MeaMode(),1);
			}
		}
		/*-------------END--------------*/
		if(((SystemPara.MeasureTime[0]*60 + SystemPara.MeasureTime[1]) >= MesTimMax)){	 //测量时间上限自动停止
			if((SystemPara.MeaState&(1<<4)))MeaModePaste();
			Mea_Free();
			bOverFla=1;
			if(0x14 == chInterFaceState);
			else MeasureProPage(Get_MeaMode(),1);
		}
//		if(0x14 == chInterFaceState | 0x43 == chInterFaceState){
//			MeaValueUpd();
//			CPS_Value=SystemPara.hwMeaCPMVal_Cs/60;
//
//			if(0x14 == chInterFaceState){
//				if((4 == Get_MeaMode()) | (1 == Get_MeaMode())){
//					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_Cs/10000),(uint16_t)(SystemPara.hwMeaCPMVal_Cs%10000));  
//				}else{
//					if(SystemPara.hwMeaCPMVal_Cs <= SystemPara.BKG)RelativeValue=0;
//					else RelativeValue=SystemPara.hwMeaCPMVal_Cs-SystemPara.BKG;
//					if(2 == Get_MeaMode())SystemPara.iMeaBqVal_Cs=RelativeValue/SystemPara.Volume;
//					if(3 == Get_MeaMode())SystemPara.iMeaBqVal_Cs=RelativeValue/SystemPara.Quality;
//					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_Cs/10000),(uint16_t)(SystemPara.iMeaBqVal_Cs%10000));
//				}
//				switch(Get_MeaMode()){
//				case 1: LCD_Display_String((156-(Size_CPM<<4)),140,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //显示CPM值
//						Size_CP=sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000)); //显示CP值
//						LCD_Display_String((164-(Size_CP<<4)),116,DisBuffer,ADDPART1,NORMAL,NORMAL);
//						sprintf(DisBuffer,"%05dcps",(uint16_t)CPS_Value);//显示CPS值
//						LCD_Display_String(96,180,DisBuffer,ADDPART1,NORMAL,NORMAL);break;
//				case 2:	
//				case 3:	
//				case 4: if(0==bMesCalib) LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //显示CPM值
//						else LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
//						break;
//				default:break;	
//				}
//			}else{	//详情中更新CP和CPM值
//				sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000));
//				LCD_Display_String(104,80,DisBuffer,ADDPART1,NORMAL,NORMAL);
//				if(Get_MeaMode() == 4);
//				else{
//					sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_Cs/10000),(uint16_t)(SystemPara.iMeaBqVal_Cs%10000));
//					LCD_Display_String(144,170,DisBuffer,ADDPART1,NORMAL,NORMAL);
//				}
//			}
//		}
		/*-----------DEBUG--------------*/
//		#ifdef DEBUG
		if(1 ==bMea_Cou){
			if(1 == bTestOveFla){
				MeasureProPage(Get_MeaMode(),0);
				MeaProDeal(3,Get_MeaMode());
				MeaResDeal(4,Get_MeaMode(),1);	//测量结束处理	0x15 bMeaResuSta
				TIM0_Close();
				TH0=0;
				TL0=0;
				StartMes();
				MeasureProPage(Get_MeaMode(),(bit)(SystemPara.MeaState&(1<<6)));
			}
		}
//		#endif
//		/*------------END---------------*/
		if(1 == bOverFla){
			MeasureProPage(Get_MeaMode(),0);
			MeaProDeal(3,Get_MeaMode());
		}
	}
//处理系统时间
	if(1 == bShowMea)PackBag();		 //考虑中：在显示测量时是自动发送测量数据还是被动发送测量数据--------
	chSec_Fla=0;
	Updata_Time(&SystemPara.SysTime[2],7);	 //读年月日   时分秒   仅读时分秒 日期更新   现在仅更新时间未更新日期
	sprintf(DisBuffer,"20%02x/%02x/%02x",(uint16_t)SystemPara.SysData[0],(uint16_t)(SystemPara.SysData[1]&0x1f),(uint16_t)(SystemPara.SysData[2]&0x3f));
	LCD_Display_String(240,0,DisBuffer,ADDPART1,NORMAL,NORMAL);
	sprintf(DisBuffer,"%02x:%02x:%02x",(uint16_t)(SystemPara.SysTime[0]&0x3f),(uint16_t)SystemPara.SysTime[1],(uint16_t)SystemPara.SysTime[2]);
	LCD_Display_String(248,16,DisBuffer,ADDPART1,NORMAL,NORMAL);
		if(0x14 == chInterFaceState | 0x43 == chInterFaceState){
			MeaValueUpd();
			CPS_Value=SystemPara.hwMeaCPMVal_Cs/60;

			if(0x14 == chInterFaceState){
				if((4 == Get_MeaMode()) | (1 == Get_MeaMode())){
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_Cs/10000),(uint16_t)(SystemPara.hwMeaCPMVal_Cs%10000));  
				}else{
					if(SystemPara.hwMeaCPMVal_Cs <= SystemPara.BKG)RelativeValue=0;
					else RelativeValue=SystemPara.hwMeaCPMVal_Cs-SystemPara.BKG;
					if((2 == Get_MeaMode()))SystemPara.iMeaBqVal_Cs=RelativeValue/SystemPara.Volume;
					if((3 == Get_MeaMode()))SystemPara.iMeaBqVal_Cs=RelativeValue/SystemPara.Quality;
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_Cs/10000),(uint16_t)(SystemPara.iMeaBqVal_Cs%10000));
				}
				switch((Get_MeaMode())){
				case 1: LCD_Display_String((156-(Size_CPM<<4)),140,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //显示CPM值
						Size_CP=sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000)); //显示CP值
						LCD_Display_String((164-(Size_CP<<4)),116,DisBuffer,ADDPART1,NORMAL,NORMAL);
						sprintf(DisBuffer,"%05dcps",(uint16_t)CPS_Value);//显示CPS值
						LCD_Display_String(96,180,DisBuffer,ADDPART1,NORMAL,NORMAL);break;
				case 2:	
				case 3:	
				case 4: if(0==bMesCalib) LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //显示CPM值
						else LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
						break;
				default:break;	
				}
			}else{	//详情中更新CP和CPM值
				sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000));
				LCD_Display_String(104,80,DisBuffer,ADDPART1,NORMAL,NORMAL);
				if(Get_MeaMode() == 4);
				else{
					sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_Cs/10000),(uint16_t)(SystemPara.hwMeaCPMVal_Cs%10000));
					LCD_Display_String(144,170,DisBuffer,ADDPART1,NORMAL,NORMAL);
				}
			}
		}
}

void InitInter(void)									//中断初始化函数
{
//初始化pac
	Stop_Measure();
	CH=0;
	CL=0;
	CCAPM0|=0X49; //0X101001B
	iCount=MS;
	CCAP0L=(uint8_t)(iCount>>8);
	CCAP0H=(uint8_t)iCount;//定时50ms
	CR=1;
//初始化定时器0
//	Timer0_Stop();				
//	PT0 = 0;					
//	TF0 = 0;					
//	Timer0_InterruptEnable();	
//	Timer0_16bit();				
//	TMOD |=  0x04;				
//	INT_CLKO &= ~0x01;			
//	TH0 = 0;
//	TL0 = 0;
	Timer0_Stop();				
	PT0 = 0;					
	TF0 = 0;					
	Timer0_InterruptEnable();	
	Timer0_16bit();				
	TMOD |=  0x04;				
	INT_CLKO &= ~0x01;			
	TH0 = 0;
	TL0 = 0;

//初始化定时器1
//	AUXR |= 0x40;                   //定时器1为1T模式
//  AUXR &= 0xdf;                   //定时器1为12T模式

	Timer1_Stop();				
    TMOD = (TMOD&0x0f)|0x40;     //设置定时器为模式0(16位自动重装载)
    TL1 = 0;                     //初始化计时值
    TH1 = 0;
    TR1 = 1;                        //定时器1开始计时
    ET1 = 1;                        //使能定时器0中断

//初始化定时器3
	Timer3_Stop(); //T3--   I-131
	Timer3_InterruptEnable();
	Timer3_FucCou();
	T3H= 0;
	T3L= 0;
//初始化定时器4
/*
//	Timer4_Stop(); //T3--  Cs-131
//	Timer4_InterruptEnable();
//	Timer4_FucCou();
//	T4H= 0;
//	T4L= 0;			
*/
//初始化串口1
	SCON  = 0x50;		// 8位数据,可变波特率
	AUXR |= 0x40;		// 定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		// 串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		// 设定定时器1为16位自动重装方式

	TL1 = 0x8F;		//设定定时初值
	TH1 = 0xFD;		//设定定时初值

	ET1 = 0;			// 禁止定时器1中断
	TR1 = 1;			// 启动定时器1	

	ES=1;
//初始化SPI
//	SPCTL|=1001<<4;
//	SPCTL&=0xdf;
//	IE2|=0x02;

//    SPDAT = 0;                  //初始化SPI数据
//    SPSTAT = 0;       //清除SPI状态位
//	IE2|=0x02;
//初始化ADC
//	CLK_DIV|=1<<5;	 //ADC=(ADC_RES<<8)|ADC_RESL
//	P1ASF=0XC0;
//	EADC=1;
	
	EA=1;
}

//void Data_Init(void)
//{
//	uint8_t chJudgeSum=0,i=4;
////	Read_EEprom(System_ram,&SystemPara.History.Record[0][0],0,25);
////	while(i--)chJudgeSum+=SystemPara.History.Record[0][i];
////	if(0 == chJudgeSum){Write_EEprom(System_ram,(uint8_t *)&SystemPara.MeachineAddr,0,38);BUZZER=0;}
////	else 
//	Read_EEprom(System_ram,(uint8_t *)&SystemPara.MeachineAddr,0,38);
//}
void main(void)										   	//主函数
{
	bit Flag=0;
	extern code uint8_t LOG[];
	uint16_t  ControlRegisterValue,RdacValue;
//	Write_EEprom(23,LOG,0,1890);	 //每页256 共8页		 8K byte可寻址
//	RTC_Init(&SystemPara.SysTime[1],6);
	InitInter() ;

	Read_EEprom(System_ram,(uint8_t *)&SystemPara.MeachineAddr,0,38);
//	Data_Init();
	LCDSystem_Init();
	DACInitialization();
	BUZZER=0;
	ControlRegisterValue = ALLOW_UPDATE | NORMAL_MODE;
	DACCommand(CMD_WRITE_TO_CONTROL_REG, &ControlRegisterValue);
	RdacValue=0x197;
	DACCommand(CMD_WRITE_TO_RDAC, &RdacValue);
//	RdacValue=0x0000;
//	DACCommand(CMD_READ_FROM_RDAC, &RdacValue);
//	SystemPara.hwHvPara = RdacValue;

//	SPDAT=12;
//	Write_RDAC(1);
//	Show_Logo(48 ,80 ,ADDPART1,NORMAL);	 //显示字符串 
	InterfaceInit();   //初始化。。。主界面显示	  SystemPara.History.Record[0][0]
//	TR0=1;
	while(1)
	{
//		sprintf(DisBuffer,"/%d/%d/%d",(uint16_t)chMunite,(uint16_t)HV_Num,(uint16_t)RdacValue);	//  /%d/%d/%d/
//		LCD_Display_String(120,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
		if(KEY)	IntFacePro();
		if(1==chSec_Fla){
	//			MesCP_Cs.Num[2] = TH0;
	//		MesCP_Cs.Num[3] = TL0;
	//		sprintf(DisBuffer,"%02d:%02d",(uint16_t)MesCP_Cs.Num[2],(uint16_t)MesCP_Cs.Num[3]);
	//		LCD_Display_String(140,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
			Data_Pro();
			sprintf(DisBuffer,"%d:%02d/%02d",(uint16_t)chInterFaceState,(uint16_t)MesCP_Cs.Num[3],(uint16_t)(Get_MeaMode()));
			LCD_Display_String(140,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
		}
		if(0==(SystemPara.MeaState&(1<<6))){
			TIM0_Close();
			TH0=0;
			TL0=0;
		}
		if(1 == bSetSucFla){
			Clear_Windows(164,192,78,16,ADDPART1,NORMAL);
			bSetSucFla = 0;
		}
		if(0 == Flag){
			if((SystemPara.MeaState&(1<<7))){
				LCD_Display_String(216,8,"C",ADDPART1,NORMAL,NORMAL);
				Flag=1;
			}
		}

//		if(1 == xxxflag){
//			xxxflag=0;
//			Read_EEprom(hwCurRAMxxx,&SystemPara.History.Record[0][0],morroAddr,25);
//			sprintf(ParaBuffer,"%2d %02x/%02x/%02x         %02x:%02x",(uint16_t)(16),(uint16_t)SystemPara.History.Record[0][0],(uint16_t)SystemPara.History.Record[0][1],(uint16_t)SystemPara.History.Record[0][2],(uint16_t)SystemPara.History.Record[0][3],(uint16_t)SystemPara.History.Record[0][4]);
//			LCD_Display_String(16,80,ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址	
//		}
//		sprintf(ParaBuffer,"%2d %02d/%02d/%02d         %02d:%02d",(uint16_t)(sd[0]),(uint16_t)sd[1],(uint16_t)sd[2],(uint16_t)sd[3],(uint16_t)sd[4],(uint16_t)sd[5]);
//		LCD_Display_String(0,0,ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址	
	}
}

void Time0(void) interrupt 1						   	//定时/计数器0  计数模式 中断函数
{
	MesCP_Cs.Num[1]++;
	if(MesCP_Cs.Num[1] >= 255){
		MesCP_Cs.Num[0]++;
		MesCP_Cs.Num[1]=0;
	}
}

void Time3(void) interrupt 19						   	//定时/计数器3  计数模式 中断函数
{
	MesCP_I.Num[1]++;
	if(MesCP_I.Num[1] >= 255){
		MesCP_I.Num[0]++;
		MesCP_I.Num[1]=0;
	}
}

//						if(1 == KEY)  LED1=0;
//						if(2 == KEY)  LED2=0;
//						if(3 == KEY)  LED3=0;
void PCA_Int(void) interrupt 7						  	//PCA中断函数 CCP0和CP中断应用
{
	static s_KeyNum = 1;
	CF = 0;	//计数溢出中断
	if(1 == CCF0)	
	{		//模块0，按键定时扫描
		CCF0=0;
		iCount+=MS;
		CCAP0L = (uint8_t) iCount;
		CCAP0H = (uint8_t)(iCount >> 8);//定时10ms

		chKeyTime++;
		chSec_Tim++;
		chBuzzerTim++;

		if(100 <= chSec_Tim){	 //1s定时中断
			chSec_Fla=1;
			chSec_Tim=0;
			if(1 == bSavPar){
				bSetSucFla=1;
				bSavPar=0;
			}
		}					              

		if(5==chBuzzerTim){// BUZZER=1;
			BUZZER=1;
			LED1=1;
			LED2=1;
			LED3=1;
		}

		if(2==chKeyTime)  //20ms
		{
			chKeyTime=0;
			if(0==(KEY1&KEY2&KEY3&KEY4&KEY5)){
				if(1==bKeyFlag){		//可以用us=1  us<<1 因为左移8八次后就复位了
					s_KeyNum <<=1;		//RLC 带进位的左移 MOV DPTR,#0X2192  MOVC ACC,@DPTR  RLC ACC
					if((1==bAvoidRepeat)){
						KEY=Get_KeyNum();
						BUZZER=0;
						chBuzzerTim=0;
						if(6 == AccKeyValue){Continue_Measure(); KEY=0;}
						bAvoidRepeat=0;
					}
				}else{
					bKeyFlag=1;
//					s_KeyNum = 1;
				}
			}else{
				bAvoidRepeat=1;
				bKeyFlag=0;
			}
		}
	}
}


