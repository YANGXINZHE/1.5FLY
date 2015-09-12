#include "sysconfig.h"
#include <intrins.h>
#include <NRF24L01.H>

unsigned char idata TxBuf[24]={0};
unsigned char idata RxBuf[24]={0};

// 无线地址设置
//地址一共40个add bit是add0~add39
//在401地址先写(add0,add1,add2......add39)

//2401和51822地址关系
// 2401[add0 :add7] =PREFIX0[bit7:bit0](add7,add6,add5,add4,add3,add2,add1,add0)
// 2401[add8 :add15]=BASE0[bit31:bit24](add15,add14,add13,add12,add11,add10,add9,add8)
// 2401[add16:add23]=BASE0[bit23:bit16](add23,add22,add21,add20,add19,add18,add17,add16)
// 2401[add24:add31]=BASE0[bit15:bit8] (add31,add30,add29,add28,add27,add26,add25,add24)
// 2401[add32:add39]=BASE0[bit7 :bit0] (add39,add38,add37,add36,add35,add34,add33,add32)

////通道3到0的低字节
//	NRF_RADIO->PREFIX0 =(0x23<<24) //通道3的低字节地址
//	                   |(0xC3<<16) //通道2的低字节地址
//	                   |(0x43<< 8) //通道1的低字节地址
//	                   |(0xE7<< 0);//通道0的低字节地址 对应2401地址的[add7:add0]
//// í¨μà7 μ? 4 μ?μí×??úμ??·
//	NRF_RADIO->PREFIX1 =(0x13<<24) //通道7的低字节地址
//	                   |(0xE3<<16) //通道6的低字节地址
//	                   |(0x63<< 8) //通道5的低字节地址
//	                   |(0xA3<< 0);//通道4的低字节地址
// 
//  NRF_RADIO->BASE0   = (0xE7<<24)  //通道0的高字节对应2401的[add15:add8]
//                      |(0xE7<<16)  //对应2401的[add23:add16]
//	                    |(0xE7<<8)   //对应2401的[add31:add24]
//	                    |(0xE7<<0);  //对应2401的[add39:add32]

//  NRF_RADIO->BASE1   = (0x43<<24)  //通道1-7的高字节
//                      |(0x43<<16)
//	                    |(0x43<<8)
//	                    |(0x43<<0);
//以上结论得出2401的地址是每8个bit首尾对调，以下是官方用的地址
const u8  ip[6]={0xE7,0xE7,0xE7,0xE7,0xE7,0x00};//·￠é?μ??·  ′ó[bit0~7] [bit8~15] [bit16~23] [bit24~31] [bit32~39]
const u8 ip0[6]={0xE7,0xE7,0xE7,0xE7,0xE7,0x00};//?óê?μ??·0 oí51822í¨???òμ÷á?(bit0?èD′)

const u8 ip1[6]={0xC2,0xC2,0xC2,0xC2,0xC2,0x00};//?óê?μ??·1 bit39~bit8?a"0x43"(bit0?èD′)

const u8 ip2[6]={0xC3,0x00};//?óê?μ??·2 bit39~8oíí¨μà112ó?,??D′bit0~bit7
const u8 ip3[6]={0xC4,0x00};//?óê?μ??·3	í¨μà2345???üéè??char
const u8 ip4[6]={0xC5,0x00};//?óê?μ??·4	D′μ?ê±oò?3Dòê?bit0~bit39
const u8 ip5[6]={0xC6,0x00};//?óê?μ??·5		??óDbit0~7óDó??￡?óê?μ??·6￡o0xC7 ?óê?μ??·7￡o0xC8

u8 caout[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é·￠é?μ??·
u8 caip0[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é?óê?μ??·0
u8 caip1[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é?óê?μ??·1
u8 caip2[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é?óê?μ??·2
u8 caip3[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é?óê?μ??·3
u8 caip4[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é?óê?μ??·4
u8 caip5[6]={0xff,0xff,0xff,0xff,0xff,0x00};//ó?óú?ì2é?óê?μ??·5
u8 careg[2]={0xff,0x00};//ó?óú2é?′??′??÷μ??μ
//const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //·￠?íμ??·
//const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //·￠?íμ??·
	
/****************************************************************************************************
/*函数：uint SPI_RW(uint uchar)
/*功能：NRF24L01的SPI写时序
/****************************************************************************************************/
u16 SPI_RW(u16 uchar)
{
	u16 bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
   	{
		MOSI = (uchar & 0x80);         // output 'uchar', MSB to MOSI
		uchar = (uchar << 1);           // shift next bit into MSB..
		SCK = 1;                      // Set SCK high..
		delayus(12);
		uchar |= MISO;       		  // capture current MISO bit
		delayus(12);
		SCK = 0;            		  // ..then set SCK low again
   	}
    return(uchar);           		  // return read uchar
}
/****************************************************************************************************
/*函数：uchar SPI_Read(uchar reg)
/*功能：NRF24L01的SPI时序
/****************************************************************************************************/
u8 SPI_Read(u8 reg)
{
	u8 reg_val;
	
	CSN = 0;                // CSN low, initialize SPI communication...
	delayus(12);
	SPI_RW(reg);            // Select register to read from..
	delayus(12);
	reg_val = SPI_RW(0);    // ..then read registervalue
	delayus(12);
	CSN = 1;                // CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
}

/****************************************************************************************************/
/*功能：NRF24L01写寄存器函数
/****************************************************************************************************/
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u16 status;
	
	CSN = 0;                   // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	CSN = 1;                   // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}
/****************************************************************************************************/
/*功能：NRF24L01读寄存器函数
/****************************************************************************************************/
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 status;
	
	CSN = 0;                   // CSN low, init SPI transaction
	SPI_RW(reg);      // select register
	status =SPI_RW(0xff);             // ..and write value to it..
	CSN = 1;                   // CSN high again	
	return(status);            // return nRF24L01 status uchar
}
/****************************************************************************************************/
/*函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
/****************************************************************************************************/
//u16 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u16 status,uchar_ctr;
	
	CSN = 0;                    		// Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status uchar
	
	for(uchar_ctr=0;uchar_ctr<len;uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0xff);    // 
	
	CSN = 1;                           
	
	return(status);                    // return nRF24L01 status uchar
}
/*********************************************************************************************************
/*函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数
/*********************************************************************************************************/
//u16 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u16 status,uchar_ctr;
	
	CSN = 0;            //SPI使能       
	status = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<len; uchar_ctr++) //
		SPI_RW(*pBuf++);
	CSN = 1;           //关闭SPI
	return(status);    // 
}

/***********************************************************************************************************
/*函数：void nRF24L01_TxPacket(unsigned char * tx_buf)
/*功能：发送 tx_buf中数据
/**********************************************************************************************************/
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	//SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//spi?ù?è?a10.5Mhz￡¨24L01μ?×?′óSPIê±?ó?a10Mhz￡?   
	CE=0;
  	NRF24L01_Write_Buf(0xA0,txbuf,32);//D′êy?Yμ?TX BUF  32??×??ú
 	CE=1;//???ˉ·￠?í	   
	while(IRQ!=0);//μè′y·￠?ííê3é
	sta=NRF24L01_Read_Reg(0x07);  //?áè?×′ì???′??÷μ??μ	   
	NRF24L01_Write_Reg(0x20+0x07,sta); //??3yTX_DS?òMAX_RT?D??±ê??
	
	if(sta&0x10)//′?μ?×?′ó??·￠′?êy
	{
		NRF24L01_Write_Reg(0xE1,0xff);//??3yTX FIFO??′??÷ 
		return MAX_TX; 
	}
	if(sta&0x20)//·￠?ííê3é
	{
	   // LCD_ShowString(50,40,"OK!");
		//LED0=!LED0;
		return TX_OK;
	}
	return 0xff;//?????-òò·￠?íê§°ü

}
/******************************************************************************************************/
/*函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/*功能：数据读取后放如rx_buf接收缓冲区中
/******************************************************************************************************/
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	sta=NRF24L01_Read_Reg(0x07);  //读取状态寄存器的值   	 
	NRF24L01_Write_Reg(0x20+0x07,sta); //清除TX_DS或MAX_RT中断标志
	careg[0]=sta;
	if(sta&0x40)//接收到数据 wei 6
	{	
		NRF24L01_Read_Buf(0x61,rxbuf,32);//读取数据
		NRF24L01_Write_Reg(0xe2,0xff);//清除RX FIFO寄存器 
		return 1; //接收成功
	}	   
	return 0;//无数据
}
/****************************************************************************************************/
/*函数：void SetRX_Mode(void)
/*功能：数据接收配置 
/****************************************************************************************************/
void NRF24L01_RX_Mode(u8 FQ)
{
	CE=0;
    NRF24L01_Write_Reg(0x20+0x00,0x0f);//配置接收的基本参数，PWR_UP,EN_CRC,16BIT_CRC,
    NRF24L01_Write_Reg(0x20+0x01,0x00);//应答设置 0X3F代表54321 应答
//	NRF24L01_Read_Buf(0x01,careg,1);   //??oˉêy?ì2é?T????′??÷0x01μ??úèY
    NRF24L01_Write_Reg(0x20+0x02,0x3f);//接收通道使能 0X3F代表54321 接收
  	NRF24L01_Write_Reg(0x20+0x05,FQ); //RF频率设置
  	NRF24L01_Write_Reg(0x20+0x06,0x0f);	//设置TX参数 0DB 2MPBS 低噪声增益开
//	NRF24L01_Write_Reg(0x20+0x06,0xa7);

	NRF24L01_Write_Buf(0X20+0X0a,(u8*)ip0,5);//写RX0接收地址
//	NRF24L01_Read_Buf(0x0a,caip0,5);	 //2éμ??·è·?¨ò?D′è?
	NRF24L01_Write_Buf(0X20+0X0b,(u8*)ip1,5);//写RX1接收地址
//	NRF24L01_Read_Buf(0x0b,caip1,5);
	NRF24L01_Write_Buf(0X20+0X0c,(u8*)ip2,1); //写RX2接收地址
//	NRF24L01_Read_Buf(0x0c,caip2,5);
	NRF24L01_Write_Buf(0X20+0X0d,(u8*)ip3,1);////写RX3接收地址 
//	NRF24L01_Read_Buf(0x0d,caip3,5);
	NRF24L01_Write_Buf(0X20+0X0e,(u8*)ip4,1);//写RX4接收地址
//	NRF24L01_Read_Buf(0x0e,caip4,5);
	NRF24L01_Write_Buf(0X20+0X0f,(u8*)ip5,1);//写RX4接收地址
//	NRF24L01_Read_Buf(0x0f,caip5,5);
	NRF24L01_Write_Buf(0X20+0X10,(u8*)ip1,5);//D′TXμ?μ??·
//	NRF24L01_Read_Buf(0x10,caout,5);

	NRF24L01_Write_Reg(0x20+0x11,32);//0通道有效数据宽度
	NRF24L01_Write_Reg(0x20+0x12,32);//0通道有效数据宽度 
	NRF24L01_Write_Reg(0x20+0x13,32);//0通道有效数据宽度
	NRF24L01_Write_Reg(0x20+0x14,32);//0通道有效数据宽度 
	NRF24L01_Write_Reg(0x20+0x15,32);//0通道有效数据宽度
	NRF24L01_Write_Reg(0x20+0x16,32);//0通道有效数据宽度		
	CE = 1; 
}
void NRF24L01_TX_Mode(u8 FQ)
{
    CE = 0;
	NRF24L01_Write_Buf(0x20+0x10,(u8*)ip,5);//D′TX·￠é?μ??· 
  	NRF24L01_Write_Buf(0x20+0x0a,(u8*)ip0,5); 

  	NRF24L01_Write_Reg(0x20+0x01,0x00);    
  	NRF24L01_Write_Reg(0x20+0x02,0x3f);  
  	NRF24L01_Write_Reg(0x20+0x04,0xff);
  	NRF24L01_Write_Reg(0x20+0x05,FQ);    
  	NRF24L01_Write_Reg(0x20+0x06,0x0f);  
//	NRF24L01_Write_Reg(0x20+0x06,0xa7);
	NRF24L01_Write_Reg(0x20+0x00,0x0e);  

	CE = 1;
}
//?ì2a24L01ê?·?′??ú
//·μ???μ:0￡?3é1|;1￡?ê§°ü	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i; 
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//D′è?5??×??úμ?μ??·.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //?á3?D′è?μ?μ??·  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//?ì2a24L01′í?ó	
	return 0;		 //?ì2aμ?24L01
}	 
	 


