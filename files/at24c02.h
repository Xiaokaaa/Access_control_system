#ifndef _AT24C02_H_
#define _At24C02_H_

#define delay4us() _nop_();_nop_();_nop_();_nop_();

sbit SDA = P3^5;	//串行数据/地址
sbit SCL = P3^4;	//串行时钟
sbit  WP = P3^3;	//写保护  0：可读可写 1：只读

extern void at24c02_Init(void);	//功能 : 初始化
extern unsigned char at24c02_Read(unsigned char address);//功能 : 从24c02中读出值
extern void at24c02_Write(unsigned char address, unsigned char info);//功能 : 写入数据 address(地址）info（值）

#endif












