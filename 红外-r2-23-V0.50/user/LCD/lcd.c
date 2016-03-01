/*****************************
 *Һ����������
 *2015/8/3
 *˵����8080ʱ��
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


//uint8_t Read_Flag (void)				//��״̬��־
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

//void Busy_Check (void)					//æ��־���
//{
//	uint8_t chBusy;
//	do{
//		chBusy = (Read_Flag()&0x40);
//	}while (chBusy);
//}
//
void Write_CMD (uint8_t cmd)   			//дָ��
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
	DB = cmd;//ָ��
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WR = 1;
	CS = 1;
	CD = 0;
	DB = 0xff;
}

void Write_Data(uint8_t dat)			//дָ���������ʾ����
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
	DB = dat;//����	WR = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WR = 1;
	CS = 1;
	CD = 1;
	DB = 0xff;
}

uint8_t Read_Data (void)				//	 ������
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

void LCD_Clear (uint16_t chPart)		// ����
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
	Write_CMD(CSRRIGHT);  //�������
	Write_CMD(MRWITE);//����д��
	while (0 < i){
		i--;
		Write_Data(0x00);
	}
}

void CSR_Locate (uint16_t hwXpos, uint8_t chYpos, uint16_t hwAddr)			 		 //��궨λ(�����ص�λ�ö�λ���λ��)
{
	uint16_t addr;
	if (hwXpos > 320||chYpos > 240)return;
	addr = ((uint16_t)chYpos)*AP+(hwXpos>> 3)+hwAddr;		//	    
	Write_CMD(CSRW);
	Write_Data((uint8_t)addr);
	Write_Data((uint8_t)(addr >> 8));
}

void Clear_Windows(uint16_t hwXpos, uint8_t chYpos, uint16_t hwLength, uint8_t chWidth, uint16_t hwAddr,bit bMode)	//���ִ�������
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

void Draw_Point(uint16_t hwXpos, uint8_t chYpos, uint16_t hwAddr,bit bMode)	 		//����
{
	uint8_t chTemp;
	if (hwXpos > 320||chYpos >240)return; 
	CSR_Locate (hwXpos,chYpos,hwAddr);
	Write_CMD(MREAD);
	chTemp = Read_Data();
	if (bMode)chTemp |= (0x80 >> (hwXpos % 8));
	else chTemp &= (( 0x80 >> (hwXpos % 8))^0xff);
	CSR_Locate(hwXpos,chYpos,hwAddr);//�������ݺ�����������Զ��ƶ�
	Write_CMD(MRWITE);
	Write_Data(chTemp);  	
}

void Draw_Line(uint16_t hwXst , uint8_t chYst , uint16_t hwXed , uint8_t chYed, uint16_t hwAddr,bit bMode)	 		//����
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
		for (y = chYst; y != chYed;y+=YStep)Draw_Point(x,y,hwAddr,bMode); 	//��ֱ��
	}else if (chYst == chYed){		  
		for (x = hwXst; x != hwXed;x+=XStep) Draw_Point(x,y,hwAddr,bMode);  //ˮƽ��
	}else if (dx > dy){			
		for (x = hwXst ; x != hwXed+XStep; x+=XStep){	  					//|k|<1ʱ
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

void Draw_FillRect (uint16_t hwXpos, uint8_t chYpos, uint16_t hwLength,uint8_t chWidth, uint16_t hwAddr ,bMode)	  	//��һ�����ľ���
{
	uint8_t i;
	if (hwXpos+hwLength > 320||chYpos+chWidth >240)return;
	for (i = 0;  chWidth > i; i++){
		Draw_Line (hwXpos,chYpos+i,hwXpos+hwLength,chYpos+i,hwAddr,bMode);
	}
}


void Draw_Scrollbar(uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint16_t hwAddr,bit bType,bit bMode)	 		//��������
{
	if (hwXpos+hwLength > 320||chYpos+8 >240)return;
	if (bType)Draw_FillRect (hwXpos,chYpos,hwLength,8,hwAddr,bMode); //ˮƽ
	else Draw_FillRect (hwXpos,chYpos,8,hwLength,hwAddr,bMode); //��ֱ
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
		case 0: Draw_Point((x+hwXc),(-y+chYc),hwAddr,bMode);		//����
				Draw_Point((y+hwXc),(-x+chYc),hwAddr,bMode);break;
		case 1: Draw_Point((-x+hwXc),(-y+chYc),hwAddr,bMode);		//����
				Draw_Point((-y+hwXc),(-x+chYc),hwAddr,bMode);break;
		case 2:	Draw_Point((x+hwXc),(y+chYc),hwAddr,bMode);			//����
				Draw_Point((y+hwXc),(x+chYc),hwAddr,bMode);break;
		case 3:	Draw_Point((-x+hwXc),(y+chYc),hwAddr,bMode);		//����
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

void Draw_ArcFrame (uint16_t hwXpos,uint8_t chYpos,uint16_t hwLength,uint8_t chWidth,uint16_t hwAddr,bit bMode)	//�����ξ��ο�
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
 *����������ʾ�ϳɷ�ʽ
 *���룺
 �ϳ�����λ��0�����غϳ� 1�����غϳ�
 ��ʾ1�����ԣ�0���ı� 1��ͼ��
 ��ʾ3������
 ��ʾ�ϳɷ�ʽ��
 00:L1+L2+L3
 01:(L1��L2��+L3
 10:(L1L2)+L3
 11:L3>L2>L1���ȵ���
 *���أ���
 */
void oylay (uint8_t bOV, uint8_t bPart1Pro, uint8_t bPart3Pro, uint8_t chMode)	   	 //�����ϲ�
{
	uint8_t chTemp = 0;
	chTemp =chMode|(bPart1Pro << 2)
	   		  |(bPart3Pro << 3)
			  |(bOV << 4);
	Write_CMD(OYLAY);
	Write_Data(chTemp);
}

/*************************************************************************
 *����������ʾ�ַ���(ͼ�η�ʽ������ȡģ)Ҳ���Ի�ͼ
 *���룺��ʾλ�á����������顢��ʾ���׵�ַ���Ƿ�����ʾ��1������0��������
 *���أ���
 */

			//���������ַ //������������//���ҵĹؼ���//��Ӣ�ı�־λ
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

void Show_Char (uint16_t hwXpos,uint8_t chYpos,char chChar,uint16_t hwAddr,bit Mode)	 //��ĸ��ʾ
{
	bit bFlag;
	uint8_t  *pChr,x,y,i,j,*pCode,num;
	int addr;
	bFlag=1;x=1;y=16;pChr=ASCII8_16 ;pCode=ASCA_small;num=Char_NUM;
	if(REVERSE == Mode) bFlag=0;
	if ((chChar & 0x80) == 0)
	{
		if((47 < chChar)&(58 > chChar)) pChr += (chChar - 48)*x*y;		   //0x20  32  ASCII���ֿ��д�ŵ���0x20���ո��ַ���֮����ַ�  
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

void Show_String(uint16_t hwXpos, uint8_t chYpos,char *pText,uint16_t hwAddr,bit Mode)	   //��ʾӢ���ַ���
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
		if (0 == (chTemp & 0x80))	 //	 �ж��Ƿ�ΪASCII��
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

void Show_Chinese (uint16_t hwXpos, uint8_t chYpos, int hwChinese,uint16_t hwAddr, bit Mode)  //��ʾ����   �ֿ�����������   �ȶ�ȡ��������8bit���ݺ��ж�ȡ
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

void Chinese_String(uint16_t hwXpos,uint8_t chYpos,char *pText, uint16_t hwAddr,bit Mode) //��ʾ�����ַ���
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
		if ((dat.c[0] & 0x80) != 0)			//���ֱ���Ϊ16λ�Ҷ���ASCII��֮�������ȱȽ��жϳ����ַ��Ƿ��Ǻ���
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


void Show_Logo(uint16_t hwXpos ,uint8_t chYpos, uint16_t hwAddr,bit bMode)	 //��ʾ�ַ���  
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
 *����������ʾ��Դ��16*32��
 *���룺��ʾλ�ã���ʾ����ַ�����OR�����磬ģʽ
 *���أ���
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
void LCD_ON(uint8_t chCsrState, uint8_t chDisState1, uint8_t chDisState24, uint8_t chDisState3)		//����ʾ��
{
	uint8_t chTemp = 0;
	chTemp = chCsrState|(chDisState1 << 2)
		 |(chDisState24 << 4)|(chDisState3 << 6);
	Write_CMD(DISPLAYON);
	Write_Data(chTemp);
//	Write_Data(0x54);//��ʾ���棬����ʾ���
}

void LCD_OFF(void)	  //����ʾ��
{
	Write_CMD(DISPLAYOFF);
}

void Scroll_Set(uint16_t hwSad1,uint16_t hwSad2,uint16_t hwSad3,uint16_t hwSad4,uint8_t chSl1,uint8_t chSl2)	//	��ʾ������
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

void CSR_Form(uint8_t chForm, uint8_t chLength, uint8_t chHeight)	 //�����״����
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
	for (i = 0 ;8 > i; i++) Write_Data(system[i]);	//��ʼ������
	Scroll_Set(ADDPART1,ADDPART2,ADDPART3,ADDPART4,SL1,SL2);	//��ʾ������
	Write_CMD(HDOTSCR);//��λ������,���ƶ�
	Write_Data(0x00);
	LCD_OFF();
	CSR_Form(1,7,7);//�������
	oylay(1,1,1,0);//��ʾ�ϳ�����
	LCD_ON(0,1,0,0);
	Write_CMD(CSRRIGHT);//�������
	LCD_Clear(ALLPART);
	CSR_Locate (0,0,ADDPART1);			 	
 }