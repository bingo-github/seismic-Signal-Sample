/******************************************************************************
 * @file     	TriggerFSM.h
 * @brief    	trigger Status Machine.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/
 
#ifndef	__TRIGGERFSM_H
#define	__TRIGGERFSM_H
#include "eventqueue.h"
#include "trigger_event.h"
#include "uart.h"
#include "RTC.h"

/*                           ״̬����                            */
//ϵͳ״̬
#define	SYS_IDLE_STATUS				0x01  //ϵͳ����״̬
#define SYS_SAMPLERDY_STATUS	0x02	//ϵͳ׼������״̬
#define SYS_SAMPLING_STATUS		0x03	//ϵͳ���ڲ���״̬����״̬�ǽ���ADS1256����״̬��ǰ��
//ADS1256����״̬
#define ADS1256_IDLE_STATUS 						0x04  //ADS1256��������״̬
#define ADS1256_CHANGING_CH0_STATUS			0x05	//ADS1256���ڽ���ͨ��0ת����״̬
#define ADS1256_CHANGED_CH0_STATUS			0x06	//ADS1256ͨ��0ת�����״̬
#define ADS1256_SAMPLING_CH0_STATUS			0x07	//ADS1256���ڽ���ͨ��0���ݲ�����״̬
#define ADS1256_SAMPLED_CH0_STATUS			0x08	//ADS1256ͨ��0�������״̬
#define ADS1256_CHANGING_CH1_STATUS			0x09	//ADS1256���ڽ���ͨ��1ת����״̬
#define ADS1256_CHANGED_CH1_STATUS			0x0A	//ADS1256ͨ��1ת�����״̬
#define ADS1256_SAMPLING_CH1_STATUS			0x0B	//ADS1256���ڽ���ͨ��1���ݲ�����״̬
#define ADS1256_SAMPLED_CH1_STATUS			0x0C	//ADS1256ͨ��1�������״̬
#define ADS1256_CHANGING_CH2_STATUS			0x0D	//ADS1256���ڽ���ͨ��2ת����״̬
#define ADS1256_CHANGED_CH2_STATUS			0x0E	//ADS1256ͨ��2ת�����״̬
#define ADS1256_SAMPLING_CH2_STATUS			0x0F	//ADS1256���ڽ���ͨ��2���ݲ�����״̬
#define ADS1256_SAMPLED_CH2_STATUS			0x10	//ADS1256ͨ��2�������״̬
#define ADS1256_CHANGING_CH3_STATUS			0x11	//ADS1256���ڽ���ͨ��3ת����״̬
#define ADS1256_CHANGED_CH3_STATUS			0x12	//ADS1256ͨ��3ת�����״̬
#define ADS1256_SAMPLING_CH3_STATUS			0x13	//ADS1256���ڽ���ͨ��3���ݲ�����״̬

typedef	struct {
	char	statusid;
} StatusMachine;


extern uint32_t tc0Reset;
extern uint8_t startAD_flag;
extern StatusMachine ads1256_Status;  	//ADS1256����״̬�ṹ��
extern StatusMachine Sys_Status;  			//ϵͳ״̬�ṹ��
extern volatile uint16_t sampleNum;
extern char AdSample_Buffer[650];								//������������
extern char DateTemp_Buffer[15];
extern char rePara_Buffer[25];

/**
  * @brief  ����ADS1256�����¼�.
  * @param  pt_lsm��״̬��ָ��		pt_event���¼�ָ��
  * @retval None 
  */
void dispatch_ads1256_event(StatusMachine *pt_lsm, Event *pt_event);

/**
  * @brief  ����ϵͳ�����¼�.
  * @param  pt_lsm��״̬��ָ��		pt_event���¼�ָ��
  * @retval None 
  */
void dispatch_sys_event(StatusMachine *pt_lsm, Event *pt_event);

/**
  * @brief  �Ӵ��ڽ��������зֳ���Ч����.
  * @param  RecSerialBuffer��	���ڽ��յ������ݴ�ŵ������ַ
	*					RecSerialLength��	���ڽ��յ������ݳ���
	*					gpsBuffer��				�������������е�GPS��Ϣ�ĵ�ַ
	*					gpsEffective��		GPS������Ч��־λ��1Ϊ��Ч��0Ϊ��Ч
	*					paraBuffer��			�������������еĲ�������ĵ�ַ
	*					paraEffective��		����������Ч��־λ��1Ϊ��Ч��0Ϊ��Ч
  * @retval None 
  */
void do_task_on_ReadEffectiveBuffer(char *RecSerialBuffer, int RecSerialLength, char *gpsBuffer, char *gpsEffective, char *paraBuffer, char *paraEffective);

#endif

 /********************************* End Of File ********************************/
 


