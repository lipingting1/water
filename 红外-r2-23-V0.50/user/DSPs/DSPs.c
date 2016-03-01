#include "app_cfg.h"

void DACInitialization(void)
{
	P1M0 &= 0xEE; //1110 1110
	P1M1 &= 0xEE;
	P3M0 &= 0x7F;//0111 1111
	P3M1 &= 0x7F;
}
void DACHardwareReset(void)
{
//	AMP_RESET = 0;
//	_nop_();
//	AMP_RESET = 1;
//	_nop_();

	HV_RESET = 0;
	_nop_();
	HV_RESET = 1;
	_nop_();
}
void DACCommand(unsigned int Command, unsigned int *Value)
{
	uint8_t i;
	uint16_t Register;

	Register=0x00;

	switch(Command)
	{
		case CMD_NOP:
			Register=CMD_NOP;
			break;

		case CMD_WRITE_TO_RDAC:
			Register=CMD_WRITE_TO_RDAC | ((*Value)&0x3FF);
			break;

		case CMD_READ_FROM_RDAC:
			Register=CMD_READ_FROM_RDAC;
			break;

		case CMD_RESET:
			Register=CMD_RESET;
			break;

		case CMD_WRITE_TO_CONTROL_REG:
			Register=CMD_WRITE_TO_CONTROL_REG  | ((*Value)&0x06);
			break;

		case CMD_READ_FROM_CONTROL_REG:
			Register=CMD_READ_FROM_CONTROL_REG;
			break;

		case CMD_SOFTWARE_POWERDOWN:
			Register=CMD_SOFTWARE_POWERDOWN  | ((*Value)&0x01);
			break;

		default:
			break;	
	}

	HV_SCLK = 1;
	_nop_();
	HV_SYNC = 1;
	_nop_();
	while(HV_RDY !=1 ) {;}	
	HV_SCLK = 0;
	_nop_();
	HV_SYNC = 0;
	_nop_();
	for(i=0; i<16; i++)
	{
		if((Register&0x8000)==0x8000)
			HV_DIN = 1;
		else
			HV_DIN = 0;
		_nop_();
		HV_SCLK = 1;
		_nop_();
		HV_SCLK = 0;
		_nop_();
		Register<<=1;	
	}

	HV_SYNC = 1;
	_nop_();

	while(HV_RDY != 0) {;}
	while(HV_RDY != 1) {;}

	if((Command==CMD_READ_FROM_RDAC)||(Command==CMD_READ_FROM_CONTROL_REG))
	{
		Register=0;
		HV_SYNC = 0;
		_nop_();
		for(i=0; i<16; i++)
		{
			Register<<=1;
			HV_SCLK = 1;
			_nop_();
			if (HV_SDO)
				Register ++;
			_nop_();
			HV_SCLK = 0;
			_nop_();
		}
			
		HV_SYNC = 1;
		_nop_();
	}

	if(Command==CMD_READ_FROM_RDAC)
	{
	 	Register&=0x03FF;
		*Value=Register;
	}
	else if(Command==CMD_READ_FROM_CONTROL_REG)
	{
		Register&=0x0006;
		*Value=Register;
	}
}
//uint16_t SPIdat_RX,SPIdat_TX;
//uint8_t  bTraFla;
//
//void SPI_Deal(void) interrupt 9		 //接受数据并
//{
//	SPSTAT&=0x7f;
//	BUZZER=0;
//	if(0 == bTraFla){
//		SPIdat_RX=SPDAT;					
//		SPDAT=(uint8_t)(SPIdat_TX>>8);
//	}else{
//		SPIdat_RX=(SPIdat_RX<<8)|SPDAT;		
//	}
//	bTraFla++;
//}
//
//uint16_t Change_Data(uint8_t cmd,uint16_t dat)
//{
//	uint8_t i=8;
//	SYNC=1;
//	_nop_();
//	SYNC=0;
//	bTraFla=0;
//	SPIdat_TX=(dat|(cmd<<9));
//	SPDAT=(uint8_t)(SPIdat_TX>>8);
//	while(2!=bTraFla);
//	_nop_();
//	SYNC=1;
//	while((!RDY) | (i--));
//	return SPIdat_RX;
//}
//
//uint16_t Read_RDAC(void)
//{
//	uint16_t dat;
//	dat=Change_Data(RD_RDAC,0);
//	return dat;
//}
//
//void Write_RDAC(uint16_t dat)	//	 uint8_t cmd,
//{
//	 Change_Data(WR_RDAC,dat);
//}
////uint16_t Read_Data(uint16_t cmd)
////{
////	uint8_t i=3;
////	while(i--){
////		_nop_();
////		SYNC=0;
////		bTraFla=0;
////		if(1==i)SPIdat_TX=(cmd<<10);
////		else SPIdat_TX=0;
////		SPDAT=(uint8_t)(SPIdat_TX>>8);
////		while(2!=bTraFla);
////		_nop_();
////		SYNC=1;
////	}
////	while(!RDY);
////	return SPIdat_RX;
////}
//
//void Write(uint8_t cmd,uint16_t dat)
//{
//	uint8_t i=8;
//	SYNC=1;
//	_nop_();
//	SYNC=0;
//	bTraFla=0;
//	SPIdat_TX=(dat|(cmd<<10));
//	SPDAT=(uint8_t)(SPIdat_TX>>8);
//	while(2!=bTraFla);
//	_nop_();
//	SYNC=1;
//	while((!RDY) | (i--));
//}
//
//uint16_t Read_Data(uint16_t cmd)
//{
//	uint8_t i=3;
//	while(i--){
//		_nop_();
//		SYNC=0;
//		bTraFla=0;
//		if(1==i)SPIdat_TX=(cmd<<10);
//		else SPIdat_TX=0;
//		SPDAT=(uint8_t)(SPIdat_TX>>8);
//		while(2!=bTraFla);
//		_nop_();
//		SYNC=1;
//	}
//	while(!RDY);
//	return SPIdat_RX;
//}
