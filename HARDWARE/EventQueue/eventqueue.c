#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eventqueue.h"

///*******************************************************************************************
//** Function name:       void Queue_Init(EventQueue* me)
//** Descriptions:	      初始化队列   
//** Input Parameters: 
//**                      me 需要检查的事件队列
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
** Descriptions:	    检查事件队列是否为满	   
** Input Parameters: 
**                      me 需要检查的事件队列
** Returned value:		0-队列未满；1-队列已满
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
** Descriptions:	    检查事件队列是否为空	   
** Input Parameters: 
**                      me 需要检查的事件队列
** Returned value:		0-队列非空；1-队列已空
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
** Descriptions:	    将事件插入队列	   
** Input Parameters: 
**                      me 需要检查的事件队列；e 事件指针，保存将要插入队列的事件信息
** Returned value:		0-队列满，无法插入新事件；1-事件已成功加入到队列中
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
** Descriptions:	    从事件队列中获得最近事件	   
** Input Parameters: 
**                      me 需要检查的事件队列；e 事件指针，用于保存所取得的事件信息
** Returned value:		0-队列空，无返回数据；1-将正确数据放置到e中
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
