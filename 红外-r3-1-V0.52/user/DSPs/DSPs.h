#ifndef _DSPs_H
#define _DSPs_H

#define CMD_NOP						0x0000
#define CMD_WRITE_TO_RDAC			0x0400
#define CMD_READ_FROM_RDAC			0x0800

#define CMD_RESET					0x1000

#define CMD_WRITE_TO_CONTROL_REG	0x1800
#define CMD_READ_FROM_CONTROL_REG	0x1C00
#define CMD_SOFTWARE_POWERDOWN		0x2000
#define ALLOW_UPDATE 0x02
#define LOCK_WIPER 0x00
#define NORMAL_MODE 0x04

//#define W_RDAC	 1
//#define R_RDAC	 2
//#define Reset		 3
//#define W_CONT	 4
//#define R_CONT	 5
//#define Power_D	 6

//void Write(uint8_t cmd,uint16_t dat);
//uint16_t Read_Data(uint16_t cmd);
//uint16_t Read_RDAC(void);
//void Write_RDAC(uint16_t dat);	//	 uint8_t cmd,
void DACCommand(unsigned int Command, unsigned int *Value);
void DACInitialization(void);
void DacAmpCommand(unsigned int Command, unsigned int *Value);
#endif
