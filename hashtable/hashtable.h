
#include "../trie/trie.h"
#include "../job_scheduler/Queue/queue.h"




typedef struct Bucket{
	int Bcurrent_size;//the size bucket has atm
	int max_size;//the max size can get before overflow

	trie_node* children;
}Bucket;


typedef struct Hashtable{
	int standart_size;//M
	int Hcurrent_size;//the size hashtable has
	int standart_bucket_size;//C glitwnoume xwro apto natan sto bucket struct
	int p;
	int round;
	int final;
	

	Bucket* table;
}Hashtable;





Hashtable* createHashTable();
void insertHashTable(Hashtable* ht, char* ngram, int version);
trie_node* lookupTrieNode(Hashtable* ht, char *word);
char* FindHashTable( void *arguments, int thread_id);



bool deleteHashTable_Versioning(Hashtable* ht, char* ngram, int version);
bool deleteHashTable(Hashtable* ht, char* ngram);


bool destroyHashTable(Hashtable* ht);

void Compress(Hashtable* ht);
void CompressTrie(trie_node* node);
void Compress2(Hashtable* ht);
void CompressTrie2(trie_node* node);

void printBucket(Bucket *b);
void printAllBuckets();

