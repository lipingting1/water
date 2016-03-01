#ifndef _HisRecord_H_
#define _HisRecord_H_

void HistoryPro(uint8_t chKey);										//历史记录处理			0X50
void HistoryDel(uint8_t chKey,uint8_t NowPageAddr,uint8_t Ptr);		//删除历史记录处理		0X51
void SaveValue(uint8_t chMode);	   									//保存测量结果处理
void His_Record(void);

#endif