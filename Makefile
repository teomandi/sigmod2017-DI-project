######################################################################
OBJS 	= trie_node/trie_node.o structures/structures.o trie/trie.o hashtable/hashtable.o bloom_filter/bloom_filter.o top_k/top_k.o job_scheduler/Queue/queue.o job_scheduler/JobScheduler.o ngrams.o 
SOURCE	= trie_node/trie_node.c structures/structures.c trie/trie.c hashtable/hashtable.c bloom_filter/bloom_filter.c top_k/top_k.c job_scheduler/Queue/queue.c job_scheduler/JobScheduler.c ngrams.c  
TEST	= trie_node/trie_node.c structures/structures.c trie/trie.c hashtable/hashtable.c bloom_filter/bloom_filter.c top_k/top_k.c job_scheduler/Queue/queue.c job_scheduler/JobScheduler.c unit_test/test.c
HEADER  = trie_node/trie_node.h structures/structures.h trie/trie.h hashtable/hashtable.h bloom_filter/bloom_filter.h top_k/top_k.h job_scheduler/Queue/queue.h job_scheduler/JobScheduler.h 
OUT  	= ngrams 
CC		= gcc
FLAGS   = -g -lm -lpthread
# -g option enables debugging mode 
# -c flag generates object code for separate files

#to run:
#./ngrams -i inputs/input2 -q queries/query1
#valgrind --leak-check=yes ./ngrams -i inputs/input1  -q queries/query1
#r -i inputs/input1 -q queries/query1

all: $(OBJS)
	$(CC)  $(OBJS) -o $(OUT) -O2  $(FLAGS)

test:
	$(CC) -g $(TEST) -o test `pkg-config --cflags --libs check`

# clean house
clean:
	rm -f $(OBJS) $(OUT) test

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################
