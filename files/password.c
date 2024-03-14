#include "at24c02.h"

////////////////////////////////////////////////
//功能 : 单一用户密码检测
//输入 : User_Number
//输出 : 密码正确与错误，错误返回0，正确返回1
////////////////////////////////////////////////
uchar Chack(uchar User_Number)
{
	uchar flag=0, i, temp[6], Address;
	Address = User_Number*10;
	for(i=0; i<6; i++)
	{
		temp[i] = at24c02_Read(Address);
		delayms(10);
		Address++;
	}
	for(i=0; i<6; i++)
	{
		if(temp[i] == UserPassword[i]) flag++;	
	}
	if(flag == 6) flag = 1;
	else flag = 0;
	return flag;
}
///////////////////////////////////////
//功能 : 所有用户密码检测
//输入 : 无
//输出 : 密码错误返回0，正确返回1
///////////////////////////////////////
uchar PassWord_Chack()
{
	uchar i = 0;
	while(i < User)
	{
		if(Chack(i) == 1)
		{
			Member = i+1;
			return 1;
		}
		i++;	
	}
	return 0;	
}
