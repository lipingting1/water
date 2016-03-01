#include "..\app_cfg.h"
#include"lcd.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

extern void ParaPage(uint8_t chMode);							//参数修改页面  
extern void MainMenu (uint8_t chKey);  							//主界面处理      0x00
extern void SaveValue(uint8_t chMode);	   						//保存测量结果处理
extern void InterfaceInit(void);       							//主界面页面
extern void MeaModeCopy(void);
extern void MeaModePaste(void);
extern void Data_Pro(void);									   	//时间处理函数

#endif
