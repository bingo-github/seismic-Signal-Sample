/******************************************************************************
 * @file     	TIMER.h
 * @brief    	Drivers for EINT peripheral in lpc17xx.
 * @version  	1.1
 * @date     	20. May. 2016
 * @note		 	None
 ******************************************************************************/

#ifndef __TIMER_H 
#define __TIMER_H
#include "lpc17xx.h"

#define CLK_TIMER0	25000000		//��ʱ��0��ʱ��Ƶ��Ϊ25000000

/**
  * @brief  Initialize of timer match.
  * @param  timer_num:		port of timer
	*					mr_num:				number of match register
  * @retval ����1��				��ʼ���ɹ�
	*							0:				δ�ɹ�
	* @note		1��Ҫ�����õ�MR0ֵ��TCֵƥ��ʱ�����жϣ����û�Ӧ��ȥ��Ӧ��ʱ�����жϺ�����������Ӧ�Ĵ���
	*					2���û�Ҳ����ͨ�����üĴ���MCR��ֵ�������Ƿ�Ҫ�����ж�
	*					3��Ҫ���û������˵�MR0ƥ���жϣ������жϺ�����Ҫ�����Ӧ���ж� ���жϼĴ����е�һλ��Ϊ1;		
  */
 int Init_MR_Timer(char timer_num, uint32_t mr_num);

/**
  * @brief  Initialize of MAT Timer.
  * @param  timer_num:		port of timer
	*					mr_num:				number of match register
	*					interrupt_state:		1��ƥ����жϣ�0��ƥ�䲻���ж�
  * @retval ����1��				��ʼ���ɹ�
	*							0:				δ�ɹ�
	*	@note		1��Ҫ�����õ�MR0ֵ��TCֵƥ��ʱ�����жϣ����û�Ӧ��ȥ��Ӧ��ʱ�����жϺ�����������Ӧ�Ĵ���
	*					2���û�Ҳ����ͨ����ֵinterrupt_state��ֵ�������Ƿ�Ҫ�����ж�
	*					3���ڵ��øú���ǰҪѡ����Ӧ�Ĺ��ܿ��߲������ó�MAT���ܣ���ӦMAT0.0 (Timer0 �ϵ�0ͨ��)
	*							�����ⲿƥ��Ĵ���EMR��MR0��MCR�Լ�MR0��Ӧ���¼����������жϡ���λ����ֹͣ��ʱ����
  */
int Init_MAT_Timer ( int timer_num, int mr_num,int interrupt_state);

/**
  * @brief  Initialize of CAP Timer(��ʱ������������).
  * @param  timer_num:		port of timer
	*					interrupt_state:		1��ƥ����жϣ�0��ƥ�䲻���ж�
  * @retval ����1��				��ʼ���ɹ�
	*							0:				δ�ɹ�
	* @note		1��Ҫ�����õ������ػ����½��ز���ʱ�����жϣ����û�Ӧ��ȥ��Ӧ��ʱ�����жϺ�����������Ӧ�Ĵ���
	*					2���û�Ҳ����ͨ����ֵinterrupt_state��ֵ����������ʱ�Ƿ�Ҫ�����ж�
	*					3���ڵ��øú���ǰҪѡ����Ӧ�Ĺ��ܿ��߲������ó�CAP�����ܣ�����ӦCAP0.0 (Timer0 �ϵ�0ͨ��)
	*					4��Ҫ���û������˵�����ʱ�����жϣ������жϺ�����Ҫ�����Ӧ���ж� ���жϼĴ����е�һλ��Ϊ4;		
  */
int Init_CAP_Timer ( int timer_num,int interrupt_state);

/**
  * @brief  Enable Timer.
  * @param  timer_num: 0,1,2,3
  * @retval None 
  */
void Enable_Timer( char timer_num );

/**
  * @brief  Disable Timer.
  * @param  timer_num: 0,1,2,3
  * @retval None 
  */
void Disable_Timer( char timer_num );

#endif

/********************************* End Of File ********************************/
