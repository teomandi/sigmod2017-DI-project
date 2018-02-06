#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"


Hashtable* createHashTable(){ 				//M=number of buckets in hash, C=size of bucket.
	Hashtable* ht = malloc(sizeof(Hashtable));
	ht->standart_size = M;
	ht->Hcurrent_size = M;
	ht->p = 0;
	ht->round = 0;
	ht->standart_bucket_size = C; 
	ht->final=0;

	int i;
	ht->table = malloc(sizeof(Bucket)*M);

	for(i=0;i<M;i++){
		ht->table[i].children = malloc(sizeof(trie_node)*C);
		ht->table[i].max_size = C;
		ht->table[i].Bcurrent_size = 0;
	}
	return ht;
}

/*H insert dexete ena ngram to opoio to spaei k pernei thn prwth tou le3h.
Epeita to pernaei apo tin sunrthsh hashcode opou bgazei ena monadiko ari8mo 
pou thn antistoixei kai sth sunexeia me ton tupo tou modulo upologizei se pia 8esi 
sto hashtable mas 8a eisax8ei. Ekei briskei alfabitika thn swsti 8esi tou 
sto antistoixo bucket. Epeita apo ekei kalite h insetTrie opou eisgei to
upoloipo ngram apo katw analogos tin periptwsi. An to bucket einai gemato 
epektinete kata C kai stin sunexeia kanoume SPLIT to bucket to opoio edeixne 
o deiktis p. Telos analoga me tin katastasi tou programmatos proxorame se epomeno
guro (an prepei) allazontas tin sunartisi eisagogis tou modulo.*/

void insertHashTable(Hashtable* ht, char* ngram, int version){

	int i, pos, hash_Pos;
	bool success;
	char* saveptr;
	char* word=strtok_r(ngram," ",&saveptr);
	bool SPLIT = false;
	
	int word_len=strlen(word);
	
		
	hash_Pos = hashCode(word) & ((int)((1 <<(ht->round)) *ht->standart_size)-1);
	if(hash_Pos < ht->p) 
		hash_Pos = hashCode(word) & ((int)((1 <<(ht->round+1)) *ht->standart_size)-1);
	/*xriazete periptwsi to bucket na einai adeio k apla na mpenei sthn prwti 8esi*/

	if(ht->table[hash_Pos].Bcurrent_size == 0){
		(ht->table[hash_Pos].children)[0] = trieNode_Init(word, word_len, version);
		ht->table[hash_Pos].Bcurrent_size++;

		if((word+word_len+1)[0]!=0){
			success = Trie_Insert(&(ht->table[hash_Pos].children)[0], word+word_len+1, version);
			if(success)
				ht->final++;
		}
		else{
			ht->table[hash_Pos].children[0].isFinal=true;
			ht->final++;
		}
		return;
	}
	
	pos = binary_search(ht->table[hash_Pos].children, ht->table[hash_Pos].Bcurrent_size, word);

	bool condition;

	if((ht->table[hash_Pos].children)[pos].word_1){
		if(strcmp((ht->table[hash_Pos].children)[pos].word_1, word) == 0)
			condition=true;
		else
			condition=false;
	}
	else{
		if(strcmp((ht->table[hash_Pos].children)[pos].word_2, word) == 0)
			condition=true;
		else
			condition=false;
	}
	if(condition){ 
		//the word exists in the bucket so we dont insert it again.. 
		if((word+word_len+1)[0]!=0){
			success = Trie_Insert(&(ht->table[hash_Pos].children)[pos], word+word_len+1, version);
			if(success)
				ht->final++;
		}
		else{
			if(!ht->table[hash_Pos].children[pos].isFinal){
				ht->table[hash_Pos].children[pos].isFinal=true;
				ht->table[hash_Pos].children[pos].A_version=version;
				ht->final++;
			}
		}
	}
	else{
		//fixaroume to pos
		bool condition_1=true;
		if((ht->table[hash_Pos].children)[pos].word_1){
			if(pos!=ht->table[hash_Pos].max_size - 1){
				if(strcmp(word, (ht->table[hash_Pos].children)[pos].word_1)>0)
					pos++;
			}
			else{//einai sto telos
				if(strcmp(word, (ht->table[hash_Pos].children)[pos].word_1)>0){
					condition_1=false;
					pos++;
				}
			}
		}
		else{
			if(pos!=ht->table[hash_Pos].max_size-1){
				if(strcmp(word, (ht->table[hash_Pos].children)[pos].word_2)>0)
					pos++;
			}
			else
				if(strcmp(word, (ht->table[hash_Pos].children)[pos].word_2)>0){
					condition_1=false;
					pos++;
				}
		}
		if(ht->table[hash_Pos].Bcurrent_size >= ht->table[hash_Pos].max_size){//an DEN xwraei sto bucket realloc se + C size   //OVERFLOW BUCKET
			ht->table[hash_Pos].children = realloc( ht->table[hash_Pos].children, (ht->table[hash_Pos].max_size + ht->standart_bucket_size)*sizeof(trie_node));
			ht->table[hash_Pos].max_size = ht->table[hash_Pos].max_size + ht-> standart_bucket_size;

			SPLIT = true;//SPLIT!!!!!!!!!!!!!!!!!!!!!!
		}
		if(condition_1){
			memmove(&((ht->table[hash_Pos].children)[pos+1]), &((ht->table[hash_Pos].children)[pos]), (ht->table[hash_Pos].Bcurrent_size - pos)*sizeof(trie_node));
			(ht->table[hash_Pos].children)[pos] = trieNode_Init(word, word_len, version);						
			ht->table[hash_Pos].Bcurrent_size ++;	

			if((word+word_len+1)[0]!=0){
				success = Trie_Insert(&(ht->table[hash_Pos].children)[pos],  word+word_len+1, version);
				if(success)
					ht->final++;
			}
				
			else
				if(!ht->table[hash_Pos].children[pos].isFinal){
					ht->table[hash_Pos].children[pos].isFinal=true;
					ht->table[hash_Pos].children[pos].A_version=version;
					ht->final++;
				}
		}
		else{			
			(ht->table[hash_Pos].children)[pos] = trieNode_Init(word, word_len, version);
			ht->table[hash_Pos].Bcurrent_size ++;

			if((word+word_len+1)[0]!=0){
				success = Trie_Insert(&(ht->table[hash_Pos].children)[pos],  word+word_len+1, version);
				if(success)
					ht->final++;
			}
			
			else			
				if(!ht->table[hash_Pos].children[pos].isFinal){
					ht->table[hash_Pos].children[pos].isFinal=true;
					ht->table[hash_Pos].children[pos].A_version=version;
					ht->final++;
				}
		}
	}

	while(SPLIT){ //while in case where a split happen inside a spilt
		SPLIT = false;
		int j, new_Pos;//TO NEWPOS aneta antik8estite apo to palio hashposh gia e3imonomisi
		trie_node temp;
		bool condition_1;

		ht->table = realloc(ht->table, sizeof(Bucket)*(ht->Hcurrent_size + 1));//one more new bucket
		ht->Hcurrent_size++;
		//init the new bucket
		ht->table[ht->Hcurrent_size -1].children = malloc(sizeof(trie_node)*ht->standart_bucket_size);
		ht->table[ht->Hcurrent_size -1].max_size = ht->standart_bucket_size;
		ht->table[ht->Hcurrent_size -1].Bcurrent_size = 0;

		/*epeidi to bucket sunexws alliwnete 8a antigra4oume
		 to bucket se ena temporary bucket k auto 8a kanoume split*/
		Bucket btemp;
		memmove(&btemp, &ht->table[ht->p], sizeof(ht->table[ht->p]));
		ht->table[ht->p].Bcurrent_size=0; //adi na to adeiasw orizw pws einai adeio.

		for(i=0; i<btemp.Bcurrent_size;i++){//pernoume ena ena ta stoixeia pou dinxei to P (pleon to btemp)
			temp = btemp.children[i];
			int ngramID;
			if(temp.word_1)
				ngramID = hashCode(temp.word_1);
				//ngramID = MurmurHash3(temp.word_1, strlen(temp.word_1), 1);
			else
				ngramID = hashCode(temp.word_2);
				//ngramID = MurmurHash3(temp.word_2, strlen(temp.word_2), 1);

			new_Pos = ngramID & ((int)((1 <<(ht->round+1)) *ht->standart_size)-1); //ypologismos tou newpos
			//an bucket adeio tote mpainei sto pos=0
			if(ht->table[new_Pos].Bcurrent_size==0){
				pos=0;
				condition_1=true;
			}
			else{//upologisoume k fixaroume to pos
				if(temp.word_1)
					pos = binary_search(ht->table[new_Pos].children, ht->table[new_Pos].Bcurrent_size, temp.word_1);
				else
					pos = binary_search(ht->table[new_Pos].children, ht->table[new_Pos].Bcurrent_size, temp.word_2);
				condition_1=true;//fix the pos
				if(temp.word_1){
					if((ht->table[new_Pos].children)[pos].word_1){
						if(pos != ht->table[new_Pos].max_size - 1){
							if(strcmp(temp.word_1, (ht->table[new_Pos].children)[pos].word_1)>0)
								pos++;
						}
						else{//einai sto telos
							if(strcmp(temp.word_1, (ht->table[new_Pos].children)[pos].word_1)>0){
								condition_1=false;
								pos++;
							}
						}
					}
					else{
						if(pos != ht->table[new_Pos].max_size - 1){
							if(strcmp(temp.word_1, (ht->table[new_Pos].children)[pos].word_2)>0)
								pos++;
						}
						else{//einai sto telos
							if(strcmp(temp.word_1, (ht->table[new_Pos].children)[pos].word_2)>0){
								condition_1=false;
								pos++;
							}
						}
					}
				}
				else{
					if((ht->table[new_Pos].children)[pos].word_1){
						if(pos != ht->table[new_Pos].max_size - 1){
							if(strcmp(temp.word_2, (ht->table[new_Pos].children)[pos].word_1)>0)
								pos++;
						}
						else{//einai sto telos
							if(strcmp(temp.word_2, (ht->table[new_Pos].children)[pos].word_1)>0){
								condition_1=false;
								pos++;
							}
						}	
					}
					else{
						if(pos != ht->table[new_Pos].max_size - 1){
							if(strcmp(temp.word_2, (ht->table[new_Pos].children)[pos].word_2)>0)
								pos++;
						}
						else{//einai sto telos
							if(strcmp(temp.word_2, (ht->table[new_Pos].children)[pos].word_2)>0){
								condition_1=false;
								pos++;
							}
						}
					}
				}
			}
			//EISAGOGI STIN 8ESH POS
			if(ht->table[new_Pos].Bcurrent_size >= ht->table[new_Pos].max_size){//an DEN xwraei sto bucket realloc se + C size   //OVERFLOW BUCKET
				ht->table[new_Pos].children = realloc( ht->table[new_Pos].children, (ht->table[new_Pos].max_size + ht->standart_bucket_size)*sizeof(trie_node) );
				ht->table[new_Pos].max_size = ht->table[new_Pos].max_size + ht-> standart_bucket_size;
				SPLIT = true;//SPLIT!!!!!!!!!!!!!!!!!!!!!!
			}
			if(condition_1){

				
				memmove(&((ht->table[new_Pos].children)[pos+1]), &((ht->table[new_Pos].children)[pos]), (ht->table[new_Pos].Bcurrent_size - pos)*sizeof(trie_node));
				(ht->table[new_Pos].children)[pos] = temp;						
				ht->table[new_Pos].Bcurrent_size ++;
			}
			else{
				(ht->table[new_Pos].children)[pos] = temp;
				ht->table[new_Pos].Bcurrent_size ++;
			}
		}
		ht->p++;
		if(ht->p == (int)((1 <<ht->round)*ht->standart_size)){//NEW ROUND!!!
			ht->p = 0;
			ht->round ++;
		}
	}
}


/*elegxei thn riza an exei thn swsth lejh...an nai tote kalei
thn trie find gia dunamiko h statiko dentro...analogos to statice trie
an vrethei final ngram to vazei sto teliko string kai to pernaei apo 
thn insert_ArrayCntr opou gemizei ton pinaka gia to topk
*/

char* FindHashTable(void* arguments, int thread_id){

	Argument *arg=(Argument*)arguments;
	char *n_gram=arg->string;
	int static_trie= arg->static_trie;
	int version =arg->version;
	Hashtable *ht=(Hashtable*) arg->hashable;
	free(arg);


	Str_Cntr return_val;								//auto tha epistrafei
	return_val.value=malloc(TEXT_SIZE);					//to string
	return_val.size=0;									//to megethos tou return val
	return_val.max_size=TEXT_SIZE;						//to megisto megethos to return_val..elegxos gia realloc
	return_val.counter=0;								//poses lejeis periexei	

	
	String_Array my_array = split_str(n_gram);

	BloomFilter bf ;
	bf=BF_Init(ht->final);
	Str_Cntr str;
	str.value=malloc(TEXT_SIZE);						//periexei to ngram pou thabei sto return_val an den uparxei hdh
	str.size=0;											//to megethos tou
	str.max_size = TEXT_SIZE;							//to max megethos pou xwraei..gia realloc
	int i;
	for(i=0; i< my_array.size; i++){
		
		
		trie_node *our_node = lookupTrieNode(ht, my_array.array[i]);
		if(our_node != NULL)
		{
			int length=strlen(my_array.array[i]);
			/*
				vrike idia lejh opote thn vazei sto str
				an einai final tote tha thn valei sto return_val 
				an den einai hdh...

				an return_val.counter einai 0 tote den thanai opote thn vazei
				alliws psaxnei sto values array kai me vash to apotelesma
				thn vazei sto array kai sto return_val*/
			
	
			if(str.max_size <= str.size + length + 1)
			{	
				// se periptwsh pou prepei na tou aujhthei to megethos
				str.value=realloc(str.value, str.max_size*2 + length + 1);
				str.max_size = str.max_size*2 + length + 1;
			}

			str.size = length +1 ;
			strncpy(str.value, my_array.array[i], length +1 );

			if(our_node->isFinal && (our_node->D_version > version || our_node->D_version==-1 ) && our_node->A_version <= version)
			{
				int position;
				bool condition_1;

				bool exist=BF_Lookup( &bf, str.value);
				if(!exist)
				{	
					insert_ArrayCntr(str.value, thread_id);		
					if(return_val.counter==0)
					{
						strncpy(return_val.value ,str.value, str.size);
						return_val.size += str.size;
						return_val.counter++;
					}
					else
					{	
						if(return_val.size + str.size +1 >= return_val.max_size)
						{
							return_val.value=realloc(return_val.value, return_val.max_size*2 + str.size );
							return_val.max_size = return_val.max_size*2 + str.size;
						}
						strncat(return_val.value, "|", 1);
						strncat(return_val.value, str.value, str.size);
						return_val.size = return_val.size + str.size;
						return_val.counter++;	
					}
				}
			}
			if(my_array.size > i+1){	
				
				if(!static_trie){
					if(our_node->size >0)
						Trie_Find(our_node, i, &str, &return_val, &bf,  &my_array, thread_id, version);		
				}
				else{
					if(our_node->size >0 || our_node->isHyperNode)
						StaticTrie_Find(our_node, i, &str, &return_val, &bf, &my_array, thread_id, version);
				}
			}
		}
		if(str.value)
		{			
			memset(str.value, 0, str.max_size);
			str.size=0;
		}
	}
	free(str.value);
	if(return_val.counter==0){
		strncpy(return_val.value ,"-1", strlen("-1")+1);
		return_val.size=+strlen("-1")+1;
	}
	if(bf.table)BF_Destroy(&bf);

	free(n_gram);
	free(my_array.array);

	return return_val.value; 
	

}





trie_node* lookupTrieNode(Hashtable* ht, char *word){

	int i, pos, hash_Pos;
	bool word_1;

 
	//hash_Pos = hashCode(word) % ((int)pow(2,ht->round)*ht->standart_size);
	hash_Pos = hashCode(word) & ((int)((1 <<ht->round)*ht->standart_size)-1);
	if(hash_Pos < ht->p) 
		hash_Pos = hashCode(word) & ((int)((1 <<(ht->round+1)) *ht->standart_size)-1);

	if(ht->table[hash_Pos].Bcurrent_size == 0)
		return NULL;
	
	pos = binary_search(ht->table[hash_Pos].children, ht->table[hash_Pos].Bcurrent_size, word);
	bool condition;

	if((ht->table[hash_Pos].children)[pos].word_1){
		if(strcmp((ht->table[hash_Pos].children)[pos].word_1, word) == 0)
			condition=true;
		else
			condition=false;
	}
	else{
		if(strcmp((ht->table[hash_Pos].children)[pos].word_2, word) == 0)
			condition=true;
		else
			condition=false;
	}

	if(!condition)
		return NULL;
	else
		return &(ht->table[hash_Pos].children)[pos];
}




/*allazei mono to version*/
bool deleteHashTable_Versioning(Hashtable* ht, char* ngram, int version){
	int i, pos, hash_Pos;	
	long int ngramID;

	char* saveptr;
	char* word=strtok_r(ngram," ",&saveptr);
	int word_len=strlen(word);

	ngramID = hashCode(word);
	hash_Pos = ngramID % ((int)((1 <<ht->round)*ht->standart_size));
	if(hash_Pos < ht->p)
		hash_Pos = ngramID % ((int)((1 <<ht->round+1)*ht->standart_size));

	pos = binary_search(ht->table[hash_Pos].children, ht->table[hash_Pos].Bcurrent_size, word);
	bool condition;

	if((ht->table[hash_Pos].children)[pos].word_1){
		if(strcmp((ht->table[hash_Pos].children)[pos].word_1, word) == 0)
			condition=true;
		else
			condition=false;
	}
	else{
		if(strcmp((ht->table[hash_Pos].children)[pos].word_2, word) == 0)
			condition=true;
		else
			condition=false;
	}
	if(condition){
		if((word+word_len+1)[0]!=0 && ht->table[hash_Pos].children[pos].size >0){
			return Trie_Delete_Versioning(&(ht->table[hash_Pos].children)[pos], word+word_len+1 , version);
		}
		else{
			if((ht->table[hash_Pos].children)[pos].isFinal){
				(ht->table[hash_Pos].children)[pos].D_version=version;
				return true;
			}
		}
	}
	return false;
}



/*diagrafei to ngram kalontas thn trie_delete

an epistrepsei true tha prepei na diagraftei kai apo to hashtable*/
bool deleteHashTable(Hashtable* ht, char* ngram){
	
	int i, pos, hash_Pos;
	long int ngramID;

	char* saveptr;
	char* word=strtok_r(ngram," ",&saveptr);
	int word_len=strlen(word);
	ngramID = hashCode(word);
	hash_Pos = ngramID % ((int)((1 <<ht->round)*ht->standart_size));
	if(hash_Pos < ht->p)
		hash_Pos = ngramID % ((int)((1 <<ht->round+1)*ht->standart_size));

	pos = binary_search(ht->table[hash_Pos].children, ht->table[hash_Pos].Bcurrent_size, word);
	bool condition;

	if((ht->table[hash_Pos].children)[pos].word_1){
		if(strcmp((ht->table[hash_Pos].children)[pos].word_1, word) == 0)
			condition=true;
		else
			condition=false;
	}
	else{
		if(strcmp((ht->table[hash_Pos].children)[pos].word_2, word) == 0)
			condition=true;
		else
			condition=false;
	}
	if(condition){
		if((word+word_len+1)[0]!=0 && ht->table[hash_Pos].children[pos].size >0 ){
			if(Trie_Delete(&(ht->table[hash_Pos].children)[pos], (word+word_len+1))){
				/*prepei na diagraftei kai apo to hashtable..alla tha diagraftei mono an den exei paidia
				kai den einai final*/
				if((ht->table[hash_Pos].children)[pos].size == 0 && !(ht->table[hash_Pos].children)[pos].isFinal  ){
					trieNode_Delete(&(ht->table[hash_Pos].children)[pos]);
					memmove(&((ht->table[hash_Pos].children)[pos]), &((ht->table[hash_Pos].children)[pos+1]), (ht->table[hash_Pos].Bcurrent_size - (pos+1))*sizeof(trie_node));
					ht->table[hash_Pos].Bcurrent_size--;
				}
			}
		}
		else{
			/*prepei na diagraftei mono mia lejh apo to hashtable
			ama exei paidia den thn svhnoume*/
			if((ht->table[hash_Pos].children)[pos].isFinal){
				(ht->table[hash_Pos].children)[pos].isFinal=false;
				if((ht->table[hash_Pos].children)[pos].size == 0){
					trieNode_Delete(&(ht->table[hash_Pos].children)[pos]);
					memmove(&((ht->table[hash_Pos].children)[pos]), &((ht->table[hash_Pos].children)[pos+1]), (ht->table[hash_Pos].Bcurrent_size - (pos+1))*sizeof(trie_node));
					ht->table[hash_Pos].Bcurrent_size--;
				}
			}
		}
	}
	return true;
}





bool destroyHashTable(Hashtable* ht ){//should destroy trie too
	int i,k;
	for(i=0;i<ht->Hcurrent_size;i++){
		for(k=0;k<ht->table[i].Bcurrent_size;k++)
			Trie_Destroy(&ht->table[i].children[k]);
		free(ht->table[i].children);
	}
	free(ht->table);
	free(ht);
	return true;
}





//----------------------------------------------------------------	STATIC 	------------------------------------------------------------


/*kalei anadromika tous kombous kai autoi
pou exoun mono ena paidi enwnontai kai ginontai
enas hypernode*/
void Compress(Hashtable* ht){
	int i,j,k;
	for(i=0; i<ht->Hcurrent_size; i++){
		for(j=0; j<ht->table[i].Bcurrent_size; j++){
			if(ht->table[i].children[j].size == 0)
				continue;
			else if(ht->table[i].children[j].size == 1){ //den 8a xriastei
				convert_toHyperNode(&ht->table[i].children[j], &ht->table[i].children[j].children[0]);
			}
			else{
				for(k=0; k< ht->table[i].children[j].size; k++)
					CompressTrie(&ht->table[i].children[j].children[k]);
			}
		}
	}
}

void CompressTrie(trie_node* node){
	int i;
	if(node->size==0)
		return;

	else if(node->size==1){
		convert_toHyperNode(node, &node->children[0]);
		CompressTrie(node);
	}
	else{
		for(i=0; i< node->size; i++)
			CompressTrie(&node->children[i]);
	}
}

/*h compress 2 kanei olous tous kombous pou prepei na ginoun hypernode mazi*/
void Compress2(Hashtable* ht){
	int i,j,k;
	for(i=0; i<ht->Hcurrent_size; i++){
		for(j=0; j<ht->table[i].Bcurrent_size; j++){
			if(ht->table[i].children[j].size == 0)
				continue;
			else{
				for(k=0; k< ht->table[i].children[j].size; k++)
					CompressTrie2(&ht->table[i].children[j].children[k]);
			}
		}
	}
}
/*
void CompressTrie2(trie_node* node){
	int i;
	if(node->size==0)
		return;
	Queue* q = createQueue();
	trie_node *Home = node;
	while(1){
		if(node->size == 1){
			Job* job = create_AnyJob((void*) node);
			insertQueue(q, job);
			node = &node->children[0];
		}
		else{
			if(!emptyQueue(q))
				convert_toHyperNode2(q);
			destroyQueue(q);
			break;
		}
	}
	for(i=0; i<Home->size;i++)
		CompressTrie(&Home->children[i]);
	return;
}*/
void CompressTrie2(trie_node* node){//ειδαγωγη τοθ βρανωηεδ
	int i;
	bool flag = false;
	if(node->size==0)
		return;
	Queue* q = createQueue();
	trie_node *Home = node;
	while(1){
		if(node->size == 1){
			Job* job = create_AnyJob((void*) node);
			insertQueue(q, job);
			node = &node->children[0];
			flag = true;
		}
		else{
			if(flag){
				Job* job = create_AnyJob((void*) node);
				insertQueue(q, job);
			}

			if(!emptyQueue(q))
				convert_toHyperNode2(q);
			destroyQueue(q);
			break;
		}
	}
	for(i=0; i<Home->size;i++)
		CompressTrie(&Home->children[i]);
	return;
}





