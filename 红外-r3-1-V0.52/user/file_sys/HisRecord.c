#include "app_cfg.h"

extern uint8_t chInterFaceState,CSR_Old,chPage;
extern bit bDisDel,bDelFlag;
extern uint8_t ParaBuffer[32],DisBuffer[12];
extern Sys_Para SystemPara;
extern float fMeaVal;  //校准测量平均值

uint8_t shun=0;
extern uint32_t hwMeaCPMVal_Cs,hwMeaCPMVal_I;


void ShowNum(uint8_t Ptr,uint8_t NowPageAddr)			//当前历史记录条数和总条数页面刷新
{
	Read_EEprom(0,(uint8_t *)&SystemPara.RecordQua,0,2);   //
	sprintf(DisBuffer,"%2d/%2d",(uint16_t)((NowPageAddr<<1)+Ptr+1),(uint16_t)SystemPara.RecordQua);
//	sprintf(&DisBuffer,"%2d/%2d/%d",(uint16_t)CSR_Old,(uint16_t)SystemPara.RecordQua,(uint16_t)chPage);
	LCD_Display_String(200,40,DisBuffer,ADDPART1,NORMAL,NORMAL);
}

void ReadRAM(uint8_t NowPageAddr,uint8_t *k,uint8_t *NowPos)//				  ,uint8_t chReadNum
{
	uint8_t NowAddr;	  //当前显示过的记录总数    总记录条数（预期<256）
	uint8_t hwCurRAM=0,chNext;		//共46条数据 23页	256byte/page 32page	  共10条数据  	   每个扇区16空闲位

	*NowPos=(NowPageAddr-1)<<1;	 //总显示数据条数
	Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][0],0,8);	  //读扇区头结点
	SystemPara.RecordQua=SystemPara.History.Record[0][0];	  	  //读扇区头结点
	//寻址扇区，确定当前显示扇区  NowPos表示当前已经显示的在本扇区中位置
	hwCurRAM= SystemPara.History.Record[0][6];
	while(1){
		Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,2);	  //读该扇区下一扇区指针 和该扇区存储数据量
		if(*NowPos < SystemPara.History.Record[0][8])break;			//疑问：如果已经显示最后一个数据  回答：这一顾虑会在下翻页中考虑如果已经是最后数据就会返回首行显示
		else (*NowPos) -= SystemPara.History.Record[0][8];
		hwCurRAM = SystemPara.History.Record[0][7];
	} 
	chNext=SystemPara.History.Record[0][7];
	Read_EEprom(hwCurRAM,(uint8_t *)&NowAddr,9,1);//读扇区链表的头节点
	NowAddr=NowAddr*PackNum+PackNum+11;
	*k=0;
	while(2 != *k){	 //NowPos--
		if(0 == *NowPos){	//找到当前读取位置		// 确定当前显示在扇区中的位置并打印到缓冲区内
			Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[*k][0],NowAddr-PackNum+1,PackNum); //4*7	 /读下个数据指针
			NowAddr=SystemPara.History.Record[*k][PackNum-1];	
			(*k)++;
			if(EMPTY == NowAddr){ //正常是读取两数据，但如果读取到尾结点读取提前结束
				if(EMPTY == chNext)break;		//首指针空，，则停止读取
				Read_EEprom(chNext,(uint8_t *)&NowAddr,9,1); //读取一个数据后溢出，翻页读下个扇区首指针
				hwCurRAM = chNext;
			}
		}else{
			(*NowPos)--;
			Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][0],NowAddr,1); //4*7	 /读下个数据指针
			NowAddr=SystemPara.History.Record[0][0];
		}
		NowAddr=NowAddr*PackNum+PackNum+11;
	}
}
uint32_t DatTypDef(uint8_t *b_char)
{
	uint8_t i=5;
	uint32_t hw_int;
		hw_int|=*b_char;
		hw_int<<=8;
		b_char++;
		hw_int|=*b_char;
		hw_int<<=8;
		b_char++;
		hw_int|=*b_char;
		hw_int<<=8;
		b_char++;
		hw_int|=*b_char;
		return 	hw_int;
}


void UpdHisRec(uint8_t NowPageAddr)	 		//历史记录页面刷新函数			当前显示, Ptr当前页面的显示的行数，FriAddr为当前页面
{
	uint32_t Gap_Value;
	uint8_t NowPos,chNum=0,k;
	ReadRAM(NowPageAddr,&k,&NowPos);
	Clear_Windows(16,96,200,112,ADDPART1,NORMAL);
	while(k!=chNum){                                                                   
		sprintf(ParaBuffer,"%2d %02d/%02x/%02x         %02x:%02x",
				(uint16_t)(((NowPageAddr-1)<<1)+chNum+1),
				(uint16_t)SystemPara.History.Record[chNum][0],
				(uint16_t)SystemPara.History.Record[chNum][1],
				(uint16_t)SystemPara.History.Record[chNum][2],
				(uint16_t)SystemPara.History.Record[chNum][3],
				(uint16_t)SystemPara.History.Record[chNum][4]);
		LCD_Display_String(16,80+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址	
		Gap_Value=DatTypDef(&SystemPara.History.Record[chNum][16]);
		sprintf(ParaBuffer,"CPM:%d%04d",(uint16_t)(Gap_Value/10000),(uint16_t)(Gap_Value%10000)); //显示CP值
		LCD_Display_String(40,96+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址  Cs-137:%dBg/Kg	  %c
		sprintf(ParaBuffer,"T:%02d:%02d:%02d",
				(uint16_t)(SystemPara.History.Record[chNum][5]),
				(uint16_t)(SystemPara.History.Record[chNum][6]),
				(uint16_t)(SystemPara.History.Record[chNum][7])); //显示CP值
		LCD_Display_String(136,96+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址  Cs-137:%dBg/Kg	  %c
		Gap_Value=DatTypDef(&SystemPara.History.Record[chNum][8]);
		sprintf(ParaBuffer,"Cs-137:%d%04ddBg/Kg",(uint16_t)(Gap_Value/10000),(uint16_t)(Gap_Value%10000)); //显示CP值
		LCD_Display_String(40,112+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址  Cs-137:%dBg/Kg	  %c
		Gap_Value=DatTypDef(&SystemPara.History.Record[chNum][12]);
		sprintf(ParaBuffer,"I-137:%d%04ddBg/Kg",(uint16_t)(Gap_Value/10000),(uint16_t)(Gap_Value%10000)); //显示CP值
		LCD_Display_String(40,128+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow标识的是当前页面显示的最后一条指令地址  Cs-137:%dBg/Kg	  %c
		chNum++;
	}
}


void His_Record(void)
{
	if(0x00 == chInterFaceState){
		CSR_Old=0;
		chPage=1;
	}
	Read_EEprom(0,(uint8_t *)&SystemPara.RecordQua,0,2);   //
	bDisDel=0;
	if((0xff == SystemPara.RecordQua) | (0 == SystemPara.RecordQua)) bDisDel=1;
	Clear_Windows(0,32,320,184,ADDPART1,NORMAL);
	LCD_Display_String(72,50,"历 史 记 录",ADDPART1,NORMAL,NORMAL); 	
	LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
	if(1 == bDisDel);
	else{
		Show_String(276,40,"<_",ADDPART1,NORMAL);  // 	 
		Show_String(276,80,"=>",ADDPART1,NORMAL);
		LCD_Display_String(264,160,"导出",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,120,"删除",ADDPART1,NORMAL,NORMAL);
	 	LCD_Display_String(8,80+(CSR_Old<<6),"#",ADDPART1,NORMAL,NORMAL);
		UpdHisRec(chPage);//初始时count=0;Page=0;显示初始的4个
		ShowNum(CSR_Old,chPage-1);
		Draw_Scrollbar(233,80+(CSR_Old<<6),64,0,ADDPART1,NORMAL);
	}
	Draw_ArcFrame(0,32,240,184,ADDPART1,NORMAL);	  //pInterFaceState=0x50;break;
	chInterFaceState = 0x50;
}

bit SaveDeal(uint8_t chMode)
{
	bit	bSavFla=0;
	uint8_t *p,i,k;						
	uint16_t hwCurRAM,NowAddr;
	uint16_t NowPos;
	switch(chMode){
	case 1: SystemPara.BKG_ALL= SystemPara.hwMeaCPMVal_ALL;
			SystemPara.BKG_Cs = hwMeaCPMVal_Cs;
			SystemPara.BKG_I  = hwMeaCPMVal_I;
			bSavFla=1;
		    break;
	case 2:
	case 3:	Read_EEprom(0,(uint8_t *)&SystemPara.RecordQua,0,2);   //  
		    Read_EEprom(0,(uint8_t *)&SystemPara.History.Record[0][0],0,7); 
			if(0 == SystemPara.RecordQua){
				//初始化总存储信息表
				SystemPara.History.Record[0][0]=0;		 //总数0+1
				SystemPara.History.Record[0][1]=0;
						 
				SystemPara.History.Record[0][2]=1;		 
				SystemPara.History.Record[0][3]=0;		 
				SystemPara.History.Record[0][4]=0;		
				SystemPara.History.Record[0][5]=0;		 //置位全部存储扇区对应状态位

				SystemPara.History.Record[0][6]=0;		 //存储扇区首个扇区地址为1
				//初始化操作页
				SystemPara.History.Record[0][7]=EMPTY;	 //下一存储扇区为EMPTY
				SystemPara.History.Record[0][8]=0;		 //改扇区存储记录总数为0
				SystemPara.History.Record[0][9]=0;		 //本首存储地址为0
				//设置本扇区全部存储区对应状态位
				SystemPara.History.Record[0][10]=1; // 0 
				SystemPara.History.Record[0][11]=0;	//该扇区的存储状态

				hwCurRAM=0;
				NowAddr=0;
			}else{
				NowPos=SystemPara.RecordQua;//(SystemPara.History.Record[0][0]<<8)|SystemPara.History.Record[0][1];
				hwCurRAM=SystemPara.History.Record[0][6];	//从首扇区开始遍历
				while(1){
					Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,2);	  //读扇区头结点
					if(EMPTY == SystemPara.History.Record[0][7])break;	//下一扇区指向空  则表示是尾扇区		
					(NowPos) -= SystemPara.History.Record[0][8];
					hwCurRAM  = SystemPara.History.Record[0][7];		//SystemPara.History.Record[0][7]下一扇区，hwCurRAM当前扇区
				}
				k=2;  
				if(RAMdata == SystemPara.History.Record[0][8]){ //扇区满溢出，从小到大查询出空闲扇区
					for(i = 0 ; i != Total_ram_Num ; i ++){
						if(0 == (SystemPara.History.Record[0][k]&(1<<(i%8))))break;
						if(7 == i%8)k++;
					}
					if(Total_ram_Num == i)break;											//未遍历到空闲山区，存储满  跳出存储程序
					SystemPara.History.Record[0][k] |= 1<<(i%8);				//查出空扇区i，
					Write_EEprom(hwCurRAM,&i,7,1);								//本扇区的改变是指向下一扇区的指针指向a,
					SystemPara.History.Record[0][7] = EMPTY;						
					SystemPara.History.Record[0][8] = 0;								//新扇区总数据数
					SystemPara.History.Record[0][9] = 0;								//新扇区首地址更新

					SystemPara.History.Record[0][10]= 1;
					SystemPara.History.Record[0][11]= 0;

					NowAddr=0;
					hwCurRAM=i;
				}else{
					Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][9],9,3);	  // 读扇区首存储地址 和 扇区存储区状态表
					NowAddr=SystemPara.History.Record[0][9];	
					while(1){	 //	寻址扇区中的尾结点 返回值NowAddr地址之后的24个值
						NowAddr=NowAddr*PackNum+36;  //只读最后一个数据36=12+24
						Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[1][0],NowAddr,1); //
						if(EMPTY == SystemPara.History.Record[1][0])break; //下一数据指向空  则表示是尾数据
						NowAddr=SystemPara.History.Record[1][0];
					}
					i=0;k=10;
					while(1){//寻址扇区中空位子
						if(0 == (SystemPara.History.Record[0][k]&(1<<(i%8)))) break;
						i++;
						if(8 == i)k++;
					}
					Write_EEprom(hwCurRAM,(uint8_t *)&i,NowAddr,1); //指针链接。。原来的尾指针0指向新增指针i
				
					SystemPara.History.Record[0][k]|=1<<(i%8); // 扇区状态表对应空置位置1
					NowAddr=i;
				}
			} //存储值写入缓冲区
			
			p=&SystemPara.SysData[1];
			SystemPara.History.Record[1][0]=shun;shun++;
			for(i=1;i<5;i++){
				SystemPara.History.Record[1][i] = *p;
				p++;
			}
			p=(uint8_t *)&SystemPara.MeasureTime[0];
			for(i=5;i<20;i++){
				SystemPara.History.Record[1][i] = *p;
				p++;
			} //写入测量时间和测量数值

			SystemPara.History.Record[1][21] = EMPTY;
			SystemPara.History.Record[0][8]++;
			if(255 == SystemPara.History.Record[0][1])SystemPara.History.Record[0][0]++;
			SystemPara.History.Record[0][1]++;

			NowAddr=(NowAddr*PackNum)+12;
			Write_EEprom(0 ,(uint8_t *)&SystemPara.History.Record[0][0],0,7); //指针链接。。原来的尾指针0指向新增指针i
			Write_EEprom(hwCurRAM ,(uint8_t *)&SystemPara.History.Record[0][7],7,5); //指针链接。。原来的尾指针0指向新增指针i
			Write_EEprom(hwCurRAM ,(uint8_t *)&SystemPara.History.Record[1][0],NowAddr,PackNum); //指针链接。。原来的尾指针0指向新增指针i

			bSavFla=1;
			break;
	case 4: 
//	if(2 <= chCalinUum){
//				fMeaVal/=3;
//				chCalinUum=0;
				SystemPara.CalibFactor_Cs=(float)StanValue/hwMeaCPMVal_Cs;
//				bSavFla=1;
//			}
			break;
	default:break;
	}
	return bSavFla;
}

void SaveValue(uint8_t chMode)	   						//保存测量结果处理
{
	bit bSavFla;	//low 下个扇区地址，high 本扇区数据量
	bSavFla=SaveDeal(chMode);
	if(1==bSavFla){
		MainMenu(chMode);
	}else{
		Clear_Windows(264,160,32,16,ADDPART1,NORMAL); 
		LCD_Display_String(100,196,"保存失败!",ADDPART1,NORMAL,NORMAL);
	}
}

void DeletRec(uint8_t NowPageAddr,uint8_t Ptr)
{
	uint16_t NowPos; 
	uint8_t  NowAddrAd,NowAddr,NowAddrSha=EMPTY;
	uint8_t chCurRAM=0,chCurRAMsha=EMPTY;
	NowPos=((NowPageAddr-1)<<1)+Ptr+1;
	Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][0],0,7);	  //读扇区头结点
    //寻址扇区，确定当前显示扇区  NowPos  	//返回值  SystemPara.History.Record[0][7]下一扇区，hwCurRAMsha当前扇区
	chCurRAM= SystemPara.History.Record[0][6];	 //读扇区头结点
	while(1){	
		Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,2);	  
		if(NowPos <= SystemPara.History.Record[0][8])break;		
		else NowPos -= SystemPara.History.Record[0][8];
		chCurRAMsha=chCurRAM;
		chCurRAM= SystemPara.History.Record[0][7];
	}
	Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][9],9,3);	  // 读扇区首存储地址 和 扇区存储区状态表

	if(1 == SystemPara.History.Record[0][8]){//当前扇区仅一条记录，则删除扇区
		if(chCurRAM == SystemPara.History.Record[0][6])SystemPara.History.Record[0][6]=SystemPara.History.Record[0][7];	  //如果删除的扇区是首扇区
		else{	//将该扇区下一扇区的地址传给前一扇区  实现
			chCurRAM=SystemPara.History.Record[0][7];//chCurRAM借用来装入下一扇区值
			Read_EEprom(chCurRAMsha,(uint8_t *)&SystemPara.History.Record[0][7],7,5);
			SystemPara.History.Record[0][7]=chCurRAM;//chCurRAM借用来装入下一扇区值
		}
		SystemPara.History.Record[0][2+(chCurRAM/8)] &= ~(1<<(chCurRAM%8)); //将该扇区状态位置闲
		chCurRAM=chCurRAMsha;
	}else{
		NowAddr=SystemPara.History.Record[0][9];
		while(1){//寻址到当前扇区地址，返回NowAddrSha（当前地址之前的指针）NowAddr（当前指针） SystemPara.History.Record[1][24]后指针
			NowAddrAd=NowAddr*PackNum+PackNum+11;
			Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[1][0],NowAddrAd,1); //
			NowPos--;
			if(0==NowPos)break;
			NowAddrSha=NowAddrAd;
			NowAddr=SystemPara.History.Record[1][0];
		}
		if(EMPTY == NowAddrSha){SystemPara.History.Record[0][9]=SystemPara.History.Record[1][0];}	  //为0,说明删除的位置是扇区的首节点
		else Write_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[1][0],NowAddrSha,1);	//否则为中间节点 第NowAddrSha	SystemPara.History.Record[1][0]
		
		SystemPara.History.Record[0][8]--;
		SystemPara.History.Record[0][10+NowAddr/8] &= ~(1<<(NowAddr%8));
	}
	if(0 == SystemPara.History.Record[0][1])SystemPara.History.Record[0][0]--;
	SystemPara.History.Record[0][1]--;

	Write_EEprom(0,(uint8_t *)&SystemPara.History.Record[0][0],0,7);
	Write_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,5);

//	Read_EEprom(0,(uint8_t *)&sd[8],10,1);	  // 读扇区内数据存储状态表
//	Read_EEprom(0,(uint8_t *)&sd[9],2,1);	  // 读扇区首存储地址 和 扇区存储区状态表
}

void HistoryDel(uint8_t chKey,uint8_t NowPageAddr,uint8_t Ptr)//)							//删除历史记录处理		0X51
{
	switch(chKey){
	case 3: chInterFaceState=0x50;  //确认删除 CSR_Old,chPage
			DeletRec(NowPageAddr,Ptr);
			Read_EEprom(0,(uint8_t *)&SystemPara.RecordQua,0,2);   //
			if(CSR_Old+1+((chPage-1)<<1) >= SystemPara.RecordQua){
				CSR_Old=(SystemPara.RecordQua-1)%2;
				chPage = SystemPara.RecordQua/2;
				if(0 == CSR_Old)chPage++;
			}
			MainMenu(5);
			break;//刷新页面				  
	case 5: MainMenu(5);
			break;
	default:break; 	
	}
}

void HistoryPro(uint8_t chKey)							// 历史记录处理			0X50
{
	switch(chKey){
	case 1 : if(1==bDisDel) break;	  //向上
			 if(0==CSR_Old){	//判断位置是否位于最上
				 Clear_Windows(8  ,80+(CSR_Old<<6),8,16,ADDPART1,NORMAL);
				 Clear_Windows(232,80+(CSR_Old<<6),8,64,ADDPART1,NORMAL); //是。。清此位置的箭头
				 if(1>=chPage){//判断是否位于首页面
					 CSR_Old=(SystemPara.RecordQua-1)%2;	  //是。。是否仅有1页。。是则向上到最后一个数据不翻页即无需刷新页面
					 if(2<SystemPara.RecordQua){
						 chPage=SystemPara.RecordQua/2;
						 if(1 == CSR_Old);
						 else chPage++;   //跳转页面到最后一页	刷新页面
						 UpdHisRec(chPage);//
					 } 
				 }else{
					 chPage--;//不是位于首页   页码	-1.
					 CSR_Old=1;	//返回到上页最后一个数据  并刷新页面
					 UpdHisRec(chPage);//
			 	 }
			 }else CSR_Old--; //不是首行。。直接上移
			 ShowNum(CSR_Old,chPage-1);
			 if(1!=CSR_Old){
				Clear_Windows(8,80+((CSR_Old+1)<<6),8,16,ADDPART1,NORMAL);//不是首行。。清下行箭头			 
				Clear_Windows(232,80+((CSR_Old+1)<<6),8,64,ADDPART1,NORMAL);
			 }
			 LCD_Display_String(8,80+(CSR_Old<<6),"#",ADDPART1,NORMAL,NORMAL);
			 Draw_Scrollbar(232,80+(CSR_Old<<6),64,0,ADDPART1,NORMAL); 
			 break;
	case 2 : if(1 == bDisDel) break;
			 if(1 == CSR_Old|((((chPage-1)<<1)+CSR_Old+1) >= SystemPara.RecordQua)){	//向下	判断是否是尾行(第三行或者是最后一页的最后行)
				 Clear_Windows(8,80+(CSR_Old<<6),8,16,ADDPART1,NORMAL);	  		//是。。清尾行箭头
				 Clear_Windows(232,80+(CSR_Old<<6),8,64,ADDPART1,NORMAL);
				 if(SystemPara.RecordQua <= (((chPage-1)<<1)+CSR_Old+1))	chPage=1;	//判断是否是尾页。。是跳转到首页
				 else chPage++;	//不是。。页数+1
				 UpdHisRec(chPage);	
				 CSR_Old=0;			   //无论是否是尾页都是跳转到新页的首行，更新页面
			 }else CSR_Old++; //不是尾行，行数+1 判断该行后是否还是有数据
			 ShowNum(CSR_Old,chPage-1);
			 if(0<CSR_Old){
			 	Clear_Windows(8	 ,80+((CSR_Old-1)<<6),8,16,ADDPART1,NORMAL);	//不是尾行。。清上行
				Clear_Windows(232,80+((CSR_Old-1)<<6),8,64,ADDPART1,NORMAL); 
			 }
			 LCD_Display_String(8,80+(CSR_Old<<6),"#",ADDPART1,NORMAL,NORMAL);
			 Draw_Scrollbar(232,80+(CSR_Old<<6),64,0,ADDPART1,NORMAL);
			 break;
	case 3 : if(1==bDisDel) break;
			 chInterFaceState = 0x51;
			 Clear_Windows(248,40,64,60,ADDPART1,NORMAL);//本底测量
			 LCD_Display_String(264,120,"确认",ADDPART1,NORMAL,NORMAL);
			 LCD_Display_String(264,200,"返回",ADDPART1,NORMAL,NORMAL);
			 LCD_Display_String(88,128," 删除 ? ",ADDPART1,REVERSE,REVERSE); break;
	case 4 : break;
	case 5 : InterfaceInit(); break;
	default: break;
	}
}


