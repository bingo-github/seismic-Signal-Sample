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
rtcTime theTime={0,0,0,1,1,1,1,2016};				//RTC时间变量，定义并初始化
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
	LPC_SC->PCONP |= (0x01 << 9);  					//启用RTC时钟控制
	if(LPC_RTC->RTC_AUX & (0X01 << 4))     	//若RTC震荡停止，则清除中断，开启RTC中断
	{
		LPC_RTC->RTC_AUX |= (0X01 << 4);
	}
	LPC_RTC->CIIR = CIIRbit;   							//设置计数增量中断
	RTC_SetTime(setTime);
	LPC_RTC->AMR = AMRbit;    							//屏蔽报警
	RTC_SetAlarmTime(alarmTime);
	LPC_RTC->CCR = 0;     									//开启校准计数（1HZ），停止复位，准备计数器开始计数
}

/**
  * @brief  Reset RTC counter.
  * @param  None
  * @retval None 
  */
void RTC_Reset(void)
{
	LPC_RTC->CCR |= (0X01 << 1);   //使RTC时钟节拍计数器复位
}

/**
  * @brief  Start RTC.
  * @param  None
  * @retval None 
  */
void RTC_Start(void)
{
	LPC_RTC->CCR = 1;  //开启RTC计数
	LPC_RTC->ILR = 0X01;   //清除计数器增量中断和报警中断
}

/**
  * @brief  Stop RTC.
  * @param  None
  * @retval None 
  */
void RTC_Stop(void)
{
	LPC_RTC->CCR &= ~(0X01);  //停止RTC计数
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
	/***************RTC中断函数**************/
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		if(ppsAlready != 1)
		{
			timeMsCounter = 0;														//新的一秒，将毫秒时间计数变量清零
			sampleNum = 0;																//采样数
			LPC_TIM0->TC = tc0Reset;											//PPS信号每秒钟开始时进行TC的重置，使之不会随时间的延迟而产生误差
		}
		else
		{
			ppsAlready = 0;																//将PPS已经发生的标志位清零
		}
		if(startAD_flag == 1)													//判断是否开始进行AD采样
		{
			ADS1256_startSample();											//开始启动ADS1256，进行采样
			startAD_flag = 0;
		}
		
		theTime.RTC_Year = LPC_RTC->YEAR % 100;				//将RTC时钟的时间存在theTime变量中
		theTime.RTC_Month = LPC_RTC->MONTH;
		theTime.RTC_Dom = LPC_RTC->DOM;
		theTime.RTC_Hour = LPC_RTC->HOUR;
		theTime.RTC_Min = LPC_RTC->MIN;
		theTime.RTC_Sec = LPC_RTC->SEC;
		
		if(Sys_Status.statusid == SYS_SAMPLERDY_STATUS)		//PPS脉冲信号是系统从准备采样状态进入采样状态的触发条件
		{
			Sys_Status.statusid = SYS_SAMPLING_STATUS;
		}
	#endif
	/****************************************/
}
/********************************* End Of File ********************************/
