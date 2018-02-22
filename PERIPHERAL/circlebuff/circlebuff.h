#ifndef __CIRCLEBUFF_H
#define __CIRCLEBUFF_H

#define USE_SPI_CIECLEBUFFER								//��SPI�ļ���ʹ�û��λ�����

// #define   ture                  0x01
// #define   false                 0x00
#define   operate_Successful    0x01	       //���建�������ȳɹ�
#define   CircleBuff_overflow	0x00	       //���������
#define   CircleBuff_empty      0x00		   //������Ϊ��
  
typedef struct {
	char *Circlebuf;
	volatile  unsigned int	CirclebufLen;
	volatile  unsigned int  TXNum;
	volatile  unsigned int  tail;
	volatile  unsigned int  head;
} ST_CircleBuff, *PST_CircleBuff;

int Define_CircleBuff_CirclebufLen(PST_CircleBuff pt_cb,char *SourceAddress,int len);
int	WriteDataToCirBuff(PST_CircleBuff pt_cb, char *SourceAddress, int len);
int	ReadDataFromCirBuff(PST_CircleBuff pt_cb,  char *DestinationAddress, int len);
int GetFreeLengthOfCirBuff(PST_CircleBuff pt_cb);

#endif
