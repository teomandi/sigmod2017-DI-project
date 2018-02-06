#include "queue.h"


/*dimiourgei oura k epistrefei dikti se authn*/
Queue* createQueue(){
	Queue* q = malloc(sizeof(Queue));
	q->first=NULL;
	q->last=NULL;
	q->size=0;
	q->no_jobs=0;
	return q;
}

/*epistrefei true(1) and h oura dn einai adeia alleiws false(0)*/
bool emptyQueue(Queue* q){
	return(q->size == 0);
}




Job* create_Job(void* (*f)(void* ,int), void* arg, int priority){
	Job *node = malloc(sizeof(Job));
	node->next=NULL;
	node->job = f; 
	node->argument=arg;
	node->priority=priority ;
	return node;
}


Job* create_AnyJob(void* arg){
	Job *node = malloc(sizeof(Job));
	node->next=NULL;
	node->argument=arg;

	return node;
}



/*eisagei sto telos tis ouras ena neo work me protereotita prty afou 
ftia3ei prwta to qNode*/ 
void insertQueue(Queue* q, Job* node){

	/*eisagogh stin oura*/
	if(emptyQueue(q)){
		q->first = node;
		q->last = node;
	}
	else{
		q->last->next = node;
		q->last = node;
	}
	q->size ++;
	q->no_jobs++;
}



/*epistrefei k afairei ton prwto kombo. PROSOXI dn ginetai free*/
Job* pop(Queue* q){
	if(emptyQueue(q)){	
		printf("ERROR: Queue is empty.\n");
		return NULL;
	}
	Job* temp = q->first;
	q->first = q->first->next;
	q->size--;
	return temp;
}

/*katastrefei apodesmeuontas olh thn mnimi tis ouras*/
void destroyQueue(Queue* q){
	Job* temp;
	while(!emptyQueue(q)){
		temp = q->first;
		q->first = temp->next;
		free(temp);
		q->size--;
	}
	free(q);
}




char* print(int k){
	char *re=malloc(70);
	sprintf(re, "*JOB* %d\n",k);
	return re;
}




/*void printQueue(Queue* q){
	printf("QUEUE:\n");
	qNode *temp=q->first;
	while(temp != q->last){
		printf("%s| %d\n", temp->work, temp->priority);
		temp=temp->next;
	}
	printf("%s| %d\n", temp->work, temp->priority);
}
*/