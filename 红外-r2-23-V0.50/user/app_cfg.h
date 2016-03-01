/************************************
 *main.c输入配置文件
 *2015/7/29
 */

#ifndef __APP_CFG_H_
#define __APP_CFG_H_

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
//口定义

#define uint8_t   		unsigned char
#define uint16_t  		unsigned int
#define int16_t			int
#define uint32_t  		unsigned long int

#define FOSC      24000000L
#define MS		  20000UL


#define StanValue   4000
#define MesTimMax   360    //测量时间上限
#define MesTimMin   2	   //测量时间下限
#define Standard    4085   //标准cp值

#define Total_ram_Num 22
#define System_ram 23
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

#define 	Timer0_Run()	 			TR0 = 1				//允许定时器0计数  
#define 	Timer1_Run()	 			TR1 = 1				//允许定时器0计数  
#define 	Timer0_Stop()	 			TR0 = 0				//允许定时器0计数  
#define 	Timer1_Stop()	 			TR1 = 0				//禁止定时器0计数
#define 	Timer0_InterruptEnable()	ET0 = 1				//允许Timer0中断.
#define		Timer0_16bit()				TMOD  = (TMOD & ~0x03) | 0x01	//16位
		
#define 	Timer3_Run()	 			T4T3M |= (1<<3)				//允许定时器3计数
#define 	Timer3_Stop()	 			T4T3M &=~(1<<3)				//禁止定时器0计数
#define 	Timer3_InterruptEnable()	IE2   |= (1<<5)					//允许Timer3中断
#define 	Timer3_FucCou()				T4T3M |= (1<<2)			 	//计数功能

#define 	Timer4_Run()	 			T4T3M |= (1<<7)				//允许定时器0计数
#define 	Timer4_Stop()	 			T4T3M &=~(1<<7)			//禁止定时器0计数
#define 	Timer4_InterruptEnable()	IE2   |= (1<<6)					//允许Timer3中断
#define 	Timer4_FucCou()				T4T3M |= (1<<6)			 	//计数功能

#define		Once_Measure()				SystemPara.MeaState=SystemPara.MeaState&~(1<<7)
#define		Continue_Measure()			SystemPara.MeaState=SystemPara.MeaState| (1<<7)
#define		Stop_Measure()				SystemPara.MeaState=SystemPara.MeaState&~(1<<6)
#define		Start_Measure()				SystemPara.MeaState=SystemPara.MeaState| (1<<6)
#define		Failed_Save()				SystemPara.MeaState=SystemPara.MeaState&~(1<<5)
#define		Success_Save()				SystemPara.MeaState=SystemPara.MeaState| (1<<5)
#define		Mea_Free()					SystemPara.MeaState=SystemPara.MeaState&~(1<<4)
#define		Mea_Busy()					SystemPara.MeaState=SystemPara.MeaState| (1<<4)

#define		Mes_Back(M)					SystemPara.MeaState=SystemPara.MeaState&0xfc		  //出
#define		Mes_Liq(M)					SystemPara.MeaState=SystemPara.MeaState&0xfd|0x01
#define		Mes_Solid(M)				SystemPara.MeaState=SystemPara.MeaState&0xfe|0x02
#define		Mes_Cail(M)					SystemPara.MeaState=SystemPara.MeaState|0x03

#define		Get_MeaMode()				1// ((SystemPara.MeaState&0x03)+1)	   
#define		Get_MeaMode2()				((SystemPara.MeaState&0x03)+1)	   				 
#define		Get_MeaModeSha()			(((SystemPara.MeaState&0x0c)>>2)+1)

#define		Open_ADC0()					ADC_CONTR=0x88
#define		Open_ADC1()					ADC_CONTR=0x89
#define 	GetADC_Sta					ADC_CONTR&0x10	// 读取中断结束标志位

typedef struct{
	uint8_t Record[2][25];
}HistoryData;

typedef struct{
	uint32_t MeachineAddr;	 //仪器编号	  //	4

	uint32_t BKG;						  //	4
	uint16_t Quality;					  //	2
	uint16_t Volume;					  //	2
	//	12
	uint16_t MTime;          //测量定时	  //	2
	uint8_t  MeasureAcc;     //测量精度	  //	1	
	uint8_t  chFucCod;		 //指令功能位 // 	1
	uint16_t Alarm_V;		 //阈值		  //	2
	uint16_t Alarm_Q;		 		  	  //	2
	//	20
	float CalibFactor_Cs;    //校准系数	  //	4
	float CalibFactor_I;	 			  //	4
	uint8_t SysData[3];      //系统日期	  //	3
	uint8_t SysTime[3];      //系统时间	  //	3
	//	34
	uint16_t hwHvPara;					  //	2
	uint16_t hwAmpPara;					  //	2
	//	38
	uint8_t  MeasureTime[3];  	//测量计时//	3
	uint8_t MeaState;					  //	1	 MeaState //开机初始化/测量状态//	1 （方式：最高位 1调试，0正常）A| 请求开始0，后台1，显示2，结束3，开始测量允许4，开始测量禁止5）
    //	42
	uint8_t chHisPage;					  //	1
	uint8_t chHisCSR;					  //	1
	//	44
	uint32_t hwMeaCPMVal_Cs;			  //	4
	uint32_t hwMeaCPMVal_I;		   		  //	4
	uint32_t iMeaBqVal_Cs;				  //	4
	uint32_t iMeaBqVal_I;				  //	4
	//	60
	HistoryData History;				  //	50
	int16_t RecordQua;					  //	2
	//	112			  
}Sys_Para;						

typedef union{
	uint8_t Num[4];
	unsigned long int SumNum;
}Uint32_t;

typedef enum{
	BAT1,
	BAT2,
	BAT3,
	BAT4,
	AC_POW,
}PowerType;

#endif /*EOF*/