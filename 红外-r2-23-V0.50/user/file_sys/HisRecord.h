#ifndef _HisRecord_H_
#define _HisRecord_H_

void HistoryPro(uint8_t chKey);										//��ʷ��¼����			0X50
void HistoryDel(uint8_t chKey,uint8_t NowPageAddr,uint8_t Ptr);		//ɾ����ʷ��¼����		0X51
void SaveValue(uint8_t chMode);	   									//��������������
void His_Record(void);

#endif