#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "structures.h"
/*
	periexei sunarthseis epiprosthetwn domwn kai
	alles sunarthseis pou xreiazontai gia  thn
	ektelesh tou programmatos
*/


//kanei init ena Char
Char Char_Init(char* word){
	Char node;
	int length=strlen(word);
	if(length <SIZE){
		node.word_1=NULL;
		strncpy(node.word_2, word, length +1);	
	}
	else{
		node.word_1 =malloc(length+1);
    	strncpy(node.word_1, word, length+1);
    }
    return node;
}


//bsearch alla gia Char
int Char_binary_search(Char *array, int size, char *searchVal){
	int low = 0;
	int high = size-1;
	int mid=0 ;
	while(low <= high){
		mid = (low+high)/2;
		if(array[mid].word_1){
			if(strcmp(searchVal, array[mid].word_1)==0)
				return mid;
			else if(strcmp(array[mid].word_1, searchVal)<1)
				low = mid+1;
			else
				high = mid-1;
		}
		else{
			if(strcmp(searchVal, array[mid].word_2)==0)
				return mid;
			else if(strcmp(array[mid].word_2, searchVal)<1)
				low = mid+1;
			else
				high = mid-1;
		}
	}
	return mid;
}


//b search gia trie_node
int binary_search(trie_node *children, int size, char *searchVal){
	int low = 0;
	int high = size-1;
	int mid ;

	if(size == 0) return 0;

	while(low <= high){

		mid = (low+high)>>1;
		if(children[mid].word_1){
			if(strcmp(searchVal, children[mid].word_1)==0)
				return mid;
			else if(strcmp(children[mid].word_1, searchVal)<1)
				low = mid+1;
			else
				high = mid-1;
		}
		else{
			if(strcmp(searchVal, children[mid].word_2)==0)
				return mid;
			else if(strcmp(children[mid].word_2, searchVal)<1)
				low = mid+1;
			else
				high = mid-1;
		}
	}

	return mid;
}


/*
	epistrefei thn epomenh lejh..pairnei enan deikth
	kai an vrei \n h \0 stamataei 
*/
char* next_word(char* ngram, int *index ){
	char *word=malloc(SIZE);
	int size=10;
	int j=0;
	while(ngram[*index]==' ')
		(*index)++;
	while(ngram[*index]!=' ' && ngram[*index]!='\0'){
		if(j<size)
			(word)[j]=ngram[*index];
		else{
			(word)=realloc((word), size+SIZE);
			size+=SIZE;
			(word)[j]=ngram[*index];
		}
		j++;
		(*index)++;
	}
	if(j<size)
		(word)[j]='\0';
	else{
		(word)=realloc((word), size+1);
		size+=1;
		(word)[j]='\0';
	}
	return word;
}



String_Array split_str(char* ngram){
	String_Array str;
	
	int max_size=VALUES;
	str.array=malloc(sizeof(char*)*VALUES);
	str.size=0;
	char* saveptr;
	char *word=strtok_r(ngram," ", &saveptr);
	//char* word=strsep(&ngram, " ");
	//char *word=strtok(ngram," ");
	while(word){
		if(str.size==max_size){
			str.array=realloc(str.array,sizeof(char*)*(2*max_size) );
			max_size=2*max_size;
		}
		str.array[str.size]=word;
		str.size++;
		//printf("%s\n",word );
		word=strtok_r(NULL, " ", &saveptr);
		//word=strsep(&ngram, " ");
		//word=strtok(NULL," ");
	}
	return str;
}





long int hashCode(char* s) {
    long int h = 0;
    while (*s) {;
        h = 31*h + (*s++);
    }
    if(h<0)
    	h=abs(h);

    return h;
}


Argument* create_Argument(void* hashtable, char* string, int string_size,  int version, bool static_trie){
	Argument* node=malloc(sizeof(Argument));
	node->version = version;
	node->static_trie=static_trie;
	node->hashable =hashtable;

	node->string=malloc(string_size+1);
	strncpy(node->string, string, string_size+1);

	return node;
}


//-----------------------------------STACK-----------------------------------------------




Stack* createStack(){
	Stack* st = malloc(sizeof(Stack));
	st->size = STACKSIZE;
	st->top = -1;
	st->snodes = malloc(sizeof(stack_node)*st->size);
	return st;
}



bool pushStack(Stack *st, trie_node* node, int kid){
	if(st->size-1 == st->top){//stack is full new size
		st->snodes = realloc(st->snodes, 2*st->size*sizeof(stack_node));
		st->size = 2*st->size;
	}
	st->top ++;
	st->snodes[st->top].tnode = node;
	st->snodes[st->top].kid_pos = kid;

	return true;
}



stack_node popStack(Stack *st){
	if(isEmpty_Stack(st)){
		stack_node sn;
		sn.tnode=NULL;
		sn.kid_pos=-2;
		return sn;
	}
	st->top--;
	return st->snodes[st->top+1];
}



void destroyStack(Stack *st){
	free(st->snodes);
	free(st);
}



bool isEmpty_Stack(Stack *st){
	return (st->top==-1);
}
int gettopStack(Stack *st){
	return st->top;
}












void printStack(Stack *st){
	int i;
	if(isEmpty_Stack(st)){
		printf("STACK: stack is empty\n");
		return;
	}
	printf("~~~~~~~~~~~~~~~size %d\n",st->size);
	for(i = st->top; i>=0; i--){
		printf("%d: \n",i);
		if(st->snodes[i].tnode->word_1)
			printf("%s : %d\n", st->snodes[i].tnode->word_1, st->snodes[i].kid_pos );
		else
			printf("%s : %d\n", st->snodes[i].tnode->word_2, st->snodes[i].kid_pos );
	}
	printf("~~~~~~~~~~~~~~~\n");
	return;
}


