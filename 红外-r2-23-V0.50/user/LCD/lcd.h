/********************************
 *液晶输出配置文件
 *2015/8/3
 */

#ifndef __LCD_H_
#define __LCD_H_
/*************include************/
#include "..\app_cfg.h"
/********************************/

// //显示首地址
#define ADDPART1        0x0000		//10KB
#define ADDPART2        0x2580
#define ADDPART3        0x4b00     
#define ADDPART4		 0x7080
#define CGRAMADD        0xc000


void LCD_Clear (uint16_t chPart);		// 清屏
void Clear_Windows(uint16_t hwXpos, uint8_t chYpos, uint16_t hwLength, uint8_t chWidth, uint16_t hwAddr,bit bMode);	//部分窗口清屏
void Draw_Scrollbar(uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint16_t hwAddr,bit bType,bit bMode);	 		//画滚动条
void Draw_ArcFrame (uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint8_t chWidth,uint16_t hwAddr,bit bMode);	//画弧形矩形框

void Show_Logo(uint16_t hwXpos ,uint8_t chYpos, uint16_t hwAddr,bit bMode);	 //显示字符串  
void Show_Power(uint16_t hwXpos, uint8_t chYpos,uint16_t hwAddr,PowerType Pic,bit bMode);  //显示电源标志
void Show_Char (uint16_t hwXpos,uint8_t chYpos,char chChar,uint16_t hwAddr,bit Mode);	 //字母显示
void Show_String(uint16_t hwXpos, uint8_t chYpos,char *pText,uint16_t hwAddr,bit Mode);	   //显示英文字符串
void Show_Chinese (uint16_t hwXpos, uint8_t chYpos, int hwChinese,uint16_t hwAddr, bit Mode); //显示汉字   字库行优先排列   先读取横排两个8bit数据后换行读取
void Chinese_String(uint16_t hwXpos,uint8_t chYpos,char *pText, uint16_t hwAddr,bit Mode); //显示汉字字符串
uint8_t LCD_Display_String(uint16_t hwXpos ,uint8_t chYpos ,char *pText ,uint16_t hwAddr,bit EMode,bit CMode);	 //中英文混合显示

void Delay1ms();		//@11.0592MHz
void Delay200us();		//@11.0592MHz
void LCDSystem_Init(void);

#endif/*EOF*/