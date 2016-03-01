#include "app_cfg.h"

//RTC和eeprom公用的底层驱动////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void _nop(uint8_t i)
{
	while(i--){
	_nop_();}
}

bit Answer_I2C(void)
{
	bit flag=1;
	SDA=1;
		_nop(1);
	SCL=1;
	flag=SDA;
		_nop(1);
	SCL=0;
	if(0 == flag)return 1;
	return 0;
}

bit Check_Busy(void)
{
	if((1 == SDA) & (1 == SCL))
		return 0;
	else 
		return 1;
}

void Start_I2C(void)
{
	SDA=1;
	_nop_();
	SCL=1;
	_nop(10);
	SDA=0;
	_nop(10);
	SCL=0;
}
void Stop_I2C(void)
{
	SDA=0;
	_nop_();
	SCL=1;
	_nop(10);
	SDA=1;
	_nop(10);
}

uint8_t Read_dat(void)
{
	uint8_t i=7,dat;
	dat=0;
	while(1){
		_nop(10);
		SCL=1;
		dat<<=1;
		_nop(10);
		if(1 == SDA)dat++;
		SCL=0;
		if(0 == i)break;
		else i--;
	}
	return dat;
}


void Write_dat(uint8_t dat)
{
	uint8_t i=7;
	while(1){
		if((dat&(1<<i)))SDA=1;
		else SDA=0;
		_nop(5);
		SCL=1;
		_nop(10);
		SCL=0;
		if(0 == i)break;
		else i--;
	}
	Answer_I2C();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///eeprom的应用函数//////////////////////////////
					//页码		//写入目的地址	//读eeprom首地址  //写入数据个数
void Read_EEprom_One(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr)	 //每页256 共8页		 2K可寻址
{
	while(Check_Busy());
	Start_I2C();
	Write_dat(0xa2);
	Write_dat(chPage);
	Write_dat(StaAddr);
	Start_I2C();
	Write_dat((EEprom_addr+0x01));
	*PurAddr=Read_dat();
	Stop_I2C();
}

void Read_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint8_t num)	 //每页256 共8页		 2K可寻址
{
	uint8_t i=0;
	while(1){
		Read_EEprom_One(chPage,PurAddr,StaAddr);
		i++;
		PurAddr++;
		StaAddr++;
		if(num == i)break;
	}
	Stop_I2C();
}

void Write_EEprom_One(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr)	 //每页256 共8页		 8K byte可寻址
{
	uint8_t i=0,ptr;
	while(Check_Busy());
	Start_I2C();
	Write_dat(0xa2);
	Write_dat(chPage);
	Write_dat(StaAddr);
	Write_dat(*PurAddr);
	Stop_I2C();
	for(i=0;i<20;i++)Read_EEprom_One(0,&ptr,i);
}

void Write_EEprom(uint8_t chPage,uint8_t *PurAddr,uint8_t StaAddr,uint16_t num)	 //每页256 共8页		 8K byte可寻址
{
	uint16_t i=0;
	while(1){
		Write_EEprom_One(chPage,PurAddr,StaAddr);	 //每页256 共8页		 8K byte可寻址
		i++;
		PurAddr++;
		if(255 == StaAddr)chPage++;
		_nop(20);
		StaAddr++;
		if(num == i)break;
	}  
	Stop_I2C();
}


////RTC的应用函数//////////////////////////////
void RTC_Init(uint8_t *p,uint8_t num)		//相当于写初始化函数	  通过写RTC来初始化时间
{
	uint8_t i=0;
	while(Check_Busy());
	Start_I2C();
	Write_dat(RTC_addr);
	Write_dat(0x01);
	while(num--){
		if(3 == num)Write_dat(0);
		else{
			Write_dat(*p);
			p--;
		}
	}
	Stop_I2C();
}

//					//页码		//写入目的地址	//读eeprom首地址  //写入数据个数
void Updata_Time(uint8_t *p,uint8_t num)	//相当于读初始化函数		 通过读RTC来更新时间寄存器中的数据 
{
	uint8_t i=0;
	while(Check_Busy());
	while(1){
		Start_I2C();
		Write_dat(RTC_addr);
		Write_dat(0x00+i);
		Start_I2C();
		Write_dat(RTC_addr|1);
		if(3 == i)Read_dat();
		else{
			*p=Read_dat();
			p--;
		}
		i++;
		if(num == i)break;
	}  
	Stop_I2C();
}


