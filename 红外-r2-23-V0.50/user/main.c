/***********************************
 *������
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
};	//ʱ���� ����	
HistoryData His={0};				  //	50
													 //�� ���� ��
bit bKeyFlag,bAvoidRepeat,bConMea=0,bHighMea=0;	//bKeyFlag����������־λ  bAvoidRepeat�������ظ���ʶλ bConMea��λ�����Ʋ�����ʶλ 
xdata uint8_t Para_Pos=0,DisBuffer[12],ParaBuffer[37],chParaNum;//chParaNum;
uint16_t iCount;
uint8_t Xper,Xpos,Yper=0,Ypos=0;
code uint8_t YposLim[4]={10,30,70,110};

uint8_t CSR_Old,chPage;	//ҳ��
//ʱ��
uint16_t chSec_Tim=0;
uint8_t chKeyTime=0,chBuzzerTim=0;
bit chSec_Fla,DatUpdFla=0;
bit bDisDel;  //�����ʷ��¼�洢����Ϊ0�ı�־λ  ���Ϊ0���λ��1  ������0
uint16_t CPM_Value;
uint8_t AccKeyValue,KEY=0,chInterFaceState=0x00,chCalinUum,chSetParMode;	//��������ڵ�ַ����ǰ����ģʽ��1����2Һ��3����4У׼������ǰ����ģʽ��������������ģʽ��1Һ��2����3��������4ϵͳ������																				  
bit  bMeaResuSta,bSavPar=0,bSetSucFla=0,bMesCalib; //	 bMeaResuSta 0����ʧ�� 1�����ɹ� 	  bSavPar 0����δ����	1�����޸Ĺ�  bSetSucFla 1������޸ĳɹ���0�޶���  bMesCalib 0��������ΪCs-137   1 I-137  

//ͨѶ
bit bShowMea=0;	//��λ����ʾ����ѭ�����Ͳ���ֵ��־λ
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

void InterfaceInit(void)       							//������ҳ��
{
	Mea_Free();
	chInterFaceState=0x00;
	chSec_Fla=1;
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
//				case 1: LCD_Display_String((156-(Size_CPM<<4)),140,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //��ʾCPMֵ
//						Size_CP=sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000)); //��ʾCPֵ
//						LCD_Display_String((164-(Size_CP<<4)),116,DisBuffer,ADDPART1,NORMAL,NORMAL);
//						sprintf(DisBuffer,"%05dcps",(uint16_t)CPS_Value);//��ʾCPSֵ
//						LCD_Display_String(96,180,DisBuffer,ADDPART1,NORMAL,NORMAL);break;
//				case 2:	
//				case 3:	
//				case 4: if(0==bMesCalib) LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //��ʾCPMֵ
//						else LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
//						break;
//				default:break;	
//				}
//			}else{	//�����и���CP��CPMֵ
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
				MeaResDeal(4,Get_MeaMode(),1);	//������������	0x15 bMeaResuSta
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
//����ϵͳʱ��
	if(1 == bShowMea)PackBag();		 //�����У�����ʾ����ʱ���Զ����Ͳ������ݻ��Ǳ������Ͳ�������--------
	chSec_Fla=0;
	Updata_Time(&SystemPara.SysTime[2],7);	 //��������   ʱ����   ����ʱ���� ���ڸ���   ���ڽ�����ʱ��δ��������
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
				case 1: LCD_Display_String((156-(Size_CPM<<4)),140,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //��ʾCPMֵ
						Size_CP=sprintf(DisBuffer,"%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000)); //��ʾCPֵ
						LCD_Display_String((164-(Size_CP<<4)),116,DisBuffer,ADDPART1,NORMAL,NORMAL);
						sprintf(DisBuffer,"%05dcps",(uint16_t)CPS_Value);//��ʾCPSֵ
						LCD_Display_String(96,180,DisBuffer,ADDPART1,NORMAL,NORMAL);break;
				case 2:	
				case 3:	
				case 4: if(0==bMesCalib) LCD_Display_String((178-(Size_CPM<<4)),104,DisBuffer,ADDPART1,NORMAL,NORMAL);	   //��ʾCPMֵ
						else LCD_Display_String((178-(Size_CPM<<4)),158,DisBuffer,ADDPART1,NORMAL,NORMAL);
						break;
				default:break;	
				}
			}else{	//�����и���CP��CPMֵ
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

void InitInter(void)									//�жϳ�ʼ������
{
//��ʼ��pac
	Stop_Measure();
	CH=0;
	CL=0;
	CCAPM0|=0X49; //0X101001B
	iCount=MS;
	CCAP0L=(uint8_t)(iCount>>8);
	CCAP0H=(uint8_t)iCount;//��ʱ50ms
	CR=1;
//��ʼ����ʱ��0
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

//��ʼ����ʱ��1
//	AUXR |= 0x40;                   //��ʱ��1Ϊ1Tģʽ
//  AUXR &= 0xdf;                   //��ʱ��1Ϊ12Tģʽ

	Timer1_Stop();				
    TMOD = (TMOD&0x0f)|0x40;     //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
    TL1 = 0;                     //��ʼ����ʱֵ
    TH1 = 0;
    TR1 = 1;                        //��ʱ��1��ʼ��ʱ
    ET1 = 1;                        //ʹ�ܶ�ʱ��0�ж�

//��ʼ����ʱ��3
	Timer3_Stop(); //T3--   I-131
	Timer3_InterruptEnable();
	Timer3_FucCou();
	T3H= 0;
	T3L= 0;
//��ʼ����ʱ��4
/*
//	Timer4_Stop(); //T3--  Cs-131
//	Timer4_InterruptEnable();
//	Timer4_FucCou();
//	T4H= 0;
//	T4L= 0;			
*/
//��ʼ������1
	SCON  = 0x50;		// 8λ����,�ɱ䲨����
	AUXR |= 0x40;		// ��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		// ����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		// �趨��ʱ��1Ϊ16λ�Զ���װ��ʽ

	TL1 = 0x8F;		//�趨��ʱ��ֵ
	TH1 = 0xFD;		//�趨��ʱ��ֵ

	ET1 = 0;			// ��ֹ��ʱ��1�ж�
	TR1 = 1;			// ������ʱ��1	

	ES=1;
//��ʼ��SPI
//	SPCTL|=1001<<4;
//	SPCTL&=0xdf;
//	IE2|=0x02;

//    SPDAT = 0;                  //��ʼ��SPI����
//    SPSTAT = 0;       //���SPI״̬λ
//	IE2|=0x02;
//��ʼ��ADC
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
void main(void)										   	//������
{
	bit Flag=0;
	extern code uint8_t LOG[];
	uint16_t  ControlRegisterValue,RdacValue;
//	Write_EEprom(23,LOG,0,1890);	 //ÿҳ256 ��8ҳ		 8K byte��Ѱַ
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
//	Show_Logo(48 ,80 ,ADDPART1,NORMAL);	 //��ʾ�ַ��� 
	InterfaceInit();   //��ʼ����������������ʾ	  SystemPara.History.Record[0][0]
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
//			LCD_Display_String(16,80,ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ	
//		}
//		sprintf(ParaBuffer,"%2d %02d/%02d/%02d         %02d:%02d",(uint16_t)(sd[0]),(uint16_t)sd[1],(uint16_t)sd[2],(uint16_t)sd[3],(uint16_t)sd[4],(uint16_t)sd[5]);
//		LCD_Display_String(0,0,ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ	
	}
}

void Time0(void) interrupt 1						   	//��ʱ/������0  ����ģʽ �жϺ���
{
	MesCP_Cs.Num[1]++;
	if(MesCP_Cs.Num[1] >= 255){
		MesCP_Cs.Num[0]++;
		MesCP_Cs.Num[1]=0;
	}
}

void Time3(void) interrupt 19						   	//��ʱ/������3  ����ģʽ �жϺ���
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
void PCA_Int(void) interrupt 7						  	//PCA�жϺ��� CCP0��CP�ж�Ӧ��
{
	static s_KeyNum = 1;
	CF = 0;	//��������ж�
	if(1 == CCF0)	
	{		//ģ��0��������ʱɨ��
		CCF0=0;
		iCount+=MS;
		CCAP0L = (uint8_t) iCount;
		CCAP0H = (uint8_t)(iCount >> 8);//��ʱ10ms

		chKeyTime++;
		chSec_Tim++;
		chBuzzerTim++;

		if(100 <= chSec_Tim){	 //1s��ʱ�ж�
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


