

#include "../structures/structures.h"
#include "../top_k/top_k.h"
#include "../bloom_filter/bloom_filter.h"



typedef struct Trie{
	trie_node root;
}Trie;




Trie* Trie_Init();
bool Trie_Insert(trie_node *our_node, char* n_gram, int version);


void Trie_Find       (trie_node *our_node, int i, Str_Cntr *str, Str_Cntr *return_val, BloomFilter *bf, String_Array* my_array, int thread_id, int version);
void  StaticTrie_Find(trie_node *our_node, int i, Str_Cntr *str, Str_Cntr *return_val, BloomFilter *bf, String_Array* my_array, int thread_id, int version);

bool Trie_Delete_Versioning(trie_node *ournode, char* n_gram, int version);
bool Trie_Delete(trie_node *ournode, char* n_gram);

void Trie_Destroy(trie_node* node);



void printKids(trie_node *ournode);
void Trie_print(trie_node* node);
