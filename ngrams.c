
#include "hashtable/hashtable.h"
#include "job_scheduler/JobScheduler.h"


int main(int argc, char *argv[]){
  
    clock_t start, end;
    time_t start_t, end_t;
    double cpu_time_used;

  	start = clock();
    start_t=time(0);

  	char* init_file=NULL;
  	char* query_file=NULL;
  
  	FILE *fp;
    char* line;
  	size_t len = 0;
  	ssize_t read;

  	/*  kanei parsing ta dedomena pou
   	dexetai apto command_line */
 	 if(argc>1){
    	int i=0;
    	for(i=0;i<argc;i++){
        	if(argv[i][0]=='-'){
            	if(argv[i][1]=='i'){
              		if (i==argc-1 || argv[i+1][0]=='-'){ 
                		printf("Error input in Command line.\n");
                  		return 0;
                	}
                	init_file =malloc(sizeof(char)*(strlen(argv[i+1])+1));
                	strncpy(init_file ,argv[i+1],strlen(argv[i+1])+1);
            	}
            	else if(argv[i][1]=='q'){
              	if (i==argc-1 || argv[i+1][0]=='-'){ 
                	printf("Error input in Command line.\n");
                  	return 0;
                	}
                	query_file =malloc(sizeof(char)*(strlen(argv[i+1])+1));
                	strncpy(query_file ,argv[i+1],strlen(argv[i+1])+1);
            	}
            }
      	}
    }
    /*
    sunexizei an exei dwthei init_file
    dhmiourgei to hash-dentro kai diavazei mia mia tis grammes
    kai tis kanei insert sto hash-trie..
    
    ektelei to init
    */
    Hashtable* ht = createHashTable();
    int current_version=0;
 	bool static_trie=false;

  	if(init_file){
    	fp = fopen(init_file, "r");
    	if(fp==NULL){
        	printf("There is no init_file named %s\n", init_file);
        	exit(1);
    	}        
    	line = NULL;     
    	len = 0;    
	    int i=0;
    	read = getline(&line, &len, fp);
    	if(strcmp(line, "STATIC\n")==0)
	        static_trie=true;
		while ((read = getline(&line, &len, fp)) != -1) {   
            int line_len=strlen(line);     
      		if(line[line_len-1]=='\n')
        		line[line_len-1]='\0';
		    insertHashTable(ht, line, current_version);
    	}
    	fclose(fp);
        if(line)free(line);
  	}

    //an einai statiko to dentro tote to sumpiezei
  	if(static_trie)
    	Compress2(ht);

    /* an exei dwthei query_file tote diavazei mia mia 
    tis grammes kai ektelei ta erwthmata!*/

 	if(query_file){
    	fp = fopen(query_file, "r");
    	if(fp==NULL){
      		printf("There is no file named %s\n", query_file);
      		exit(1);
    	}

        
        JobScheduler* JS= initialize_scheduler(THREADS);        //o job scheduler
        int priority=0;                                         //to priority pou thaxei to kathe Q 
        Queue* d_queue = createQueue();                         //h oura pou tha bainoun oi komboi gia na diagraftoun
    	bool was_Q=false;                                       //an einai true allazoyme to current_version
        init_TopK();                                            //arxikopoihsh gia to topk
      

        line = NULL;      
    	len = 0;
    	while ((read = getline(&line, &len, fp)) != -1) {    
            int line_len=strlen(line);
            if(line[line_len-1]=='\n')
        		line[line_len-1]='\0';
                char action= line[0];
                
                if(action=='A'){
                    if(was_Q){
                        current_version++;
                        was_Q=false;
                    }
                    insertHashTable(ht, line+2, current_version);
                }

          		else if(action=='Q'){
                    was_Q=true;
                    priority++; 
                    Argument* argument = create_Argument(ht, line+2, line_len-2, current_version, static_trie);
                    Job *job = create_Job((void*)FindHashTable, (void*)argument, priority);
                    submit_job(JS, job);
                }


                /*arxika kaleitai h deleteHashTable_Versioning h opoia
                allazei to D_version kai epistrefei true an vrethike to ngram
                tote to vazei se mia oura opou meta tha ginwntai delete */
        		else if(action=='D'){
                    if(was_Q){
                        current_version++;
                        was_Q=false;
                    }
                    
                    char *ngram=malloc(line_len-1);
                    memcpy(ngram, line+2, line_len-1);   
                    if(deleteHashTable_Versioning(ht, line+2, current_version)){
                        Job* job = create_AnyJob((void*) ngram);
                        insertQueue(d_queue, job);
                    }
                    else
                        free(ngram);
                }


          		else if(action=='F')
          		{	 
                    JS->q->no_jobs=priority;
                    execute_all_jobs(JS);
                    wait_all_tasks_finish(JS);	
                    int i;
                    for(i=0;i<JS->results.size; i++){
                        printf("%s\n",JS->results.array[i]);
                        free(JS->results.array[i]);
                    }
                    JS->results.size=0;
                    priority=0;


                    /*kanei delete ta ngram pou eginan delete mesa sthn riph*/
                   while(!emptyQueue(d_queue)){
                        Job *job = pop(d_queue);
                        deleteHashTable(ht, (char*)job->argument);
                        free((char*)job->argument);
                        free(job);
                    }

                    int k=atoi(line+2);
    	          	calculate_TopK(k);
             
            
      		}
    	}
    	
        //apodesmeuontai 
    	if(line)free(line);
    	destroy_TopK();
        destroyQueue(d_queue);
    	fclose(fp);
        destroy_JobScheduler(JS);
	}
  	destroyHashTable(ht);

  	if(init_file)free(init_file);
  	if(query_file)free(query_file);

  	end_t=time(0);
    end=clock();
    double cpu_time_used_2= ((double)(end - start)/CLOCKS_PER_SEC);
  	cpu_time_used = ((double)(end_t - start_t));
  	printf("Seconds: %f\nCPU Time  %f \nThreads: %d\n", cpu_time_used, cpu_time_used_2,THREADS );


  	return 0;
}