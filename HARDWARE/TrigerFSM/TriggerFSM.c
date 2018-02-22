/******************************************************************************
 * @file     	TriggerFSM.c
 * @brief    	trigger Status Machine.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/
 
#include "TriggerFSM.h"
#include "EINT.h"

#define unusefulID	0x00
/****** private variable ******/
char AdSample_traBuffer[1500];						//采样所得的要发送出去的数据
uint8_t traFlag = 0;
char instrumentID = unusefulID;								
char rePara_traBuffer[25];

/****** public variable ******/
uint32_t tc0Reset = 0; 
uint8_t startAD_flag = 0;	
char DateTemp_Buffer[15];
char AdSample_Buffer[650];
char rePara_Buffer[25];
volatile uint16_t sampleNum = 0;
StatusMachine Sys_Status = {SYS_IDLE_STATUS};  			//系统状态结构体
StatusMachine ads1256_Status = {ADS1256_IDLE_STATUS};  	//ADS1256采样状态结构体

extern char gpsBuffer[30];									//串口接收到GPS时间信息的接收数组
extern char paraBuffer[30];								//串口接收到的参数设置命令的接收数组

/**
  * @brief  执行获取一个采样点数据的任务.
  * @param  None
  * @retval None 
  */
void do_task_on_GetOneSamPointData(void)
{
	char ADS1256_recData[12];
	uint8_t i;
	ADS1256_ReadSampleData(ADS1256_recData,12);
	for(i=0;i<12;i++)
	{
		AdSample_Buffer[12+sampleNum] = ADS1256_recData[i];
		sampleNum++;
	}
}

/**
  * @brief  分配ADS1256队列事件.
  * @param  pt_lsm：状态机指针		pt_event：事件指针
  * @retval None 
  */
void dispatch_ads1256_event(StatusMachine *pt_lsm, Event *pt_event)
{
	//由于本程序中只有一个状态，因此，只需直接判断事件，不需判断状态
	switch (pt_lsm->statusid)
	{
		case ADS1256_IDLE_STATUS:
			if(pt_event->flag == ADS1256_SAM_TIMEUP_EVENT)
			{
				ads1256_Status.statusid = ADS1256_CHANGING_CH0_STATUS;
				ADS1256_changeCH(ADS1256_MUXP_AIN0|ADS1256_MUXN_AIN1);
			}
			break;
		case ADS1256_CHANGING_CH0_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_CHANGED_CH0_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_CHANGED_CH0_STATUS)
				{
					ads1256_Status.statusid = ADS1256_SAMPLING_CH0_STATUS;
					ADS1256_sampleOneCH();
				}
			}
			break;
		case ADS1256_SAMPLING_CH0_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_SAMPLED_CH0_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_SAMPLED_CH0_STATUS)
				{
					ads1256_Status.statusid = ADS1256_CHANGING_CH1_STATUS;
					ADS1256_changeCH(ADS1256_MUXP_AIN2|ADS1256_MUXN_AIN3);
				}
			}
			break;
		case ADS1256_CHANGING_CH1_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_CHANGED_CH1_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_CHANGED_CH1_STATUS)
				{
					ads1256_Status.statusid = ADS1256_SAMPLING_CH1_STATUS;
					ADS1256_sampleOneCH();
				}
			}
			break;
		case ADS1256_SAMPLING_CH1_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_SAMPLED_CH1_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_SAMPLED_CH1_STATUS)
				{
					ads1256_Status.statusid = ADS1256_CHANGING_CH2_STATUS;
					ADS1256_changeCH(ADS1256_MUXP_AIN4|ADS1256_MUXN_AIN5);
				}
			}
			break;
		case ADS1256_CHANGING_CH2_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_CHANGED_CH2_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_CHANGED_CH2_STATUS)
				{
					ads1256_Status.statusid = ADS1256_SAMPLING_CH2_STATUS;
					ADS1256_sampleOneCH();
				}
			}
			break;
		case ADS1256_SAMPLING_CH2_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_SAMPLED_CH2_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_SAMPLED_CH2_STATUS)
				{
					ads1256_Status.statusid = ADS1256_CHANGING_CH3_STATUS;
					ADS1256_changeCH(ADS1256_MUXP_AIN6|ADS1256_MUXN_AIN7);
				}
			}
			break;
		case ADS1256_CHANGING_CH3_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_CHANGED_CH3_STATUS;
				if(ADS_DRDY_Value != (1 << ADS_DRDY_BIT) && ads1256_Status.statusid == ADS1256_CHANGED_CH3_STATUS)
				{
					ads1256_Status.statusid = ADS1256_SAMPLING_CH3_STATUS;
					ADS1256_sampleOneCH();
				}
			}
			break;		
		case ADS1256_SAMPLING_CH3_STATUS:
			if(pt_event->flag == ADS1256_TRA_FINISH_EVENT)
			{
				ads1256_Status.statusid = ADS1256_IDLE_STATUS;
			}
		default:
			break;
	}
}

/**
  * @brief  执行获取一个采样点数据的任务.
  * @param  None
  * @retval None 
  */
void do_task_on_TimeSet(void)
{
	DateTemp_Buffer[0] = gpsBuffer[0];
	DateTemp_Buffer[1] = gpsBuffer[1];
	DateTemp_Buffer[2] = gpsBuffer[2];
	DateTemp_Buffer[3] = gpsBuffer[3];
	DateTemp_Buffer[4] = gpsBuffer[4];
	DateTemp_Buffer[5] = gpsBuffer[5];
	timeSetTaskEffective = 1;
}

/**
  * @brief  执行获取一个采样点数据的任务.
  * @param  None
  * @retval None 
  */
void do_task_on_RdyStartSam(void)
{
	Enable_Timer(1);
	startAD_flag = 1;			//置位标志位，使之在下一秒初开始采样
	sampleNum = 0;								//收到停止采样，则该秒采样到的数据作废，采样个数清为0
}

/**
  * @brief  设置参数.
  * @param  None
  * @retval None 
  */
void do_task_on_SamSetPara(void)
{
	switch(paraBuffer[2])						//增益设置
	{
		case 1:
			ADS1256_SetSampleGain(ADS1256_GAIN_1);
			rePara_Buffer[9] = 1;
			break;
		case 2:
			ADS1256_SetSampleGain(ADS1256_GAIN_2);
			rePara_Buffer[9] = 2;
			break;
		case 4:
			ADS1256_SetSampleGain(ADS1256_GAIN_4);
			rePara_Buffer[9] = 4;
			break;
		case 8:
			ADS1256_SetSampleGain(ADS1256_GAIN_8);
			rePara_Buffer[9] = 8;
			break;
		case 16:
			ADS1256_SetSampleGain(ADS1256_GAIN_16);
			rePara_Buffer[9] = 16;
			break;
		case 32:
			ADS1256_SetSampleGain(ADS1256_GAIN_32);
			rePara_Buffer[9] = 32;
			break;
		case 64:
			ADS1256_SetSampleGain(ADS1256_GAIN_64);
			rePara_Buffer[9] = 64;
			break;
		default:
			break;
	}
	switch(paraBuffer[1])						//设置采样速率
	{
		case 1:
			ADS1256_SetSampleRate(SAM_RATE_50SPS);												//设定采样频率
			tc0Reset = CLK_TIMER0/SAM_RATE_50SPS/2;						//用于保存每新一秒来临时，TC0所应该赋值的数，用于使采样频率时间的校准
			rePara_Buffer[8] = 0x01;
			break;
		case 2:
			ADS1256_SetSampleRate(SAM_RATE_100SPS);   //同case 1
			tc0Reset = CLK_TIMER0/SAM_RATE_100SPS/2;
			rePara_Buffer[8] = 0x02;
			break;
		case 3:
			ADS1256_SetSampleRate(SAM_RATE_200SPS);   //同case 1
			tc0Reset = CLK_TIMER0/SAM_RATE_200SPS/2;
			rePara_Buffer[8] = 0x03;
			break;
		case 4:
			ADS1256_SetSampleRate(SAM_RATE_250SPS);   //同case 1
			tc0Reset = CLK_TIMER0/SAM_RATE_250SPS/2;
			rePara_Buffer[8] = 0x04;
			break;
		default:
			break;
	}
	instrumentID = paraBuffer[6];
	a_Event.flag = UART_INAUIRY_EVENT;											//记下此事件为时间设置命令
	EventQueue_insert(&sample_EventQueue, &a_Event);				//将该事件插入队伍
}

/**
  * @brief  将数据转换成通讯协议要求的格式，即将有效数据内的0x10变为0x10 0x10.
  * @param  None
  * @retval None 
  */
int do_task_on_changeToProtocol(char origTraBuffer[], int origTraBufferLen, char trasfTraBuffer[]) 
{
	int i;
	int addLen=0;
	trasfTraBuffer[0]=0x10;
	trasfTraBuffer[1]=0x01;
	for(i=2;i<(origTraBufferLen-2);i++)
	{
		if(origTraBuffer[i]==0x10)
		{
			trasfTraBuffer[i+addLen]=origTraBuffer[i];
			addLen++;
			trasfTraBuffer[i+addLen]=origTraBuffer[i];
		}
		else trasfTraBuffer[i+addLen]=origTraBuffer[i];
	}
	trasfTraBuffer[i+addLen]=0x10;
	i++;
	trasfTraBuffer[i+addLen]=0x02;
	return (origTraBufferLen+addLen);
}

/**
  * @brief  数据采集完成，将数据发送出去.
  * @param  None
  * @retval None 
  */
void do_task_on_SamPointEnough(void)
{
	int len;
	if((traFlag >= CLK_TIMER0/tc0Reset/2/50) && (timeMsCounter<1100))
	{
		len=do_task_on_changeToProtocol(AdSample_Buffer,614,AdSample_traBuffer);
		WriteUART_Cache(0,AdSample_traBuffer,len);
	}
	else
	{
		traFlag++;
		if(traFlag >50)
		{
			traFlag = 50;
		}
	}
	sampleNum = 0;
}

/**
  * @brief  停止采样.
  * @param  None
  * @retval None 
  */
void do_task_on_SamStop(void)
{
	ADS1256_stopSample();
	Disable_Timer(1);
	LPC_TIM1->TC = 0;
	sampleNum = 0;								//收到停止采样，则该秒采样到的数据作废，采样个数清为0
	traFlag = 0;
}

/**
  * @brief  停止采样.
  * @param  None
  * @retval None 
  */
void do_task_on_FirstPoint(void)
{
	if((AdSample_Buffer[10]*256+AdSample_Buffer[11]) >= (timeMsCounter-100))			//当(timeMsCounter-2)比原来的毫秒时间小，这说明是新一秒，则更新秒时间
	{
		AdSample_Buffer[4] = theTime.RTC_Year;
		AdSample_Buffer[5] = theTime.RTC_Month;
		AdSample_Buffer[6] = theTime.RTC_Dom;
		AdSample_Buffer[7] = theTime.RTC_Hour;
		AdSample_Buffer[8] = theTime.RTC_Min;
		AdSample_Buffer[9] = theTime.RTC_Sec;
	}
	AdSample_Buffer[10] = (timeMsCounter-tc0Reset/25000) /256;
	AdSample_Buffer[11] = (timeMsCounter-tc0Reset/25000) %256;
}

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
void do_task_on_ReadEffectiveBuffer(char *RecSerialBuffer, int RecSerialLength, char *gpsBuffer, char *gpsEffective, char *paraBuffer, char *paraEffective)
{
	int i, j;
	int len = 0;
	for(i=0; i<(RecSerialLength-1); )  
	{
		if(RecSerialBuffer[i]==0x10 && RecSerialBuffer[i+1]==0x01 && (RecSerialBuffer[i+4]&0x80)==0x00)
		{
			if(RecSerialBuffer[i+2]*256+RecSerialBuffer[i+3] < 100)
			{
				if(RecSerialBuffer[i+4+RecSerialBuffer[i+2]*256+RecSerialBuffer[i+3]]==0x10 && RecSerialBuffer[i+5+RecSerialBuffer[i+2]*256+RecSerialBuffer[i+3]]==0x02)
				{
					len = RecSerialBuffer[i+2]*256+RecSerialBuffer[i+3]+6;
					if(RecSerialBuffer[i+5] == 0x01)            //查询
					{
						if((RecSerialBuffer[i+6]==instrumentID) && (RecSerialBuffer[i+7]==0x00) && (instrumentID != unusefulID))
						{
							a_Event.flag = UART_INAUIRY_EVENT;											//记下此事件为时间设置命令
							EventQueue_insert(&sample_EventQueue, &a_Event);				//将该事件插入队伍
						}
					}
					else if(RecSerialBuffer[i+5] == 0x02)               //参数状态设置
					{
						if((instrumentID==unusefulID) && (RecSerialBuffer[i+8]==0x03) && (RecSerialBuffer[i+14]!=unusefulID))
						{
							for(j=0; j<7; j++)
							{
								*(paraBuffer+j) = *(RecSerialBuffer+i+8+j);
							}
							*paraEffective = 1;
						}
						else if((RecSerialBuffer[i+6]==instrumentID) && (RecSerialBuffer[i+7]==0x00) && (instrumentID != unusefulID))
						{
							if((RecSerialBuffer[i+8]==0x03) && (RecSerialBuffer[i+14]!=unusefulID))
							{
								for(j=0; j<7; j++)
								{
									*(paraBuffer+j) = *(RecSerialBuffer+i+8+j);
								}
								*paraEffective = 1;
							}
							else if(RecSerialBuffer[i+8] != 0x03)
							{
								for(j=0; j<7; j++)
								{
									*(paraBuffer+j) = *(RecSerialBuffer+i+8+j);
								}
								*paraEffective = 1;
							}
						}
					}
					else if(RecSerialBuffer[i+5] == 0x03)               //GPS信号
					{
						if((RecSerialBuffer[i+6]==instrumentID) && (RecSerialBuffer[i+7]==0x00))
						{
							for(j=0; j<6; j++)
							{
								*(gpsBuffer+j) = *(RecSerialBuffer+i+8+j);
							}
							*gpsEffective = 1;
						}
					}
					i = i+len;
				}
			}
		}
		else
		{
			i++;
		}
	}
    
}

/**
  * @brief  发送状态参数应答
  * @param  None
  * @retval None 
  */
void do_task_on_rePara(void)
{
	int len;
	rePara_Buffer[6] = instrumentID;
	if(Sys_Status.statusid == UART_TIME_SET_EVENT)
	{
		rePara_Buffer[7] = 0x01;
	}
	else if(Sys_Status.statusid==SYS_SAMPLERDY_STATUS || Sys_Status.statusid==SYS_SAMPLING_STATUS)
	{
		rePara_Buffer[7] = 0x02;
	}
	rePara_Buffer[10] = rePara_Buffer[9];
	rePara_Buffer[11] = rePara_Buffer[9];
	rePara_Buffer[12] = rePara_Buffer[9];
	len = do_task_on_changeToProtocol(rePara_Buffer, 15, rePara_traBuffer);
	WriteUART_Cache(0,rePara_traBuffer,len);
}

/**
  * @brief  分配系统队列事件.
  * @param  pt_lsm：状态机指针		pt_event：事件指针
  * @retval None 
  */
void dispatch_sys_event(StatusMachine *pt_lsm, Event *pt_event)
{
	switch(pt_lsm->statusid)
	{
		case SYS_IDLE_STATUS:
			if(pt_event->flag == UART_TIME_SET_EVENT)
			{
				//接收串口设置时间事件，状态不改变
				do_task_on_TimeSet();
			}
			else if(pt_event->flag == UART_SAM_START_EVENT)
			{
				do_task_on_RdyStartSam();
				pt_lsm->statusid = SYS_SAMPLERDY_STATUS;
			}
			else if(pt_event->flag == UART_SAM_PARA_SET_EVENT)
			{
				do_task_on_SamSetPara();
				//系统待机状态下，设置采样参数不改变状态
			}
			else if(pt_event->flag == UART_SAM_STOP_EVENT)
			{
				//此处需要向上位机发送提示，已经处于停止处于待机状态
				//协议需要讨论
			}
			else if(pt_event->flag == SAM_POINT_ENOUGH_EVENT)
			{
				do_task_on_SamPointEnough();
				//发送采样数据不改变状态
			}
			else if(pt_event->flag == SAM_ONE_POINT_FINISH_EVENT)
			{
				do_task_on_GetOneSamPointData();
				//读取采样数据，不改变状态
			}
			else if(pt_event->flag == SAM_FIRST_POINT_EVENT)
			{
				do_task_on_FirstPoint();
			}
			else if(pt_event->flag == UART_INAUIRY_EVENT)
			{
				do_task_on_rePara();
			}
			break;
		case SYS_SAMPLERDY_STATUS:
			if(pt_event->flag == UART_TIME_SET_EVENT)
			{
				//接收串口设置时间事件，状态不改变
				do_task_on_TimeSet();
			}
			else if(pt_event->flag == UART_SAM_START_EVENT)
			{
				//此处需要向上位机发送提示，已经开始采样
				//协议需要讨论
			}
			else if(pt_event->flag == UART_SAM_PARA_SET_EVENT)
			{
				//此处需要向上位机发送提示，处于采样状态，需要停止才能设置参数
				//协议需要讨论
			}
			else if(pt_event->flag == UART_SAM_STOP_EVENT)
			{
				do_task_on_SamStop();
				pt_lsm->statusid = SYS_IDLE_STATUS;
			}
			else if(pt_event->flag == SAM_POINT_ENOUGH_EVENT)
			{
				do_task_on_SamPointEnough();
				//发送采样数据不改变状态
			}
			else if(pt_event->flag == SAM_ONE_POINT_FINISH_EVENT)
			{
				do_task_on_GetOneSamPointData();
				//读取采样数据，不改变状态
			}
			else if(pt_event->flag == SAM_FIRST_POINT_EVENT)
			{
				do_task_on_FirstPoint();
			}
			else if(pt_event->flag == UART_INAUIRY_EVENT)
			{
				do_task_on_rePara();
			}
			break;
		case SYS_SAMPLING_STATUS:
			if(pt_event->flag == UART_TIME_SET_EVENT)
			{
				//接收串口设置时间事件，状态不改变
				do_task_on_TimeSet();
			}
			else if(pt_event->flag == UART_SAM_START_EVENT)
			{
				//此处需要向上位机发送提示，已经开始采样
				//协议需要讨论
			}
			else if(pt_event->flag == UART_SAM_PARA_SET_EVENT)
			{
				//此处需要向上位机发送提示，处于采样状态，需要停止才能设置参数
				//协议需要讨论
			}
			else if(pt_event->flag == UART_SAM_STOP_EVENT)
			{
				do_task_on_SamStop();
				pt_lsm->statusid = SYS_IDLE_STATUS;
			}
			else if(pt_event->flag == SAM_POINT_ENOUGH_EVENT)
			{
				do_task_on_SamPointEnough();
				//发送采样数据不改变状态
			}
			else if(pt_event->flag == SAM_ONE_POINT_FINISH_EVENT)
			{
				do_task_on_GetOneSamPointData();
				//读取采样数据，不改变状态
			}
			else if(pt_event->flag == SAM_FIRST_POINT_EVENT)
			{
				do_task_on_FirstPoint();
			}
			else if(pt_event->flag == UART_INAUIRY_EVENT)
			{
				do_task_on_rePara();
			}
			break;
		default:
			break;
	}
}

/********************************* End Of File ********************************/
 

