#ifndef _hashtable_h
#define _hashtable_h

typedef struct entry {
	char value[16];
	struct entry *next;
} entry;

typedef struct hashtable {
	int size;
	int unique;
	struct entry **table;
} hashtable;

struct hashtable *make_table(int size);

unsigned int make_key(char* str);

void insert(struct hashtable *table, char *value);

#endif
