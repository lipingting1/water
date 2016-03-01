#include "app_cfg.h"

void AvoidError(void)
{
	IAP_ADDRH=0xff;			 //��ַ�����ڷ�FLASHλ�� ���������
	IAP_ADDRL=0xff;
	IAP_CONTR = 0;			//��ֹISP/IAP����
	IAP_CMD   = 0;			//����
	IAP_TRIG  = 0;
}

//����һ����ʼ��ȡλ��																
void Read_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size)
{
	uint16_t i=0;
	EA=0;
	IAP_CMD = 1;  //000000001B	������
	IAP_CONTR=ISP_EN+ISP_WAIT_FREQUENCY;
	do{
		IAP_ADDRH=(uint8_t)(eeprom_addr>>8);
		IAP_ADDRL=(uint8_t) eeprom_addr;
		ISP_TRIG();
		_nop_();
		*pAddr = IAP_DATA;
		eeprom_addr++;
		if(i < StaPos)i++;
		else pAddr++; 
	}while(--Size);
	AvoidError();
	EA=1;
}

//����һ����ʼд��λ��
void Write_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size)
{
	uint16_t i=0;
	EA=0;
	IAP_CONTR=ISP_EN+ISP_WAIT_FREQUENCY;
	IAP_CMD=2;	//000000010B	д����
	do{
		IAP_ADDRH=(uint8_t)(eeprom_addr>>8);
		IAP_ADDRL=(uint8_t) eeprom_addr;
		if(i < StaPos)IAP_DATA=0xff;
		else IAP_DATA = *pAddr;			//�����ݵ�ISP_DATA��ֻ�����ݸı�ʱ����������
		ISP_TRIG();
		_nop_();
		if(i < StaPos)i++;
		else pAddr++;
		eeprom_addr++;
	}while(--Size);
	AvoidError();
	EA=1;
}

void Clear_eeprom(uint16_t eeprom_addr)
{
	EA=0;
	IAP_ADDRH=(uint8_t)(eeprom_addr>>8);
	IAP_ADDRL=(uint8_t) eeprom_addr;
	IAP_CONTR=ISP_EN+ISP_WAIT_FREQUENCY;
	IAP_CMD=3;//000000011B	��������
	ISP_TRIG();
	_nop_();
	AvoidError();
	EA=1;
}

