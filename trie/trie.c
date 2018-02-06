#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"




Trie* Trie_Init(){
	Trie* trie=malloc(sizeof(Trie));
	trie->root=trieNode_Init(" ",1, 0);

	return trie;
}

/*  gia kathe lejh psaxnei sto kathe epipedo tou trie gia thn lejh
	an thn vrei sunexizei sthn epomenh...an tou dwthei diaforetikh
	tote thn vazei sthn swsth thesh kai metakinei ola ta upoloipa me
	memmove...an den vrei oute megaluterh thn topothetei sto telos tou
	array
*/
bool Trie_Insert(trie_node *our_node, char* n_gram, int version){

	int i=0;
	char* saveptr;
	char* word=strtok_r(n_gram," ",&saveptr);

	while(word){	
		if(our_node->size==0){
			int word_len=strlen(word);
			//an den exei paidia to node
			(our_node->children)[0]=trieNode_Init(word, word_len, version);
			our_node->size++;
			our_node=&(our_node->children)[0];
		}
		else{
			//psaxnei tous kombous
			int pos = binary_search(our_node->children, our_node->size, word);
			/*	
				epeidh to struct exei word_1 kai word_2 panta ginetai elegxos gia
				na jeroume poia timh periexei thn lejh
				gia ama vrike mikrotero na to kanei na deijei ston megalutero
				in case pounai ektos oriwn
			*/
			bool condition;
			if((our_node->children)[pos].word_1){
				if(strcmp(word, (our_node->children)[pos].word_1)==0)
					condition=true;
				else
					condition=false;
			}
			else{
				if(strcmp(word, (our_node->children)[pos].word_2)==0)
					condition=true;
				else
					condition=false;
			}

			if(condition)
				// brethike node me thn idia lejh
				our_node=&(our_node->children)[pos];
			else{ 
				int word_len=strlen(word);
				/*
				prepei to pos na deixnei se thesh opou h lejh nanai megaluterh
				apo to word...h binary search epistrefei thn thesh ths pio kontinhs
				*/
				bool condition_1=true;
				if((our_node->children)[pos].word_1){
					if(pos!=our_node->size-1){
						if(strcmp(word, (our_node->children)[pos].word_1)>0)
							pos++;
					}
					else{//einai sto telos
						if(strcmp(word, (our_node->children)[pos].word_1)>0){
							condition_1=false;
							pos++;
						}
					}
				}
				else{
					if(pos!=our_node->size-1){
						if(strcmp(word, (our_node->children)[pos].word_2)>0)
							pos++;
					}
					else
						if(strcmp(word, (our_node->children)[pos].word_2)>0){
							condition_1=false;
							pos++;
						}
				}
				/*	an den uparxei xwros kanei realloc
					an vrike megalutero tote to vazei sth thesh pos kai ta upoloipa ta
					kanei shift mia thesh...alliws to vazei sto telos
				*/
				if(our_node->size >= our_node->max_size){
					(our_node->children) = realloc(our_node->children, 2*our_node->max_size * sizeof(trie_node));
					our_node->max_size= 2*our_node->max_size;
				}
				if(condition_1){
					memmove(&((our_node->children)[pos+1]), &((our_node->children)[pos]), (our_node->size-pos)*sizeof(trie_node));
					(our_node->children)[pos] = trieNode_Init(word, word_len, version);						
					our_node->size++;	
					our_node=&(our_node->children)[pos];
				}
				else{			
					(our_node->children)[pos] = trieNode_Init(word, word_len, version);
					our_node->size++;
					our_node = &(our_node->children)[pos];
				}	
			}
		}
		word=strtok_r(NULL," ",&saveptr);
	}
	if(!our_node->isFinal){
		our_node->isFinal=true;
		our_node->A_version=version;
		return true;
	}
	else
		return false;	
}


/*allazei mono to version*/
bool Trie_Delete_Versioning(trie_node *ournode, char* n_gram, int version){
	int i, index=0;
	bool found;
	char *saveptr;
	char* word=strtok_r(n_gram," ", &saveptr);
		
	while(word){
		found = binary_search(ournode->children, ournode->size, word);
		bool condition;
		if(ournode->children[found].word_1){
			if(strcmp(ournode->children[found].word_1, word)==0)
				condition=true;
			else
				condition=false;
		}
		else{
			if(strcmp(ournode->children[found].word_2, word)==0)
				condition=true;
			else
				condition=false;
		}
		if(condition){
			ournode = &ournode->children[found];
		}
		else
			return false;
		
		word=strtok_r(NULL," ", &saveptr);
	}
	if(ournode->isFinal){
		ournode->D_version=version;
		return true;
	}
	else
		return false;
}




/*arxika psaxnei to ngram kai otan vriskei swsto kombo ton
vazei sthn stoiva..an vrethei to ngram jekinaei thn diagrafei
alliws katastrefei thn stoiva

sthn diagrafh o kombos diagrafetai mono an den exei paidia..
an exei apla to allazoume to final se false

epishs an vrethei kombos pou exei paidia stamataei h diagrafh
opws kai an vrethei kai allo final(gia auto uparxei to deleted,
gia na sigoureutoume pws mono to swsto final tha diagraftei)

epistrefei true an tha prepei na diagrapsoume kai thn riza apo
to hashtable*/
bool Trie_Delete(trie_node *ournode, char* n_gram){
	int i, index=0;
	bool found;
	char *saveptr;
	char* word=strtok_r(n_gram," ", &saveptr);

	//anazhtaei tous kombous kai tous vazei sthn stoiva
	Stack *st = createStack();
	while(word){
		found = binary_search(ournode->children, ournode->size, word);
		bool condition;
		if(ournode->children[found].word_1){
			if(strcmp(ournode->children[found].word_1, word)==0)
				condition=true;
			else
				condition=false;
		}
		else{
			if(strcmp(ournode->children[found].word_2, word)==0)
				condition=true;
			else
				condition=false;
		}

		if(condition){
			pushStack(st, ournode, found);
			ournode = &ournode->children[found];
		}
		else{
			destroyStack(st);
			return false;
		}
		word=strtok_r(NULL," ", &saveptr);
	}
	
	if(!ournode->isFinal){
		destroyStack(st);
		return false;
	}
	
	//arxizei thn diagrafh
	stack_node sn;
	bool deleted=false ,stop_del=false;
	while(!isEmpty_Stack(st) && !stop_del){
		sn = popStack(st);
		bool delete = false;

		trie_node* d_node = &sn.tnode->children[sn.kid_pos];
		
		if(d_node->size > 0){
			if(d_node->isFinal && !deleted){
				d_node->isFinal=false;
				deleted = true;
				d_node->D_version=-1;
			}
			stop_del=true;
		}
		else{
			if(d_node->isFinal){
				if(!deleted){
					delete=true;
					deleted=true;
				}
				else
					stop_del=true;
			}
			else
				delete=true;
		}
		if(delete){
			trieNode_Delete(d_node);
			//alliws ftiaxnei ton pinaka tou patera tou
			memmove(&((sn.tnode->children)[sn.kid_pos]), &((sn.tnode->children)[sn.kid_pos+1]), ((sn.tnode->size-(sn.kid_pos+1))*sizeof(trie_node)));
			sn.tnode->size--;
		}
	}
	destroyStack(st);
	return stop_del;
}






//katastrefei olo to dentro anadromika
void Trie_Destroy(trie_node* node){
	int i;
	for(i=0;i<node->size;i++){
		Trie_Destroy(&node->children[i]);
	}

	trieNode_Delete(node);
}





/*
	ejetazei ta upodendra pouxoun dwthei apto root..
	pairnei tis lejeis apo ekei pou toxei afei to hashtable
	kai psaxnei gia idies..ama vrei ejetazei gia to an einai 
	finals..an einai tis vazei sto teliko string, epishs tis 
	peranaei kai apo thn insert_ArrayCntr opou gemizei ton
	pinaka tou topk...an den vrethei omoia termatizei thn diadikasia.
*/

void Trie_Find(trie_node *our_node, int index,  Str_Cntr *str, Str_Cntr *return_val, BloomFilter *bf, String_Array* my_array, int thread_id, int version)
{		
	//jekinaei na koitaei tis epomenes lejeis kai ta paidia tou komvou..
	int j;
	for(j=index+1; j<my_array->size; j++)
	{
		if(our_node->size > 0){
			int pos_2 = binary_search(our_node->children, our_node->size, my_array->array[j]);
			bool condition_2;
			if((our_node->children)[pos_2].word_1)
			{		
				if(strcmp(my_array->array[j], (our_node->children)[pos_2].word_1)==0)
						condition_2=true;
					else
						condition_2=false;
			}
			else
			{
				if(strcmp(my_array->array[j], (our_node->children)[pos_2].word_2)==0)
						condition_2=true;
					else
						condition_2=false;
			}
			
			if(condition_2)
			{
				//vrhke idia lejh ope epanalamvanei thn idia diadikasia me panw..
				int length=strlen(my_array->array[j]);
				if(str->max_size <= str->size + length+1)
				{
					str->value=realloc(str->value, str->max_size*2 + length + 1);
					str->max_size = str->max_size*2 + length + 1;
				}
				
				
				strncat(str->value, " ", 1);				
				strncat(str->value, my_array->array[j], length);
				
				str->size += length+1;

				if((our_node->children)[pos_2].isFinal && ((our_node->children)[pos_2].D_version > version || (our_node->children)[pos_2].D_version==-1 )
				 && (our_node->children)[pos_2].A_version <= version)
				{
					bool exist=BF_Lookup(bf, str->value);
					
					if(!exist)
					{	
						insert_ArrayCntr(str->value, thread_id);
						if(return_val->counter==0){
							strncpy(return_val->value, str->value, str->size);
							return_val->size =+ str->size;
							return_val->counter++;
						}
						else
						{
							if(return_val->size + str->size >= return_val->max_size){
								return_val->value = realloc(return_val->value, return_val->size*2 + str->size);
								return_val->max_size = return_val->size*2 + str->size;
							}
								
							strncat(return_val->value, "|", 1);
							strncat(return_val->value, str->value, str->size-1);
							return_val->size = return_val->size + str->size;
							return_val->counter++;	
						}
					}
				}
				our_node=&(our_node->children)[pos_2];

			}
			else//den vrike idia lejh..
				break;			
		}
		else
			break;	
	}
	
}



//-----------------------------------------------------------	STATIC 	---------------------------------------------------------------------

/* elegxei an einai hyper...an einai tote elegxei tis lejeis pou exei
kai an vrethoun oles tote sunexizei ston epomeno..an den einai hyper
symperiferetai kanonika*/
void  StaticTrie_Find(trie_node *our_node, int i, Str_Cntr *str, Str_Cntr *return_val, BloomFilter *bf, String_Array* array, int thread_id, int version)
{	
	int j;
	for(j=i+1; j< array->size ; j++)
	{			//sunexizei to ngram apo ekei pou to afhse to panw while
		
		/***********************************	HYPER NODE 	****************************************************/

		if(our_node->isHyperNode){
			bool failed=false;
			int word_pos = abs(our_node->hyperNode_array[0]);
			int k;
			for(k=1; k<our_node->hyperNode_size; k++)
			{			
				if(j < array->size){
					char *word=malloc(abs(our_node->hyperNode_array[k])+1);
					memcpy( word, our_node->hyperNode_word + word_pos, abs(our_node->hyperNode_array[k]));
					word[abs(our_node->hyperNode_array[k])]='\0';

					bool condition;
					if(strcmp(array->array[j], word)==0)
						condition=true;
					else
						condition=false;

					
					if(condition)
					{
						int length = strlen(array->array[j]);
						if(str->max_size <= str->size + length + 1)
						{
							str->value=realloc(str->value, str->max_size*2 + length + 1);
							str->max_size= str->max_size*2 + length +1;
						}
						str->size += length + 1;
						strncat(str->value, " ", 1);
						strncat(str->value, array->array[j], length);

						if(our_node->hyperNode_array[k]<0)
						{
							/***********************************/
							int position;
							bool condition_1;
							bool exist=BF_Lookup( bf, str->value);
							if(!exist)
							{	
								insert_ArrayCntr(str->value, thread_id);		 	
								if(return_val->counter==0)
								{
									memcpy(return_val->value ,str->value, str->size);
									return_val->size += str->size;
									return_val->counter++;
								}
								else
								{
									if(return_val->size + str->size >= return_val->max_size)
									{
										return_val->value=realloc(return_val->value, return_val->max_size*2 + str->size );
										return_val->max_size = return_val->max_size*2 + str->size;
									}
									return_val->size = return_val->size + str->size+1;
									strncat(return_val->value, "|", 1);
									strncat(return_val->value, str->value, str->size);	
									return_val->counter++;	
								}
							}
							/***********************************/
						}
						word_pos+=abs(our_node->hyperNode_array[k]);
						free(word);
						j++;
					}
					else{
						free(word);
						failed=true;
						break;
					}
				}
				else
					break;
			}
			if(j >= array->size || failed)
				break;
		}
		/***********************************	 	****************************************************/

		if(our_node->size>0)
		{	
			int pos_2;
			pos_2 = binary_search(our_node->children, our_node->size, array->array[j]);

			bool condition_2;
			if((our_node->children)[pos_2].word_1)
			{	
				if(strcmp(array->array[j], (our_node->children)[pos_2].word_1)==0)
					condition_2=true;
				else
					condition_2=false;	
			}
			else
			{
				if(strcmp(array->array[j], (our_node->children)[pos_2].word_2)==0)
					condition_2=true;
				else
					condition_2=false;
			
			}
			if(condition_2)
			{
				//vrhke idia lejh ope epanalamvanei thn idia diadikasia me panw..
				int length;
				length = strlen(array->array[j]);

				if(str->max_size <= str->size + length+1)
				{
					str->value=realloc(str->value, str->max_size*2 + length + 1);
					str->max_size = str->max_size*2 + length + 1;
				}
				
				str->size += length+1;
				strncat(str->value, " ", 1);
				strncat(str->value, array->array[j], length); 

				if((our_node->children)[pos_2].isFinal)
				{
					bool exist=BF_Lookup(bf, str->value);
					if(!exist)
					{	
						insert_ArrayCntr(str->value, thread_id);
						if(return_val->counter==0){
							memcpy(return_val->value, str->value, str->size);
							return_val->size =+ str->size;
							return_val->counter++;
						}
						else
						{
							if(return_val->size + str->size >= return_val->max_size){
								return_val->value = realloc(return_val->value, return_val->size*2 + str->size);
								return_val->max_size = return_val->size*2 + str->size;
							}
							strncat(return_val->value, "|", 1);
							strncat(return_val->value, str->value, str->size-1);
							return_val->size = return_val->size + str->size;
							return_val->counter++;
							
						}
					}
				}
				our_node=&(our_node->children)[pos_2];
			}
			else//den vrike idia lejh..
				break;			
		}
		else
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------


void printKids(trie_node *ournode){
	int i;
	bool f=false;
	if(ournode->isFinal)f=true;
	if(f){
		if(ournode->word_1)
			printf("node: F=%s=\n", ournode->word_1);
		else
			printf("node: F=%s=\n", ournode->word_2);
	}
	else{
		if(ournode->word_1)
			printf("node: =%s=\n", ournode->word_1);
		else
			printf("node: =%s=\n", ournode->word_2);
	}
	printf("kids: \n");
	for(i=0; i<ournode->size;i++){
		if(ournode->children[i].word_1)
			printf("\t%s\n", ournode->children[i].word_1);
		else
			printf("\t%s\n", ournode->children[i].word_2);
	}
	printf("\n");
	
}

void Trie_print(trie_node* node){
	int i;
	for(i=0;i<node->size;i++){
		Trie_print(&node->children[i]);
	}
	printKids(node);
}



