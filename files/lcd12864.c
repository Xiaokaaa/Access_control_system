#include <reg52.h>
#include <intrins.h>
#include "lcd12864.h"
	    
unsigned char code AC_TABLE[]=
{0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
 0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
 0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f};//32��������ʾ�����ַ

void SendByte(unsigned char Dbyte)	//���� ������һ���ֽ�
{
	unsigned char i;
	for(i=0; i<8; i++)
	{
		SCK = 0;
		Dbyte = Dbyte << 1;	
		SID = CY;	//CY����λ��־λ����SID
		SCK = 1;
		SCK = 0;
	}
}

unsigned char ReceiveByte(void)	 //���� ������һ���ֽ�
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

void CheckBusy( void )//���� �����æ״̬
{
	do SendByte(0xfc);     //11111,RW(1),RS(0),0
	while(0x80 & ReceiveByte());
}

void WriteCommand( unsigned char Cbyte )	//���� ��дһ���ֽڵ�ָ��
{
	CS = 1;
	CheckBusy();
	SendByte(0xf8);     //11111,RW(0),RS(0),0
	SendByte(0xf0 & Cbyte);	//����λ
	SendByte(0xf0 & Cbyte << 4);	//����λ
	CS = 0;
}

void WriteData( unsigned char Dbyte )	//���� ��дһ���ֽڵ�����
{
	CS = 1;
	CheckBusy();
	SendByte(0xfa);          //11111,RW(0),RS(1),0
	SendByte(0xf0 & Dbyte);
	SendByte(0xf0 & Dbyte << 4);
	CS = 0;
}

void Lcd_Init( void )	//���� ��lcd��ʼ��
{
     WriteCommand(0x30);	//����ָ�����
     WriteCommand(0x03);	//��ַ��λ��
     WriteCommand(0x0c);	//������ʾ
     WriteCommand(0x01);	//�����ʾ
     WriteCommand(0x06);	//������趨��
}

void Location_xy_12864(unsigned char x,unsigned char y)   //���� ���趨���
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

void Lcd_ClearTXT( void )	//���� ������ı�
{
	unsigned char i;
	WriteCommand(0x30);
	WriteCommand(0x80);
	for(i=0; i<64; i++) WriteData(0x20);	// 0x20�����հ׷�
	Location_xy_12864(0,0);	    
}

void PutStr(unsigned char row, unsigned char col, unsigned char *puts)	//���� ����ʾ�ַ���
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
void Interface(unsigned char num)  //ѡ����ʾ����
{
	switch(num)
	{
		case 1: Lcd_ClearTXT();					//��ӭ����
				PutStr(0,0,"    ��ӭʹ��    ");
				PutStr(1,0,"  �����Ž�ϵͳ  ");
				PutStr(2,0,"�����������ˢ��");
				break;
		case 2:	PutStr(0,0,"  �����޸ĳɹ�  ");//�����޸ĳɹ�����
				PutStr(3,0,"  �����������  ");
				break;
		case 3:	PutStr(0,0,"  �����޸�ʧ��  ");//�����޸�ʧ�ܽ���
				PutStr(1,0,"������������벻"); 
				PutStr(2,0,"һ�£������²���");       
				PutStr(3,0,"  �����������  "); 	
				break;
		case 4:	PutStr(0,0,"����1 : ���Ž�");//�˵�����
				PutStr(1,0,"����2 : �޸�����");
				PutStr(2,0,"����3 : ��Ƭ����");
				PutStr(3,0,"����4 : �˳�ϵͳ");
				break;
		case 5:	
				break;
		default:break;
	}
}  

