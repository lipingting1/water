#ifndef _APP_CFG_H
#define _APP_CFG_H

					//ҳ��		//д��Ŀ�ĵ�ַ	//��eeprom�׵�ַ  //д�����ݸ���
void Write_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint16_t num);	 //ÿҳ256 ��8ҳ		 8K byte��Ѱַ
void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num);	 //ÿҳ256 ��8ҳ		 2K��Ѱַ


#endif
																		   