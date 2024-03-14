#ifndef	_RC522_H_
#define _RC522_H_

///////////////////////////////////
//MFRC522命令字
///////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令的执行
#define PCD_AUTHENT           0x0E               //执行读卡器的Mifare标准认证
#define PCD_RECEIVE           0x08               //激活接收器电路
#define PCD_TRANSMIT          0x04               //发送FIFO缓冲区的命令
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位MFRC522
#define PCD_CALCCRC           0x03               //激活CRC协处理器或执行自测试

///////////////////////////////////
//Mifare_One卡片命令字
///////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO  64字节
#define MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE0：命令和状态
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE1：命令     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE2：CFC    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE3:测试      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		  0x3F

//////////////////////////////////////
//和MF522通讯时返回的错误代码
//////////////////////////////////////
#define 	MI_OK                   0
#define 	MI_NOTAGERR           (-1)
#define 	MI_ERR                (-2)



sbit  spi_cs = P2^0;	//数据接口
sbit  spi_ck = P2^1;	//时钟接口
sbit  spi_mosi = P2^2;	//主出从入
sbit  spi_miso = P2^3;	//主入从出
sbit  spi_rst = P2^4;	//复位

#define SET_SPI_CS  spi_cs = 1
#define CLR_SPI_CS  spi_cs = 0

#define SET_SPI_CK  spi_ck = 1
#define CLR_SPI_CK  spi_ck = 0

#define SET_SPI_MOSI  spi_mosi = 1
#define CLR_SPI_MOSI  spi_mosi = 0

#define STU_SPI_MISO  spi_miso


#define SET_RC522RST  spi_rst = 1
#define CLR_RC522RST  spi_rst = 0


extern char PcdReset(void);//功能：复位RC522
extern char PcdRequest(unsigned char req_code,unsigned char *pTagType);//功能：寻卡
extern void PcdAntennaOn(void);	//功能 ：开启天线 
extern void PcdAntennaOff(void);//功能 ：关闭天线 
extern char M500PcdConfigISOType(unsigned char type);//功能 ：设置RC522的工作方式 
extern char PcdAnticoll(unsigned char *pSnr);//功能 ：防冲撞
extern char PcdSelect(unsigned char *pSnr);//功能 ：选定卡片
extern char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);//功能：验证卡片密码
extern char PcdWrite(unsigned char addr,unsigned char *pData);//功能 ：写数据到M1卡一块
extern char PcdRead(unsigned char addr,unsigned char *pData);//功能 ：读取M1卡一块数据
extern char PcdHalt(void);//功能 ：命令卡片进入休眠状态
extern void Init_rc522(void);//功能 ：RC522初始化

#endif
