 #include "reg52.h"
 #include "intrins.h"
 #include "rc522.h"
 #include "string.h"

void delay_ns(unsigned int ns)	//ns级延时
{
	unsigned int i;
	for(i=0; i<ns; i++)
	{
		_nop_();
		_nop_();
	}
}

unsigned char SPIReadByte(void)		//功能 ：读SPI数据 
{
	unsigned char SPICount;		// 用于时钟输出数据的计数器
	unsigned char SPIData;                  
	SPIData = 0;
	for (SPICount = 0; SPICount < 8; SPICount++)	// 准备在要读取的数据时钟
	{
		SPIData <<= 1;     
		CLR_SPI_CK;		
		if(STU_SPI_MISO) SPIData |= 0x01;
		SET_SPI_CK;  
	} 
	return (SPIData);      // 最后返回读取数据
}

void SPIWriteByte(unsigned char SPIData)		//功能 ：写SPI数据 
{
	unsigned char SPICount; 		// 用于时钟输出数据的计数器
	for (SPICount = 0; SPICount < 8; SPICount++)
	{
		if (SPIData & 0x80) SET_SPI_MOSI;
		else CLR_SPI_MOSI;
		delay_ns(1);
		CLR_SPI_CK;delay_ns(1);
		SET_SPI_CK;delay_ns(1);
		SPIData <<= 1;
	}         	
}


///////////////////////////////////////////////
//功    能：读RC5222寄存器
//参数说明：Address:寄存器地址
///////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char ucAddr;
    unsigned char ucResult = 0;
	CLR_SPI_CS;
    ucAddr = ((Address << 1) & 0x7E) | 0x80;
	SPIWriteByte(ucAddr);	// 写SPI数据
	ucResult = SPIReadByte();    // 读SPI数据
	SET_SPI_CS;
	return ucResult;
}

////////////////////////////////////////////////////////////
//功    能：写RC522寄存器
//参数说明：Address:寄存器地址
//          value:写入的值
////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char ucAddr;
	CLR_SPI_CS;
    ucAddr = ((Address << 1) & 0x7E);
	SPIWriteByte(ucAddr);	//选定寄存器
	SPIWriteByte(value);	//写入数据
	SET_SPI_CS;
}   

////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg:寄存器地址
//          mask:清位值
////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x00;
    tmp = ReadRawRC(reg);	//读出要清除的清除位掩码数据
    WriteRawRC(reg, tmp & ~mask);  // 要清除的清除位掩码
} 


//////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg:寄存器地址
//          mask:置位值
//////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x00;
    tmp = ReadRawRC(reg);	//读出要清除的清除位掩码数据
    WriteRawRC(reg,tmp | mask);  // 写入的清除位掩码
}

///////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command:RC522命令字
//          pInData:通过RC522发送到卡片的数据
//          InLenByte:发送数据的字节长度
//          pOutData:接收到的卡片返回数据
//          *pOutLenBit:返回数据的位长度
///////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
        case PCD_AUTHENT: 	irqEn   = 0x12;
							waitFor = 0x10;
							break;
		case PCD_TRANSCEIVE:irqEn   = 0x77;
							waitFor = 0x30;
							break;
		default: break;
    }
	
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++) 
		WriteRawRC(FIFODataReg, pInData[i]);    
    WriteRawRC(CommandReg, Command);
   
    if (Command == PCD_TRANSCEIVE)   
		SetBitMask(BitFramingReg,0x80);  
    
	i = 2000;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do 
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg, 0x80);

    if (i!=0)
    {    
        if(!(ReadRawRC(ErrorReg) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   
					*pOutLenBit = (n-1)*8 + lastBits;   
				}
                else
                {   
					*pOutLenBit = n*8;   
				}
                if (n == 0)
                {   
					n = 1;    
				}
                if (n > MAXRLEN)
                {   
					n = MAXRLEN;   
				}
                for (i=0; i<n; i++)
                {   
					pOutData[i] = ReadRawRC(FIFODataReg);    
				}
            }
        }
        else
        {   
			status = MI_ERR;   
		}
        
    }
    SetBitMask(ControlReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}
                      
////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
	char status;  
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);		//清RC522寄存器位
	WriteRawRC(BitFramingReg,0x07); 	//写RC522寄存器
	SetBitMask(TxControlReg,0x03);		//置RC522寄存器位
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   
		status = MI_ERR;   
	}
   
	return status;
}

////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr:卡片序列号，4字节
//返    回: 成功返回MI_OK
////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);

    if (status == MI_OK)
	{
		for (i=0; i<4; i++)
		{   
			*(pSnr+i)  = ucComMF522Buf[i];
			snr_check ^= ucComMF522Buf[i];
		}
		if (snr_check != ucComMF522Buf[i])
   		{   
			status = MI_ERR;    
		}
    }
    SetBitMask(CollReg,0x80);
    return status;
}

///////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////
char PcdReset(void)
{
	SET_RC522RST;
    delay_ns(10);
	CLR_RC522RST;
    delay_ns(10);
	SET_RC522RST;
    delay_ns(10);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_ns(10);
    
    WriteRawRC(ModeReg,0x3D);	//和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);	//开启522，设置分频
    WriteRawRC(TPrescalerReg,0x3E);	//定时器当前值
	WriteRawRC(TxAutoReg,0x40);//（控制天线设置）调制发送信号为100%ASK
   
    return MI_OK;
}

char M500PcdConfigISOType(unsigned char type)//设置RC522的工作方式 
{
   if (type == 'A')      //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);
       WriteRawRC(RxSelReg,0x86);
       WriteRawRC(RFCfgReg,0x7F);
   	   WriteRawRC(TReloadRegL,30);
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   delay_ns(1000);
       PcdAntennaOn();
   }
   else return -1;
   
   return MI_OK;
}

void PcdAntennaOn(void)	//功能 ：开启天线  
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

void PcdAntennaOff(void)	//功能 ：关闭天线 
{
	ClearBitMask(TxControlReg, 0x03);
}

void Init_rc522(void)	//功能 ：RC522初始化 
{
	PcdReset();
	PcdAntennaOff();  
	PcdAntennaOn();
	M500PcdConfigISOType( 'A' );
}
