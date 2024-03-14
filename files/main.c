#include <reg52.h>
#include <intrins.h>
#include "main.h"
#include "at24c02.h"
#include "lcd12864.h"
#include "rc522.h"
#include "password.h"
#include "key.h"
 
#define buffer1ID  0x01
#define buffer2ID  0x02

uchar Card_type[20];	   
sbit Relay = P3^6;//�̵�������
sbit Beep = P3^2;	//������
extern unsigned char times[];
///uchar  K; 
uchar  sec, Member = 1;

uchar  code buffer[User][6] = {{"000000"}};    //��ʼ����    
        
uchar dz[4];
uchar time[4];			 
uchar mima[6];
uchar mimag[6];
uchar mimaID[6]={1,2,3,4,5,6};
uchar Address1=20,tempx=20; 
uchar Address2=20;          

uchar  Key;
uchar  flag_id;      
uint PageID;
uchar data K_confirmma=11;
uchar sum[2];
int summaf,summas;

uchar  xdata UserPassword[7]={0};//����
uchar  xdata  DSY_BUFFER[16]={0};//����
uchar  ID_ASC[8];
uchar  IDn[8];
uchar  numaddress[6]={0x38,0x40,0x48,0x50,0x58,0x60};
uchar  idnum;      

void delayms(unsigned int z)   //ms��ʱ
{
	unsigned int x,y;
	for(x=z; x>0; x--)
		for(y=110; y>0; y--);
}
/*************************************************************************************************
void delay(uint tt)
{  
	uchar i;
   while(tt--)
   {
   for(i=0;i<125;i++);
   }
}
  
unsigned char Keycan(void) //����ɨ����� P1.0--P1.3Ϊ���� P1.4--P1.7Ϊ����
{
  unsigned char rcode, ccode;
  P1 = 0xF0;      // ��ȫ0��ɨ���룬��������
  if((P1&0xF0) != 0xF0) // ���м�����
  {
    delay(1);// ��ʱȥ���� 
    if((P1&0xF0) != 0xF0)
    {  rcode = 0xFE;         // ����ɨ���ֵ
    while((rcode&0x10) != 0)
    {
       P1 = rcode;         // �����ɨ����
      if((P1&0xF0) != 0xF0) // �����м�����
      {
       ccode = (P1&0xF0)|0x0F;
       //do{;}
	   while((P1&0xF0) != 0xF0); //�ȴ����ͷ�
       return ((~rcode) + (~ccode)); // ���ؼ�����
      }
    else
    rcode = (rcode<<1)|0x01; // ��ɨ��������һλ
    }
    }
  }  
  return 0; // �޼����£�����ֵΪ0
}
        
void KeyDeal(unsigned char Key)
{ 
	if(Key!=0)
	{   
		switch(Key)
		{
			case 0x11: K=1; break;	 
			case 0x21: K=2; break;  
			case 0x41: K=3; break;  
			case 0x81: break;       
			case 0x12: K=4; break;
			case 0x22: K=5; break;
			case 0x42: K=6; break;
			case 0x82: break;		
			case 0x14: K=7; break;
			case 0x24: K=8; break;
			case 0x44: K=9; break;
			case 0x84:  break;		
			case 0x18: K=11;break;		
			case 0x28: K=0; break;
			case 0x48: K=12;break;
			case 0x88: break;		
			default: break;
		}
	}
}
*****************************************************************************************************/   
      
void UART_Init()	//���� : ��Ƭ�����ڳ�ʼ��
{
	SCON= 0x50;               //��ʽ1	  REN=1; �������
	PCON=0x00;                //SMOD=0
	TMOD= 0x21;               //��ʱ��1��ʱ��ʽ2
	TH1= 0xFD;                //11.0592MHz  ģ��Ĭ�ϲ�����Ϊ9600bps
	TL1= 0xFD;										 
	TR1= 1;                   //������ʱ�� 
	EA=1;       
}      

void change_Pin()	//������
{
	uchar i, j=0, Pin1[6], Pin2[6];
	uchar k,temp;
	Lcd_ClearTXT(); //Һ�����
	PutStr(1,1,"������������");
	for(i=0;i<6;i++) 
		Pin1[i] = 0;									
	Key = Keycan(); //�����󰴼�
	while(Key != K_confirm)
	{
		Key = Keycan();
		KeyDeal(Key);
		delay(30);				                              
		if(Key==0)K=10;
		if((K>=0)&&(K<=9))	//�޸�����0��9
	     {
	      mima1[j]=K;
		  if(j<6)
		  {	
				WriteCommand(0x89+j);		//ָ����������ʾλ��
				WriteData(0x0f);	
		  }
	      ++j;
		  if(j==7)
	      j=6;												   //@@#yinhuang
	     }   //��ʾLCD12864������ʾ 
		 if(K==34)		//����ɾ����
		  {
		    if(j==0)
			{
				WriteCommand(0x89);		//ָ����������ʾλ��
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //ָ����������ʾλ��
				WriteData(0x20);
			}
		}
	}
	Lcd_ClearTXT();//Һ�����
	j=0;
	PutStr(1,0,"���ٴ�����������");
	for(i=0;i<6;i++) mima2[i]=0;									
	   Key=Keycan();   //�����󰴼�
      while(Key!=K_confirm)
	   {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))	//�޸�����0��9
	     {
	      mima2[j]=K;
		  if(j<6)
		  {		
				WriteCommand(0x89+j);		//ָ����������ʾλ��
				WriteData(0x0f);	
		  }
	      ++j;
		  if(j==7)
	      j=6;												   //@@#yinhuang
	     }   //��ʾLCD12864������ʾ 
		 if(K==34)		//����ɾ����
		  {
		    if(j==0)
			{
				WriteCommand(0x89);		//ָ����������ʾλ��
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //ָ����������ʾλ��
				WriteData(0x20);
			}
		}
	}
	Lcd_ClearTXT(); //Һ�����
	if((mima1[0]==mima2[0])&&(mima1[1]==mima2[1])&&(mima1[2]==mima2[2])&&  \
	   (mima1[3]==mima2[3])&&(mima1[4]==mima2[4])&&(mima1[5]==mima2[5]))
	{
		for(i=0;i<6;i++)
		mimaID[i]=mima1[i];
		/////////////////////////////////////////////////////
		for(i=0;i<6;i++)           //����������6λ����
		{
			UserPassword[i]=mima1[i]+0x30;                          	
		}
		temp=(Member-1)*10;	
		delayms(5);
		for(k=0;k<6;k++)
		{
			at24c02_write(temp,UserPassword[k]);
			delayms(10);
			temp++;
		}  
		//////////////////////////////////////////////////////
		PutStr(0,1,"�����޸ĳɹ�"); 
		PutStr(3,0,"  �����������");
		while(Keycan()==0);
	}
	else
	{
		PutStr(0,0,"  �����޸�ʧ��  ");
		PutStr(1,0,"������������벻"); 
		PutStr(2,0,"һ�£������²���");       
		PutStr(3,0,"  �����������"); 
		while(Keycan()==0);
	}
	Lcd_ClearTXT();
}
void LCD_Data(uchar LCDbyte,uchar i )  //��ʾ����
{
	 uchar bai,shi;	//ʮ����
	 bai=LCDbyte/100;
	 shi=LCDbyte%100/10;
	 if(i==0)
	{
	 ID_ASC[0]=bai+0x30;
	 ID_ASC[1]=shi+0x30;}
	 if(i==1)
	{
	 ID_ASC[2]=bai+0x30;
	 ID_ASC[3]=shi+0x30;}
	 if(i==2)
	{
	 ID_ASC[4]=bai+0x30;
	 ID_ASC[5]=shi+0x30;}
	 if(i==3)
	{
	 ID_ASC[6]=bai+0x30;
	 ID_ASC[7]=shi+0x30;}

}	

uchar IC_READ( void )	//����
{
    uchar i;
    if( PcdRequest( PICC_REQIDL, Card_type ) != MI_OK )    //Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�
    {
            return 0;
    }
    if( PcdAnticoll( Card_type ) != MI_OK )                  //����ײ�����ؿ������к� 4�ֽ�
    {
        return 0;
    }										
    for(i=0; i<4; i++) //��ID��ת����ASCII
    {
		LCD_Data(Card_type[i],i);
    }
    return 1;
}


void checkcard(void)	//У�鿨
{
	uchar j;
	uchar IDAddress;
	IC_READ();
	for(idnum=0;idnum<6;idnum++)
	{
		IDAddress= numaddress[idnum];
		for(j=0;j<8;j++)
		{
			IDn[j]=	at24c02_read(IDAddress);
			delayms(10);
			IDAddress++;
		}
		 
		if( (ID_ASC[0]==IDn[0])&&(ID_ASC[1]==IDn[1])&&(ID_ASC[2]==IDn[2])&&(ID_ASC[3]==IDn[3])&&  \
		    (ID_ASC[4]==IDn[4])&&(ID_ASC[5]==IDn[5])&&(ID_ASC[6]==IDn[6])&&(ID_ASC[7]==IDn[7]) )
		  break;
	}
	if(idnum<6) flag_id=1;
	else flag_id=0; 
}

void shuaka(void)	//ˢ��
{
	if(IC_READ()==1)
	{
		checkcard();    
		if(flag_id==1)//ˢ��ɨ��
		{
			flag_id=0;
			Lcd_ClearTXT();
			WriteCommand(0x89);
			WriteCommand(0x8B);
			PutStr(1,2,"���Ѵ�");
			Relay=0;	//�̵�����
			PutStr(2,0,"���ţ�");
			WriteCommand(0x8B);//��ʾλ��
			WriteData(ID_ASC[0]); //��ʾ��λ����
			WriteData(ID_ASC[1]);
			WriteData(ID_ASC[2]);
			WriteData(ID_ASC[3]);
			WriteData(ID_ASC[4]);
			WriteData(ID_ASC[5]);
			WriteData(ID_ASC[6]);
			WriteData(ID_ASC[7]);
			delay(2500); 
			Relay=1;	//�̵����ر�
			delay(1000);
			welcome();
		}
		else		 //δע�Ῠ��ʾ
		{
			Lcd_ClearTXT();//��ʾ���
		 	PutStr(1,2,"��δע��");
			PutStr(2,0,"���ţ�");
			WriteCommand(0x8B);	//��ʾλ��
			WriteData(ID_ASC[0]);  //��ʾ��λ����
			WriteData(ID_ASC[1]);
			WriteData(ID_ASC[2]);
			WriteData(ID_ASC[3]);
			WriteData(ID_ASC[4]);
			WriteData(ID_ASC[5]);
			WriteData(ID_ASC[6]);
			WriteData(ID_ASC[7]);
			Beep=0;
			delay(300);
			Beep=1;	  //�������ر�
			delay(3000);
			
			welcome();
		}
	}
}

void kapianguanli(void)	//��Ƭ����
{
	uchar numID[6]={0,1,2,3,4,5},i=0,k;
	uchar j,temp00;
	Lcd_ClearTXT();
	PutStr(0,2,"��ˢ��");
	KeyDeal(Keycan());
	while( Key!=K_exit )
	{
		WriteCommand(0x89);
		WriteData('I');
		WriteData('D');
		WriteData(0X30+numID[i]);
		WriteData(':');
		
		IC_READ();
		Key=Keycan();
	    KeyDeal(Key);
		temp00=numaddress[i];
		for(k=0;k<8;k++)
		{
			IDn[k]=	at24c02_read(temp00);
			delayms(10);
			temp00++;
		}
		temp00=numaddress[i];
		if((IDn[0]==0x30)&&(IDn[1]==0x30)&&(IDn[2]==0x30)&&(IDn[3]==0x30)&&(IDn[4]==0x30)&&(IDn[5]==0x30)&&(IDn[6]==0x30)&&(IDn[7]==0x30))
		{
			if(IC_READ()==1)
			{
			    WriteCommand(0x8B);
				WriteData(ID_ASC[0]);
				WriteData(ID_ASC[1]);
				WriteData(ID_ASC[2]);
				WriteData(ID_ASC[3]);
				WriteData(ID_ASC[4]);
				WriteData(ID_ASC[5]);
				WriteData(ID_ASC[6]);
				WriteData(ID_ASC[7]);
			}
			else
			{
				WriteCommand(0x8B);
				WriteData(' ');
				WriteData(' ');
				WriteData(' ');
				WriteData(' ');
				WriteData(' ');
				WriteData(' ');
				WriteData(' ');
				WriteData(' ');
			}
	    	}			
		else
		{
			WriteCommand(0x8B);
			WriteData(IDn[0]);
			WriteData(IDn[1]);
			WriteData(IDn[2]);
			WriteData(IDn[3]);
			WriteData(IDn[4]);
			WriteData(IDn[5]);
			WriteData(IDn[6]);
			WriteData(IDn[7]);	
		}
		if(Key==0x82)	//�洢��
		{
			 IC_READ();
			 if(IC_READ()==1)
			 {
			 	 checkcard();
				 if(flag_id==1)
				 {	 
				 	 flag_id=0;
				 	 Lcd_ClearTXT();
				 	 PutStr(0,2,"����ע��");
					 PutStr(1,2,"�뻻��Ƭ");
					 PutStr(2,1,"�����������");
					 while(Keycan()==0);
					 Lcd_ClearTXT();
					 PutStr(0,2,"��ˢ��");
				 }
				 else
				 {
				 	for(j=0;j<8;j++)
					 {
					 at24c02_write(temp00,ID_ASC[j]);
				     delayms(10);
					 temp00++;
					 }
					 Lcd_ClearTXT();
					 PutStr(0,2,"ע��ɹ�");
					 PutStr(1,1,"�����������");
					 while(Keycan()==0);
					 Lcd_ClearTXT();
					 PutStr(0,2,"��ˢ��");
				 }
			 }
		}
		if(Key==0x84)	//ɾ����
		{
			 if((IDn[0]==0x30)&&(IDn[1]==0x30)&&(IDn[2]==0x30)&&(IDn[3]==0x30)&&(IDn[4]==0x30)&&(IDn[5]==0x30)&&(IDn[6]==0x30)&&(IDn[7]==0x30))
			 {
			 	 Lcd_ClearTXT();
			 	 PutStr(1,2,"δ�洢��");
				 PutStr(2,1,"�����������");
				 while(Keycan()==0);
				 Lcd_ClearTXT();
				 PutStr(0,2,"��ˢ��");
			 }
			 else
			{
				for(j=0;j<8;j++)
				 {
				 IDn[j]=0;
				 at24c02_write(temp00,IDn[j]+0x30);
				 delayms(10);
				 temp00++;
				 }
				 delayms(50);
				 Lcd_ClearTXT();
				 PutStr(0,2,"ɾȥ�ɹ�");
				 PutStr(1,1,"�����������");
				 while(Keycan()==0);
				 Lcd_ClearTXT();
				 PutStr(0,2,"��ˢ��");
			 }	
		}
		if(Key==0x81)	//������л�
		{
			i++;
		}
		if(i==6)
		{
			i=0;
		}
	
	}
}

void  Administrator(void)//����Աģʽ
{ 
	uchar i,j=0,x=1;
   	uchar Right_flag;
    Lcd_ClearTXT();
	PutStr(1,1,"���������룺");	
	for(i=0;i<6;i++)mima[i]=0;									
	Key=Keycan();
    while(Key!=K_confirm)
	 {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))
	     {
	      mima[j]=K;
		  if(j<6)
		  {	
		   WriteCommand(0x89+j);		//ָ����������ʾλ��
	       WriteData(0x0f);
		  }
	      ++j;
		  if(j==7)
	      j=6;												  
	     }   //��ʾLCD12864������ʾ 
		 if(K==11)		//����ɾ����
		  {
		    if(j==0)
			{
		      WriteCommand(0x89);		//ָ����������ʾλ��
	          WriteData(0x20);
			}
			else
			{
			  --j;
		      WriteCommand(0x89+j);	    //ָ����������ʾλ��
	          WriteData(0x20);
			}
		  }
	   }
    Lcd_ClearTXT();//��ʾ���

	  for(i=0;i<6;i++)
         {
            UserPassword[i]=mima[i]+0x30;
          } 
	 if(j==6){Right_flag=PassWord_Chack(); }
	 if (Right_flag==1)
	 {
	  	Right_flag=0;

		while(Key!=K_exit)
      {	 
		PutStr(0,0,"����1 : ���Ž�"); 	//�˵�������ʾ
		PutStr(1,0,"����2 : �޸�����");
		PutStr(2,0,"����3 : ��Ƭ����");
		PutStr(3,0,"����4 : �˳�ϵͳ");
		Key=Keycan();
	    KeyDeal(Key);  
	    switch(K)
	    {
	     case 1:    { Lcd_ClearTXT();
            		  PutStr(1,2,"���Ѵ�");  //����1 ���Ž�
				      Relay=0;
				      delay(2500); 
				      Relay=1;	
				      delay(1000);
				      Key=0;K=0;
				      PutStr(0,0,"����1 : ���Ž�"); 
		              PutStr(1,0,"����2 : �޸�����");
		              PutStr(2,0,"����3 : ��Ƭ����");
		              PutStr(3,0,"����4 : �˳�ϵͳ");}break;  
		 case 4:  {  Right_flag=0; K=0;Key=K_exit;}break;  //����4 �˳�ϵͳ
		 case 3:	kapianguanli();Key=0;continue;  	//����3 ��Ƭ����
		 case 2:    gaimima();break;  //����2 �޸�����
	     default: break;
	    }
      }
     }
    else   //�������
    {
        Beep=0;//�򿪷�����
		delay(300);
		Beep=1;
		PutStr(1,2,"�������");
		PutStr(2,0,"  �����²�����"); 
		PutStr(3,0,"  �����������"); 
	    while(Keycan()==0);
    }
    Key=0;
  
    welcome();	//��ʾ��ʼ����

    j=0;
}
     
void main()
{ 	
	uchar i=0,j=0,x=1;//check;
   	uchar Right_flag;
	Init_rc522();      //RC522��ʼ��
	UART_Init();	   //���ڳ�ʼ��		  
	at24c02_Init();     //24C02��ʼ��
	
	delay(10);

	Lcd_Init();	//LCD12864��ʼ��				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	welcome();	//��ʾ��ʼ����
	
	while(1)  
	{ 
		Key=Keycan();//�������								
		while(Key!=0X48)	     //�ȴ�����ȷ����������
		{	 
			shuaka();
			Key=Keycan();
			KeyDeal(Key);
			delay(30);
			if(Key==0x88)
			{
				Administrator();
			}		                              
			if(Key==0)	   //û�а������£�K��ֵΪ10,ʲô����ִ��
				K=10;
			if((K>=0)&&(K<=9))	 //�����ּ�����
			{
				mima[j]=K;		 //������ֵ��������
				if(j<6)
				{	
					WriteCommand(0x99+j);		//ָ����������ʾλ��*
					WriteData(0x0f);	
				}
				++j;
				if(j==7)
				j=6;												  
			}  
			else if(K==11)		//����ɾ����
			{
				if(j==0)
				{
					WriteCommand(0x99);		//ָ����������ʾһ���ո�
					WriteData(0x20);
				}
				else
				{
					--j;
					WriteCommand(0x99+j);   //ָ����������ʾһ���ո���ɾ��һ��*
					WriteData(0x20);
				}
			}
		}
		 for(i=0;i<6;i++)
         {
            UserPassword[i]=mima[i]+0x30;
	
          } 
		if(j==6)			 //�˶�����
		{
			Right_flag=PassWord_Chack();
		}
		if (Right_flag==1)   //������ȷ
		{
			Right_flag=0;
			Lcd_ClearTXT();
			PutStr(1,2,"���Ѵ�");
			Relay=0;
			delay(2500); 
			Relay=1;	
			delay(1000);
			
			welcome();	//��ʾ��ʼ����

			j=0;
	     }
	    else				//������󣬼�������whileѭ����⡢Ѱ�����ȴ�ȷ��������
	    {
	    Lcd_ClearTXT();
			PutStr(1,2,"�������");
			PutStr(2,0,"  �����²�����"); 
			Beep=0;
			delay(300);
			Beep=1;
			delay(2000);
		    
			welcome();	//��ʾ��ʼ����

			j=0;	
	    }
		//Key=0;			   //����ֵ����				  
	}	
}