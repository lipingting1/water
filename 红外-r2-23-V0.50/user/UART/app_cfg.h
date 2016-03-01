#include "..\app_cfg.h"
#include"lcd.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

extern void InterfaceInit(void);       							//主界面页面
extern void MeasureBusyPage(void);							 	//测量忙碌页面
extern void MainMenu (uint8_t chKey); 							//主界面处理      0x00
extern void StartMes(void);
extern void MeaValueUpd(void);
extern bit  SaveDeal(uint8_t chMode);

extern void ReadRAM(uint8_t NowPageAddr,uint8_t *k,uint8_t *NowPos);
extern void DeletRec(uint8_t NowPageAddr,uint8_t Ptr);

#endif