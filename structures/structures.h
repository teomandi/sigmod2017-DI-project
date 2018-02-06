
#ifndef DEF

#include "../trie_node/trie_node.h"
#include <stdint.h>


#include <math.h>

#define DEF 1




typedef struct Char{
	char* word_1;
	char word_2[SIZE];
}Char;


Char Char_Init(char* word);
int Char_binary_search(Char *array, int size, char *searchVal);


/* xrisimopoihtai sthn find_2 apothikeuontai oi lejeis*/
typedef struct String_Array{
	int size;
	char** array;
	int max;

}String_Array;

String_Array split_str(char* ngram);



//apothikeuontai strings
//xrisimopoieitai apo thn find
typedef struct Str_Cntr{
	int counter;
	int size;
	int max_size;
	char* value;
}Str_Cntr;


char* next_word(char* ngram, int *index );				//spaei thn lejh se array apo strings
int binary_search(trie_node *children, int size, char *searchVal);			//pairnei thn lejh char char


/*metablhtes tou find*/
typedef struct Argument{
	int version;
	bool static_trie;
	char *string;
	void* hashable;
}Argument;

Argument* create_Argument(void* ht, char* string, int string_size, int version, int static_trie);


long int hashCode(char* s);


//-------------------------------------------------	STACK CONTROL--xrisimopoihtai sthn delete-----------------------

typedef struct stack_node{
	trie_node* tnode;
	int kid_pos;
}stack_node;


typedef struct Stack{
	stack_node* snodes;
	int top;
	int size;
}Stack;

Stack* createStack();
bool pushStack(Stack *st, trie_node* node, int kid);
stack_node popStack(Stack *st);
bool isEmpty_Stack(Stack *st);
void destroyStack(Stack *st);
void printStack(Stack *st);
int gettopStack(Stack *st);








#endif