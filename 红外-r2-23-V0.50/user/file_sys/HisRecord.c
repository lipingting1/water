#include "app_cfg.h"

extern uint8_t chInterFaceState,CSR_Old,chPage;
extern bit bDisDel,bDelFlag;
extern uint8_t ParaBuffer[32],DisBuffer[12];
extern Sys_Para SystemPara;
extern float fMeaVal;  //У׼����ƽ��ֵ

uint8_t shun=0;
extern uint32_t hwMeaCPMVal_Cs,hwMeaCPMVal_I;


void ShowNum(uint8_t Ptr,uint8_t NowPageAddr)			//��ǰ��ʷ��¼������������ҳ��ˢ��
{
	Read_EEprom(0,(uint8_t *)&SystemPara.RecordQua,0,2);   //
	sprintf(DisBuffer,"%2d/%2d",(uint16_t)((NowPageAddr<<1)+Ptr+1),(uint16_t)SystemPara.RecordQua);
//	sprintf(&DisBuffer,"%2d/%2d/%d",(uint16_t)CSR_Old,(uint16_t)SystemPara.RecordQua,(uint16_t)chPage);
	LCD_Display_String(200,40,DisBuffer,ADDPART1,NORMAL,NORMAL);
}

void ReadRAM(uint8_t NowPageAddr,uint8_t *k,uint8_t *NowPos)//				  ,uint8_t chReadNum
{
	uint8_t NowAddr;	  //��ǰ��ʾ���ļ�¼����    �ܼ�¼������Ԥ��<256��
	uint8_t hwCurRAM=0,chNext;		//��46������ 23ҳ	256byte/page 32page	  ��10������  	   ÿ������16����λ

	*NowPos=(NowPageAddr-1)<<1;	 //����ʾ��������
	Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][0],0,8);	  //������ͷ���
	SystemPara.RecordQua=SystemPara.History.Record[0][0];	  	  //������ͷ���
	//Ѱַ������ȷ����ǰ��ʾ����  NowPos��ʾ��ǰ�Ѿ���ʾ���ڱ�������λ��
	hwCurRAM= SystemPara.History.Record[0][6];
	while(1){
		Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,2);	  //����������һ����ָ�� �͸������洢������
		if(*NowPos < SystemPara.History.Record[0][8])break;			//���ʣ�����Ѿ���ʾ���һ������  �ش���һ���ǻ����·�ҳ�п�������Ѿ���������ݾͻ᷵��������ʾ
		else (*NowPos) -= SystemPara.History.Record[0][8];
		hwCurRAM = SystemPara.History.Record[0][7];
	} 
	chNext=SystemPara.History.Record[0][7];
	Read_EEprom(hwCurRAM,(uint8_t *)&NowAddr,9,1);//�����������ͷ�ڵ�
	NowAddr=NowAddr*PackNum+PackNum+11;
	*k=0;
	while(2 != *k){	 //NowPos--
		if(0 == *NowPos){	//�ҵ���ǰ��ȡλ��		// ȷ����ǰ��ʾ�������е�λ�ò���ӡ����������
			Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[*k][0],NowAddr-PackNum+1,PackNum); //4*7	 /���¸�����ָ��
			NowAddr=SystemPara.History.Record[*k][PackNum-1];	
			(*k)++;
			if(EMPTY == NowAddr){ //�����Ƕ�ȡ�����ݣ��������ȡ��β����ȡ��ǰ����
				if(EMPTY == chNext)break;		//��ָ��գ�����ֹͣ��ȡ
				Read_EEprom(chNext,(uint8_t *)&NowAddr,9,1); //��ȡһ�����ݺ��������ҳ���¸�������ָ��
				hwCurRAM = chNext;
			}
		}else{
			(*NowPos)--;
			Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][0],NowAddr,1); //4*7	 /���¸�����ָ��
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


void UpdHisRec(uint8_t NowPageAddr)	 		//��ʷ��¼ҳ��ˢ�º���			��ǰ��ʾ, Ptr��ǰҳ�����ʾ��������FriAddrΪ��ǰҳ��
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
		LCD_Display_String(16,80+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ	
		Gap_Value=DatTypDef(&SystemPara.History.Record[chNum][16]);
		sprintf(ParaBuffer,"CPM:%d%04d",(uint16_t)(Gap_Value/10000),(uint16_t)(Gap_Value%10000)); //��ʾCPֵ
		LCD_Display_String(40,96+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ  Cs-137:%dBg/Kg	  %c
		sprintf(ParaBuffer,"T:%02d:%02d:%02d",
				(uint16_t)(SystemPara.History.Record[chNum][5]),
				(uint16_t)(SystemPara.History.Record[chNum][6]),
				(uint16_t)(SystemPara.History.Record[chNum][7])); //��ʾCPֵ
		LCD_Display_String(136,96+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ  Cs-137:%dBg/Kg	  %c
		Gap_Value=DatTypDef(&SystemPara.History.Record[chNum][8]);
		sprintf(ParaBuffer,"Cs-137:%d%04ddBg/Kg",(uint16_t)(Gap_Value/10000),(uint16_t)(Gap_Value%10000)); //��ʾCPֵ
		LCD_Display_String(40,112+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ  Cs-137:%dBg/Kg	  %c
		Gap_Value=DatTypDef(&SystemPara.History.Record[chNum][12]);
		sprintf(ParaBuffer,"I-137:%d%04ddBg/Kg",(uint16_t)(Gap_Value/10000),(uint16_t)(Gap_Value%10000)); //��ʾCPֵ
		LCD_Display_String(40,128+(chNum<<6),ParaBuffer,ADDPART1,NORMAL,NORMAL);//NowShow��ʶ���ǵ�ǰҳ����ʾ�����һ��ָ���ַ  Cs-137:%dBg/Kg	  %c
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
	LCD_Display_String(72,50,"�� ʷ �� ¼",ADDPART1,NORMAL,NORMAL); 	
	LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
	if(1 == bDisDel);
	else{
		Show_String(276,40,"<_",ADDPART1,NORMAL);  // 	 
		Show_String(276,80,"=>",ADDPART1,NORMAL);
		LCD_Display_String(264,160,"����",ADDPART1,NORMAL,NORMAL);
		LCD_Display_String(264,120,"ɾ��",ADDPART1,NORMAL,NORMAL);
	 	LCD_Display_String(8,80+(CSR_Old<<6),"#",ADDPART1,NORMAL,NORMAL);
		UpdHisRec(chPage);//��ʼʱcount=0;Page=0;��ʾ��ʼ��4��
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
				//��ʼ���ܴ洢��Ϣ��
				SystemPara.History.Record[0][0]=0;		 //����0+1
				SystemPara.History.Record[0][1]=0;
						 
				SystemPara.History.Record[0][2]=1;		 
				SystemPara.History.Record[0][3]=0;		 
				SystemPara.History.Record[0][4]=0;		
				SystemPara.History.Record[0][5]=0;		 //��λȫ���洢������Ӧ״̬λ

				SystemPara.History.Record[0][6]=0;		 //�洢�����׸�������ַΪ1
				//��ʼ������ҳ
				SystemPara.History.Record[0][7]=EMPTY;	 //��һ�洢����ΪEMPTY
				SystemPara.History.Record[0][8]=0;		 //�������洢��¼����Ϊ0
				SystemPara.History.Record[0][9]=0;		 //���״洢��ַΪ0
				//���ñ�����ȫ���洢����Ӧ״̬λ
				SystemPara.History.Record[0][10]=1; // 0 
				SystemPara.History.Record[0][11]=0;	//�������Ĵ洢״̬

				hwCurRAM=0;
				NowAddr=0;
			}else{
				NowPos=SystemPara.RecordQua;//(SystemPara.History.Record[0][0]<<8)|SystemPara.History.Record[0][1];
				hwCurRAM=SystemPara.History.Record[0][6];	//����������ʼ����
				while(1){
					Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,2);	  //������ͷ���
					if(EMPTY == SystemPara.History.Record[0][7])break;	//��һ����ָ���  ���ʾ��β����		
					(NowPos) -= SystemPara.History.Record[0][8];
					hwCurRAM  = SystemPara.History.Record[0][7];		//SystemPara.History.Record[0][7]��һ������hwCurRAM��ǰ����
				}
				k=2;  
				if(RAMdata == SystemPara.History.Record[0][8]){ //�������������С�����ѯ����������
					for(i = 0 ; i != Total_ram_Num ; i ++){
						if(0 == (SystemPara.History.Record[0][k]&(1<<(i%8))))break;
						if(7 == i%8)k++;
					}
					if(Total_ram_Num == i)break;											//δ����������ɽ�����洢��  �����洢����
					SystemPara.History.Record[0][k] |= 1<<(i%8);				//���������i��
					Write_EEprom(hwCurRAM,&i,7,1);								//�������ĸı���ָ����һ������ָ��ָ��a,
					SystemPara.History.Record[0][7] = EMPTY;						
					SystemPara.History.Record[0][8] = 0;								//��������������
					SystemPara.History.Record[0][9] = 0;								//�������׵�ַ����

					SystemPara.History.Record[0][10]= 1;
					SystemPara.History.Record[0][11]= 0;

					NowAddr=0;
					hwCurRAM=i;
				}else{
					Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[0][9],9,3);	  // �������״洢��ַ �� �����洢��״̬��
					NowAddr=SystemPara.History.Record[0][9];	
					while(1){	 //	Ѱַ�����е�β��� ����ֵNowAddr��ַ֮���24��ֵ
						NowAddr=NowAddr*PackNum+36;  //ֻ�����һ������36=12+24
						Read_EEprom(hwCurRAM,(uint8_t *)&SystemPara.History.Record[1][0],NowAddr,1); //
						if(EMPTY == SystemPara.History.Record[1][0])break; //��һ����ָ���  ���ʾ��β����
						NowAddr=SystemPara.History.Record[1][0];
					}
					i=0;k=10;
					while(1){//Ѱַ�����п�λ��
						if(0 == (SystemPara.History.Record[0][k]&(1<<(i%8)))) break;
						i++;
						if(8 == i)k++;
					}
					Write_EEprom(hwCurRAM,(uint8_t *)&i,NowAddr,1); //ָ�����ӡ���ԭ����βָ��0ָ������ָ��i
				
					SystemPara.History.Record[0][k]|=1<<(i%8); // ����״̬���Ӧ����λ��1
					NowAddr=i;
				}
			} //�洢ֵд�뻺����
			
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
			} //д�����ʱ��Ͳ�����ֵ

			SystemPara.History.Record[1][21] = EMPTY;
			SystemPara.History.Record[0][8]++;
			if(255 == SystemPara.History.Record[0][1])SystemPara.History.Record[0][0]++;
			SystemPara.History.Record[0][1]++;

			NowAddr=(NowAddr*PackNum)+12;
			Write_EEprom(0 ,(uint8_t *)&SystemPara.History.Record[0][0],0,7); //ָ�����ӡ���ԭ����βָ��0ָ������ָ��i
			Write_EEprom(hwCurRAM ,(uint8_t *)&SystemPara.History.Record[0][7],7,5); //ָ�����ӡ���ԭ����βָ��0ָ������ָ��i
			Write_EEprom(hwCurRAM ,(uint8_t *)&SystemPara.History.Record[1][0],NowAddr,PackNum); //ָ�����ӡ���ԭ����βָ��0ָ������ָ��i

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

void SaveValue(uint8_t chMode)	   						//��������������
{
	bit bSavFla;	//low �¸�������ַ��high ������������
	bSavFla=SaveDeal(chMode);
	if(1==bSavFla){
		MainMenu(chMode);
	}else{
		Clear_Windows(264,160,32,16,ADDPART1,NORMAL); 
		LCD_Display_String(100,196,"����ʧ��!",ADDPART1,NORMAL,NORMAL);
	}
}

void DeletRec(uint8_t NowPageAddr,uint8_t Ptr)
{
	uint16_t NowPos; 
	uint8_t  NowAddrAd,NowAddr,NowAddrSha=EMPTY;
	uint8_t chCurRAM=0,chCurRAMsha=EMPTY;
	NowPos=((NowPageAddr-1)<<1)+Ptr+1;
	Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][0],0,7);	  //������ͷ���
    //Ѱַ������ȷ����ǰ��ʾ����  NowPos  	//����ֵ  SystemPara.History.Record[0][7]��һ������hwCurRAMsha��ǰ����
	chCurRAM= SystemPara.History.Record[0][6];	 //������ͷ���
	while(1){	
		Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,2);	  
		if(NowPos <= SystemPara.History.Record[0][8])break;		
		else NowPos -= SystemPara.History.Record[0][8];
		chCurRAMsha=chCurRAM;
		chCurRAM= SystemPara.History.Record[0][7];
	}
	Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][9],9,3);	  // �������״洢��ַ �� �����洢��״̬��

	if(1 == SystemPara.History.Record[0][8]){//��ǰ������һ����¼����ɾ������
		if(chCurRAM == SystemPara.History.Record[0][6])SystemPara.History.Record[0][6]=SystemPara.History.Record[0][7];	  //���ɾ����������������
		else{	//����������һ�����ĵ�ַ����ǰһ����  ʵ��
			chCurRAM=SystemPara.History.Record[0][7];//chCurRAM������װ����һ����ֵ
			Read_EEprom(chCurRAMsha,(uint8_t *)&SystemPara.History.Record[0][7],7,5);
			SystemPara.History.Record[0][7]=chCurRAM;//chCurRAM������װ����һ����ֵ
		}
		SystemPara.History.Record[0][2+(chCurRAM/8)] &= ~(1<<(chCurRAM%8)); //��������״̬λ����
		chCurRAM=chCurRAMsha;
	}else{
		NowAddr=SystemPara.History.Record[0][9];
		while(1){//Ѱַ����ǰ������ַ������NowAddrSha����ǰ��ַ֮ǰ��ָ�룩NowAddr����ǰָ�룩 SystemPara.History.Record[1][24]��ָ��
			NowAddrAd=NowAddr*PackNum+PackNum+11;
			Read_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[1][0],NowAddrAd,1); //
			NowPos--;
			if(0==NowPos)break;
			NowAddrSha=NowAddrAd;
			NowAddr=SystemPara.History.Record[1][0];
		}
		if(EMPTY == NowAddrSha){SystemPara.History.Record[0][9]=SystemPara.History.Record[1][0];}	  //Ϊ0,˵��ɾ����λ�����������׽ڵ�
		else Write_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[1][0],NowAddrSha,1);	//����Ϊ�м�ڵ� ��NowAddrSha	SystemPara.History.Record[1][0]
		
		SystemPara.History.Record[0][8]--;
		SystemPara.History.Record[0][10+NowAddr/8] &= ~(1<<(NowAddr%8));
	}
	if(0 == SystemPara.History.Record[0][1])SystemPara.History.Record[0][0]--;
	SystemPara.History.Record[0][1]--;

	Write_EEprom(0,(uint8_t *)&SystemPara.History.Record[0][0],0,7);
	Write_EEprom(chCurRAM,(uint8_t *)&SystemPara.History.Record[0][7],7,5);

//	Read_EEprom(0,(uint8_t *)&sd[8],10,1);	  // �����������ݴ洢״̬��
//	Read_EEprom(0,(uint8_t *)&sd[9],2,1);	  // �������״洢��ַ �� �����洢��״̬��
}

void HistoryDel(uint8_t chKey,uint8_t NowPageAddr,uint8_t Ptr)//)							//ɾ����ʷ��¼����		0X51
{
	switch(chKey){
	case 3: chInterFaceState=0x50;  //ȷ��ɾ�� CSR_Old,chPage
			DeletRec(NowPageAddr,Ptr);
			Read_EEprom(0,(uint8_t *)&SystemPara.RecordQua,0,2);   //
			if(CSR_Old+1+((chPage-1)<<1) >= SystemPara.RecordQua){
				CSR_Old=(SystemPara.RecordQua-1)%2;
				chPage = SystemPara.RecordQua/2;
				if(0 == CSR_Old)chPage++;
			}
			MainMenu(5);
			break;//ˢ��ҳ��				  
	case 5: MainMenu(5);
			break;
	default:break; 	
	}
}

void HistoryPro(uint8_t chKey)							// ��ʷ��¼����			0X50
{
	switch(chKey){
	case 1 : if(1==bDisDel) break;	  //����
			 if(0==CSR_Old){	//�ж�λ���Ƿ�λ������
				 Clear_Windows(8  ,80+(CSR_Old<<6),8,16,ADDPART1,NORMAL);
				 Clear_Windows(232,80+(CSR_Old<<6),8,64,ADDPART1,NORMAL); //�ǡ������λ�õļ�ͷ
				 if(1>=chPage){//�ж��Ƿ�λ����ҳ��
					 CSR_Old=(SystemPara.RecordQua-1)%2;	  //�ǡ����Ƿ����1ҳ�����������ϵ����һ�����ݲ���ҳ������ˢ��ҳ��
					 if(2<SystemPara.RecordQua){
						 chPage=SystemPara.RecordQua/2;
						 if(1 == CSR_Old);
						 else chPage++;   //��תҳ�浽���һҳ	ˢ��ҳ��
						 UpdHisRec(chPage);//
					 } 
				 }else{
					 chPage--;//����λ����ҳ   ҳ��	-1.
					 CSR_Old=1;	//���ص���ҳ���һ������  ��ˢ��ҳ��
					 UpdHisRec(chPage);//
			 	 }
			 }else CSR_Old--; //�������С���ֱ������
			 ShowNum(CSR_Old,chPage-1);
			 if(1!=CSR_Old){
				Clear_Windows(8,80+((CSR_Old+1)<<6),8,16,ADDPART1,NORMAL);//�������С��������м�ͷ			 
				Clear_Windows(232,80+((CSR_Old+1)<<6),8,64,ADDPART1,NORMAL);
			 }
			 LCD_Display_String(8,80+(CSR_Old<<6),"#",ADDPART1,NORMAL,NORMAL);
			 Draw_Scrollbar(232,80+(CSR_Old<<6),64,0,ADDPART1,NORMAL); 
			 break;
	case 2 : if(1 == bDisDel) break;
			 if(1 == CSR_Old|((((chPage-1)<<1)+CSR_Old+1) >= SystemPara.RecordQua)){	//����	�ж��Ƿ���β��(�����л��������һҳ�������)
				 Clear_Windows(8,80+(CSR_Old<<6),8,16,ADDPART1,NORMAL);	  		//�ǡ�����β�м�ͷ
				 Clear_Windows(232,80+(CSR_Old<<6),8,64,ADDPART1,NORMAL);
				 if(SystemPara.RecordQua <= (((chPage-1)<<1)+CSR_Old+1))	chPage=1;	//�ж��Ƿ���βҳ��������ת����ҳ
				 else chPage++;	//���ǡ���ҳ��+1
				 UpdHisRec(chPage);	
				 CSR_Old=0;			   //�����Ƿ���βҳ������ת����ҳ�����У�����ҳ��
			 }else CSR_Old++; //����β�У�����+1 �жϸ��к��Ƿ���������
			 ShowNum(CSR_Old,chPage-1);
			 if(0<CSR_Old){
			 	Clear_Windows(8	 ,80+((CSR_Old-1)<<6),8,16,ADDPART1,NORMAL);	//����β�С���������
				Clear_Windows(232,80+((CSR_Old-1)<<6),8,64,ADDPART1,NORMAL); 
			 }
			 LCD_Display_String(8,80+(CSR_Old<<6),"#",ADDPART1,NORMAL,NORMAL);
			 Draw_Scrollbar(232,80+(CSR_Old<<6),64,0,ADDPART1,NORMAL);
			 break;
	case 3 : if(1==bDisDel) break;
			 chInterFaceState = 0x51;
			 Clear_Windows(248,40,64,60,ADDPART1,NORMAL);//���ײ���
			 LCD_Display_String(264,120,"ȷ��",ADDPART1,NORMAL,NORMAL);
			 LCD_Display_String(264,200,"����",ADDPART1,NORMAL,NORMAL);
			 LCD_Display_String(88,128," ɾ�� ? ",ADDPART1,REVERSE,REVERSE); break;
	case 4 : break;
	case 5 : InterfaceInit(); break;
	default: break;
	}
}


