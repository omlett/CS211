pa4/                                                                                                0000755 0001750 0001750 00000000000 13020367265 007356  5                                                                                                    ustar   om                              om                                                                                                                                                                                                                     pa4/c-sim.c                                                                                         0000644 0001750 0001750 00000045756 13020367251 010546  0                                                                                                    ustar   om                              om                                                                                                                                                                                                                     #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "c-sim.h"

//-----Global variable representing the cache-----//
//-----New York Islanders - Yes! Yes! Yes!-----//
cache *tavares = NULL;

//-----Determines if n is a power of 2-----//
int powerOfTwo(int n) {
    	while ((n % 2 == 0) && n > 1)
        	n /= 2;
    	return (n == 1);
}

//-----Converts a binary string to a decimal-form integer-----//
int binaryToDecimal(char *binary) {
    	int i;
    	int result = 0;
    	int power = 0;
    	for (i = strlen(binary) - 1; i >= 0; i--) {
    	    	int added = (binary[i] - '0') * (int)pow(2, power);
    	    	result += added;
    	    	power++;
    	}
    	return result;
}

//-----Convert hex address to binary-----//
char* binaryFromHex(char *address) {
    	int i;
    	char *to_bin = malloc(sizeof(char) * 33);
    	for(i = 2; i < strlen(address); i++) {
        	switch(address[i]) {
			case '0': strcat(to_bin,"0000"); break;
            		case '1': strcat(to_bin,"0001"); break;
            		case '2': strcat(to_bin,"0010"); break;
            		case '3': strcat(to_bin,"0011"); break;
            		case '4': strcat(to_bin,"0100"); break;
            		case '5': strcat(to_bin,"0101"); break;
            		case '6': strcat(to_bin,"0110"); break;
            		case '7': strcat(to_bin,"0111"); break;
            		case '8': strcat(to_bin,"1000"); break;
            		case '9': strcat(to_bin,"1001"); break;
            		case 'a': strcat(to_bin,"1010"); break;
            		case 'b': strcat(to_bin,"1011"); break;
            		case 'c': strcat(to_bin,"1100"); break;
            		case 'd': strcat(to_bin,"1101"); break;
            		case 'e': strcat(to_bin,"1110"); break;
            		case 'f': strcat(to_bin,"1111"); break;
        	}
    	}
	strcat(to_bin,"\0");
    	return to_bin;
}

//-----Calculate # of Addresses to Trace-----//
int getNumLines(FILE *trace_file) {
	int ch, num_lines = 0;
    	do {
		ch = fgetc(trace_file);
    	    	if (ch == '\n')
            		num_lines++;
    	} while (ch != EOF);
    	return num_lines;
}

//-----Adds a character to the end of a string-----//
void append(char* s, char c)
{
    	int len = strlen(s);
    	s[len] = c;
    	s[len + 1] = '\0';
}

//-----Initialize default cache values-----//
void dropThePuck() {
    	tavares = malloc(sizeof(cache));
    	tavares -> hits = 0;
    	tavares -> misses = 0;
    	tavares -> reads = 0;
    	tavares -> writes = 0;
    	tavares -> cache_size = 0;
    	tavares -> block_size = 0;
    	tavares -> set_size = 0;
    	tavares -> associativity = NULL;
    	tavares -> assoc_value = 0;
    	tavares -> write_policy = NULL;
    	tavares -> num_sets = 0;
    	tavares -> sets = NULL;
}

//-----Creates a line in the cache-----//
cache_line* makeLine() {
    	cache_line *newLine;
    	newLine = malloc(sizeof(cache_line));
    	newLine -> valid = 0;
	newLine -> dirty = 0;
    	newLine -> tag = (char *)malloc(sizeof(char) * 33);
    	newLine -> lru_index = 0;
	newLine -> fifo_index = 0;
    	return newLine;
}

//-----Create and malloc a new set-----//
void createSet(cache_set *set) {
    	set -> lines = malloc(tavares -> set_size * sizeof(cache_line));
}

//-----Initializes the Cache-----//
void initializeCache() {
	int i, l;
	tavares -> sets = malloc(tavares -> num_sets * sizeof(cache_set*));
       	for (i = 0; i < tavares -> num_sets; i++) {
		tavares -> sets[i] = malloc(sizeof(cache_set));
		createSet(tavares -> sets[i]);
            	for (l = 0; l < tavares -> set_size; l++) {
			tavares -> sets[i] -> lines[l] = makeLine();
            	}
        }
}

//-----Initializes an address_strip-----//
void initializeAddressStrip(address_strip *memory) {
    	memory -> num_tag_bits = 0;
    	memory -> num_set_index_bits = 0;
    	memory -> num_block_offset_bits = 0;
    	memory -> set_index = NULL;
    	memory -> decimal_sindex = 0;
    	memory -> tag = NULL;
    	memory -> readwrite = NULL;
}

//-----Prints results of requested trace-----//
void printResults() {
    	printf("Memory reads: %d\n", tavares -> reads);
    	printf("Memory writes: %d\n", tavares -> writes);
    	printf("Cache hits: %d\n", tavares -> hits);
    	printf("Cache misses: %d\n", tavares -> misses);
}

//-----Calculates # of sets-----//
int calcNumSets() {
    	if (strcmp(tavares -> associativity, "direct") == 0)
        	return (tavares -> cache_size)/(tavares -> block_size);
    	else if (tavares -> assoc_value != 0)
        	return (tavares -> cache_size)/(tavares -> block_size * tavares -> assoc_value);
    	else
		return 1;
}

//-----Calculates # of tag bits-----//
int calcNumTagBits(int mem_length, address_strip *memory) {
    	return mem_length - memory -> num_set_index_bits - memory -> num_block_offset_bits;
}

//-----Calculates # of block offset bits-----//
int calcNumBlockOffsetBits() {
    	return (int)(log((double)tavares -> block_size)/log(2.0));
}

//-----Calculates # of set index bits-----//
int calcNumSetIndexBits() {
    	return (int)(log((double)tavares -> num_sets)/log(2.0));
}

//-----Updates appropriate values for the memory address-----//
void updateAddressValues(int mem_length, address_strip *memory, char *address) {
    	int i;
    	memory -> num_block_offset_bits = calcNumBlockOffsetBits();
    	memory -> num_set_index_bits = calcNumSetIndexBits();
    	memory -> num_tag_bits = calcNumTagBits(mem_length, memory);
    	
	memory -> tag = malloc(memory -> num_tag_bits * sizeof(char) + 1);
    	memory -> set_index = malloc(memory -> num_set_index_bits * sizeof(char) + 1);
/*
	for (i = 0; i < memory -> num_set_index_bits; i++) {
        	append(memory -> set_index, address[i]);
    	}
    	
    	while (i < (memory -> num_set_index_bits + memory -> num_tag_bits)) {
        	append(memory -> tag, address[i]);
        	i++;
    	}
*/
    	for (i = 0; i < memory -> num_tag_bits; i++) {
        	append(memory -> tag, address[i]);
    	}
    	
    	while (i < (memory -> num_set_index_bits + memory -> num_tag_bits)) {
        	append(memory -> set_index, address[i]);
        	i++;
    	}

    	memory -> decimal_sindex = binaryToDecimal(memory -> set_index); //-----Stores decimal value of the set index-----//
}

//-----LRU Helper-----//
int getHighestIndex(address_strip *memory) {
	int i, high_index = 0, lru_i = 0, fifo_i = 0;
	for (i = 0; i < tavares -> set_size; i++) {
		if (strcmp(tavares -> replacement_policy, "LRU") == 0){
			if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index > lru_i) {
                	    	lru_i = tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index;
                	    	high_index = i;
                	}
		}
		else{
			if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index > fifo_i) {
                	    	fifo_i = tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index;
                	    	high_index = i;
                	}
		}
	}
	return high_index;
}

//-----<replacement policy> Counter Bookkeeping-----//
void updateRecents(address_strip *memory) {
    	int i, high_index = getHighestIndex(memory);
    	for(i = 0; i < tavares -> set_size; i++) {
        	tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index++;
		tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index++;
    	}
    	tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> lru_index = 0;	//-----Resets the most recently used index-----//
	tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> fifo_index = 0;	//-----Resets the highest index for FIFO-----//
}

//-----Write Through Bookkeeping-----//
void bookThrough(address_strip *memory) {
	int i, high_index = 0, lru_i = 0, fifo_i = 0;;
	for (i = 0; i < tavares -> set_size; i++) {
                if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> valid == 0) {
                    	strcpy(tavares -> sets[memory -> decimal_sindex] -> lines[i] -> tag, memory -> tag);
                    	tavares -> sets[memory -> decimal_sindex] -> lines[i] -> valid = 1;
			updateRecents(memory);
                    	return;
                }
		if (strcmp(tavares -> replacement_policy, "LRU") == 0){
			if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index > lru_i) {
				lru_i = tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index;
				high_index = i;
			}
		}
		else{
			if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index > fifo_i) {
				fifo_i = tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index;
				high_index = i;
			}
		}
	}
	tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> tag = memory -> tag;
	updateRecents(memory);
}

//-----Write Through Ops-----//
void writeThrough(address_strip *memory) {
	int i, j;
    	for (i = 0; i < tavares -> set_size; i++) {
        	if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> valid == 1 && strcmp(memory -> tag, tavares -> sets[memory -> decimal_sindex] -> lines[i] -> tag) == 0) {
                	tavares -> hits++;
			tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index = 0;
			if (strcmp(memory -> readwrite, "W") == 0) {
				tavares -> writes++;
			}
			for (j = 0; j < tavares -> set_size; j++) {
				tavares -> sets[memory -> decimal_sindex] -> lines[j] -> fifo_index++;
				if (j != i) {
					tavares -> sets[memory -> decimal_sindex] -> lines[j] -> lru_index++;
				}
			}
			return;
            	}
    	}
	tavares -> misses++;
	tavares -> reads++;
	if (strcmp(memory -> readwrite, "W") == 0) {
		tavares -> writes++;
	}
	bookThrough(memory);
}

//-----Write Through Bookkeeping-----//
void bookBack(address_strip *memory) {
	int i, high_index = 0, lru_i = 0, fifo_i = 0;
	for (i = 0; i < tavares -> set_size; i++) {
                if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> valid == 0) {
                    	strcpy(tavares -> sets[memory -> decimal_sindex] -> lines[i] -> tag, memory -> tag);
                    	tavares -> sets[memory -> decimal_sindex] -> lines[i] -> valid = 1;
			if (tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> dirty == 1) {
				tavares -> writes++;
			}
			if (strcmp(memory -> readwrite, "R") == 0) {
				tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> dirty = 0;
			}
			else {
				tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> dirty = 1;
			}
			updateRecents(memory);
                    	return;
                }
		if (strcmp(tavares -> replacement_policy, "LRU") == 0){
			if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index > lru_i) {
				lru_i = tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index;
				high_index = i;
			}
		}
		else{
			if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index > fifo_i) {
				lru_i = tavares -> sets[memory -> decimal_sindex] -> lines[i] -> fifo_index;
				high_index = i;
			}
		}
	}
	if (tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> dirty == 1) {
		tavares -> writes++;
	}
	if (strcmp(memory -> readwrite, "R") == 0) {
		tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> dirty = 0;
	}
	else {
		tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> dirty = 1;
	}
	tavares -> sets[memory -> decimal_sindex] -> lines[high_index] -> tag = memory -> tag;
	updateRecents(memory);
}

//-----Write Back Ops-----//
void writeBack(address_strip *memory) {
	int i, j;
    	for (i = 0; i < tavares -> set_size; i++) {
        	if (tavares -> sets[memory -> decimal_sindex] -> lines[i] -> valid == 1 && strcmp(memory -> tag, tavares -> sets[memory -> decimal_sindex] -> lines[i] -> tag) == 0) {
                	tavares -> hits++;
			tavares -> sets[memory -> decimal_sindex] -> lines[i] -> lru_index = 0;
			if (strcmp(memory -> readwrite, "W") == 0) {
				tavares -> sets[memory -> decimal_sindex] -> lines[i] -> dirty = 1;
			}
			for (j = 0; j < tavares -> set_size; j++) {
				tavares -> sets[memory -> decimal_sindex] -> lines[j] -> fifo_index++;
				if (j != i) {
					tavares -> sets[memory -> decimal_sindex] -> lines[j] -> lru_index++;
				}
			}
			return;
            	}
    	}
	tavares -> misses++;
	tavares -> reads++;
	bookBack(memory);
}

//-----Simulate Cache with <trace file>-----//
void beginTrace(FILE *trace_file, int num_lines) {
    	int mem_length;
    	char ip[12], rw[2], address[11], *binary_add;
    	address_strip *memory;
    	while ((fscanf(trace_file, "%s %s %s", ip, rw, address) != EOF) && (strcmp(ip, "#eof") != 0)) {
        	memory = malloc(sizeof(address_strip));
        	initializeAddressStrip(memory);
        	if (strcmp(rw, "W") == 0) {
            		memory -> readwrite = "W";
        	}
        	else {
            		memory -> readwrite = "R";
        	}
        	binary_add = binaryFromHex(address);
        	mem_length = strlen(binary_add);
        	updateAddressValues(mem_length, memory, binary_add);

		//-----Write Through vs. Write Back-----//
        	if (strcmp(tavares -> write_policy, "wt") == 0) {
            		writeThrough(memory);
        	}
		else {
			writeBack(memory);
		}
		free(binary_add);
		free(memory);
    	}
}

//-----Frees the cache-----//
void freeCache() {
	int i, j;
	for (i = 0; i < tavares -> num_sets; i++) {
		for (j = 0; j < tavares -> set_size; j++) {
			free(tavares -> sets[i] -> lines[j] -> tag);
			free(tavares -> sets[i] -> lines[j]);
		}
		free(tavares -> sets[i] -> lines);
		free(tavares -> sets[i]);
	}
	free(tavares);
}

//-----Main method - parses input-----//
int main(int argc, char *argv[]) {
    	char *argument, *assoc, *assoc_substring, *replacementpolicy, *assoc_val, *writepolicy;
    	int cachesize, n, blocksize, num_lines;
    	FILE *sniped;
	
	// Error Check:	Argument Count
    	if (argc != 7 && argc != 2) {
        	fprintf(stderr, "ERROR: Invalid amount of arguments\n");
    	}
    	else {
        	if (argc == 2) {
            		argument = argv[1];
			
			// Error Check:	Format of Help Request
            		if (strcmp(argument, "-h") == 0) {
                		printf("Usage: \"./c-sim <cache size> <associativity> <block size> <replacement policy> <write policy> <trace file>\"\n");
                		exit(0);
            		}
            		else {
                		fprintf(stderr, "ERROR: argv[1] must be -h when argc == 2.\n");
                		exit(0);
            		}
        	}
		
		// Initialize the Cache
        	dropThePuck();
		// Retreive <cache size>
        	cachesize = atoi(argv[1]);
		
		// Error Check:	Size Zero Cache
        	if (cachesize == 0) {
            		fprintf(stderr, "ERROR: cache size cannot be zero.\n");
            		exit(0);
        	}
        	else {

			// Error Check:	Cache size & Power of Two Compatibility
            		if (powerOfTwo(cachesize) == 1) {
                		tavares -> cache_size = cachesize;
            		}
            		else {
                		fprintf(stderr, "ERROR: failed power of two test.\n");
                		exit(0);
            		}
        	}

		// Retreive <associativity>
        	assoc = argv[2];

        	if (strcmp(assoc, "direct") == 0) {
            		tavares -> associativity = assoc;
        	}
        	else if (strcmp(assoc, "assoc") == 0) {
            		tavares -> associativity = assoc;
        	}
        	else {
            		assoc_substring = malloc(6);
            		strncpy(assoc_substring, assoc, 6);

			// Error Check: Format of Associativity Parameter
            		if (strcmp(assoc_substring, "assoc:") == 0) {
				// Error Check:	Associativity Existence
                		if (strlen(assoc) - 6 == 0) {
                    			fprintf(stderr, "ERROR: associativity value not detected.\n");
                    			free(assoc_substring);
                    			exit(0);
                		}

                		assoc_val = malloc(strlen(assoc) - 6);
                		strncpy(assoc_val, assoc + 6, strlen(assoc) - 6);
                		n = atoi(assoc_val);

				// Error Check:	Associativity Zero
                		if (n == 0) {
                    			fprintf(stderr, "ERROR: associativity value cannot be zero.\n");
                    			free(assoc_val);
                    			free(assoc_substring);
                    			exit(0);
                		}
                		else {

					// Error Check:	Associativity & Power of Two Compatibility
                    			if (powerOfTwo(n) == 1) {
                        			tavares -> associativity = assoc;
                        			tavares -> assoc_value = n;
                    			}
                    			else {
                        			fprintf(stderr, "ERROR: associativity value failed power of two test.\n");
                        			free(assoc_val);
                        			free(assoc_substring);
                        			exit(0);
                    			}
                		}
            		}
            		else {
                		fprintf(stderr, "ERROR: invalid associativity.\n");
                		exit(0);
            		}
        	}
		
		// Retreive <block size>
		blocksize = atoi(argv[3]);

		// Error Check: Block Size Zero
        	if (blocksize == 0) {
            		fprintf(stderr, "ERROR: cache block size cannot be zero.\n");
            		exit(0);
        	}
        	else {
			// Error Check: Block Size & Power of Two Compatibility
            		if (powerOfTwo(blocksize) == 1) {
                		tavares -> block_size = blocksize;
            		}
            		else {
                		fprintf(stderr, "ERROR: block size failed power of two test.\n");
                		exit(0);
            		}
        	}

		// Retreive <replacement policy>
		replacementpolicy = argv[4];

		// Error Check: Format of <replacement policy>
		if (strcmp(replacementpolicy, "FIFO") == 0) {
            		tavares -> replacement_policy = replacementpolicy;
        	}
        	else if (strcmp(replacementpolicy, "LRU") == 0) {
            		tavares -> replacement_policy = replacementpolicy;
        	}
        	else {
            		fprintf(stderr, "ERROR: invalid replacement policy\n");
            		free(writepolicy);
            		exit(0);
        	}
		
		// Retreive <write policy>
        	writepolicy = argv[5];

		// Error Check: Format of <write policy>
        	if (strcmp(writepolicy, "wt") == 0) {
            		tavares -> write_policy = writepolicy;
        	}
        	else if (strcmp(writepolicy, "wb") == 0) {
            		tavares -> write_policy = writepolicy;
        	}
        	else {
            		fprintf(stderr, "ERROR: invalid write policy\n");
            		free(writepolicy);
            		exit(0);
        	}

        	tavares -> num_sets = calcNumSets();
        	if (tavares -> assoc_value > 0) {
            		tavares -> set_size = tavares -> assoc_value;
        	}
        	else if (strcmp(tavares -> associativity, "direct") == 0) {
            		tavares -> set_size = 1;
        	}
        	else {
            		tavares -> set_size = (tavares -> cache_size)/(tavares -> block_size);
        	}

        	initializeCache();		//-----Initializes an empty cache-----//

        	sniped = fopen(argv[6], "r");
        	if (sniped == NULL) {
            		fprintf(stderr, "ERROR: could not open trace file.\n");
            		exit(0);
        	}
        	else {
			num_lines = getNumLines(sniped);
        		fclose(sniped);
        		sniped = fopen(argv[6], "r");
            		beginTrace(sniped, num_lines);
        	}
		printResults();
		freeCache();
    	}
    	return 0;
}
                  pa4/Makefile                                                                                        0000644 0001750 0001750 00000000225 13017762145 011017  0                                                                                                    ustar   om                              om                                                                                                                                                                                                                     CC	=	gcc
FLAGS	=	-pedantic -Wall -g -lm -w
COMPILE	=	${CC} ${FLAGS}

all:	c-sim
c-sim:	c-sim.c
	${COMPILE} -o c-sim c-sim.c
clean:
	rm -rf *.o c-sim
                                                                                                                                                                                                                                                                                                                                                                           pa4/Report.txt                                                                                      0000644 0001750 0001750 00000014611 13020367234 011371  0                                                                                                    ustar   om                              om                                                                                                                                                                                                                     Pranav Katkamwar - Report.pdf
(pranavk)
Assign4: Cache Simulator
--------------------------------------------------------------------------------------------------------------------------------
Files included:
	- Makefile
	- c-sim.c
	- c-sim.h
	- Report.pdf

Working Features: All
	- Write Through, Write Back
	- LRU, FIFO
	- Help for useage (-h)
--------------------------------------------------------------------------------------------------------------------------------
Test Cases Passed: All

om@om-debian:~/Documents/211/Assign4$ ./c-sim 4 direct 1 FIFO wt mytrace.txt
Memory reads: 5
Memory writes: 3
Cache hits: 1
Cache misses: 5
om@om-debian:~/Documents/211/Assign4$ ./c-sim 4 assoc:2 1 FIFO wt mytrace.txt
Memory reads: 3
Memory writes: 3
Cache hits: 3
Cache misses: 3
om@om-debian:~/Documents/211/Assign4$ ./c-sim 4 assoc 1 FIFO wt mytrace.txt
Memory reads: 3
Memory writes: 3
Cache hits: 3
Cache misses: 3
om@om-debian:~/Documents/211/Assign4$ ./c-sim 4 assoc:2 1 LRU wt mytrace.txt
Memory reads: 4
Memory writes: 3
Cache hits: 2
Cache misses: 4
om@om-debian:~/Documents/211/Assign4$ ./c-sim 4 assoc:2 1 LRU wb mytrace.txt
Memory reads: 4
Memory writes: 2
Cache hits: 2
Cache misses: 4
--------------------------------------------------------------------------------------------------------------------------------
Locality of Reference Trace Results:
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wb trace1.txt
Memory reads: 336
Memory writes: 332
Cache hits: 664
Cache misses: 336

Hit Rate = 664/1000 = 0.66

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wb trace1.txt
Memory reads: 667
Memory writes: 333
Cache hits: 333
Cache misses: 667

Hit Rate = 333/1000 = 0.33
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wb trace2.txt
Memory reads: 3298
Memory writes: 2856
Cache hits: 6702
Cache misses: 3298

Hit Rate = 6702/10000 = 0.67

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wb trace2.txt
Memory reads: 5351
Memory writes: 2861
Cache hits: 4649
Cache misses: 5351

Hit Rate = 4649/10000 = 0.46
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wt trace1.txt
Memory reads: 336
Memory writes: 334
Cache hits: 664
Cache misses: 336

Hit Rate = 664/1000 = 0.66

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wt trace1.txt
Memory reads: 667
Memory writes: 334
Cache hits: 333
Cache misses: 667

Hit Rate = 333/1000 = 0.33
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wt trace2.txt
Memory reads: 3499
Memory writes: 2861
Cache hits: 6501
Cache misses: 3499

Hit Rate = 6501/10000 = 0.65

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 FIFO wt trace2.txt
Memory reads: 6759
Memory writes: 2861
Cache hits: 3241
Cache misses: 6759

Hit Rate = 3241/10000 = 0.32
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wb trace1.txt
Memory reads: 336
Memory writes: 330
Cache hits: 664
Cache misses: 336

Hit Rate = 664/1000 = 0.66

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wb trace1.txtMemory reads: 667
Memory writes: 333
Cache hits: 333
Cache misses: 667

Hit Rate = 333/1000 = 0.33
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wb trace2.txt
Memory reads: 3292
Memory writes: 2853
Cache hits: 6708
Cache misses: 3292

Hit Rate = 6708/10000 = 0.67

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wb trace2.txt 
Memory reads: 6760
Memory writes: 2861
Cache hits: 3240
Cache misses: 6760

Hit Rate = 3240/10000 = 0.32
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wt trace1.txt
Memory reads: 336
Memory writes: 334
Cache hits: 664
Cache misses: 336

Hit Rate = 664/1000 = 0.66

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wt trace1.txt 
Memory reads: 667
Memory writes: 334
Cache hits: 333
Cache misses: 667

Hit Rate = 333/1000 = 0.33
***********************************************************************************************
--------Method A: TAG | Set Index | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wt trace2.txt
Memory reads: 3292
Memory writes: 2861
Cache hits: 6708
Cache misses: 3292

Hit Rate = 6708/10000 = 0.67

--------Method B: Set Index | TAG | Block Offset--------
om@om-debian:~/Documents/211/Assign4$ ./c-sim 32 assoc:2 4 LRU wt trace2.txt 
Memory reads: 6760
Memory writes: 2861
Cache hits: 3240
Cache misses: 6760

Hit Rate = 3240/10000 = 0.32
***********************************************************************************************
--------------------------------------------------------------------------------------------------------------------------------
Suppose:
Method A: TAG | Set Index | Block Offset
Method B: Set Index | TAG | Block Offset

Answer to Part 4:
The hit rate for Method B is substantially lower in all cases tested, because the most significant bits of an address are not equivalent to a mod function. These bits do not guarantee a consistent mapping from memory to cache. Thus, more misses occur.










end report.
                                                                                                                       pa4/c-sim.h                                                                                         0000644 0001750 0001750 00000005643 13020335704 010540  0                                                                                                    ustar   om                              om                                                                                                                                                                                                                     #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//-----Struct for a cache line-----//
typedef struct {
	int valid;
	int dirty;
	int lru_index;
	int fifo_index;
	char *tag;
} cache_line;

//-----Struct for a set; stores a line-----//
typedef struct {
	cache_line **lines;
} cache_set;

//-----Struct for a cache; pointer to array of sets and stores all relevant variables-----//
typedef struct {
        int hits;
        int misses;
        int reads;
        int writes;
        int cache_size;
        int block_size;
	int set_size;
        int assoc_value;
        int num_sets;
	char *replacement_policy;
	char *associativity;
	char *write_policy;
	cache_set **sets;
} cache;

//-----Struct that stores relevant values of a line in the trace file including address-related values-----//
typedef struct {
	int num_tag_bits;
	int num_set_index_bits;
	int num_block_offset_bits;
	int decimal_sindex;
	char *set_index;
	char *tag;
	char *readwrite;
} address_strip;


//-----Determines if n is a power of 2-----//
int powerOfTwo(int n);

//-----Returns the decimal form of a binary string-----//
int binaryToDecimal(char *binary);

//-----Returns the binary form of a hexadecimal address-----//
char *binaryFromHex(char *address);

//-----Gets number of lines in the trace file-----//
int getNumLines(FILE *trace_file);

//-----Appends a character to a string-----//
void append(char *s, char c);

//-----Initializes the cache-----//
void dropThePuck();

//-----Creates and initializes a cache line-----//
cache_line* makeLine();

//-----Creates and mallocs a set-----//
void createSet(cache_set *set);

//-----Creates a cold cache-----//
void initializeCache();

//-----Initializes memory address information-----//
void initializeAddressStrip(address_strip *memory);

//-----Prints out cache information-----//
void printResults();

//-----Calculates number of sets in cache-----//
int calcNumSets();

//-----Calculates number of tag bits-----//
int calcNumTagBits();

//-----Calculates number of block offset bits-----//
int calcNumBlockOffsetBits();

//-----Calculates number of set index bits-----//
int calcNumSetIndexBits();

//-----Updates the address values/parameters-----//
void updateAddressValues();

//-----Gets high index for LRU-----//
int getHighestIndex(address_strip *memory); 

//-----Updates indices used for LRU algorithm-----//
void updateRecents(address_strip *memory);

//-----LRU algorithm for updating a write-through cache-----//
void lruWT(address_strip *memory);

//-----Update operations for a write-through cache-----//
void writeThrough(address_strip *memory);

//-----LRU algorithm for updating a write-back cache-----//
void lruWB(address_strip *memory);

//-----Update operations for a write-back cache-----//
void writeBack(address_strip *memory);

//-----Updates the cache given a trace file-----//
void beginTrace(FILE *trace_file, int num_lines);

//-----Main method-----//
int main(int argc, char *argv[]);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             