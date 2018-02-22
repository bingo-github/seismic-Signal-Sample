#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "circlebuff.h"


//���λ����������
/********************************************************************************************************
** Function name:       int Define_CircleBuff_CirclebufLen(ST_CircleBuff *pt_cb,char *SourceAddress,int len)
** Descriptions:	     ���建��������	   
pt_cb��Ҫ����Ļ�����
SourceAddress  ���������ݴ�ŵĵ�ַ 
len������������
** Returned value:		CircleBuff_overflow  �����ɹ�
**********************************************************************************************************/ 
int Define_CircleBuff_CirclebufLen(PST_CircleBuff pt_cb,char *SourceAddress,int len)
{
	pt_cb->CirclebufLen=len;
	pt_cb->Circlebuf=SourceAddress;
	return operate_Successful;
}
/********************************************************************************************************
** Function name:       int	WriteDataToCirBuff(ST_CircleBuff *pt_cb, char *SourceAddress, int len)
** Descriptions:	     �򻺳���C��д������	   
pt_cb����д�����ݵĻ�����C
SourceAddress����д������Դ��ַ
len����д�����ݳ���
** Returned value:			ʵ��д�뻺����C�����ݳ���
CircleBuff_overflow�����ͻ��������
**********************************************************************************************************/
int	WriteDataToCirBuff(PST_CircleBuff pt_cb, char *SourceAddress, int len)
{
	int    PutNum,j;
	PutNum = 0;
	
	if (pt_cb->TXNum <= (pt_cb->CirclebufLen - 1))  // ���ͻ�����δ��
	{
		for(j=0;j<len;j++)
		{			   
			pt_cb->TXNum++;
			pt_cb->Circlebuf[pt_cb->tail]=SourceAddress[j];
			pt_cb->tail++;
			PutNum=j+1;
			if((pt_cb->tail == pt_cb->CirclebufLen )&&(pt_cb->TXNum <= (pt_cb->CirclebufLen - 1)))//�ж��ǲ��ǵ���������β��      
			{
				pt_cb->tail = 0;
			}
			if(pt_cb->TXNum>(pt_cb->CirclebufLen - 1)||(pt_cb->tail==pt_cb->head))
			{
				break;
			}
		}	  
		return PutNum;		    // ���뻺�����ɹ�
	}else                                                 // ���ͻ���������
	{
		return CircleBuff_overflow;  // ���ͻ��������
	}

}
/********************************************************************************************************
** Function name:          int	ReadDataFromCirBuff(ST_CircleBuff *pt_cb, char *DestinationAddress, int len)
** Descriptions:         	pt_cb����������ݵĻ�����C
*                	        DestinationAddress���������ݷ��õ�����Դ��ַ
*                      	len����������ݵĳ���
** Returned value:			ʵ�ʶ���������C�����ݳ���
CircleBuff_empty�����ջ�����Ϊ��
**********************************************************************************************************/
int	ReadDataFromCirBuff(PST_CircleBuff pt_cb, char *DestinationAddress, int len)
{
	int  RemoveNum,n,TX_CircleBuff_Num;
	RemoveNum = 0;
	
	TX_CircleBuff_Num=((pt_cb->tail-pt_cb->head + pt_cb->CirclebufLen)% pt_cb->CirclebufLen);
	if(pt_cb->TXNum==0 && TX_CircleBuff_Num > 0)				//�˴��������⣬��ͷβһ�����������ô����
	{
		pt_cb->TXNum = TX_CircleBuff_Num;
	}
	if (pt_cb->TXNum > 0)  //���ͻ�����������
	{	 
		for(n=0;n<len;n++)
		{
			DestinationAddress[n]=pt_cb->Circlebuf[pt_cb->head];
			pt_cb->TXNum--;
			pt_cb->head++;
			RemoveNum = n+1;
			if (pt_cb->head == pt_cb->CirclebufLen )//�ж��ǲ��ǵ���������β��      
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
		return RemoveNum;		    // ȡ�����ݳɹ�
	}else                                // ������Ϊ��
	{
		return CircleBuff_empty;  
	}
}

/********************************************************************************************************
** Function name:        int GetFreeLengthOfCirBuff(ST_CircleBuff *pt_cb)
** Descriptions:         	��û�����C�������ݳ���
*                           pt_cb����д�����ݵĻ�����                                                
** Returned value:		     �û������Ŀ��೤��

*********************************************************************************************************/
int GetFreeLengthOfCirBuff(PST_CircleBuff pt_cb)
{
	int CirBuffNum;
	CirBuffNum =((pt_cb->tail-pt_cb->head + pt_cb->CirclebufLen)% pt_cb->CirclebufLen);
	return CirBuffNum;
}
