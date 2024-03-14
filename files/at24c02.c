#include <reg52.h>
#include <intrins.h>
#include "main.h"
#include "at24c02.h"


void flash(void)   //功能：延时,时间为2个NOP，大概为2us
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

void at24c02_Init(void)   //功能 : 初始化
{	
	WP = 0;		//允许读写
	SCL = 1;
	flash();
	SDA = 1;
	flash();
}

void start(void)	//功能 : 启动I2C总线
{
	SCL = 1; 	//时钟高电平
	flash();
	SDA = 1;	//SDA 由高到低 为起始信号
	flash(); 
	SDA = 0; 
	flash(); 
	SCL = 0; 
	flash();
}

void stop() 	//功能 : 停止I2C总线
{
	SCL = 0;
	flash();
	SDA = 0; 	//SDA 由低到高 为停止信号
	flash();
	SCL = 1;
	flash();
	SDA = 1;
	flash();
}

void writex(uchar j)	//功能 : 字节写，值为j
{  
	uchar i,temp;
	temp = j;
	for(i=0; i<8; i++)
	{
		SCL = 0; 
		flash(); 
		SDA = (bit)(temp & 0x80); //最高位开始写
		flash();
		SCL = 1; 
		flash();
		temp = temp << 1; 
	}
	SCL = 0;
	flash(); 
}

uchar readx(void)	//功能 : 字节读，返回k
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

void ack(void)		//功能 : I2C总线时钟
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

uchar at24c02_Read(uchar address)	//功能：地址读
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

void at24c02_Write(uchar address, uchar info)	//功能：地址写
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
