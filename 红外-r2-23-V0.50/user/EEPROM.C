#include <STC15F2K60S2.H>

#include "..\app_cfg.h"

void AvoidError(void)
{
IAP_ADDRH=0xff;
IAP_ADDRL=0xff;
ISP_CONTR = 0;			//½ûÖ¹ISP/IAP²Ù×÷
ISP_CMD   = 0;			//È¥³ýISP/IAPÃüÁî
ISP_TRIG  = 0;
}


uint8_t Read_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint8_t Size)
{
EA=0;
IAP_CMD=0X01;//000000001B	¶ÁÃüÁî
do{
IAP_ADDRH=eeprom_addr/256;
IAP_ADDRL=eeprom_addr%256;
IAP_TRIG=();
*pAddr=IAP_DATA;
eeprom_addr++;
}while(Size--);
_nop_();
EA=1;
}



void Write_EEprom(uint16_t eeprom_addr,uint16_t *pAddr,uint_8 Size)
{
EA=0;
IAP_CMD=0X02;	//000000010B	Ð´ÃüÁî
do{
IAP_ADDRH=eeprom_addr/256;
IAP_ADDRL=eeprom_addr%256;
IAP_DATA=*pAddr;
IAP_TRIG();
pAddr++;
eeprom_addr++;
}while(Size--)
}



void Clear_eeprom(uint16_t eeprom_addr)
{
IAP_ADDRH=eeprom_addr/256;
IAP_ADDRL=eeprom_addr%256;
IAP_CMD=0X03;//000000011B	²Á³ýÃüÁî
IAP_TRIG();
}

