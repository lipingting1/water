/************************************
 *main.c���������ļ�
 *2015/7/29
 */

#ifndef __MAIN_APP_CFG_H_
#define __MAIN_APP_CFG_H_

 /******************include***********/
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "stdio.h"

/*************************************/
sbit KEY1=P7^4;
sbit KEY2=P7^5;
sbit KEY3=P7^6;
sbit KEY4=P7^7;
sbit KEY5=P4^5;
//�ڶ���

#define uint8_t   		unsigned char
#define uint16_t  		unsigned int
#define int16_t			int
#define uint32_t  		unsigned long int

#define FOSC      24000000L
#define MS		  40000UL
#define BAUD      9600

#define StanValue   4000
#define MesTimMax   360    //����ʱ������
#define MesTimMin   2	   //����ʱ������
#define Standard    4085   //��׼cpֵ

#define Total_ram_Num 22
#define System_ram 22
#define EMPTY	0XFF
#define NORMAL	1
#define REVERSE	0

sbit BackLight = P4^6;
sbit LED1= P5^1;
sbit LED2= P3^6;
sbit LED3= P5^0;

sbit BUZZER=P7^0;

#define TIM0_Open()		 	TR0=1
#define TIM0_Close() 		TR0=0
#define TIM1_Open()		 	TR1=1
#define TIM1_Close() 		TR1=0
#define TIM4_Open()		 	T4T3M |= 0X80
#define TIM4_Close() 		T4T3M &= 0X7F

#define Init_T0				TH0=0,TL0=0;
#define Init_T1				TH1=0,TL1=0;
#define Init_T4				T4H=0,T4L=0;


#define		Timer0_16bit()				TMOD  = (TMOD & ~0x03) | 0x01	//16λ
#define		Timer1_16bit()				TMOD  = (TMOD & ~0x30) | 0x10	//16λ
		
#define 	Timer4_FucCou()				T4T3M |= (1<<6)			 	//��������

#define		Once_Measure()				SystemPara.MeaState=SystemPara.MeaState&~(1<<7)
#define		Continue_Measure()			SystemPara.MeaState=SystemPara.MeaState| (1<<7)
#define		Stop_Measure()				SystemPara.MeaState=SystemPara.MeaState&~(1<<6)
#define		Start_Measure()				SystemPara.MeaState=SystemPara.MeaState| (1<<6)
#define		Failed_Save()				SystemPara.MeaState=SystemPara.MeaState&~(1<<5)
#define		Success_Save()				SystemPara.MeaState=SystemPara.MeaState| (1<<5)
#define		Mea_Free()					SystemPara.MeaState=SystemPara.MeaState&~(1<<4)
#define		Mea_Busy()					SystemPara.MeaState=SystemPara.MeaState| (1<<4)

#define		Mes_Back(M)					SystemPara.MeaState=SystemPara.MeaState&0xfc		  //��
#define		Mes_Liq(M)					SystemPara.MeaState=SystemPara.MeaState&0xfd|0x01
#define		Mes_Solid(M)				SystemPara.MeaState=SystemPara.MeaState&0xfe|0x02
#define		Mes_Cail(M)					SystemPara.MeaState=SystemPara.MeaState|0x03

#define		Get_MeaMode()				((SystemPara.MeaState&0x03)+1)	   
//#define		Get_MeaMode2()				((SystemPara.MeaState&0x03)+1)	 1//   				 
#define		Get_MeaModeSha()			(((SystemPara.MeaState&0x0c)>>2)+1)

#define		Open_ADC0()					ADC_CONTR=0x88
#define		Open_ADC1()					ADC_CONTR=0x89
#define 	GetADC_Sta					ADC_CONTR&0x10	// ��ȡ�жϽ�����־λ

typedef struct{
	uint8_t Record[2][25];
}HistoryData;

typedef struct{
	uint32_t MeachineAddr;	 //�������	  //	4

	uint32_t BKG_ALL;					  //	4	 
	uint32_t BKG_Cs;					  //	4
	uint32_t BKG_I;						  //	4
	//	16
	uint16_t Volume;					  //	2
	uint16_t Quality;					  //	2
	//	20
	uint16_t MTime;          //������ʱ	  //	2
	uint8_t  MeasureAcc;     //��������	  //	1	
	uint8_t  chFucCod;		 //ָ���λ // 	1
	uint16_t Alarm_V;		 //��ֵ		  //	2
	uint16_t Alarm_Q;		 		  	  //	2
	//	28
	float CalibFactor_Cs;    //У׼ϵ��	  //	4
	float CalibFactor_I;	 			  //	4
	uint8_t SysData[3];      //ϵͳ����	  //	3
	uint8_t SysTime[3];      //ϵͳʱ��	  //	3
	uint16_t hwHvPara;					  //	2
	uint16_t hwAmpPara;					  //	2
	//	46
	int16_t RecordQua;					  //	2
	//	48
	uint8_t MeaState;					  //	1	 MeaState //������ʼ��/����״̬//	1 ����ʽ�����λ 1���ԣ�0������A| ����ʼ0����̨1����ʾ2������3����ʼ��������4����ʼ������ֹ5��
	uint8_t  MeasureTime[3];  	//������ʱ//	3
	//	52
	uint32_t iMeaBqVal_Cs;				  //	4
	uint32_t iMeaBqVal_I;				  //	4
	uint32_t hwMeaCPMVal_ALL;		   	  //	4
//	uint32_t hwMeaCPMVal_Cs;			  //	4
//	uint32_t hwMeaCPMVal_I;		   		  //	4
	//	60
	HistoryData History;				  //	50
	uint8_t chHisPage;					  //	1
	uint8_t chHisCSR;					  //	1
	//	112			  
}Sys_Para;						


typedef enum{
	BAT1,
	BAT2,
	BAT3,
	BAT4,
	AC_POW,
}PowerType;

#endif /*EOF*/