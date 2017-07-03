#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

struct hashtable *make_table(int size) {
	int x;

	struct hashtable *table = (struct hashtable*) malloc(sizeof(struct hashtable));
	if (table == NULL){
		return NULL;
	}
	table->size = size;
	table->table = malloc(sizeof(struct entry*) * size);
	if (table->table == NULL){
		return NULL;
	}
	table->unique = 0;
	
	for (x = 0; x < size; x++) {
		table->table[x] = NULL;
	}
	return table;
}

unsigned int make_key(char* str) {
	unsigned int result = 0;
	int c ;
	if ('0' == *str && 'x' == *(str+1)) { str+=2;
		while (*str) {
			result = result << 4;
			if (c=(*str-'0'),(c>=0 && c <=9)) result|=c;
			else if (c=(*str-'A'),(c>=0 && c <=5)) result|=(c+10);
			else if (c=(*str-'a'),(c>=0 && c <=5)) result|=(c+10);
			else break;
			++str;
		}
	}

	return result;
}

void insert(struct hashtable *table, char * value) {
	int key;
	struct entry *ptr;

	struct entry *insert_entry = (struct entry*) malloc(sizeof(struct entry));
	if (insert_entry == NULL){
		return;
	}
	insert_entry->value[0] = *value;
	insert_entry->next = NULL;
	
	key = make_key(value) % table->size;
	

	if (table->table[key] == NULL) {
		table->table[key] = insert_entry;
		table->unique++;
		return;
	}
	
	int check = 0;
	ptr = table->table[key];
	
	while (ptr->next != NULL) {

		ptr = ptr->next;
	}
	ptr->next = insert_entry;
	return;	
}
