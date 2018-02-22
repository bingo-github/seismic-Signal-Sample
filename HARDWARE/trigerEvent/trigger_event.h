/******************************************************************************
 * @file     	trigger_event.h
 * @brief    	trigger event.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/

#ifndef __TRIGGER_EVENT_H
#define	__TRIGGER_EVNET_H
#include "eventqueue.h"
#include "ADS1256.h"
#include "TriggerFSM.h"

//系统事件
#define UART_TIME_SET_EVENT						0x01				//串口接收到的时间设置命令事件
#define	UART_SAM_START_EVENT					0x02				//串口接受到的开始采样命令事件
#define	UART_SAM_STOP_EVENT						0x03				//串口接受到的停止采样命令事件
#define	UART_SAM_PARA_SET_EVENT				0x04				//串口接受到的设置采样参数命令事件
#define UART_INAUIRY_EVENT						0x05
#define SAM_POINT_ENOUGH_EVENT				0x06				//数据包采样满
#define SAM_ONE_POINT_FINISH_EVENT		0x07				//一个采样点数据收集完成
#define SAM_FIRST_POINT_EVENT					0x08				//采样数据包的第一个采样点，需要加时间信息
//ADS1256采样事件
#define ADS1256_SAM_TIMEUP_EVENT 			0x09				//定时器0的开始采样时间到
#define ADS1256_TRA_FINISH_EVENT			0x0A				//命令发送完成

extern EventQueue	sample_EventQueue; 	//系统采样事件队列
extern EventQueue  ads1256_EventQueue; //ADS1256采样过程事件的事件队列
extern Event a_Event;        					//采样的事件

/**
  * @brief  Initiate trigger queue.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void TriggerQueue_Init(EventQueue* prt_queue);

/**
  * @brief  检查一个数据包的数据是否采集完成.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckSamPointEnough(EventQueue* prt_queue);

/**
  * @brief  检查是否已经至少有一个采样点采样完成.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckOnePointFinish(EventQueue* prt_queue);

#endif

/********************************* End Of File ********************************/



