#include "..\app_cfg.h"

#ifndef _SPI_APP_CFG_H_
#define _SPI_APP_CFG_H_


sbit HV_SYNC  = P1^2;
sbit HV_DIN   = P1^3;
sbit HV_SDO   = P1^4;
sbit HV_SCLK  = P1^5;
sbit HV_RESET = P6^5;
sbit HV_RDY   = P6^4;

sbit AMP_SYNC  = P5^4;
sbit AMP_DIN   = P4^0;
sbit AMP_SDO   = P4^1;
sbit AMP_SCLK  = P4^3;
sbit AMP_RESET = P3^7;
sbit AMP_RDY   = P6^6;

#define CMD_NOP						0x0000
#define CMD_WRITE_TO_RDAC			0x0400
#define CMD_READ_FROM_RDAC			0x0800

#define CMD_RESET					0x1000

#define CMD_WRITE_TO_CONTROL_REG	0x1800
#define CMD_READ_FROM_CONTROL_REG	0x1C00
#define ALLOW_UPDATE 0x02
#define LOCK_WIPER 0x00
#define NORMAL_MODE 0x04
#define CMD_SOFTWARE_POWERDOWN		0x2000


//#define EN_SPI() SPTCL=SPTCL|(1<<6)
//sbit SYNC=P1^5;
//sbit RDY=P1^4;
//
//#define Do_Not 		0
//#define WR_RDAC		1
//#define RD_RDAC		2
//#define Reset		3
//#define WR_CONT		4
//#define RD_CONT		5
//#define Set_Mode	6

#endif
