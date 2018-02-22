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
			LPC_SC->EXTMODE  |= (0x01 << 0);                 /* �ⲿ�ж�0ѡ��Ϊ���ش���      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 0);	             /* �½��ش����ж�               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 0);	             /* �����ش����ж�               */
			}
			LPC_SC->EXTINT    = (0x01 << 0);                 /* ����ⲿ�ж�0���жϱ�־      */
			NVIC_EnableIRQ(EINT0_IRQn);
			break;
		case EINT1:
			LPC_PINCON->PINSEL4 |= (0x01 << 22);
			LPC_SC->EXTMODE  |= (0x01 << 1);                /* �ⲿ�ж�1ѡ��Ϊ���ش���      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 1);	             /* �½��ش����ж�               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 1);	             /* �����ش����ж�               */
			}
			LPC_SC->EXTINT    = (0x01 << 1);                 /* ����ⲿ�ж�1���жϱ�־      */
			NVIC_EnableIRQ(EINT1_IRQn);
			break;
		case EINT2:
			LPC_PINCON->PINSEL4 |= (0x01 << 24);
			LPC_SC->EXTMODE  |= (0x01 << 2);                /* �ⲿ�ж�2ѡ��Ϊ���ش���      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 2);	            /* �½��ش����ж�               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 2);	            /* �����ش����ж�               */
			}
			LPC_SC->EXTINT    = (0x01 << 2);                /* ����ⲿ�ж�2���жϱ�־      */
			NVIC_EnableIRQ(EINT2_IRQn);
			break;
		case EINT3:
			LPC_PINCON->PINSEL4 |= (0x01 << 26);
			LPC_SC->EXTMODE  |= (0x01 << 3);                /* �ⲿ�ж�3ѡ��Ϊ���ش���      */
			if(extPolar == EINT_FALL)
			{
				LPC_SC->EXTPOLAR &= ~(0x01 << 3);	            /* �½��ش����ж�               */
			}
			else
			{
				LPC_SC->EXTPOLAR |= (0x01 << 3);	           /* �����ش����ж�               */
			}
			LPC_SC->EXTINT    = (0x01 << 3);                /* ����ⲿ�ж�3���жϱ�־      */
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
void EINT0_IRQHandler (void)												//P2.10��ISP�йأ������øùܽŵ��ⲿ�ж�
{
  LPC_SC->EXTINT = (0x01 << 0);                     /* ����ⲿ�жϱ�־             */ 
  /*******************�жϺ���*********************/
	
	/*******************�жϺ���*********************/
}

/**
  * @brief  Interrupt of EINT1.
  *
  * @param  None
  * @retval None 
  */
void EINT1_IRQHandler (void)
{
  LPC_SC->EXTINT = (0x01 << 1);                    /* ����ⲿ�жϱ�־             */ 
  /*******************�жϺ���*********************/
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		timeMsCounter = 0;
		ppsAlready = 1;
		sampleNum = 0;
		LPC_TIM0->TC = tc0Reset;											//PPS�ź�ÿ���ӿ�ʼʱ����TC�����ã�ʹ֮������ʱ����ӳٶ��������,����ʱ��Ϊdt/2,tc0Reset = CLK_TIMER0/sampleFreq_50SPS/2
		if(startAD_flag == 1)													//�ж���һ�뿪ʼ������־�Ƿ�����
		{
			ADS1256_startSample();											//��ʼ����ADS1256�����в���
			RTC_Reset();																//
			RTC_Start();																	//��ʼRTCʱ�ӣ������RTC�ж�
			startAD_flag = 0;														//����һ�뿪ʼ������־λ����
		}
		
		if(timeSetTaskEffective == 1)
		{
			theTime.RTC_Year = DateTemp_Buffer[0];		//��	//�������յ���GPSʱ���źű�����DateMidTemp_Buffer��
			theTime.RTC_Month = DateTemp_Buffer[1];		//��
			theTime.RTC_Dom = DateTemp_Buffer[2];			//��
			theTime.RTC_Hour = DateTemp_Buffer[3];		//ʱ
			theTime.RTC_Min = DateTemp_Buffer[4];			//��
			theTime.RTC_Sec = DateTemp_Buffer[5];			//��
			RTC_SetTime(theTime);													//����RTCʱ��
			RTC_Reset();
			RTC_Start();																	//��ʼRTCʱ�ӣ������RTC�ж�
			timeSetTaskEffective = 0;
		}
		
		if(Sys_Status.statusid == SYS_SAMPLERDY_STATUS)		//PPS�����ź���ϵͳ��׼������״̬�������״̬�Ĵ�������
		{
			Sys_Status.statusid = SYS_SAMPLING_STATUS;
		}
	#endif	
	/*******************�жϺ���*********************/
}

/**
  * @brief  Interrupt of EINT2.
  *
  * @param  None
  * @retval None 
  */
void EINT2_IRQHandler (void)
{
  LPC_SC->EXTINT = (0x01 << 2);                                              /* ����ⲿ�жϱ�־*/ 
  /*******************�жϺ���*********************/
	
	/*******************�жϺ���*********************/
}

/**
  * @brief  Interrupt of EINT3.
  *
  * @param  None
  * @retval None 
  */
void EINT3_IRQHandler (void)
{
  LPC_SC->EXTINT = (0x01 << 3);                                              /* ����ⲿ�жϱ�־             */ 
  /*******************�жϺ���*********************/
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		if((LPC_GPIOINT->IntStatus & 0x01) == 0x01)
		{
			if((LPC_GPIOINT->IO0IntStatF & (1 << 19)) == (1 << 19))
			{
				LPC_GPIOINT->IO0IntClr |= (1 << 19);															//���жϱ�־λ
				
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
	/*******************�жϺ���*********************/
}

/********************************* End Of File ********************************/
