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

//ϵͳ�¼�
#define UART_TIME_SET_EVENT						0x01				//���ڽ��յ���ʱ�����������¼�
#define	UART_SAM_START_EVENT					0x02				//���ڽ��ܵ��Ŀ�ʼ���������¼�
#define	UART_SAM_STOP_EVENT						0x03				//���ڽ��ܵ���ֹͣ���������¼�
#define	UART_SAM_PARA_SET_EVENT				0x04				//���ڽ��ܵ������ò������������¼�
#define UART_INAUIRY_EVENT						0x05
#define SAM_POINT_ENOUGH_EVENT				0x06				//���ݰ�������
#define SAM_ONE_POINT_FINISH_EVENT		0x07				//һ�������������ռ����
#define SAM_FIRST_POINT_EVENT					0x08				//�������ݰ��ĵ�һ�������㣬��Ҫ��ʱ����Ϣ
//ADS1256�����¼�
#define ADS1256_SAM_TIMEUP_EVENT 			0x09				//��ʱ��0�Ŀ�ʼ����ʱ�䵽
#define ADS1256_TRA_FINISH_EVENT			0x0A				//��������

extern EventQueue	sample_EventQueue; 	//ϵͳ�����¼�����
extern EventQueue  ads1256_EventQueue; //ADS1256���������¼����¼�����
extern Event a_Event;        					//�������¼�

/**
  * @brief  Initiate trigger queue.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void TriggerQueue_Init(EventQueue* prt_queue);

/**
  * @brief  ���һ�����ݰ��������Ƿ�ɼ����.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckSamPointEnough(EventQueue* prt_queue);

/**
  * @brief  ����Ƿ��Ѿ�������һ��������������.
  * @param  prt_queue:	event queue
  * @retval None 
  */
void CheckOnePointFinish(EventQueue* prt_queue);

#endif

/********************************* End Of File ********************************/



