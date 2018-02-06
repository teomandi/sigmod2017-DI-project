
#include "top_k.h"

Array_Cntr ArrayCntr_Init()
{
	Array_Cntr topk;
	topk.array=malloc(sizeof(Word_Cntr)*VALUES);
	topk.index=0;
	topk.size=VALUES;

	return topk;
}


Word_Cntr WordCntr_Init(char* word)
{
	Word_Cntr  wc;
	wc.word=Char_Init(word);
	wc.counter=1;

	return wc;
}



//dhmiourgei tis domes pou xrisimopoiountai
void init_TopK()
{   
    //arxikopoihseis gia to array kai to hashtable
    int i;
    for(i=0; i<THREADS; i++){
        topk[i]=ArrayCntr_Init();
    }
    final= ArrayCntr_Init();

    //arxikopoihseis gia to minheap
    minHeap.heap=NULL;

    //arxikopoihseis gia to string me ta topk
    topk_results.value = malloc(TEXT_SIZE);
    topk_results.size = 0;
    topk_results.max_size = TEXT_SIZE;
}



void destroy_TopK(){
    int i;
    for(i=0; i<THREADS; i++){
       if(topk[i].array)free(topk[i].array);
    }
    if(final.array)free(final.array);
    if(minHeap.heap)free(minHeap.heap);
    if(topk_results.value)free(topk_results.value);
}



//kanei free ta Char kai ta positions tou hashtable
void clean_TopK(){
    int i,j;
    for(i=0; i<THREADS; i++){
        for(j=0; j<topk[i].index; j++){   
            if(topk[i].array[j].word.word_1){
                free(topk[i].array[j].word.word_1);
                topk[i].array[j].word.word_1=NULL;
            }
        }
        topk[i].index=0;
        memset(topk[i].array, 0, topk[i].size*sizeof(Word_Cntr));
    }

    for(j=0; j<final.index; j++){
        if(final.array[j].word.word_1){
            free(final.array[j].word.word_1);
            final.array[j].word.word_1=NULL;
        }
    }
    memset(final.array, 0, final.size*sizeof(Word_Cntr));
    final.index=0;
}





/*sunthrei to minheap...leitourgei anadromika
kai fernei to mikrotero sth riza..an vrethoun me ises
times dialegei to megalutero alfarithmitiko..*/
void heapify(Word_Cntr **minHeap, int i, int size)
{
	int left = 2*i + 1 ;
	int right = 2*i + 2;
	int smallest = i;

	if(left < size)
	{  
		if(minHeap[left]->counter < minHeap[i]->counter)
			smallest=left;
		else if(minHeap[left]->counter == minHeap[i]->counter)
		{	
			if(minHeap[left]->word.word_1)
			{
				if(minHeap[i]->word.word_1)
				{   
					if(strcmp(minHeap[left]->word.word_1, minHeap[i]->word.word_1) > 0)
						smallest=left;
				}
				else
				{   
					if(strcmp(minHeap[left]->word.word_1, minHeap[i]->word.word_2) > 0)
						smallest=left;
				}
			}
			else
			{
				if(minHeap[i]->word.word_1)
				{   
					if(strcmp(minHeap[left]->word.word_2, minHeap[i]->word.word_1) > 0)
						smallest=left;
				}
				else
				{   
					if(strcmp(minHeap[left]->word.word_2, minHeap[i]->word.word_2) > 0)
						smallest=left;
				}	
			}
		}
	}
	if(right < size)
	{	
		if(minHeap[right]->counter < minHeap[smallest]->counter)
			smallest = right;

		else if(minHeap[right]->counter == minHeap[smallest]->counter)
		{ 
			if(minHeap[right]->word.word_1)
			{
				if(minHeap[smallest]->word.word_1)
				{
					if(strcmp(minHeap[right]->word.word_1, minHeap[smallest]->word.word_1) > 0)
						smallest=right;
				}
				else
				{
					if(strcmp(minHeap[right]->word.word_1, minHeap[smallest]->word.word_2) > 0)
						smallest=right;
				}
			}
			else
			{
				if(minHeap[smallest]->word.word_1)
				{
					if(strcmp(minHeap[right]->word.word_2, minHeap[smallest]->word.word_1) > 0)
						smallest=right;
				}
				else
				{  
					if(strcmp(minHeap[right]->word.word_2, minHeap[smallest]->word.word_2) > 0)
						smallest=right;
				}	
			}
		}
	}

	if(smallest != i){
		
		Word_Cntr *temp = minHeap[i];
		minHeap[i] = minHeap[smallest];
		minHeap[smallest] = temp;

		
		heapify(minHeap, smallest, size);
	}
}



/*arxika kanei merge ta array kai sthn sunexeia dhmiourgei
enan swro megethous k opou ton gemizei me tis k prwtes theseis
tou telikou array...sthn sunexeia ton kanei heapify kai meta sugkrinei
ola ta ypoloia stoixeia tou pinaka me thn riza tou swrou...an vrethei
ngram me megalutero counter( h iso kai mikroterh lejh) tote to kanei auto 
riza kai janakalei jeapify

sto telos o swros periexei ta topk kai gemizoume ena string anapoda kai to
ektupwnoume*/
void calculate_TopK(int k){	
	
    Merge_Arrays();

    if(minHeap.heap==NULL)
    {
        minHeap.heap = malloc(sizeof(Word_Cntr*)*k);
        minHeap.size=k;
    }
    else if( minHeap.size < k)
    {
        minHeap.heap = realloc(minHeap.heap, sizeof(Word_Cntr*)*k);
        minHeap.size=k;
    }
    memset(minHeap.heap, 0, minHeap.size * sizeof(Word_Cntr*));

    /*se auto to shmeio exei dhmiourghthei ena array (topk.array) me thn kathe lejh kai
    kai to poses fores exei katametrithei */
    
    //dhmiourgei ena minHeap megethous k
    int j;
    if(final.index<k)k=final.index;
    if(k>0){
        for(j=0; j<k; j++){
            minHeap.heap[j] = &final.array[j];
        }
        for(j= k/2 - 1; j >= 0; j--)
            heapify(minHeap.heap, j, k);

        /*gemizei to min heap...h riza periexei to mikrotero..
        an einai megalutero apo thn riza bainei sth thesh ths kai ekteleietai
        heapify...
        an exoun idia timh paei me vash thn lejh*/              
        for(j=k; j<final.index; j++)
        { 	
            if(minHeap.heap[0]->counter < final.array[j].counter)
            {
            	minHeap.heap[0] = &final.array[j];
                heapify(minHeap.heap, 0, k);
            }
            else if(minHeap.heap[0]->counter == final.array[j].counter)
            {
                if(minHeap.heap[0]->word.word_1)
                {
                	if(final.array[j].word.word_1)
                    {
                    	if(strcmp(minHeap.heap[0]->word.word_1, final.array[j].word.word_1) > 0){
                        	minHeap.heap[0] = &final.array[j];
                        	heapify(minHeap.heap, 0, k);
                        }
                    }
                    else
                    {
                        if(strcmp(minHeap.heap[0]->word.word_1, final.array[j].word.word_2) > 0){
                        	minHeap.heap[0] = &final.array[j];
                        	heapify(minHeap.heap, 0, k);
                      	}
                    }
                }
                else
                {
                   	if(final.array[j].word.word_1)
                   	{
                       	if(strcmp(minHeap.heap[0]->word.word_2, final.array[j].word.word_1) > 0){
                       		minHeap.heap[0] = &final.array[j];
                       		heapify(minHeap.heap, 0, k);
                       	}
                    }
                
                	else
                	{
                  		if(strcmp(minHeap.heap[0]->word.word_2, final.array[j].word.word_2) > 0){
                      		minHeap.heap[0] = &final.array[j];
                       		heapify(minHeap.heap, 0, k);
                   		} 
                	}
            	}
        	}
        }

        /*exei diamorfwthei o swros
    	
        diamorfwnei to string pou tha ektupwthei

        to heap periexei to mikrotero sthn riza opote kathe fora ginetai
        pop apo thn riza kai gemizei to string anapoda*/

       
        for(j=k-1; j>=0; j--)
        {
        	int length;
            if(minHeap.heap[0]->word.word_1)
            {
                length = strlen(minHeap.heap[0]->word.word_1);
                if(length + 1 + topk_results.size >= topk_results.max_size)
                {
                	topk_results.value = realloc(topk_results.value, 2*topk_results.max_size);
                    topk_results.max_size = 2*topk_results.max_size;
                }
                if(topk_results.size == 0)
                {
                	strncpy(topk_results.value, minHeap.heap[0]->word.word_1, strlen( minHeap.heap[0]->word.word_1) + 1);
                    topk_results.size = length + 1;
                }
                else
                {
                	memmove(topk_results.value + length+1, topk_results.value, topk_results.size);
                    memmove(topk_results.value + length, "|", 1);
                    memmove(topk_results.value, minHeap.heap[0]->word.word_1, length);
                    topk_results.size += length+1;
                }                 
            }
            else
            {
                length = strlen(minHeap.heap[0]->word.word_2);
                if(length + 1 + topk_results.size >= topk_results.max_size)
                {
            	    topk_results.value = realloc(topk_results.value, 2*topk_results.max_size);
                    topk_results.max_size = 2*topk_results.max_size;
                }
                if(topk_results.size == 0)
                {
                	strncpy(topk_results.value, minHeap.heap[0]->word.word_2, strlen( minHeap.heap[0]->word.word_2) + 1);
                    topk_results.size = length + 1;
                }
                else
                {
                    memmove(topk_results.value + length+1, topk_results.value, topk_results.size);
                    memmove(topk_results.value + length, "|", 1);
                    memmove(topk_results.value, minHeap.heap[0]->word.word_2, length);
                    topk_results.size += length+1;
                }                 
            } 
            minHeap.heap[0] = minHeap.heap[j];
            heapify(minHeap.heap, 0, j); 
        }

        //kanei print ta top
        printf("Top: %s\n",topk_results.value);

        //katharizei oti ekane ..gia na mhn exoun periexomeno thn epomenh fora
        memset(topk_results.value, 0, topk_results.max_size);
        topk_results.size=0;
    }

    
    clean_TopK();

}





/*kanei merge ola ta array tautoxrona..
arxika koitame se ola ta array thn thesh 0 kai psaxnoume to
pio mikro ngram me to pou to vroume karatame thn thesh tou 
kai to counter tou...an to janavroume athroizoume to counter
an vroume mikrotero tote kratame auth thn lejh kai ton counter

sto telos exei kraththei oi lejh kai to poses fores emfanisthke
opote to vazoume sthn katallhlh thesh tou telikou array(final)
kai se osous pinakes xrisimopoihsame thn lejh tous aujanoume 
ton deikth */
void Merge_Arrays(){

    int indexes[THREADS];               //o deikths tou kathe pinaka

    int *ptr_index[THREADS];            //deiktes stous deiktes twn pinakwn
    int size_ptr=0, counter=0, i=0;
    char *word=NULL;
    bool end_cond=false;

    memset(indexes, 0, sizeof(int)*THREADS);

    while(!end_cond){
        for(i=0;i<THREADS; i++){

            if(indexes[i] < topk[i].index){             
                bool word_c;
                if(topk[i].array[indexes[i]].word.word_1)
                    word_c=true;
                else 
                    word_c=false;

                if(!word){
                    //an den touxei dwthei lejh vazei thn prwth pou vrskei
                    if(word_c)
                        word=topk[i].array[indexes[i]].word.word_1;
                    else
                        word=topk[i].array[indexes[i]].word.word_2;
                    counter=topk[i].array[indexes[i]].counter;
                    ptr_index[0] = &indexes[i];
                    size_ptr=1;
                }

                else{//exei lejh kai psaxnei omoies h mikroteres
                    int comparison;
                   
                    if(word_c)
                        comparison=strcmp(word, topk[i].array[indexes[i]].word.word_1);
                    else
                        comparison=strcmp(word, topk[i].array[indexes[i]].word.word_2);
                    
                    if(comparison > 0){
                        if(word_c)
                            word=topk[i].array[indexes[i]].word.word_1;
                        else
                            word=topk[i].array[indexes[i]].word.word_2;

                        counter=topk[i].array[indexes[i]].counter;
                        ptr_index[0]=&indexes[i];
                        size_ptr=1;
                    }
                    else if(comparison == 0){
                        counter += topk[i].array[indexes[i]].counter;
                        ptr_index[size_ptr] = &indexes[i];
                        size_ptr++;

                    }
                }
            }
        }
        //an vrike lejh thn vazei ston teliko pinaka
        if(word){
            if(final.index == final.size){
                final.array = realloc(final.array, final.size*2*sizeof(Word_Cntr));
                final.size = final.size*2;
            }
            final.array[final.index]=WordCntr_Init(word);
            final.array[final.index].counter=counter;
            final.index++;

            word=NULL;
        }

        end_cond=true;
        for(i=0; i<THREADS; i++){
            if(i<size_ptr){
                (*ptr_index[i])++;
            }
            if(indexes[i] < topk[i].index){
                end_cond=false;
            }
        }
        size_ptr=0;
    }
}


/*kanei binary search ston pinaka..an to vrei to eisage*/
bool insert_ArrayCntr(char *ngram, int index){

    if(topk[index].index==0){
        topk[index].array[0]=WordCntr_Init(ngram); 
        topk[index].index++;
        return true;       
    }
    int pos=binarySearch(topk[index].array, topk[index].index, ngram);
    bool word_1;
    if(topk[index].array[pos].word.word_1)
        word_1=true;
    else
        word_1=false;
    bool condition;
    if(word_1)
        if(strcmp(ngram, topk[index].array[pos].word.word_1)==0)
            condition=true;
        else
            condition=false;
    else
         if(strcmp(ngram, topk[index].array[pos].word.word_2)==0)
            condition=true;
        else
            condition=false;

    if(condition){
        topk[index].array[pos].counter++;
        return true;
    }
    else{
        condition=true;
        if(word_1){
            if(pos!=topk[index].index-1){
                if(strcmp(ngram,topk[index].array[pos].word.word_1) >0)
                    pos++;
            }
            else{//einai sto telos
                if(strcmp(ngram, topk[index].array[pos].word.word_1)>0){
                    condition=false;
                    pos++; 
                }
            }
        }
        else{
            if(pos!=topk[index].index-1){
                if(strcmp(ngram, topk[index].array[pos].word.word_2)>0)
                    pos++;
            }
            else{ 
                if(strcmp(ngram, topk[index].array[pos].word.word_2)>0){
                    condition=false;
                    pos++;
                }
            }
        }
        /*  an den uparxei xwros kanei realloc
            an vrike megalutero tote to vazei sth thesh pos kai ta upoloipa ta
            kanei shift mia thesh...alliws to vazei sto telos            */
        if(topk[index].index >= topk[index].size){
            (topk[index].array) = realloc(topk[index].array, 2*topk[index].size * sizeof(Word_Cntr));
            topk[index].size= 2*topk[index].size;
        }

        
        if(condition){ 
            memmove(&(topk[index].array[pos+1]), &(topk[index].array[pos]), (topk[index].index-pos)*sizeof(Word_Cntr));
            topk[index].array[pos] = WordCntr_Init(ngram);           
            topk[index].index++;   
        }
        else{           
            topk[index].array[pos] = WordCntr_Init(ngram);    
            topk[index].index++;
        }
    }
    return true;
}




int binarySearch(Word_Cntr *array, int size, char *searchVal){
    int low = 0;
    int high = size-1;
    int mid ;

    if(size == 0) return 0;

    while(low <= high){
        mid = (low+high)>>1;
        if(array[mid].word.word_1){
            if(strcmp(searchVal,array[mid].word.word_1)==0)
                return mid;
            else if(strcmp(array[mid].word.word_1, searchVal)<1)
                low = mid+1;
            else
                high = mid-1;
        }
        else{
            if(strcmp(searchVal, array[mid].word.word_2)==0)
                return mid;
            else if(strcmp(array[mid].word.word_2, searchVal)<1)
                low = mid+1;
            else
                high = mid-1;
        }
    }
    return mid;
}


