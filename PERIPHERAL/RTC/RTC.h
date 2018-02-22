/******************************************************************************
 * @file     	RTC.h
 * @brief    	Drivers for RTC peripheral in lpc17xx.
 * @version  	1.1
 * @date     	19. May. 2016
 * @note		 	None
 ******************************************************************************/

#ifndef __RTC_H
#define __RTC_H
#include "lpc17xx.h"

//struct of RTC time
typedef struct 
{
	uint8_t RTC_Sec;  
	uint8_t RTC_Min;
	uint8_t RTC_Hour;
	uint8_t RTC_Dom;
	uint8_t RTC_Dow;
	uint16_t RTC_Doy;
	uint8_t RTC_Month;
	uint16_t RTC_Year;
}rtcTime;

extern rtcTime theTime;
extern uint8_t timeSetTaskEffective;
extern volatile int timeMsCounter;
extern uint8_t ppsAlready;

//CIIR bit
#define RTC_CIIR_NONE		0							//RTC时钟任意一个计数器增1，均不产生中断
#define	RTC_CIIR_IMSEC	(1 << 0)
#define	RTC_CIIR_IMMIN	(1 << 1)
#define	RTC_CIIR_IMHOUR	(1 << 2)
#define	RTC_CIIR_IMDOM	(1 <<	3)
#define	RTC_CIIR_IMDOW	(1 << 4)
#define	RTC_CIIR_IMDOY	(1 << 5)
#define	RTC_CIIR_IMMON	(1 << 6)
#define	RTC_CIIR_IMYEAR	(1 << 7)
#define RTC_CIIR_ALL		0xFF					//RTC时钟任意一个计数器增1，均产生中断

//ARM bit
#define RTC_AMR_NONE		0							//不屏蔽所有的报警寄存器
#define	RTC_AMR_AMRSEC	(1 << 0)
#define	RTC_AMR_AMRMIN	(1 << 1)
#define	RTC_AMR_AMRHOUR	(1 << 2)
#define	RTC_AMR_AMRDOM	(1 << 3)
#define	RTC_AMR_AMRDOW	(1 << 4)
#define	RTC_AMR_AMRDOY	(1 << 5)
#define	RTC_AMR_AMRMON	(1 << 6)
#define	RTC_AMR_AMRYEAR	(1 << 7)
#define RTC_AMR_ALL			0xFF					//屏蔽所有的报警寄存器

/**
  * @brief  set RTC time.
  * @param  setTime: 	RTC time
  * @retval None 
  */
void RTC_SetTime(rtcTime setTime);

/**
  * @brief  set RTC alarm time.
  * @param  alarmTime: 	RTC alarm time
  * @retval None 
  */
void RTC_SetAlarmTime(rtcTime alarmTime);

/**
  * @brief  Initialize  the EINT.
  *
  * @param  EINT_num: 	EINT port
	*					extPolar:		Polar of EINT(EINT_RISE or EINT_FALL)
  * @retval None 
  */
void RTC_Init(uint8_t CIIRbit, rtcTime setTime, uint8_t AMRbit, rtcTime alarmTime);

/**
  * @brief  Reset RTC counter.
  * @param  None
  * @retval None 
  */
void RTC_Reset(void);

/**
  * @brief  Start RTC.
  * @param  None
  * @retval None 
  */
void RTC_Start(void);

/**
  * @brief  Stop RTC.
  * @param  None
  * @retval None 
  */
void RTC_Stop(void);

/**
  * @brief  Get RTC time.
  * @param  None
  * @retval None 
  */
rtcTime RTC_GetTime(void);

/**
  * @brief  Set alarm amr.
  * @param  alarmAmr:		
  * @retval None 
  */
void RTC_SetAlarmAmr(uint8_t AMRbit);

#endif

/********************************* End Of File ********************************/

