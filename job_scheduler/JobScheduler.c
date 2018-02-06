#include "JobScheduler.h"




/*dhmiourgei k arxikopoiei ton jS*/
JobScheduler* initialize_scheduler(int execution_threads){
	int err;
	
	JobScheduler* JS = malloc(sizeof(JobScheduler));
	JS->q=createQueue();
	JS->tids=malloc(sizeof(pthread_t)*execution_threads);;
	JS->execution_threads=execution_threads;

	JS->results.array=malloc(sizeof(char*)*VALUES);
    JS->results.max=VALUES;
    JS->results.size=0;
	

	JS->occupied=true;
	JS->executed=-1;
	JS->final_task=false;
	JS->waitting=0;

	pthread_mutex_init(&JS->mtx_read, NULL);
	pthread_mutex_init(&JS->mtx_write, NULL);
	pthread_mutex_init(&JS->mtx_master, NULL);

	pthread_cond_init(&JS->c_read, NULL);
	pthread_cond_init(&JS->c_write, NULL);
	pthread_cond_init(&JS->c_master, NULL);
	

	/*ftiaxnoume ta threads k ta bazoume na trexoun thn routina job*/
	int i;
	for(i=0;i<JS->execution_threads;i++){
		JS->self=i;
		if(err=pthread_create(&JS->tids[i], NULL, JobRutine, (void*)JS)){
			perror2 ("pthread_create ", err);
			exit(1) ;
		}
	}
	return JS;
}



//topothetei job ston job scheduler
void submit_job(JobScheduler* JS,  Job* job){

	pthread_mutex_lock(&JS->mtx_read);
	insertQueue(JS->q, job);
	pthread_mutex_unlock(&JS->mtx_read);
}


/*ekkeinei ola ta threads gia na jekinhsoun thn douleia
tous.. epipleon kanei lock ton master mutex gia na prolavei
na pagwsei prin teleisoun thn douleia tous ta threads kai
pane na ton jupnhsoun prin koimithei..spanio alla mporei
na sumvei*/
void execute_all_jobs(JobScheduler* JS){
	pthread_mutex_lock(&JS->mtx_read);
	JS->occupied = false;
	JS->executed=0;

	pthread_cond_broadcast(&JS->c_read);
	pthread_mutex_lock(&JS->mtx_master);
	pthread_mutex_unlock(&JS->mtx_read);
	
}



/*pagwnei kai perimenei signal na jupnhsei
epipleon katalambainei to occupied etsi wste sigoura
ola ta threads na koimithoun*/
void wait_all_tasks_finish(JobScheduler* JS){
	
	pthread_cond_wait(&JS->c_master, &JS->mtx_master);
	pthread_mutex_unlock(&JS->mtx_master);
	
	pthread_mutex_lock(&JS->mtx_read);
	JS->occupied=true;
	pthread_mutex_unlock(&JS->mtx_read);
}



/*einai h loopa opou kathorizontai oi douleies ton threads
sthn arxh an exou kanei oles tis douleies jupnane ton master
meta an einai adeia h oura h katalhmenh tote egklovizontai
opote jupnhsoun vgainei enas pairnei thn douleia apo to queue
kai jupnaei ena allo thread...ektelei thn douleia tou kai sthn 
sunexeia paei kai to grafei sthn thesh tou pinaka pou prepei

an dwthei shma na katastrafoun ta threads me to pou jupnane 
mpainoun sto if(JS->final_task) kai kanoun exit */
void* JobRutine(void* js){
	JobScheduler* JS = (JobScheduler*)js;
	Job *job; 
	bool first_time=true;
	int self;
	while(1){
		pthread_mutex_lock(&JS->mtx_read);
		if(first_time){
			self=JS->waitting;
			first_time=false;
		}
		JS->waitting++;
		while(emptyQueue(JS->q) || JS->occupied){

			/*exoun teleiwsei thn douleia tous kai jupnane to main 
			thead*/
			pthread_mutex_lock(&JS->mtx_master);
			if(emptyQueue(JS->q) && JS->executed==JS->q->no_jobs ){;
				pthread_cond_signal(&JS->c_master);
			}
			pthread_mutex_unlock(&JS->mtx_master);

			/*hrthe shma na katastrafoun*/
			if(JS->final_task){
				pthread_mutex_unlock(&JS->mtx_read);
				pthread_exit(0);				
			}	
			pthread_cond_wait(&JS->c_read, &JS->mtx_read);
		}
		/*pairnei mia douleia..thn ektelei kai jupanei ena allo thread*/
		JS->waitting--;
		JS->occupied=true;
		pthread_mutex_unlock(&JS->mtx_read);

			
		job = pop(JS->q);

		pthread_mutex_lock(&JS->mtx_read);
		JS->occupied=false;
		if(!emptyQueue(JS->q)) pthread_cond_signal(&JS->c_read);
		pthread_mutex_unlock(&JS->mtx_read); 
		
		/*an touxe anatethei douleia paei kai thn grafei sth thesh tou
		pinaka pou tou anhkei*/
		if(job){
			
			pthread_mutex_lock(&JS->mtx_write);
			if(job->priority-1 >= JS->results.max){
				JS->results.array = realloc(JS->results.array, sizeof(char*)*JS->results.max*4);
				JS->results.max = JS->results.max*4;
			}
			pthread_mutex_unlock(&JS->mtx_write);
			
			char* temp=(char*) (job->job)(job->argument, self );

			JS->results.array[job->priority-1]=temp;
	
			pthread_mutex_lock(&JS->mtx_write);
			JS->executed++;
			JS->results.size++;
			pthread_mutex_unlock(&JS->mtx_write);
			

			free(job);
			job=NULL;	
		}
	}
}




/*jupnaei ola ta threads kai ta katastrefei ena-ena*/
void  destroy_JobScheduler(JobScheduler* JS){
	int err , i;

	pthread_mutex_lock(&JS->mtx_read); 
	JS->final_task=true;
	JS->occupied=true;
	pthread_cond_broadcast(&JS->c_read);
	pthread_mutex_unlock(&JS->mtx_read); 

	for(i=0; i<JS->execution_threads; i++){
		if (err = pthread_join(JS->tids[i], NULL)) { 
			perror2("pthread_join ", err); 
			exit(1);
		}
	}
	destroyQueue(JS->q);
	free(JS->results.array);
	free(JS->tids);
	free(JS);
}

