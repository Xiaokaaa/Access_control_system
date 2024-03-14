 #include "reg52.h"
 #include "intrins.h"
 #include "rc522.h"
 #include "string.h"

void delay_ns(unsigned int ns)	//ns����ʱ
{
	unsigned int i;
	for(i=0; i<ns; i++)
	{
		_nop_();
		_nop_();
	}
}

unsigned char SPIReadByte(void)		//���� ����SPI���� 
{
	unsigned char SPICount;		// ����ʱ��������ݵļ�����
	unsigned char SPIData;                  
	SPIData = 0;
	for (SPICount = 0; SPICount < 8; SPICount++)	// ׼����Ҫ��ȡ������ʱ��
	{
		SPIData <<= 1;     
		CLR_SPI_CK;		
		if(STU_SPI_MISO) SPIData |= 0x01;
		SET_SPI_CK;  
	} 
	return (SPIData);      // ��󷵻ض�ȡ����
}

void SPIWriteByte(unsigned char SPIData)		//���� ��дSPI���� 
{
	unsigned char SPICount; 		// ����ʱ��������ݵļ�����
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
//��    �ܣ���RC5222�Ĵ���
//����˵����Address:�Ĵ�����ַ
///////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char ucAddr;
    unsigned char ucResult = 0;
	CLR_SPI_CS;
    ucAddr = ((Address << 1) & 0x7E) | 0x80;
	SPIWriteByte(ucAddr);	// дSPI����
	ucResult = SPIReadByte();    // ��SPI����
	SET_SPI_CS;
	return ucResult;
}

////////////////////////////////////////////////////////////
//��    �ܣ�дRC522�Ĵ���
//����˵����Address:�Ĵ�����ַ
//          value:д���ֵ
////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char ucAddr;
	CLR_SPI_CS;
    ucAddr = ((Address << 1) & 0x7E);
	SPIWriteByte(ucAddr);	//ѡ���Ĵ���
	SPIWriteByte(value);	//д������
	SET_SPI_CS;
}   

////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg:�Ĵ�����ַ
//          mask:��λֵ
////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x00;
    tmp = ReadRawRC(reg);	//����Ҫ��������λ��������
    WriteRawRC(reg, tmp & ~mask);  // Ҫ��������λ����
} 


//////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg:�Ĵ�����ַ
//          mask:��λֵ
//////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x00;
    tmp = ReadRawRC(reg);	//����Ҫ��������λ��������
    WriteRawRC(reg,tmp | mask);  // д������λ����
}

///////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command:RC522������
//          pInData:ͨ��RC522���͵���Ƭ������
//          InLenByte:�������ݵ��ֽڳ���
//          pOutData:���յ��Ŀ�Ƭ��������
//          *pOutLenBit:�������ݵ�λ����
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
    
	i = 2000;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
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
//��    �ܣ�Ѱ��
//����˵��: req_code:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
	char status;  
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);		//��RC522�Ĵ���λ
	WriteRawRC(BitFramingReg,0x07); 	//дRC522�Ĵ���
	SetBitMask(TxControlReg,0x03);		//��RC522�Ĵ���λ
 
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
//��    �ܣ�����ײ
//����˵��: pSnr:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
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
    
    WriteRawRC(ModeReg,0x3D);	//��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);	//����522�����÷�Ƶ
    WriteRawRC(TPrescalerReg,0x3E);	//��ʱ����ǰֵ
	WriteRawRC(TxAutoReg,0x40);//�������������ã����Ʒ����ź�Ϊ100%ASK
   
    return MI_OK;
}

char M500PcdConfigISOType(unsigned char type)//����RC522�Ĺ�����ʽ 
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

void PcdAntennaOn(void)	//���� ����������  
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

void PcdAntennaOff(void)	//���� ���ر����� 
{
	ClearBitMask(TxControlReg, 0x03);
}

void Init_rc522(void)	//���� ��RC522��ʼ�� 
{
	PcdReset();
	PcdAntennaOff();  
	PcdAntennaOn();
	M500PcdConfigISOType( 'A' );
}
