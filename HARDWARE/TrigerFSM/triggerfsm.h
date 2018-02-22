/******************************************************************************
 * @file     	TriggerFSM.h
 * @brief    	trigger Status Machine.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/
 
#ifndef	__TRIGGERFSM_H
#define	__TRIGGERFSM_H
#include "eventqueue.h"
#include "trigger_event.h"
#include "uart.h"
#include "RTC.h"

/*                           状态类型                            */
//系统状态
#define	SYS_IDLE_STATUS				0x01  //系统空闲状态
#define SYS_SAMPLERDY_STATUS	0x02	//系统准备采样状态
#define SYS_SAMPLING_STATUS		0x03	//系统正在采样状态，该状态是进入ADS1256采样状态的前提
//ADS1256采样状态
#define ADS1256_IDLE_STATUS 						0x04  //ADS1256采样空闲状态
#define ADS1256_CHANGING_CH0_STATUS			0x05	//ADS1256正在进行通道0转换的状态
#define ADS1256_CHANGED_CH0_STATUS			0x06	//ADS1256通道0转换完成状态
#define ADS1256_SAMPLING_CH0_STATUS			0x07	//ADS1256正在进行通道0数据采样的状态
#define ADS1256_SAMPLED_CH0_STATUS			0x08	//ADS1256通道0采样完成状态
#define ADS1256_CHANGING_CH1_STATUS			0x09	//ADS1256正在进行通道1转换的状态
#define ADS1256_CHANGED_CH1_STATUS			0x0A	//ADS1256通道1转换完成状态
#define ADS1256_SAMPLING_CH1_STATUS			0x0B	//ADS1256正在进行通道1数据采样的状态
#define ADS1256_SAMPLED_CH1_STATUS			0x0C	//ADS1256通道1采样完成状态
#define ADS1256_CHANGING_CH2_STATUS			0x0D	//ADS1256正在进行通道2转换的状态
#define ADS1256_CHANGED_CH2_STATUS			0x0E	//ADS1256通道2转换完成状态
#define ADS1256_SAMPLING_CH2_STATUS			0x0F	//ADS1256正在进行通道2数据采样的状态
#define ADS1256_SAMPLED_CH2_STATUS			0x10	//ADS1256通道2采样完成状态
#define ADS1256_CHANGING_CH3_STATUS			0x11	//ADS1256正在进行通道3转换的状态
#define ADS1256_CHANGED_CH3_STATUS			0x12	//ADS1256通道3转换完成状态
#define ADS1256_SAMPLING_CH3_STATUS			0x13	//ADS1256正在进行通道3数据采样的状态

typedef	struct {
	char	statusid;
} StatusMachine;


extern uint32_t tc0Reset;
extern uint8_t startAD_flag;
extern StatusMachine ads1256_Status;  	//ADS1256采样状态结构体
extern StatusMachine Sys_Status;  			//系统状态结构体
extern volatile uint16_t sampleNum;
extern char AdSample_Buffer[650];								//采样所得数据
extern char DateTemp_Buffer[15];
extern char rePara_Buffer[25];

/**
  * @brief  分配ADS1256队列事件.
  * @param  pt_lsm：状态机指针		pt_event：事件指针
  * @retval None 
  */
void dispatch_ads1256_event(StatusMachine *pt_lsm, Event *pt_event);

/**
  * @brief  分配系统队列事件.
  * @param  pt_lsm：状态机指针		pt_event：事件指针
  * @retval None 
  */
void dispatch_sys_event(StatusMachine *pt_lsm, Event *pt_event);

/**
  * @brief  从串口接收数据中分出有效数据.
  * @param  RecSerialBuffer：	串口接收到的数据存放的数组地址
	*					RecSerialLength：	串口接收到的数据长度
	*					gpsBuffer：				解析串口数据中的GPS信息的地址
	*					gpsEffective：		GPS数据有效标志位，1为有效，0为无效
	*					paraBuffer：			解析串口数据中的参数命令的地址
	*					paraEffective：		参数命令有效标志位，1为有效，0为无效
  * @retval None 
  */
void do_task_on_ReadEffectiveBuffer(char *RecSerialBuffer, int RecSerialLength, char *gpsBuffer, char *gpsEffective, char *paraBuffer, char *paraEffective);

#endif

 /********************************* End Of File ********************************/
 


