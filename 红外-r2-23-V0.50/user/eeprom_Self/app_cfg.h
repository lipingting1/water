#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#include "..\app_cfg.h"


#define Empty 0
#define Non_Empty 1

#define 	ISP_TRIG()	IAP_TRIG = 0x5A,	IAP_TRIG = 0xA5		//ISP¥•∑¢√¸¡Ó

#define ISP_EN			(1<<7)
#define ISP_SWBS		(1<<6)
#define ISP_SWRST		(1<<5)
#define ISP_CMD_FAIL	(1<<4)
#define ISP_WAIT_1MHZ	7
#define ISP_WAIT_2MHZ	6
#define ISP_WAIT_3MHZ	5
#define ISP_WAIT_6MHZ	4
#define ISP_WAIT_12MHZ	3
#define ISP_WAIT_20MHZ	2
#define ISP_WAIT_24MHZ	1
#define ISP_WAIT_30MHZ	0

#if (FOSC >= 24000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_30MHZ
#elif (FOSC >= 20000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_24MHZ
#elif (FOSC >= 12000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_20MHZ
#elif (FOSC >= 6000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_12MHZ
#elif (FOSC >= 3000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_6MHZ
#elif (FOSC >= 2000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_3MHZ
#elif (FOSC >= 1000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_2MHZ
#else
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_1MHZ
#endif

#endif
