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

#define    SPI_CS      (1 << 16)                                        /* P0.16��ΪƬѡ��              */
#define    SPI_CE_Low()    (LPC_GPIO0->FIOCLR |=  SPI_CS)                   /* ѡ�д�����                   */
#define    SPI_CE_High()   (LPC_GPIO0->FIOSET |=  SPI_CS)

#define SPI_FALSE		0x00
#define SPI_TREU		0x01

#define SPI_MASTER     		0x00						//����ģʽ
#define SPI_SLAVER 				0x01						//�ӻ�ģʽ

#define SPI_CPHA_FIRST   	0x00						//ʱ����λ��������SCK�ĵ�һ��ʱ���ؽ��в���
#define SPI_CPHA_SECOND  	0x01						//ʱ����λ��������SCK�ĵڶ���ʱ���ؽ��в���

#define SPI_CPOL_HIGH			0x00						//ʱ�Ӽ��ԣ�����SCKΪ����Ч
#define SPI_CPOL_LOW 			0x01						//ʱ�Ӽ��ԣ�����SCKΪ����Ч

#define SPI_LSBF_MSB			0x00						//SPI���ݴ���MSB��λ7������
#define SPI_LSBF_LSB			0x01						//SPI���ݴ���LSB��λ0������

#define SPI_INT_ENABLE 		0x00						//����SPI�ж�
#define SPI_INT_DISABLE 	0x01						//��ֹSPI�ж�

#define SPI_VALID_TRUE		0x00						//������Ч����
#define SPI_VALID_FALSE		0x01						//������Ч���ݣ����Խ�������

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
  * @param  MODE: 	����ģʽ��SPI_MASTER or SPI_SLAVER��
	*					CPHA:		ʱ����λ��SPI_CPHA_FIRST or SPI_CPHA_SECOND��
	*					CPOL:		ʱ�Ӽ��ԣ�SPI_CPOL_HIGH or SPI_CPOL_LOW��
	*					LSBF:		��λ��ǰ���ǵ�λ��ǰ��SPI_LSBF_MSB or SPI_LSBF_LSB��
	*					InterruptEnable:	�ж�ʹ�ܣ�SPI_INT_ENABLE or SPI_INT_DISABLE��
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
  * @retval ʵ�ʶ�ȡ���ֽ���	����0��ʾ��ȡʧ��
  */
uint8_t Read_SPI(PST_CircleBuff pt_cb, char *DestinationAddress, int len);

#endif

/********************************* End Of File ********************************/
