#ifndef _eeprom_CFG_H_
#define _eeprom_CFG_H_

					//页码		//写入目的地址	//读eeprom首地址  //写入数据个数
void Write_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint16_t num);	 //每页256 共8页		 8K byte可寻址
void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num);	 //每页256 共8页		 2K可寻址


#endif
																		   