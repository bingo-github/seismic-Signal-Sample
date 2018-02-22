/******************************************************************************
 * @file     	TIMER.c
 * @brief    	Drivers for EINT peripheral in lpc17xx.
 * @version  	1.1
 * @date     	20. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "timer.h"
#include "TriggerFSM.h"

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
 int Init_MR_Timer(char timer_num, uint32_t mr_num) 
{
	if(timer_num==0)
	{
		LPC_SC->PCONP |= (1<<1);                         //�򿪹��ʿ���    
		LPC_TIM0->TCR = 0X02;                            //��λ��ʱ��  
		LPC_TIM0->PR   = 0x00;                           //Ԥ��Ƶ����  ʹ֮ÿ��1����TC��1
		LPC_TIM0->MR0  = mr_num;                         //ƥ��Ĵ����붨ʱ��������ֵ��Ƚ� 
		LPC_TIM0->IR  = 0x3f;                            //�������жϱ�־λ  
		LPC_TIM0->MCR = 0X03;								            //MR0��TCֵƥ��ʱ�������ж�,TC��λ,Ҫ�ǲ������ж����ֵӦΪ2
		LPC_TIM0->CTCR = 0;                             //��ʱ��ģʽ                  
		NVIC_EnableIRQ(TIMER0_IRQn);  
		return 1;	
	} 
	if(timer_num==1)
	{
		LPC_SC->PCONP |= (1<<2);                         //�򿪹��ʿ���    
		LPC_TIM1->TCR = 0X02;                            //��λ��ʱ��  
		#ifdef USE_ADS1256_SEISMIC_SIGNAL
			LPC_TIM1->PR   = 0x61A8;                         //ʹ֮ÿ1msTC��1
			LPC_TIM1->MCR = 0X00;								             //MR1��TCֵƥ��ʱ���������ж�Ҳ����λ
		#else
			LPC_TIM1->PR   = 0x00;                           //Ԥ��Ƶ����  
			LPC_TIM1->MCR = 0X03;								             ////MR1��TCֵƥ��ʱ�������ж�,TC��λ
		#endif
		LPC_TIM1->MR0  = mr_num;                         //ƥ��Ĵ����붨ʱ��������ֵ��Ƚ�  
		LPC_TIM1->IR  = 0x3f;                            //�������жϱ�־λ  
		LPC_TIM1->CTCR = 0;                              //��ʱ��ģʽ                  
		LPC_TIM1->TCR  = 0x01;                          //������ʱ��  
		NVIC_EnableIRQ(TIMER1_IRQn);  
		return 1;	
	} 
	if(timer_num==2)
	{
		LPC_SC->PCONP |= (1<<22);                        //�򿪹��ʿ���    
		LPC_TIM2->TCR = 0X02;                            //��λ��ʱ��  
		LPC_TIM2->PR   = 0x00;                           //Ԥ��Ƶ����  
		LPC_TIM2->MR0  = mr_num;                         //ƥ��Ĵ����붨ʱ��������ֵ��Ƚ�  
		LPC_TIM2->IR  = 0xff;                            //�������жϱ�־λ  
		LPC_TIM2->MCR = 0X03;								             //MR0��TCֵƥ��ʱ�������ж�,TC��λ��Ҫ�ǲ������ж����ֵӦΪ2
		LPC_TIM2->CTCR = 0;                              //��ʱ��ģʽ                  
		LPC_TIM2->TCR  = 0x01;                           //������ʱ��  
		NVIC_EnableIRQ(TIMER2_IRQn);  
		return 1;	
	} 
	if(timer_num==3)
	{
		LPC_SC->PCONP |= (1<<23);                         //�򿪹��ʿ���    
		LPC_TIM3->TCR = 0X02;                             //��λ��ʱ��  
		LPC_TIM3->PR   = 0x00;                            //Ԥ��Ƶ����  
		LPC_TIM3->MR0  = mr_num;                          //ƥ��Ĵ����붨ʱ��������ֵ��Ƚ�  
		LPC_TIM3->IR  = 0xff;                             //�������жϱ�־λ  
		LPC_TIM3->MCR = 0X03;								              //MR0��TCֵƥ��ʱ�������ж�,TC��λ��Ҫ�ǲ������ж����ֵӦΪ2
		LPC_TIM3->CTCR = 0;                               //��ʱ��ģʽ                  
		LPC_TIM3->TCR  = 0x01; 
		NVIC_EnableIRQ(TIMER3_IRQn); 
		return 1;	
	} 
	return 0;
}

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
int Init_MAT_Timer ( int timer_num, int mr_num,int interrupt_state) 
{
	if ( timer_num == 0 )
	{
		LPC_SC->PCONP |= (1<<1);                         //�򿪹��ʿ���  
		LPC_TIM0->MR0 = mr_num;
		LPC_TIM0->EMR = 0x30;			                       //���õ�����ƥ��ʱ������õĹܽŵ�ƽ�Ƿ�ת���ǲ����仯
		if(interrupt_state==1)
		{
			LPC_TIM0->MCR = 0x03;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
			NVIC_EnableIRQ(TIMER0_IRQn);                    
		}
		else
			LPC_TIM0->MCR = 0x02;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
		return (1);
	}
	if ( timer_num == 1 )
	{
		LPC_SC->PCONP |= (1<<2);                        //�򿪹��ʿ���    
		LPC_TIM1->MR0 = mr_num;
		LPC_TIM1->EMR = 0x30;			                      //���õ�����ƥ��ʱ������õĹܽŵ�ƽ�Ƿ�ת���ǲ����仯
		LPC_TIM1->IR  = 0xff;                           //�������жϱ�־λ         
		if(interrupt_state==1)
		{
			LPC_TIM1->MCR = 0x03;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
			NVIC_EnableIRQ(TIMER1_IRQn);                    
		}
		else
			LPC_TIM1->MCR = 0x02;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
		return (1);
	}

	if ( timer_num == 2 )
	{

		LPC_SC->PCONP |= (1<<22);                        //�򿪹��ʿ���    
		LPC_TIM2->MR0 = mr_num;
		LPC_TIM2->EMR = 0x30;			                      //���õ�����ƥ��ʱ������õĹܽŵ�ƽ�Ƿ�ת���ǲ����仯
		LPC_TIM2->IR  = 0xff;                           //�������жϱ�־λ         
		if(interrupt_state==1)
		{
			LPC_TIM2->MCR = 0x03;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
			NVIC_EnableIRQ(TIMER1_IRQn);                    
		}
		else
			LPC_TIM2->MCR = 0x02;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
		return (1);
	}
	if ( timer_num == 3 )
	{
		LPC_SC->PCONP |= (1<<23);                        //�򿪹��ʿ���    
		LPC_TIM3->MR0 = mr_num;
		LPC_TIM3->EMR = 0x30;			                      //���õ�����ƥ��ʱ������õĹܽŵ�ƽ�Ƿ�ת���ǲ����仯
		LPC_TIM3->IR  = 0xff;                           //�������жϱ�־λ         
		if(interrupt_state==1)
		{
			LPC_TIM3->MCR = 0x03;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
			NVIC_EnableIRQ(TIMER3_IRQn);                    
		}
		else
			LPC_TIM3->MCR = 0x02;				                   //����MR0��TCֵƥ��ʱ�Ƿ�����жϣ�2��ʾ�������ж�ֻ�Ǹ�λTC��3��ʾ�����жϸ�λTC
		return (1);
	}				 
	return (0);
}

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
int Init_CAP_Timer ( int timer_num,int interrupt_state) 
{
	if ( timer_num == 0 )
	{
		LPC_PINCON->PINSEL3 |= (1<<20)|(1<<21)|(1<<22)|(1<<23);  
		LPC_SC->PCONP |= (1<<1);                         //�򿪹��ʿ���  
		LPC_TIM0->TCR = 0x03;                            //ʹ�ܶ�ʱ��  
		LPC_TIM0->PR  = 0x00;                            //Ԥ��Ƶ����  
		LPC_TIM0->IR  = 0x3f;                            //�������жϱ�־λ   
		LPC_TIM0->TCR = 0x01;
		if(interrupt_state == 1)
		{
			LPC_TIM0->CCR |= (1<<0)|(1<<2)|(1<<3)|(1<<5);  //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
			NVIC_EnableIRQ(TIMER0_IRQn);                    
		}
		else
			LPC_TIM0->CCR = 0x01;				                   //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
		return (1);
	}
	if ( timer_num == 1 )
	{
		LPC_SC->PCONP |= (1<<2);                         //�򿪹��ʿ���  
		LPC_TIM1->TCR = 0x03;                            //��λ��ʱ��  
		LPC_TIM1->PR   = 0x00;                           //Ԥ��Ƶ����  
		LPC_TIM1->IR  = 0x3f;                            //�������жϱ�־λ   
		LPC_TIM1->TCR = 0x01;
		if(interrupt_state==1)
		{
			LPC_TIM1->CCR = 0X2D;                             //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
			NVIC_EnableIRQ(TIMER1_IRQn);                    
		}
		else
			LPC_TIM1->CCR = 0x01;				                   //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
		return (1);
	}
	if ( timer_num == 2 )
	{
		LPC_SC->PCONP |= (1<<22);                         //�򿪹��ʿ���  
		LPC_TIM2->TCR = 0x03;                             //��λ��ʱ��  
		LPC_TIM2->PR   = 0x00;                            //Ԥ��Ƶ����  
		LPC_TIM2->IR  = 0x3f;                             //�������жϱ�־λ   
		LPC_TIM2->TCR = 0x01;
		if(interrupt_state==1)
		{
			LPC_TIM2->CCR = 0X2D;                                //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
			NVIC_EnableIRQ(TIMER2_IRQn);                    
		}
		else
			LPC_TIM2->CCR = 0x01;				                   //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
		return (1);
	}
	if ( timer_num == 3 )
	{
		LPC_SC->PCONP |= (1<<23);                         //�򿪹��ʿ���  
		LPC_TIM3->TCR = 0x03;                             //��λ��ʱ��  
		LPC_TIM3->PR   = 0x00;                            //Ԥ��Ƶ����  
		LPC_TIM3->IR  = 0x3f;                             //�������жϱ�־λ   
		LPC_TIM3->TCR = 0x01; 
		if(interrupt_state==1)
		{
			LPC_TIM3->CCR = 0X2D;                                //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
			NVIC_EnableIRQ(TIMER3_IRQn);                    
		}
		else
			LPC_TIM3->CCR = 0x01;				                   //���ò���ʽ�Ͳ����ʱ��Ҫ��Ҫ�����жϣ�1��ʾ�����жϣ�0��ʾ�������ж�
		return (1);
	}				 
	return (0);
}

/**
  * @brief  Enable Timer.
  * @param  timer_num: 0,1,2,3
  * @retval None 
  */
void Enable_Timer( char timer_num )
{
	if ( timer_num == 0 )
	{
		LPC_TIM0->TCR = 1;
	}
	if ( timer_num == 1 )
	{
		LPC_TIM1->TCR = 1;
	}
	if ( timer_num == 2 )
	{
		LPC_TIM2->TCR = 1;
	}
	if ( timer_num == 3 )
	{
		LPC_TIM3->TCR = 1;
	}
	return;
}

/**
  * @brief  Disable Timer.
  * @param  timer_num: 0,1,2,3
  * @retval None 
  */
void Disable_Timer( char timer_num )
{
	if ( timer_num == 0 )
	{
		LPC_TIM0->TCR = 0;
	}
	if ( timer_num == 1 )
	{
		LPC_TIM1->TCR = 0;
	}
	if ( timer_num == 2 )
	{
		LPC_TIM2->TCR = 0;
	}
	if ( timer_num == 3 )
	{
		LPC_TIM3->TCR = 0;
	}
	return;
}

/**
  * @brief  Interrupt of Timer0.
  * @param  None
  * @retval None 
  */
void TIMER0_IRQHandler(void)
{
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		char i;
	#endif
	LPC_TIM0->IR |= 0x3F;
	#ifdef USE_ADS1256_SEISMIC_SIGNAL
		timeMsCounter += LPC_TIM0->MR0/25000;
		if(sampleNum == 0)											//�ж��Ƿ�Ϊÿ�����ݰ��ĵ�һ�������㣬���ǣ���Ӹò������ʱ����Ϣ
		{
			a_Event.flag = SAM_FIRST_POINT_EVENT;
			EventQueue_insert(&sample_EventQueue, &a_Event);
		}
		//ǰ���ж���һ�β������Ѿ������������ж������Ѿ����ڿ���״̬
		if((GetFreeLengthOfCirBuff(&pt_spi_tra_cbuffer) == 0) && ads1256_Status.statusid == ADS1256_IDLE_STATUS)
		{
			a_Event.flag = ADS1256_SAM_TIMEUP_EVENT;
			EventQueue_insert(&ads1256_EventQueue, &a_Event);
		}
		else
		{
			for(i=0;i<12;i++)
			{
				AdSample_Buffer[12+sampleNum] = 0xFF;
				sampleNum++;
			}
			ads1256_Status.statusid = ADS1256_IDLE_STATUS;
		}
	#endif
	
	return;
	//���жϱ�־
}

/**
  * @brief  Interrupt of Timer1.
  * @param  None
  * @retval None 
  */
void TIMER1_IRQHandler (void)  
{  
	LPC_TIM1->IR = 1;			/* clear uint32_terrupt flag */
	return;
}

/**
  * @brief  Interrupt of Timer2.
  * @param  None
  * @retval None 
  */
void TIMER2_IRQHandler (void)  
{  
	LPC_TIM2->IR = 1;			/* clear uint32_terrupt flag */
	return;
}

/**
  * @brief  Interrupt of Timer2.
  * @param  None
  * @retval None 
  */
void TIMER3_IRQHandler (void)  
{  
	LPC_TIM3->IR = 1;			/* clear uint32_terrupt flag */
	return;
}

/********************************* End Of File ********************************/
