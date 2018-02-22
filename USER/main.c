/******************************************************************************
 * @file     	main.c
 * @brief    	main file.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/
#include "lpc17xx.h"
#include "RTC.h"
#include "EINT.h"
#include "SPI.h"
#include "ADS1256.h"
#include "TriggerFSM.h"

//串口环形缓冲区的参数定义
char uart0_rec_cbuffer[800];
char uart0_tra_cbuffer[800];
ST_CircleBuff pt_uart0_rec_cbuffer;
ST_CircleBuff pt_uart0_tra_cbuffer;
int RceSerialLength;								//串口接收到的数据长度
char RceSerialBuffer[200];					//串口数据接收数组
char gpsBuffer[30];									//串口接收到GPS时间信息的接收数组
char paraBuffer[30];								//串口接收到的参数设置命令的接收数组
char gpsEffective, paraEffective;		//GPS和参数设置命令的有效标志位

/*
 *系统节拍中断
 *uint32_t SystemCoreClock = 100000000;	//系统时钟
 */
void SysTick_Handler (void) 
{
	//1ms调用
	#ifdef	USE_TIMER_PACKET
	uart0proc_timerproc();
	uart2proc_timerproc();
	uart3proc_timerproc();
	#endif
}

int main(void)
{	
	Event a_receive_event;		//定义接受事件的变量
	SystemInit();
	
	//初始化赋值包头包尾和表示有效数据长度的值
	AdSample_Buffer[0]=0x10;				//包头
	AdSample_Buffer[1]=0x01;
	AdSample_Buffer[2]=0x02;				//有效数据长度
	AdSample_Buffer[3]=0x62;
	AdSample_Buffer[612]=0x10;			//包尾
	AdSample_Buffer[613]=0x02;
	//初始化应答数据包
	rePara_Buffer[0]=0x10;
	rePara_Buffer[1]=0x01;
	rePara_Buffer[2]=0x00;
	rePara_Buffer[3]=0x09;
	rePara_Buffer[4]=0x80;
	rePara_Buffer[5]=0x01;
	rePara_Buffer[13]=0x10;
	rePara_Buffer[14]=0x02;
	
	//串口环形缓冲区初始化
	UARTBuffer_Init(0,&pt_uart0_rec_cbuffer,&pt_uart0_tra_cbuffer,uart0_rec_cbuffer,uart0_tra_cbuffer,700,700);
	UARTInit(0,115200);
	SysTick_Config(SystemCoreClock/1000 - 1);
	
	Init_EINT(EINT1, EINT_RISE);						//与PPS脉冲相连，捕获PPS上升沿
	
	RTC_Init(RTC_CIIR_IMSEC, theTime, RTC_AMR_ALL, theTime);//RTC时钟初始化，秒增量中断开启，关闭所有报警位
	RTC_Start();								//RTC开始
	NVIC_EnableIRQ(RTC_IRQn);		//开启中断
	
	TriggerQueue_Init(&sample_EventQueue);				//触发事件队列初始化
	
	TriggerQueue_Init(&ads1256_EventQueue);				//触发事件队列初始化
	ADS1256_Init(ADS1256_MUXP_AIN0|ADS1256_MUXN_AIN1, ADS1256_GAIN_1, SAM_RATE_250SPS);//ADS1256初始化
	tc0Reset = CLK_TIMER0/SAM_RATE_250SPS/2;						//用于保存每新一秒来临时，TC0所应该赋值的数，用于使采样频率时间的校准
	while(1)
	{		
		//CheckSerialReceive(&sample_EventQueue);
		if(g_UART[0].IsExistPacket == 1)
		{
			RceSerialLength = ReadUART_Cache(0,RceSerialBuffer,g_UART[0].PacketLen);
			g_UART[0].IsExistPacket = 0;
			do_task_on_ReadEffectiveBuffer(RceSerialBuffer, RceSerialLength, gpsBuffer, &gpsEffective, paraBuffer, &paraEffective);
			if(gpsEffective == 1)
			{
				gpsEffective = 0;
				a_Event.flag = UART_TIME_SET_EVENT;											//记下此事件为时间设置命令
				EventQueue_insert(&sample_EventQueue, &a_Event);				//将该事件插入队伍
			}
			if(paraEffective == 1)
			{
				paraEffective = 0;
				if(paraBuffer[0] == 0x03)
				{
					//判断为串口接收到的采样参数设置命令事件
					a_Event.flag = UART_SAM_PARA_SET_EVENT;										//记下此时间为采样参数设置命令
					EventQueue_insert(&sample_EventQueue, &a_Event);				//将该事件插入队伍
				}
				else if(paraBuffer[0] == 0x02)
				{
					//判断为串口接收到的开始采样命令事件
					a_Event.flag = UART_SAM_START_EVENT;											//记下此时间为开始采样
					EventQueue_insert(&sample_EventQueue, &a_Event);				//将该事件插入队伍
				}
				else if(paraBuffer[0] == 0x01)
				{
					//判断为串口接收到的停止采样命令事件
					a_Event.flag = UART_SAM_STOP_EVENT;											//记下此时间为停止采样
					EventQueue_insert(&sample_EventQueue, &a_Event);				//将该事件插入队伍
				}
			}
		}
		
		//检测SPI接收缓冲区中是否有一个采样点的完整数据
		CheckOnePointFinish(&sample_EventQueue);
		
		//检测每个数据包里的采样点是否已经采集满
		CheckSamPointEnough(&sample_EventQueue);
		
		//处理系统过程产生的事件
		while(EventQueue_get(&sample_EventQueue, &a_receive_event) == 1)					//判断队列里是否有未完成事件，并取出尾事件
			dispatch_sys_event(&Sys_Status, &a_receive_event);												//处理队列里取出的事件
		
		//处理ADS1256采样过程中产生的事件
		while(EventQueue_get(&ads1256_EventQueue, &a_receive_event) == 1)
			dispatch_ads1256_event(&ads1256_Status, &a_receive_event);												//处理队列里取出的事件
		
	}
}
