/***********************************
 *主函数
 *2015/7/29
*/

/****************include************/
#include "app_cfg.h"
#include "lcd.h"
#include "EEPROM.h"
//#include "main.h"
#include "SetPara.h"
#include "MeaPro.h"
#include "HisRecord.h"
#include "I2C.h"
#include "DSPs.h"
#include "UART.h"

extern code uint8_t LOG[],Para[] ;	//	0x11111111, 120,10, 0,5,0,1000,1000, 
extern bit UART_flag;
code uint8_t YposLim[4]={10,30,70,110};
Sys_Para SystemPara = {	   				//		 0,0, 
	0x11111111,250,120,120, 100,100, 112,5,0,1000,1000,  0.98,0.98,{0x16,0x01,0x22},{0x13,0x15,0},769,0x1ff, {0}, 0,0,0, 0,0,0,0,  {0},1, 
};	//时，分 ，秒
HistoryData His={0};				//	50

//#define _DEBUG_
#ifdef _DEBUG_
	bit chSec_Fla1;
	uint16_t xdata hwMeasureTime;
	uint32_t wCpmAll,wCpmCs,wCpmI;
#endif

xdata uint8_t Para_Pos=0,DisBuffer[12],ParaBuffer[37],chParaNum;//chParaNum;
uint8_t Xper,Xpos,Yper=0,Ypos=0;

//时间
uint16_t chSec_Tim=0,iCount;
uint8_t AccKeyValue,KEY=0,chKeyTime=0,chBuzzerTim=0;  //按键、入口地址
bit bKeyFlag,bAvoidRepeat,chSec_Fla;	//bKeyFlag按键防抖标志位 bAvoidRepeat按键防重复标识位 chSec_Fla秒标记位  

//如果历史记录存储个数为0的标志位  如果为0则该位置1  否则置0
bit bDisDel;  
uint8_t CSR_Old,chPage=0;	//页面

//通讯
bit bShowMea=0,bConMea=0;	//上位机显示测量循环发送测量值标志位  //   bConMea上位机控制测量标识位 

static uint8_t HV_Num=0;

uint16_t  ControlRegisterValue,RdacValue;
//测量
uint32_t hwMeaCPMVal_Cs,hwMeaCPMVal_I;
uint32_t MesCP_Cs,MesCP_I,MesCP_ALL;
uint16_t xdata hwCpsAll,hwCpsCs,hwCpsI;

uint8_t chInterFaceState=0x00,chSetParMode,bSetSucFla=0;	//当前测量模式（1本底2液体3固体4校准）、当前测量模式副本、参数设置模式（1液体2固体3测量参数4系统参数）																				  
bit  bMeaResuSta,bSavPar=0; //bMeaResuSta 0测量失败 1测量成功 	  bSavPar 0参数未更改1参数修改过()  bSetSucFla 1清除“修改成功”0无动作  

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
	uint32_t RelativeValue;	  //SystemPara.chMeaValBuf
	uint8_t Size_CPM,Size_CP;//,CPS_Value_H;
	uint16_t add;
	chSec_Fla=0;
	Updata_Time(&SystemPara.SysTime[2],7);	 //读年月日   时分秒   仅读时分秒 日期更新   现在仅更新时间未更新日期
	if(1 == bSavPar)bSetSucFla++;
	if(2 == bSetSucFla){
		Clear_Windows(164,192,78,16,ADDPART1,NORMAL);
		bSavPar = 0;
		bSetSucFla = 0;
	}
	sprintf(DisBuffer,"20%02x/%02x/%02x",(uint16_t)SystemPara.SysData[0],(uint16_t)(SystemPara.SysData[1]&0x1f),(uint16_t)(SystemPara.SysData[2]&0x3f));
	LCD_Display_String(240,0,DisBuffer,ADDPART1,NORMAL,NORMAL);
	sprintf(DisBuffer,"%02x:%02x:%02x",(uint16_t)(SystemPara.SysTime[0]&0x3f),(uint16_t)SystemPara.SysTime[1],(uint16_t)SystemPara.SysTime[2]);
	LCD_Display_String(248,16,DisBuffer,ADDPART1,NORMAL,NORMAL);
//	chMunite++;
//	if(15 == chMunite){
//		chMunite=0;
//		HV_Num++;
//		RdacValue=HV_Num*10;
//		sprintf(DisBuffer,"%4d",(uint16_t)RdacValue);	//  /%d/%d/%d/
//		LCD_Display_String(80,220,DisBuffer,ADDPART1,NORMAL,NORMAL);

//		DACCommand(CMD_WRITE_TO_RDAC, &RdacValue);

//		if(0x03ff1 == RdacValue) HV_Num=0xff;
//	}  MesCP_Cs+= hwCpsCs;
	bMea_Cou=(bit)(SystemPara.MeaState&(1<<7));	// bFtat;
	if((SystemPara.MeaState&(1<<6))){
		MesCP_Cs +=hwCpsCs;
		MesCP_ALL +=hwCpsAll;
		MesCP_I +=hwCpsI;

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
		/*-----------DEBUG--------------*/
//		#ifdef DEBUG
		if(1 ==bMea_Cou){
			if(1 == bTestOveFla){
				MeasureProPage(Get_MeaMode(),0);
				MeaProDeal(3,Get_MeaMode());
				MeaResDeal(4,Get_MeaMode(),1);	//测量结束处理	0x15 bMeaResuSta
				TIM0_Close();
				TIM1_Close();
				TIM4_Close();
				Init_T0;
				Init_T1;
				Init_T4;

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
	if(0x14 == chInterFaceState | 0x43 == chInterFaceState){
		MeaValueUpd();
		if(0x14 == chInterFaceState){
			switch((Get_MeaMode())){
			case 1: Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_ALL/10000),(uint16_t)(SystemPara.hwMeaCPMVal_ALL%10000));		//显示本底CPM值
					LCD_Display_String((156-(Size_CPM<<4)),140,DisBuffer,ADDPART1,NORMAL,NORMAL);	   
					Size_CP=sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_ALL/10000),(uint16_t)(MesCP_ALL%10000)); 	//显示CP值
					LCD_Display_String((164-(Size_CP<<4)),116,DisBuffer,ADDPART1,NORMAL,NORMAL);
					break;

			case 2:	add=SystemPara.Volume;
			case 3:	if(3 == (Get_MeaMode()))add=SystemPara.Quality;
					if(hwMeaCPMVal_Cs <= SystemPara.BKG_ALL){ RelativeValue=0; SystemPara.iMeaBqVal_Cs=0;}  //Cs的Bq值
					else{
						RelativeValue = hwMeaCPMVal_Cs-SystemPara.BKG_Cs;	 
						SystemPara.iMeaBqVal_Cs = RelativeValue/add;		 ///*CalibFactor_Cs
					}
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_Cs/10000),(uint16_t)(SystemPara.iMeaBqVal_Cs%10000));
					LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);//显示CPM值

					if(hwMeaCPMVal_I <= SystemPara.BKG_I) {RelativeValue=0;SystemPara.iMeaBqVal_I=0;}	  //I的Bq值
					else{
						RelativeValue=hwMeaCPMVal_I-SystemPara.BKG_I;
						SystemPara.iMeaBqVal_I=RelativeValue/add;
					}
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_I/10000),(uint16_t)(SystemPara.iMeaBqVal_I%10000));
					LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
					break;

			case 4: Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(hwMeaCPMVal_Cs/10000),(uint16_t)(hwMeaCPMVal_Cs%10000));
					LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);//显示CPM值
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(hwMeaCPMVal_I/10000),(uint16_t)(hwMeaCPMVal_I%10000));
					LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
					break;

			default:break;	
			}
		}else{	//详情中更新CP和CPM值
			sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_ALL/10000),(uint16_t)(MesCP_ALL%10000));	  //显示CP值
			LCD_Display_String(104,80,DisBuffer,ADDPART1,NORMAL,NORMAL);
			sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs/10000),(uint16_t)(MesCP_Cs%10000));	  //显示CP值
			LCD_Display_String(136,100,DisBuffer,ADDPART1,NORMAL,NORMAL);
			sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_I/10000),(uint16_t)(MesCP_I%10000));	  //显示CP值
			LCD_Display_String(128,120,DisBuffer,ADDPART1,NORMAL,NORMAL);

			sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_ALL/10000),(uint16_t)(SystemPara.hwMeaCPMVal_ALL%10000));	  //显示CP值
			LCD_Display_String(112,150,DisBuffer,ADDPART1,NORMAL,NORMAL);
			if(Get_MeaMode() == 4);
			else{
				sprintf(DisBuffer,"%d%04d",(uint16_t)(hwMeaCPMVal_Cs/10000),(uint16_t)(hwMeaCPMVal_Cs%10000));
				LCD_Display_String(144,170,DisBuffer,ADDPART1,NORMAL,NORMAL);
				sprintf(DisBuffer,"%d%04d",(uint16_t)(hwMeaCPMVal_I/10000),(uint16_t)(hwMeaCPMVal_I%10000));
				LCD_Display_String(136,190,DisBuffer,ADDPART1,NORMAL,NORMAL);
			}
		}
	}
	if(1 == bShowMea)PackBag();		 //考虑中：在显示测量时是自动发送测量数据还是被动发送测量数据--------
}

void InitPCA(void)
{
	CR=0;

	CH=0;
	CL=0;
	CCAPM0|=0X49; //0X101001B
	iCount=MS;
	CCAP0L=(uint8_t)(iCount>>8);
	CCAP0H=(uint8_t)iCount;//定时50ms

	CR=1;
}
void InitInter(void)									//中断初始化函数
{
//初始化pac
	Stop_Measure();

	InitPCA();

	TIM0_Close();	//	I131		
	PT0 = 0;					
	TF0 = 0;					
	Timer0_16bit();
	AUXR |=  0x80;	//1T				
	TMOD |=  0x04;				
	INT_CLKO &= ~0x01;			
	Init_T0;
	TIM0_Open();

//初始化定时器1
	TIM1_Close();
	PT1 = 0;
	TF0 = 0;
	Timer1_16bit();
	AUXR |= 0x40;                   //定时器1为1T模式
	TMOD |= 0x40;     //设置定时器为模式0(16位自动重装载)
	INT_CLKO &= ~0x02;
    Init_T1;
	TIM1_Open();
						 
//初始化定时器4	  //T4-- Cs-137
	TIM4_Close();  
	T4T3M |= 0x20;                  //定时器4为1T模式
	Timer4_FucCou();
	Init_T4;
	TIM4_Open();		

//初始化串口1
	SCON = 0x50; 
	T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR |= 0x04;                //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
	AUXR |= 0X10;
    ES = 1;                     //使能串口1中断
	
	EA=1;
}



void main(void)										   	//主函数
{
	bit Flag=0;
	InitInter() ;

//	Write_EEprom(23,LOG,0,1890);	 //每页256 共8页		 8K byte可寻址
//	Write_EEprom(22,(uint8_t *)&SystemPara.MeachineAddr,0,50);	 //每页256 共8页		 8K byte可寻址
//	RTC_Init(&SystemPara.SysTime[1],6);

	Write_EEprom(22,(uint8_t *)&SystemPara.MeachineAddr,50,50);	 //每页256 共8页		 8K byte可寻址
	Read_EEprom(System_ram,(uint8_t *)&SystemPara.MeachineAddr,0,50);

	LCDSystem_Init();

	DACInitialization();
	ControlRegisterValue = ALLOW_UPDATE | NORMAL_MODE;
//
//	DACCommand(CMD_WRITE_TO_CONTROL_REG, &ControlRegisterValue);
//	RdacValue=(uint16_t)((1252.7-SystemPara.hwHvPara)/1.2095);
//	DACCommand(CMD_WRITE_TO_RDAC, &RdacValue);
//	DacAmpCommand(CMD_WRITE_TO_CONTROL_REG, &ControlRegisterValue);		 
//	DacAmpCommand(CMD_WRITE_TO_RDAC, &SystemPara.hwAmpPara);

	Show_Logo(48 ,80 ,ADDPART1,NORMAL);	 //显示字符串 
	SystemPara.MeaState=0;
	InterfaceInit();   //初始化。。。主界面显示	  SystemPara.History.Record[0][0]
	BUZZER=0;
	while(1)
	{
//		sprintf(DisBuffer,"/%d/%d",(uint16_t)chPage,(uint16_t)SystemPara.MTime);	//  /%d/%d/%d/
//		LCD_Display_String(120,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
		if(KEY)
			IntFacePro();

		if(1==chSec_Fla)
			Data_Pro();

		if(1 == UART_flag){
			UART_flag=0;
			chRecSuccess();
		}

		if(0 == Flag){
			if((SystemPara.MeaState&(1<<7))){
				LCD_Display_String(216,8,"C",ADDPART1,NORMAL,NORMAL);
				Flag=1;
			}
		}
#ifdef _DEBUG_
if((SystemPara.MeaState&(1<<6))){
	if (chSec_Fla1){
		Usart_Debug("SystemPara.hwAmpPara=%4d\r\n",SystemPara.hwAmpPara);
		chSec_Fla1 = 0;
		if(0==(SystemPara.MeaState&(1<<6))){
			hwMeasureTime = 0;
			wCpmAll = wCpmCs = wCpmI = 0;
		}else{
			hwMeasureTime++;
			wCpmAll += hwCpsAll;
			wCpmCs += hwCpsCs;
			wCpmI += hwCpsI;
		}
		Usart_Debug("hwMeasureTime=%4d\r\n",(uint16_t)hwMeasureTime);
		Usart_Debug("hwCpsAll=%4d,hwCpsCs=%4d,hwCpsI=%4d\r\n",(uint16_t)hwCpsAll,(uint16_t)hwCpsCs,(uint16_t)hwCpsI);
		if (hwMeasureTime){
			Usart_Debug("wCpmAll=%4d, wCpmCs=%4d, wCpmI=%4d\r\n",(uint16_t)(wCpmAll*60/hwMeasureTime),(uint16_t)(wCpmCs*60/hwMeasureTime),(uint16_t)(wCpmI*60/hwMeasureTime));
		}
	}
}
#endif
	}
}

void PCA_Int(void) interrupt 7						  	//PCA中断函数 CCP0和CP中断应用
{
	static s_KeyNum = 1;
	CF = 0;	//计数溢出中断
	if(1 == CCF0)	
	{		//模块0，按键定时扫描
		CCF0=0;
		iCount+=MS;
		CCAP0L = (uint8_t) iCount;
		CCAP0H = (uint8_t)(iCount >> 8);//定时20ms

		chKeyTime++;
		chSec_Tim++;
		chBuzzerTim++;


		if(50 <= chSec_Tim){	 //1s定时中断

			chSec_Fla=1;
#ifdef _DEBUG_
	chSec_Fla1=1;
#endif
			chSec_Tim=0;

	   	TIM4_Close();
		hwCpsCs = (T4H*256) + T4L;
		T4H=0;
		T4L=0;
		TIM4_Open();

		TIM1_Close();
		hwCpsAll = (TH1*256) + TL1;
		TH1=0;
		TL1=0;
		TIM1_Open();

		TIM0_Close();
		hwCpsI = (TH0 *256) + TL0;
		TH0=0;
		TL0=0;
		TIM0_Open();

		}					              

		if(5==chBuzzerTim){
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


