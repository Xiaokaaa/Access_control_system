#include "STCEEPROM.h"

#define ISP_BYTE_READ    1		//ISP�ֽڶ�
#define ISP_BYTE_PROGRAM 2		//ISPֱ�ӱ��
#define ISP_SECTOR_ERASE 3		//ISP��������

/********************************************************************
* ���� : EEPROM����������
* ���� : Address�����������ĵ�ַ)
* ��� : ��
***********************************************************************/
void ISP_ERASE(Address)
{
	ISP_ADDRL = Address;
	ISP_ADDRH = Address >> 8;
	ISP_CONTR = 0x81;				
	ISP_CMD = ISP_SECTOR_ERASE;	
	ISP_TRIG = 0x46;	//PDF�ֲ���˵���� ISP_TRIG ��д�� 0x46, Ȼ��д�� 0xb9
	ISP_TRIG = 0xb9;
	_nop_();
	_nop_();	
}

/********************************************************************
* ���� : ISP_READ()
* ���� : ��EEPROM��Ӧ�ĵ�ַ�ж���ֵ
* ���� : Address(��Ҫ��ȡEEPROM�ĵ�ַ��
* ��� : Read_Data����EEPROM������ֵ��
*********************************************************************/
unsigned int ISP_READ(Address)
{
	unsigned char Read_Data;
	ISP_ADDRL = Address;
	ISP_ADDRH = Address >> 8;
	EA = 0;	
	ISP_CONTR = 0x81;
	ISP_CMD = ISP_BYTE_READ;
	ISP_TRIG = 0x46;
	ISP_TRIG = 0xb9;
	_nop_();
	_nop_();
	Read_Data = ISP_DATA;
	return Read_Data;	
}

/********************************************************************
* ���� : ISP_PROGRAM()
* ���� : ʵ��EEPROM���ֽڱ��
* ���� : AddressΪ��̵ĵ�ַ��Data_tempΪ��Ҫ��̵�ֵ
* ��� : ��
***********************************************************************/
void ISP_PROGRAM(Address, Data_temp)
{
	ISP_DATA = Data_temp;
	ISP_ADDRL = Address;
	ISP_ADDRH = Address >> 8;
	ISP_CONTR = 0x81;
	ISP_CMD = ISP_BYTE_PROGRAM;
	ISP_TRIG = 0x46;
	ISP_TRIG = 0xb9;
	_nop_();
	_nop_(); 	
}