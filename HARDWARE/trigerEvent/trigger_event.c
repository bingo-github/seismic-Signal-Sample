/******************************************************************************
 * @file     	trigger_event.c
 * @brief    	trigger event.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "trigger_event.h"

EventQueue	sample_EventQueue; 	//ϵͳ�����¼�����
EventQueue  ads1256_EventQueue; //ADS1256���������¼����¼�����
Event a_Event;        					//�������¼�

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
  * @brief  ����Ƿ��Ѿ�������һ��������������.
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
  * @brief  ���һ�����ݰ��������Ƿ�ɼ����.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckSamPointEnough(EventQueue* prt_queue)
{
	if(sampleNum >= 600)
	{
		//��⵽ÿ�����ݰ���50���������Ѿ��ɼ����
		a_Event.flag = SAM_POINT_ENOUGH_EVENT;										//�������ݰ����������¼�
		EventQueue_insert(prt_queue, &a_Event);						//�����¼����������
	}
}

 /********************************* End Of File ********************************/
