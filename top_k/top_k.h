
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#ifndef DEF_2
#define DEF_2

#include "../structures/structures.h"


/*periexei thn lejh kai to poses
fores exei emfanistei*/
typedef struct Word_Cntr{
	Char word;
	int counter;
}Word_Cntr;


typedef struct Array_Cntr{
	Word_Cntr *array;
	int index;
	int size;
}Array_Cntr;


typedef struct MinHeap{
	Word_Cntr **heap;
	int size;
}MinHeap;

Array_Cntr ArrayCntr_Init();
Word_Cntr WordCntr_Init(char *word);



void heapify(Word_Cntr **minHeap, int i, int size);

static Array_Cntr final;
static Array_Cntr topk[THREADS];
static MinHeap minHeap;
static Str_Cntr topk_results;


void init_TopK();
void calculate_TopK(int k);
void destroy_TopK();
void clean_TopK();

void Merge_Arrays();
int binarySearch(Word_Cntr *array, int size, char *searchVal);
bool insert_ArrayCntr(char *ngram, int index);

#endif