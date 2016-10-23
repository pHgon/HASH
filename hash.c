#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENCADEAMENTO 0
#define LINEAR       1
#define QUADRATICA   2
#define DUPLO        3

int option (char argument[]);

void main(int argc,char *argv[]){
    if(argc == 1){
    	printf("\nERROR: No argument received .. Program closed!\n");
    	exit(-1);
    }
	
	FILE *file = fopen (argv[2], "r");
	if (file == NULL){
		printf("\nERROR: File not found .. Program closed!\n");
		exit(-1);
	}

	switch (option(argv[1])){
		case ENCADEAMENTO:
			break;
		case LINEAR:
			break;
		case QUADRATICA:
			break;
		case DUPLO:
			break;
		default:
			break;
	}
}


int option (char argument[]){
	if ((strcmp(argument, "-encadeamento")) == 0)
		return ENCADEAMENTO;
	else{
		if ((strcmp(argument, "-linear")) == 0)
			return LINEAR;
		else{
			if ((strcmp(argument, "-quadratica")) == 0)
				return QUADRATICA;
			else{
				if ((strcmp(argument, "-hash_duplo")) == 0)
					return DUPLO;
				else
					return -1;
			}
		}
	}
}