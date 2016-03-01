/***********************************
 *������
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
};	//ʱ���� ����
HistoryData His={0};				//	50

//#define _DEBUG_
#ifdef _DEBUG_
	bit chSec_Fla1;
	uint16_t xdata hwMeasureTime;
	uint32_t wCpmAll,wCpmCs,wCpmI;
#endif

xdata uint8_t Para_Pos=0,DisBuffer[12],ParaBuffer[37],chParaNum;//chParaNum;
uint8_t Xper,Xpos,Yper=0,Ypos=0;

//ʱ��
uint16_t chSec_Tim=0,iCount;
uint8_t AccKeyValue,KEY=0,chKeyTime=0,chBuzzerTim=0;  //��������ڵ�ַ
bit bKeyFlag,bAvoidRepeat,chSec_Fla;	//bKeyFlag����������־λ bAvoidRepeat�������ظ���ʶλ chSec_Fla����λ  

//�����ʷ��¼�洢����Ϊ0�ı�־λ  ���Ϊ0���λ��1  ������0
bit bDisDel;  
uint8_t CSR_Old,chPage=0;	//ҳ��

//ͨѶ
bit bShowMea=0,bConMea=0;	//��λ����ʾ����ѭ�����Ͳ���ֵ��־λ  //   bConMea��λ�����Ʋ�����ʶλ 

static uint8_t HV_Num=0;

uint16_t  ControlRegisterValue,RdacValue;
//����
uint32_t hwMeaCPMVal_Cs,hwMeaCPMVal_I;
uint32_t MesCP_Cs,MesCP_I,MesCP_ALL;
uint16_t xdata hwCpsAll,hwCpsCs,hwCpsI;

uint8_t chInterFaceState=0x00,chSetParMode,bSetSucFla=0;	//��ǰ����ģʽ��1����2Һ��3����4У׼������ǰ����ģʽ��������������ģʽ��1Һ��2����3��������4ϵͳ������																				  
bit  bMeaResuSta,bSavPar=0; //bMeaResuSta 0����ʧ�� 1�����ɹ� 	  bSavPar 0����δ����1�����޸Ĺ�()  bSetSucFla 1������޸ĳɹ���0�޶���  

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

void InterfaceInit(void)       							//������ҳ��
{
	Mea_Free();
	chInterFaceState=0x00;
	if(0 == (SystemPara.MeaState&(1<<6)))Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
	Clear_Windows(0,0,320,216,ADDPART1,NORMAL);
	Show_Power(224,8,ADDPART1,BAT1,NORMAL);
	LCD_Display_String(16,8,"SMMU-R-һ�����Ի�ȼ����",ADDPART1,NORMAL,NORMAL);
	if((SystemPara.MeaState&(1<<7)))LCD_Display_String(216,8,"C",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(72,60,"�� ӭ ʹ ��",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(65,112,"��ЯʽʳƷ&ˮ",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(57,128,"�����Ի�ȼ����",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,40,"���ײ���",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,80,"Һ�����",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,120,"�������",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,160,"��������",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(248,200,"��ʷ��¼",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame (0,32,240,184,ADDPART1,NORMAL);
}

void MainMenu(uint8_t chKey)  							//�����洦��      0x00
{
	switch(chKey){
	case 1:	StartBacMes();
			break;  //���ײ��� 
	case 2:		   
	case 3: StartLiqSolMes(chKey);
			break;	//�������
				  	//Һ�����
	case 4:	StartPara();
			break;	//��������
	case 5:	His_Record();
			break; 	//��ʷ��¼
	default:break;							  
	}				
}

void PreMea(uint8_t chKey,uint8_t chMode)				//����Ԥ����	  0x10
{
	switch(chMode){
	case 1:switch(chKey){	//����
			case 1: if((SystemPara.MeaState&(1<<4))) MeasureBusyPage();
				    else{
					    StartMes();
					    MeasureProPage(Get_MeaMode(),1); //��ʼ����
				    }
					break;
			case 2,3,4:break;
			case 5: if((SystemPara.MeaState&(1<<4))){	
						MeaModePaste();
						
					    Mea_Free();
					}
				    InterfaceInit();break;//����
			default:break;
			}break;
	case 4:switch(chKey){						   						//У׼
			case 1:break;
			case 2:
			case 3: Clear_Windows(0,32,320,184,ADDPART1,NORMAL);	  	//�����������ҳ��	�ֵ���ڷ�ʽΪ 3,4
					LCD_Display_String(96,60,"�� �� �� ��",ADDPART1,NORMAL,NORMAL);
					chSetParMode=chKey+1;
					ParaPage(chSetParMode);break;
			case 4: if((SystemPara.MeaState&(1<<4)))	MeasureBusyPage();
					else{
					if(0 == (SystemPara.MeaState&(1<<6)))StartMes();
					MeasureProPage(Get_MeaMode(),1);
					}break;
			case 5: if(0 == (SystemPara.MeaState&(1<<6)));//chMesMode=Free;				  //����
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

void IntFacePro(void)								   	//������
{
	switch(chInterFaceState)
	{
		case 0x00 :MainMenu  (KEY);													break;  //������	 		
		case 0x50 :HistoryPro(KEY);													break;  //��ʷҳ�淭������
		case 0x51 :HistoryDel(KEY,chPage,CSR_Old);									break;  //ɾ����ʷ��¼

		case 0x10 :PreMea(KEY,Get_MeaMode());										break;	 //����Ԥ����
		case 0x11 :MeasureBusy(KEY);												break;//����ҳ��Ͳ������ҳ��
		case 0x14 :MeaProDeal(KEY,Get_MeaMode());									break;
		case 0x15 :MeaResDeal(KEY,Get_MeaMode(),bMeaResuSta);						break;	

		case 0x41 :ParPagUpd(KEY,chSetParMode);										break;	  //�����޸�   0x41		KEY		  chSetParMode
		case 0x43 :MeaDetailDeal(KEY,Get_MeaMode());								break;

		default:break;
	}KEY=0;
}

void Data_Pro(void)									   	//ʱ�䴦����
{
	bit bOverFla=0,bTestOveFla=0,bDebFla=0,bMea_Cou;
	uint32_t RelativeValue;	  //SystemPara.chMeaValBuf
	uint8_t Size_CPM,Size_CP;//,CPS_Value_H;
	uint16_t add;
	chSec_Fla=0;
	Updata_Time(&SystemPara.SysTime[2],7);	 //��������   ʱ����   ����ʱ���� ���ڸ���   ���ڽ�����ʱ��δ��������
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
		/*-----------DEBUG--------------*///��������ʹ��
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
		if(((SystemPara.MeasureTime[0]*60 + SystemPara.MeasureTime[1]) >= MesTimMax)){	 //����ʱ�������Զ�ֹͣ
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
				MeaResDeal(4,Get_MeaMode(),1);	//������������	0x15 bMeaResuSta
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
//����ϵͳʱ��
	if(0x14 == chInterFaceState | 0x43 == chInterFaceState){
		MeaValueUpd();
		if(0x14 == chInterFaceState){
			switch((Get_MeaMode())){
			case 1: Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_ALL/10000),(uint16_t)(SystemPara.hwMeaCPMVal_ALL%10000));		//��ʾ����CPMֵ
					LCD_Display_String((156-(Size_CPM<<4)),140,DisBuffer,ADDPART1,NORMAL,NORMAL);	   
					Size_CP=sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_ALL/10000),(uint16_t)(MesCP_ALL%10000)); 	//��ʾCPֵ
					LCD_Display_String((164-(Size_CP<<4)),116,DisBuffer,ADDPART1,NORMAL,NORMAL);
					break;

			case 2:	add=SystemPara.Volume;
			case 3:	if(3 == (Get_MeaMode()))add=SystemPara.Quality;
					if(hwMeaCPMVal_Cs <= SystemPara.BKG_ALL){ RelativeValue=0; SystemPara.iMeaBqVal_Cs=0;}  //Cs��Bqֵ
					else{
						RelativeValue = hwMeaCPMVal_Cs-SystemPara.BKG_Cs;	 
						SystemPara.iMeaBqVal_Cs = RelativeValue/add;		 ///*CalibFactor_Cs
					}
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_Cs/10000),(uint16_t)(SystemPara.iMeaBqVal_Cs%10000));
					LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);//��ʾCPMֵ

					if(hwMeaCPMVal_I <= SystemPara.BKG_I) {RelativeValue=0;SystemPara.iMeaBqVal_I=0;}	  //I��Bqֵ
					else{
						RelativeValue=hwMeaCPMVal_I-SystemPara.BKG_I;
						SystemPara.iMeaBqVal_I=RelativeValue/add;
					}
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.iMeaBqVal_I/10000),(uint16_t)(SystemPara.iMeaBqVal_I%10000));
					LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
					break;

			case 4: Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(hwMeaCPMVal_Cs/10000),(uint16_t)(hwMeaCPMVal_Cs%10000));
					LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);//��ʾCPMֵ
					Size_CPM=sprintf(DisBuffer,"%d%04d",(uint16_t)(hwMeaCPMVal_I/10000),(uint16_t)(hwMeaCPMVal_I%10000));
					LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
					break;

			default:break;	
			}
		}else{	//�����и���CP��CPMֵ
			sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_ALL/10000),(uint16_t)(MesCP_ALL%10000));	  //��ʾCPֵ
			LCD_Display_String(104,80,DisBuffer,ADDPART1,NORMAL,NORMAL);
			sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs/10000),(uint16_t)(MesCP_Cs%10000));	  //��ʾCPֵ
			LCD_Display_String(136,100,DisBuffer,ADDPART1,NORMAL,NORMAL);
			sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_I/10000),(uint16_t)(MesCP_I%10000));	  //��ʾCPֵ
			LCD_Display_String(128,120,DisBuffer,ADDPART1,NORMAL,NORMAL);

			sprintf(DisBuffer,"%d%04d",(uint16_t)(SystemPara.hwMeaCPMVal_ALL/10000),(uint16_t)(SystemPara.hwMeaCPMVal_ALL%10000));	  //��ʾCPֵ
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
	if(1 == bShowMea)PackBag();		 //�����У�����ʾ����ʱ���Զ����Ͳ������ݻ��Ǳ������Ͳ�������--------
}

void InitPCA(void)
{
	CR=0;

	CH=0;
	CL=0;
	CCAPM0|=0X49; //0X101001B
	iCount=MS;
	CCAP0L=(uint8_t)(iCount>>8);
	CCAP0H=(uint8_t)iCount;//��ʱ50ms

	CR=1;
}
void InitInter(void)									//�жϳ�ʼ������
{
//��ʼ��pac
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

//��ʼ����ʱ��1
	TIM1_Close();
	PT1 = 0;
	TF0 = 0;
	Timer1_16bit();
	AUXR |= 0x40;                   //��ʱ��1Ϊ1Tģʽ
	TMOD |= 0x40;     //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
	INT_CLKO &= ~0x02;
    Init_T1;
	TIM1_Open();
						 
//��ʼ����ʱ��4	  //T4-- Cs-137
	TIM4_Close();  
	T4T3M |= 0x20;                  //��ʱ��4Ϊ1Tģʽ
	Timer4_FucCou();
	Init_T4;
	TIM4_Open();		

//��ʼ������1
	SCON = 0x50; 
	T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR |= 0x04;                //T2Ϊ1Tģʽ, ��������ʱ��2
    AUXR |= 0x01;               //ѡ��ʱ��2Ϊ����1�Ĳ����ʷ�����
	AUXR |= 0X10;
    ES = 1;                     //ʹ�ܴ���1�ж�
	
	EA=1;
}



void main(void)										   	//������
{
	bit Flag=0;
	InitInter() ;

//	Write_EEprom(23,LOG,0,1890);	 //ÿҳ256 ��8ҳ		 8K byte��Ѱַ
//	Write_EEprom(22,(uint8_t *)&SystemPara.MeachineAddr,0,50);	 //ÿҳ256 ��8ҳ		 8K byte��Ѱַ
//	RTC_Init(&SystemPara.SysTime[1],6);

	Write_EEprom(22,(uint8_t *)&SystemPara.MeachineAddr,50,50);	 //ÿҳ256 ��8ҳ		 8K byte��Ѱַ
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

	Show_Logo(48 ,80 ,ADDPART1,NORMAL);	 //��ʾ�ַ��� 
	SystemPara.MeaState=0;
	InterfaceInit();   //��ʼ����������������ʾ	  SystemPara.History.Record[0][0]
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

void PCA_Int(void) interrupt 7						  	//PCA�жϺ��� CCP0��CP�ж�Ӧ��
{
	static s_KeyNum = 1;
	CF = 0;	//��������ж�
	if(1 == CCF0)	
	{		//ģ��0��������ʱɨ��
		CCF0=0;
		iCount+=MS;
		CCAP0L = (uint8_t) iCount;
		CCAP0H = (uint8_t)(iCount >> 8);//��ʱ20ms

		chKeyTime++;
		chSec_Tim++;
		chBuzzerTim++;


		if(50 <= chSec_Tim){	 //1s��ʱ�ж�

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
				if(1==bKeyFlag){		//������us=1  us<<1 ��Ϊ����8�˴κ�͸�λ��
					s_KeyNum <<=1;		//RLC ����λ������ MOV DPTR,#0X2192  MOVC ACC,@DPTR  RLC ACC
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


