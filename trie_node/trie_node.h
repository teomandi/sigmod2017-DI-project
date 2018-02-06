#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../job_scheduler/Queue/queue.h"

#define CHILDREN 5
#define STACKSIZE 30

#define SIZE 10			//megethos lejhs
#define TEXT_SIZE 1000	//megethos keimenou..px return_val, results
#define VALUES 100		//megethos pinakas elegxou uparjhs lejhs..kai allon pinakwn genikotera

#define P 0.088			//h pithanothta false positive sto bloom filter
#define THREADS 6		//arithmos thread..

#define M 512			//m kai c tou linear hash
#define C 50



typedef int bool;
#define true 1
#define false 0

typedef struct  trie_node{
	char* word_1;
	char word_2[SIZE];

	struct trie_node* children;
	int size; //posa paidia exei
	int max_size; //poso xwro exei desmeusei
	bool isFinal;

	bool isHyperNode;				//an einai hyperNode
	char* hyperNode_word;
	int  hyperNode_wordSize;
	int  hyperNode_wordMax;

	int* hyperNode_array;			//to array me to megethos ton lejwn
	int hyperNode_size; 			//posa stoixeia periexei to array
	int hyperNode_max;


	/*versioning*/
	int A_version;
	int D_version;

}trie_node;

trie_node trieNode_Init(char* word, int word_size, int version);
bool trieNode_Delete(trie_node* node);

void convert_toHyperNode(trie_node* father, trie_node* son);
void convert_toHyperNode2(Queue* q);

void swap_arrays(trie_node **board, trie_node **new_board);