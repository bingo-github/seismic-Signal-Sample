#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eventqueue.h"

///*******************************************************************************************
//** Function name:       void Queue_Init(EventQueue* me)
//** Descriptions:	      ��ʼ������   
//** Input Parameters: 
//**                      me ��Ҫ�����¼�����
//** Returned value:		
//*********************************************************************************************/ 
void Queue_Init(EventQueue* me)
{
	me->head	=0;
	me->tail=0;
	me->size=0;
}
/*******************************************************************************************
** Function name:       EventQueue_isfull
** Descriptions:	    ����¼������Ƿ�Ϊ��	   
** Input Parameters: 
**                      me ��Ҫ�����¼�����
** Returned value:		0-����δ����1-��������
*********************************************************************************************/ 
int	EventQueue_isfull(EventQueue* me)
{
	if(me->size<QUEUE_SIZE)
	{
		return(1);
	}
	else{
		return(0);
	}
}

/*******************************************************************************************
** Function name:       EventQueue_isempty
** Descriptions:	    ����¼������Ƿ�Ϊ��	   
** Input Parameters: 
**                      me ��Ҫ�����¼�����
** Returned value:		0-���зǿգ�1-�����ѿ�
*********************************************************************************************/ 
int EventQueue_isempty(EventQueue* me)
{
	if (me->head == me->tail)
	{
		return (1);
	}else{
		return (0);
	}
}

/*******************************************************************************************
** Function name:       EventQueue_insert
** Descriptions:	    ���¼��������	   
** Input Parameters: 
**                      me ��Ҫ�����¼����У�e �¼�ָ�룬���潫Ҫ������е��¼���Ϣ
** Returned value:		0-���������޷��������¼���1-�¼��ѳɹ����뵽������
*********************************************************************************************/ 
int	EventQueue_insert(EventQueue* me, Event* e)
{
	if (!EventQueue_isfull(me))
	{
		return 0;
	}
	me->itsevent[me->head].flag = e->flag;
	me->itsevent[me->head].param_ptr = e->param_ptr;
	me->head++;
	me->size++;
	if (me->head == QUEUE_SIZE)
	{
		me->head = 0;
	}
	if((me->size > (QUEUE_SIZE - 1))||(me->head == QUEUE_SIZE))
	{
		return (0);
	}
	return (1);
}

/*******************************************************************************************
** Function name:       EventQueue_get
** Descriptions:	    ���¼������л������¼�	   
** Input Parameters: 
**                      me ��Ҫ�����¼����У�e �¼�ָ�룬���ڱ�����ȡ�õ��¼���Ϣ
** Returned value:		0-���пգ��޷������ݣ�1-����ȷ���ݷ��õ�e��
*********************************************************************************************/ 
int	EventQueue_get(EventQueue* me, Event* e)
{
	if (EventQueue_isempty(me))
	{
		return (0);
	}

	e->flag = me->itsevent[me->tail].flag;
	e->param_ptr = me->itsevent[me->tail].param_ptr;
	me->tail++;
	me->size--;
	if (me->tail == QUEUE_SIZE)
	{
		me->tail = 0;
	}
	if(me->tail==me->head)
	{
		me->tail=0;
		me->head=0;
	}
	return (1);
}
