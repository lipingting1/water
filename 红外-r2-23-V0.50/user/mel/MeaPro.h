#ifndef _EEPROM_H_
#define _EEPROM_H_

void StartBacMes(void);
void StartLiqSolMes(uint8_t chKey);
void MeasureBusyPage(void);							 	//����æµҳ��
void MeasureBusy(uint8_t chKey);						//����æµ����  		0x11
void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode);	//��������鿴����  0x43
void MeasureProPage(uint8_t chMode,bit bMeaState); 		//�����ɹ�ҳ�� �� ������ҳ��		   0x12
void MeaProDeal(uint8_t chKey,uint8_t chMode);			//�����д���			0x14
void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta);	//������������	0x15 bMeaResuSta
void StartMes(void);

//extern void StartBacMes(void);
//extern void StartLiqSolMes(uint8_t chKey);
//extern void MeasureBusyPage(void);							 	//����æµҳ��
//extern void MeasureBusy(uint8_t chKey);						//����æµ����  		0x11
//extern void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode);	//��������鿴����  0x43
//extern void MeasureProPage(uint8_t chMode,bit bMeaState); 		//�����ɹ�ҳ�� �� ������ҳ��		   0x12
//extern void MeaProDeal(uint8_t chKey,uint8_t chMode);			//�����д���			0x14
//extern void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta);	//������������	0x15 bMeaResuSta

#endif


