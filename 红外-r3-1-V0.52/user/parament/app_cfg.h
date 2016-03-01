#include "..\app_cfg.h"
#include"lcd.h"
#include "I2C.h"
#include "EEPROM.h"
#include "DSPs.h"
#include "MeaPro.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

extern void InterfaceInit(void);       							//主界面页面
extern void MainMenu (uint8_t chKey); 							//主界面处理      0x00
extern void MeaModeCopy(void);
extern void MeaModePaste(void);

#endif
