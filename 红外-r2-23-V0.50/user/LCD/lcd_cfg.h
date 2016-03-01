/**********************************
 *液晶配置头文件
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
#define DB  P2 //数据口

 //指令表
 //系统控制
 #define SYSTEMSET       0x40//初始化设置，8个参数
 #define SLEEPIN         0x53//休闲模式设置
 //显示操作
 #define DISPLAYON       0x59//显示开，1个参数		0101 1001
 #define DISPLAYOFF      0x58//显示关，1个参数		0101 1000

 #define SCROLL          0x44//显示域设置，10个参数
 #define CGRAMADR        0x5c//CGRAM首址设置，2个参数

  //绘制操作
 #define CSRFORM         0x5d//光标形状设置，2个参数
 #define CSRW            0x46//设置光标地址，2个参数
 #define CSRR            0x47//读出光标地址，2个参数
 #define CSRRIGHT        0x4c//光标右移
 #define CSRLIFT         0x4d//光标左移
 #define CSRUP           0x4e//光标上移
 #define CSRDOWN         0x4f//光标下移

 #define HDOTSCR         0x5a//点位移设置，1个参数
 #define OYLAY           0x5b//显示合成方式，1个参数
 //存储操作
 #define MRWITE          0x42//显示数据写入
 #define MREAD           0x43//显示数据读出
 //显示首地址
 #define ADDPART1        0x0000		//10KB
 #define ADDPART2        0x2580
 #define ADDPART3        0x7000     
 #define ADDPART4		 0x8000
 #define CGRAMADD        0xc000
 //显示部分
 #define ALLPART		 0
 #define PART1           1
 #define PART2           2
 #define PART3           3
 #define PART4           4
 //显示状态
 #define SHUT            0 
 #define SHOW            1
 #define BLINK_32        2
 #define BLINK_64        3
 //
 #define AP              0x28 //显示区宽度，对四个显示区是统一的
// #define CR1			 0x27//有效显示宽度
 #define LF              0xf0//控制器所需要控制的扫描行数
 #define SL1             LF//显示1、3区所控制的行数
 #define SL2             LF//显示2、4区所控制的行数
 #define RAM1			 9600//显示1区显存
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

uint8_t code system[8] = {0x30,0x87,0x0f,0x27,0x30,0xef,0x28,0x00};	//系统设置参数

extern void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num);	 //每页256 共8页		 2K可寻址


#endif /*EOF*/
