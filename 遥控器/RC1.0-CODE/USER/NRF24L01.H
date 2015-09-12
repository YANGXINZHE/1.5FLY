#ifndef __NRF24L01_H
#define __NRF24L01_H

extern unsigned char idata TxBuf[24];
extern unsigned char idata RxBuf[24];

//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload
//***************************************NRF24L01寄存器指令*******************************************************
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define Nop             0xFF  	// 保留
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG2         0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置

#define MAX_TX  		0x10  //′?μ?×?′ó·￠?í′?êy?D??
#define TX_OK   		0x20  //TX·￠?ííê3é?D??
#define RX_OK   		0x40  //?óê?μ?êy?Y?D??

//******************************************************************************************

void NRF24L01_RX_Mode(u8 FQ);
void NRF24L01_TX_Mode(u8 FQ);
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 u8s);//D′êy?Y??
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 u8s);//?áêy?Y??		  
u8 NRF24L01_Read_Reg(u8 reg);			//?á??′??÷
u8 NRF24L01_Write_Reg(u8 reg, u8 value);//D′??′??÷
u8 NRF24L01_Check(void);//?ì2é24L01ê?·?′??ú
u8 NRF24L01_TxPacket(u8 *txbuf);//·￠?íò???°üμ?êy?Y
u8 NRF24L01_RxPacket(u8 *rxbuf);//?óê?ò???°üμ?êy?Y

extern const u8 ip[6];//·é?ú
extern const u8 ip0[6];//·é?ú3?0
extern const u8 ip1[6];//·é?ú3?1
extern const u8 ip2[6];//·é?ú3?2
extern const u8 ip3[6];//·é?ú3?3
extern const u8 ip4[6];//·é?ú3?4
extern const u8 ip5[6];//·é?ú3?5							   	   

extern u8 caip0[6];//?ì2é?óê?μ??·0
extern u8 caip1[6];//?ì2é?óê?μ??·1
extern u8 caip2[6];//?ì2é?óê?μ??·2
extern u8 caip3[6];//?ì2é?óê?μ??·3
extern u8 caip4[6];//?ì2é?óê?μ??·4
extern u8 caip5[6];//?ì2é?óê?μ??·5
extern u8 caout[6];//?ì2é·￠é?μ??·
extern u8 careg[2]; //?ì2é??′??÷×′ì?￡¨?òêy?μ￡?


#endif  