#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENCADEAMENTO 0
#define LINEAR       1
#define QUADRATICA   2
#define DUPLO        3
#define L_FACTOR      0.75
#define INI_SIZE      500



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

FILE *iniOutputFile (){
	FILE *file = fopen("log_output.txt", "w");
	return file;
}

// Funcao HASH Rotation ou Rolling, recebe a chave a ser codificada e o tamanho
unsigned rot_hash (void *key, int len){
    unsigned char *p = key;
    unsigned h = 0;
    int i;
    for (i = 0; i < len; i++) {
        h = (h << 4) ^ (h >> 28) ^ p[i];
    }
    return h;
}

// Funcao Le do arquivo de entrada
unsigned readInput (FILE *inputFile, char input1[], char input2[]){
	fscanf(inputFile, "%s %s", input1, input2);
	int tam = strlen(input2);
	tam--;
	input2[tam] = '\0'; // Limpa as " do final da string
	return rot_hash(input2, tam);
}

void linear (FILE *inputFile, FILE *outputFile){
	char input1[7], input2[100];
	unsigned key;

	key = readInput(inputFile, input1, input2);
	
	//printf("%s | %s | %u | %u\n\n", input1, input2, key, key%500);
}


void main(int argc,char *argv[]){
    if(argc == 1){
    	printf("\nERROR: No argument received .. Program closed!\n");
    	exit(-1);
    }
	
	FILE *inputFile = fopen (argv[2], "r");
	if (inputFile == NULL){
		printf("\nERROR: File not found .. Program closed!\n");
		exit(-1);
	}

	FILE *outputFile;

	switch (option(argv[1])){
		case ENCADEAMENTO:
			break;
		case LINEAR:
			outputFile = iniOutputFile();
			linear(inputFile, outputFile);
			exit(1);
			break;
		case QUADRATICA:
			break;
		case DUPLO:
			break;
		default:
			printf("\nERROR: Invalid Argument .. Program closed!\n");
			break;
	}
	fclose(inputFile);
	fclose(outputFile);
}