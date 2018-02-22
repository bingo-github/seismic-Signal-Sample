#ifndef __CIRCLEBUFF_H
#define __CIRCLEBUFF_H

#define USE_SPI_CIECLEBUFFER								//在SPI文件中使用环形缓冲区

// #define   ture                  0x01
// #define   false                 0x00
#define   operate_Successful    0x01	       //定义缓冲区长度成功
#define   CircleBuff_overflow	0x00	       //缓冲区溢出
#define   CircleBuff_empty      0x00		   //缓冲区为空
  
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
