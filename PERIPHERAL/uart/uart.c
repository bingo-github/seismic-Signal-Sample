/*****************************************************************************
 *   uart.c:  UART API file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
//�ú�����֧�ֶ�UART1�Ĳ���, ������ʹ��UART1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <LPC17xx.H>
#include "uart.h"
#include "circlebuff.h"

#ifdef	USE_TIMER_PACKET
static int nDelayTime_uart0;
static int nDelayTime_uart2;
static int nDelayTime_uart3;

void uart0proc_timerproc(void)
{
	if (nDelayTime_uart0 == 0)
	{	
		return ;
	}else{	
		nDelayTime_uart0--;
		
		if (nDelayTime_uart0 == 0)
		{
			if (g_UART[0].IsExistPacket == 1)
			{
				g_UART[0].ErrorNo = 1; //����δ��������ݰ�
			}
			g_UART[0].IsExistPacket = 1;
			g_UART[0].PacketLen = GetFreeLengthOfCirBuff(g_UART[0].pt_uart_rec_cbuffer);	
		}
	}
}

void uart2proc_timerproc(void)
{
	if (nDelayTime_uart2 == 0)
	{	
		return ;
	}else{	
		nDelayTime_uart2--;
		
		if (nDelayTime_uart2 == 0)
		{
			if (g_UART[2].IsExistPacket == 1)
			{
				g_UART[2].ErrorNo = 1; //����δ��������ݰ�
			}
			g_UART[2].IsExistPacket = 1;
			g_UART[2].PacketLen = GetFreeLengthOfCirBuff(g_UART[2].pt_uart_rec_cbuffer);	
		}
	}
}

void uart3proc_timerproc(void)
{
	if (nDelayTime_uart3 == 0)
	{	
		return ;
	}else{	
		nDelayTime_uart3--;
		
		if (nDelayTime_uart3 == 0)
		{
			if (g_UART[3].IsExistPacket == 1)
			{
				g_UART[3].ErrorNo = 1; //����δ��������ݰ�
			}
			g_UART[3].IsExistPacket = 1;
			g_UART[3].PacketLen = GetFreeLengthOfCirBuff(g_UART[3].pt_uart_rec_cbuffer);	
		}
	}
}

#endif

ST_UARTState g_UART[4];

/************************************************************************************************************
**����ԭ��		:  void UARTBuffer_Init(unsigned short int portNum, PST_CircleBuff pt_uart_rec_cbuffer, PST_CircleBuff pt_uart_tra_cbuffer,char 
                                    *pt_rec_buffer, char *pt_tra_buffer,int rec_buf_length, int tra_buf_length)
**����˵��		:  ST_UARTState *pt_UART   UART�ṹ�����
                     pt_rec_buffer 		  ���ջ�������ַ
										 pt_tra_buffer  	  ���ͻ�������ַ
										 rec_buf_length		  ���ջ���������
										 tra_buf_length		  ���ͻ���������
**����ֵ		 :	
**˵	��	 :	��ʼ����̫�����ܷ��ͻ�����
**ע���   :	
************************************************************************************************************/  
void UARTBuffer_Init(unsigned short int portNum, PST_CircleBuff pt_uart_rec_cbuffer, PST_CircleBuff pt_uart_tra_cbuffer,char *pt_rec_buffer, char *pt_tra_buffer,int rec_buf_length, int tra_buf_length)                  
{
	g_UART[portNum].pt_uart_rec_cbuffer=pt_uart_rec_cbuffer;
	g_UART[portNum].pt_uart_tra_cbuffer=pt_uart_tra_cbuffer;
	Define_CircleBuff_CirclebufLen(g_UART[portNum].pt_uart_rec_cbuffer,pt_rec_buffer,rec_buf_length);
	Define_CircleBuff_CirclebufLen(g_UART[portNum].pt_uart_tra_cbuffer,pt_tra_buffer,tra_buf_length);
}

/************************************************************************************************************
**����ԭ��	:  WriteDataToUART_Rx_Cache(char port_num,ST_UARTState *pt_uart)
**����˵��	:  	ST_UARTState *pt_uart  ���ڽṹ�����
**����ֵ		:	    1����ʾ���ݰ�д��ɹ�
                  0: ��ʾʧ��
**˵	��		:	�ѼĴ����е����ݷŵ��û��Զ���Ļ��λ�������
**ע �� �� 		:	
************************************************************************************************************/
int WriteDataToUART_Rx_Cache(unsigned short int port_num, ST_UARTState *pt_uart)
{
	LPC_UART_TypeDef *ptr_uart_base;

	int writen_data_length;
	
	if (port_num >= 4)
		return -1;
	
	if (port_num == 0)
	{
		ptr_uart_base = LPC_UART0; 
	}else if (port_num == 2)
	{
		ptr_uart_base = LPC_UART2; 
	}else if (port_num == 3)
	{
		ptr_uart_base = LPC_UART3; 
	}else{
		return -1;
	}
	
	writen_data_length = WriteDataToCirBuff(pt_uart->pt_uart_rec_cbuffer,(char *)& ptr_uart_base->RBR,1);
	
	return (writen_data_length);	
}
/************************************************************************************************************
**����ԭ��  :  int  ReadUART_Cache(unsigned short int portNum, char *DestinationAddress, int len)
**����˵��  :  portNum�����ڶ˿ں�
               DestinationAddress���������ݴ�ŵĵ�ַ
               len��׼�������ĳ���, �����������ݺ�ʵ�ʶ������ݲ�һ�»���ô��?
**����ֵ  :    �� 
**˵ ��  :     �Ӵ��ڵĽ��ջ������ж�ȡ���ݵ�ָ���ĵ�ַ
************************************************************************************************************/
int  ReadUART_Cache(unsigned short int portNum, char *DestinationAddress, int len)
{
	int LEN=0;
	LEN = ReadDataFromCirBuff(g_UART[portNum].pt_uart_rec_cbuffer,DestinationAddress,len);
	
	return LEN;
}
/********************************************************************************************************
** Function name:       int	WriteUART_Cache(unsigned short int portNum, char *SourceAddress, int len)
** Descriptions:	      portNum�����ڶ˿ں�
                        SourceAddress���������ݴ�ŵĵ�ַ
                        len��׼��д������ݳ��� 
** Returned value:			ʵ��д�뻺����C�����ݳ���
												CircleBuff_overflow�����ͻ��������
**********************************************************************************************************/
int	WriteUART_Cache(unsigned short int portNum, char *SourceAddress, int len)
{
	int LEN = 0;
	uint8_t LSRValue;
	LPC_UART_TypeDef *uart_base;
	
	if (portNum == 0)
	{
		uart_base = LPC_UART0;
	}else if (portNum == 2)
	{
		uart_base = LPC_UART2;
	}else if (portNum == 3)
	{
		uart_base = LPC_UART3;
	}else{
		return (-1); //invalid port number
	}


	LEN = WriteDataToCirBuff(g_UART[portNum].pt_uart_tra_cbuffer,SourceAddress,len);
	
	//�ж��£����Ͷ˿��Ƿ�æµ�����æ��������������У��������ɹ���	
	LSRValue = uart_base->LSR;
	if (LSRValue & LSR_THRE)
	{
		g_UART[portNum].TxEmpty = 1;
		ReadDataFromUART_Tx_Cache(portNum, &g_UART[portNum]);			
	}
	
	return LEN;
}
/************************************************************************************************************
**����ԭ��  :    int  UART_ReadFromTBuf(PST_UART_CircleBuff pt_uart)
**����˵��  :  PST_UART_CircleBuff pt_uart��������ָ��
**����ֵ  : 1�������ɹ�
            0������ʧ��
**˵ ��  : ���ݴӷ��ͻ�������������
************************************************************************************************************/
int  ReadDataFromUART_Tx_Cache(unsigned short int port_num,ST_UARTState *pt_uart)
{ 
 	LPC_UART_TypeDef *ptr_uart_base;

	if (port_num == 0)
	{
		ptr_uart_base = LPC_UART0; 
	}else if (port_num == 2)
	{
		ptr_uart_base = LPC_UART2; 
	}else if (port_num == 3)
	{
		ptr_uart_base = LPC_UART3; 
	}else{
		return -1;
	}
	
	if ( pt_uart->pt_uart_tra_cbuffer->TXNum > 0)       // ������������
  {
		ptr_uart_base->THR =pt_uart->pt_uart_tra_cbuffer->Circlebuf[pt_uart->pt_uart_tra_cbuffer->head];
	  
		pt_uart->pt_uart_tra_cbuffer->head++;
		pt_uart->pt_uart_tra_cbuffer->TXNum--;
    if (pt_uart->pt_uart_tra_cbuffer->head == pt_uart->pt_uart_tra_cbuffer->CirclebufLen)
    {
			pt_uart->pt_uart_tra_cbuffer->head = 0;
    }
		if((pt_uart->pt_uart_tra_cbuffer->TXNum==0)||(pt_uart->pt_uart_tra_cbuffer->head==pt_uart->pt_uart_tra_cbuffer->tail))
		{
			pt_uart->pt_uart_tra_cbuffer->tail=0;
			pt_uart->pt_uart_tra_cbuffer->head=0;
    }
		return 1;
	}
	else 
		return 0;
}

/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART0_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
	
	
  IIRValue = LPC_UART0->IIR;
 
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
		LSRValue = LPC_UART0->LSR;
		/* Receive Line Status */
		if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			g_UART[0].Status = LSRValue;
			Dummy = LPC_UART0->RBR;		/* Dummy read on RX to clear interrupt, then bail out */
			return;
		}
	}
	else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
		
		WriteDataToUART_Rx_Cache(0, &g_UART[0]);
		
		#ifdef	USE_TIMER_PACKET
		nDelayTime_uart0 = 8; //��ʱ8ms
		#endif
  }
  else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
  {
		/* Character Time-out indicator */
		g_UART[0].Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
		/* THRE interrupt */
		LSRValue = LPC_UART0->LSR;		/* Check status in the LSR to see if valid data in U0THR or not */
		if ( LSRValue & LSR_THRE )
		{
			g_UART[0].TxEmpty = 1;
			ReadDataFromUART_Tx_Cache(0,&g_UART[0]);			
		}
		else
		{
			g_UART[0].TxEmpty = 0;
		}
  }    
}

/*****************************************************************************
** Function name:		UART2_IRQHandler
**
** Descriptions:		UART2 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART2_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
	
	
  IIRValue = LPC_UART2->IIR;
 
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
		LSRValue = LPC_UART2->LSR;
		/* Receive Line Status */
		if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			g_UART[2].Status = LSRValue;
			Dummy = LPC_UART2->RBR;		/* Dummy read on RX to clear interrupt, then bail out */
			return;
		}
	}
	else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
		
		WriteDataToUART_Rx_Cache(2, &g_UART[2]);
		
		#ifdef	USE_TIMER_PACKET
		nDelayTime_uart2 = 8; //��ʱ8ms
		#endif
  }
  else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
  {
		/* Character Time-out indicator */
		g_UART[2].Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
		/* THRE interrupt */
		LSRValue = LPC_UART2->LSR;		/* Check status in the LSR to see if valid data in U0THR or not */
		if ( LSRValue & LSR_THRE )
		{
			g_UART[2].TxEmpty = 1;
			ReadDataFromUART_Tx_Cache(2,&g_UART[2]);			
		}
		else
		{
			g_UART[2].TxEmpty = 0;
		}
  }    
}

/*****************************************************************************
** Function name:		UART3_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART3_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
	
	
  IIRValue = LPC_UART3->IIR;
 
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
		LSRValue = LPC_UART3->LSR;
		/* Receive Line Status */
		if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			g_UART[3].Status = LSRValue;
			Dummy = LPC_UART3->RBR;		/* Dummy read on RX to clear interrupt, then bail out */
			return;
		}
	}
	else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
		
		WriteDataToUART_Rx_Cache(3, &g_UART[3]);
		
		#ifdef	USE_TIMER_PACKET
		nDelayTime_uart3 = 8; //��ʱ8ms
		#endif
  }
  else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
  {
		/* Character Time-out indicator */
		g_UART[3].Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
		/* THRE interrupt */
		LSRValue = LPC_UART3->LSR;		/* Check status in the LSR to see if valid data in U0THR or not */
		if ( LSRValue & LSR_THRE )
		{
			g_UART[3].TxEmpty = 1;
			ReadDataFromUART_Tx_Cache(3,&g_UART[3]);			
		}
		else
		{
			g_UART[3].TxEmpty = 0;
		}
  }    
}


/************************************************************************************************************
**����ԭ��:      int UARTInit( unsigned short int PortNum, int baudrate ) 
**����˵��:      ���ڳ�ʼ��
**����˵����     PortNum�����ڱ�ţ�0��2��3 //�Ѳ��Զ˿�Ϊ0
                 baudrate:������
**����ֵ:	       1����ʾ���óɹ�
                 0����ʾ���ò��ɹ�
**ע �� �� 		:	1�����ó�ʼ����ʱ��ҪŪ������ڵ�ʱ���Ƕ��ټ�ϵͳ�ָ����ڵ�ʱ�ӣ�����������й�ϵ��Ҳ���Ǻ����е�	FpclkֵҪ��.h�ļ��ж����
                2������ĺ���ֵ�����˹̶��Ŀ��߶��ڵĴ��ڣ�Ҫ���뻻������Ŀ��ߣ���ֻ��Ҫ�޸Ĺܽ����ü���
                   ���磺����3�����溯�����õ�����P0.0��P0.3��Ҫ������P0.25��P0.26��ֻ��Ҫ�ı䴮��3����Ӧ����䣺
                   LPC_PINCON->PINSEL1 |= (3<<18)|(3<<20);
************************************************************************************************************/
int UARTInit( unsigned short int PortNum, int baudrate )
{
  uint32_t Fdiv;
  LPC_UART_TypeDef *ptr_uart_base;
	
	if (PortNum == 0)
	{
		LPC_SC->PCONP |= (1 << 3);
		LPC_PINCON->PINSEL0 |= 	0x00000050; //TXD P0.2, RXD P0.3
		ptr_uart_base = LPC_UART0;
	}else if (PortNum == 2)
	{
		LPC_SC->PCONP |= (1 << 24);
		LPC_PINCON->PINSEL0 |= 	0x00500000; //TXD P0.10 , RXD P0.11
		ptr_uart_base = LPC_UART2;
	}else if(PortNum == 3)
	{
		LPC_SC->PCONP |= (1 << 25);
		LPC_PINCON->PINSEL0 |= 	0x0000000A; //TXD P0.0 , RXD P0.1
		ptr_uart_base = LPC_UART3;
	}else{
		return -1;
	}
	
	ptr_uart_base->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( Fpclk / 16 ) / baudrate ;	/*baud rate */
	ptr_uart_base->DLM = Fdiv / 256;							
	ptr_uart_base->DLL = Fdiv % 256;
	ptr_uart_base->LCR = 0x03;		/* DLAB = 0 */
	ptr_uart_base->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */
	ptr_uart_base->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
	
	if ( PortNum == 0 )
  {
			NVIC_EnableIRQ(UART0_IRQn);
			//NVIC_SetPriority(UART0_IRQn, NVIC_PRIORTY_UART0); 
			return (1);
  }
	else if ( PortNum == 2 )
  {
			NVIC_EnableIRQ(UART2_IRQn);
			return (1); 
	}
	else if(PortNum == 3)
	{
			NVIC_EnableIRQ(UART3_IRQn);
			return (1); 
	}
	return 0;

}

/******************************************************************************
**                            End Of File
******************************************************************************/
