#ifndef _EEPROM_H_
#define _EEPROM_H_

#define EEPROM_Addr1 0Xdc00  //��111���� //�ܹ�122������	   512byte
#define EEPROM_Addr2 0Xde00  //��112����
#define EEPROM_Addr3 0Xe000  //��113����//�ܹ�122������

void Read_EEprom (uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size);
void Write_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size);
void Clear_eeprom(uint16_t eeprom_addr);

#endif
