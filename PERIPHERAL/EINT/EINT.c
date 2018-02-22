/******************************************************************************
 * @file     	EINT.c
 * @brief    	Drivers for EINT peripheral in lpc17xx.
 * @version  	1.1
 * @date     	19. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "EINT.h"
#include "TriggerFSM.h"


/**
  * @brief  Initialize  the EINT.
  *
  * @param  EINT_num: 	EINT port
	*					extPolar:		Polar of EINT(EINT_RISE or EINT_FALL)
  * @retval None 
  */
void Init_EINT(uint8_t EINT_num, uint8_t extPolar)
{
	switch(EINT_num)
	{
		case EINT0:
			LPC_PINCON->PINSEL4 |= (0x01 << 20);
			LPC_SC->EXTMODE  |= (0x01 << 0);                 /* 外部中断0选择为边沿触发      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 0);	             /* 下降沿触发中断               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 0);	             /* 上升沿触发中断               */
			}
			LPC_SC->EXTINT    = (0x01 << 0);                 /* 清除外部中断0的中断标志      */
			NVIC_EnableIRQ(EINT0_IRQn);
			break;
		case EINT1:
			LPC_PINCON->PINSEL4 |= (0x01 << 22);
			LPC_SC->EXTMODE  |= (0x01 << 1);                /* 外部中断1选择为边沿触发      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 1);	             /* 下降沿触发中断               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 1);	             /* 上升沿触发中断               */
			}
			LPC_SC->EXTINT    = (0x01 << 1);                 /* 清除外部中断1的中断标志      */
			NVIC_EnableIRQ(EINT1_IRQn);
			break;
		case EINT2:
			LPC_PINCON->PINSEL4 |= (0x01 << 24);
			LPC_SC->EXTMODE  |= (0x01 << 2);                /* 外部中断2选择为边沿触发      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 2);	            /* 下降沿触发中断               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 2);	            /* 上升沿触发中断               */
			}
			LPC_SC->EXTINT    = (0x01 << 2);                /* 清除外部中断2的中断标志      */
			NVIC_EnableIRQ(EINT2_IRQn);
			break;
		case EINT3:
			LPC_PINCON->PINSEL4 |= (0x01 << 26);
			LPC_SC->EXTMODE  |= (0x01 << 3);                /* 外部中断3选择为边沿触发      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 3);	            /* 下降沿触发中断               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 3);	           /* 上升沿触发中断               */
			}
			LPC_SC->EXTINT    = (0x01 << 3);                /* 清除外部中断3的中断标志      */
			NVIC_EnableIRQ(EINT3_IRQn);
			break;
		default:
			break;
	}
}

/**
  * @brief  Interrupt of EINT0.
  *
  * @param  None
  * @retval None 
  */
void EINT0_IRQHandler (void)												//P2.10与ISP有关，请慎用该管脚的外部中断
{
  LPC_SC->EXTINT = (0x01 << 0);                     /* 清除外部中断标志             */ 
  /*******************中断函数*********************/
	
	/*******************中断函数*********************/
}

/**
  * @brief  Interrupt of EINT1.
  *
  * @param  None
  * @retval None 
  */
void EINT1_IRQHandler (void)
{
  LPC_SC->EXTINT = (0x01 << 1);                    /* 清除外部中断标志             */ 
  /*******************中断函数*********************/
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		timeMsCounter = 0;
		ppsAlready = 1;
		sampleNum = 0;
		LPC_TIM0->TC = tc0Reset;											//PPS信号每秒钟开始时进行TC的重置，使之不会随时间的延迟而产生误差,初试时间为dt/2,tc0Reset = CLK_TIMER0/sampleFreq_50SPS/2
		if(startAD_flag == 1)													//判断新一秒开始采样标志是否允许
		{
			ADS1256_startSample();											//开始启动ADS1256，进行采样
			RTC_Reset();																//
			RTC_Start();																	//开始RTC时钟，并清除RTC中断
			startAD_flag = 0;														//将新一秒开始采样标志位清零
		}
		
		if(timeSetTaskEffective == 1)
		{
			theTime.RTC_Year = DateTemp_Buffer[0];		//年	//将串口收到的GPS时间信号保存在DateMidTemp_Buffer中
			theTime.RTC_Month = DateTemp_Buffer[1];		//月
			theTime.RTC_Dom = DateTemp_Buffer[2];			//日
			theTime.RTC_Hour = DateTemp_Buffer[3];		//时
			theTime.RTC_Min = DateTemp_Buffer[4];			//分
			theTime.RTC_Sec = DateTemp_Buffer[5];			//秒
			RTC_SetTime(theTime);													//更新RTC时间
			RTC_Reset();
			RTC_Start();																	//开始RTC时钟，并清除RTC中断
			timeSetTaskEffective = 0;
		}
		
		if(Sys_Status.statusid == SYS_SAMPLERDY_STATUS)		//PPS脉冲信号是系统从准备采样状态进入采样状态的触发条件
		{
			Sys_Status.statusid = SYS_SAMPLING_STATUS;
		}
	#endif	
	/*******************中断函数*********************/
}

/**
  * @brief  Interrupt of EINT2.
  *
  * @param  None
  * @retval None 
  */
void EINT2_IRQHandler (void)
{
  LPC_SC->EXTINT = (0x01 << 2);                                              /* 清除外部中断标志*/ 
  /*******************中断函数*********************/
	
	/*******************中断函数*********************/
}

/**
  * @brief  Interrupt of EINT3.
  *
  * @param  None
  * @retval None 
  */
void EINT3_IRQHandler (void)
{
  LPC_SC->EXTINT = (0x01 << 3);                                              /* 清除外部中断标志             */ 
  /*******************中断函数*********************/
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		if((LPC_GPIOINT->IntStatus & 0x01) == 0x01)
		{
			if((LPC_GPIOINT->IO0IntStatF & (1 << 19)) == (1 << 19))
			{
				LPC_GPIOINT->IO0IntClr |= (1 << 19);															//清中断标志位
				
				switch(ads1256_Status.statusid)
				{
					case ADS1256_CHANGED_CH0_STATUS:
						ads1256_Status.statusid = ADS1256_SAMPLING_CH0_STATUS;
						ADS1256_sampleOneCH();
						break;
					case ADS1256_SAMPLED_CH0_STATUS:
						ads1256_Status.statusid = ADS1256_CHANGING_CH1_STATUS;
						ADS1256_changeCH(ADS1256_MUXP_AIN2|ADS1256_MUXN_AIN3);
						break;
					case ADS1256_CHANGED_CH1_STATUS:
						ads1256_Status.statusid = ADS1256_SAMPLING_CH1_STATUS;
						ADS1256_sampleOneCH();
						break;
					case ADS1256_SAMPLED_CH1_STATUS:
						ads1256_Status.statusid = ADS1256_CHANGING_CH2_STATUS;
						ADS1256_changeCH(ADS1256_MUXP_AIN4|ADS1256_MUXN_AIN5);
						break;
					case ADS1256_CHANGED_CH2_STATUS:
						ads1256_Status.statusid = ADS1256_SAMPLING_CH2_STATUS;
						ADS1256_sampleOneCH();
						break;
					case ADS1256_SAMPLED_CH2_STATUS:
						ads1256_Status.statusid = ADS1256_CHANGING_CH3_STATUS;
						ADS1256_changeCH(ADS1256_MUXP_AIN6|ADS1256_MUXN_AIN7);
						break;
					case ADS1256_CHANGED_CH3_STATUS:
						ads1256_Status.statusid = ADS1256_SAMPLING_CH3_STATUS;
						ADS1256_sampleOneCH();
						break;
					default:
						break;
				}
			}
		}
	#endif
	/*******************中断函数*********************/
}

/********************************* End Of File ********************************/
