#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "Queue/queue.h"
#include "../structures/structures.h"


#define perror2(s ,e) fprintf(stderr, "%s: %s \n", s, strerror(e))



typedef struct JobScheduler{
	int execution_threads;
	Queue* q;
	pthread_t* tids;
	String_Array results;
	int self;
	
	/*diafores metavlhtes pou vohthane ston elegxo ths rohs
	tou jobscheduler*/
	bool occupied;
	int executed;
	bool final_task;
	int waitting;	

	
	/*gia otan kapoios paei na diavasei apo
	thn oura h paei na diavasei sugkekrimenes
	metavlhtes*/
	pthread_cond_t c_read; 
	pthread_mutex_t mtx_read;
	
	/*otan kapoion paei na peirajei
	to executed..pou leei poses douleis
	exoun ginei*/
	pthread_cond_t c_write;
	pthread_mutex_t mtx_write;

	/*pagwnei ton master pou perhmenei
	ta threads na kanoun thn douleia tous*/
	pthread_cond_t c_master;
	pthread_mutex_t mtx_master;


}JobScheduler;


JobScheduler* initialize_scheduler(int execution_threads);

void submit_job(JobScheduler* js ,  Job* job);

void* JobRutine(void* js);
void  destroy_JobScheduler(JobScheduler* JS);

void execute_all_jobs(JobScheduler* JS);
void wait_all_tasks_finish(JobScheduler* JS); 