#include "..\app_cfg.h"
#include"lcd.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

extern void ParaPage(uint8_t chMode);							//�����޸�ҳ��  
extern void MainMenu (uint8_t chKey);  							//�����洦��      0x00
extern void SaveValue(uint8_t chMode);	   						//��������������
extern void InterfaceInit(void);       							//������ҳ��
extern void MeaModeCopy(void);
extern void MeaModePaste(void);
extern void Data_Pro(void);									   	//ʱ�䴦����

#endif
