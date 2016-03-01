#include "app_cfg.h"

void AvoidError(void)
{
	IAP_ADDRH=0xff;			 //地址放置于非FLASH位置 避免误操作
	IAP_ADDRL=0xff;
	IAP_CONTR = 0;			//禁止ISP/IAP操作
	IAP_CMD   = 0;			//待机
	IAP_TRIG  = 0;
}

//增加一个开始读取位置																
void Read_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size)
{
	uint16_t i=0;
	EA=0;
	IAP_CMD = 1;  //000000001B	读命令
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

//增加一个开始写的位置
void Write_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size)
{
	uint16_t i=0;
	EA=0;
	IAP_CONTR=ISP_EN+ISP_WAIT_FREQUENCY;
	IAP_CMD=2;	//000000010B	写命令
	do{
		IAP_ADDRH=(uint8_t)(eeprom_addr>>8);
		IAP_ADDRL=(uint8_t) eeprom_addr;
		if(i < StaPos)IAP_DATA=0xff;
		else IAP_DATA = *pAddr;			//送数据到ISP_DATA，只有数据改变时才需重新送
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
	IAP_CMD=3;//000000011B	擦除命令
	ISP_TRIG();
	_nop_();
	AvoidError();
	EA=1;
}

