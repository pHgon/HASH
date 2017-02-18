#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENCADEAMENTO 0
#define LINEAR       1
#define QUADRATICA   2
#define DUPLO        3
#define L_FACTOR     0.75
#define INI_SIZE     500



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

// Funcao HASH Rotation ou Rolling, recebe a chave a ser codificada e o tamanho
int rot_hash(void *key, int len){
    unsigned char *p = key;
    int h = 0;
    int i;
    for (i = 0; i < len; i++){
        h = (h << 4) ^ (h >> 28) ^ p[i];
    }
    return abs(h);
}

// Le do arquivo de entrada e retorna o valor da Key do Rotation Hash
int readInput (FILE *inputFile, char input1[], char input2[]){
	fscanf(inputFile, "%s \"%s", input1, input2);
	int tam = strlen(input2);
	tam--;
	input2[tam] = '\0'; // Limpa as " do final da string
	return rot_hash(input2, tam);
}

// Inicia a Hash
char **startHash (int size){
	int i;
	char **ptr = (char**) malloc (size * sizeof(char*));
	for (i=0; i<size; i++){
		ptr[i] = (char*) malloc (100 * sizeof(char));
	}
	return ptr;
}

// Insere na Hash
void insert (char **ptr, int size, char input[], unsigned key){
	int i = key;
	do{
		if (ptr[i] == NULL){
			ptr[i] = input;
			printf("%s\n", ptr[i]);
			break;
		}
		else{	
			i++;
			if(i==size)
				i = 0;
		}
	} while (i != key);
}

void linear (FILE *inputFile, FILE *outputFile){
	char input1[7], input2[100];
	int key, hashSize = INI_SIZE, loadHash = 0;
	char **head = startHash(hashSize);

	int i;
	//for (i=0; i<10; i++){
		key = readInput (inputFile, input1, input2);

		if(strcmp(input1, "INSERT") == 0){
			insert(head, hashSize, input2, key);
			loadHash++;
			//if((loadHash/hashSize)>=L_FACTOR)
		}
		else{
			if(input1=="DELETE"){

			}
			else{
				if(input1=="GET"){

				}
				else{
					printf("\nERROR: input1 contain a invalid command .. Program closed!\n");
					exit(-1);
				}
			}
		}
	//}
}


void main(int argc,char *argv[]){
    if(argc == 1){
    	printf("\nERROR: No argument received .. Program closed!\n");
    	exit(-1);
    }
	
	FILE *inputFile  = fopen (argv[2], "r");
	FILE *outputFile = fopen("log_output.txt", "w");
	if (inputFile == NULL){
		printf("\nERROR: File not found .. Program closed!\n");
		exit(-1);
	}

	switch (option(argv[1])){
		case ENCADEAMENTO:
			break;
		case LINEAR:
			linear(inputFile, outputFile);
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