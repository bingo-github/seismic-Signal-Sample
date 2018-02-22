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

#define CLK_TIMER0	25000000		//定时器0的时钟频率为25000000

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
 int Init_MR_Timer(char timer_num, uint32_t mr_num);

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
int Init_MAT_Timer ( int timer_num, int mr_num,int interrupt_state);

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
