/******************************************************************************
 * @file     	EINT.h
 * @brief    	Drivers for EINT peripheral in lpc17xx.
 * @version  	1.1
 * @date     	19. May. 2016
 * @note		 	None
 ******************************************************************************/

#ifndef __EINT_H
#define __EINT_H
#include "lpc17xx.h"
#include "ADS1256.h"

//select EINT_num
#define	EINT0	0x00
#define EINT1 0x01
#define EINT2 0x02
#define EINT3 0x03

//EXTPOLAR
#define EINT_RISE	0x00		//ÉÏÉýÑØ
#define EINT_FALL	0x01		//ÏÂ½µÑØ

/**
  * @brief  Initialize  the EINT.
  *
  * @param  EINT_num: 	EINT port
	*					extPolar:		Polar of EINT(EINT_RISE or EINT_FALL)
  * @retval None 
  */
void Init_EINT(uint8_t EINT_num, uint8_t extPolar);

#endif

 /********************************* End Of File ********************************/

