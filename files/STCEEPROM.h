#ifndef __STCEEPROM_H__
#define __STCEEPROM_H__

#include<reg52.h>
#include<intrins.h>
/*����������STC��Ƭ���ļĴ���*/
/*���������STC89C51RC��PDF�ĵ�35ҳ��ʼ*/
sfr ISP_DATA  = 0xE2;
sfr ISP_ADDRH = 0xE3;
sfr ISP_ADDRL = 0xE4;
sfr ISP_CMD   = 0xE5;
sfr ISP_TRIG  = 0xE6;
sfr ISP_CONTR = 0xE7;

void ISP_ERASE(Address);
unsigned int ISP_READ(Address);
void ISP_PROGRAM(Address, Data_temp);

#endif
