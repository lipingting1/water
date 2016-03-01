/**********************************
 *Һ������ͷ�ļ�
 *2015/8/3
 */

#ifndef __LCD_CFG_H_
#define __LCD_CFG_H_

/*************include*************/

#include "..\app_cfg.h"
#include ".\font.h"


 /***********************************/
 /*************define****************/
#define ABS(a)        ((0 < (a))?(a):(-a))
#define BIGGER(a,b)	  (a>b?a:b)
#define DB  P2 //���ݿ�

 //ָ���
 //ϵͳ����
 #define SYSTEMSET       0x40//��ʼ�����ã�8������
 #define SLEEPIN         0x53//����ģʽ����
 //��ʾ����
 #define DISPLAYON       0x59//��ʾ����1������		0101 1001
 #define DISPLAYOFF      0x58//��ʾ�أ�1������		0101 1000

 #define SCROLL          0x44//��ʾ�����ã�10������
 #define CGRAMADR        0x5c//CGRAM��ַ���ã�2������

  //���Ʋ���
 #define CSRFORM         0x5d//�����״���ã�2������
 #define CSRW            0x46//���ù���ַ��2������
 #define CSRR            0x47//��������ַ��2������
 #define CSRRIGHT        0x4c//�������
 #define CSRLIFT         0x4d//�������
 #define CSRUP           0x4e//�������
 #define CSRDOWN         0x4f//�������

 #define HDOTSCR         0x5a//��λ�����ã�1������
 #define OYLAY           0x5b//��ʾ�ϳɷ�ʽ��1������
 //�洢����
 #define MRWITE          0x42//��ʾ����д��
 #define MREAD           0x43//��ʾ���ݶ���
 //��ʾ�׵�ַ
 #define ADDPART1        0x0000		//10KB
 #define ADDPART2        0x2580
 #define ADDPART3        0x7000     
 #define ADDPART4		 0x8000
 #define CGRAMADD        0xc000
 //��ʾ����
 #define ALLPART		 0
 #define PART1           1
 #define PART2           2
 #define PART3           3
 #define PART4           4
 //��ʾ״̬
 #define SHUT            0 
 #define SHOW            1
 #define BLINK_32        2
 #define BLINK_64        3
 //
 #define AP              0x28 //��ʾ����ȣ����ĸ���ʾ����ͳһ��
// #define CR1			 0x27//��Ч��ʾ���
 #define LF              0xf0//����������Ҫ���Ƶ�ɨ������
 #define SL1             LF//��ʾ1��3�������Ƶ�����
 #define SL2             LF//��ʾ2��4�������Ƶ�����
 #define RAM1			 9600//��ʾ1���Դ�
 #define RAM2            9600
 #define ALLRAM          32768//32KB

 #define HZ_NUM          130
 #define Char_NUM        41

 /*******************************************/
 sbit WR = P7^1;
 sbit RD = P7^2;
 sbit CS = P7^3;
 sbit CD = P4^2;
 sbit RST= P4^4;

uint8_t code system[8] = {0x30,0x87,0x0f,0x27,0x30,0xef,0x28,0x00};	//ϵͳ���ò���

extern void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num);	 //ÿҳ256 ��8ҳ		 2K��Ѱַ


#endif /*EOF*/
