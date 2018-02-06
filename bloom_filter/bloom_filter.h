#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../structures/structures.h"






typedef struct BloomFilter{
	char *table;
	int hash_count;
	int size;
	int n;
}BloomFilter;




BloomFilter BF_Init(int n);
bool BF_Lookup(BloomFilter* bf, char* ngram);
void BF_Insert(BloomFilter* bf, char* ngram);
void BF_Destroy(BloomFilter* bf);

BloomFilter* BF_Resize(BloomFilter *bf, int n);


uint32_t MurmurHash3(const char *key, uint32_t len, uint32_t seed);