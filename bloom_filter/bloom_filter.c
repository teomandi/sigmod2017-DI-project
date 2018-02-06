
#include "bloom_filter.h"

/*parnei ws n ton arithmo ton final ngram dioti einai
to megisto to opoio mporei na tou dwthei kai na prepei 
na ta kanei insert

ta m kai k (size, hash_count) upologizontai me bash tupous
pou vrethikan sto wikipedia kai se alla site pou psajame
p: einai h pithanothta positive false*/
BloomFilter BF_Init(int n){
	BloomFilter bf;

  if(n>500000)
    if(n>1300000)
        bf.n=50000; //<---- (0)
     else
        bf.n=10000; //<---- (0)
  else
    bf.n=n;

	bf.size = ceil((-1)*(bf.n * log(P)) / pow(log(2.0),2));
	bf.hash_count= round(log(2.0) * bf.size / bf.n);
  //printf("k: %d  m: %d n: %d n: %d %f\n",bf.hash_count, bf.size, bf.n, n, P);exit(1);
	bf.table=malloc(sizeof(char)*bf.size);
	memset(bf.table, 0, sizeof(char)*bf.size);

	return bf;
}



/*kalei thn murmurhash hash_count fores kai an vrei 0 shmainei
pws den upirxe kai epistrefei false..alliws an vrei mono 1 tote
epistrefei true */
bool BF_Lookup(BloomFilter* bf, char* ngram)
{
    int i;
	bool exist=true;
    int temp=0;
    int length=strlen(ngram);
    int pos=1;
	for(i=0; i < bf->hash_count-1; i++){
	   temp=pos;
       pos=(MurmurHash3(ngram, length, i))% bf->size;
		if(bf->table[pos]==0)
		{
		  bf->table[pos]=1;
		  exist=false;
		}
	}
    pos=(pos+temp)%bf->size;
    if(bf->table[pos]==0)
    {
        bf->table[pos]=1;
        exist=false;
    }


    return exist;
}



//den xreisimopoihtai,,
void BF_Insert(BloomFilter* bf, char* ngram)
{
	int i;
	for(i=0; i < bf->hash_count; i++){
		int pos=(MurmurHash3(ngram, sizeof(ngram), i))% bf->size;
		bf->table[pos]=1;
	}
}


void BF_Destroy(BloomFilter* bf)
{
	free(bf->table);
	bf->table=NULL;
}



uint32_t MurmurHash3(const char *key, uint32_t len, uint32_t seed)
{
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t h = 0;
  uint32_t k = 0;
  uint8_t *d = (uint8_t *) key; // 32 bit extract from `key'
  const uint32_t *chunks = NULL;
  const uint8_t *tail = NULL; // tail - last 8 bytes
  int i = 0;
  int l = len / 4; // chunk length

  h = seed;

  chunks = (const uint32_t *) (d + l * 4); // body
  tail = (const uint8_t *) (d + l * 4); // last 8 byte chunk of `key'

  // for each 4 byte chunk of `key'
  for (i = -l; i != 0; ++i) {
    // next 4 byte chunk of `key'
    k = chunks[i];

    // encode next 4 byte chunk of `key'
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    // append to hash
    h ^= k;
    h = (h << r2) | (h >> (32 - r2));
    h = h * m + n;
  }

  k = 0;

  // remainder
  switch (len & 3) { // `len % 4'
    case 3: k ^= (tail[2] << 16);
    case 2: k ^= (tail[1] << 8);

    case 1:
      k ^= tail[0];
      k *= c1;
      k = (k << r1) | (k >> (32 - r1));
      k *= c2;
      h ^= k;
  }

  h ^= len;

  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);

  return abs(h);
}
