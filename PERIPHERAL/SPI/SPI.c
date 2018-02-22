/******************************************************************************
 * @file     	SPI.c
 * @brief    	Drivers for SPI peripheral in lpc17xx.
 * @version  	1.1
 * @date     	20. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "SPI.h"

/****** public variable ******/
uint8_t SPI_Sending = SPI_FALSE;											//正在发送
int SPI_RecByteNum = 0;																//记录SPI接收缓冲区中的字节数
char Rec_FlagByte = 0x3f;		//发送0x3f，使ADS1256发回一个数据，之所以选用0x3f，是因为0x3f在ADS1256通讯中并没有特殊含义

#ifdef USE_SPI_CIECLEBUFFER
	char spi_rec_cbuffer[100];
	char spi_tra_cbuffer[100];
	ST_CircleBuff pt_spi_rec_cbuffer;
	ST_CircleBuff pt_spi_tra_cbuffer;
	char SPI_cbuffer[100];
#endif

/****** private variable ******/
uint8_t receive_index = SPI_FALSE;										//表示是否要收数据

/**
  * @brief  Initialize the SPI.
  * @param  MODE: 	主从模式（SPI_MASTER or SPI_SLAVER）
	*					CPHA:		时钟相位（SPI_CPHA_FIRST or SPI_CPHA_SECOND）
	*					CPOL:		时钟极性（SPI_CPOL_HIGH or SPI_CPOL_LOW）
	*					LSBF:		高位在前还是低位在前（SPI_LSBF_MSB or SPI_LSBF_LSB）
	*					InterruptEnable:	中断使能（SPI_INT_ENABLE or SPI_INT_DISABLE）
  * @retval None 
  */
void SPI_Init(uint8_t MODE, uint8_t CPHA, uint8_t CPOL, uint8_t LSBF, uint8_t InterruptEnable)
{
	if(MODE == SPI_MASTER)
	{		
		/* 初始化SPI引脚   */   
    LPC_PINCON->PINSEL0 |= (0x03ul << 30);                             /* 设置P0.15脚为SCK脚  */
    LPC_PINCON->PINSEL1 &= ~(0x03 << 0);
    LPC_GPIO0->FIODIR   |=  (1 << 16);                                    /* 设置SPI的片选引脚*/
    LPC_PINCON->PINSEL1 |=  (0x03 << 2) | (0x03 << 4);                 /* 设置P0.17、P0.18    */
                                                                        /* 引脚为SPI引脚       */
		LPC_SPI->SPCCR  =  0x48;                                            /* 设置SPI时钟分频     */
		LPC_SPI->SPCR |= (0 << 2);                                      /* SPI控制器每次传输，每次传送8位*/
		LPC_SPI->SPCR |= (1 << 5);																			/*设置为SPI主机模式*/
		
		SPI_CE_Low();																													//选中从机
	}
	else if(MODE == SPI_SLAVER)
	{		
		/* 设置SPI的通讯脚 */
    LPC_PINCON->PINSEL1 |=  (0x03 << 0)|(0x03 << 2)|(0x03 << 4);        /* 配置P0.16为SSEL0,P0.17为MISO0,*/
                                                                        /* P0.18为MOSI0                  */   
    LPC_PINCON->PINSEL0 |=  (0x03ul << 30);                             /* 设置P0.15为SCK0功能           */
		LPC_SPI->SPCR &= ~(1 << 5);																						//设置SPI从机模式
	}
	if(CPHA == SPI_CPHA_FIRST)
		LPC_SPI->SPCR &= ~(1 << 3);																//时钟相位，设置在SCK的第一个时钟沿进行采样
	else if(CPHA == SPI_CPHA_SECOND)
		LPC_SPI->SPCR |= (1 << 3);																//时钟相位，设置在SCK的第二个时钟沿进行采样
	
	if(CPOL == SPI_CPOL_HIGH)
		LPC_SPI->SPCR &= ~(1 << 4);																//时钟极性，设置SCK为高有效
	else if(CPOL == SPI_CPOL_LOW)
		LPC_SPI->SPCR |= (1 << 4);																//时钟极性，设置SCK为低有效
	
	if(LSBF == SPI_LSBF_MSB)
		LPC_SPI->SPCR &= ~(1 << 6);																//SPI数据传输MSB（位7）在先
	else if(LSBF == SPI_LSBF_LSB)
		LPC_SPI->SPCR |= (1 << 6);																//SPI数据传输LSB（位0）在先
	
	if(InterruptEnable == SPI_INT_DISABLE)
		LPC_SPI->SPCR &= ~(1 << 7);																//禁止SPI中断
	else if(InterruptEnable == SPI_INT_ENABLE)
	{
		LPC_SPI->SPCR |= (1 << 7);																//开启SPI中断
		NVIC_EnableIRQ(SPI_IRQn);                                           /* 初始化SPI的中断接口           */
    NVIC_SetPriority(SPI_IRQn, 2);      
	}
}

/**
  * @brief  Send data by SPI.
  * @param  pt_cb: 				Point of SPI circle buffer.
	*					SPI_buffer:		Point of SPI buffer
	*					BufferLen:		Length of SPI buffer
	*					Valid_tran:		Valid of transmit data
  * @retval None 
  */
void Send_SPI(PST_CircleBuff pt_cb, char *SPI_buffer, uint8_t BufferLen, uint8_t Valid_tran)
{
	char OneSpiByte;
	if(Valid_tran == SPI_VALID_TRUE)																			//数据有效，即发送数据
		WriteDataToCirBuff(pt_cb, SPI_buffer, BufferLen);
	else if(Valid_tran == SPI_VALID_FALSE)																//数据无效，即接收数据
		WriteDataToCirBuff(pt_cb, &Rec_FlagByte, 1);
	if(SPI_Sending == SPI_FALSE)																					//如果SPI并没有正在发送
	{
		ReadDataFromCirBuff(pt_cb, &OneSpiByte, 1);													//在发送缓冲区中读取一个字节数据发送
		if(OneSpiByte == Rec_FlagByte)
		{
			receive_index = SPI_TREU;
			SPI_CE_Low();
			LPC_SPI->SPDR = OneSpiByte;
		}
		else
		{
			receive_index = SPI_FALSE;
			SPI_CE_Low();
			LPC_SPI->SPDR = OneSpiByte;
		}
		SPI_Sending = SPI_TREU;
	}
}

/**
  * @brief  Read data from SPI.
  * @param  pt_cb: 								Point of SPI circle buffer.
	*					DestinationAddress:		Address of Destination
	*					len:									Length of buffer
  * @retval 实际读取的字节数	返回0表示读取失败
  */
uint8_t Read_SPI(PST_CircleBuff pt_cb, char *DestinationAddress, int len)
{
	int reLEN = 0;
	if(SPI_RecByteNum >= len)						//如果SPI接受缓冲区中的字节数大于等于所要读取的字节数
	{
		reLEN = ReadDataFromCirBuff(pt_cb, DestinationAddress, len);
		SPI_RecByteNum = SPI_RecByteNum-reLEN;
		return reLEN;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  Interrupt of SPI.
  * @param  None
  * @retval None 
  */
void  SPI_IRQHandler(void)
{
  uint32_t tmp = 0;
	char OneSpiByte;
	char reDAata;
	if((LPC_SPI->SPSR & (1 << 7)) == (1 << 7))									//传输完成标志位
	{
		tmp = (uint32_t)LPC_SPI->SPSR;   
		if(receive_index == SPI_TREU)
		{
			reDAata=LPC_SPI->SPDR;
			WriteDataToCirBuff(&pt_spi_rec_cbuffer,&reDAata,1);
			SPI_RecByteNum++;
			receive_index = SPI_FALSE;
		}
		LPC_SPI->SPINT = 0x01;                                              /* 清除标志位                   */
//			CE_High();	
		if(GetFreeLengthOfCirBuff(&pt_spi_tra_cbuffer) > 0)	
		{
			ReadDataFromCirBuff(&pt_spi_tra_cbuffer,&OneSpiByte,1);
			if(OneSpiByte == Rec_FlagByte)
			{
				receive_index = SPI_TREU;
				SPI_CE_Low();
				LPC_SPI->SPDR = OneSpiByte;
			}
			else
			{
				receive_index = SPI_FALSE;
				SPI_CE_Low();
				LPC_SPI->SPDR = OneSpiByte;
			}
			SPI_Sending = SPI_TREU;
		}
		else
		{
			SPI_Sending = SPI_FALSE;
			#ifdef USE_ADS1256_SEISMIC_SIGNAL
				a_Event.flag = ADS1256_TRA_FINISH_EVENT;
				EventQueue_insert(&ads1256_EventQueue, &a_Event);
			#endif
		}
		tmp = tmp;
	}
}

/********************************* End Of File ********************************/
