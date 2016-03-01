/*****************************
 *液晶驱动函数
 *2015/8/3
 *说明：8080时序
 */

/****************include***********/
#include ".\lcd_cfg.h"
/**********************************/

void Delay1ms()		//@11.0592MHz
{																																
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}
void Delay200us()		//@11.0592MHz
{
	unsigned char i, j;

	i = 3;
	j = 35;
	do
	{
		while (--j);
	} while (--i);
}


//uint8_t Read_Flag (void)				//读状态标志
//{
//	uint8_t temp = 0;
//	DB = 0xff;
//	CS = 0;
//	CD = 0;
//	WR = 1;
//	RD = 0;
//	_nop_();
//	_nop_();
//	temp =DB;
//	RD = 1;
//	CS = 1;
//	CD = 1;
//	return temp;
//}

//void Busy_Check (void)					//忙标志检测
//{
//	uint8_t chBusy;
//	do{
//		chBusy = (Read_Flag()&0x40);
//	}while (chBusy);
//}
//
void Write_CMD (uint8_t cmd)   			//写指令
{
//	Busy_Check();
	CS = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CD = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	DB = 0xff;
	RD = 1;
	WR = 0;
	DB = cmd;//指令
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WR = 1;
	CS = 1;
	CD = 0;
	DB = 0xff;
}

void Write_Data(uint8_t dat)			//写指令参数和显示数据
{
//	Busy_Check();
	CS = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CD = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	DB =0xff;
	RD = 1;
	WR = 0;
	DB = dat;//数据	WR = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WR = 1;
	CS = 1;
	CD = 1;
	DB = 0xff;
}

uint8_t Read_Data (void)				//	 读数据
{
	uint8_t temp = 0;
//	Busy_Check();
	CS = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CD = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WR = 1;
	RD = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	temp = DB;
	RD = 1;
	CS = 1;
	CD = 0;
	return temp;
}

void LCD_Clear (uint16_t chPart)		// 清屏
{
	uint16_t i;
	Write_CMD(CSRW);
	switch (chPart){
	case PART1: i = RAM1;
				Write_Data((uint8_t)ADDPART1);
				Write_Data((ADDPART1 >> 8));break;
	case PART2: i = RAM2;
				Write_Data((uint8_t)ADDPART2);
				Write_Data((ADDPART2 >> 8));break;
	case PART3: i = RAM1;
				Write_Data((uint8_t)ADDPART3);
				Write_Data((ADDPART3 >> 8));break;
	case PART4: i = RAM2;
				Write_Data((uint8_t)ADDPART4);
				Write_Data((ADDPART4 >> 8));break;
	case ALLPART: i = 32768;
				Write_Data(0x00);
				Write_Data(0x00);break;		
	default: i = 0;break;
	}
	Write_CMD(CSRRIGHT);  //光标右移
	Write_CMD(MRWITE);//数据写入
	while (0 < i){
		i--;
		Write_Data(0x00);
	}
}

void CSR_Locate (uint16_t hwXpos, uint8_t chYpos, uint16_t hwAddr)			 		 //光标定位(按像素点位置定位光标位置)
{
	uint16_t addr;
	if (hwXpos > 320||chYpos > 240)return;
	addr = ((uint16_t)chYpos)*AP+(hwXpos>> 3)+hwAddr;		//	    
	Write_CMD(CSRW);
	Write_Data((uint8_t)addr);
	Write_Data((uint8_t)(addr >> 8));
}

void Clear_Windows(uint16_t hwXpos, uint8_t chYpos, uint16_t hwLength, uint8_t chWidth, uint16_t hwAddr,bit bMode)	//部分窗口清屏
{
	uint16_t i,x;
	uint8_t	j;
	if (hwXpos+hwLength > 320||chYpos+chWidth > 240)return;
	if (0 == hwLength%8)x = hwLength/8;
	else x = hwLength/8+1;
	Write_CMD(CSRRIGHT);
	for (j = 0;chWidth > j;j++){
		CSR_Locate (hwXpos,chYpos+j,hwAddr);
		Write_CMD(MRWITE);
		for (i = 0;x > i ;i++)
		if (bMode)Write_Data(0x00);
		else Write_Data(0xff);
	}
}

void Draw_Point(uint16_t hwXpos, uint8_t chYpos, uint16_t hwAddr,bit bMode)	 		//画点
{
	uint8_t chTemp;
	if (hwXpos > 320||chYpos >240)return; 
	CSR_Locate (hwXpos,chYpos,hwAddr);
	Write_CMD(MREAD);
	chTemp = Read_Data();
	if (bMode)chTemp |= (0x80 >> (hwXpos % 8));
	else chTemp &= (( 0x80 >> (hwXpos % 8))^0xff);
	CSR_Locate(hwXpos,chYpos,hwAddr);//读出数据后光标根据设置自动移动
	Write_CMD(MRWITE);
	Write_Data(chTemp);  	
}

void Draw_Line(uint16_t hwXst , uint8_t chYst , uint16_t hwXed , uint8_t chYed, uint16_t hwAddr,bit bMode)	 		//画线
{
	char  XStep,YStep,e;
	int dy,dx;		//
	uint8_t y;
	uint16_t x;
	if (hwXed > 320||chYed >240)return;
	YStep = (chYed >= chYst)?1:-1;
	XStep = (hwXed >= hwXst)?1:-1;
	dy = chYed - chYst;
	dx = hwXed - hwXst;
	dy = ABS(dy);
	y = chYst;
	dx = ABS(dx);
	x = hwXst;
	e = 0;
	if (hwXst == hwXed){					  
		for (y = chYst; y != chYed;y+=YStep)Draw_Point(x,y,hwAddr,bMode); 	//垂直线
	}else if (chYst == chYed){		  
		for (x = hwXst; x != hwXed;x+=XStep) Draw_Point(x,y,hwAddr,bMode);  //水平线
	}else if (dx > dy){			
		for (x = hwXst ; x != hwXed+XStep; x+=XStep){	  					//|k|<1时
			Draw_Point (x,y,hwAddr,bMode);
			e += dy; 
			if ((e << 1) > dx){
				y +=YStep; 
				e -=dx;
			}
		}	
	}else{ 
		for (y = chYst ; y != chYed+YStep; y+=YStep){						//|k|>1
			Draw_Point (x,y,hwAddr,bMode);
			e += dx;
			if ((e << 1) > dy){
				x += XStep;
				e -= dy;
			}
		}
	}
}

void Draw_FillRect (uint16_t hwXpos, uint8_t chYpos, uint16_t hwLength,uint8_t chWidth, uint16_t hwAddr ,bMode)	  	//画一个填充的矩形
{
	uint8_t i;
	if (hwXpos+hwLength > 320||chYpos+chWidth >240)return;
	for (i = 0;  chWidth > i; i++){
		Draw_Line (hwXpos,chYpos+i,hwXpos+hwLength,chYpos+i,hwAddr,bMode);
	}
}


void Draw_Scrollbar(uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint16_t hwAddr,bit bType,bit bMode)	 		//画滚动条
{
	if (hwXpos+hwLength > 320||chYpos+8 >240)return;
	if (bType)Draw_FillRect (hwXpos,chYpos,hwLength,8,hwAddr,bMode); //水平
	else Draw_FillRect (hwXpos,chYpos,8,hwLength,hwAddr,bMode); //垂直
}

void Draw_Arc(uint16_t hwXc,uint8_t chYc,uint16_t hwR,uint16_t hwAddr,uint8_t chDir,bit bMode)
{
	uint16_t x,y;
	int p;
	x = hwR;
	y = 0;
	p = 3-2*hwR;
	while (x >y){
		switch (chDir){
		case 0: Draw_Point((x+hwXc),(-y+chYc),hwAddr,bMode);		//上右
				Draw_Point((y+hwXc),(-x+chYc),hwAddr,bMode);break;
		case 1: Draw_Point((-x+hwXc),(-y+chYc),hwAddr,bMode);		//上左
				Draw_Point((-y+hwXc),(-x+chYc),hwAddr,bMode);break;
		case 2:	Draw_Point((x+hwXc),(y+chYc),hwAddr,bMode);			//下右
				Draw_Point((y+hwXc),(x+chYc),hwAddr,bMode);break;
		case 3:	Draw_Point((-x+hwXc),(y+chYc),hwAddr,bMode);		//下左
				Draw_Point((-y+hwXc),(x+chYc),hwAddr,bMode);break;	 
		}
		if (p < 0){
			p = p+4*y+6;
			y++;
		}else{
			p = p+4*(y-x)+6;
			x--;
			y++;
		}
	}	
}

void Draw_ArcFrame (uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint8_t chWidth,uint16_t hwAddr,bit bMode)	//画弧形矩形框
{
	if (hwXpos+hwLength > 320||chYpos+chWidth >240)return;
	Draw_Arc(hwXpos+8,chYpos+8,8,hwAddr,1,bMode);
	Draw_Line(hwXpos+8,chYpos,hwXpos+hwLength-8,chYpos,hwAddr,bMode);
	Draw_Arc(hwXpos+hwLength-8,chYpos+8,8,hwAddr,0,bMode);
	Draw_Line(hwXpos+hwLength,chYpos+8,hwXpos+hwLength,chYpos+chWidth-8,hwAddr,bMode);
	Draw_Arc(hwXpos+hwLength-8,chYpos+chWidth-8,8,hwAddr,2,bMode);
	Draw_Line(hwXpos+hwLength-8,chYpos+chWidth,hwXpos+8,chYpos+chWidth,hwAddr,bMode);
	Draw_Arc(hwXpos+8,chYpos+chWidth-8,8,hwAddr,3,bMode);
	Draw_Line(hwXpos,chYpos+chWidth-8,hwXpos,chYpos+8,hwAddr,bMode);
}

/***************************************
//
/***************************************
 *函数名：显示合成方式
 *输入：
 合成设置位：0：二重合成 1：三重合成
 显示1区属性：0：文本 1：图形
 显示3区属性
 显示合成方式：
 00:L1+L2+L3
 01:(L1⊕L2）+L3
 10:(L1L2)+L3
 11:L3>L2>L1优先叠加
 *返回：无
 */
void oylay (uint8_t bOV, uint8_t bPart1Pro, uint8_t bPart3Pro, uint8_t chMode)	   	 //分区合并
{
	uint8_t chTemp = 0;
	chTemp =chMode|(bPart1Pro << 2)
	   		  |(bPart3Pro << 3)
			  |(bOV << 4);
	Write_CMD(OYLAY);
	Write_Data(chTemp);
}

/*************************************************************************
 *函数名：显示字符串(图形方式、横向取模)也可以画图
 *输入：显示位置、长、宽、数组、显示区首地址、是否负向显示（1：负向0：正常）
 *返回：无
 */

			//查找数组地址 //数据总数据量//查找的关键词//中英文标志位
int search(uint16_t *array ,uint8_t n, uint16_t key, bit bType)
{
	uint16_t dat,i;
	for (i = 0;n > i; i++){
		if(bType == 0){
			dat = *array;
			array++;
		}else{
			dat = *((uint8_t *)array);
			((uint8_t *)array)++;
		}
		if (dat==key) return i;		
	}
	return -1;
}

void Show_Char (uint16_t hwXpos,uint8_t chYpos,char chChar,uint16_t hwAddr,bit Mode)	 //字母显示
{
	bit bFlag;
	uint8_t  *pChr,x,y,i,j,*pCode,num;
	int addr;
	bFlag=1;x=1;y=16;pChr=ASCII8_16 ;pCode=ASCA_small;num=Char_NUM;
	if(REVERSE == Mode) bFlag=0;
	if ((chChar & 0x80) == 0)
	{
		if((47 < chChar)&(58 > chChar)) pChr += (chChar - 48)*x*y;		   //0x20  32  ASCII码字库中存放的是0x20（空格字符）之后的字符  
		else{
			addr=search((uint16_t *)pCode,num,chChar,1);
			if(-1 == addr)return;
			pChr += (addr+10)*x*y;
		}
		Write_CMD(CSRRIGHT);
		for (i =0 ;y > i;i++){
			CSR_Locate(hwXpos,chYpos+i,hwAddr);
			Write_CMD(MRWITE);
			for (j=0;x >j;j++){
				if (bFlag) Write_Data(*pChr);
				else Write_Data(~*pChr);
				pChr++;
			}
		}
	}	
}

void Show_String(uint16_t hwXpos, uint8_t chYpos,char *pText,uint16_t hwAddr,bit Mode)	   //显示英文字符串
{
	char chTemp;
	char j,x,y;
	uint16_t i;
	i = hwXpos;j = chYpos;x=1;y=16;
	while (1)
	{
		chTemp = *pText++;
		if (i > 320-(x<<3)){
			i = 0;
			j +=y;
		}
		if (0 == (chTemp & 0x80))	 //	 判断是否为ASCII码
		{
			if (0x0d == chTemp )		//ENTER
			{
				j += y;
				i = hwXpos;
				continue;
			}
			Show_Char(i ,j,chTemp,hwAddr,Mode);
			i += (x<<3);
			if (i > 320-(x<<3)){
				i = hwXpos;
				j +=y; 
			}
		}
		else break;
		if (*pText == 0)break;
	}							
}

void Show_Chinese (uint16_t hwXpos, uint8_t chYpos, int hwChinese,uint16_t hwAddr, bit Mode)  //显示汉字   字库行优先排列   先读取横排两个8bit数据后换行读取
{
	bit bFlag;
	uint16_t i ,j,x,y,*pCode;
	uint8_t *pStr;
	uint8_t addr;
	pCode=HZ;
	addr = search (pCode,HZ_NUM,hwChinese,0);
	bFlag=1;
	if (-1 == addr)return;
	if(Mode == REVERSE) bFlag=0;
	x=2;y=16;pStr=HZ_Table16_16+addr*x*y;
	Write_CMD(CSRRIGHT);
	for (i = 0;i < y; i++){
		CSR_Locate(hwXpos,chYpos+i,hwAddr);
		Write_CMD(MRWITE);
		for (j = 0;j < x;j++){
			if (bFlag)Write_Data(*pStr);
			else Write_Data(~*pStr);
			pStr++;
		}		
	}
}

void Chinese_String(uint16_t hwXpos,uint8_t chYpos,char *pText, uint16_t hwAddr,bit Mode) //显示汉字字符串
{
	union{
	    char c[2];
	    int i;
  	}dat;
	uint8_t j,x,y;
	uint16_t i;
	i = hwXpos;j = chYpos;x=2;y=16;
	while (1)
	{
		dat.c[0] = *pText++;
		if (i > 320-(x<<3)){
			i = 0;
			j +=y ;
		}
		if ((dat.c[0] & 0x80) != 0)			//汉字编码为16位且都是ASCII码之后，所以先比较判断出该字符是否是汉字
		{
			dat.c[1] = *pText++;
			Show_Chinese(i ,j ,dat.i ,hwAddr,Mode);
			i += (x<<3);
			if (i > 320-(x<<3)){
			i = hwXpos;
			j +=y; 
			}
		}
		else break;
		if (*pText == 0)break;
	}
}

uint8_t LCD_Display_String(uint16_t hwXpos ,uint8_t chYpos ,char *pText ,uint16_t hwAddr,bit EMode,bit CMode)
{ 
	uint8_t len=0;
	union
	{
		uint8_t c[2];
		uint16_t i;
	}dat;
	uint8_t j,x1,y1,x2,y2;
	uint16_t i;
	i=hwXpos;j=chYpos;
	x1=1;y1=16;x2=2;y2=16;
	while (1)
	{
		dat.c[0] = *pText++;
//		if (i > 320-(x1<<3)||i >320-(x2<<3)){
//			i = 0;
//			j += BIGGER(y1,y2);
//		}
		if ((dat.c[0] & 0x80) != 0)
		{
			dat.c[1] = *pText++;
		}
		else
		{
			if (dat.c[0]==0x0d)
			{
				j+= BIGGER(y1,y2)+4;
				i = hwXpos;
				continue;
			}
		}
		if ((dat.c[0] & 0x80) != 0)
		{
			Show_Chinese(i ,j ,dat.i ,hwAddr,CMode);
			i += (x2<<3);
			if (i > 320-(x2<<3)){
				i = hwXpos;
				j +=y2; 
			}
			len += 2;
		}
		else
		{
			Show_Char(i ,j,dat.c[0],hwAddr,EMode);
			i += (x1<<3);
			if (i > 320-(x1<<3)){
				i = hwXpos;
				j +=y1; 
			}
			len++;
		}
		if (*pText == 0) break;
	}
	return len;
}


void Show_Logo(uint16_t hwXpos ,uint8_t chYpos, uint16_t hwAddr,bit bMode)	 //显示字符串  
{

	extern uint8_t DisBuffer[12];
	uint8_t i,j,Picture;
	uint16_t chNum=5888;
	if (hwXpos > 80||chYpos > 170)return;
	Write_CMD(CSRRIGHT);
	for (i = 0; i < 63; i++){
		CSR_Locate(hwXpos,chYpos+i,hwAddr);
		Write_CMD(MRWITE);
		for (j = 0;j < 30;j++){		  //30=240/8
			Read_EEprom(chNum/256,&Picture,chNum%256,1);//			 %256	  /256	*Ptr
			if (bMode) Write_Data(Picture);											   
			else Write_Data(~Picture);
			chNum++;
		}
	}
}

/**************************************
 *函数名：显示电源（16*32）
 *输入：显示位置，显示区地址，电池OR交流电，模式
 *返回：无
 */
void Show_Power(uint16_t hwXpos, uint8_t chYpos,uint16_t hwAddr,PowerType Pic,bit bMode)
{
	Show_String(hwXpos,chYpos,"{",ADDPART1,NORMAL);
	switch (Pic){
		case BAT1:	Draw_FillRect (hwXpos+1,chYpos+5,6,1,hwAddr,bMode);
		case BAT2:	Draw_FillRect (hwXpos+1,chYpos+8,6,1,hwAddr,bMode);
		case BAT3:	Draw_FillRect (hwXpos+1,chYpos+11,6,1,hwAddr,bMode);
		case BAT4:	break;
		case AC_POW:Show_String(hwXpos,chYpos,"}",ADDPART1,NORMAL);break;
		default:break;
	}
}
void LCD_ON(uint8_t chCsrState, uint8_t chDisState1, uint8_t chDisState24, uint8_t chDisState3)		//开显示屏
{
	uint8_t chTemp = 0;
	chTemp = chCsrState|(chDisState1 << 2)
		 |(chDisState24 << 4)|(chDisState3 << 6);
	Write_CMD(DISPLAYON);
	Write_Data(chTemp);
//	Write_Data(0x54);//显示画面，不显示光标
}

void LCD_OFF(void)	  //关显示屏
{
	Write_CMD(DISPLAYOFF);
}

void Scroll_Set(uint16_t hwSad1,uint16_t hwSad2,uint16_t hwSad3,uint16_t hwSad4,uint8_t chSl1,uint8_t chSl2)	//	显示域设置
{
	Write_CMD(SCROLL);
	Write_Data((uint8_t)hwSad1);
	Write_Data((uint8_t)(hwSad1 >> 8));
	Write_Data(chSl1); 
	Write_Data((uint8_t)hwSad2);
	Write_Data((uint8_t)(hwSad2 >> 8));
	Write_Data(chSl2); 
	Write_Data((uint8_t)hwSad3);
	Write_Data((uint8_t)(hwSad3 >> 8));
	Write_Data((uint8_t)hwSad4);
	Write_Data((uint8_t)(hwSad4 >> 8));	
}

void CSR_Form(uint8_t chForm, uint8_t chLength, uint8_t chHeight)	 //光标形状设置
{
	Write_CMD(CSRFORM);
	Write_Data(chLength);
	Write_Data((uint8_t)((chForm << 7)|chHeight));	
}

void LCDSystem_Init(void)
 {
	uint8_t i;
	P2M0 = 0x00;
	P2M1 = 0x00;
	P4M0 &= 0xFB;
	P4M1 &= 0xFB;
	BackLight=0;
	RST = 0;
	Delay200us();
	CD = 0;
	WR = 1;
	RD = 1;
	CS = 1;
	RST = 1;
	Delay1ms();
	Write_CMD(SYSTEMSET);
	for (i = 0 ;8 > i; i++) Write_Data(system[i]);	//初始化设置
	Scroll_Set(ADDPART1,ADDPART2,ADDPART3,ADDPART4,SL1,SL2);	//显示域设置
	Write_CMD(HDOTSCR);//点位移设置,不移动
	Write_Data(0x00);
	LCD_OFF();
	CSR_Form(1,7,7);//光标设置
	oylay(1,1,1,0);//显示合成设置
	LCD_ON(0,1,0,0);
	Write_CMD(CSRRIGHT);//光标右移
	LCD_Clear(ALLPART);
	CSR_Locate (0,0,ADDPART1);			 	
 }