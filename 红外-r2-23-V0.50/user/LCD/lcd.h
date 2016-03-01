/********************************
 *Һ����������ļ�
 *2015/8/3
 */

#ifndef __LCD_H_
#define __LCD_H_
/*************include************/
#include "..\app_cfg.h"
/********************************/

// //��ʾ�׵�ַ
#define ADDPART1        0x0000		//10KB
#define ADDPART2        0x2580
#define ADDPART3        0x4b00     
#define ADDPART4		 0x7080
#define CGRAMADD        0xc000


void LCD_Clear (uint16_t chPart);		// ����
void Clear_Windows(uint16_t hwXpos, uint8_t chYpos, uint16_t hwLength, uint8_t chWidth, uint16_t hwAddr,bit bMode);	//���ִ�������
void Draw_Scrollbar(uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint16_t hwAddr,bit bType,bit bMode);	 		//��������
void Draw_ArcFrame (uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint8_t chWidth,uint16_t hwAddr,bit bMode);	//�����ξ��ο�

void Show_Logo(uint16_t hwXpos ,uint8_t chYpos, uint16_t hwAddr,bit bMode);	 //��ʾ�ַ���  
void Show_Power(uint16_t hwXpos, uint8_t chYpos,uint16_t hwAddr,PowerType Pic,bit bMode);  //��ʾ��Դ��־
void Show_Char (uint16_t hwXpos,uint8_t chYpos,char chChar,uint16_t hwAddr,bit Mode);	 //��ĸ��ʾ
void Show_String(uint16_t hwXpos, uint8_t chYpos,char *pText,uint16_t hwAddr,bit Mode);	   //��ʾӢ���ַ���
void Show_Chinese (uint16_t hwXpos, uint8_t chYpos, int hwChinese,uint16_t hwAddr, bit Mode); //��ʾ����   �ֿ�����������   �ȶ�ȡ��������8bit���ݺ��ж�ȡ
void Chinese_String(uint16_t hwXpos,uint8_t chYpos,char *pText, uint16_t hwAddr,bit Mode); //��ʾ�����ַ���
uint8_t LCD_Display_String(uint16_t hwXpos ,uint8_t chYpos ,char *pText ,uint16_t hwAddr,bit EMode,bit CMode);	 //��Ӣ�Ļ����ʾ

void Delay1ms();		//@11.0592MHz
void Delay200us();		//@11.0592MHz
void LCDSystem_Init(void);

#endif/*EOF*/