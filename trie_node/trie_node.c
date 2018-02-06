#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "trie_node.h"



trie_node trieNode_Init(char* word, int word_size, int version){
	trie_node node;
	if(word_size<SIZE){
		node.word_1=NULL;
		strncpy(node.word_2, word, word_size+1);	
	}
	else{
		node.word_1 =malloc(word_size+1);
    	strncpy(node.word_1, word, word_size+1);
    }
    node.isFinal=false;
	node.size=0;
	node.max_size=CHILDREN;
    node.children= malloc(CHILDREN * sizeof(trie_node));

   	node.isHyperNode=false;
    node.hyperNode_array=NULL;
    node.hyperNode_word=NULL;
    node.hyperNode_size=0; 
    node.hyperNode_max=0;

    node.A_version=version;
	node.D_version=-1;

    return node;
}


bool trieNode_Delete(trie_node* node){
    if(node->word_1)free(node->word_1);
    if(node->isHyperNode){
    	if(node->hyperNode_word)free(node->hyperNode_word);
    	if(node->hyperNode_array)free(node->hyperNode_array);
    }
    free(node->children);
    return true;
}



//----------------------------------------------------------------------	STATIC 	-----------------------------------------------------------------------


/*metatrepei ena node se hyperNode dhladh o pateras ginetai
fuse me ton son kai h lejh vrisketai sthn hypernode_word 
kai dhmiorgeitai enas pinakas me to posh xarakthres einai h 
kathe lejh...epishs o pateras pairnei to array tou paidiou 
kai to paidi diagrafetai*/
void convert_toHyperNode2(Queue* q){
	//diamorfwnei thn lejh sto hyperNonde_word 
	int i;
	int son_sz, father_sz; 
	trie_node *son, *father;
	
	Job* job = pop(q);
	father =(trie_node*)job->argument;
	free(job);
while(!emptyQueue(q)){

	job = pop(q);
	son =(trie_node*)job->argument;
	free(job);

	if(father->isHyperNode){
		/*An einai hypernode tote elegxei an uparxei xwros sto string kai to eisxwrei
		sth sunexeia elegxei an exei xwro to array kai eisxwrei th nea timh kai an
		einai final thn kanei arnhtikh*/
		if(son->word_1){
			son_sz=strlen(son->word_1);
			//father_sz=strlen(father->hyperNode_word);

			if(father->hyperNode_wordSize + son_sz >= father->hyperNode_wordMax)
			{
				father->hyperNode_word = realloc(father->hyperNode_word, father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz + 1);	
				father->hyperNode_wordMax = father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz;
			}
			
			strncat(father->hyperNode_word, son->word_1, son_sz);
			father->hyperNode_wordSize += son_sz;
		}
		else{
			son_sz=strlen(son->word_2);
			//father_sz=strlen(father->hyperNode_word);

			if(father->hyperNode_wordSize + son_sz >= father->hyperNode_wordMax)
			{
				father->hyperNode_word=realloc(father->hyperNode_word, father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz + 1);	
				father->hyperNode_wordMax= father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz;
			}
			
			strncat(father->hyperNode_word, son->word_2, son_sz);
			father->hyperNode_wordSize += son_sz;
		}
		if(father->hyperNode_max <= father->hyperNode_size)
		{
			father->hyperNode_array = realloc(father->hyperNode_array, (father->hyperNode_max *2) * sizeof(int));
			father->hyperNode_max = father->hyperNode_max *2;
		}

		father->hyperNode_array[father->hyperNode_size] = son_sz;
		if(son->isFinal)
			father->hyperNode_array[father->hyperNode_size] *= -1;
		father->hyperNode_size++;

	}
	else{
		/*Sundeie tis duo lejeis kai dhmiourgei to array*/
		if(son->word_1){
			if(father->word_1)
			{
				son_sz=strlen(son->word_1);
				father_sz=strlen(father->word_1);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz + 1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;
			
				strncpy(father->hyperNode_word, father->word_1, father_sz+1);
				strncat(father->hyperNode_word, son->word_1, son_sz);
			}
			else
			{
				son_sz=strlen(son->word_1);
				father_sz=strlen(father->word_2);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz + 1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;
			
				strncpy(father->hyperNode_word, father->word_2, father_sz+1);
				strncat(father->hyperNode_word, son->word_1, son_sz);
			}
		}
		else{
			if(father->word_1){
				son_sz=strlen(son->word_2);
				father_sz=strlen(father->word_1);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz+1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;

				strncpy(father->hyperNode_word, father->word_1, father_sz+1);
				strncat(father->hyperNode_word, son->word_2, son_sz);
			}
			else{
			
				son_sz=strlen(son->word_2);
				father_sz=strlen(father->word_2);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz + 1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;

				strncpy(father->hyperNode_word, father->word_2, father_sz+1);
				strncat(father->hyperNode_word, son->word_2, son_sz);
			}
		}

	
		father->hyperNode_array = malloc(VALUES * sizeof(int));
		father->hyperNode_array[0] = father_sz;
		father->hyperNode_array[1] = son_sz;
		if(father->isFinal)
			father->hyperNode_array[0] *= -1;
		
		if(son->isFinal)
			father->hyperNode_array[1] *= -1;
		

		father->hyperNode_size=2;
		father->hyperNode_max=VALUES;
		father->isHyperNode=true;
		
	}
	
	//antalasoun childrens
	swap_arrays(&father->children, &son->children);
	father->size = son->size;

	
	trieNode_Delete(son);
}

}



/*metatrepei ena node se hyperNode dhladh o pateras ginetai
fuse me ton son kai h lejh vrisketai sthn hypernode_word 
kai dhmiorgeitai enas pinakas me to posh xarakthres einai h 
kathe lejh...epishs o pateras pairnei to array tou paidiou 
kai to paidi diagrafetai*/
void convert_toHyperNode(trie_node* father, trie_node* son){
	//diamorfwnei thn lejh sto hyperNonde_word 
	int i;
	int son_sz, father_sz; 
	if(father->isHyperNode){
		/*An einai hypernode tote elegxei an uparxei xwros sto string kai to eisxwrei
		sth sunexeia elegxei an exei xwro to array kai eisxwrei th nea timh kai an
		einai final thn kanei arnhtikh*/
		if(son->word_1){
			son_sz=strlen(son->word_1);
			//father_sz=strlen(father->hyperNode_word);

			if(father->hyperNode_wordSize + son_sz >= father->hyperNode_wordMax)
			{
				father->hyperNode_word = realloc(father->hyperNode_word, father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz + 1);	
				father->hyperNode_wordMax = father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz;
			}
			
			strncat(father->hyperNode_word, son->word_1, son_sz);
			father->hyperNode_wordSize += son_sz;
		}
		else{
			son_sz=strlen(son->word_2);
			//father_sz=strlen(father->hyperNode_word);

			if(father->hyperNode_wordSize + son_sz >= father->hyperNode_wordMax)
			{
				father->hyperNode_word=realloc(father->hyperNode_word, father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz + 1);	
				father->hyperNode_wordMax= father->hyperNode_wordMax*2 + father->hyperNode_wordSize + son_sz;
			}
			
			strncat(father->hyperNode_word, son->word_2, son_sz);
			father->hyperNode_wordSize += son_sz;
		}
		if(father->hyperNode_max <= father->hyperNode_size)
		{
			father->hyperNode_array = realloc(father->hyperNode_array, (father->hyperNode_max *2) * sizeof(int));
			father->hyperNode_max = father->hyperNode_max *2;
		}

		father->hyperNode_array[father->hyperNode_size] = son_sz;
		if(son->isFinal)
			father->hyperNode_array[father->hyperNode_size] *= -1;
		father->hyperNode_size++;

	}
	else{
		/*Sundeie tis duo lejeis kai dhmiourgei to array*/
		if(son->word_1){
			if(father->word_1)
			{
				son_sz=strlen(son->word_1);
				father_sz=strlen(father->word_1);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz + 1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;
			
				strncpy(father->hyperNode_word, father->word_1, father_sz+1);
				strncat(father->hyperNode_word, son->word_1, son_sz);
			}
			else
			{
				son_sz=strlen(son->word_1);
				father_sz=strlen(father->word_2);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz + 1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;
			
				strncpy(father->hyperNode_word, father->word_2, father_sz+1);
				strncat(father->hyperNode_word, son->word_1, son_sz);
			}
		}
		else{
			if(father->word_1){
				son_sz=strlen(son->word_2);
				father_sz=strlen(father->word_1);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz+1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;

				strncpy(father->hyperNode_word, father->word_1, father_sz+1);
				strncat(father->hyperNode_word, son->word_2, son_sz);
			}
			else{
			
				son_sz=strlen(son->word_2);
				father_sz=strlen(father->word_2);

				father->hyperNode_word=malloc(father_sz + son_sz + SIZE);
				father->hyperNode_wordSize = father_sz + son_sz + 1;
				father->hyperNode_wordMax = father_sz + son_sz + SIZE;

				strncpy(father->hyperNode_word, father->word_2, father_sz+1);
				strncat(father->hyperNode_word, son->word_2, son_sz);
			}
		}

	
		father->hyperNode_array = malloc(VALUES * sizeof(int));
		father->hyperNode_array[0] = father_sz;
		father->hyperNode_array[1] = son_sz;
		if(father->isFinal)
			father->hyperNode_array[0] *= -1;
		
		if(son->isFinal)
			father->hyperNode_array[1] *= -1;
		

		father->hyperNode_size=2;
		father->hyperNode_max=VALUES;
		father->isHyperNode=true;
		
	}
	
	//antalasoun childrens
	swap_arrays(&father->children, &son->children);
	father->size = son->size;

	
	trieNode_Delete(son);
}


//gia thn convert to hyperNode..antalasei tous pinakes twn children
void swap_arrays(trie_node **board, trie_node **new_board){
  trie_node *temp = *board;
  *board = *new_board;
  *new_board = temp;

}