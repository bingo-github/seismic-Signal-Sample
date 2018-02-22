/*****************************************************************************
 *   uart.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __UART_H 
#define __UART_H
#include "circlebuff.h"

//#define	USE_PACK_TIMING_INT

#define  oper_read   0
#define  oper_write   1

//涉及到LPC1768处理器的时钟, 
#define Fcclk	(100000000)
#define Fpclk	(Fcclk / 4)       

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define BUFSIZE		0xff
#define MAX_UART_RX_DATA_SIZE   256                 // max. incoming TCP data size (even!)
#define RxUARTBuffer   ((unsigned char *)_RxUARTBuffer)
#define UART_RX_BUF      ((unsigned char *)RxUARTBuffer)

#define	USE_TIMER_PACKET

/**************************************************************************
	缓冲区结构体	
***************************************************************************/
typedef struct {
	volatile char	TxEmpty;
	volatile char	Status;
	volatile unsigned short int ErrorNo;
	volatile unsigned short int PacketLen;
	volatile unsigned char	IsExistPacket;
	PST_CircleBuff  pt_uart_rec_cbuffer;	//UART接受数据缓存
	PST_CircleBuff  pt_uart_tra_cbuffer;	//UART发送缓冲
} ST_UARTState, *PST_UARTState;

extern ST_UARTState g_UART[4];

int UARTInit( unsigned short int portNum, int Baudrate );
void UARTBuffer_Init(unsigned short int portNum, PST_CircleBuff pt_uart_rec_cbuffer, PST_CircleBuff pt_uart_tra_cbuffer,char *pt_rec_buffer, char *pt_tra_buffer,int rec_buf_length, int tra_buf_length);
void UART0_IRQHandler (void);
void UART2_IRQHandler (void);
void UART3_IRQHandler (void);
int ReadUART_Cache(unsigned short int portNum, char *DestinationAddress, int len);
int	WriteUART_Cache(unsigned short int portNum, char *SourceAddress, int len);

//内部接口
int WriteDataToUART_Rx_Cache(unsigned short int port_num, ST_UARTState *pt_uart);
int ReadDataFromUART_Tx_Cache(unsigned short int port_num, ST_UARTState *pt_uart);

//用户事件入口
//typedef void (*OnUARTDataReceived)(void);
#ifdef	USE_TIMER_PACKET
void uart0proc_timerproc(void);
void uart2proc_timerproc(void);
void uart3proc_timerproc(void);
#endif

#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
