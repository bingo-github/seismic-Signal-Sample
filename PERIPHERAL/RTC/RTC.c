/******************************************************************************
 * @file     	RTC.c
 * @brief    	Drivers for RTC peripheral in lpc17xx.
 * @version  	1.1
 * @date     	19. May. 2016
 * @note		 	None
 ******************************************************************************/
 
#include "RTC.h"
#include "LPC17xx.h"
#include "TriggerFSM.h"

//public variable
rtcTime theTime={0,0,0,1,1,1,1,2016};				//RTCʱ����������岢��ʼ��
uint8_t timeSetTaskEffective = 0;
volatile int timeMsCounter = 0;
uint8_t ppsAlready = 0;

/**
  * @brief  set RTC time.
  * @param  setTime: 	RTC time
  * @retval None 
  */
void RTC_SetTime(rtcTime setTime)
{
	LPC_RTC->SEC = setTime.RTC_Sec;
	LPC_RTC->MIN = setTime.RTC_Min;
	LPC_RTC->HOUR = setTime.RTC_Hour;
	LPC_RTC->DOM = setTime.RTC_Dom;
	LPC_RTC->DOW = setTime.RTC_Dow;
	LPC_RTC->DOY = setTime.RTC_Doy;
	LPC_RTC->MONTH = setTime.RTC_Month;
	LPC_RTC->YEAR = setTime.RTC_Year;
}

/**
  * @brief  set RTC alarm time.
  * @param  alarmTime: 	RTC alarm time
  * @retval None 
  */
void RTC_SetAlarmTime(rtcTime alarmTime)
{
	LPC_RTC->ALSEC = alarmTime.RTC_Sec;
	LPC_RTC->ALMIN = alarmTime.RTC_Min;
	LPC_RTC->ALHOUR = alarmTime.RTC_Hour;
	LPC_RTC->ALDOM = alarmTime.RTC_Dom;
	LPC_RTC->ALDOW = alarmTime.RTC_Dow;
	LPC_RTC->ALDOY = alarmTime.RTC_Doy;
	LPC_RTC->ALMON = alarmTime.RTC_Month;
	LPC_RTC->ALYEAR = alarmTime.RTC_Year;
}

/**
  * @brief  Initialize the RTC.
  * @param  CIIRbit: 		set bit of CIIR register
	*					setTime:		RTC time
	*					AMRbit:			set bit of AMR register
	*					alarmTime:	RTC alarm time
  * @retval None 
  */
void RTC_Init(uint8_t CIIRbit, rtcTime setTime, uint8_t AMRbit, rtcTime alarmTime)
{
	LPC_SC->PCONP |= (0x01 << 9);  					//����RTCʱ�ӿ���
	if(LPC_RTC->RTC_AUX & (0X01 << 4))     	//��RTC��ֹͣ��������жϣ�����RTC�ж�
	{
		LPC_RTC->RTC_AUX |= (0X01 << 4);
	}
	LPC_RTC->CIIR = CIIRbit;   							//���ü��������ж�
	RTC_SetTime(setTime);
	LPC_RTC->AMR = AMRbit;    							//���α���
	RTC_SetAlarmTime(alarmTime);
	LPC_RTC->CCR = 0;     									//����У׼������1HZ����ֹͣ��λ��׼����������ʼ����
}

/**
  * @brief  Reset RTC counter.
  * @param  None
  * @retval None 
  */
void RTC_Reset(void)
{
	LPC_RTC->CCR |= (0X01 << 1);   //ʹRTCʱ�ӽ��ļ�������λ
}

/**
  * @brief  Start RTC.
  * @param  None
  * @retval None 
  */
void RTC_Start(void)
{
	LPC_RTC->CCR = 1;  //����RTC����
	LPC_RTC->ILR = 0X01;   //��������������жϺͱ����ж�
}

/**
  * @brief  Stop RTC.
  * @param  None
  * @retval None 
  */
void RTC_Stop(void)
{
	LPC_RTC->CCR &= ~(0X01);  //ֹͣRTC����
}

/**
  * @brief  Get RTC time.
  * @param  None
  * @retval None 
  */
rtcTime RTC_GetTime(void)
{
	rtcTime getTime;
	getTime.RTC_Sec = LPC_RTC->SEC;
	getTime.RTC_Min = LPC_RTC->MIN;
	getTime.RTC_Hour = LPC_RTC->HOUR;
	getTime.RTC_Dom = LPC_RTC->DOM;
	getTime.RTC_Dow = LPC_RTC->DOW;
	getTime.RTC_Doy = LPC_RTC->DOY;
	getTime.RTC_Month = LPC_RTC->MONTH;
	getTime.RTC_Year = LPC_RTC->YEAR;
	return getTime;
}

/**
  * @brief  Set alarm amr.
  * @param  alarmAmr:		
  * @retval None 
  */
void RTC_SetAlarmAmr(uint8_t AMRbit)
{
	LPC_RTC->AMR = AMRbit;
}

/**
  * @brief  Interrupt of RTC.
  * @param  None
  * @retval None 
  */
void RTC_IRQHandler(void)
{
	LPC_RTC->ILR |= 0X03;   
	/***************RTC�жϺ���**************/
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		if(ppsAlready != 1)
		{
			timeMsCounter = 0;														//�µ�һ�룬������ʱ�������������
			sampleNum = 0;																//������
			LPC_TIM0->TC = tc0Reset;											//PPS�ź�ÿ���ӿ�ʼʱ����TC�����ã�ʹ֮������ʱ����ӳٶ��������
		}
		else
		{
			ppsAlready = 0;																//��PPS�Ѿ������ı�־λ����
		}
		if(startAD_flag == 1)													//�ж��Ƿ�ʼ����AD����
		{
			ADS1256_startSample();											//��ʼ����ADS1256�����в���
			startAD_flag = 0;
		}
		
		theTime.RTC_Year = LPC_RTC->YEAR % 100;				//��RTCʱ�ӵ�ʱ�����theTime������
		theTime.RTC_Month = LPC_RTC->MONTH;
		theTime.RTC_Dom = LPC_RTC->DOM;
		theTime.RTC_Hour = LPC_RTC->HOUR;
		theTime.RTC_Min = LPC_RTC->MIN;
		theTime.RTC_Sec = LPC_RTC->SEC;
		
		if(Sys_Status.statusid == SYS_SAMPLERDY_STATUS)		//PPS�����ź���ϵͳ��׼������״̬�������״̬�Ĵ�������
		{
			Sys_Status.statusid = SYS_SAMPLING_STATUS;
		}
	#endif
	/****************************************/
}
/********************************* End Of File ********************************/
