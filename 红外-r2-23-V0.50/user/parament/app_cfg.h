#include "..\app_cfg.h"
#include"lcd.h"
#include "I2C.h"
#include "EEPROM.h"

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#define CMD_WRITE_TO_RDAC			0x0400

extern void InterfaceInit(void);       							//������ҳ��
extern void MeasureBusyPage(void);							 	//����æµҳ��
extern void MainMenu (uint8_t chKey); 							//�����洦��      0x00
extern void StartMes (void); 									//�����洦��      0x00
extern void MeaModeCopy(void);
extern void MeaModePaste(void);
extern void Write_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint16_t num);	 //ÿҳ256 ��8ҳ		 8K byte��Ѱַ
extern void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num);	 //ÿҳ256 ��8ҳ		 2K��Ѱַ
extern void MeasureProPage(uint8_t chMode,bit bMeaState); 		//�����ɹ�ҳ�� �� ������ҳ��		   0x12
extern void DACCommand(unsigned int Command, unsigned int *Value);

#endif
