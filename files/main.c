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
sbit Relay = P3^6;//继电器控制
sbit Beep = P3^2;	//蜂鸣器
extern unsigned char times[];
///uchar  K; 
uchar  sec, Member = 1;

uchar  code buffer[User][6] = {{"000000"}};    //初始密码    
        
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

uchar  xdata UserPassword[7]={0};//密码
uchar  xdata  DSY_BUFFER[16]={0};//缓存
uchar  ID_ASC[8];
uchar  IDn[8];
uchar  numaddress[6]={0x38,0x40,0x48,0x50,0x58,0x60};
uchar  idnum;      

void delayms(unsigned int z)   //ms延时
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
  
unsigned char Keycan(void) //按键扫描程序 P1.0--P1.3为行线 P1.4--P1.7为列线
{
  unsigned char rcode, ccode;
  P1 = 0xF0;      // 发全0行扫描码，列线输入
  if((P1&0xF0) != 0xF0) // 若有键按下
  {
    delay(1);// 延时去抖动 
    if((P1&0xF0) != 0xF0)
    {  rcode = 0xFE;         // 逐行扫描初值
    while((rcode&0x10) != 0)
    {
       P1 = rcode;         // 输出行扫描码
      if((P1&0xF0) != 0xF0) // 本行有键按下
      {
       ccode = (P1&0xF0)|0x0F;
       //do{;}
	   while((P1&0xF0) != 0xF0); //等待键释放
       return ((~rcode) + (~ccode)); // 返回键编码
      }
    else
    rcode = (rcode<<1)|0x01; // 行扫描码左移一位
    }
    }
  }  
  return 0; // 无键按下，返回值为0
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
      
void UART_Init()	//功能 : 单片机串口初始化
{
	SCON= 0x50;               //方式1	  REN=1; 允许接收
	PCON=0x00;                //SMOD=0
	TMOD= 0x21;               //定时器1定时方式2
	TH1= 0xFD;                //11.0592MHz  模块默认波特率为9600bps
	TL1= 0xFD;										 
	TR1= 1;                   //启动定时器 
	EA=1;       
}      

void change_Pin()	//改密码
{
	uchar i, j=0, Pin1[6], Pin2[6];
	uchar k,temp;
	Lcd_ClearTXT(); //液晶清除
	PutStr(1,1,"请输入新密码");
	for(i=0;i<6;i++) 
		Pin1[i] = 0;									
	Key = Keycan(); //检测矩阵按键
	while(Key != K_confirm)
	{
		Key = Keycan();
		KeyDeal(Key);
		delay(30);				                              
		if(Key==0)K=10;
		if((K>=0)&&(K<=9))	//修改密码0到9
	     {
	      mima1[j]=K;
		  if(j<6)
		  {	
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);	
		  }
	      ++j;
		  if(j==7)
	      j=6;												   //@@#yinhuang
	     }   //显示LCD12864并行显示 
		 if(K==34)		//按了删除键
		  {
		    if(j==0)
			{
				WriteCommand(0x89);		//指定第三行显示位置
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //指定第三行显示位置
				WriteData(0x20);
			}
		}
	}
	Lcd_ClearTXT();//液晶清除
	j=0;
	PutStr(1,0,"请再次输入新密码");
	for(i=0;i<6;i++) mima2[i]=0;									
	   Key=Keycan();   //检测矩阵按键
      while(Key!=K_confirm)
	   {
	     Key=Keycan();
	     KeyDeal(Key);
	     delay(30);				                              
	     if(Key==0)K=10;
	     if((K>=0)&&(K<=9))	//修改密码0到9
	     {
	      mima2[j]=K;
		  if(j<6)
		  {		
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);	
		  }
	      ++j;
		  if(j==7)
	      j=6;												   //@@#yinhuang
	     }   //显示LCD12864并行显示 
		 if(K==34)		//按了删除键
		  {
		    if(j==0)
			{
				WriteCommand(0x89);		//指定第三行显示位置
				WriteData(0x20);
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //指定第三行显示位置
				WriteData(0x20);
			}
		}
	}
	Lcd_ClearTXT(); //液晶清除
	if((mima1[0]==mima2[0])&&(mima1[1]==mima2[1])&&(mima1[2]==mima2[2])&&  \
	   (mima1[3]==mima2[3])&&(mima1[4]==mima2[4])&&(mima1[5]==mima2[5]))
	{
		for(i=0;i<6;i++)
		mimaID[i]=mima1[i];
		/////////////////////////////////////////////////////
		for(i=0;i<6;i++)           //密码限制在6位以内
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
		PutStr(0,1,"密码修改成功"); 
		PutStr(3,0,"  按任意键继续");
		while(Keycan()==0);
	}
	else
	{
		PutStr(0,0,"  密码修改失败  ");
		PutStr(1,0,"两次输入的密码不"); 
		PutStr(2,0,"一致，请重新操作");       
		PutStr(3,0,"  按任意键继续"); 
		while(Keycan()==0);
	}
	Lcd_ClearTXT();
}
void LCD_Data(uchar LCDbyte,uchar i )  //显示卡号
{
	 uchar bai,shi;	//十进制
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

uchar IC_READ( void )	//读卡
{
    uchar i;
    if( PcdRequest( PICC_REQIDL, Card_type ) != MI_OK )    //寻天线区内未进入休眠状态的卡，返回卡片类型 2字节
    {
            return 0;
    }
    if( PcdAnticoll( Card_type ) != MI_OK )                  //防冲撞，返回卡的序列号 4字节
    {
        return 0;
    }										
    for(i=0; i<4; i++) //卡ID号转化成ASCII
    {
		LCD_Data(Card_type[i],i);
    }
    return 1;
}


void checkcard(void)	//校验卡
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

void shuaka(void)	//刷卡
{
	if(IC_READ()==1)
	{
		checkcard();    
		if(flag_id==1)//刷卡扫描
		{
			flag_id=0;
			Lcd_ClearTXT();
			WriteCommand(0x89);
			WriteCommand(0x8B);
			PutStr(1,2,"门已打开");
			Relay=0;	//继电器打开
			PutStr(2,0,"卡号：");
			WriteCommand(0x8B);//显示位置
			WriteData(ID_ASC[0]); //显示八位卡号
			WriteData(ID_ASC[1]);
			WriteData(ID_ASC[2]);
			WriteData(ID_ASC[3]);
			WriteData(ID_ASC[4]);
			WriteData(ID_ASC[5]);
			WriteData(ID_ASC[6]);
			WriteData(ID_ASC[7]);
			delay(2500); 
			Relay=1;	//继电器关闭
			delay(1000);
			welcome();
		}
		else		 //未注册卡显示
		{
			Lcd_ClearTXT();//显示清除
		 	PutStr(1,2,"卡未注册");
			PutStr(2,0,"卡号：");
			WriteCommand(0x8B);	//显示位置
			WriteData(ID_ASC[0]);  //显示八位卡号
			WriteData(ID_ASC[1]);
			WriteData(ID_ASC[2]);
			WriteData(ID_ASC[3]);
			WriteData(ID_ASC[4]);
			WriteData(ID_ASC[5]);
			WriteData(ID_ASC[6]);
			WriteData(ID_ASC[7]);
			Beep=0;
			delay(300);
			Beep=1;	  //蜂鸣器关闭
			delay(3000);
			
			welcome();
		}
	}
}

void kapianguanli(void)	//卡片管理
{
	uchar numID[6]={0,1,2,3,4,5},i=0,k;
	uchar j,temp00;
	Lcd_ClearTXT();
	PutStr(0,2,"请刷卡");
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
		if(Key==0x82)	//存储卡
		{
			 IC_READ();
			 if(IC_READ()==1)
			 {
			 	 checkcard();
				 if(flag_id==1)
				 {	 
				 	 flag_id=0;
				 	 Lcd_ClearTXT();
				 	 PutStr(0,2,"卡已注册");
					 PutStr(1,2,"请换卡片");
					 PutStr(2,1,"按任意键继续");
					 while(Keycan()==0);
					 Lcd_ClearTXT();
					 PutStr(0,2,"请刷卡");
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
					 PutStr(0,2,"注册成功");
					 PutStr(1,1,"按任意键继续");
					 while(Keycan()==0);
					 Lcd_ClearTXT();
					 PutStr(0,2,"请刷卡");
				 }
			 }
		}
		if(Key==0x84)	//删除卡
		{
			 if((IDn[0]==0x30)&&(IDn[1]==0x30)&&(IDn[2]==0x30)&&(IDn[3]==0x30)&&(IDn[4]==0x30)&&(IDn[5]==0x30)&&(IDn[6]==0x30)&&(IDn[7]==0x30))
			 {
			 	 Lcd_ClearTXT();
			 	 PutStr(1,2,"未存储卡");
				 PutStr(2,1,"按任意键继续");
				 while(Keycan()==0);
				 Lcd_ClearTXT();
				 PutStr(0,2,"请刷卡");
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
				 PutStr(0,2,"删去成功");
				 PutStr(1,1,"按任意键继续");
				 while(Keycan()==0);
				 Lcd_ClearTXT();
				 PutStr(0,2,"请刷卡");
			 }	
		}
		if(Key==0x81)	//卡序号切换
		{
			i++;
		}
		if(i==6)
		{
			i=0;
		}
	
	}
}

void  Administrator(void)//管理员模式
{ 
	uchar i,j=0,x=1;
   	uchar Right_flag;
    Lcd_ClearTXT();
	PutStr(1,1,"请输入密码：");	
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
		   WriteCommand(0x89+j);		//指定第三行显示位置
	       WriteData(0x0f);
		  }
	      ++j;
		  if(j==7)
	      j=6;												  
	     }   //显示LCD12864并行显示 
		 if(K==11)		//按了删除键
		  {
		    if(j==0)
			{
		      WriteCommand(0x89);		//指定第三行显示位置
	          WriteData(0x20);
			}
			else
			{
			  --j;
		      WriteCommand(0x89+j);	    //指定第三行显示位置
	          WriteData(0x20);
			}
		  }
	   }
    Lcd_ClearTXT();//显示清除

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
		PutStr(0,0,"按键1 : 打开门禁"); 	//菜单界面显示
		PutStr(1,0,"按键2 : 修改密码");
		PutStr(2,0,"按键3 : 卡片管理");
		PutStr(3,0,"按键4 : 退出系统");
		Key=Keycan();
	    KeyDeal(Key);  
	    switch(K)
	    {
	     case 1:    { Lcd_ClearTXT();
            		  PutStr(1,2,"门已打开");  //按键1 打开门禁
				      Relay=0;
				      delay(2500); 
				      Relay=1;	
				      delay(1000);
				      Key=0;K=0;
				      PutStr(0,0,"按键1 : 打开门禁"); 
		              PutStr(1,0,"按键2 : 修改密码");
		              PutStr(2,0,"按键3 : 卡片管理");
		              PutStr(3,0,"按键4 : 退出系统");}break;  
		 case 4:  {  Right_flag=0; K=0;Key=K_exit;}break;  //按键4 退出系统
		 case 3:	kapianguanli();Key=0;continue;  	//按键3 卡片管理
		 case 2:    gaimima();break;  //按键2 修改密码
	     default: break;
	    }
      }
     }
    else   //密码错误
    {
        Beep=0;//打开蜂鸣器
		delay(300);
		Beep=1;
		PutStr(1,2,"密码错误");
		PutStr(2,0,"  请重新操作！"); 
		PutStr(3,0,"  按任意键继续"); 
	    while(Keycan()==0);
    }
    Key=0;
  
    welcome();	//显示初始界面

    j=0;
}
     
void main()
{ 	
	uchar i=0,j=0,x=1;//check;
   	uchar Right_flag;
	Init_rc522();      //RC522初始化
	UART_Init();	   //串口初始化		  
	at24c02_Init();     //24C02初始化
	
	delay(10);

	Lcd_Init();	//LCD12864初始化				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	welcome();	//显示初始界面
	
	while(1)  
	{ 
		Key=Keycan();//按键检测								
		while(Key!=0X48)	     //等待按下确定键后跳出
		{	 
			shuaka();
			Key=Keycan();
			KeyDeal(Key);
			delay(30);
			if(Key==0x88)
			{
				Administrator();
			}		                              
			if(Key==0)	   //没有按键按下，K赋值为10,什么都不执行
				K=10;
			if((K>=0)&&(K<=9))	 //有数字键按下
			{
				mima[j]=K;		 //将按键值放入数组
				if(j<6)
				{	
					WriteCommand(0x99+j);		//指定第四行显示位置*
					WriteData(0x0f);	
				}
				++j;
				if(j==7)
				j=6;												  
			}  
			else if(K==11)		//按了删除键
			{
				if(j==0)
				{
					WriteCommand(0x99);		//指定第四行显示一个空格
					WriteData(0x20);
				}
				else
				{
					--j;
					WriteCommand(0x99+j);   //指定第四行显示一个空格，来删掉一个*
					WriteData(0x20);
				}
			}
		}
		 for(i=0;i<6;i++)
         {
            UserPassword[i]=mima[i]+0x30;
	
          } 
		if(j==6)			 //核对密码
		{
			Right_flag=PassWord_Chack();
		}
		if (Right_flag==1)   //密码正确
		{
			Right_flag=0;
			Lcd_ClearTXT();
			PutStr(1,2,"门已打开");
			Relay=0;
			delay(2500); 
			Relay=1;	
			delay(1000);
			
			welcome();	//显示初始界面

			j=0;
	     }
	    else				//密码错误，继续进入while循环检测、寻卡、等待确定键按下
	    {
	    Lcd_ClearTXT();
			PutStr(1,2,"密码错误");
			PutStr(2,0,"  请重新操作！"); 
			Beep=0;
			delay(300);
			Beep=1;
			delay(2000);
		    
			welcome();	//显示初始界面

			j=0;	
	    }
		//Key=0;			   //按键值清零				  
	}	
}