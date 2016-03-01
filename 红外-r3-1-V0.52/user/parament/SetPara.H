#ifndef _SetPara_H_
#define _SetPara_H_

//void ParaPage(uint8_t chMode);							//参数修改页面  
//void ParSetUpdPage(uint8_t chMode);		   				//参数修改界面刷新 		0x42
//void ParPagUpd(uint8_t chKey,uint8_t chMode);			//参数修改操做处理      0x41		KEY		  chSetParMode
//void StartPara(void);
extern void ParaPage(uint8_t chMode);							//参数修改页面  
extern void ParSetUpdPage();		   				//参数修改界面刷新 		0x42
extern void ParPagUpd(uint8_t chKey,uint8_t chMode);			//参数修改操做处理      0x41		KEY		  chSetParMode
extern void StartPara(void);

#endif
