/******************************************************************************
 * @file     	SPI.c
 * @brief    	Drivers for SPI peripheral in lpc17xx.
 * @version  	1.1
 * @date     	20. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "SPI.h"

/****** public variable ******/
uint8_t SPI_Sending = SPI_FALSE;											//���ڷ���
int SPI_RecByteNum = 0;																//��¼SPI���ջ������е��ֽ���
char Rec_FlagByte = 0x3f;		//����0x3f��ʹADS1256����һ�����ݣ�֮����ѡ��0x3f������Ϊ0x3f��ADS1256ͨѶ�в�û�����⺬��

#ifdef USE_SPI_CIECLEBUFFER
	char spi_rec_cbuffer[100];
	char spi_tra_cbuffer[100];
	ST_CircleBuff pt_spi_rec_cbuffer;
	ST_CircleBuff pt_spi_tra_cbuffer;
	char SPI_cbuffer[100];
#endif

/****** private variable ******/
uint8_t receive_index = SPI_FALSE;										//��ʾ�Ƿ�Ҫ������

/**
  * @brief  Initialize the SPI.
  * @param  MODE: 	����ģʽ��SPI_MASTER or SPI_SLAVER��
	*					CPHA:		ʱ����λ��SPI_CPHA_FIRST or SPI_CPHA_SECOND��
	*					CPOL:		ʱ�Ӽ��ԣ�SPI_CPOL_HIGH or SPI_CPOL_LOW��
	*					LSBF:		��λ��ǰ���ǵ�λ��ǰ��SPI_LSBF_MSB or SPI_LSBF_LSB��
	*					InterruptEnable:	�ж�ʹ�ܣ�SPI_INT_ENABLE or SPI_INT_DISABLE��
  * @retval None 
  */
void SPI_Init(uint8_t MODE, uint8_t CPHA, uint8_t CPOL, uint8_t LSBF, uint8_t InterruptEnable)
{
	if(MODE == SPI_MASTER)
	{		
		/* ��ʼ��SPI����   */   
    LPC_PINCON->PINSEL0 |= (0x03ul << 30);                             /* ����P0.15��ΪSCK��  */
    LPC_PINCON->PINSEL1 &= ~(0x03 << 0);
    LPC_GPIO0->FIODIR   |=  (1 << 16);                                    /* ����SPI��Ƭѡ����*/
    LPC_PINCON->PINSEL1 |=  (0x03 << 2) | (0x03 << 4);                 /* ����P0.17��P0.18    */
                                                                        /* ����ΪSPI����       */
		LPC_SPI->SPCCR  =  0x48;                                            /* ����SPIʱ�ӷ�Ƶ     */
		LPC_SPI->SPCR |= (0 << 2);                                      /* SPI������ÿ�δ��䣬ÿ�δ���8λ*/
		LPC_SPI->SPCR |= (1 << 5);																			/*����ΪSPI����ģʽ*/
		
		SPI_CE_Low();																													//ѡ�дӻ�
	}
	else if(MODE == SPI_SLAVER)
	{		
		/* ����SPI��ͨѶ�� */
    LPC_PINCON->PINSEL1 |=  (0x03 << 0)|(0x03 << 2)|(0x03 << 4);        /* ����P0.16ΪSSEL0,P0.17ΪMISO0,*/
                                                                        /* P0.18ΪMOSI0                  */   
    LPC_PINCON->PINSEL0 |=  (0x03ul << 30);                             /* ����P0.15ΪSCK0����           */
		LPC_SPI->SPCR &= ~(1 << 5);																						//����SPI�ӻ�ģʽ
	}
	if(CPHA == SPI_CPHA_FIRST)
		LPC_SPI->SPCR &= ~(1 << 3);																//ʱ����λ��������SCK�ĵ�һ��ʱ���ؽ��в���
	else if(CPHA == SPI_CPHA_SECOND)
		LPC_SPI->SPCR |= (1 << 3);																//ʱ����λ��������SCK�ĵڶ���ʱ���ؽ��в���
	
	if(CPOL == SPI_CPOL_HIGH)
		LPC_SPI->SPCR &= ~(1 << 4);																//ʱ�Ӽ��ԣ�����SCKΪ����Ч
	else if(CPOL == SPI_CPOL_LOW)
		LPC_SPI->SPCR |= (1 << 4);																//ʱ�Ӽ��ԣ�����SCKΪ����Ч
	
	if(LSBF == SPI_LSBF_MSB)
		LPC_SPI->SPCR &= ~(1 << 6);																//SPI���ݴ���MSB��λ7������
	else if(LSBF == SPI_LSBF_LSB)
		LPC_SPI->SPCR |= (1 << 6);																//SPI���ݴ���LSB��λ0������
	
	if(InterruptEnable == SPI_INT_DISABLE)
		LPC_SPI->SPCR &= ~(1 << 7);																//��ֹSPI�ж�
	else if(InterruptEnable == SPI_INT_ENABLE)
	{
		LPC_SPI->SPCR |= (1 << 7);																//����SPI�ж�
		NVIC_EnableIRQ(SPI_IRQn);                                           /* ��ʼ��SPI���жϽӿ�           */
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
	if(Valid_tran == SPI_VALID_TRUE)																			//������Ч������������
		WriteDataToCirBuff(pt_cb, SPI_buffer, BufferLen);
	else if(Valid_tran == SPI_VALID_FALSE)																//������Ч������������
		WriteDataToCirBuff(pt_cb, &Rec_FlagByte, 1);
	if(SPI_Sending == SPI_FALSE)																					//���SPI��û�����ڷ���
	{
		ReadDataFromCirBuff(pt_cb, &OneSpiByte, 1);													//�ڷ��ͻ������ж�ȡһ���ֽ����ݷ���
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
  * @retval ʵ�ʶ�ȡ���ֽ���	����0��ʾ��ȡʧ��
  */
uint8_t Read_SPI(PST_CircleBuff pt_cb, char *DestinationAddress, int len)
{
	int reLEN = 0;
	if(SPI_RecByteNum >= len)						//���SPI���ܻ������е��ֽ������ڵ�����Ҫ��ȡ���ֽ���
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
	if((LPC_SPI->SPSR & (1 << 7)) == (1 << 7))									//������ɱ�־λ
	{
		tmp = (uint32_t)LPC_SPI->SPSR;   
		if(receive_index == SPI_TREU)
		{
			reDAata=LPC_SPI->SPDR;
			WriteDataToCirBuff(&pt_spi_rec_cbuffer,&reDAata,1);
			SPI_RecByteNum++;
			receive_index = SPI_FALSE;
		}
		LPC_SPI->SPINT = 0x01;                                              /* �����־λ                   */
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
