#ifndef	__EVENTQUEUE_H
#define __EVENTQUEUE_H

#define   QUEUE_SIZE  500

typedef struct{
	unsigned short int	flag;
	void*	param_ptr;
}Event;

typedef struct{
	int head;
	int size;
	int tail;
  Event itsevent[QUEUE_SIZE];
}EventQueue;

int	EventQueue_insert(EventQueue* me, Event* e);
int	EventQueue_get(EventQueue* me, Event* e);
int	EventQueue_isfull(EventQueue* me);
void Queue_Init(EventQueue* me);

#endif
