#include <reg52.h>
#include <intrins.h>
#include "at24c02.h"
#include "lcd12864.h"
#include "rc522.h"

#define uchar unsigned char
#define uint unsigned int
 

#define K_confirm  0x48    //确认按键定义		S15 0100 1000
#define K_exit  0x18	//退出按键定义   	S13 0001 1000
#define K_delete 0x82	//删除按键定义 		S8  1000 0010

#define User 1
uchar Card_type[20];	   
sbit Relay = P3^6;//继电器控制
sbit Beep = P3^2;	//蜂鸣器

uchar  Member = 1,sec;     
        
uchar mima[6];
uchar mimag[6];
uchar PIN[6]={1,2,3,4,5,6};	//用于存储门禁密码
       
 
uchar  K;	//键值
uchar  Key;		
uchar  flag_id;//校验标志      

uchar  xdata UserPassword[7]={0};//密码

uchar  ID_ASC[8];
uchar  IDn[8];
uchar  numaddress[6]={0x38,0x40,0x48,0x50,0x58,0x60};
uchar  idnum;      

void delayms(unsigned int z) //延时函数
{
	unsigned int x,y;
	for(x=z; x>0; x--)
		for(y=110; y>0; y--);
}

/********************************************************************
* 功能 : 单一用户密码检测
* 输入 : User_Number
* 输出 : 密码正确与错误，错误返回0，正确返回1
********************************************************************/
uchar Chack(uchar User_Number)
{
	uchar flag=0, i, temp[6], Address;
	Address = User_Number*10;
	for(i=0; i<6; i++)
	{
		temp[i] = at24c02_Read(Address);
		delayms(10);
		Address++;
	}
	for(i=0; i<6; i++)
	{
		if(temp[i] == UserPassword[i])
			flag++;	
	}
	if(flag==6) flag=1;
	else flag=0;
	return flag;
}

/********************************************************************
* 功能 : 所有用户密码检测
* 输入 : 无
* 输出 : 密码正确与错误，错误返回0，正确返回1
********************************************************************/
uchar PassWord_Chack()
{
	uchar i=0;
	while(i<User)
	{
		if(Chack(i)==1)
		{
			Member=i+1;
			return 1;
		}
		i++;	
	}
	return 0;	
}
   
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
		{  
			rcode = 0xFE;         // 逐行扫描初值
			while((rcode&0x10) != 0)
			{
				P1 = rcode;         // 输出行扫描码
				if((P1&0xF0) != 0xF0) // 本行有键按下
				{
					ccode = (P1&0xF0)|0x0F;
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
        
void KeyDeal(unsigned char Key)//矩阵按键定义
{ 
	if(Key != 0)
	{   
		switch(Key)
		{
			case 0x11: K=1; break;	 
			case 0x21: K=2; break;  
			case 0x41: K=3; break;  
			case 0x81: 		break;       
			case 0x12: K=4; break;
			case 0x22: K=5; break;
			case 0x42: K=6; break;
			case 0x82: 		break;		
			case 0x14: K=7; break;
			case 0x24: K=8; break;
			case 0x44: K=9; break;
			case 0x84:  	break;		
			case 0x18: K=11;break;	//删除键		
			case 0x28: K=0; break;
			case 0x48: K=12;break;
			case 0x88: 		break;		
			default: break;
		}
	}
}   
        
void Change_Pin()	//改密码
{
	uchar i, j=0, Pin1[6], Pin2[6];
	uchar k, temp;
	Lcd_ClearTXT(); //液晶清除
	PutStr(1,1,"请输入新密码");
	for(i=0;i<6;i++) Pin1[i] = 0;									
	Key = Keycan(); //检测矩阵按键
	while(Key != K_confirm)
	{
		Key = Keycan();
		KeyDeal(Key);
		delay(30);				                              
		if(Key == 0) K = 10;  //没有按下按键，不进行任何操作
		if((K >= 0) && (K <= 9))	//修改密码0到9
		{
			Pin1[j] = K;
			if(j<6)
			{	
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);	//显示*号
			}
			++j;
			if(j==7) j=6;	//？？？？？？？？？？？？？									
		}  
		if(K == 34)		//按了删除键   34？？？？？？？？？？？？
		{
			if(j == 0)
			{
				WriteCommand(0x89);		//指定第三行显示位置
				WriteData(0x20);	//写空白
			}
			else
			{
				--j;
				WriteCommand(0x89+j);	    //指定第三行显示位置
				WriteData(0x20);
			}
		}
	}
	Lcd_ClearTXT();
	j = 0;
	PutStr(1,0,"请再次输入新密码");
	for(i=0; i<6; i++) Pin2[i] = 0;									
	Key = Keycan();
	while(Key != K_confirm)
	{
		Key = Keycan();
		KeyDeal(Key);
	    delay(30);				                              
	    if(Key == 0) K = 10;
	    if((K >= 0) && (K <= 9))	//修改密码0到9
	    {
			Pin2[j] = K;
			if(j<6)
			{		
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);	
			}
			++j;
			if(j==7) j=6;												   //@@#yinhuang
	    } 
		if(K == 34)
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
	if((Pin1[0]==Pin2[0])&&(Pin1[1]==Pin2[1])&&(Pin1[2]==Pin2[2])&&  \
	   (Pin1[3]==Pin2[3])&&(Pin1[4]==Pin2[4])&&(Pin1[5]==Pin2[5]))
	{
		for(i=0; i<6; i++)
			PIN[i] = Pin1[i];
		for(i=0; i<6; i++)           //密码限制在6位以内
		{
			UserPassword[i] = Pin1[i] + 0x30;                          	
		}
		temp = (Member-1)*10;	
		delayms(5);
		for(k=0; k<6; k++)
		{
			at24c02_Write(temp, UserPassword[k]);
			delayms(10);
			temp++;
		}  
		Interface(2);	//密码修改成功界面
		while(Keycan() == 0);
	}
	else
	{
		Interface(3);	//密码修改失败界面
		while(Keycan()==0);
	}
	Lcd_ClearTXT();
}

void LCD_Data(uchar LCDbyte,uchar i)  //卡号处理
{
	uchar bai,shi;	//十进制
	bai = LCDbyte/100;
	shi = LCDbyte%100/10;
	if(i == 0)
	{
		ID_ASC[0] = bai+0x30;		//ASCII码数字字符
		ID_ASC[1] = shi+0x30;
	}
	if(i == 1)
	{
		ID_ASC[2] = bai+0x30;
		ID_ASC[3] = shi+0x30;
	}
	if(i == 2)
	{
		ID_ASC[4] = bai+0x30;
		ID_ASC[5] = shi+0x30;
	}
	if(i == 3)
	{
		ID_ASC[6] = bai+0x30;
		ID_ASC[7] = shi+0x30;
	}

}	

uchar IC_Read( void )	//读卡
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
    for(i=0; i<4; i++) 	//卡ID号转化成ASCII
    {
		LCD_Data(Card_type[i],i);
    }
    return 1;	//读卡成功
}

void checkcard(void)	//校验卡
{
	uchar j;
	uchar IDAddress;
	IC_Read();
	for(idnum=0; idnum<6; idnum++)		//系统存卡容量为6
	{
		IDAddress = numaddress[idnum];
		for(j=0; j<8; j++)		//读取AT24C02存储的卡号
		{
			IDn[j] = at24c02_Read(IDAddress);
			delayms(10);
			IDAddress++;
		} 
		if( (ID_ASC[0]==IDn[0])&&(ID_ASC[1]==IDn[1])&&(ID_ASC[2]==IDn[2])&&(ID_ASC[3]==IDn[3])&&  \
		    (ID_ASC[4]==IDn[4])&&(ID_ASC[5]==IDn[5])&&(ID_ASC[6]==IDn[6])&&(ID_ASC[7]==IDn[7]) )
		break;
	}
	if(idnum<6) flag_id = 1;	//校验成功
	else flag_id = 0; //校验失败
}

void Credit_card(void)	//刷卡
{
	if(IC_Read() == 1)	//读卡成功
	{
		checkcard();    
		if(flag_id == 1)//校验成功    解锁失败处理
		{
			flag_id = 0;
			Lcd_ClearTXT();
			PutStr(1,2,"门已打开");	
			PutStr(2,0,"卡号：");
			WriteCommand(0x8B);
			WriteData(ID_ASC[0]);
			WriteData(ID_ASC[1]);
			WriteData(ID_ASC[2]);
			WriteData(ID_ASC[3]);
			WriteData(ID_ASC[4]);
			WriteData(ID_ASC[5]);
			WriteData(ID_ASC[6]);
			WriteData(ID_ASC[7]);
			Relay = 0;
			delay(2500); 
			Relay=1;
			delay(1000);
			Interface(1);
		}
		else					//解锁失败处理
		{
			Lcd_ClearTXT();
		 	PutStr(1,2,"卡未注册");	
			PutStr(2,0,"卡号：");
			WriteCommand(0x8B);
			WriteData(ID_ASC[0]); 
			WriteData(ID_ASC[1]);
			WriteData(ID_ASC[2]);
			WriteData(ID_ASC[3]);
			WriteData(ID_ASC[4]);
			WriteData(ID_ASC[5]);
			WriteData(ID_ASC[6]);
			WriteData(ID_ASC[7]);
			Beep = 0;
			delay(300);
			Beep = 1;
			delay(3000);
			Interface(1);
		}
	}
}

void Card_manage(void)	//卡片管理
{
	uchar numID[6]={0,1,2,3,4,5},i=0,k;
	uchar j,temp00;
	Lcd_ClearTXT();
	PutStr(0,2,"请刷卡");
	KeyDeal(Keycan());
	while( Key != K_exit )
	{
		WriteCommand(0x89);
		WriteData('I');
		WriteData('D');
		WriteData(0X30 + numID[i]);
		WriteData(':');
		
		IC_Read();
	    Key = Keycan();
	    KeyDeal(Key);
		temp00 = numaddress[i];
		for(k=0; k<8; k++)
		{
			IDn[k]=	at24c02_Read(temp00);
			delayms(10);
			temp00++;
		}
		temp00 = numaddress[i];
		if((IDn[0]==0x30)&&(IDn[1]==0x30)&&(IDn[2]==0x30)&&(IDn[3]==0x30)&& \
		   (IDn[4]==0x30)&&(IDn[5]==0x30)&&(IDn[6]==0x30)&&(IDn[7]==0x30))
		{
			if(IC_Read() == 1)
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
		if(Key == 0x82)		//注册卡
		{
			IC_Read();
			if(IC_Read() == 1)
			{
			 	checkcard();
				if(flag_id == 1)
				{	 
				 	flag_id = 0;
				 	Lcd_ClearTXT();
				 	PutStr(0,2,"卡已注册");
					PutStr(1,2,"请换卡片");
					PutStr(2,1,"按任意键继续");
					while(Keycan() == 0);
					Lcd_ClearTXT();
					PutStr(0,2,"请刷卡");
				}
				else
				{
				 	for(j=0; j<8; j++)
					{
						at24c02_Write(temp00,ID_ASC[j]);
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
		if(Key == 0x84)	//注销卡
		{
			if((IDn[0]==0x30)&&(IDn[1]==0x30)&&(IDn[2]==0x30)&&(IDn[3]==0x30)&& \
			   (IDn[4]==0x30)&&(IDn[5]==0x30)&&(IDn[6]==0x30)&&(IDn[7]==0x30))
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
					at24c02_Write(temp00,IDn[j]+0x30);
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
		if(Key == 0x81)	i++;//卡序号切换
		if(i == 6) i=0;
	}
}

void  Administrator(void)//管理员模式
{ 
	uchar i,j=0,x=1;
   	uchar Right_flag;
    Lcd_ClearTXT();
	PutStr(1,1,"请输入密码：");	
	for(i=0; i<6; i++) mima[i] = 0;	//密码初始化						
	Key = Keycan();
    while(Key != K_confirm)
	{
	    Key = Keycan();
	    KeyDeal(Key);
	    delay(30);				                              
	    if(Key == 0) K = 10;
	    if((K>=0) && (K<=9))
	    {
			mima[j] = K;
			if(j<6)
			{	
				WriteCommand(0x89+j);		//指定第三行显示位置
				WriteData(0x0f);
			}
			++j;
			if(j == 7) j=6;												  
	    }
		if(K == 11)		//按了删除键
		{
		    if(j == 0)
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
	for(i=0; i<6; i++)
	{
		UserPassword[i] = mima[i]+0x30;
	} 
	if(j == 6) {Right_flag = PassWord_Chack();}
	if(Right_flag == 1)
	{
		Right_flag=0;
		while(Key != K_exit)
		{	 
			Interface(4);	//菜单界面
			Key = Keycan();
			KeyDeal(Key);  
			switch(K)
			{
				case 1:	Lcd_ClearTXT();
						PutStr(1,2,"门已打开"); //按键1：打开门禁
						Relay = 0;
						delay(2500); 
						Relay = 1;	
						delay(1000);
						Key = 0; K = 0;
						Interface(4);
						break;  
				case 4:	Right_flag = 0;   		//按键4：退出系统
						K = 0; Key = K_exit;
						break;
				case 3:	Card_manage();  		//按键3：卡片管理
						Key = 0;
						break;
				case 2: Change_Pin();  			//按键2：修改密码
						break;
				default:break;
			}
		}
	}
    else   //密码错误
    {
		Beep=0;//打开蜂鸣器
		delay(300);
		Beep=1;
		PutStr(1,0,"  密码错误");
		PutStr(2,0,"  请重新操作！"); 
		PutStr(3,0,"  按任意键继续"); 
		while(Keycan()==0);
    }
    Key = 0;
    Interface(1);	//显示初始界面
    j=0;
}  
    
void UART_Init()	//功能 : 单片机串口初始化
{
	SCON = 0x50;               //串口方式1	//REN=1; 允许接收
	PCON = 0x00;                //SMOD=0
	TMOD = 0x21;               //定时器1定时方式2
	 TH1 = 0xFD;                //11.0592MHz  模块默认波特率为9600bps
	 TL1 = 0xFD;										 
	 TR1 = 1;                   //启动定时器 
	  EA = 1;       
}
    
void main()
{ 	
	uchar i=0, j=0, x=1;
   	uchar Right_flag;
	
	Init_rc522(); 
	UART_Init();	  
	at24c02_Init(); 
	delay(10);
	Lcd_Init();				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	Interface(1);	//显示初始界面
	
	while(1)  
	{ 
		Key = Keycan();								
		while(Key != K_confirm)	     //等待按下确定键后跳出
		{	 
			Credit_card();	//刷卡处理
			Key = Keycan();
			KeyDeal(Key);
			delay(30);
			if(Key == 0x88)	Administrator();	//按下菜单键
	                              
			if(Key == 0) K=10;   //没有按键按下，什么都不执行
				
			if((K>=0) && (K<=9))	 //有数字键按下
			{
				mima[j] = K;		 //输入密码
				if(j<6)
				{	
					WriteCommand(0x99+j);		//指定第四行显示位置*
					WriteData(0x0f);	
				}
				++j;
				if(j==7) j=6;												  
			}  
			else if(K == 11)		//按了删除键
			{
				if(j == 0)
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
		
		for(i=0; i<6; i++) UserPassword[i]=mima[i]+0x30;

		if(j == 6)	Right_flag = PassWord_Chack();

		if (Right_flag == 1)   //密码正确
		{
			Right_flag=0;
			Lcd_ClearTXT();
			PutStr(1,2,"门已打开");
			Relay=0;
			delay(2500); 
			Relay=1;	
			delay(1000);	
			Interface(1);	//显示初始界面
			j=0;
	    }
	    else
	    {
			Lcd_ClearTXT();
			PutStr(1,0,"    密码错误  ");
			PutStr(2,0,"  请重新操作！"); 
			Beep = 0;
			delay(300);
			Beep = 1;
			delay(2000);
			Interface(1);	//显示初始界面
			j=0;	
	    }			  
	}	
}