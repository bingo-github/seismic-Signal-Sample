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

//���ڻ��λ������Ĳ�������
char uart0_rec_cbuffer[800];
char uart0_tra_cbuffer[800];
ST_CircleBuff pt_uart0_rec_cbuffer;
ST_CircleBuff pt_uart0_tra_cbuffer;
int RceSerialLength;								//���ڽ��յ������ݳ���
char RceSerialBuffer[200];					//�������ݽ�������
char gpsBuffer[30];									//���ڽ��յ�GPSʱ����Ϣ�Ľ�������
char paraBuffer[30];								//���ڽ��յ��Ĳ�����������Ľ�������
char gpsEffective, paraEffective;		//GPS�Ͳ��������������Ч��־λ

/*
 *ϵͳ�����ж�
 *uint32_t SystemCoreClock = 100000000;	//ϵͳʱ��
 */
void SysTick_Handler (void) 
{
	//1ms����
	#ifdef	USE_TIMER_PACKET
	uart0proc_timerproc();
	uart2proc_timerproc();
	uart3proc_timerproc();
	#endif
}

int main(void)
{	
	Event a_receive_event;		//��������¼��ı���
	SystemInit();
	
	//��ʼ����ֵ��ͷ��β�ͱ�ʾ��Ч���ݳ��ȵ�ֵ
	AdSample_Buffer[0]=0x10;				//��ͷ
	AdSample_Buffer[1]=0x01;
	AdSample_Buffer[2]=0x02;				//��Ч���ݳ���
	AdSample_Buffer[3]=0x62;
	AdSample_Buffer[612]=0x10;			//��β
	AdSample_Buffer[613]=0x02;
	//��ʼ��Ӧ�����ݰ�
	rePara_Buffer[0]=0x10;
	rePara_Buffer[1]=0x01;
	rePara_Buffer[2]=0x00;
	rePara_Buffer[3]=0x09;
	rePara_Buffer[4]=0x80;
	rePara_Buffer[5]=0x01;
	rePara_Buffer[13]=0x10;
	rePara_Buffer[14]=0x02;
	
	//���ڻ��λ�������ʼ��
	UARTBuffer_Init(0,&pt_uart0_rec_cbuffer,&pt_uart0_tra_cbuffer,uart0_rec_cbuffer,uart0_tra_cbuffer,700,700);
	UARTInit(0,115200);
	SysTick_Config(SystemCoreClock/1000 - 1);
	
	Init_EINT(EINT1, EINT_RISE);						//��PPS��������������PPS������
	
	RTC_Init(RTC_CIIR_IMSEC, theTime, RTC_AMR_ALL, theTime);//RTCʱ�ӳ�ʼ�����������жϿ������ر����б���λ
	RTC_Start();								//RTC��ʼ
	NVIC_EnableIRQ(RTC_IRQn);		//�����ж�
	
	TriggerQueue_Init(&sample_EventQueue);				//�����¼����г�ʼ��
	
	TriggerQueue_Init(&ads1256_EventQueue);				//�����¼����г�ʼ��
	ADS1256_Init(ADS1256_MUXP_AIN0|ADS1256_MUXN_AIN1, ADS1256_GAIN_1, SAM_RATE_250SPS);//ADS1256��ʼ��
	tc0Reset = CLK_TIMER0/SAM_RATE_250SPS/2;						//���ڱ���ÿ��һ������ʱ��TC0��Ӧ�ø�ֵ����������ʹ����Ƶ��ʱ���У׼
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
				a_Event.flag = UART_TIME_SET_EVENT;											//���´��¼�Ϊʱ����������
				EventQueue_insert(&sample_EventQueue, &a_Event);				//�����¼��������
			}
			if(paraEffective == 1)
			{
				paraEffective = 0;
				if(paraBuffer[0] == 0x03)
				{
					//�ж�Ϊ���ڽ��յ��Ĳ����������������¼�
					a_Event.flag = UART_SAM_PARA_SET_EVENT;										//���´�ʱ��Ϊ����������������
					EventQueue_insert(&sample_EventQueue, &a_Event);				//�����¼��������
				}
				else if(paraBuffer[0] == 0x02)
				{
					//�ж�Ϊ���ڽ��յ��Ŀ�ʼ���������¼�
					a_Event.flag = UART_SAM_START_EVENT;											//���´�ʱ��Ϊ��ʼ����
					EventQueue_insert(&sample_EventQueue, &a_Event);				//�����¼��������
				}
				else if(paraBuffer[0] == 0x01)
				{
					//�ж�Ϊ���ڽ��յ���ֹͣ���������¼�
					a_Event.flag = UART_SAM_STOP_EVENT;											//���´�ʱ��Ϊֹͣ����
					EventQueue_insert(&sample_EventQueue, &a_Event);				//�����¼��������
				}
			}
		}
		
		//���SPI���ջ��������Ƿ���һ�����������������
		CheckOnePointFinish(&sample_EventQueue);
		
		//���ÿ�����ݰ���Ĳ������Ƿ��Ѿ��ɼ���
		CheckSamPointEnough(&sample_EventQueue);
		
		//����ϵͳ���̲������¼�
		while(EventQueue_get(&sample_EventQueue, &a_receive_event) == 1)					//�ж϶������Ƿ���δ����¼�����ȡ��β�¼�
			dispatch_sys_event(&Sys_Status, &a_receive_event);												//���������ȡ�����¼�
		
		//����ADS1256���������в������¼�
		while(EventQueue_get(&ads1256_EventQueue, &a_receive_event) == 1)
			dispatch_ads1256_event(&ads1256_Status, &a_receive_event);												//���������ȡ�����¼�
		
	}
}
