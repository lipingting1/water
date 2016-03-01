#include "..\app_cfg.h"
#include"lcd.h"
#include <Stdarg.h>

#ifndef _UART_APP_CFG_H_
#define _UART_APP_CFG_H_

#define SEND_BUFFER_NUM 120

extern void InterfaceInit(void);       							//������ҳ��
extern void MeasureBusyPage(void);							 	//����æµҳ��
extern void MainMenu (uint8_t chKey); 							//�����洦��      0x00
extern void StartMes(void);
extern void MeaValueUpd(void);
extern bit  SaveDeal(uint8_t chMode);

extern void ReadRAM(uint8_t NowPageAddr,uint8_t *k,uint8_t *NowPos);
extern void DeletRec(uint8_t NowPageAddr,uint8_t Ptr);

#endif