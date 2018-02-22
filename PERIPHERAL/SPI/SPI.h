/******************************************************************************
 * @file     	SPI.h
 * @brief    	Drivers for SPI peripheral in lpc17xx.
 * @version  	1.1
 * @date     	20. May. 2016
 * @note		 	None
 ******************************************************************************/

#ifndef __SPI_H
#define __SPI_H
#include "lpc17xx.h"
#include "circlebuff.h"
#include "TriggerFSM.h"

#define    SPI_CS      (1 << 16)                                        /* P0.16口为片选脚              */
#define    SPI_CE_Low()    (LPC_GPIO0->FIOCLR |=  SPI_CS)                   /* 选中从器件                   */
#define    SPI_CE_High()   (LPC_GPIO0->FIOSET |=  SPI_CS)

#define SPI_FALSE		0x00
#define SPI_TREU		0x01

#define SPI_MASTER     		0x00						//主机模式
#define SPI_SLAVER 				0x01						//从机模式

#define SPI_CPHA_FIRST   	0x00						//时钟相位，设置在SCK的第一个时钟沿进行采样
#define SPI_CPHA_SECOND  	0x01						//时钟相位，设置在SCK的第二个时钟沿进行采样

#define SPI_CPOL_HIGH			0x00						//时钟极性，设置SCK为高有效
#define SPI_CPOL_LOW 			0x01						//时钟极性，设置SCK为低有效

#define SPI_LSBF_MSB			0x00						//SPI数据传输MSB（位7）在先
#define SPI_LSBF_LSB			0x01						//SPI数据传输LSB（位0）在先

#define SPI_INT_ENABLE 		0x00						//开启SPI中断
#define SPI_INT_DISABLE 	0x01						//禁止SPI中断

#define SPI_VALID_TRUE		0x00						//发送有效数据
#define SPI_VALID_FALSE		0x01						//发送无效数据，用以接收数据

/****** public variable ******/
extern uint8_t SPI_Sending;
extern int SPI_RecByteNum;
extern char Rec_FlagByte;
 
#ifdef USE_SPI_CIECLEBUFFER
	extern char spi_rec_cbuffer[100];
	extern char spi_tra_cbuffer[100];
	extern ST_CircleBuff pt_spi_rec_cbuffer;
	extern ST_CircleBuff pt_spi_tra_cbuffer;
	extern char SPI_cbuffer[100];
#endif
/*****************************/

/**
  * @brief  Initialize the SPI.
  * @param  MODE: 	主从模式（SPI_MASTER or SPI_SLAVER）
	*					CPHA:		时钟相位（SPI_CPHA_FIRST or SPI_CPHA_SECOND）
	*					CPOL:		时钟极性（SPI_CPOL_HIGH or SPI_CPOL_LOW）
	*					LSBF:		高位在前还是低位在前（SPI_LSBF_MSB or SPI_LSBF_LSB）
	*					InterruptEnable:	中断使能（SPI_INT_ENABLE or SPI_INT_DISABLE）
  * @retval None 
  */
void SPI_Init(uint8_t MODE, uint8_t CPHA, uint8_t CPOL, uint8_t LSBF, uint8_t InterruptEnable);

/**
  * @brief  Send data by SPI.
  * @param  pt_cb: 				Point of SPI circle buffer.
	*					SPI_buffer:		Point of SPI buffer
	*					BufferLen:		Length of SPI buffer
	*					Valid_tran:		Valid of transmit data
  * @retval None 
  */
void Send_SPI(PST_CircleBuff pt_cb, char *SPI_buffer, uint8_t BufferLen, uint8_t Valid_tran);

/**
  * @brief  Read data from SPI.
  * @param  pt_cb: 								Point of SPI circle buffer.
	*					DestinationAddress:		Address of Destination
	*					len:									Length of buffer
  * @retval 实际读取的字节数	返回0表示读取失败
  */
uint8_t Read_SPI(PST_CircleBuff pt_cb, char *DestinationAddress, int len);

#endif

/********************************* End Of File ********************************/
