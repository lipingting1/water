#include "app_cfg.h"

extern Sys_Para SystemPara;
extern bit bConMea,bShowMea;	//bConMea标记上位机控制测量	  bShowMea显示测量状态位
extern uint8_t DisBuffer[12];
extern uint8_t code Kind[4][5];

bit WR_En,bDataRecStar,bCRC,bOveJua=0;
uint8_t DataBagBuff[40];
uint8_t chRecPtr=0,chSendPtr=0;  //接收和发送指针
uint8_t chRecLen=0,chSendLen,chDataLen,chTraDatLen,CRC_Data=0;

//#define AbandonData()	bDataRecStar=0;chRecPtr=0;

void chAbandonData(void)
{
	bDataRecStar=0;
	bOveJua=0;
	chRecPtr=255;
	chSendPtr=0;
	chSendLen=1;
	REN=0;
}

uint16_t CRC_Check(uint8_t *buf , uint8_t count)
{
	uint16_t CRCV = 0xFFFF;
	uint8_t iB ,i ,j;
	for (i = 0; i < count; i++){
		CRCV ^= *buf++;
		for (j = 0; j <= 7; j++){
			iB 	 = (uint8_t )(CRCV & 0x0001);
			CRCV = (uint16_t)(CRCV >> 1);
			if (iB == 1) {
				CRCV = (uint16_t)(CRCV ^ 0xA001);
			}
		}
	}
	return CRCV;
}

void PackBag(void) //编辑发送包
{
	uint8_t i,*p;
	uint16_t temp;
	p=(uint8_t *)&SystemPara.MeachineAddr+DataBagBuff [11];
	DataBagBuff [5]=(6+chTraDatLen)|0x80;	//前面的包头和仪器编码继续保留 重算数据总长度
	DataBagBuff [7]=0;
	for(i=0;i<7;i++){
		DataBagBuff [7]+=DataBagBuff [i];	
	}//求和校验 将数据存入LCR  //命令和命令长度保留
	for(i=12;i<12+chTraDatLen;i++){
		DataBagBuff[i]=*p; //写入需要读的数据
		p++;
	}
    temp = CRC_Check(&DataBagBuff[8],chTraDatLen+4);   //写入CRC校验码
    DataBagBuff [chTraDatLen+13] = (uint8_t) temp;
    DataBagBuff [chTraDatLen+12] = (uint8_t)(temp >> 8);
	chSendPtr = 0;				 //为发送编辑初始值  发送指针赋0 发送数据包长度
	chSendLen = chTraDatLen+14;
	SBUF=DataBagBuff [0];		 //开始发送
}

void chRecSuccess(void)
{
	uint8_t *p,i,NowPos;
	bit bSavFla;
	REN=0;bDataRecStar=0;chRecPtr=255;bOveJua=0;
	p=(uint8_t *)&SystemPara.MeachineAddr+DataBagBuff [11];
	if(0==WR_En){					  		//读寄存器数据
		PackBag();
	}else{
		for(i=12;i<12+chDataLen;i++){
			*p=DataBagBuff [i];
			p++;
		}
	}
	if((bit)(DataBagBuff[8] & 0x80)){
			BUZZER=0;
	switch(SystemPara.chFucCod){
		case 0x11:sprintf(DisBuffer,"%s测量",(&Kind[Get_MeaMode()-1][0]));//开始测量
				  LCD_Display_String(232,220,"T:",ADDPART1,NORMAL,NORMAL);
				  LCD_Display_String(0,220,DisBuffer,ADDPART1,NORMAL,NORMAL);
				  StartMes();
				  break;
		case 0x12:bShowMea=0;									//后台测量
				  break;
		case 0x13:bShowMea=1;									//显示测量
				  break;
		case 0x14:Stop_Measure();						  		//停止测量
				  break;
		case 0X15:bSavFla=SaveDeal(Get_MeaMode());				//保存数据 保存数据考虑状态位更新的时间
				  if(1 == bSavFla)Success_Save();
				  else Failed_Save();
				  break;
//		case 0x21:															//修改参数
//				  break;
		case 0x31:ReadRAM(SystemPara.chHisPage,&i,&NowPos);					//查看历史记录
				  break;																	
		case 0x32:DeletRec(SystemPara.chHisPage,SystemPara.chHisCSR);		//删除历史记录
				  break;
		default:break;
		}
	}
	if(WR_En) REN=1;
}

void RecDeal(void)
{
	uint8_t sum_Check,i,data1=0x41;
	if(SBUF==0X02 || 1==bDataRecStar){
		bDataRecStar=1;
		DataBagBuff [chRecPtr]=SBUF;
		if(0x03 == DataBagBuff [chRecPtr]){
			sum_Check=0;
			for(i=0;i<7;i++){
				sum_Check+=DataBagBuff[i];
			}
		}
		if(7==chRecPtr){
			if(DataBagBuff [7] != (uint8_t)sum_Check){chAbandonData();SBUF=sum_Check;}  // | 8!=chRecPtrLRC校验 校验成功继续接受 失败则清除接受标志位
		}
		if(9 == chRecPtr){
			chDataLen=(DataBagBuff [8]&0x0f)*DataBagBuff [chRecPtr]; //如果是写  数据包带的数据为 数据长度*数据个数
			if(0x30 == (DataBagBuff [8]&0x70)){
				WR_En=0;
//				data1=0x51;	  //读
				chTraDatLen=chDataLen;
				chDataLen=0;
			}	//如果是读  包带的数据为0个
			if(0x60==(DataBagBuff [8]&0x70)){
				WR_En=1;	  //写
//				data1=0x61;
			}
			chRecLen=13+chDataLen;
			bOveJua=1;
		}
		if((chRecLen <= chRecPtr)&(1 == bOveJua)){		 //数据接受完成
			if(0==CRC_Check(&DataBagBuff[8],chDataLen+6))chRecSuccess();
			else {chAbandonData();SBUF=data1;}
		}
		chRecPtr++;
	}
}

void RecCRC(void)
{
	uint16_t CRC_temp; 
	if(SBUF==0X12){
		bCRC=0;
		CRC_temp=CRC_Check(&DataBagBuff[1],CRC_Data-1);
	    DataBagBuff [0] = (uint8_t) CRC_temp;
	    DataBagBuff [1] = (uint8_t)(CRC_temp >> 8);
		chSendPtr=0;
		chSendLen=2;
		REN=0;
		CRC_Data=0;
		SBUF=DataBagBuff [0];
	}else{
		DataBagBuff[CRC_Data]=SBUF;
		CRC_Data++;
	}
}

void Uart() interrupt 4 
{
	if(1 == RI){
		RI=0;
//		if(SBUF==0x07|1==bCRC){
//			bCRC=1;
//			RecCRC();
//		}
//		else 
		RecDeal();
	}
	if(1 == TI){
		TI=0;
		chSendPtr++;
		if(chSendLen <= chSendPtr)REN=1; //   
		else SBUF=DataBagBuff [chSendPtr];
	}
}

