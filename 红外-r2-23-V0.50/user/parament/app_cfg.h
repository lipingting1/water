#include "..\app_cfg.h"
#include"lcd.h"
#include "I2C.h"
#include "EEPROM.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#define CMD_WRITE_TO_RDAC			0x0400

extern void InterfaceInit(void);       							//主界面页面
extern void MeasureBusyPage(void);							 	//测量忙碌页面
extern void MainMenu (uint8_t chKey); 							//主界面处理      0x00
extern void StartMes (void); 									//主界面处理      0x00
extern void MeaModeCopy(void);
extern void MeaModePaste(void);
extern void Write_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint16_t num);	 //每页256 共8页		 8K byte可寻址
extern void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num);	 //每页256 共8页		 2K可寻址
extern void MeasureProPage(uint8_t chMode,bit bMeaState); 		//测量成功页面 和 测量中页面		   0x12
extern void DACCommand(unsigned int Command, unsigned int *Value);

#endif
