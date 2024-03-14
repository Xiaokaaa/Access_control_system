#include <reg52.h>
#include <intrins.h>
#include "main.h"
#include "at24c02.h"


void flash(void)   //���ܣ���ʱ,ʱ��Ϊ2��NOP�����Ϊ2us
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

void at24c02_Init(void)   //���� : ��ʼ��
{	
	WP = 0;		//�����д
	SCL = 1;
	flash();
	SDA = 1;
	flash();
}

void start(void)	//���� : ����I2C����
{
	SCL = 1; 	//ʱ�Ӹߵ�ƽ
	flash();
	SDA = 1;	//SDA �ɸߵ��� Ϊ��ʼ�ź�
	flash(); 
	SDA = 0; 
	flash(); 
	SCL = 0; 
	flash();
}

void stop() 	//���� : ֹͣI2C����
{
	SCL = 0;
	flash();
	SDA = 0; 	//SDA �ɵ͵��� Ϊֹͣ�ź�
	flash();
	SCL = 1;
	flash();
	SDA = 1;
	flash();
}

void writex(uchar j)	//���� : �ֽ�д��ֵΪj
{  
	uchar i,temp;
	temp = j;
	for(i=0; i<8; i++)
	{
		SCL = 0; 
		flash(); 
		SDA = (bit)(temp & 0x80); //���λ��ʼд
		flash();
		SCL = 1; 
		flash();
		temp = temp << 1; 
	}
	SCL = 0;
	flash(); 
}

uchar readx(void)	//���� : �ֽڶ�������k
{
	uchar i, j, k = 0;
	for(i=0; i<8; i++)
	{
		SCL = 0;
		flash();		
		if(SDA == 1) j = 1;
		else j = 0;
		k = (k << 1) | j; 
		SCL = 1;
		flash();
	} 
	return(k);
}

void ack(void)		//���� : I2C����ʱ��
{
	uchar i = 0;
	SCL = 1;
	flash();
	while((SDA == 1) && (i < 255)) 
	{
		i++;
	}
	SCL = 0;
	flash();
}

uchar at24c02_Read(uchar address)	//���ܣ���ַ��
{
	uchar i;
	start();
	writex(0xa0);
	ack();
	writex(address);
	ack();
	start();
	writex(0xa1);
	ack();
	i = readx();
	stop();
	return(i);
}

void at24c02_Write(uchar address, uchar info)	//���ܣ���ַд
{
	start();
	writex(0xa0);
	ack();
	writex(address);
	ack();
	writex(info);
	ack();
	stop();
}
