#ifndef _MAIN_H_
#define _MAIN_H_

extern void StartBacMes(void);
extern void StartLiqSolMes(uint8_t chKey);
extern void MeasureBusyPage(void);							 	//����æµҳ��
extern void MeasureBusy(uint8_t chKey);						//����æµ����  		0x11
extern void MeaDetailDeal(uint8_t chKey,uint8_t chMesMode);	//��������鿴����  0x43
extern void MeasureProPage(uint8_t chMode,bit bMeaState); 		//�����ɹ�ҳ�� �� ������ҳ��		   0x12
extern void MeaProDeal(uint8_t chKey,uint8_t chMode);			//�����д���			0x14
extern void MeaResDeal(uint8_t chKey,uint8_t chMode,uint8_t bMeaResuSta);	//������������	0x15 bMeaResuSta
extern void StartMes(void);
extern void MeaValueUpd(void);

extern void RecDeal(void);
extern uint8_t UartTest();


extern void Updata_Time(uint8_t *p,uint8_t num);
extern void RTC_Init(uint8_t *p,uint8_t num);

extern void _nop(uint8_t i);

extern void Write_RDAC(uint16_t dat);	//	 uint8_t cmd,

#endif