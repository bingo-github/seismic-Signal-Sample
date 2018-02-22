/******************************************************************************
 * @file     	ADS1256.c
 * @brief    	Drivers for ADS1256.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/
 
#ifndef __ADS1256_H
#define __ADS1256_H
#include "lpc17xx.h"
#include "SPI.h"
#include "timer.h"

#define USE_ADS1256_SEISMIC_SIGNAL

#define ADS_DRDY_Value	(LPC_GPIO0->FIOPIN & (1 << 19))		//DRDY的电平值,用于判断ADS1256是否处于可用状态
#define ADS_DRDY_BIT		19																//用于DRDY的引脚的位数

// define multiplexer codes 
#define ADS1256_MUXP_AIN0   0x00 
#define ADS1256_MUXP_AIN1   0x10 
#define ADS1256_MUXP_AIN2   0x20 
#define ADS1256_MUXP_AIN3   0x30 
#define ADS1256_MUXP_AIN4   0x40 
#define ADS1256_MUXP_AIN5   0x50 
#define ADS1256_MUXP_AIN6   0x60 
#define ADS1256_MUXP_AIN7   0x70 
#define ADS1256_MUXP_AINCOM 0x80 
 
#define ADS1256_MUXN_AIN0   0x00 
#define ADS1256_MUXN_AIN1   0x01 
#define ADS1256_MUXN_AIN2   0x02 
#define ADS1256_MUXN_AIN3   0x03 
#define ADS1256_MUXN_AIN4   0x04 
#define ADS1256_MUXN_AIN5   0x05 
#define ADS1256_MUXN_AIN6   0x06 
#define ADS1256_MUXN_AIN7   0x07 
#define ADS1256_MUXN_AINCOM 0x08   

// define gain codes 
#define ADS1256_GAIN_1      0x00 
#define ADS1256_GAIN_2      0x01 
#define ADS1256_GAIN_4      0x02 
#define ADS1256_GAIN_8      0x03 
#define ADS1256_GAIN_16     0x04 
#define ADS1256_GAIN_32     0x05 
#define ADS1256_GAIN_64     0x06 
//#define ADS1256_GAIN_64     0x07 

//define rate
#define SAM_RATE_50SPS	50
#define SAM_RATE_100SPS	100
#define SAM_RATE_200SPS	200
#define SAM_RATE_250SPS	250

/**
  * @brief  设置采样频率.
  * @param  rate:		采样频率
  * @retval None 
  */
void ADS1256_SetSampleRate(uint16_t rate);

/**
  * @brief  设置采样增益.
  * @param  GAIN:		采样增益
  * @retval None 
  */
void ADS1256_SetSampleGain(uint8_t GAIN);

/**
  * @brief  Initialize the ADS1256.
  * @param  channel:	采样通道
	*					gain:			采样增益
	*					rate:	采样频率
  * @retval None 
  */
void ADS1256_Init(char channel,char gain,uint16_t rate);

/**
  * @brief  开始采样.
  * @param  None
  * @retval None 
  */
void ADS1256_startSample(void);

/**
  * @brief  停止采样.
  * @param  None
  * @retval None 
  */
void ADS1256_stopSample(void);

/**
  * @brief  变换采样通道.
  * @param  toChannel: 目标通道
  * @retval None 
  */
void ADS1256_changeCH(char toChannel);

/**
  * @brief  停止采样.
  * @param  None
  * @retval None 
  */
void ADS1256_sampleOneCH(void);

/**
  * @brief  采集当前一个通道是数据.
  * @param  None
  * @retval None 
  */
void ADS1256_sampleOneCH(void);

/**
  * @brief  读取ADS1256采样数据.
  * @param  DestinationAddress：	Address of Destination
	*					len:									Length of buffer
  * @retval None 
  */
uint8_t ADS1256_ReadSampleData(char *DestinationAddress, int len);

#endif

/********************************* End Of File ********************************/

