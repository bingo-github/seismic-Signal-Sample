#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "circlebuff.h"


//环形缓存操作函数
/********************************************************************************************************
** Function name:       int Define_CircleBuff_CirclebufLen(ST_CircleBuff *pt_cb,char *SourceAddress,int len)
** Descriptions:	     定义缓冲区长度	   
pt_cb，要定义的缓冲区
SourceAddress  缓冲区数据存放的地址 
len，缓冲区长度
** Returned value:		CircleBuff_overflow  操作成功
**********************************************************************************************************/ 
int Define_CircleBuff_CirclebufLen(PST_CircleBuff pt_cb,char *SourceAddress,int len)
{
	pt_cb->CirclebufLen=len;
	pt_cb->Circlebuf=SourceAddress;
	return operate_Successful;
}
/********************************************************************************************************
** Function name:       int	WriteDataToCirBuff(ST_CircleBuff *pt_cb, char *SourceAddress, int len)
** Descriptions:	     向缓冲区C中写入数据	   
pt_cb，需写入数据的缓冲区C
SourceAddress，待写入数据源地址
len，待写入数据长度
** Returned value:			实际写入缓冲区C的数据长度
CircleBuff_overflow：发送缓冲区溢出
**********************************************************************************************************/
int	WriteDataToCirBuff(PST_CircleBuff pt_cb, char *SourceAddress, int len)
{
	int    PutNum,j;
	PutNum = 0;
	
	if (pt_cb->TXNum <= (pt_cb->CirclebufLen - 1))  // 发送缓冲区未满
	{
		for(j=0;j<len;j++)
		{			   
			pt_cb->TXNum++;
			pt_cb->Circlebuf[pt_cb->tail]=SourceAddress[j];
			pt_cb->tail++;
			PutNum=j+1;
			if((pt_cb->tail == pt_cb->CirclebufLen )&&(pt_cb->TXNum <= (pt_cb->CirclebufLen - 1)))//判断是不是到缓冲区的尾部      
			{
				pt_cb->tail = 0;
			}
			if(pt_cb->TXNum>(pt_cb->CirclebufLen - 1)||(pt_cb->tail==pt_cb->head))
			{
				break;
			}
		}	  
		return PutNum;		    // 放入缓冲区成功
	}else                                                 // 发送缓冲区已满
	{
		return CircleBuff_overflow;  // 发送缓冲区溢出
	}

}
/********************************************************************************************************
** Function name:          int	ReadDataFromCirBuff(ST_CircleBuff *pt_cb, char *DestinationAddress, int len)
** Descriptions:         	pt_cb，需读出数据的缓冲区C
*                	        DestinationAddress，读出数据放置的数据源地址
*                      	len，需读出数据的长度
** Returned value:			实际读出缓冲区C的数据长度
CircleBuff_empty：接收缓冲区为空
**********************************************************************************************************/
int	ReadDataFromCirBuff(PST_CircleBuff pt_cb, char *DestinationAddress, int len)
{
	int  RemoveNum,n,TX_CircleBuff_Num;
	RemoveNum = 0;
	
	TX_CircleBuff_Num=((pt_cb->tail-pt_cb->head + pt_cb->CirclebufLen)% pt_cb->CirclebufLen);
	if(pt_cb->TXNum==0 && TX_CircleBuff_Num > 0)				//此处存在问题，在头尾一样的情况下怎么处理
	{
		pt_cb->TXNum = TX_CircleBuff_Num;
	}
	if (pt_cb->TXNum > 0)  //发送缓冲区有数据
	{	 
		for(n=0;n<len;n++)
		{
			DestinationAddress[n]=pt_cb->Circlebuf[pt_cb->head];
			pt_cb->TXNum--;
			pt_cb->head++;
			RemoveNum = n+1;
			if (pt_cb->head == pt_cb->CirclebufLen )//判断是不是到缓冲区的尾部      
			{
				pt_cb->head = 0;
			}
			if((pt_cb->TXNum==0)||(pt_cb->head==pt_cb->tail))
			{
				pt_cb->tail=0;
				pt_cb->head=0;
				break;
			} 	
		}	
		return RemoveNum;		    // 取出数据成功
	}else                                // 缓冲区为空
	{
		return CircleBuff_empty;  
	}
}

/********************************************************************************************************
** Function name:        int GetFreeLengthOfCirBuff(ST_CircleBuff *pt_cb)
** Descriptions:         	获得缓冲区C空余数据长度
*                           pt_cb，需写入数据的缓存区                                                
** Returned value:		     该缓冲区的空余长度

*********************************************************************************************************/
int GetFreeLengthOfCirBuff(PST_CircleBuff pt_cb)
{
	int CirBuffNum;
	CirBuffNum =((pt_cb->tail-pt_cb->head + pt_cb->CirclebufLen)% pt_cb->CirclebufLen);
	return CirBuffNum;
}
