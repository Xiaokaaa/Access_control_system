#include <reg52.h>
#include <intrins.h>
#include "lcd12864.h"
	    
unsigned char code AC_TABLE[]=
{0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
 0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
 0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f};//32个汉字显示坐标地址

void SendByte(unsigned char Dbyte)	//功能 ：发送一个字节
{
	unsigned char i;
	for(i=0; i<8; i++)
	{
		SCK = 0;
		Dbyte = Dbyte << 1;	
		SID = CY;	//CY：进位标志位赋给SID
		SCK = 1;
		SCK = 0;
	}
}

unsigned char ReceiveByte(void)	 //功能 ：接收一个字节
{
	unsigned char i,temp1,temp2;
	temp1 = temp2 = 0;
	for(i=0; i<8; i++)
	{
		temp1 = temp1 << 1;
		SCK = 0;
		SCK = 1;                
		SCK = 0;
		if(SID) temp1++;
	}
	for(i=0; i<8; i++)
	{
		temp2 = temp2 << 1;
		SCK = 0;
		SCK = 1;
		SCK = 0;
		if(SID) temp2++;
	}
	return ((0xf0 & temp1) + (0x0f & temp2));
}

void CheckBusy( void )//功能 ：检查忙状态
{
	do SendByte(0xfc);     //11111,RW(1),RS(0),0
	while(0x80 & ReceiveByte());
}

void WriteCommand( unsigned char Cbyte )	//功能 ：写一个字节的指令
{
	CS = 1;
	CheckBusy();
	SendByte(0xf8);     //11111,RW(0),RS(0),0
	SendByte(0xf0 & Cbyte);	//高四位
	SendByte(0xf0 & Cbyte << 4);	//低四位
	CS = 0;
}

void WriteData( unsigned char Dbyte )	//功能 ：写一个字节的数据
{
	CS = 1;
	CheckBusy();
	SendByte(0xfa);          //11111,RW(0),RS(1),0
	SendByte(0xf0 & Dbyte);
	SendByte(0xf0 & Dbyte << 4);
	CS = 0;
}

void Lcd_Init( void )	//功能 ：lcd初始化
{
     WriteCommand(0x30);	//基本指令操作
     WriteCommand(0x03);	//地址归位？
     WriteCommand(0x0c);	//整体显示
     WriteCommand(0x01);	//清除显示
     WriteCommand(0x06);	//进入点设定？
}

void Location_xy_12864(unsigned char x,unsigned char y)   //功能 ：设定光标
{
	switch(x)
	{
		case 0: x=0x80; break;
		case 1: x=0x90; break;
		case 2: x=0x88; break;
		case 3: x=0x98; break;
		default: x=0x80;
	}
	y = y & 0x07;
	WriteCommand(0x30);
	WriteCommand(y + x);
	WriteCommand(y + x);

}

void Lcd_ClearTXT( void )	//功能 ：清除文本
{
	unsigned char i;
	WriteCommand(0x30);
	WriteCommand(0x80);
	for(i=0; i<64; i++) WriteData(0x20);	// 0x20：半宽空白符
	Location_xy_12864(0,0);	    
}

void PutStr(unsigned char row, unsigned char col, unsigned char *puts)	//功能 ：显示字符串
{    
	WriteCommand(0x30);
	WriteCommand(AC_TABLE[8*row + col]);
	while(*puts != '\0')
	{
		if(col == 8)
		{
			col = 0;
			row++;
		}
		if(row == 4) row=0;
		WriteCommand(AC_TABLE[8*row + col]);
		WriteData(*puts);
		puts++;
		if(*puts != '\0')
		{
			WriteData(*puts);
			puts++;
			col++;
		}  
	}
}
void Interface(unsigned char num)  //选择显示界面
{
	switch(num)
	{
		case 1: Lcd_ClearTXT();					//欢迎界面
				PutStr(0,0,"    欢迎使用    ");
				PutStr(1,0,"  智能门禁系统  ");
				PutStr(2,0,"请输入密码或刷卡");
				break;
		case 2:	PutStr(0,0,"  密码修改成功  ");//密码修改成功界面
				PutStr(3,0,"  按任意键继续  ");
				break;
		case 3:	PutStr(0,0,"  密码修改失败  ");//密码修改失败界面
				PutStr(1,0,"两次输入的密码不"); 
				PutStr(2,0,"一致，请重新操作");       
				PutStr(3,0,"  按任意键继续  "); 	
				break;
		case 4:	PutStr(0,0,"按键1 : 打开门禁");//菜单界面
				PutStr(1,0,"按键2 : 修改密码");
				PutStr(2,0,"按键3 : 卡片管理");
				PutStr(3,0,"按键4 : 退出系统");
				break;
		case 5:	
				break;
		default:break;
	}
}  

