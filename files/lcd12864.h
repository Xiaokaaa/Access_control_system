#ifndef _LCD12864_H_
#define _LCD12864_H_

sbit  CS = P2^5;		//RS  1����ʾ����  0����ʾָ��
sbit SID = P2^6;	//RW  1����   0��д
sbit SCK = P2^7;	//EN  ʹ���ź�

extern void WriteCommand( unsigned char Cbyte );//���� ��дһ���ֽڵ�ָ��
extern void WriteData( unsigned char Dbyte );//���� ��дһ���ֽڵ�����
extern void Lcd_Init( void );//���� ��lcd��ʼ��
extern void Lcd_ClearTXT( void );//���� ������ı�
extern void PutStr(unsigned char row,unsigned char col,unsigned char *puts);//���� ����ʾ�ַ���

extern void Interface(unsigned char num);  ////ѡ����ʾ����

#endif
