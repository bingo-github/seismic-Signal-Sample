/******************************************************************************
 * @file     	trigger_event.c
 * @brief    	trigger event.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "trigger_event.h"

EventQueue	sample_EventQueue; 	//系统采样事件队列
EventQueue  ads1256_EventQueue; //ADS1256采样过程事件的事件队列
Event a_Event;        					//采样的事件

/**
  * @brief  Initiate trigger queue.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void TriggerQueue_Init(EventQueue* prt_queue)
{
	Queue_Init(prt_queue);
}

/**
  * @brief  检查是否已经至少有一个采样点采样完成.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckOnePointFinish(EventQueue* prt_queue)
{
	if(SPI_RecByteNum >= 12)
	{
		a_Event.flag = SAM_ONE_POINT_FINISH_EVENT;
		EventQueue_insert(prt_queue, &a_Event);
	}
}

/**
  * @brief  检查一个数据包的数据是否采集完成.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckSamPointEnough(EventQueue* prt_queue)
{
	if(sampleNum >= 600)
	{
		//检测到每个数据包的50个采样点已经采集完毕
		a_Event.flag = SAM_POINT_ENOUGH_EVENT;										//记下数据包采样满的事件
		EventQueue_insert(prt_queue, &a_Event);						//将该事件插进队列中
	}
}

 /********************************* End Of File ********************************/
