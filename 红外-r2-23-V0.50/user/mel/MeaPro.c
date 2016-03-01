#include"app_cfg.h"

extern uint8_t chInterFaceState,ParaBuffer[32],DisBuffer[12],chCalinUum,chSetParMode;
extern bit bMeaResuSta,bConMea,chSec_Fla;
extern Uint32_t	MesCP_Cs,MesCP_I;
extern Sys_Para SystemPara;
float fMeaVal;
extern uint16_t sdf;

uint8_t code Reason[3][20]={
							{"ʱ�����!"},
						    {"���׹���!"},
						    {"У׼������������!"},		  
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
						{"�� ��"},
						{"Һ ��"},
						{"�� ��"},
						{"У ׼"},
						};

void StartMes(void)
{
	Start_Measure();
	TIM0_Open();
	MesCP_Cs.SumNum=0;
	MesCP_I.SumNum =0;
	SystemPara.MeasureTime[0]=0;
	SystemPara.MeasureTime[1]=0;
	SystemPara.MeasureTime[2]=0;
	chSec_Fla=1;
}

void MeasureBusyPage(void)							 	//����æµҳ��
{
	Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
	LCD_Display_String(80,60,"�� �� æ",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(60,128,"��������������!",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(264,120,"������",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame (0,32,240,184,ADDPART1,NORMAL);
	chInterFaceState=0x11;
}

void MeasureBusy(uint8_t chKey)							//����æµ����  		0x11
{
	switch(chKey){
	case 1,2,4:break;
	case 3: if((SystemPara.MeaState&(1<<4)))MeaModePaste();		// ���æ ���ò������౸�� �ͷŲ���æ
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
	LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);
	sprintf(DisBuffer,"CP:��:%d%04d",(uint16_t)(MesCP_Cs.SumNum/10000),(uint16_t)(MesCP_Cs.SumNum%10000));
	LCD_Display_String(56,80 ,DisBuffer,ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(80,100 ,"Cs-137:---",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(80,120, "I-131:---",ADDPART1,NORMAL,NORMAL);
	LCD_Display_String(80,140,"����:---",ADDPART1,NORMAL,NORMAL);
	if(2 == chMode | 3 == chMode){
		LCD_Display_String(56,170,"CPM:Cs-137:---",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(88,190,"I-131:---",ADDPART1,NORMAL,NORMAL);
	}
}

void MeasureProPage(uint8_t chMode,bit bMeaState) 		//�����ɹ�ҳ�� �� ������ҳ��		   0x12
{
	bit bShoMeaStaF;														//chMode��־��ǰ����ģʽ:1���ء�2Һ�塢3���塢4У׼����	bMeaState��ʶ:0�����С�1�����ɹ�
	if(1==bMeaState){
		Timer0_Run();				//��ʼ����
		chInterFaceState=0x14;	//ҳ�洦����
		bConMea=0;
		Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
		LCD_Display_String(108,60,"�� �� ��",ADDPART1,NORMAL,NORMAL);	
		LCD_Display_String(264,120,"��ֹ",ADDPART1,NORMAL,NORMAL);
		if(1 != chMode)LCD_Display_String(264,80,"����",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
		sprintf(DisBuffer,"BKG:%d%04d",(uint16_t)(SystemPara.BKG/10000),(uint16_t)(SystemPara.BKG%10000));
		LCD_Display_String(0,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
		bShoMeaStaF=1;
		switch(chMode){
		case 1: //LCD_Display_String(60,60,"�� �� ",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(32,220,"������",ADDPART1,NORMAL,NORMAL);
				break;
		case 2: //LCD_Display_String(60,60,"Һ �� ",ADDPART1,NORMAL,NORMAL);
				sprintf(DisBuffer,"V:%dml",SystemPara.Volume);				
				LCD_Display_String(130,220,DisBuffer,ADDPART1,NORMAL,NORMAL);	
				break;
		case 3: //LCD_Display_String(60,60,"�� �� ",ADDPART1,NORMAL,NORMAL);   
				sprintf(ParaBuffer,"M:%04dg V:%dml",SystemPara.Quality,SystemPara.Volume);
				LCD_Display_String(96,220,ParaBuffer,ADDPART1,NORMAL,NORMAL);		  //�±�  Һ�����
				break;
		case 4://LCD_Display_String(60,60,"У ׼ ",ADDPART1,NORMAL,NORMAL);
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
			LCD_Display_String(80,120,"����:--%",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(44,144,"I-131:",ADDPART1,NORMAL,NORMAL);
			LCD_Display_String(80,180,"����:--%",ADDPART1,NORMAL,NORMAL);
		}
	}else{
		chInterFaceState=0x15;	//���ҳ�洦����
		LCD_Display_String(264,160,"����",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,120,"�ز�",ADDPART1,NORMAL,NORMAL);
		if(1==chMode)Clear_Windows(0,220,80,16,ADDPART1,NORMAL);
		if(4==chMode){
			chCalinUum++;
			fMeaVal+=SystemPara.hwMeaCPMVal_Cs;
		}
	}
	LCD_Display_String(232,220,"T:",ADDPART1,NORMAL,NORMAL);
	Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);
}


void MeaProDeal(uint8_t chKey,uint8_t chMode)			//�����д���			0x14
{
	uint8_t chReason;
	switch(chKey){
	case 2: if(1 == chMode)break;
			MeaDetail(chMode);break;
	case 3:	Stop_Measure();				//��ֹ
			LCD_Display_String(156,60 ,"�� ��",ADDPART1,NORMAL,NORMAL);
			if(1 != chMode)LCD_Display_String(264,80,"����",ADDPART1,NORMAL,NORMAL);
			if(SystemPara.MTime > (SystemPara.MeasureTime[0]*60+SystemPara.MeasureTime[1])){
				bMeaResuSta=0;
				chReason=0;
				chInterFaceState=0x15;
				Clear_Windows(0,80,320,120,ADDPART1,NORMAL);
				LCD_Display_String(80,144,&Reason[chReason][0],ADDPART1,NORMAL,NORMAL);	 //��ʾ����ʧ��ԭ��
				LCD_Display_String(96 ,96 ,"ʧ �� !",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(264,120,"�ز�",ADDPART1,NORMAL,NORMAL);
				LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
				if(1!=chMode)LCD_Display_String(264,80,"����",ADDPART1,NORMAL,NORMAL);
				if(1==chMode)LCD_Display_String(32,220,"����ʧ��!",ADDPART1,NORMAL,NORMAL);
				Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);break;
			}
			bMeaResuSta=1;
			MeasureProPage(Get_MeaMode(),(bit)(SystemPara.MeaState&(1<<6)));break; //	�ж�ʱ���Ƿ񵽴��ٽ�ʱ�� ������Ҫ���������ʧ��ԭ��
	case 5: if(4==chMode | 0==(SystemPara.MeaState&(1<<6)))MainMenu(chMode);  /*��Ҫ����//////	 */
			else InterfaceInit();
			break;
	default:break;
	}
}

void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta)	//������������	0x15 bMeaResuSta
{
	switch(chKey){
	case 2: if(1 == chMode)break;
			MeaDetail(chMode);break;
	case 3: StartMes();
			Clear_Windows(0,216,320,24,ADDPART1,NORMAL);
		    MeasureProPage(Get_MeaMode(),1);break;	   //�ز�
	case 4: if(0==bMeaResuSta);
			else SaveValue(chMode);
			break;  //��ת���������  ����		
	case 5: chCalinUum=0;
			MainMenu(chMode);break;	   //����
	default:break;
	}		
}
void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode)//��������鿴����  0x43
{
	switch(chKey){
	case 5: if((SystemPara.MeaState&(1<<6)))MeasureProPage(chMesMode,(bit)(SystemPara.MeaState&(1<<6)));
			else MeaProDeal(3,Get_MeaMode());
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
		LCD_Display_String(72 ,60 ,"�� �� �� ��",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(248,40 ,"��ʼ����",ADDPART1,NORMAL,NORMAL);
	    LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(8  ,112,"1 �뽫׼���õĴ���ˮ��������",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(24 ,130,"��.",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(8  ,148,"2 ����ʼ���������ײ���.",ADDPART1,NORMAL,NORMAL);
		Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL); 
	}
	else MeasureProPage(Get_MeaMode(),1);
}

void StartLiqSolMes(uint8_t chKey)
{
	if(0==(SystemPara.MeaState&(1<<6))||((SystemPara.MeaState&(1<<6))&&(chKey!=Get_MeaMode())||(SystemPara.MeaState&(1<<4))))
	{
		if(0==(SystemPara.MeaState&(1<<4))){   //�жϲ���æ
			if((SystemPara.MeaState&(1<<6))&&(chKey!=Get_MeaMode())){
				Mea_Busy();
				MeaModeCopy();
			}
			if(2==chKey)Mes_Liq(M);
			else Mes_Solid(M);
			chSetParMode=chKey-1;
		}
		Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
		LCD_Display_String(70,60," �� �� Ʒ �� Ϣ",ADDPART1,NORMAL,NORMAL);	   
		ParaPage(chSetParMode);	
	} 
	else MeasureProPage(Get_MeaMode(),1);
}

void MeaValueUpd(void)
{
	uint16_t MeaSecTim;
	uint32_t CP_Value;
	MesCP_Cs.Num[2] = TH0;
	MesCP_Cs.Num[3] = TL0;
	MeaSecTim = (SystemPara.MeasureTime[0])*3600+SystemPara.MeasureTime[1]*60+SystemPara.MeasureTime[2];	//��������
	CP_Value = MesCP_Cs.SumNum/MeaSecTim;
	SystemPara.hwMeaCPMVal_Cs = CP_Value*60;
//	CP_Value = MesCP_I.SumNum/MeaSecTim;
//	SystemPara.hwMeaCPMVal_I = CP_Value*60;
}