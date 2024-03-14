#ifndef _AT24C02_H_
#define _At24C02_H_

#define delay4us() _nop_();_nop_();_nop_();_nop_();

sbit SDA = P3^5;	//��������/��ַ
sbit SCL = P3^4;	//����ʱ��
sbit  WP = P3^3;	//д����  0���ɶ���д 1��ֻ��

extern void at24c02_Init(void);	//���� : ��ʼ��
extern unsigned char at24c02_Read(unsigned char address);//���� : ��24c02�ж���ֵ
extern void at24c02_Write(unsigned char address, unsigned char info);//���� : д������ address(��ַ��info��ֵ��

#endif












