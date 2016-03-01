#ifndef _EEPROM_H_
#define _EEPROM_H_

#define EEPROM_Addr1 0Xdc00  //第111扇区 //总共122个扇区	   512byte
#define EEPROM_Addr2 0Xde00  //第112扇区
#define EEPROM_Addr3 0Xe000  //第113扇区//总共122个扇区

void Read_EEprom (uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size);
void Write_EEprom(uint16_t eeprom_addr,uint8_t *pAddr,uint16_t StaPos,uint16_t Size);
void Clear_eeprom(uint16_t eeprom_addr);

#endif
