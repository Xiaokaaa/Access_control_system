#ifndef _LCD12864_H_
#define _LCD12864_H_

sbit  CS = P2^5;		//RS  1：显示数据  0：显示指令
sbit SID = P2^6;	//RW  1：读   0：写
sbit SCK = P2^7;	//EN  使能信号

extern void WriteCommand( unsigned char Cbyte );//功能 ：写一个字节的指令
extern void WriteData( unsigned char Dbyte );//功能 ：写一个字节的数据
extern void Lcd_Init( void );//功能 ：lcd初始化
extern void Lcd_ClearTXT( void );//功能 ：清除文本
extern void PutStr(unsigned char row,unsigned char col,unsigned char *puts);//功能 ：显示字符串

extern void Interface(unsigned char num);  ////选择显示界面

#endif
