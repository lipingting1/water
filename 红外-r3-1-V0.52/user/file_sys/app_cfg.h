#include "..\app_cfg.h"
#include "EEPROM.h"
#include "lcd.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_
				
#define RAMdata	11
#define PackNum 25

extern void MainMenu (uint8_t chKey);  							//主界面处理      0x00
extern void InterfaceInit(void);       							//主界面页面

#endif