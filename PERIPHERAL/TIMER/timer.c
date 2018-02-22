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
  * @retval 返回1：				初始化成功
	*							0:				未成功
	* @note		1：要是设置的MR0值和TC值匹配时产生中断，则用户应该去相应定时器的中断函数中做出相应的处理
	*					2：用户也可以通过设置寄存器MCR的值来决定是否要产生中断
	*					3：要是用户设置了当MR0匹配中断，则在中断函数中要清除相应的中断 及中断寄存器中第一位置为1;		
  */
 int Init_MR_Timer(char timer_num, uint32_t mr_num) 
{
	if(timer_num==0)
	{
		LPC_SC->PCONP |= (1<<1);                         //打开功率控制    
		LPC_TIM0->TCR = 0X02;                            //复位定时器  
		LPC_TIM0->PR   = 0x00;                           //预分频清零  使之每过1毫秒TC加1
		LPC_TIM0->MR0  = mr_num;                         //匹配寄存器与定时器计数器值相比较 
		LPC_TIM0->IR  = 0x3f;                            //清所有中断标志位  
		LPC_TIM0->MCR = 0X03;								            //MR0与TC值匹配时将产生中断,TC复位,要是不产生中断则改值应为2
		LPC_TIM0->CTCR = 0;                             //定时器模式                  
		NVIC_EnableIRQ(TIMER0_IRQn);  
		return 1;	
	} 
	if(timer_num==1)
	{
		LPC_SC->PCONP |= (1<<2);                         //打开功率控制    
		LPC_TIM1->TCR = 0X02;                            //复位定时器  
		#ifdef USE_ADS1256_SEISMIC_SIGNAL
			LPC_TIM1->PR   = 0x61A8;                         //使之每1msTC加1
			LPC_TIM1->MCR = 0X00;								             //MR1与TC值匹配时将不产生中断也不复位
		#else
			LPC_TIM1->PR   = 0x00;                           //预分频清零  
			LPC_TIM1->MCR = 0X03;								             ////MR1与TC值匹配时将产生中断,TC复位
		#endif
		LPC_TIM1->MR0  = mr_num;                         //匹配寄存器与定时器计数器值相比较  
		LPC_TIM1->IR  = 0x3f;                            //清所有中断标志位  
		LPC_TIM1->CTCR = 0;                              //定时器模式                  
		LPC_TIM1->TCR  = 0x01;                          //启动定时器  
		NVIC_EnableIRQ(TIMER1_IRQn);  
		return 1;	
	} 
	if(timer_num==2)
	{
		LPC_SC->PCONP |= (1<<22);                        //打开功率控制    
		LPC_TIM2->TCR = 0X02;                            //复位定时器  
		LPC_TIM2->PR   = 0x00;                           //预分频清零  
		LPC_TIM2->MR0  = mr_num;                         //匹配寄存器与定时器计数器值相比较  
		LPC_TIM2->IR  = 0xff;                            //清所有中断标志位  
		LPC_TIM2->MCR = 0X03;								             //MR0与TC值匹配时将产生中断,TC复位，要是不产生中断则改值应为2
		LPC_TIM2->CTCR = 0;                              //定时器模式                  
		LPC_TIM2->TCR  = 0x01;                           //启动定时器  
		NVIC_EnableIRQ(TIMER2_IRQn);  
		return 1;	
	} 
	if(timer_num==3)
	{
		LPC_SC->PCONP |= (1<<23);                         //打开功率控制    
		LPC_TIM3->TCR = 0X02;                             //复位定时器  
		LPC_TIM3->PR   = 0x00;                            //预分频清零  
		LPC_TIM3->MR0  = mr_num;                          //匹配寄存器与定时器计数器值相比较  
		LPC_TIM3->IR  = 0xff;                             //清所有中断标志位  
		LPC_TIM3->MCR = 0X03;								              //MR0与TC值匹配时将产生中断,TC复位，要是不产生中断则改值应为2
		LPC_TIM3->CTCR = 0;                               //定时器模式                  
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
	*					interrupt_state:		1则匹配进中断；0则匹配不进中断
  * @retval 返回1：				初始化成功
	*							0:				未成功
	*	@note		1：要是设置的MR0值和TC值匹配时产生中断，则用户应该去相应定时器的中断函数中做出相应的处理
	*					2：用户也可以通过设值interrupt_state的值来决定是否要产生中断
	*					3：在调用该函数前要选择相应的功能口线并且设置成MAT功能，对应MAT0.0 (Timer0 上的0通道)
	*							配置外部匹配寄存器EMR、MR0，MCR以及MR0对应的事件（可以是中断、复位或是停止定时器）
  */
int Init_MAT_Timer ( int timer_num, int mr_num,int interrupt_state) 
{
	if ( timer_num == 0 )
	{
		LPC_SC->PCONP |= (1<<1);                         //打开功率控制  
		LPC_TIM0->MR0 = mr_num;
		LPC_TIM0->EMR = 0x30;			                       //设置当产生匹配时间后设置的管脚电平是翻转还是不做变化
		if(interrupt_state==1)
		{
			LPC_TIM0->MCR = 0x03;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
			NVIC_EnableIRQ(TIMER0_IRQn);                    
		}
		else
			LPC_TIM0->MCR = 0x02;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
		return (1);
	}
	if ( timer_num == 1 )
	{
		LPC_SC->PCONP |= (1<<2);                        //打开功率控制    
		LPC_TIM1->MR0 = mr_num;
		LPC_TIM1->EMR = 0x30;			                      //设置当产生匹配时间后设置的管脚电平是翻转还是不做变化
		LPC_TIM1->IR  = 0xff;                           //清所有中断标志位         
		if(interrupt_state==1)
		{
			LPC_TIM1->MCR = 0x03;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
			NVIC_EnableIRQ(TIMER1_IRQn);                    
		}
		else
			LPC_TIM1->MCR = 0x02;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
		return (1);
	}

	if ( timer_num == 2 )
	{

		LPC_SC->PCONP |= (1<<22);                        //打开功率控制    
		LPC_TIM2->MR0 = mr_num;
		LPC_TIM2->EMR = 0x30;			                      //设置当产生匹配时间后设置的管脚电平是翻转还是不做变化
		LPC_TIM2->IR  = 0xff;                           //清所有中断标志位         
		if(interrupt_state==1)
		{
			LPC_TIM2->MCR = 0x03;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
			NVIC_EnableIRQ(TIMER1_IRQn);                    
		}
		else
			LPC_TIM2->MCR = 0x02;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
		return (1);
	}
	if ( timer_num == 3 )
	{
		LPC_SC->PCONP |= (1<<23);                        //打开功率控制    
		LPC_TIM3->MR0 = mr_num;
		LPC_TIM3->EMR = 0x30;			                      //设置当产生匹配时间后设置的管脚电平是翻转还是不做变化
		LPC_TIM3->IR  = 0xff;                           //清所有中断标志位         
		if(interrupt_state==1)
		{
			LPC_TIM3->MCR = 0x03;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
			NVIC_EnableIRQ(TIMER3_IRQn);                    
		}
		else
			LPC_TIM3->MCR = 0x02;				                   //设置MR0与TC值匹配时是否产生中断：2表示不产生中断只是复位TC，3表示产生中断复位TC
		return (1);
	}				 
	return (0);
}

/**
  * @brief  Initialize of CAP Timer(定时器捕获功能设置).
  * @param  timer_num:		port of timer
	*					interrupt_state:		1则匹配进中断；0则匹配不进中断
  * @retval 返回1：				初始化成功
	*							0:				未成功
	* @note		1：要是设置的上升沿或者下降沿捕获时产生中断，则用户应该去相应定时器的中断函数中做出相应的处理
	*					2：用户也可以通过设值interrupt_state的值来决定捕获时是否要产生中断
	*					3：在调用该函数前要选择相应的功能口线并且设置成CAP捕获功能，即对应CAP0.0 (Timer0 上的0通道)
	*					4：要是用户设置了当捕获时产生中断，则在中断函数中要清除相应的中断 及中断寄存器中第一位置为4;		
  */
int Init_CAP_Timer ( int timer_num,int interrupt_state) 
{
	if ( timer_num == 0 )
	{
		LPC_PINCON->PINSEL3 |= (1<<20)|(1<<21)|(1<<22)|(1<<23);  
		LPC_SC->PCONP |= (1<<1);                         //打开功率控制  
		LPC_TIM0->TCR = 0x03;                            //使能定时器  
		LPC_TIM0->PR  = 0x00;                            //预分频清零  
		LPC_TIM0->IR  = 0x3f;                            //清所有中断标志位   
		LPC_TIM0->TCR = 0x01;
		if(interrupt_state == 1)
		{
			LPC_TIM0->CCR |= (1<<0)|(1<<2)|(1<<3)|(1<<5);  //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
			NVIC_EnableIRQ(TIMER0_IRQn);                    
		}
		else
			LPC_TIM0->CCR = 0x01;				                   //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
		return (1);
	}
	if ( timer_num == 1 )
	{
		LPC_SC->PCONP |= (1<<2);                         //打开功率控制  
		LPC_TIM1->TCR = 0x03;                            //复位定时器  
		LPC_TIM1->PR   = 0x00;                           //预分频清零  
		LPC_TIM1->IR  = 0x3f;                            //清所有中断标志位   
		LPC_TIM1->TCR = 0x01;
		if(interrupt_state==1)
		{
			LPC_TIM1->CCR = 0X2D;                             //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
			NVIC_EnableIRQ(TIMER1_IRQn);                    
		}
		else
			LPC_TIM1->CCR = 0x01;				                   //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
		return (1);
	}
	if ( timer_num == 2 )
	{
		LPC_SC->PCONP |= (1<<22);                         //打开功率控制  
		LPC_TIM2->TCR = 0x03;                             //复位定时器  
		LPC_TIM2->PR   = 0x00;                            //预分频清零  
		LPC_TIM2->IR  = 0x3f;                             //清所有中断标志位   
		LPC_TIM2->TCR = 0x01;
		if(interrupt_state==1)
		{
			LPC_TIM2->CCR = 0X2D;                                //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
			NVIC_EnableIRQ(TIMER2_IRQn);                    
		}
		else
			LPC_TIM2->CCR = 0x01;				                   //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
		return (1);
	}
	if ( timer_num == 3 )
	{
		LPC_SC->PCONP |= (1<<23);                         //打开功率控制  
		LPC_TIM3->TCR = 0x03;                             //复位定时器  
		LPC_TIM3->PR   = 0x00;                            //预分频清零  
		LPC_TIM3->IR  = 0x3f;                             //清所有中断标志位   
		LPC_TIM3->TCR = 0x01; 
		if(interrupt_state==1)
		{
			LPC_TIM3->CCR = 0X2D;                                //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
			NVIC_EnableIRQ(TIMER3_IRQn);                    
		}
		else
			LPC_TIM3->CCR = 0x01;				                   //设置捕获方式和捕获的时候要不要产生中断，1表示产生中断，0表示不产生中断
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
		if(sampleNum == 0)											//判断是否为每个数据包的第一个采样点，若是，添加该采样点的时间信息
		{
			a_Event.flag = SAM_FIRST_POINT_EVENT;
			EventQueue_insert(&sample_EventQueue, &a_Event);
		}
		//前者判断上一次采样点已经结束，后者判断现在已经处于空闲状态
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
	//清中断标志
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
