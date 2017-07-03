#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"




int main(int argc, char** argv){
	int size = 32768;	
	
	if (argc != 2){
		printf("error\n");
		return 0;
 	}

	FILE *sniped = fopen(argv[1], "r");
	struct hashtable *table = NULL;
	table = make_table(size);
	

	if(sniped == NULL){
		printf("error\n");
		return 0;
	}
	
	char *curr_addr = malloc(32*sizeof(char));
	if(fgets(curr_addr, 32, sniped) != NULL){
		int z;

		for (z = 0; z < 32; z++){
			if (*(curr_addr+z) == '\n'){
				*(curr_addr+z) = '\0';
				z = 33;
			}
		}

		// Error Check
		const char *compare = curr_addr;
		if (strlen(compare) == 0){
			printf("0");
			return 0;
		}
		else if (strlen(compare) < 14){
			//printf("error\n");
			return 0;
		}
		else{
			insert(table, curr_addr);
		}
	}
	else{
		printf("error file corrupt");
		return 0;
	}

	while (fgets(curr_addr, 32, sniped) != NULL){
		int z;
		for (z = 0; z < 32; z++){
			if (*(curr_addr+z) == '\n'){
				*(curr_addr+z) = '\0';
				z = 33;
			}
		}
		
		const char *compare = curr_addr;
		if (strlen(compare) < 14){
			printf("error\n");
		}
		else{
			insert(table, curr_addr);
		}
	}
	
	int unique = table->unique;


	printf("%d\n", unique);

	return 0;
}
