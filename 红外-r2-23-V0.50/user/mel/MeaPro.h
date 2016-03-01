#ifndef _EEPROM_H_
#define _EEPROM_H_

void StartBacMes(void);
void StartLiqSolMes(uint8_t chKey);
void MeasureBusyPage(void);							 	//测量忙碌页面
void MeasureBusy(uint8_t chKey);						//测量忙碌处理  		0x11
void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode);	//测量详情查看界面  0x43
void MeasureProPage(uint8_t chMode,bit bMeaState); 		//测量成功页面 和 测量中页面		   0x12
void MeaProDeal(uint8_t chKey,uint8_t chMode);			//测量中处理			0x14
void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta);	//测量结束处理	0x15 bMeaResuSta
void StartMes(void);

//extern void StartBacMes(void);
//extern void StartLiqSolMes(uint8_t chKey);
//extern void MeasureBusyPage(void);							 	//测量忙碌页面
//extern void MeasureBusy(uint8_t chKey);						//测量忙碌处理  		0x11
//extern void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode);	//测量详情查看界面  0x43
//extern void MeasureProPage(uint8_t chMode,bit bMeaState); 		//测量成功页面 和 测量中页面		   0x12
//extern void MeaProDeal(uint8_t chKey,uint8_t chMode);			//测量中处理			0x14
//extern void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta);	//测量结束处理	0x15 bMeaResuSta

#endif


