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
char AdSample_traBuffer[1500];						//�������õ�Ҫ���ͳ�ȥ������
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
StatusMachine Sys_Status = {SYS_IDLE_STATUS};  			//ϵͳ״̬�ṹ��
StatusMachine ads1256_Status = {ADS1256_IDLE_STATUS};  	//ADS1256����״̬�ṹ��

extern char gpsBuffer[30];									//���ڽ��յ�GPSʱ����Ϣ�Ľ�������
extern char paraBuffer[30];								//���ڽ��յ��Ĳ�����������Ľ�������

/**
  * @brief  ִ�л�ȡһ�����������ݵ�����.
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
  * @brief  ����ADS1256�����¼�.
  * @param  pt_lsm��״̬��ָ��		pt_event���¼�ָ��
  * @retval None 
  */
void dispatch_ads1256_event(StatusMachine *pt_lsm, Event *pt_event)
{
	//���ڱ�������ֻ��һ��״̬����ˣ�ֻ��ֱ���ж��¼��������ж�״̬
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
  * @brief  ִ�л�ȡһ�����������ݵ�����.
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
  * @brief  ִ�л�ȡһ�����������ݵ�����.
  * @param  None
  * @retval None 
  */
void do_task_on_RdyStartSam(void)
{
	Enable_Timer(1);
	startAD_flag = 1;			//��λ��־λ��ʹ֮����һ�����ʼ����
	sampleNum = 0;								//�յ�ֹͣ�������������������������ϣ�����������Ϊ0
}

/**
  * @brief  ���ò���.
  * @param  None
  * @retval None 
  */
void do_task_on_SamSetPara(void)
{
	switch(paraBuffer[2])						//��������
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
	switch(paraBuffer[1])						//���ò�������
	{
		case 1:
			ADS1256_SetSampleRate(SAM_RATE_50SPS);												//�趨����Ƶ��
			tc0Reset = CLK_TIMER0/SAM_RATE_50SPS/2;						//���ڱ���ÿ��һ������ʱ��TC0��Ӧ�ø�ֵ����������ʹ����Ƶ��ʱ���У׼
			rePara_Buffer[8] = 0x01;
			break;
		case 2:
			ADS1256_SetSampleRate(SAM_RATE_100SPS);   //ͬcase 1
			tc0Reset = CLK_TIMER0/SAM_RATE_100SPS/2;
			rePara_Buffer[8] = 0x02;
			break;
		case 3:
			ADS1256_SetSampleRate(SAM_RATE_200SPS);   //ͬcase 1
			tc0Reset = CLK_TIMER0/SAM_RATE_200SPS/2;
			rePara_Buffer[8] = 0x03;
			break;
		case 4:
			ADS1256_SetSampleRate(SAM_RATE_250SPS);   //ͬcase 1
			tc0Reset = CLK_TIMER0/SAM_RATE_250SPS/2;
			rePara_Buffer[8] = 0x04;
			break;
		default:
			break;
	}
	instrumentID = paraBuffer[6];
	a_Event.flag = UART_INAUIRY_EVENT;											//���´��¼�Ϊʱ����������
	EventQueue_insert(&sample_EventQueue, &a_Event);				//�����¼��������
}

/**
  * @brief  ������ת����ͨѶЭ��Ҫ��ĸ�ʽ��������Ч�����ڵ�0x10��Ϊ0x10 0x10.
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
  * @brief  ���ݲɼ���ɣ������ݷ��ͳ�ȥ.
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
  * @brief  ֹͣ����.
  * @param  None
  * @retval None 
  */
void do_task_on_SamStop(void)
{
	ADS1256_stopSample();
	Disable_Timer(1);
	LPC_TIM1->TC = 0;
	sampleNum = 0;								//�յ�ֹͣ�������������������������ϣ�����������Ϊ0
	traFlag = 0;
}

/**
  * @brief  ֹͣ����.
  * @param  None
  * @retval None 
  */
void do_task_on_FirstPoint(void)
{
	if((AdSample_Buffer[10]*256+AdSample_Buffer[11]) >= (timeMsCounter-100))			//��(timeMsCounter-2)��ԭ���ĺ���ʱ��С����˵������һ�룬�������ʱ��
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
  * @brief  �Ӵ��ڽ��������зֳ���Ч����.
  * @param  RecSerialBuffer��	���ڽ��յ������ݴ�ŵ������ַ
	*					RecSerialLength��	���ڽ��յ������ݳ���
	*					gpsBuffer��				�������������е�GPS��Ϣ�ĵ�ַ
	*					gpsEffective��		GPS������Ч��־λ��1Ϊ��Ч��0Ϊ��Ч
	*					paraBuffer��			�������������еĲ�������ĵ�ַ
	*					paraEffective��		����������Ч��־λ��1Ϊ��Ч��0Ϊ��Ч
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
					if(RecSerialBuffer[i+5] == 0x01)            //��ѯ
					{
						if((RecSerialBuffer[i+6]==instrumentID) && (RecSerialBuffer[i+7]==0x00) && (instrumentID != unusefulID))
						{
							a_Event.flag = UART_INAUIRY_EVENT;											//���´��¼�Ϊʱ����������
							EventQueue_insert(&sample_EventQueue, &a_Event);				//�����¼��������
						}
					}
					else if(RecSerialBuffer[i+5] == 0x02)               //����״̬����
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
					else if(RecSerialBuffer[i+5] == 0x03)               //GPS�ź�
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
  * @brief  ����״̬����Ӧ��
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
  * @brief  ����ϵͳ�����¼�.
  * @param  pt_lsm��״̬��ָ��		pt_event���¼�ָ��
  * @retval None 
  */
void dispatch_sys_event(StatusMachine *pt_lsm, Event *pt_event)
{
	switch(pt_lsm->statusid)
	{
		case SYS_IDLE_STATUS:
			if(pt_event->flag == UART_TIME_SET_EVENT)
			{
				//���մ�������ʱ���¼���״̬���ı�
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
				//ϵͳ����״̬�£����ò����������ı�״̬
			}
			else if(pt_event->flag == UART_SAM_STOP_EVENT)
			{
				//�˴���Ҫ����λ��������ʾ���Ѿ�����ֹͣ���ڴ���״̬
				//Э����Ҫ����
			}
			else if(pt_event->flag == SAM_POINT_ENOUGH_EVENT)
			{
				do_task_on_SamPointEnough();
				//���Ͳ������ݲ��ı�״̬
			}
			else if(pt_event->flag == SAM_ONE_POINT_FINISH_EVENT)
			{
				do_task_on_GetOneSamPointData();
				//��ȡ�������ݣ����ı�״̬
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
				//���մ�������ʱ���¼���״̬���ı�
				do_task_on_TimeSet();
			}
			else if(pt_event->flag == UART_SAM_START_EVENT)
			{
				//�˴���Ҫ����λ��������ʾ���Ѿ���ʼ����
				//Э����Ҫ����
			}
			else if(pt_event->flag == UART_SAM_PARA_SET_EVENT)
			{
				//�˴���Ҫ����λ��������ʾ�����ڲ���״̬����Ҫֹͣ�������ò���
				//Э����Ҫ����
			}
			else if(pt_event->flag == UART_SAM_STOP_EVENT)
			{
				do_task_on_SamStop();
				pt_lsm->statusid = SYS_IDLE_STATUS;
			}
			else if(pt_event->flag == SAM_POINT_ENOUGH_EVENT)
			{
				do_task_on_SamPointEnough();
				//���Ͳ������ݲ��ı�״̬
			}
			else if(pt_event->flag == SAM_ONE_POINT_FINISH_EVENT)
			{
				do_task_on_GetOneSamPointData();
				//��ȡ�������ݣ����ı�״̬
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
				//���մ�������ʱ���¼���״̬���ı�
				do_task_on_TimeSet();
			}
			else if(pt_event->flag == UART_SAM_START_EVENT)
			{
				//�˴���Ҫ����λ��������ʾ���Ѿ���ʼ����
				//Э����Ҫ����
			}
			else if(pt_event->flag == UART_SAM_PARA_SET_EVENT)
			{
				//�˴���Ҫ����λ��������ʾ�����ڲ���״̬����Ҫֹͣ�������ò���
				//Э����Ҫ����
			}
			else if(pt_event->flag == UART_SAM_STOP_EVENT)
			{
				do_task_on_SamStop();
				pt_lsm->statusid = SYS_IDLE_STATUS;
			}
			else if(pt_event->flag == SAM_POINT_ENOUGH_EVENT)
			{
				do_task_on_SamPointEnough();
				//���Ͳ������ݲ��ı�״̬
			}
			else if(pt_event->flag == SAM_ONE_POINT_FINISH_EVENT)
			{
				do_task_on_GetOneSamPointData();
				//��ȡ�������ݣ����ı�״̬
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
 

