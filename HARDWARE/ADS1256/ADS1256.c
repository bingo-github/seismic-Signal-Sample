/******************************************************************************
 * @file     	ADS1256.c
 * @brief    	Drivers for ADS1256.
 * @version  	1.1
 * @date     	21. May. 2016
 * @note		 	None
 ******************************************************************************/

#include "ADS1256.h"

#define ADS_REST_High() 	(LPC_GPIO0->FIOSET |= (1 << 20))					//��P0.20����
#define ADS_REST_Low()	 	(LPC_GPIO0->FIOCLR |= (1 << 20))					//��P0.20����
#define ADS_REST_Value		(LPC_GPIO0->FIOPIN & (1 << 20))						//REST�ĵ�ƽֵ
#define ADS_REST_BIT		20																				//����REST�����ŵ�λ��

//define drate codes 				//�ò���Ƶ����ADS1256�Ĳ���Ƶ�ʣ������Ʋ�׼�������˲��ö�ʱ��������ϵͳ�Ĳ���Ƶ��
#define ADS1256_DRATE_30000SPS   0xF0 
#define ADS1256_DRATE_15000SPS   0xE0 
#define ADS1256_DRATE_7500SPS   0xD0 
#define ADS1256_DRATE_3750SPS   0xC0 
#define ADS1256_DRATE_2000SPS   0xB0 
#define ADS1256_DRATE_1000SPS   0xA1 
#define ADS1256_DRATE_500SPS    0x92 
#define ADS1256_DRATE_100SPS    0x82 
#define ADS1256_DRATE_60SPS     0x72 
#define ADS1256_DRATE_50SPS     0x63 
#define ADS1256_DRATE_30SPS     0x53 
#define ADS1256_DRATE_25SPS     0x43 
#define ADS1256_DRATE_15SPS     0x33 
#define ADS1256_DRATE_10SPS     0x23 
#define ADS1256_DRATE_5SPS      0x13 
#define ADS1256_DRATE_2_5SPS    0x03

// define commands 
#define ADS1256_CMD_WAKEUP   0x00 
#define ADS1256_CMD_RDATA    0x01 
#define ADS1256_CMD_RDATAC   0x03 
#define ADS1256_CMD_SDATAC   0x0f 
#define ADS1256_CMD_RREG     0x10 
#define ADS1256_CMD_WREG     0x50 
#define ADS1256_CMD_SELFCAL  0xf0 
#define ADS1256_CMD_SELFOCAL 0xf1 
#define ADS1256_CMD_SELFGCAL 0xf2 
#define ADS1256_CMD_SYSOCAL  0xf3 
#define ADS1256_CMD_SYSGCAL  0xf4 
#define ADS1256_CMD_SYNC     0xfc 
#define ADS1256_CMD_STANDBY  0xfd 
#define ADS1256_CMD_REST    0xfe 

// define the ADS1256 register values 
#define ADS1256_STATUS       0x00   
#define ADS1256_MUX          0x01   
#define ADS1256_ADCON        0x02   
#define ADS1256_DRATE        0x03   
#define ADS1256_IO           0x04   
#define ADS1256_OFC0         0x05   
#define ADS1256_OFC1         0x06   
#define ADS1256_OFC2         0x07   
#define ADS1256_FSC0         0x08   
#define ADS1256_FSC1         0x09   
#define ADS1256_FSC2         0x0A 

/**
  * @brief  Initialize the I/O for ADS1256(RESET and DRDY).
  * @param  None
  * @retval None 
  */
void AdsIo_Init(void)
{	
	LPC_PINCON->PINSEL1 &= ~(0X03 << 8);	//P0.20��REST�ã��ø�I/O��ģ��SPI����ADS1256����λ�źţ��͵�ƽ��Ч
	LPC_GPIO0->FIODIR |= (1 << 20);
	//P0.19�ܽŵ�GPIO�жϹ��ܳ�ʼ�������ڼ��DRDY�½��شӶ������ж�
	LPC_PINCON->PINSEL1 &= ~(0x03 << 6);				//����P0.19ΪGPIO����
	LPC_GPIOINT->IO0IntClr |= 0xFFFFFF;					//����P0�ڵ�GPIO�жϱ�־
	LPC_GPIOINT->IO0IntEnF |= (1 << 19);				//����P0.19Ϊ�½����ж�ʹ��
	NVIC_EnableIRQ(EINT3_IRQn);									//DRDY�½����жϵ��жϺ�����EINT3�жϺ�����
  NVIC_SetPriority(EINT3_IRQn, 2);
}

/**
  * @brief  Delay for ADS1256.
  * @param  dal:		Delay num
  * @retval None 
  */
void Delay_ads1256(unsigned int dal)
{
	unsigned int i;
	for(i = 0;i < dal*5;i ++);
}

/**
  * @brief  Write a byte to a register.
  * @param  regaddr:		Address of register
	*					databyte:		the byte to write
  * @retval None 
  */
void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
	char temp_Byte[3];
	temp_Byte[0] = (ADS1256_CMD_WREG | (regaddr & 0xF));
	temp_Byte[1] = 0;
	temp_Byte[2] = databyte;
	Send_SPI(&pt_spi_tra_cbuffer,temp_Byte,3,SPI_VALID_TRUE);	//ͨ��SPI��ADS1256����д�Ĵ�������
}

/**
  * @brief  ���ò���Ƶ��.
  * @param  rate:		����Ƶ��
  * @retval None 
  */
void ADS1256_SetSampleRate(uint16_t rate)
{
	uint32_t mr_num;
	mr_num = CLK_TIMER0/rate;				//����ڶ�ʱ��0ʱ��ΪCLK_TIMER0������£�ÿ���ж�ȡAD��������SFreq��ʱ������Ҫ���õ�MR0ֵ
	Init_MR_Timer(0,mr_num);					//��ʼ����ʱ��0��ƥ�书��
}

/**
  * @brief  ���ò�������.
  * @param  GAIN:		��������
  * @retval None 
  */
void ADS1256_SetSampleGain(uint8_t GAIN)
{
	char temp_spiByte[4];
	temp_spiByte[0] = (ADS1256_CMD_WREG | (ADS1256_ADCON & 0xF));
	temp_spiByte[1] = 0;
	temp_spiByte[2] = GAIN;
	temp_spiByte[3] = ADS1256_CMD_SELFCAL;
	Send_SPI(&pt_spi_tra_cbuffer,temp_spiByte,4,SPI_VALID_TRUE);
	Delay_ads1256(30000);
}

/**
  * @brief  Initialize the ADS1256.
  * @param  channel:	����ͨ��
	*					gain:			��������
	*					rate:	����Ƶ��
  * @retval None 
  */
void ADS1256_Init(char channel,char gain,uint16_t rate)
{
	char temp_Byte;
	//����spi���λ�����
	Define_CircleBuff_CirclebufLen(&pt_spi_rec_cbuffer,spi_rec_cbuffer,100);
	Define_CircleBuff_CirclebufLen(&pt_spi_tra_cbuffer,spi_tra_cbuffer,100);
	SPI_Init(SPI_MASTER, SPI_CPHA_SECOND, SPI_CPOL_HIGH, SPI_LSBF_MSB, SPI_INT_ENABLE);
	
	ADS1256_SetSampleRate(rate);							//ʹ�ö�ʱ��0���ж�ʱ����Ƶ��
	
	AdsIo_Init();																		//���ų�ʼ��
	SPI_CE_Low();																		//��ʼ��������Ƭѡ�źţ�ѡ��ADоƬ
	ADS_REST_High();																//	���͸�λ����
	Delay_ads1256(5000);
	ADS1256WREG(ADS1256_STATUS,0x04);               // ��λ��ǰ��У׼
	ADS1256WREG(ADS1256_MUX,channel);                  // ��ʼ���˿�
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_3750SPS); // ADS1256оƬ����10sps
	temp_Byte = ADS1256_CMD_SELFCAL;
	Send_SPI(&pt_spi_tra_cbuffer,&temp_Byte,1,SPI_VALID_TRUE);
	Delay_ads1256(5000);
	ADS1256_SetSampleGain(gain);
}

/**
  * @brief  ��ʼ����.
  * @param  None
  * @retval None 
  */
void ADS1256_startSample(void)
{
	Enable_Timer(0);									//������ʱ��0����ʼ����
}

/**
  * @brief  ֹͣ����.
  * @param  None
  * @retval None 
  */
void ADS1256_stopSample(void)
{
	Disable_Timer(0);
}

/**
  * @brief  �任����ͨ��.
  * @param  toChannel: Ŀ��ͨ��
  * @retval None 
  */
void ADS1256_changeCH(char toChannel)
{
	char temp_spiByte[3];
	temp_spiByte[0] = (ADS1256_CMD_WREG | (ADS1256_MUX & 0xF));
	temp_spiByte[1] = 0;
	temp_spiByte[2] = toChannel;
	Send_SPI(&pt_spi_tra_cbuffer,temp_spiByte,3,SPI_VALID_TRUE);
}

/**
  * @brief  �ɼ���ǰһ��ͨ��������.
  * @param  None
  * @retval None 
  */
void ADS1256_sampleOneCH(void)
{
	char temp_spiByte[6];
	temp_spiByte[0] = ADS1256_CMD_SYNC;
	temp_spiByte[1] = ADS1256_CMD_WAKEUP;
	temp_spiByte[2] = ADS1256_CMD_RDATA;
	temp_spiByte[3] = Rec_FlagByte;
	temp_spiByte[4] = Rec_FlagByte;
	temp_spiByte[5] = Rec_FlagByte;
	Send_SPI(&pt_spi_tra_cbuffer,temp_spiByte,6,SPI_VALID_TRUE);
}

/**
  * @brief  ��ȡADS1256��������.
  * @param  DestinationAddress��	Address of Destination
	*					len:									Length of buffer
  * @retval None 
  */
uint8_t ADS1256_ReadSampleData(char *DestinationAddress, int len)
{
	int reLEN = 0;
	if(SPI_RecByteNum >= len)						//���SPI���ܻ������е��ֽ������ڵ�����Ҫ��ȡ���ֽ���
	{
		reLEN = ReadDataFromCirBuff(&pt_spi_rec_cbuffer, DestinationAddress, len);	//��SPI�������ж�ȡ����
		SPI_RecByteNum = SPI_RecByteNum-reLEN;
		return reLEN;
	}
	else
	{
		return 0;
	}
}

/********************************* End Of File ********************************/
