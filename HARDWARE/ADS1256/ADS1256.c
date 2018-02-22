/******************************************************************************
 * @file     	ADS1256.c
 * @brief    	Drivers for ADS1256.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "ADS1256.h"

#define ADS_REST_High() 	(LPC_GPIO0->FIOSET |= (1 << 20))					//将P0.20拉高
#define ADS_REST_Low()	 	(LPC_GPIO0->FIOCLR |= (1 << 20))					//将P0.20拉低
#define ADS_REST_Value		(LPC_GPIO0->FIOPIN & (1 << 20))						//REST的电平值
#define ADS_REST_BIT		20																				//用于REST的引脚的位数

//define drate codes 				//该采样频率是ADS1256的采样频率，但控制不准，因此因此采用定时器来控制系统的采样频率
#define ADS1256_DRATE_30000SPS   0xF0 
#define ADS1256_DRATE_15000SPS   0xE0 
#define ADS1256_DRATE_7500SPS   0xD0 
#define ADS1256_DRATE_3750SPS   0xC0 
#define ADS1256_DRATE_2000SPS   0xB0 
#define ADS1256_DRATE_1000SPS   0xA1 
#define ADS1256_DRATE_500SPS    0x92 
#define ADS1256_DRATE_100SPS    0x82 
#define ADS1256_DRATE_60SPS     0x72 
#define ADS1256_DRATE_50SPS     0x63 
#define ADS1256_DRATE_30SPS     0x53 
#define ADS1256_DRATE_25SPS     0x43 
#define ADS1256_DRATE_15SPS     0x33 
#define ADS1256_DRATE_10SPS     0x23 
#define ADS1256_DRATE_5SPS      0x13 
#define ADS1256_DRATE_2_5SPS    0x03

// define commands 
#define ADS1256_CMD_WAKEUP   0x00 
#define ADS1256_CMD_RDATA    0x01 
#define ADS1256_CMD_RDATAC   0x03 
#define ADS1256_CMD_SDATAC   0x0f 
#define ADS1256_CMD_RREG     0x10 
#define ADS1256_CMD_WREG     0x50 
#define ADS1256_CMD_SELFCAL  0xf0 
#define ADS1256_CMD_SELFOCAL 0xf1 
#define ADS1256_CMD_SELFGCAL 0xf2 
#define ADS1256_CMD_SYSOCAL  0xf3 
#define ADS1256_CMD_SYSGCAL  0xf4 
#define ADS1256_CMD_SYNC     0xfc 
#define ADS1256_CMD_STANDBY  0xfd 
#define ADS1256_CMD_REST    0xfe 

// define the ADS1256 register values 
#define ADS1256_STATUS       0x00   
#define ADS1256_MUX          0x01   
#define ADS1256_ADCON        0x02   
#define ADS1256_DRATE        0x03   
#define ADS1256_IO           0x04   
#define ADS1256_OFC0         0x05   
#define ADS1256_OFC1         0x06   
#define ADS1256_OFC2         0x07   
#define ADS1256_FSC0         0x08   
#define ADS1256_FSC1         0x09   
#define ADS1256_FSC2         0x0A 

/**
  * @brief  Initialize the I/O for ADS1256(RESET and DRDY).
  * @param  None
  * @retval None 
  */
void AdsIo_Init(void)
{	
	LPC_PINCON->PINSEL1 &= ~(0X03 << 8);	//P0.20做REST用，用该I/O口模拟SPI控制ADS1256，复位信号，低电平有效
	LPC_GPIO0->FIODIR |= (1 << 20);
	//P0.19管脚的GPIO中断功能初始化，用于检测DRDY下降沿从而产生中断
	LPC_PINCON->PINSEL1 &= ~(0x03 << 6);				//设置P0.19为GPIO功能
	LPC_GPIOINT->IO0IntClr |= 0xFFFFFF;					//清零P0口的GPIO中断标志
	LPC_GPIOINT->IO0IntEnF |= (1 << 19);				//设置P0.19为下降沿中断使能
	NVIC_EnableIRQ(EINT3_IRQn);									//DRDY下降沿中断的中断函数在EINT3中断函数中
  NVIC_SetPriority(EINT3_IRQn, 2);
}

/**
  * @brief  Delay for ADS1256.
  * @param  dal:		Delay num
  * @retval None 
  */
void Delay_ads1256(unsigned int dal)
{
	unsigned int i;
	for(i = 0;i < dal*5;i ++);
}

/**
  * @brief  Write a byte to a register.
  * @param  regaddr:		Address of register
	*					databyte:		the byte to write
  * @retval None 
  */
void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
	char temp_Byte[3];
	temp_Byte[0] = (ADS1256_CMD_WREG | (regaddr & 0xF));
	temp_Byte[1] = 0;
	temp_Byte[2] = databyte;
	Send_SPI(&pt_spi_tra_cbuffer,temp_Byte,3,SPI_VALID_TRUE);	//通过SPI给ADS1256发送写寄存器命令
}

/**
  * @brief  设置采样频率.
  * @param  rate:		采样频率
  * @retval None 
  */
void ADS1256_SetSampleRate(uint16_t rate)
{
	uint32_t mr_num;
	mr_num = CLK_TIMER0/rate;				//算出在定时器0时钟为CLK_TIMER0的情况下，每秒中读取AD采样数据SFreq次时，所需要设置的MR0值
	Init_MR_Timer(0,mr_num);					//初始化定时器0的匹配功能
}

/**
  * @brief  设置采样增益.
  * @param  GAIN:		采样增益
  * @retval None 
  */
void ADS1256_SetSampleGain(uint8_t GAIN)
{
	char temp_spiByte[4];
	temp_spiByte[0] = (ADS1256_CMD_WREG | (ADS1256_ADCON & 0xF));
	temp_spiByte[1] = 0;
	temp_spiByte[2] = GAIN;
	temp_spiByte[3] = ADS1256_CMD_SELFCAL;
	Send_SPI(&pt_spi_tra_cbuffer,temp_spiByte,4,SPI_VALID_TRUE);
	Delay_ads1256(30000);
}

/**
  * @brief  Initialize the ADS1256.
  * @param  channel:	采样通道
	*					gain:			采样增益
	*					rate:	采样频率
  * @retval None 
  */
void ADS1256_Init(char channel,char gain,uint16_t rate)
{
	char temp_Byte;
	//定义spi环形缓冲区
	Define_CircleBuff_CirclebufLen(&pt_spi_rec_cbuffer,spi_rec_cbuffer,100);
	Define_CircleBuff_CirclebufLen(&pt_spi_tra_cbuffer,spi_tra_cbuffer,100);
	SPI_Init(SPI_MASTER, SPI_CPHA_SECOND, SPI_CPOL_HIGH, SPI_LSBF_MSB, SPI_INT_ENABLE);
	
	ADS1256_SetSampleRate(rate);							//使用定时器0进行定时采样频率
	
	AdsIo_Init();																		//引脚初始化
	SPI_CE_Low();																		//初始化先拉低片选信号，选中AD芯片
	ADS_REST_High();																//	发送复位命令
	Delay_ads1256(5000);
	ADS1256WREG(ADS1256_STATUS,0x04);               // 高位在前、校准
	ADS1256WREG(ADS1256_MUX,channel);                  // 初始化端口
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_3750SPS); // ADS1256芯片数据10sps
	temp_Byte = ADS1256_CMD_SELFCAL;
	Send_SPI(&pt_spi_tra_cbuffer,&temp_Byte,1,SPI_VALID_TRUE);
	Delay_ads1256(5000);
	ADS1256_SetSampleGain(gain);
}

/**
  * @brief  开始采样.
  * @param  None
  * @retval None 
  */
void ADS1256_startSample(void)
{
	Enable_Timer(0);									//开启定时器0，开始采样
}

/**
  * @brief  停止采样.
  * @param  None
  * @retval None 
  */
void ADS1256_stopSample(void)
{
	Disable_Timer(0);
}

/**
  * @brief  变换采样通道.
  * @param  toChannel: 目标通道
  * @retval None 
  */
void ADS1256_changeCH(char toChannel)
{
	char temp_spiByte[3];
	temp_spiByte[0] = (ADS1256_CMD_WREG | (ADS1256_MUX & 0xF));
	temp_spiByte[1] = 0;
	temp_spiByte[2] = toChannel;
	Send_SPI(&pt_spi_tra_cbuffer,temp_spiByte,3,SPI_VALID_TRUE);
}

/**
  * @brief  采集当前一个通道是数据.
  * @param  None
  * @retval None 
  */
void ADS1256_sampleOneCH(void)
{
	char temp_spiByte[6];
	temp_spiByte[0] = ADS1256_CMD_SYNC;
	temp_spiByte[1] = ADS1256_CMD_WAKEUP;
	temp_spiByte[2] = ADS1256_CMD_RDATA;
	temp_spiByte[3] = Rec_FlagByte;
	temp_spiByte[4] = Rec_FlagByte;
	temp_spiByte[5] = Rec_FlagByte;
	Send_SPI(&pt_spi_tra_cbuffer,temp_spiByte,6,SPI_VALID_TRUE);
}

/**
  * @brief  读取ADS1256采样数据.
  * @param  DestinationAddress：	Address of Destination
	*					len:									Length of buffer
  * @retval None 
  */
uint8_t ADS1256_ReadSampleData(char *DestinationAddress, int len)
{
	int reLEN = 0;
	if(SPI_RecByteNum >= len)						//如果SPI接受缓冲区中的字节数大于等于所要读取的字节数
	{
		reLEN = ReadDataFromCirBuff(&pt_spi_rec_cbuffer, DestinationAddress, len);	//从SPI缓冲区中读取数据
		SPI_RecByteNum = SPI_RecByteNum-reLEN;
		return reLEN;
	}
	else
	{
		return 0;
	}
}

/********************************* End Of File ********************************/
