#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef Q

#define Q 1

typedef int bool;
#define true 1
#define false 0


typedef struct Job{
	void* (*job)(void* , int);
	void* argument;
	int priority ;
	struct Job* next;
}Job;


Job* create_Job(void* (*f)(void* , int), void* arg, int priority);

Job* create_AnyJob(void* arg);

typedef struct Queue{
	Job* first;
	Job* last;
	int size;
	int no_jobs;
}Queue;


Queue* createQueue();
bool emptyQueue(Queue* q);
void insertQueue(Queue* q, Job *node);
Job* pop(Queue* q);
void destroyQueue(Queue* q);


char* print(int k);

#endif